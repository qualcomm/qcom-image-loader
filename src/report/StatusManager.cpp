#include "report/StatusManager.h"

#include "report/DataContainer.h"
#include "report/Thread.h"
#include "util/Event.h"
#include "util/SystemHelper.h"
#include "util/ThisThread.h"
#include "util/ThreadHelper.h"

namespace Device {
namespace Report {

static const uint64_t KILOBYTE = 1024U;
static const uint64_t MEGABYTE = 1024U * KILOBYTE;
static const uint64_t GIGABYTE = 1024U * MEGABYTE;
static const uint64_t MEMORY_USAGE_THRESHOLD(2 * GIGABYTE);
static const uint64_t MEMORY_USAGE_CRITICAL_THRESHOLD(((uint64_t)6) * GIGABYTE);
static const uint64_t MEMORY_USAGE_SAFE_THRESHOLD(((uint64_t)4) * GIGABYTE);
static const double CPU_USAGE_THERSHOLD(90.0);
static const double CPU_USAGE_SAFE_THRESHOLD(50.0);
static const size_t THREAD_COUNT_THRESHOLD(200);
static const uint64_t MIN_DISK_SPACE_THRESHOLD(1 * GIGABYTE);
static const double BUFFER_POOL_CACHE_MISS_THRESHOLD(20.0);
static const uint8_t CRITICAL_THREAD_UNRESPONSIVE_THRESHOLD = 12; // Around 2 minutes
static const uint8_t MEDIUM_THREAD_UNRESPONSIVE_THRESHOLD = 24;   // Around 4 minutes

// ----------------------------------------------------------------------------
// MonitorWorker
//
/// Runs the status monitor on a separate thread to ensure the check doesn't
/// create a deadlock that blocks the application's execution.
// ----------------------------------------------------------------------------
class MonitorWorker : public Util::StdThreadWrapper::Work
{
   TOOLS_FORBID_COPY(MonitorWorker);

public:
   MonitorWorker(const Util::CheckedPointer<StatusManager>& pManager, const std::chrono::seconds& checkPeriod)
   : Util::StdThreadWrapper::Work()
   , m_pManager(pManager)
   , m_checkPeriod(checkPeriod)
   , m_stopEvent()
   {
   }
   virtual ~MonitorWorker()
   {
   }

   virtual void onRun()
   {
      while(!isStopSignaled())
      {
         m_pManager->checkStatus();

         Util::ThisThread::waitForEvent(&m_stopEvent, m_checkPeriod);
      }
   }

   virtual void onStop()
   {
      Util::StdThreadWrapper::Work::onStop();
      m_stopEvent.signal();
   }

private:
   Util::CheckedPointer<StatusManager> m_pManager;
   std::chrono::seconds m_checkPeriod;
   Util::Event m_stopEvent;
};

// ----------------------------------------------------------------------------
// StatusManager
//
// ----------------------------------------------------------------------------
StatusManager::StatusManager()
: m_checkPeriodSeconds(1.0)
, m_activeThreads()
, m_activeDataContainers()
, m_mutex()
, m_pMonitorThread()
, m_pMonitorWorker()
, m_qfsHealthStatus()
, m_threadBadState()
, m_bMemoryUsageCritical(false)
, m_bCpuUsageCritical(false)
{
   m_qfsHealthStatus.init();
}

// ----------------------------------------------------------------------------
// ~StatusManager
//
// ----------------------------------------------------------------------------
StatusManager::~StatusManager()
{
   TOOLS_IGNORE_EXCEPTIONS(endMonitoringStatus());
}

// ----------------------------------------------------------------------------
// beginMonitoringStatus
//
/// Starts a thread to periodically check the status of all tracked items
// ----------------------------------------------------------------------------
void StatusManager::beginMonitoringStatus(const std::chrono::seconds& satusCheckPeriod)
{
   m_checkPeriodSeconds = static_cast<double>(satusCheckPeriod.count());
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   if(m_pMonitorThread == nullptr)
   {
      m_pMonitorWorker = std::make_shared<MonitorWorker>(shared_from_this(), satusCheckPeriod);
      m_pMonitorThread = std::make_shared<Util::StdThreadWrapper>(m_pMonitorWorker);
      m_pMonitorThread->start();
   }
}

// ----------------------------------------------------------------------------
// endMonitoringStatus
//
/// Ends the thread checking the status of all tracked items
// ----------------------------------------------------------------------------
void StatusManager::endMonitoringStatus()
{
   if(m_pMonitorThread != nullptr && Util::StdThreadWrapper::State::CREATED != m_pMonitorThread->getState())
   {
      m_pMonitorThread->stop();
      m_pMonitorThread->waitForStop();
      m_pMonitorThread = NULL;
      m_pMonitorWorker = NULL;
   }
}

// ----------------------------------------------------------------------------
// checkStatus
//
/// Runs a check against all actively tracked items and logs if there are
/// any concerns
///
/// IMPORTANT!! - This function should not call anything that interacts with
/// objects on another object or locks mutexes.  It must be observing only and
/// not doing anything that could change the state of the objects it's watching.
/// It should NEVER lock mutexes on anything (aside from its own) to ensure
/// it never creates a deadlock or timing issue.
// ----------------------------------------------------------------------------
void StatusManager::checkStatus()
{
   m_qfsHealthStatus.clear();

   checkSystemStatus();

   checkBufferPoolStatus();

   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   ThreadStatus& threadStatus = m_qfsHealthStatus.threadStatus;
   threadStatus.threadInfoList.clear();
   auto createThreadInfo([&](const Device::Report::Thread::Work& pitT) {
      ThreadInfo threadInfo;
      threadInfo.threadDescription = pitT.getName();
      threadInfo.errorCode = THREAD_UNRESPONSIVE;
      threadInfo.errorCount = m_threadBadState[(pitT.getThreadId())];
      threadInfo.priority = pitT.getThreadHealthMonitorPriority();
      threadStatus.threadInfoList.push_back(threadInfo);
      return threadInfo;
   });

   for(auto& itT: m_activeThreads)
   {
      if(!itT->isAlive())
      {
         ++m_threadBadState[(itT->getThreadId())];
         if(::Device::Report::Thread::HealthMonitorPriority::Critical == itT->getThreadHealthMonitorPriority())
         {
            if(CRITICAL_THREAD_UNRESPONSIVE_THRESHOLD < m_threadBadState[(itT->getThreadId())])
            {
               FLOG_WARNING(("Critical thread unresponsive time exceeded "
                             "threshold: " +
                             std::to_string(itT->getThreadId()))
                               .c_str());
               createThreadInfo(*itT);
            }
         }
         if(::Device::Report::Thread::HealthMonitorPriority::Medium == itT->getThreadHealthMonitorPriority())
         {
            if(MEDIUM_THREAD_UNRESPONSIVE_THRESHOLD < m_threadBadState[(itT->getThreadId())])
            {
               FLOG_WARNING(("Thread unresponsive time exceeded threshold: " + std::to_string(itT->getThreadId()))
                               .c_str());
               createThreadInfo(*itT);
            }
         }
      }
      else
      {
         // if alive reset the counter.
         m_threadBadState[(itT->getThreadId())] = 0;
      }
      itT->checkThreadStatus();
   }

   std::list<DataContainerStatusInfo>& dataContainerStatusInfoList =
      m_qfsHealthStatus.dataContainerStatus.dataContainerStatusInfoList;
   dataContainerStatusInfoList.clear();

   DataContainerVector::iterator itV = m_activeDataContainers.begin();
   DataContainerVector::iterator endV = m_activeDataContainers.end();
   for(; itV != endV; ++itV)
   {
      if(!(*itV)->checkStatus())
      {
         DataContainerStatusInfo dataContainerStatusInfo;
         dataContainerStatusInfo.errorCode = DATA_OVER_THRESHOLD;
         dataContainerStatusInfo.containerName = (*itV)->getName();
         dataContainerStatusInfo.containerSize = (*itV)->getSize();
         dataContainerStatusInfo.reportThreshold = (*itV)->getThreshold();
         dataContainerStatusInfoList.push_back(dataContainerStatusInfo);
      }
   }
}

// ----------------------------------------------------------------------------
// checkSystemStatus
//
/// Checks the health of the system and reports potential issues
// ----------------------------------------------------------------------------
void StatusManager::checkSystemStatus()
{
#ifdef TOOLS_TARGET_WINDOWS
   try
   {
      SystemStatusInfo& systemStatusInfo = m_qfsHealthStatus.systemStatusInfo;
      systemStatusInfo.processMemoryUsage = Util::getProcessMemoryUsageBytes();
      systemStatusInfo.cpuUsage = Util::CpuLoad::Get();
      systemStatusInfo.nThreads = Util::getNumThreads(Util::getPid());
      systemStatusInfo.availableDisk = Util::getAvailableDiskBytes();

      if(MEMORY_USAGE_THRESHOLD < systemStatusInfo.processMemoryUsage || m_qfsHealthStatus.bFatalMemoryError)
      {
         systemStatusInfo.errorCode = PROCESS_MEMORY_USAGE_THRESHOLD;
         FLOG_WARNING(("WARNING!! Memory usage: " + std::to_string(systemStatusInfo.processMemoryUsage) + " bytes")
                         .c_str());
      }

      // Declare memory in critical condition when usage has exceeded critical
      // threshold and clear after it has dropped below memory warning threshold
      m_bMemoryUsageCritical =
         (MEMORY_USAGE_CRITICAL_THRESHOLD < systemStatusInfo.processMemoryUsage ||
          (m_bMemoryUsageCritical && MEMORY_USAGE_SAFE_THRESHOLD < systemStatusInfo.processMemoryUsage))
            ? true
            : false;

      if(CPU_USAGE_THERSHOLD < systemStatusInfo.cpuUsage)
      {
         systemStatusInfo.errorCode = PROCESS_CPU_USAGE_THERSHOLD;
         FLOG_WARNING(("WARNING!! CPU usage: " + std::to_string(systemStatusInfo.cpuUsage) + "%").c_str());
      }

      // Declare CPU in critical condition when usage has exceeded critical
      // threshold and clear after it has dropped below CPU warning threshold
      m_bCpuUsageCritical =
         (CPU_USAGE_THERSHOLD < systemStatusInfo.cpuUsage ||
          (m_bCpuUsageCritical && CPU_USAGE_SAFE_THRESHOLD < systemStatusInfo.cpuUsage))
            ? true
            : false;

      if(THREAD_COUNT_THRESHOLD < systemStatusInfo.nThreads)
      {
         systemStatusInfo.errorCode = PROCESS_THREAD_COUNT_THRESHOLD;
         FLOG_WARNING(("WARNING!! Thread count: " + std::to_string(systemStatusInfo.nThreads)).c_str());
      }

      if(m_qfsHealthStatus.bFatalStorageError || MIN_DISK_SPACE_THRESHOLD > systemStatusInfo.availableDisk)
      {
         if(MIN_DISK_SPACE_THRESHOLD < systemStatusInfo.availableDisk)
         {
            m_qfsHealthStatus.bFatalStorageError = false;
            FLOG_INFO(("Disk space recovered: " + std::to_string(systemStatusInfo.availableDisk) + " bytes").c_str());
         }
         else
         {
            systemStatusInfo.errorCode = PROCESS_MIN_DISK_SPACE_THRESHOLD;
            FLOG_WARNING(("WARNING!! Disk space: " + std::to_string(systemStatusInfo.availableDisk) + " bytes").c_str()
            );
         }
      }
   }
   TOOLS_CATCH(e, FLOG_ERROR(e.what()));

#endif
}

// ----------------------------------------------------------------------------
// checkBufferPoolStatus
//
/// Checks how the buffer pool is faring
// ----------------------------------------------------------------------------
void StatusManager::checkBufferPoolStatus() const
{
#ifdef ENABLE_BUFFER_POOL
   uint64_t hits = BufferPool::getInstance().getCacheHits();
   uint64_t misses = BufferPool::getInstance().getCacheMisses();

   double cacheMissPercentage = Math::percent(misses, hits + misses);
   if(BUFFER_POOL_CACHE_MISS_THRESHOLD < cacheMissPercentage)
   {
      FLOG_INFO(("WARNING!! Buffer pool cache miss percentage: " + std::to_string(cacheMissPercentage)).c_str());
   }
#endif
}

} // namespace Report
} // namespace Device

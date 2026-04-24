// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "report/Thread.h"

#include "report/StatusManager.h"


namespace Device {
namespace Report {
namespace Thread {

// ----------------------------------------------------------------------------
// Work
//
// ----------------------------------------------------------------------------
Work::Work(const std::string& threadName, const HealthMonitorPriority healthMonitorPriority)
: Util::StdThreadWrapper::Work()
, m_bAlive(true)
, m_threadName(threadName)
, m_threadId()
, m_healthMonitorPriority(healthMonitorPriority)
{
   if(HealthMonitorPriority::Critical < m_healthMonitorPriority || HealthMonitorPriority::Low > m_healthMonitorPriority)
   {
      m_healthMonitorPriority = HealthMonitorPriority::Low;
   }
}

// ----------------------------------------------------------------------------
// ~Work
//
// ----------------------------------------------------------------------------
Work::~Work()
{
}

// ----------------------------------------------------------------------------
// checkThreadStatus
//
// ----------------------------------------------------------------------------
void Work::checkThreadStatus() const
{
   std::shared_ptr<Util::StdThreadWrapper> pThread = getThread();
   if(NULL != pThread && Util::StdThreadWrapper::State::RUNNING == pThread->getState())
   {
      if(!m_bAlive && m_healthMonitorPriority > HealthMonitorPriority::Low)
      {
         FLOG_WARNING(("WARNING!! Thread not reporting itself alive: " + m_threadName + " (TID: " + m_threadId + ")")
                         .c_str());
      }
      else
      {
         // Set false to check if it gets set back to true before the next check
         m_bAlive = false;
      }
   }
}

// ----------------------------------------------------------------------------
// isStopSignaled
//
/// Sets that the thread is still alive.  If it's calling this check, it can
/// be assumed it's still executing
// ----------------------------------------------------------------------------
bool Work::isStopSignaled() const
{
   setAlive();
   return Util::StdThreadWrapper::Work::isStopSignaled();
}

// ----------------------------------------------------------------------------
// run
//
/// Registers the thread to be tracked
// ----------------------------------------------------------------------------
void Work::run(std::shared_ptr<Util::StdThreadWrapper> pThread)
{
   if(NULL != pThread)
   {
      m_threadId = std::to_string(pThread->getId());
   }
   else
   {
      m_threadId = "N/A";
   }

   try
   {
      m_pThread = pThread;
      setState(Util::StdThreadWrapper::State::RUNNING);
      m_pThread->notifyThreadStarted();
      onRun();
   }
   catch(...)
   {
      setState(Util::StdThreadWrapper::State::STOPPED);
      throw;
   }

   setState(Util::StdThreadWrapper::State::STOPPED);
}

// ----------------------------------------------------------------------------
// getName
//
/// @returns The name of this thread object to report with
// ----------------------------------------------------------------------------
std::string Work::getName() const
{
   return m_threadName;
}

// ----------------------------------------------------------------------------
// getThreadId
//
/// @returns The thread Id of this thread
// ----------------------------------------------------------------------------
uint32_t Work::getThreadId() const
{
   if(nullptr != getThread())
   {
      return getThread()->getId();
   }
   return 0xFFFFFFFF;
}

} // namespace Thread
} // namespace Report
} // namespace Device

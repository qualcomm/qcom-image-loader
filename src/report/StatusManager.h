// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "protocol/Base.h"
#include "report/Fwd.h"
#include "report/Thread.h"

#include <chrono>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Device {
namespace Report {

class MonitorWorker;

// ----------------------------------------------------------------------------
// StatusManager
//
/// Manages all items that can report their status for QFS to provide a way
/// to determine failures or potential issues
//
/// Important threads and data containers must inherit off
/// Device::Report::Thread and Device::Report::DataContainer to support checking
/// their status. This way the application can query and log when there are
/// issues with the running items.  Not all threads or data containers will
/// inherit off these items, only those we feel the need to track.  So it is not
/// comprehensive, but should be able to show the status of the most important
/// items
// ----------------------------------------------------------------------------
class StatusManager : public std::enable_shared_from_this<StatusManager>
{
   TOOLS_FORBID_COPY(StatusManager);

public:
   enum StatisticsErrorCode
   {
      NO_ERROR_CODE = 0,
      PROCESS_CPU_USAGE_THERSHOLD = 1,
      PROTOCOL_UNRESPONSIVE = 2,
      PROTOCOL_NO_RX = 3,
      PROTOCOL_NO_TX = 4,
      DATA_OVER_THRESHOLD = 5,
      THREAD_UNRESPONSIVE = 6,
      PROCESS_THREAD_COUNT_THRESHOLD = 7,
      PROCESS_MEMORY_USAGE_THRESHOLD = 8,
      PROCESS_MIN_DISK_SPACE_THRESHOLD = 9,
      THRIFT_SOCKET_ERROR = 10,
      DEVICE_GENERATED_DATA_ERROR = 11
   };

   struct SystemStatusInfo
   {
      SystemStatusInfo()
      : errorCode(NO_ERROR_CODE)
      , processMemoryUsage(0)
      , cpuUsage(0)
      , nThreads(0)
      , availableDisk(100)
      {
      }
      StatisticsErrorCode errorCode;
      uint64_t processMemoryUsage;
      double cpuUsage;
      size_t nThreads;
      uint64_t availableDisk;
   };

   struct ThreadInfo
   {
      StatisticsErrorCode errorCode;
      std::string threadDescription;
      size_t errorCount;
      Thread::HealthMonitorPriority priority;
   };

   struct ThreadStatus
   {
      std::list<ThreadInfo> threadInfoList;
   };

   struct DataContainerStatusInfo
   {
      StatisticsErrorCode errorCode;
      std::string containerName;
      int32_t containerSize;
      int32_t reportThreshold;
   };

   struct DataContainerStatus
   {
      std::list<DataContainerStatusInfo> dataContainerStatusInfoList;
   };

   struct QfsHealthStatus
   {
      QfsHealthStatus()
      : systemStatusInfo()
      , threadStatus()
      , dataContainerStatus()
      , bFatalMemoryError(false)
      , bFatalStorageError(false)
      {
      }
      SystemStatusInfo systemStatusInfo;
      ThreadStatus threadStatus;
      DataContainerStatus dataContainerStatus;
      bool bFatalMemoryError;
      bool bFatalStorageError;

      void init()
      {
         clear();
      }

      void clear()
      {
         systemStatusInfo.errorCode = NO_ERROR_CODE;
         threadStatus.threadInfoList.clear();
         dataContainerStatus.dataContainerStatusInfoList.clear();
      }
   };

   StatusManager();
   virtual ~StatusManager();

   void beginMonitoringStatus(const std::chrono::seconds& satusCheckPeriod);
   void endMonitoringStatus();

   void checkStatus();

   bool isMemoryUsageCritical() const
   {
      return m_bMemoryUsageCritical;
   }
   bool isCpuUsageCritical() const
   {
      return m_bCpuUsageCritical;
   }

private:
   typedef std::vector<const Thread::Work*> ThreadVector;
   typedef std::vector<const DataContainer*> DataContainerVector;
   typedef std::unordered_map<uint32_t, size_t> ThreadBadState;

   void checkSystemStatus();
   void checkBufferPoolStatus() const;

   double m_checkPeriodSeconds; ///< How often to check the status

   ThreadVector m_activeThreads;               ///< All actively tracked threads
   DataContainerVector m_activeDataContainers; ///< All actively tracked data containers
   std::recursive_mutex m_mutex;               ///< Protects the member variables

   std::shared_ptr<Util::StdThreadWrapper> m_pMonitorThread;
   std::shared_ptr<MonitorWorker> m_pMonitorWorker;
   mutable QfsHealthStatus m_qfsHealthStatus; ///< Health  status report.
   ThreadBadState m_threadBadState;
   bool m_bMemoryUsageCritical;
   bool m_bCpuUsageCritical;
};

} // namespace Report
} // namespace Device

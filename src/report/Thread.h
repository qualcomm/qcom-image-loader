// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "report/Fwd.h"
#include "util/StringHelper.h"
#include "util/ThreadHelper.h"

namespace Device {
namespace Report {
namespace Thread {

enum class HealthMonitorPriority
{
   Low = 0,
   Normal = 1,
   Medium = 2,
   Critical = 3
};

// ----------------------------------------------------------------------------
// Work
//
/// Registers the thread to be monitored
// ----------------------------------------------------------------------------
class Work : public Util::StdThreadWrapper::Work
{
   TOOLS_FORBID_COPY(Work);

public:
   Work(
      const std::string& threadName,
      const HealthMonitorPriority healthMonitorPriority = HealthMonitorPriority::Normal
   );
   virtual ~Work();

   void checkThreadStatus() const;

   virtual bool isStopSignaled() const;
   virtual void run(std::shared_ptr<Util::StdThreadWrapper> pThread);

   std::string getName() const;
   inline bool isAlive() const
   {
      return (m_bAlive);
   }
   inline HealthMonitorPriority getThreadHealthMonitorPriority() const
   {
      return m_healthMonitorPriority;
   }
   uint32_t getThreadId() const;

protected:
   inline void setAlive() const
   {
      m_bAlive = true;
   }


private:
   mutable volatile bool m_bAlive;
   std::string m_threadName;
   std::string m_threadId;
   HealthMonitorPriority m_healthMonitorPriority;
};

} // namespace Thread
} // namespace Report
} // namespace Device

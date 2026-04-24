// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "tracker/FunctionTracker.h"

#include "device/Exception.h"
#include "util/TimeHelper.h"

namespace QC {

// ----------------------------------------------------------------------------
// FunctionTracker
//
// ----------------------------------------------------------------------------
FunctionTracker::FunctionTracker(
   const std::string& clientName,
   int32_t port,
   const std::string& serviceName,
   const std::string& function,
   const std::string& parameterList,
   std::string& lastError
)
: m_clientName(clientName)
, m_port(std::to_string(port))
, m_serviceName(serviceName)
, m_functionName(function)
, m_lastError(lastError)
, m_startTime(std::chrono::steady_clock::now())
{
   FLOG_INFO(
      "Entering RPC - Client: " + m_clientName + " - " + m_port + " Service: " + m_serviceName +
      " Function: " + m_functionName + "(" +
      (2 > parameterList.size() ? std::string() : parameterList.substr(0, parameterList.size() - 2)) + ")"
   );
}

// ----------------------------------------------------------------------------
// ~FunctionTracker
//
// ----------------------------------------------------------------------------
FunctionTracker::~FunctionTracker()
{
   auto stopTime = std::chrono::steady_clock::now();
   auto duration = std::chrono::duration_cast<Util::duration>(stopTime - m_startTime);
   std::string durationStr = Util::format_duration(duration);

   if(!m_lastError.empty())
   {
      FLOG_ERROR(
         "Exiting  RPC - ERROR!!! Client: " + m_clientName + " - " + m_port + " Service: " + m_serviceName +
         " Function: " + m_functionName + " Error: " + m_lastError + " Duration: " + durationStr.c_str()
      );
   }
   else
   {
      FLOG_INFO(
         "Exiting  RPC - Client: " + m_clientName + " - " + m_port + " Service: " + m_serviceName +
         " Function: " + m_functionName + " Duration: " + durationStr.c_str()
      );
   }
}


} // namespace QC

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "DeviceDiscovery.h"
#include "Spinner.h"

#include <chrono>
#include <functional>
#include <mutex>
#include <string>

namespace QC {
namespace CLI {

/**
 * @brief RAII wrapper for DLL operations with progress tracking
 *
 * This class provides a reusable way to wrap any DLL operation with:
 * - Automatic spinner/progress bar display
 * - Real-time progress updates via DLL callbacks
 * - Automatic cleanup on scope exit
 * - Timing information
 *
 * Usage:
 *   ProgressWrapper wrapper("Firmware Download");
 *   auto result = wrapper.execute([&]() {
 *       return softwareDownload.downloadBuild(path, options);
 *   });
 */
class ProgressWrapper
{
public:
   /**
    * @brief Construct a progress wrapper with an operation name
    * @param operationName The name to display during progress (e.g., "Firmware
    * Download")
    */
   explicit ProgressWrapper(const std::string& operationName);

   /**
    * @brief Destructor - ensures cleanup even if exception is thrown
    */
   ~ProgressWrapper();

   /**
    * @brief Execute a DLL operation with progress tracking
    * @param operation Lambda/function that performs the DLL operation
    * @return The ErrorType result from the operation
    *
    * Example:
    *   auto result = wrapper.execute([&]() {
    *       return softwareDownload.downloadBuild(path, options);
    *   });
    */
   QC::ErrorType execute(std::function<QC::ErrorType()> operation);

   /**
    * @brief Get the duration of the last executed operation
    * @return Duration in seconds
    */
   long long getDurationSeconds() const
   {
      return m_durationSeconds;
   }

   // Disable copy and move
   ProgressWrapper(const ProgressWrapper&) = delete;
   ProgressWrapper& operator=(const ProgressWrapper&) = delete;
   ProgressWrapper(ProgressWrapper&&) = delete;
   ProgressWrapper& operator=(ProgressWrapper&&) = delete;

private:
   // Static callback function for service events
   static void
   serviceEventCallback(const std::string& serviceName, int64_t eventId, const std::string& eventDescription);

   void startProgress();
   void stopProgress();

   // Static members for callback access (protected by mutex)
   static Spinner* g_pActiveProgressSpinner;    ///< For progress/percentage updates
   static Spinner* g_pActiveEventSpinner;       ///< For other event messages
   static int g_lastPercentage;                 ///< Last reported percentage
   static std::string g_lastInformationMessage; ///< Last filename information
   static std::string g_currentOperationName;   ///< Current operation name
   static std::mutex g_callbackMutex;           ///< Protects static members from race conditions

   // Instance member variables
   std::string m_operationName;
   Spinner m_progressSpinner; // For progress/percentage updates
   Spinner m_eventSpinner;    // For other event messages
   long long m_durationSeconds;
   bool m_bIsActive;
};

} // namespace CLI
} // namespace QC

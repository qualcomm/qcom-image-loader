// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "ProgressWrapper.h"

#include "CliCommands.h"
#include "Definitions.h"

#include <KL/kLogger.h>
#include <stdexcept>

namespace QC {
namespace CLI {

// Initialize static members
Spinner* ProgressWrapper::g_pActiveProgressSpinner = nullptr;
Spinner* ProgressWrapper::g_pActiveEventSpinner = nullptr;
int ProgressWrapper::g_lastPercentage = -1;
std::string ProgressWrapper::g_lastInformationMessage = "";
std::string ProgressWrapper::g_currentOperationName = "Operation";
std::mutex ProgressWrapper::g_callbackMutex;

ProgressWrapper::ProgressWrapper(const std::string& operationName)
: m_operationName(operationName)
, m_durationSeconds(0)
, m_bIsActive(false)
{
}

ProgressWrapper::~ProgressWrapper()
{
   if(m_bIsActive)
   {
      stopProgress();
   }
}

void ProgressWrapper::startProgress()
{
   if(m_bIsActive)
   {
      return; // Already started
   }

   // Set the operation name for the callback
   g_currentOperationName = m_operationName;

   // Configure line offsets for multi-line display
   // IMPORTANT: Ensure we're on a fresh line first, then create blank lines for spinners
   // This prevents overwriting any previous output
   KL::Logger::get_instance().writeRawConsole("\n");  // Move to new line (from any previous output)

   // Create space for event spinner line
   KL::Logger::get_instance().writeRawConsole("\n");

   // Start event spinner on this fresh line with offset 0 (no cursor movement)
   // m_eventSpinner.setLineOffset(0);
   // m_eventSpinner.start("Events");

   // Move to next line for progress spinner
   KL::Logger::get_instance().writeRawConsole("\n");

   // Start progress spinner on this fresh line
   m_progressSpinner.setLineOffset(0);
   m_progressSpinner.start("Initializing " + m_operationName);

   // NOW set the proper offsets for future updates
   // Event spinner should update 1 line up, progress spinner on current line
   // m_eventSpinner.setLineOffset(1);
   m_progressSpinner.setLineOffset(0);

   // Initialize progress tracking variables
   g_lastPercentage = 0;
   g_lastInformationMessage = "";

   // Set global spinner pointers for callback access
   g_pActiveProgressSpinner = &m_progressSpinner;
   // g_pActiveEventSpinner = &m_eventSpinner;

   // Note: KLogger line offset is now automatically managed by Spinner::start()
   // Each spinner registers itself with kLogger when started

   // Set up service event callback
   QC::DeviceDiscovery::setServiceEventCallback(&serviceEventCallback);

   m_bIsActive = true;
}

void ProgressWrapper::stopProgress()
{
   if(!m_bIsActive)
   {
      return; // Already stopped
   }

   // Clear service event callback
   QC::DeviceDiscovery::setServiceEventCallback(nullptr);

   // Note: KLogger line offset is now automatically managed by Spinner::stop()
   // Each spinner unregisters itself from kLogger when stopped

   // Clear global spinner pointers
   g_pActiveProgressSpinner = nullptr;
   // g_pActiveEventSpinner = nullptr;

   // Reset progress tracking variables
   g_lastPercentage = -1;
   g_lastInformationMessage = "";

   // Stop both spinners
   m_progressSpinner.stop();
   // m_eventSpinner.stop();

   m_bIsActive = false;
}

QC::ErrorType ProgressWrapper::execute(std::function<QC::ErrorType()> operation)
{
   // Start progress tracking
   startProgress();

   // Record start time
   auto startTime = std::chrono::high_resolution_clock::now();

   QC::ErrorType result;
   try
   {
      // Execute the operation
      result = operation();
   }
   catch(...)
   {
      // Ensure cleanup happens even if exception is thrown
      stopProgress();
      throw;
   }

   // Record end time
   auto endTime = std::chrono::high_resolution_clock::now();
   m_durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

   // Stop progress tracking
   stopProgress();

   return result;
}

#ifdef TOOLS_TARGET_WINDOWS
void __stdcall ProgressWrapper::
   serviceEventCallback(const std::string& serviceName, int64_t eventId, const std::string& eventDescription)
#else
void ProgressWrapper::
   serviceEventCallback(const std::string& serviceName, int64_t eventId, const std::string& eventDescription)
#endif
{
   (void)serviceName; // Suppress unused parameter warning

   // Lock mutex to protect static member access from race conditions
   std::lock_guard<std::mutex> lock(g_callbackMutex);

   // Early return if spinners not active
   // if(nullptr == g_pActiveProgressSpinner || nullptr == g_pActiveEventSpinner)
   if (nullptr == g_pActiveProgressSpinner)
   {
      return; // No spinners active
   }

   // Ignore FIREHOSE_INFORMATION events - they contain verbose INFO messages
   if(EventId::FIREHOSE_INFORMATION == eventId)
   {
      return;
   }

   // Handle FIREHOSE_PROGRESSION events - percentage updates
   if(EventId::FIREHOSE_PROGRESSION == eventId)
   {
      try
      {
         // Parse percentage from eventDescription (format: "XX.XX%")
         std::string desc = eventDescription;
         size_t percentPos = desc.find('%');
         if(std::string::npos != percentPos)
         {
            std::string percentStr = desc.substr(0, percentPos);
            float percentage = std::stof(percentStr);
            int intPercentage = static_cast<int>(percentage);

            // Sanity check: percentage should always increase (or stay same)
            // and be within valid range [0, 100]
            if(intPercentage >= 0 && intPercentage <= 100)
            {
               if(intPercentage >= g_lastPercentage)
               {
                  // Valid progression - update progress spinner
                  g_lastPercentage = intPercentage;

                  // Include filename if available
                  std::string displayMessage = g_currentOperationName;
                  if(!g_lastInformationMessage.empty())
                  {
                     displayMessage += ": " + g_lastInformationMessage;
                  }

                  g_pActiveProgressSpinner->update(intPercentage, displayMessage);
               }
               // If percentage decreased, ignore it (sanity check failed)
            }
         }
      }
      catch(const std::exception& e)
      {
         // If parsing fails, ignore (spinner output already flushed via kLogger)
         (void)e; // Suppress unused parameter warning
      }
   }
   // Handle FIREHOSE_TRANSFER_RAW_PROGRAMMER events - contains filename
   // information
   else if(EventId::FIREHOSE_TRANSFER_RAW_PROGRAMMER == eventId)
   {
      // Extract filename from description (format: 'program ...
      // filename="boot.img" ...')
      size_t filenamePos = eventDescription.find("filename=\"");
      if(std::string::npos != filenamePos)
      {
         filenamePos += 10; // Skip 'filename="'
         size_t endPos = eventDescription.find("\"", filenamePos);
         if(std::string::npos != endPos)
         {
            std::string filename = eventDescription.substr(filenamePos, endPos - filenamePos);
            g_lastInformationMessage = filename;

            // Update progress spinner with current progress and filename
            if(g_lastPercentage >= 0)
            {
               //std::string displayMessage = eventDescription;
               //g_pActiveEventSpinner->updateSpinnerOnly(displayMessage);
               std::string displayMessage = g_currentOperationName + ": " + filename;
               g_pActiveProgressSpinner->update(g_lastPercentage, displayMessage);
               // Note: Spinner::update() already flushes via kLogger's consoleOverwriteLine()
            }
         }
      }
   }
   // Handle all other Firehose events - display on event spinner line
   else
   {
      // Update event spinner with the event description
      std::string displayMessage = eventDescription;
      // g_pActiveEventSpinner->updateSpinnerOnly(displayMessage);
      // Note: Spinner::updateSpinnerOnly() already flushes via kLogger's consoleOverwriteLine()
   }
}

} // namespace CLI
} // namespace QC

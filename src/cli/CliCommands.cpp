// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "CliCommands.h"

#include "CliHelp.h"
#include "Definitions.h"
#include "DeviceDiscovery.h"
#include "ExceptionHandler.h"
#include "FileSystem.h"
#include "ProgressWrapper.h"
#include "SoftwareDownload.h"
#include "SoftwareDownloadUtility.h"
#include "Spinner.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <KL/kLogger.h>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <thread>

namespace QC {
namespace CLI {

#define QC_CMDLINE_IGNORE_EXCEPTION(expr)                                                                              \
   do                                                                                                                  \
   {                                                                                                                   \
      try                                                                                                              \
      {                                                                                                                \
         expr;                                                                                                         \
      }                                                                                                                \
      catch(const std::exception& e)                                                                                   \
      {                                                                                                                \
         std::ostringstream _qc_oss;                                                                                   \
         _qc_oss << #expr << " failed in " << __func__ << " (" << __FILE__ << ":" << __LINE__ << "): " << e.what();    \
         CFLOG_ERROR(_qc_oss.str(), /*printMsgOnly=*/false);                                                           \
      }                                                                                                                \
      catch(...)                                                                                                       \
      {                                                                                                                \
         std::ostringstream _qc_oss;                                                                                   \
         _qc_oss << #expr << " failed in " << __func__ << " (" << __FILE__ << ":" << __LINE__                          \
                 << "): unknown exception";                                                                            \
         CFLOG_ERROR(_qc_oss.str(), /*printMsgOnly=*/false);                                                           \
      }                                                                                                                \
   } while(0)

#define QC_TRY_CLEANUP(softwareDownloadPtr)                                                                            \
   do                                                                                                                  \
   {                                                                                                                   \
      QC_CMDLINE_IGNORE_EXCEPTION(if(softwareDownloadPtr) { (softwareDownloadPtr)->destroyService(); });               \
      QC_CMDLINE_IGNORE_EXCEPTION(QC::DeviceDiscovery::stopMonitoring());                                              \
   } while(0);

const std::string STRING_PATTERN_EDL = ".*?EDL|QDLoader.*?";

std::string toUpper(const std::string& str)
{
   std::string result = str;
   std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
   return result;
}

int CliCommands::execute(const CliOptions& options)
{
   try
   {
      switch(options.command)
      {
         case CliOptions::CommandType::HELP:
            // Check if help was requested
            CliHelp::showUsage();
            return 0;
         case CliOptions::CommandType::LIST_DEVICES:
            return listDevices();

         case CliOptions::CommandType::READ_IMAGES:
         case CliOptions::CommandType::DOWNLOAD_BUILD:
         case CliOptions::CommandType::UFS_PROVISION:
            return downloadBuild(options);

         case CliOptions::CommandType::CREATE_VIP_DIGEST:
         case CliOptions::CommandType::CREATE_VALIDATION_DIGEST:
            return createDigest(options);

         case CliOptions::CommandType::ERASE_FLASH:
            return eraseFlash(options);

         case CliOptions::CommandType::GET_FLASH_INFO:
            return getFlashInfo(options);

         case CliOptions::CommandType::RESET_DEVICE:
            return resetDevice(options);

         case QC::CLI::CliOptions::CommandType::DISPLAY_VERSION:
            return displayVersion();

         case CliOptions::CommandType::NONE:
            QC_THROW_CMDLINE_ERROR(
               QC::Common::Exception::MISSING_REQUIRED_PARAMETER,
               "command",
               "No command specified. Use -h or --help for usage information."
            );

         default:
            QC_THROW_CMDLINE_ERROR(
               QC::Common::Exception::INVALID_PARAMETERS,
               "command",
               "Unknown command. Use -h or --help for "
               "usage information."
            );
      }
   }
   catch(const QC::Common::Exception& e)
   {
      // Print formatted message with suggestion if available
      std::ostringstream oss;
      oss << "Error: " << e.getFormattedMessage();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      return 1;
   }

   catch(const std::exception& e)
   {
      std::ostringstream oss;
      oss << "Error: " << e.what();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      return 1;
   }
}

std::string CliCommands::getProtocolName(QC::ProtocolType type)
{
   switch(type)
   {
      case QC::ProtocolType::PROT_SAHARA:
         return "SAHARA";
      case QC::ProtocolType::PROT_FIREHOSE:
         return "FIREHOSE";
      default:
         return "UNKNOWN";
   }
}

int CliCommands::listDevices()
{
   try
   {
      // Note: Callback setup removed to avoid lambda issues
      // Will rely on direct device enumeration instead

      // Start device discovery monitoring
      QC::DeviceDiscovery::startMonitoring();

      // Wait longer for device enumeration (increased from 10 to 15 seconds)
      CFLOG_INFO("Waiting for device enumeration...", false);
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));

      // Get list of devices with enhanced error handling
      std::list<QC::DeviceInfo> devices;
      try
      {
         devices = QC::DeviceDiscovery::getDeviceList();
         CFLOG_INFO("Device enumeration completed. Found " + std::to_string(devices.size()) + " device(s).", true);
      }
      catch(const std::exception& e)
      {
         CFLOG_ERROR(std::string("Error getting device list: ") + e.what(), true);
         CFLOG_ERROR("Continuing with empty device list", true);
      }
      catch(...)
      {
         CFLOG_ERROR("Unknown error getting device list", true);
         CFLOG_ERROR("Continuing with empty device list", true);
      }

      if(devices.empty())
      {
         CFLOG_INFO(
            std::string("No devices found\n"
                        "Troubleshooting tips:\n"
                        "1. Ensure the device is connected via USB\n"
                        "2. Check if the device is in EDL/Download mode\n"
                        "3. Verify USB permissions (run with sudo if needed)\n"
                        "4. Check if device drivers are installed"),
            true
         );

         QC::DeviceDiscovery::stopMonitoring();
         return 0;
      }

      CFLOG_INFO("Available Devices:\n", true);

      for(auto& device: devices)
      {
         try
         {
            std::string deviceId =
               !device.serialNumber.empty()
                  ? device.serialNumber
                  : (!device.adbSerialNumber.empty() ? device.adbSerialNumber
                     : !device.description.empty()
                        ? device.description
                        : "UNKNOWN");

            CFLOG_INFO("[ " + deviceId + " ]", true);

            CFLOG_INFO(
               "Device Description : " + (!device.description.empty() ? device.description : "Unknown Device"),
               true
            );
            CFLOG_INFO("Device Handle      : " + std::to_string(device.deviceHandle), true);

            if(!device.serialNumber.empty())
            {
               CFLOG_INFO("Serial Number      : " + device.serialNumber, true);
            }
            else
            {
               CFLOG_INFO("Serial Number      : UNKNOWN", true);
            }

            if(!device.adbSerialNumber.empty())
            {
               CFLOG_INFO("ADB Serial Number  : " + device.adbSerialNumber, true);
            }
            else
            {
               CFLOG_INFO("ADB Serial Number  : UNKNOWN", true);
            }

            if(!device.vid.empty())
            {
               CFLOG_INFO("VID                : " + device.vid, true);
            }
            else
            {
               CFLOG_INFO("VID               : UNKNOWN", true);
            }

            if(!device.pid.empty())
            {
               CFLOG_INFO("PID                : " + device.pid, true);
            }
            else
            {
               CFLOG_INFO("PID                : UNKNOWN", true);
            }

            if(!device.edlChipId.empty())
            {
               CFLOG_INFO("EDL Chip ID        : " + device.edlChipId, true);
            }
            else
            {
               CFLOG_INFO("EDL Chip ID        : UNKNOWN", true);
            }

            if(!device.location.empty())
            {
               CFLOG_INFO("Location           : " + device.location, true);
            }
            else
            {
               CFLOG_INFO("Location           : UNKNOWN", true);
            }

            // Get and display protocols for this device
            try
            {
               if(device.deviceHandle != 0)
               {
                  std::list<QC::ProtocolInfo> protocolList = QC::DeviceDiscovery::getProtocolList(device.deviceHandle);
                  if(!protocolList.empty())
                  {
                     CFLOG_INFO("Protocols          :", true);
                     for(const auto& protocolInfo: protocolList)
                     {
                        CFLOG_INFO(
                           "Protocol Type: " + getProtocolName(protocolInfo.protocolType) + " --- " +
                              protocolInfo.description,
                           true
                        );
                        if(!protocolInfo.alternateDescription.empty())
                        {
                           CFLOG_INFO(" (" + protocolInfo.alternateDescription + ")", true);
                        }
                        if(protocolInfo.protocolType == QC::ProtocolType::PROT_SAHARA ||
                           protocolInfo.protocolType == QC::ProtocolType::PROT_FIREHOSE)
                        {
                           std::regex pattern(STRING_PATTERN_EDL);
                           if(std::regex_search(protocolInfo.description, pattern))
                           {
                              CFLOG_INFO("EDL device found!!!", true);
                           }
                        }
                     }
                     CFLOG_INFO("\n", true);
                  }
                  else
                  {
                     CFLOG_INFO("Protocols          : None available", true);
                  }
               }
               else
               {
                  CFLOG_INFO("Protocols          : Invalid device handle", true);
               }
            }
            catch(const std::exception& e)
            {
               CFLOG_ERROR(std::string("Protocols          : Error - ") + e.what(), true);
            }
            catch(...)
            {
               CFLOG_ERROR("Protocols          : Unknown error", true);
            }

            CFLOG_INFO("\n", true);
         }
         catch(const std::exception& e)
         {
            CFLOG_ERROR(std::string("Error processing device: ") + e.what(), true);
         }
         catch(...)
         {
            CFLOG_ERROR("Unknown error processing device", true);
         }
      }

      CFLOG_INFO("Total devices found: " + std::to_string(devices.size()), true);

      // Stop monitoring
      CFLOG_INFO("Stopping device monitoring...", true);
      QC::DeviceDiscovery::stopMonitoring();
      return 0;
   }
   catch(const QC::Common::Exception& e)
   {
      std::ostringstream oss;
      oss << "Error: " << e.getFormattedMessage();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_CMDLINE_IGNORE_EXCEPTION(QC::DeviceDiscovery::stopMonitoring());
      return 1;
   }

   catch(const std::exception& e)
   {
      std::ostringstream oss;
      oss << "Error listing devices: " << e.what();
      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      QC_CMDLINE_IGNORE_EXCEPTION(QC::DeviceDiscovery::stopMonitoring());
      return 1;
   }

   catch(...)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << "(): Unknown Error";

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_CMDLINE_IGNORE_EXCEPTION(QC::DeviceDiscovery::stopMonitoring());
      return 1;
   }
}

void CliCommands::printDownloadOptions(const QC::DownloadBuildOptions downloadOptions)
{
   std::ostringstream memoryTypeString;
   memoryTypeString << "Memory Type: ";
   switch(downloadOptions.memoryType)
   {
      case QC::MemoryType::MEMORY_TYPE_UFS:
         memoryTypeString << "UFS";
         break;
      case QC::MemoryType::MEMORY_TYPE_EMMC:
         memoryTypeString << "EMMC";
         break;
      case QC::MemoryType::MEMORY_TYPE_NAND:
         memoryTypeString << "NAND";
         break;
      case QC::MemoryType::MEMORY_TYPE_SPINOR:
         memoryTypeString << "SPINOR";
         break;
      default:
         memoryTypeString << "Unknown";
         break;
   }
   CFLOG_INFO(memoryTypeString.str(), false);
   if(downloadOptions.__isset.slot)
   {
      CFLOG_INFO("Slot: " + std::to_string(downloadOptions.slot), false);
   }
   if(downloadOptions.__isset.erase)
   {
      CFLOG_INFO(std::string("Erase: ") + (downloadOptions.erase ? "Yes" : "No"), false);
   }
   if(downloadOptions.__isset.resetAfterDownload)
   {
      CFLOG_INFO(std::string("Reset After Download: ") + (downloadOptions.resetAfterDownload ? "Yes" : "No"), false);
   }
   if(downloadOptions.__isset.validationMode)
   {
      CFLOG_INFO("Validation Mode: " + std::to_string(downloadOptions.validationMode), false);
      if(downloadOptions.__isset.validationDigestsPath)
      {
         CFLOG_INFO("Validation digest file: " + downloadOptions.validationDigestsPath, false);
      }
   }

   if(downloadOptions.__isset.firehoseProgPath && !downloadOptions.firehoseProgPath.empty())
   {
      std::string filename = FileSystem::getFilename(downloadOptions.firehoseProgPath);
      CFLOG_INFO("Device programmer: " + filename, false);
   }

   if(downloadOptions.__isset.saharaImageList && !downloadOptions.saharaImageList.empty())
   {
      CFLOG_INFO("Sahara image list (" + std::to_string(downloadOptions.saharaImageList.size()) + "):", false);
      for(const auto& saharaImage: downloadOptions.saharaImageList)
      {
         int imageId = saharaImage.first;
         std::string filename = saharaImage.second;
         std::ostringstream oss;
         oss << "  - " << std::setw(4) << std::setfill('0') << imageId << " : " << filename;
         CFLOG_INFO(oss.str(), false);
      }
   }

   // Display auto-detected files (already set by parser)
   if(downloadOptions.__isset.rawXmlList && !downloadOptions.rawXmlList.empty())
   {
      CFLOG_INFO("Raw program XML files (" + std::to_string(downloadOptions.rawXmlList.size()) + "):", false);
      for(const auto& xml: downloadOptions.rawXmlList)
      {
         std::string filename = FileSystem::getFilename(xml);
         CFLOG_INFO("  - " + filename, false);
      }
   }

   if(downloadOptions.__isset.patchXmlList && !downloadOptions.patchXmlList.empty())
   {
      CFLOG_INFO("Patch program XML files (" + std::to_string(downloadOptions.patchXmlList.size()) + "):", false);
      for(const auto& xml: downloadOptions.patchXmlList)
      {
         std::string filename = FileSystem::getFilename(xml);
         CFLOG_INFO("  - " + filename, false);
      }
   }

   if(downloadOptions.__isset.signedDigestsPath)
   {
      CFLOG_INFO("Signed digest file: " + downloadOptions.signedDigestsPath, false);
   }

   if(downloadOptions.__isset.chainedDigestsPath)
   {
      CFLOG_INFO("Chained digest file: " + downloadOptions.chainedDigestsPath, false);
   }

   if(downloadOptions.__isset.validateImageSize)
   {
      CFLOG_INFO(std::string("Validate Image Size: ") + (downloadOptions.validateImageSize ? "Yes" : "No"), false);
   }

   CFLOG_INFO("\n", false);
}

QC::DeviceInfo CliCommands::findTargetDevice(const std::string& deviceIdentifier)
{
   // Helper lambda to check if a device matches the identifier
   auto matchesIdentifier = [&deviceIdentifier](const QC::DeviceInfo& device) -> bool {
      std::string upperDeviceIdentifier = toUpper(deviceIdentifier);
      std::string upperSerialNumber = toUpper(device.serialNumber);
      std::string upperAdbSerialNumber = toUpper(device.adbSerialNumber);
      std::string upperDeviceDescription = toUpper(device.description);
      return (upperSerialNumber == upperDeviceIdentifier || upperAdbSerialNumber == upperDeviceIdentifier ||
              upperDeviceDescription == upperDeviceIdentifier);
   };

   // Helper lambda to validate device and check EDL mode
   auto validateDevice = [](const QC::DeviceInfo& device, bool& edlModeFound, bool& edlMessageShown) -> void {
      edlModeFound = false; // Reset before checking
      std::list<QC::ProtocolInfo> protocolList = QC::DeviceDiscovery::getProtocolList(device.deviceHandle);
      if(!protocolList.empty())
      {
         if (!edlMessageShown)
         {
            CFLOG_INFO("Protocols on device : ", false);
         }
         for(const auto& protocolInfo: protocolList)
         {
            if (!edlMessageShown)
            {
               CFLOG_INFO(
                  "Protocol Type: " + getProtocolName(protocolInfo.protocolType) + " --- " + protocolInfo.description,
                  false
               );
               if (!protocolInfo.alternateDescription.empty())
               {
                  CFLOG_INFO(" (" + protocolInfo.alternateDescription + ")", false);
               }
            }
            if(protocolInfo.protocolType == QC::ProtocolType::PROT_SAHARA ||
               protocolInfo.protocolType == QC::ProtocolType::PROT_FIREHOSE)
            {
               std::regex pattern(STRING_PATTERN_EDL);
               if(std::regex_search(protocolInfo.description, pattern))
               {
                  if (!edlMessageShown)
                  {
                     CFLOG_INFO("EDL device found!!!", false);
                  }
                  edlModeFound = true;
               }
            }
         }
         if (!edlMessageShown)
         {
            CFLOG_INFO("\n", false);
         }
      }
      else
      {
         CFLOG_INFO("Protocols          : None available", false);
      }
   };

   // Poll device list every 1 second for up to 30 seconds
   CFLOG_INFO("Searching for device: " + deviceIdentifier, false);

   bool deviceFound = false;
   bool edlModeFound = false;
   bool edlMessageShown = false; // Track if we've shown the EDL waiting message
   QC::DeviceInfo targetDevice;

   auto startTime = std::chrono::steady_clock::now();
   auto timeout = std::chrono::seconds(30);

   while(!deviceFound || !edlModeFound)
   {
      // Check timeout
      auto elapsed = std::chrono::steady_clock::now() - startTime;
      if(elapsed >= timeout)
      {
         break; // Timeout reached
      }

      // Get current device list
      std::list<QC::DeviceInfo> devices = QC::DeviceDiscovery::getDeviceList();

      // Check if our target device is in the list
      bool foundInThisScan = false;
      for(const auto& device: devices)
      {
         if(matchesIdentifier(device))
         {
            targetDevice = device;
            foundInThisScan = true;

            if(!deviceFound)
            {
               CFLOG_INFO("Target device found: " + device.description, false);
               deviceFound = true;
            }

            // Validate the device to check EDL mode
            validateDevice(targetDevice, edlModeFound, edlMessageShown);

            if(!edlModeFound && deviceFound && !edlMessageShown)
            {
               // Device found but not in EDL mode - print message only once
               CFLOG_INFO("Device found but not in EDL mode.", false);
               CFLOG_INFO("Please put the device into EDL (Emergency Download) mode manually.", false);
               CFLOG_INFO("Waiting for device to enter EDL mode...", false);
               edlMessageShown = true;
            }

            if(edlModeFound)
            {
               // Success - device found and in EDL mode
               break;
            }
         }
      }

      // If device was previously found but now missing
      if(deviceFound && !foundInThisScan)
      {
         CFLOG_INFO("Device was removed.", false);
         deviceFound = false;
         edlModeFound = false;
         edlMessageShown = false; // Reset message flag in case device comes back
      }

      // If we found device in EDL mode, we're done
      if(deviceFound && edlModeFound)
      {
         break;
      }

      // Wait 1 second before next poll
      std::this_thread::sleep_for(std::chrono::seconds(1));
   }

   if(!deviceFound)
   {
      CFLOG_ERROR("Error: Device not found: " + deviceIdentifier, false);

      // Get current device list for error reporting
      std::list<QC::DeviceInfo> currentDevices = QC::DeviceDiscovery::getDeviceList();

      if(currentDevices.empty())
      {
         CFLOG_ERROR("No devices currently connected.", false);
      }
      else
      {
         CFLOG_ERROR("Available devices:", false);
         for(const auto& device: currentDevices)
         {
            CFLOG_ERROR("  - Handle: " + std::to_string(device.deviceHandle), false);
            if(!device.serialNumber.empty())
            {
               CFLOG_ERROR(", Serial: " + device.serialNumber, false);
            }
            if(!device.adbSerialNumber.empty())
            {
               CFLOG_ERROR(", ADB Serial: " + device.adbSerialNumber, false);
            }
            CFLOG_ERROR("\n", false);
         }
      }

      QC_THROW_DEVICE_ERROR(QC::Common::Exception::DEVICE_NOT_FOUND, deviceIdentifier, "device discovery");
   }

   if(!edlModeFound)
   {
      QC_THROW_DEVICE_ERROR(
         QC::Common::Exception::DEVICE_NOT_FOUND,
         deviceIdentifier,
         "EDL mode validation - device not in download mode",
         CliHelp::edlHelpModeString
      );
   }

   CFLOG_INFO(
      "Found target device:"
      " Handle: " +
         std::to_string(targetDevice.deviceHandle) + " - Description: " + targetDevice.description,
      false
   );
   if(!targetDevice.serialNumber.empty())
   {
      CFLOG_INFO("  Serial Number: " + targetDevice.serialNumber, false);
   }
   CFLOG_INFO("\n", false);

   return targetDevice;
}

int CliCommands::downloadBuild(const CliOptions& options)
{
   std::unique_ptr<QC::SoftwareDownload> softwareDownload;
   try
   {
      // Display appropriate header based on command type
      switch(options.command)
      {
         case CliOptions::CommandType::DOWNLOAD_BUILD:
            CFLOG_INFO("=== Qualcomm Image Loader - Download Build Images ===", true);
            break;
         case CliOptions::CommandType::READ_IMAGES:
            CFLOG_INFO("=== Qualcomm Image Loader - Read Build Images ===", true);
            break;
         case CliOptions::CommandType::UFS_PROVISION:
            CFLOG_INFO("=== Qualcomm Image Loader - UFS Provisioning ===", true);
            break;
         default:
            QC_THROW(
               QC::Common::Exception::INVALID_PARAMETERS,
               std::string("Invalid command options for : ") + __func__,
               "downloadBuild function"
            );
      }

      CFLOG_INFO("Target Device: " + options.deviceId, true);
      CFLOG_INFO("Build Path: " + options.buildPath, true);
      printDownloadOptions(options.downloadBuildOptions);

      // Simple spinner for device discovery
      Spinner discoverySpinner;
      QC::DeviceDiscovery::startMonitoring();

      CFLOG_INFO("Waiting for device enumeration...", false);
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));

      // Properly finish the discovery spinner before device info logging
      discoverySpinner.finish("Device enumeration completed");

      // Find the target device
      QC::DeviceInfo targetDevice = findTargetDevice(options.deviceId);

      // Create SoftwareDownload instance
      CFLOG_INFO("Initializing software download service...", true);
      softwareDownload.reset(new QC::SoftwareDownload(targetDevice));

      // Initialize the service
      QC::ErrorType result = softwareDownload->initializeService();
      THROW_FROM_ERROR_TYPE(result, "service initialization");

      CFLOG_INFO("Service initialized successfully.", true);

      // Determine operation name based on command type
      std::string operationName;
      switch(options.command)
      {
         case CliOptions::CommandType::DOWNLOAD_BUILD:
            operationName = "Image Download";
            break;
         case CliOptions::CommandType::READ_IMAGES:
            operationName = "Reading Images";
            break;
         case CliOptions::CommandType::UFS_PROVISION:
            operationName = "UFS Provisioning";
            break;
         default:
            operationName = "Operation";
            break;
      }

      // Use ProgressWrapper for automatic progress tracking
      CFLOG_INFO(
         "Starting " + operationName +
            "...\n "
            "This may take several minutes depending on the build "
            "size.",
         true
      );


      ProgressWrapper progressWrapper(operationName);
      result = progressWrapper.execute([&]() {
         return softwareDownload->downloadBuild(options.buildPath, options.downloadBuildOptions);
      });

      // Throw exception if download failed
      THROW_FROM_ERROR_TYPE(result, "firmware download");

      // Success path
      CFLOG_INFO("\n", false);
      switch(options.command)
      {
         case CliOptions::CommandType::DOWNLOAD_BUILD:
            CFLOG_INFO("Image download completed successfully!", true);
            break;
         case CliOptions::CommandType::READ_IMAGES:
            CFLOG_INFO("Read images completed successfully!", true);
            break;
         case CliOptions::CommandType::UFS_PROVISION:
            CFLOG_INFO("UFS provisioning completed successfully!", true);
            break;
         default:
            QC_THROW(
               QC::Common::Exception::INVALID_PARAMETERS,
               std::string("Invalid command options for : ") + __func__,
               "downloadBuild function"
            );
      }
      CFLOG_INFO("Process time: " + std::to_string(progressWrapper.getDurationSeconds()) + " seconds", true);
      if(options.downloadBuildOptions.__isset.resetAfterDownload && options.downloadBuildOptions.resetAfterDownload)
      {
         CFLOG_INFO("Device will reset automatically after 10 seconds.", true);
      }
      else
      {
         CFLOG_INFO("Device remains in download mode (reset disabled).", true);
      }

      // Cleanup
      CFLOG_INFO("Cleaning up...", true);
      softwareDownload->destroyService();
      QC::DeviceDiscovery::stopMonitoring();

      return 0;
   }
   catch(const QC::Common::Exception& e)
   {
      std::ostringstream oss;
      oss << "Error: " << e.getFormattedMessage();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }
   catch(const std::exception& e)
   {
      std::ostringstream oss;

      oss << "Error executing " << __func__ << "(): " << e.what();
      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }
   catch(...)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << "(): Unknown Error";

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }
}

int CliCommands::createDigest(const CliOptions& options)
{
   try
   {
      // Display appropriate header based on command type
      switch(options.command)
      {
         case CliOptions::CommandType::CREATE_VALIDATION_DIGEST:
            CFLOG_INFO(
               "=== Qualcomm Image Loader - Create Build Validation "
               "Digest ===",
               true
            );
            break;
         case CliOptions::CommandType::CREATE_VIP_DIGEST:
            CFLOG_INFO("=== Qualcomm Image Loader - Create VIP Digest ===", true);
            break;
         default:
            QC_THROW(
               QC::Common::Exception::INVALID_PARAMETERS,
               std::string("Invalid command options for : ") + __func__,
               "createDigest function"
            );
      }

      CFLOG_INFO("Build Path: " + options.buildPath, true);
      printDownloadOptions(options.downloadBuildOptions);

      // Determine operation name based on command type
      std::string operationName;
      switch(options.command)
      {
         case CliOptions::CommandType::CREATE_VALIDATION_DIGEST:
            operationName = "Creating Build Validation Digest";
            break;
         case CliOptions::CommandType::CREATE_VIP_DIGEST:
            operationName = "Creating VIP Digest";
            break;
         default:
            operationName = "Creating Digest";
            break;
      }

      // Start the process
      CFLOG_INFO(
         "Starting process build hash...\n"
         "This may take several minutes depending on the build size.",
         false
      );

      // Use ProgressWrapper for automatic progress tracking (if DLL emits
      // events)
      ProgressWrapper progressWrapper(operationName);
      QC::ErrorType result;
      result.errorCode = QC::ErrorCode::DEVICE_UNKNOWN_ERROR;

      switch(options.command)
      {
         case CliOptions::CommandType::CREATE_VALIDATION_DIGEST:
            result = progressWrapper.execute([&]() {
               return QC::SoftwareDownloadUtility::
                  createDigestsForBuildValidation(options.buildPath, options.downloadBuildOptions, options.outputPath);
            });
            break;
         case CliOptions::CommandType::CREATE_VIP_DIGEST:
            result = progressWrapper.execute([&]() {
               return QC::SoftwareDownloadUtility::
                  createDigestsForVipDownload(options.buildPath, options.downloadBuildOptions, options.outputPath);
            });
            break;
         default:
            QC_THROW(
               QC::Common::Exception::INVALID_PARAMETERS,
               std::string("Invalid command options for : ") + __func__,
               "createDigest function"
            );
      }

      // Throw exception if digest creation failed
      THROW_FROM_ERROR_TYPE(result, "digest creation");

      // Success path
      printDigestFileLocation(options);
      CFLOG_INFO(
         "Process build hash completed successfully!\n"
         "Process time: " +
            std::to_string(progressWrapper.getDurationSeconds()) + " seconds",
         false
      );

      QC::SoftwareDownloadUtility::clean();
      return 0;
   }
   catch(const QC::Common::Exception& e)
   {
      std::ostringstream oss;
      oss << "Error: " << e.getFormattedMessage();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_CMDLINE_IGNORE_EXCEPTION(QC::SoftwareDownloadUtility::clean());
      return 1;
   }

   catch(const std::exception& e)
   {
      std::ostringstream oss;

      oss << "Error executing " << __func__ << "(): " << e.what();
      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      QC_CMDLINE_IGNORE_EXCEPTION(QC::SoftwareDownloadUtility::clean());
      return 1;
   }

   catch(...)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << "(): Unknown Error";

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_CMDLINE_IGNORE_EXCEPTION(QC::SoftwareDownloadUtility::clean());
      return 1;
   }
}

int CliCommands::eraseFlash(const CliOptions& options)
{
   std::unique_ptr<QC::SoftwareDownload> softwareDownload;

   try
   {
      switch(options.command)
      {
         case CliOptions::CommandType::ERASE_FLASH:
            CFLOG_INFO("=== Qualcomm Image Loader - Erase Flash ===", true);
            break;
         default:
            QC_THROW(
               QC::Common::Exception::INVALID_PARAMETERS,
               std::string("Invalid command options for : ") + __func__,
               "eraseFlash function"
            );
      }

      CFLOG_INFO("Target Device: " + options.deviceId, false);
      CFLOG_INFO("Build Path: " + options.buildPath, false);
      printDownloadOptions(options.downloadBuildOptions);


      // Start device discovery to find the target device
      CFLOG_INFO("Starting device discovery...", false);
      QC::DeviceDiscovery::startMonitoring();

      // Wait for device enumeration
      CFLOG_INFO("Waiting for device enumeration...", false);
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));

      // Find the target device
      QC::DeviceInfo targetDevice = findTargetDevice(options.deviceId);

      // Create SoftwareDownload instance
      CFLOG_INFO("Initializing software download service...", false);
      softwareDownload.reset(new QC::SoftwareDownload(targetDevice));

      // Initialize the service
      QC::ErrorType result = softwareDownload->initializeService();
      THROW_FROM_ERROR_TYPE(result, "service initialization");

      CFLOG_INFO("Service initialized successfully.", false);

      // Use ProgressWrapper for automatic progress tracking
      CFLOG_INFO(
         "Starting flash erase..."
         "This may take several minutes depending on the partition "
         "size.",
         false
      );

      ProgressWrapper progressWrapper("Flash Erase");
      result = progressWrapper.execute([&]() { return softwareDownload->erasePartition(options.downloadBuildOptions); }
      );

      // Throw exception if erase failed
      THROW_FROM_ERROR_TYPE(result, "flash erase");

      // Success path
      CFLOG_INFO(
         "Erase flash completed successfully!"
         "Process time: " +
            std::to_string(progressWrapper.getDurationSeconds()) + " seconds",
         false
      );
      if(options.downloadBuildOptions.__isset.resetAfterDownload && options.downloadBuildOptions.resetAfterDownload)
      {
         CFLOG_INFO("Device will reset automatically after 10 seconds.", false);
      }
      else
      {
         CFLOG_INFO("Device remains in download mode (reset disabled).", false);
      }

      // Cleanup
      CFLOG_INFO("Cleaning up...", false);
      softwareDownload->destroyService();
      QC::DeviceDiscovery::stopMonitoring();

      return 0;
   }
   catch(const QC::Common::Exception& e)
   {
      std::ostringstream oss;
      oss << "Error: " << e.getFormattedMessage();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }

   catch(const std::exception& e)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << e.what();
      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }

   catch(...)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << "(): Unknown Error";

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }
}

int CliCommands::getFlashInfo(const CliOptions& options)
{
   std::unique_ptr<QC::SoftwareDownload> softwareDownload;

   try
   {
      CFLOG_INFO("=== Qualcomm Image Loader - Get Flash Info ===", false);

      CFLOG_INFO("Target Device: " + options.deviceId, false);
      printDownloadOptions(options.downloadBuildOptions);


      // Start device discovery to find the target device
      CFLOG_INFO("Starting device discovery...", false);
      QC::DeviceDiscovery::startMonitoring();

      // Wait for device enumeration
      CFLOG_INFO("Waiting for device enumeration...", false);
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));

      // Find the target device
      QC::DeviceInfo targetDevice = findTargetDevice(options.deviceId);

      // Create SoftwareDownload instance
      CFLOG_INFO("Initializing software download service...", false);
      softwareDownload.reset(new QC::SoftwareDownload(targetDevice));

      // Initialize the service
      QC::ErrorType result = softwareDownload->initializeService();
      THROW_FROM_ERROR_TYPE(result, "service initialization");

      CFLOG_INFO("Service initialized successfully.", false);

      // Get flash information
      CFLOG_INFO("Retrieving flash information from device...", false);
      std::vector<QC::FlashInfo> flashInfo;
      result = softwareDownload->getFlashInfo(flashInfo, options.downloadBuildOptions);

      // Throw exception if getting flash info failed
      THROW_FROM_ERROR_TYPE(result, "get flash information");

      if(flashInfo.empty())
      {
         CFLOG_ERROR("Error: No flash information returned from device", false);
         softwareDownload->destroyService();
         QC::DeviceDiscovery::stopMonitoring();
         return 1;
      }

      CFLOG_INFO("Flash information retrieved successfully!\n", false);

      // Display flash information in table format (similar to ADYANT)
      const QC::FlashInfo& mainInfo = flashInfo[0];


      std::ostringstream out;

      auto line = [](char ch, int n) { return std::string(n, ch); };

      auto appendKv = [&out](const std::string& key, const std::string& value) {
         out << "  " << std::left << std::setw(25) << key << "| " << value << '\n';
      };

      out << line('-', 52) << '\n';
      out << "  " << std::left << std::setw(25) << "PROPERTY" << "| " << "VALUE" << '\n';
      out << line('-', 52) << '\n';


      if(mainInfo.__isset.prodName)
      {
         appendKv("Product Name", mainInfo.prodName);
      }
      if(mainInfo.__isset.manufacturerId)
      {
         std::ostringstream v;
         v << "0x" << std::hex << std::uppercase << mainInfo.manufacturerId;
         appendKv("Manufacturer ID", v.str());
      }
      if(mainInfo.__isset.memType)
      {
         appendKv("Memory Type", mainInfo.memType);
      }
      if(mainInfo.__isset.fwVersion)
      {
         std::ostringstream v;
         v << "0x" << std::hex << std::uppercase << mainInfo.fwVersion;
         appendKv("Firmware Version", v.str());
      }
      if(mainInfo.__isset.numPhysicalPartitions)
      {
         std::ostringstream v;
         v << mainInfo.numPhysicalPartitions;
         appendKv("No. Physical Partitions", v.str());
      }
      if(mainInfo.__isset.serialNum)
      {
         std::ostringstream v;
         v << "0x" << std::hex << std::uppercase << mainInfo.serialNum;
         appendKv("Serial Number", v.str());
      }
      if(!targetDevice.serialNumber.empty())
      {
         std::ostringstream v;
         v << "0x" << std::hex << std::uppercase << targetDevice.serialNumber;
         appendKv("Device Serial Number", v.str());
      }

      CFLOG_INFO(out.str(), false);

      // Get protocol description
      std::list<QC::ProtocolInfo> protocols = QC::DeviceDiscovery::getProtocolList(targetDevice.deviceHandle);

      std::string saharaDesc;
      for(const auto& proto: protocols)
      {
         if(proto.protocolType == QC::ProtocolType::PROT_SAHARA)
         {
            saharaDesc = proto.description;
            break;
         }
      }

      if(!saharaDesc.empty())
      {
         std::ostringstream oss;
         oss << "  " << std::left << std::setw(25) << "Protocol Description" << "| " << saharaDesc << '\n';
         CFLOG_INFO(oss.str(), /*printMsgOnly=*/false);
      }

      if(mainInfo.__isset.specVersion)
      {
         std::ostringstream oss;
         oss << "  " << std::left << std::setw(25) << "UFS spec version" << "| " << mainInfo.specVersion << '\n';
         CFLOG_INFO(oss.str(), /*printMsgOnly=*/false);
      }

      // ---- Separator lines + blank line (keep visual spacing) ----
      {
         std::ostringstream oss;
         oss << std::string(52, '-') << '\n' << '\n';
         CFLOG_INFO(oss.str(), /*printMsgOnly=*/false);
      }


      // Display block information for each partition (LUN info)
      if(options.skipLunInfo)
      {
         CFLOG_INFO(
            "LUN Information: Skipped (--skip-lun-info flag used)"
            "Note: Use without --skip-lun-info to see detailed LUN "
            "information",
            false
         );
      }
      else if(flashInfo.size() > 1)
      {
         std::ostringstream lun;

         auto line = [](char ch, int n) { return std::string(n, ch); };
         auto appendHeaders = [&lun, line] {
            lun << line('-', 56) << '\n';
            lun << "  " << std::left << std::setw(11) << "INDEX" << "| " << std::setw(13) << "TOTAL BLOCKS" << "| "
                << std::setw(13) << "BLOCK SIZE" << "| " << std::setw(13) << "PAGE SIZE" << '\n';
            lun << line('-', 56) << '\n';
         };

         appendHeaders();


         for(size_t i = 1; i < flashInfo.size(); i++)
         {
            const QC::FlashInfo& blockInfo = flashInfo[i];

            // Start index column
            std::ostringstream row;
            row << "  " << std::left << std::setw(11) << (i - 1);

            if(blockInfo.__isset.totalBlocks)
            {
               std::ostringstream v;
               v << "0x" << std::hex << std::uppercase << blockInfo.totalBlocks;
               row << "| " << std::setw(13) << v.str();
            }
            else
            {
               row << "| " << std::setw(13) << "N/A";
            }

            if(blockInfo.__isset.blockSize)
            {
               std::ostringstream v;
               v << "0x" << std::hex << std::uppercase << blockInfo.blockSize;
               row << "| " << std::setw(13) << v.str();
            }
            else
            {
               row << "| " << std::setw(13) << "N/A";
            }

            if(blockInfo.__isset.pageSize)
            {
               std::ostringstream v;
               v << "0x" << std::hex << std::uppercase << blockInfo.pageSize;
               row << "| " << std::setw(13) << v.str();
            }
            else
            {
               row << "| " << std::setw(13) << "N/A";
            }

            row << '\n';
            lun << row.str();
         }

         lun << std::string(56, '-') << '\n';
         CFLOG_INFO(lun.str(), /*printMsgOnly=*/false);
      }
      else
      {
         CFLOG_INFO("LUN Information: Not available from device", false);
      }

      if(options.downloadBuildOptions.__isset.resetAfterDownload && options.downloadBuildOptions.resetAfterDownload)
      {
         CFLOG_INFO("Device will reset automatically after 10 seconds.", false);
      }
      else
      {
         CFLOG_INFO("Device remains in download mode (reset disabled).", false);
      }
      // Cleanup
      CFLOG_INFO("Cleaning up...", false);
      softwareDownload->destroyService();
      QC::DeviceDiscovery::stopMonitoring();

      return 0;
   }
   catch(const QC::Common::Exception& e)
   {
      std::ostringstream oss;
      oss << "Error: " << e.getFormattedMessage();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }

   catch(const std::exception& e)
   {
      std::ostringstream oss;
      oss << "Error getting flash info: " << e.what();
      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }

   catch(...)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << "(): Unknown Error";

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }
}

int CliCommands::resetDevice(const CliOptions& options)
{
   std::unique_ptr<QC::SoftwareDownload> softwareDownload;

   try
   {
      CFLOG_INFO("=== Qualcomm Image Loader - Reset Device ===", false);

      CFLOG_INFO("Target Device: " + options.deviceId, false);
      // printDownloadOptions(options.downloadBuildOptions);


      // Start device discovery to find the target device
      CFLOG_INFO("Starting device discovery...", false);
      QC::DeviceDiscovery::startMonitoring();

      // Wait for device enumeration
      CFLOG_INFO("Waiting for device enumeration...", false);
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));

      // Find the target device
      QC::DeviceInfo targetDevice = findTargetDevice(options.deviceId);

      // Create SoftwareDownload instance
      CFLOG_INFO("Initializing software download service...", false);
      softwareDownload.reset(new QC::SoftwareDownload(targetDevice));

      // Initialize the service
      QC::ErrorType result = softwareDownload->initializeService();
      THROW_FROM_ERROR_TYPE(result, "service initialization");

      CFLOG_INFO("Service initialized successfully.", false);

      // Reset device
      CFLOG_INFO("Resetting device...", false);
      result = softwareDownload->resetDevice(0, true);
      THROW_FROM_ERROR_TYPE(result, "device reset");

      CFLOG_INFO("Reset device successfully!", false);

      // Cleanup
      CFLOG_INFO("Cleaning up...", false);
      softwareDownload->destroyService();
      QC::DeviceDiscovery::stopMonitoring();

      return 0;
   }
   catch(const QC::Common::Exception& e)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << e.getFormattedMessage();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }

   catch(const std::exception& e)
   {
      std::ostringstream oss;
      oss << "Error resetting device: " << e.what();

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }

   catch(...)
   {
      std::ostringstream oss;
      oss << "Error executing " << __func__ << "(): " << "(): Unknown Error";

      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);
      QC_TRY_CLEANUP(softwareDownload);
      return 1;
   }
}

void CliCommands::printDigestFileLocation(const CliOptions& options)
{
   std::string digestHeaderExtension = ".mbn"; // default extension is .mbn
   switch(options.command)
   {
      case CliOptions::CommandType::CREATE_VALIDATION_DIGEST:
         CFLOG_INFO("BuildValidation.digest file created in " + options.outputPath, false);
         break;

      case CliOptions::CommandType::CREATE_VIP_DIGEST:

         if(options.downloadBuildOptions.digestHeaderType == QC::DigestHeaderType::DIGEST_HEADER_TYPE_ELF)
         {
            digestHeaderExtension = ".elf";
         }

         CFLOG_INFO(
            "The following VIP files have been created in " + options.outputPath + ":" +
               "\n\t\t\t\t- ChainedTableOfDigests.bin" + "\n\t\t\t\t- DIGEST_TABLE.bin" +
               "\n\t\t\t\t- DigestsToSign.bin" + digestHeaderExtension,
            false
         );
         break;

      default:
         CFLOG_ERROR("Invalid Digest Creation command: " + CliOptions::getCommandString(options.command), false);
         break;
   }
}


int CliCommands::displayVersion()
{
   CliHelp::showVersion();
   return 0;
}

} // namespace CLI
} // namespace QC

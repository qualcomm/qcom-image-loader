// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "FileSystem.h"
#include "ImageManagementDefinitions.h"

#include <iostream>
#include <KL/kLogger.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace QC {
namespace CLI {

/**
 * @brief Command-line options data structure
 *
 * This structure holds all parsed command-line arguments for the
 * Qualcomm Image Loader CLI application.
 */
class CliOptions
{
public:
   /**
    * @brief Command type enumeration
    */
   enum class CommandType
   {
      NONE,
      LIST_DEVICES,
      DOWNLOAD_BUILD,
      UFS_PROVISION,
      CREATE_VIP_DIGEST,
      CREATE_VALIDATION_DIGEST,
      ERASE_FLASH,
      GET_FLASH_INFO,
      READ_IMAGES,
      RESET_DEVICE,
      DISPLAY_VERSION,
      HELP
   };

   // Command type
   CommandType command = CommandType::NONE;

   // Device identification
   std::string deviceId; // --device

   // Build paths
   std::string buildPath;     // --build
   std::string cdtBinaryPath; // --cdt

   // Digest creation
   std::string outputPath; // --out

   // Advanced options
   bool skipLunInfo = false; // --skip-lun-info

   // Embedded DownloadBuildOptions object
   QC::DownloadBuildOptions downloadBuildOptions;

   // Logging
   bool verbose = false; // --verbose
   KL::LogOption logOptions = KL::LogOption::None;

   bool portTrace = false; // --verbose
   CliOptions()
   : downloadBuildOptions(QC::MemoryType::MEMORY_TYPE_UFS)
   {
   }

   ~CliOptions()
   {
   }

   void setCommandType(CliOptions::CommandType command)
   {
      if(this->command == CliOptions::CommandType::NONE)
      {
         this->command = command;
         return;
      }
      else if(this->command == CliOptions::CommandType::CREATE_VIP_DIGEST)
      {
         if(command == CliOptions::CommandType::CREATE_VIP_DIGEST ||
            command == CliOptions::CommandType::CREATE_VALIDATION_DIGEST)
         {
            this->command = command;
            return;
         }
      }
      throw std::invalid_argument("Duplicated command type.");
   }

   /// <summary>
   /// All argument validation was done in CliParser, here only check if there
   /// is any missing argument for current command
   /// </summary>
   void validate()
   {
      /********** Shall never failed here, consider remove it later **********/
      // Check for device argument
      switch(command)
      {
         case CliOptions::CommandType::DOWNLOAD_BUILD:
         case CliOptions::CommandType::UFS_PROVISION:
         case CliOptions::CommandType::ERASE_FLASH:
         case CliOptions::CommandType::GET_FLASH_INFO:
         case CliOptions::CommandType::READ_IMAGES:
         case CliOptions::CommandType::RESET_DEVICE:
            if(deviceId.empty())
            {
               throw std::invalid_argument("Missing required parameter: --device");
            }
            break;
         case CliOptions::CommandType::NONE:
            throw std::invalid_argument("Unable to determine command. Use "
                                        "--devices or provide required "
                                        "parameters.");
         default:
            break;
      }

      // Check for each function
      switch(command)
      {
         case CliOptions::CommandType::LIST_DEVICES:
            // No additional validation needed
            break;

         case CliOptions::CommandType::DOWNLOAD_BUILD:
            if(buildPath.empty())
            {
               throw std::invalid_argument("Missing required parameter: --build");
            }
            if(!downloadBuildOptions.__isset.firehoseProgPath && !downloadBuildOptions.__isset.saharaImageList)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--device-programmer and can not "
                                           "found programmer from: --build");
            }
            if(!downloadBuildOptions.__isset.rawXmlList && !downloadBuildOptions.__isset.singleImagePath)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--raw-program and can not found "
                                           "program XML from: --build");
            }
            if(downloadBuildOptions.__isset.partitionIndexList && !downloadBuildOptions.__isset.erase)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--erase while --partition-index "
                                           "is set");
            }
            break;
         case CliOptions::CommandType::READ_IMAGES:
            if(buildPath.empty())
            {
               throw std::invalid_argument("Missing required parameter: --build");
            }
            if(!downloadBuildOptions.__isset.firehoseProgPath && !downloadBuildOptions.__isset.saharaImageList)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--device-programmer and can not "
                                           "found programmer from: --build");
            }
            if(!downloadBuildOptions.__isset.rawXmlList)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--raw-program and can not found "
                                           "program XML from: --build");
            }
            if(!downloadBuildOptions.__isset.readImagesPath)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--read-image-path");
            }
            break;

         case CliOptions::CommandType::UFS_PROVISION:
            if(buildPath.empty())
            {
               throw std::invalid_argument("Missing required parameter: --build");
            }
            if(!downloadBuildOptions.__isset.firehoseProgPath && !downloadBuildOptions.__isset.saharaImageList)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--device-programmer and can not "
                                           "found programmer from: --build");
            }
            if(!downloadBuildOptions.__isset.ufsProvisioningPath)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--ufs-provision-xml");
            }
            break;

         case CliOptions::CommandType::ERASE_FLASH:
            if(!downloadBuildOptions.__isset.firehoseProgPath && !downloadBuildOptions.__isset.saharaImageList)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--device-programmer and can not "
                                           "found programmer from: --build");
            }
            // Default partition index if not specified: UFS -> 0,1,2,4,5; others -> 0
            if(!downloadBuildOptions.__isset.partitionIndexList)
            {
               if(downloadBuildOptions.memoryType == QC::MemoryType::MEMORY_TYPE_UFS)
               {
                  downloadBuildOptions.__set_partitionIndexList({0, 1, 2, 4, 5});
               }
               else
               {
                  downloadBuildOptions.__set_partitionIndexList({0});
               }
            }
            break;

         case CliOptions::CommandType::GET_FLASH_INFO:
            if(!downloadBuildOptions.__isset.firehoseProgPath && !downloadBuildOptions.__isset.saharaImageList)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--device-programmer and can not "
                                           "found programmer from: --build");
            }
            break;

         case CliOptions::CommandType::CREATE_VIP_DIGEST:
            if(buildPath.empty())
            {
               throw std::invalid_argument("Missing required parameter: --build");
            }
            if((!downloadBuildOptions.__isset.rawXmlList || downloadBuildOptions.rawXmlList.empty()) &&
               !downloadBuildOptions.__isset.ufsProvisioningPath)
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--raw-program and can not found "
                                           "program XML from: --build");
            }
            if(outputPath.empty())
            {
               throw std::invalid_argument("Missing required parameter: --out");
            }

            break;
         case CliOptions::CommandType::CREATE_VALIDATION_DIGEST:
            if(buildPath.empty())
            {
               throw std::invalid_argument("Missing required parameter: --build");
            }
            if(!downloadBuildOptions.__isset.rawXmlList || downloadBuildOptions.rawXmlList.empty())
            {
               throw std::invalid_argument("Missing required parameter: "
                                           "--raw-program and can not found "
                                           "program XML from: --build");
            }
            if(outputPath.empty())
            {
               throw std::invalid_argument("Missing required parameter: --out");
            }

            break;
         case QC::CLI::CliOptions::CommandType::DISPLAY_VERSION:
            break;
         case CliOptions::CommandType::NONE:
            throw std::invalid_argument("Unable to determine command. Use "
                                        "--devices or provide required "
                                        "parameters.");

         default:
            break;
      }
   }

   /**
    * @brief Prints human readable command
    *
    * @param cmd Current command
    * @return Human readable command
    */
   static std::string getCommandString(const CommandType& cmd)
   {
      switch(cmd)
      {
         case CommandType::NONE:
            return "NONE";
         case CommandType::LIST_DEVICES:
            return "LIST_DEVICES";
         case CommandType::DOWNLOAD_BUILD:
            return "DOWNLOAD_BUILD";
         case CommandType::UFS_PROVISION:
            return "UFS_PROVISION";
         case CommandType::CREATE_VIP_DIGEST:
            return "CREATE_VIP_DIGEST";
         case CommandType::CREATE_VALIDATION_DIGEST:
            return "CREATE_VALIDATION_DIGEST";
         case CommandType::ERASE_FLASH:
            return "ERASE_FLASH";
         case CommandType::GET_FLASH_INFO:
            return "GET_FLASH_INFO";
         case CommandType::READ_IMAGES:
            return "READ_IMAGES";
         case CommandType::RESET_DEVICE:
            return "RESET_DEVICE";
         case CommandType::DISPLAY_VERSION:
            return "DISPLAY_VERSION";
         case CommandType::HELP:
            return "HELP";
         default:
            return std::string("UNKNOWN_COMMAND (") + std::to_string(static_cast<int>(cmd)) + ")";
      }
   }
};

} // namespace CLI

} // namespace QC

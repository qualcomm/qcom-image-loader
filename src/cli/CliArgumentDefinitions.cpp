// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "CliArgumentDefinitions.h"

#include "CliOptions.h"
#include "CliParserUtil.h"
#include "ExceptionHandler.h"
#include "FileSystem.h"

#include <cstddef>
#include <KL/kLogger.h>
#include <stdexcept>
#include <vector>

namespace QC {
namespace CLI {

#define REQUIRE_ABSOLUTE_PATH(somepath)                                                                                \
   if(!FileSystem::isAbsolutePath(somepath))                                                                           \
   {                                                                                                                   \
      QC_THROW_CMDLINE_ERROR(                                                                                          \
         QC::Common::Exception::INVALID_PARAMETERS,                                                                    \
         "path",                                                                                                       \
         "Relative path is not supported: " + value                                                                    \
      );                                                                                                               \
   }
// Static member definitions
std::map<std::string, ArgumentDefinition> CliArgumentDefinitions::argumentDefinitions;
std::vector<CommandDefinition> CliArgumentDefinitions::commandDefinitions;
bool CliArgumentDefinitions::initialized = false;

const std::map<std::string, ArgumentDefinition>& CliArgumentDefinitions::getArgumentDefinitions()
{
   if(!initialized)
   {
      initialize();
   }
   return argumentDefinitions;
}

const std::vector<CommandDefinition>& CliArgumentDefinitions::getCommandDefinitions()
{
   if(!initialized)
   {
      initialize();
   }
   return commandDefinitions;
}

std::vector<ArgumentDefinition> CliArgumentDefinitions::getArgumentsByCategory(ArgumentCategory category)
{
   if(!initialized)
   {
      initialize();
   }

   std::vector<ArgumentDefinition> result;
   for(const auto& pair: argumentDefinitions)
   {
      if(pair.second.category == category)
      {
         result.push_back(pair.second);
      }
   }
   return result;
}

const ArgumentDefinition* CliArgumentDefinitions::getArgumentDefinition(const std::string& name)
{
   if(!initialized)
   {
      initialize();
   }

   auto it = argumentDefinitions.find(name);
   return (it != argumentDefinitions.end()) ? &it->second : nullptr;
}

void CliArgumentDefinitions::initialize()
{
   if(initialized) return;
   // Sort options alphabetically in word

   // DEVICE OPTIONS
   argumentDefinitions["device"] = {
      "device",
      "Specify target device identifier for device operation",
      ArgumentType::STRING,
      ArgumentCategory::DEVICE_OPTIONS,
      "",
      {},
      true,
      "12345",
      "Use SERIAL NUMBER from --devices command",
      [](QC::CLI::CliOptions& options, const std::string& value) { options.deviceId = value; }
   };

   // DOWNLOAD OPTIONS
   argumentDefinitions["active-partition"] = {
      "active-partition",
      "Set the bootable partition index during flat build download.",
      ArgumentType::INTEGER,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --active-partition=1",
      "e.g. 1 for UFS and 0 for eMMC.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_activePartition(CliParserUtil::parseNaturalNumber(value));
      }
   };

   argumentDefinitions["build"] = {
      "build",
      "Given absolute path to flat build directory",
      ArgumentType::PATH,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      true,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true",
      "Directory shall contain both images and XML config files",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         REQUIRE_ABSOLUTE_PATH(value)
         if(FileSystem::isDirectory(value))
         {
            // Convert to absolute path
            options.buildPath = value;
         }
         else
         {
            QC_THROW_FILE_ERROR(QC::Common::Exception::DIRECTORY_NOT_FOUND, value, "--build directory validation");
         }
      }
   };

   argumentDefinitions["cdt"] = {
      "cdt",
      "Given absolute path to CDT (Configuration Data Table) binary file",
      ArgumentType::PATH,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --cdt=/path/to/cdt",
      "Used for replace CDT binary during --flash-build",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         REQUIRE_ABSOLUTE_PATH(value)
         options.cdtBinaryPath = CliParserUtil::parseFilePath("", value);
      }
   };

   argumentDefinitions["chained-digest"] = {
      "chained-digest",
      "Given absolute path or relative path to --build path for chained digest "
      "file, used for vip download.",
      ArgumentType::STRING,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --cdt=/path/to/cdt "
      "--chained-digest=path/to/chained/digest/file "
      "--signed-digest=path/to/chained/signed/file",
      "Must be used together with signed digest file. Chained digest file AND "
      "signed digest file needs to be created before performing a VIP "
      "download. See digest file creation section.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         if(options.buildPath.empty())
         {
            REQUIRE_ABSOLUTE_PATH(value)
         }
         options.downloadBuildOptions.__set_chainedDigestsPath(CliParserUtil::parseFilePath(options.buildPath, value));
      }
   };

   argumentDefinitions["device-programmer"] = {
      "device-programmer",
      "Given absolute path to a device programmer file or sahara XML used "
      "during Sahara transfer",
      ArgumentType::PATH,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      false,
      "qil --ufs-provision --device=12345 "
      "--device-programmer=/path/to/programmer "
      "--ufs-provision-xml=/path/to/xml",
      "Can be relative path if --build path is also given in some process.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         if(options.buildPath.empty())
         {
            REQUIRE_ABSOLUTE_PATH(value)
         }

         // Convert to absolute path
         std::string absolutePath = FileSystem::getAbsolutePath(FileSystem::joinPath(options.buildPath, value));

         // Check if file exists
         if(!FileSystem::fileExists(absolutePath))
         {
            QC_THROW_FILE_ERROR(
               QC::Common::Exception::FILE_NOT_FOUND,
               absolutePath,
               "device programmer file validation"
            );
         }

         // Set build path if empty (use device programmer's directory)
         if(options.buildPath.empty())
         {
            std::string deviceProgDirectory = FileSystem::getDirectoryPath(absolutePath);
            if(!deviceProgDirectory.empty() && FileSystem::isDirectory(deviceProgDirectory))
            {
               options.buildPath = deviceProgDirectory;
            }
         }

         // Check if it's XML (Sahara config) or binary (device programmer)
         if(FileSystem::hasExtension(absolutePath, ".xml"))
         {
            // Parse as Sahara XML and populate sahara image list
            auto saharaImages = FileSystem::readSaharaImageListFromFile(absolutePath, options.buildPath);
            for(const auto& saharaImage: saharaImages)
            {
               if(!FileSystem::isFile(saharaImage.second))
               {
                  QC_THROW_FILE_ERROR(
                     QC::Common::Exception::FILE_NOT_FOUND,
                     saharaImage.second,
                     "sahara XML file validation"
                  );
               }
            }
            if(!saharaImages.empty())
            {
               options.downloadBuildOptions.__set_saharaImageList(saharaImages);
            }
         }
         else
         {
            // Traditional binary device programmer
            options.downloadBuildOptions.__set_firehoseProgPath(absolutePath);
         }
      },
   };

   argumentDefinitions["erase"] = {
      "erase",
      "Specify enabling erasure of a specific partition before download",
      ArgumentType::FLAG,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --erase",
      "If --partition-index is not specified, it will erase default "
      "configuration (0,1,2,4,5 for UFS and 0 for eMMC)",
      [](QC::CLI::CliOptions& options, const std::string& value) { options.downloadBuildOptions.__set_erase(true); }
   };

   argumentDefinitions["memory-type"] = {
      "memory-type",
      "Target memory type",
      ArgumentType::STRING,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {"UFS", "EMMC", "NAND", "SPINOR"},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --erase",
      "Specify type of flash memory on the target device.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_memoryType(CliParserUtil::parseMemoryType(value));
      }
   };

   argumentDefinitions["reset"] = {
      "reset",
      "Enable or disable reset device after firehose process completion",
      ArgumentType::BOOLEAN,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true",
      "Device will remain in firehose mode if set to false.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_resetAfterDownload(CliParserUtil::parseBool(value));
      }
   };

   argumentDefinitions["signed-digest"] = {
      "signed-digest",
      "Given absolute path or relative path to --build path for signed digest "
      "file, used for vip download.",
      ArgumentType::STRING,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --cdt=/path/to/cdt "
      "--signed-digest=path/to/chained/signed/file",
      "Signed digest file needs to be created before performing a VIP "
      "download. See digest file creation section.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         if(options.buildPath.empty())
         {
            REQUIRE_ABSOLUTE_PATH(value)
         }
         options.downloadBuildOptions.__set_signedDigestsPath(CliParserUtil::parseFilePath(options.buildPath, value));
      }
   };

   argumentDefinitions["slot"] = {
      "slot",
      "Configure memory slot number",
      ArgumentType::INTEGER,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "0",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --slot=0",
      "Default to slot 0 if not specified",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_slot(CliParserUtil::parseNaturalNumber(value.c_str()));
      }
   };

   argumentDefinitions["validation-mode"] = {
      "validation-mode",
      "Validate firmware images during download.",
      ArgumentType::INTEGER,
      ArgumentCategory::DOWNLOAD_OPTIONS,
      "0",
      {"0", "1", "2", "3", "4"},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --erase --validation-mode=1",
      "No validation if not specified:\n 0 - No validation\n 1 - Binary "
      "Readback\n 2 - SHA256 Readback\n 3 - Binary readback with digest file "
      "(Requires Build Validation File)\n 4 - SHA256 Readback with digest file "
      "(Requires Build Validation File)",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         QC::ValidationMode::type validationMode = CliParserUtil::parseValidationMode(value);

         options.downloadBuildOptions.__set_validationMode(validationMode);
         if(validationMode == QC::ValidationMode::VALIDATION_MODE_SHA256_READBACK_WITH_DIGESTS_FILE ||
            validationMode == QC::ValidationMode::VALIDATION_MODE_BINARY_READBACK_WITH_DIGESTS_FILE)
         {
            std::string buildValidationFile = FileSystem::findBuildValidationFile(options.buildPath);
            CFLOG_INFO("Build validation file found: " + buildValidationFile, false);
            options.downloadBuildOptions
               .__set_validationDigestsPath(CliParserUtil::parseFilePath(options.buildPath, buildValidationFile));
         }
      }
   };

   // ADVANCED OPTIONS
   argumentDefinitions["firehose-init-time"] = {
      "firehose-init-time",
      "Configure firehose initialization time in ms.",
      ArgumentType::INTEGER,
      ArgumentCategory::ADVANCED_OPTIONS,
      "3000",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --firehose-init-time=20000",
      "Increase for slower devices or connections",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_firehoseInitializeTimeInMs(CliParserUtil::parseNaturalNumber(value));
      }
   };

   argumentDefinitions["firehose-rx-timeout"] = {
      "firehose-rx-timeout",
      "Configure firehose data reception timeout in ms",
      ArgumentType::INTEGER,
      ArgumentCategory::ADVANCED_OPTIONS,
      "100000",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --firehose-rx-time=5000",
      "Increase for slower devices or connections",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_downloadRxTimeoutInMs(CliParserUtil::parseNaturalNumber(value));
      }
   };

   argumentDefinitions["zlp-aware-host"] = {
      "zlp-aware-host",
      "Enable ZLP (Zero Length Packet) aware host for USB transfers",
      ArgumentType::BOOLEAN,
      ArgumentCategory::ADVANCED_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --zlp-aware-host=true",
      "Optional: If not specified, uses FirehoseLoader default. Set to true or false to override.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_zlpAwareHost(CliParserUtil::parseBool(value));
      }
   };

   argumentDefinitions["partition-index"] = {
      "partition-index",
      "Comma-separated list of partition indexes to erase during download, "
      "erase partition only or get flash info",
      ArgumentType::STRING_LIST,
      ArgumentCategory::ADVANCED_OPTIONS,
      "0,1,2,4,5 for UFS; 0 for eMMC",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --erase "
      "--partition-index=0,1,2,3,4,5,6,7,8",
      "",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_partitionIndexList(CliParserUtil::parseNaturalNumberList(value, ','));
      }
   };

   argumentDefinitions["patch-program"] = {
      "patch-program",
      "Semicolon-separated list of patch XML files",
      ArgumentType::STRING_LIST,
      ArgumentCategory::ADVANCED_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --patch-program=patch0.xml;patch1.xml",
      "Specify dedicated XML files to override auto-detection of patch files, "
      "only used in download build.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_patchXmlList(CliParserUtil::parseFilePathList(options.buildPath, value, ';')
         );
      }
   };

   argumentDefinitions["raw-program"] = {
      "raw-program",
      "Semicolon-separated list of rawprogram XML files",
      ArgumentType::STRING_LIST,
      ArgumentCategory::ADVANCED_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true "
      "--raw-program=rawprogram0.xml;rawprogram1.xml",
      "Specify dedicated XML files to override auto-detection of rawprogram "
      "files, only used in download build & read images.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_rawXmlList(CliParserUtil::parseFilePathList(options.buildPath, value, ';'));
      }
   };

   argumentDefinitions["skip-sahara"] = {
      "skip-sahara",
      "While device already in firehose mode. Enable skipping the transfer of "
      "device programmer using Sahara",
      ArgumentType::FLAG,
      ArgumentCategory::ADVANCED_OPTIONS,
      "",
      {},
      false,
      "",
      "Normally used in a subsequent command when the previous command was "
      "executed with '--reset=false' or when the previous command failed and "
      "the device remains in firehose mode.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_skipSahara(true);
      }
   };

   argumentDefinitions["skip-lun-info"] = {
      "skip-lun-info",
      "Enable skipping getting detailed LUN info for --get-flash-info.",
      ArgumentType::FLAG,
      ArgumentCategory::ADVANCED_OPTIONS,
      "",
      {},
      false,
      "",
      "qil --get-flash-info --device=12345 --memory-type=UFS "
      "--device-programmer=/path/to/programmer --reset=true --skip-lun-info",
      [](QC::CLI::CliOptions& options, const std::string& value) { options.skipLunInfo = true; }
   };

   argumentDefinitions["validate-image-size"] = {
      "validate-image-size",
      "Validate image sizes against rawprogram.xml during download",
      ArgumentType::FLAG,
      ArgumentCategory::ADVANCED_OPTIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --reset=true --validate-image-size",
      "Compares image file sizes with sizes specified in rawprogram.xml. "
      "Download fails if any image exceeds the size defined in rawprogram.xml",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_validateImageSize(true);
      }
   };

   // UFS PROVISION
   argumentDefinitions["ufs-provision-xml"] = {
      "ufs-provision-xml",
      "Given absolute Path to UFS provision XML file.",
      ArgumentType::PATH,
      ArgumentCategory::UFS_PROVISIONING,
      "",
      {},
      false,
      "qil --ufs-provision --device=12345 "
      "--device-programmer=/path/to/programmer "
      "--ufs-provision-xml=/path/to/xml",
      "Only used together with --ufs-provision & "
      "--create-ufs-provision-vip-digest",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         if(options.buildPath.empty())
         {
            REQUIRE_ABSOLUTE_PATH(value)
         }
         // Convert to absolute path
         std::string absolutePath = FileSystem::getAbsolutePath(FileSystem::joinPath(options.buildPath, value));
         // Check if file exists
         if(!FileSystem::fileExists(absolutePath))
         {
            QC_THROW_FILE_ERROR(
               QC::Common::Exception::FILE_NOT_FOUND,
               absolutePath,
               "UFS provisioning XML file validation"
            );
         }
         options.downloadBuildOptions.__set_ufsProvisioningPath(absolutePath);
         // Set build path if empty (use UFS Provision XML's directory, major
         // used in create VIP digest)
         if(options.buildPath.empty())
         {
            std::string xmlDirectory = FileSystem::getDirectoryPath(absolutePath);
            if(!xmlDirectory.empty() && FileSystem::isDirectory(xmlDirectory))
            {
               options.buildPath = xmlDirectory;
            }
         }
      },
   };

   // DIGEST CREATION
   argumentDefinitions["digest-header-type"] = {
      "digest-header-type",
      "Digest header format type",
      ArgumentType::STRING,
      ArgumentCategory::DIGEST_CREATION,
      "MBN",
      {"MBN, ELF"},
      false,
      "qil --create-flash-build-vip-digest --build=/path/to/build "
      "--memory-type=UFS --out=/output/path --erase --reset=true "
      "--digest-header-type=ELF",
      "Configure digest header type for VIP digest only used in create VIP "
      "digest. If --digest-header-type is not set, will create MBN type diges",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_digestHeaderType(CliParserUtil::parseDigestHeaderType(value));
      }
   };

   argumentDefinitions["out"] = {
      "out",
      "Output path to save generated digest file",
      ArgumentType::PATH,
      ArgumentCategory::DIGEST_CREATION,
      "",
      {},
      false,
      "qil --create-ufs-provision-vip-digest --ufs-provision-xml=/path/to/xml "
      "--out=/output/path",
      "Must be used inside create vip digest or build validation digest "
      "process.",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         REQUIRE_ABSOLUTE_PATH(value)
         if(FileSystem::isDirectory(value))
         {
            options.outputPath = value;
         }
         else
         {
            QC_THROW_FILE_ERROR(
               QC::Common::Exception::DIRECTORY_NOT_FOUND,
               value,
               "--out directory validation - Please specify "
               "an existing directory"
            );
         }
         if(!FileSystem::isDirectoryWritable(value))
         {
            QC_THROW_FILE_ERROR(
               QC::Common::Exception::FILE_WRITE_ERROR,
               value,
               "--out directory validation - Please specify a "
               "directory with write permissions"
            );
         }
      }
   };

   // FLASH OPERATIONS
   argumentDefinitions["image-path"] = {
      "image-path",
      "Specify binary image path for --send-image",
      ArgumentType::STRING,
      ArgumentCategory::FLASH_OPERATIONS,
      "",
      {},
      false,
      "qil --device=12345 --send-image --image-path=/path/to/image "
      "--memory-type=UFS --start-sector=1 --lun=0 "
      "--device-programmer=/path/to/programmer",
      "",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         if(options.buildPath.empty())
         {
            REQUIRE_ABSOLUTE_PATH(value)
         }
         options.downloadBuildOptions.__set_singleImagePath(CliParserUtil::parseFilePath(options.buildPath, value));
      }
   };

   argumentDefinitions["lun"] = {
      "lun",
      "Configure partition index for --send-image.",
      ArgumentType::INTEGER,
      ArgumentCategory::FLASH_OPERATIONS,
      "",
      {},
      false,
      "qil --device=12345 --send-image --image-path=/path/to/image "
      "--memory-type=UFS --start-sector=1 --lun=0 "
      "--device-programmer=/path/to/programmer",
      "",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_lun(CliParserUtil::parseNaturalNumber(value));
      }
   };

   argumentDefinitions["read-image-path"] = {
      "read-image-path",
      "Directory path to store read-out binary images. "
      "For flash-build: stores binaries read back from device during validation (validation-mode 1 or 3 only). "
      "For read-images: saves partition images read from device.",
      ArgumentType::PATH,
      ArgumentCategory::FLASH_OPERATIONS,
      "",
      {},
      false,
      "qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --validation-mode=1 --read-image-path=/validation_output --reset=true\n"
      "qil --read-images --device=12345 --build=/path/to/build "
      "--memory-type=UFS --read-image-path=/output --reset=true",
      "Optional for flash-build (used with validation-mode 1 or 3), required for read-images",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         REQUIRE_ABSOLUTE_PATH(value)
         if(FileSystem::isDirectory(value))
         {
            options.downloadBuildOptions.__set_readImagesPath(value);
         }
         else
         {
            QC_THROW_FILE_ERROR(
               QC::Common::Exception::DIRECTORY_NOT_FOUND,
               value,
               "--read-image-path directory validation - "
               "Please specify an existing directory"
            );
         }
         if(!FileSystem::isDirectoryWritable(value))
         {
            QC_THROW_FILE_ERROR(
               QC::Common::Exception::FILE_WRITE_ERROR,
               value,
               "--read-image-path directory validation - "
               "Please specify a "
               "directory with write permissions"
            );
         }
      }
   };

   argumentDefinitions["start-sector"] = {
      "start-sector",
      "Configure start sector for --send-image.",
      ArgumentType::INTEGER,
      ArgumentCategory::FLASH_OPERATIONS,
      "",
      {},
      false,
      "qil --device=12345 --send-image --image-path=/path/to/image "
      "--memory-type=UFS --start-sector=1 --lun=0 "
      "--device-programmer=/path/to/programmer",
      "",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.downloadBuildOptions.__set_startSector(CliParserUtil::parseNaturalNumber(value));
      },
   };

   argumentDefinitions["xml-path"] = {
      "xml-path",
      "Configure command XML file path used by --send-xml",
      ArgumentType::PATH,
      ArgumentCategory::FLASH_OPERATIONS,
      "",
      {},
      false,
      "qil --send-xml --device=12345 --memory-type=UFS "
      "--device-programmer=/path/to/programmer --xml-path=/path/to/xml "
      "--reset=true",
      "Can be used to send peek command",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         if(options.buildPath.empty())
         {
            REQUIRE_ABSOLUTE_PATH(value)
         }
         options.downloadBuildOptions.__set_rawXmlList(CliParserUtil::parseFilePathList(options.buildPath, value, ';'));
      }
   };

   // LOGGING
   argumentDefinitions["verbose"] = {
      "verbose",
      "Enable verbose logging output",
      ArgumentType::FLAG,
      ArgumentCategory::LOGGING,
      "",
      {},
      false,
      "qil --devices --verbose",
      "Shows detailed operation logs and debug information",
      [](QC::CLI::CliOptions& options, const std::string& value) {
         options.verbose = true;
         options.logOptions |= KL::LogOption::DebugToFile;
         KL::Logger::get_instance().setOptions(options.logOptions);
         KL::Logger::get_instance().setLevel(KL::Level::Debug);
      }
   };

   // Command definitions
   // NOTE: Some optional arguments depend on the build argument and must appear
   // after it. Specifically, device-programmer, raw-program, and patch-program
   // require build to be processed first. Misordering these dependencies can
   // lead to incorrect behavior or parsing errors. Sort commands alphabetically
   // in word
   commandDefinitions = {
      {"create-flash-build-vip-digest",
       "Create flash build VIP digest. Offline process, no device needed",
       {
          argumentDefinitions["build"],
          argumentDefinitions["memory-type"],
          argumentDefinitions["out"],
          argumentDefinitions["reset"],
       },
       {
          argumentDefinitions["slot"],
          argumentDefinitions["erase"],
          argumentDefinitions["cdt"],
          argumentDefinitions["validation-mode"],
          argumentDefinitions["raw-program"],
          argumentDefinitions["patch-program"],
          argumentDefinitions["digest-header-type"],
          argumentDefinitions["zlp-aware-host"],
          argumentDefinitions["verbose"],
       },
       "qil --create-flash-build-vip-digest --build=/path/to/build "
       "--memory-type=UFS --out=/output/path --erase --reset=true",
       CliOptions::CommandType::CREATE_VIP_DIGEST,
       nullptr},
      {"create-ufs-provision-vip-digest",
       "Create UFS provision VIP digest. Offline process, no device needed",
       {
          argumentDefinitions["out"],
          // Not suggest to add reset for UFS provision, since device might not
          // resetabble argumentDefinitions["reset"],
          argumentDefinitions["ufs-provision-xml"],
       },
       {argumentDefinitions["slot"],
        argumentDefinitions["digest-header-type"],
        argumentDefinitions["zlp-aware-host"],
        argumentDefinitions["verbose"]},
       "qil --create-ufs-provision-vip-digest --ufs-provision-xml=/path/to/xml "
       "--out=/output/path",
       CliOptions::CommandType::CREATE_VIP_DIGEST,
       nullptr},
      {"create-validation-digest",
       "Create build validation digest. Offline process, no device needed",
       {
          argumentDefinitions["build"],
          argumentDefinitions["memory-type"],
          argumentDefinitions["out"],

       },
       {argumentDefinitions["raw-program"], argumentDefinitions["zlp-aware-host"], argumentDefinitions["verbose"]},
       "qil --create-validation-digest --build=/path/to/build "
       "--memory-type=UFS --out=/output/path",
       CliOptions::CommandType::CREATE_VALIDATION_DIGEST,
       nullptr},
      {"devices",
       "List all available device identifiers",
       {},
       {argumentDefinitions["verbose"]},
       "qil --devices",
       CliOptions::CommandType::LIST_DEVICES,
       nullptr},
      {"erase-partitions",
       "Erase specified partitions in device",
       {
          argumentDefinitions["device"],
          argumentDefinitions["device-programmer"],
          argumentDefinitions["memory-type"],
          // Not suggest to add reset for erase partition, since device might
          // not resetabble
          // argumentDefinitions["reset"]
       },
       {argumentDefinitions["slot"],
        argumentDefinitions["partition-index"],
        argumentDefinitions["skip-sahara"],
        argumentDefinitions["firehose-init-time"],
        argumentDefinitions["firehose-rx-timeout"],
        argumentDefinitions["zlp-aware-host"],
        argumentDefinitions["verbose"]},
       "qil --erase-partitions --device=12345 --memory-type=UFS, "
       "--device-programmer=/path/to/programmer --partition-index='0,1,2,4,5'",
       CliOptions::CommandType::ERASE_FLASH,
       nullptr},
      {"flash-build",
       "Flash firmware build to device",
       {argumentDefinitions["device"],
        argumentDefinitions["build"],
        argumentDefinitions["memory-type"],
        argumentDefinitions["reset"]},
       {
          argumentDefinitions["read-image-path"],
          argumentDefinitions["slot"],
          argumentDefinitions["erase"],
          argumentDefinitions["device-programmer"],
          argumentDefinitions["cdt"],
          argumentDefinitions["active-partition"],
          argumentDefinitions["chained-digest"],
          argumentDefinitions["signed-digest"],
          argumentDefinitions["validation-mode"],
          argumentDefinitions["raw-program"],
          argumentDefinitions["partition-index"],
          argumentDefinitions["patch-program"],
          argumentDefinitions["skip-sahara"],
          argumentDefinitions["firehose-init-time"],
          argumentDefinitions["firehose-rx-timeout"],
          argumentDefinitions["validate-image-size"],
          argumentDefinitions["zlp-aware-host"],
          argumentDefinitions["verbose"],
       },
       "qil --flash-build --device=12345 --build=/path/to/build "
       "--memory-type=UFS --erase --reset=true\n"
       "qil --flash-build --device=12345 --build=/path/to/build "
       "--memory-type=UFS --validation-mode=1 --read-image-path=/validation_output --reset=true",
       CliOptions::CommandType::DOWNLOAD_BUILD,
       nullptr},
      {"get-flash-info",
       "Get device flash information",
       {argumentDefinitions["device"],
        argumentDefinitions["memory-type"],
        argumentDefinitions["device-programmer"],
        argumentDefinitions["reset"]},
       {argumentDefinitions["slot"],
        argumentDefinitions["partition-index"],
        argumentDefinitions["skip-lun-info"],
        argumentDefinitions["skip-sahara"],
        argumentDefinitions["firehose-init-time"],
        argumentDefinitions["firehose-rx-timeout"],
        argumentDefinitions["zlp-aware-host"],
        argumentDefinitions["verbose"]},
       "qil --get-flash-info --device=12345 --memory-type=UFS "
       "--device-programmer=/path/to/programmer --reset=true",
       CliOptions::CommandType::GET_FLASH_INFO,
       nullptr},
      {"help", "Display help information", {}, {}, "qil --help", CliOptions::CommandType::HELP, nullptr},
      {"-h", "Display help information", {}, {}, "qil -h", CliOptions::CommandType::HELP, nullptr},
      {"read-images",
       "Read partition images from device",
       {argumentDefinitions["device"],
        argumentDefinitions["build"],
        argumentDefinitions["memory-type"],
        argumentDefinitions["read-image-path"],
        argumentDefinitions["reset"]},
       {argumentDefinitions["slot"],
        argumentDefinitions["erase"],
        argumentDefinitions["device-programmer"],
        argumentDefinitions["raw-program"],
        argumentDefinitions["skip-sahara"],
        argumentDefinitions["firehose-init-time"],
        argumentDefinitions["firehose-rx-timeout"],
        argumentDefinitions["zlp-aware-host"],
        argumentDefinitions["verbose"]},
       "qil --read-images --device=12345 --build=/path/to/build "
       "--memory-type=UFS --read-image-path=/output --reset=true",
       CliOptions::CommandType::READ_IMAGES,
       [](QC::CLI::CliOptions& options) { options.downloadBuildOptions.__set_readImages(true); }},
      {"reset-device",
       "Reset device from firehose mode<BR>Normally used in a subsequent "
       "command when the previous command was executed with '--reset=false'.",
       {
          argumentDefinitions["device"],
       },
       {argumentDefinitions["zlp-aware-host"], argumentDefinitions["verbose"]},
       "qil --reset-device --device=12345",
       CliOptions::CommandType::RESET_DEVICE,
       nullptr},
      {"send-xml",
       "Send a firehose command sequence in an XML file. Can be used to send "
       "peek command.",
       {argumentDefinitions["device"],
        argumentDefinitions["device-programmer"],
        argumentDefinitions["memory-type"],
        argumentDefinitions["xml-path"],
        argumentDefinitions["reset"]},
       {
          argumentDefinitions["slot"],
          argumentDefinitions["skip-sahara"],
          argumentDefinitions["firehose-init-time"],
          argumentDefinitions["firehose-rx-timeout"],
          argumentDefinitions["zlp-aware-host"],
          argumentDefinitions["verbose"],
       },
       "qil --send-xml --device=12345 --memory-type=UFS "
       "--device-programmer=/path/to/programmer --xml-path=/path/to/xml "
       "--reset=true",
       CliOptions::CommandType::DOWNLOAD_BUILD,
       nullptr},
      {"send-image",
       "Send a binary image to a user defined region in device (With partition "
       "index and start index)",
       {argumentDefinitions["device"],
        argumentDefinitions["device-programmer"],
        argumentDefinitions["memory-type"],
        argumentDefinitions["image-path"],
        argumentDefinitions["lun"],
        argumentDefinitions["start-sector"],
        argumentDefinitions["reset"]},
       {
          argumentDefinitions["slot"],
          argumentDefinitions["skip-sahara"],
          argumentDefinitions["firehose-init-time"],
          argumentDefinitions["firehose-rx-timeout"],
          argumentDefinitions["zlp-aware-host"],
          argumentDefinitions["verbose"],
       },
       "qil --send-image --device=12345 --memory-type=UFS "
       "--device-programmer=/path/to/programmer --image-path=/path/to/image "
       "--start-sector=1 --lun=0 --reset=true",
       CliOptions::CommandType::DOWNLOAD_BUILD,
       nullptr},
      {"ufs-provision",
       "Execute a UFS provision.",
       {
          argumentDefinitions["device"],
          argumentDefinitions["device-programmer"],
          // Not suggest to add reset for UFS provision, since device might not
          // resetabble argumentDefinitions["reset"],
          argumentDefinitions["ufs-provision-xml"],
       },
       {
          argumentDefinitions["slot"],
          argumentDefinitions["chained-digest"],
          argumentDefinitions["signed-digest"],
          argumentDefinitions["skip-sahara"],
          argumentDefinitions["firehose-init-time"],
          argumentDefinitions["firehose-rx-timeout"],
          argumentDefinitions["zlp-aware-host"],
          argumentDefinitions["verbose"],
       },
       "qil --ufs-provision --device=12345 "
       "--device-programmer=/path/to/programmer "
       "--ufs-provision-xml=/path/to/xml",
       CliOptions::CommandType::UFS_PROVISION,
       nullptr},
      {"version",
       "Display qil Application version",
       {},
       {},
       "qil --version",
       CliOptions::CommandType::DISPLAY_VERSION,
       nullptr}
   };

   initialized = true;
}

const std::string CliArgumentDefinitions::getValidOptions(const std::string& argument)
{
   std::string validOptions = " Valid options are <";
   const auto& values = CliArgumentDefinitions::argumentDefinitions[argument].validValues;

   for(size_t i = 0; i < values.size(); ++i)
   {
      validOptions += values[i];
      if(i != values.size() - 1) // Add comma only if not the last element
      {
         validOptions += ", ";
      }
   }

   validOptions += ">";
   return validOptions;
}


} // namespace CLI
} // namespace QC

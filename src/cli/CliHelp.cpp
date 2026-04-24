// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "CliHelp.h"

#include "CliArgumentDefinitions.h"
#include "DeviceDiscovery.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <KL/kLogger.h>

namespace QC {
namespace CLI {

const std::string CliHelp::edlHelpModeString =
   "\nDevice is not in EDL mode. Please switch to EDL using one of the "
   "following options:\n"
   "1. ADB\n"
   "2. Hardware Switch\n"
   "Please refer to qil documentation for additional information.\n";


void CliHelp::showVersion()
{
   CFLOG_INFO("Qualcomm Image Loader " + QC::DeviceDiscovery::getDLLVersion(), true);
}

void CliHelp::showUsage()
{
   CFLOG_INFO("Qualcomm Image Loader - Software Download CLI\n", true);
   CFLOG_INFO("Usage:\n", true);
   CFLOG_INFO("  qil <command> [options] \n", true);

   // Show commands first
   showCommands();

   // Show all argument categories
   showArgumentCategory(ArgumentCategory::COMMANDS, "COMMANDS");
   showArgumentCategory(ArgumentCategory::DEVICE_OPTIONS, "DEVICE OPTIONS");
   showArgumentCategory(ArgumentCategory::DOWNLOAD_OPTIONS, "DOWNLOAD OPTIONS");
   showArgumentCategory(ArgumentCategory::ADVANCED_OPTIONS, "ADVANCED OPTIONS");
   showArgumentCategory(ArgumentCategory::UFS_PROVISIONING, "UFS PROVISIONING");
   showArgumentCategory(ArgumentCategory::DIGEST_CREATION, "DIGEST CREATION");
   showArgumentCategory(ArgumentCategory::FLASH_OPERATIONS, "FLASH OPERATIONS");
   showArgumentCategory(ArgumentCategory::LOGGING, "LOGGING");

   // Show examples
   showExamples();

   // Show notes
   showNotes();
}

void CliHelp::showCommands()
{
   const auto& commands = CliArgumentDefinitions::getCommandDefinitions();

   CFLOG_INFO("Common commands:\n", true);

   for(const auto& cmd: commands)
   {
      if(cmd.name != "-h")
      {
         CFLOG_INFO("  --" + cmd.name + "\n", true);
      }
      else
      {
         CFLOG_INFO("  " + cmd.name + "\n", true);
      }
      CFLOG_INFO("    " + cmd.description + "\n", true);

      if(!cmd.requiredArgs.empty())
      {
         std::string requiredOptions = "    ";
         CFLOG_INFO("    Required Options:", true);
         for(size_t i = 0; i < cmd.requiredArgs.size(); ++i)
         {
            if(i > 0) requiredOptions += ", ";
            requiredOptions += "--";
            requiredOptions += cmd.requiredArgs[i].name;
         }
         CFLOG_INFO(requiredOptions, true);

         CFLOG_INFO("\n", true);
      }

      if(!cmd.optionalArgs.empty())
      {
         std::string optionalOptions = "    ";

         CFLOG_INFO("    Optional Options: \t", true);
         for(size_t i = 0; i < cmd.optionalArgs.size(); ++i)
         {
            if(i > 0) optionalOptions += ", ";
            optionalOptions += "--";
            optionalOptions += cmd.optionalArgs[i].name;
         }
         CFLOG_INFO(optionalOptions, true);
         CFLOG_INFO("\n", true);
      }

      CFLOG_INFO("    Example: " + cmd.example + "\n\n", true);
   }
}

void CliHelp::showArgumentCategory(ArgumentCategory category, const std::string& categoryName)
{
   auto args = CliArgumentDefinitions::getArgumentsByCategory(category);

   if(args.empty()) return;

   CFLOG_INFO(categoryName + ":", true);

   for(const auto& arg: args)
   {
      showArgumentDetails(arg);
   }
}

void CliHelp::showArgumentDetails(const ArgumentDefinition& arg)
{
   // Format: --argument <type>    Description
   std::ostringstream line;
   line << "  --" << std::left << std::setw(20) << formatArgumentUsage(arg);


   if(arg.required)
   {
      line << " (required)";
   }

   line << "\n";

   // Show additional details if available
   if(!arg.defaultValue.empty())
   {
      line << "    Default: " + arg.defaultValue + "\n";
   }

   if(!arg.validValues.empty())
   {
      line << "    Valid values: ";
      for(size_t i = 0; i < arg.validValues.size(); ++i)
      {
         if(i > 0) line << ", ";
         line << arg.validValues[i];
      }
      line << "\n";
   }

   if(!arg.example.empty() && arg.type != ArgumentType::FLAG)
   {
      line << "    Example: " + arg.example + "\n";
   }

   if(!arg.notes.empty())
   {
      line << "    Note: " + arg.notes + "\n";
   }

   CFLOG_INFO(line.str(), true);
}

std::string CliHelp::formatArgumentUsage(const ArgumentDefinition& arg)
{
   std::string usage = arg.name;

   switch(arg.type)
   {
      case ArgumentType::FLAG:
         // No additional formatting for flags
         break;
      case ArgumentType::STRING:
         usage += " <string>";
         break;
      case ArgumentType::INTEGER:
         usage += " <number>";
         break;
      case ArgumentType::BOOLEAN:
         usage += " <bool>";
         break;
      case ArgumentType::STRING_LIST:
         usage += " <list>";
         break;
      case ArgumentType::PATH:
         usage += " <path>";
         break;
   }

   return usage;
}

void CliHelp::showExamples()
{
   CFLOG_INFO("EXAMPLES:\n", true);

   const auto& commands = CliArgumentDefinitions::getCommandDefinitions();

   for(const auto& cmd: commands)
   {
      CFLOG_INFO("  # " + cmd.description, true);
      CFLOG_INFO("  " + cmd.example, true);
      CFLOG_INFO("\n", true);
   }

   // // Additional complex examples
   CFLOG_INFO("  # Download with erase and custom timeouts", true);
   CFLOG_INFO(
      "  qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --erase --firehose-init-time=45000 "
      "--reset=true",
      true
   );
   CFLOG_INFO("\n", true);

   CFLOG_INFO("  # Program with erase specific partitions only", true);
   CFLOG_INFO(
      "  qil --flash-build --device=12345 --build=/path/to/build "
      "--erase --partition-index=0,1,2 --memory-type=UFS --reset=true",
      true
   );
   CFLOG_INFO("\n", true);

   CFLOG_INFO("  # Download flat build and replace CDT binary", true);
   CFLOG_INFO(
      "  qil --flash-build --device=12345 --build=/path/to/build "
      "--memory-type=UFS --cdt=/path/to/cdt --reset=true",
      true
   );
   CFLOG_INFO("\n", true);

   CFLOG_INFO(
      "  # A complex sample for UFS provision and download to multiple "
      "flash using skip sahara",
      true
   );
   CFLOG_INFO(
      "  qil --ufs-provision --device=12345 "
      "--device-programmer=/path/to/programmer "
      "--ufs-provision-xml=/path/to/xml",
      true
   );
   CFLOG_INFO(
      "  qil --flash-build --device=12345 --skip-sahara "
      "--build=/path/to/build_ufs --memory-type=UFS --reset=false",
      true
   );
   CFLOG_INFO(
      "  qil --flash-build --device=12345 --skip-sahara "
      "--build=/path/to/build_spinor --memory-type=SPINOR --reset=true",
      true
   );
   CFLOG_INFO("\n", true);

   CFLOG_INFO("  # Reset device from firehose", true);
   CFLOG_INFO(
      "  qil --flash-build --device=12345 --build=/path/to/build_ufs "
      "--memory-type=UFS --reset=false",
      true
   );
   CFLOG_INFO("  qil --reset-device --device=12345", true);
   CFLOG_INFO("\n", true);
}

void CliHelp::showNotes()
{
   CFLOG_INFO("NOTES:", true);
   CFLOG_INFO("  - Options are insensitive", true);
   CFLOG_INFO("  - Boolean values: TRUE/FALSE, 1/0, YES/NO (case-insensitive)", true);
   CFLOG_INFO("  - Use --devices first to get device IDs", true);
   CFLOG_INFO(
      "  - Some operations require only device programmer "
      "(--device-programmer)",
      true
   );
   CFLOG_INFO(
      "  - Device state hopping sequence: EDL(Sahara) --> push sahara "
      "images --> EDL(Firehose) --> push firehose images --> firehose "
      "reset --> HLOS\\Crash",
      true
   );
   CFLOG_INFO("  - Use --verbose for detailed operation logs", true);
   CFLOG_INFO("\n", true);
}

} // namespace CLI
} // namespace QC

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "CliParser.h"

#include "Cdt.h"
#include "CliArgumentDefinitions.h"
#include "CliHelp.h"
#include "CliOptions.h"
#include "ExceptionHandler.h"
#include "FileSystem.h"

#include <algorithm>
#include <cctype>
#include <KL/kLogger.h>
#include <sstream>
#include <stdexcept>

#define CHECK_IF_VALUE_EXISTS                                                                                          \
   if(currentArg->second.empty())                                                                                      \
   {                                                                                                                   \
      QC_THROW_CMDLINE_ERROR(                                                                                          \
         QC::Common::Exception::MISSING_REQUIRED_PARAMETER,                                                            \
         currentArg->first,                                                                                            \
         "Missing value for: '--" + currentArg->first + "',\nUse -h or --help for usage information"                   \
      );                                                                                                               \
   }                                                                                                                   \
   std::string value = currentArg->second;
#define CHECK_IF_NO_VALUE                                                                                              \
   if(!currentArg->second.empty())                                                                                     \
   {                                                                                                                   \
      QC_THROW_CMDLINE_ERROR(                                                                                          \
         QC::Common::Exception::INVALID_PARAMETERS,                                                                    \
         currentArg->first,                                                                                            \
         "Redundant value for: '--" + currentArg->first + "',\nUse -h or --help for usage information"                 \
      );                                                                                                               \
   }

namespace QC {
namespace CLI {

// Static member definitions
std::set<std::string> CliParser::VALID_OPTIONS;

void CliParser::initializeValidOptions()
{
   for(const auto& cmd: CliArgumentDefinitions::getCommandDefinitions())
   {
      CliParser::VALID_OPTIONS.insert(cmd.name);
   }

   for(const auto& argument: CliArgumentDefinitions::getArgumentDefinitions())
   {
      CliParser::VALID_OPTIONS.insert(argument.second.name);
   }
}

CLI_ARGUMENT CliParser::extractOption(const std::string& arg)
{
   std::set<std::string, std::string> option;
   if(arg == "-h")
   {
      return CLI_ARGUMENT(arg, std::string());
   }
   else if(arg.rfind("--", 0) == 0)
   {
      size_t eqPos = arg.find('=');
      if(eqPos != std::string::npos)
      {
         std::string key = arg.substr(2, eqPos - 2);
         if(!isValidOption(key))
         {
            QC_THROW_CMDLINE_ERROR(
               QC::Common::Exception::INVALID_PARAMETERS,
               key,
               "Unrecognized option: '" + std::string(arg) +
                  "'\nUse -h or --help for usage "
                  "information"
            );
         }
         std::string value = arg.substr(eqPos + 1);
         return CLI_ARGUMENT(key, value);
      }
      else
      {
         std::string key = arg.substr(2);
         if(!isValidOption(key))
         {
            QC_THROW_CMDLINE_ERROR(
               QC::Common::Exception::INVALID_PARAMETERS,
               key,
               "Unrecognized option: '" + std::string(arg) +
                  "'\nUse -h or --help for usage "
                  "information"
            );
         }
         return CLI_ARGUMENT(key, std::string());
      }
   }
   QC_THROW_CMDLINE_ERROR(
      QC::Common::Exception::INVALID_PARAMETERS,
      arg,
      "Unrecognized option: '" + std::string(arg) + "'\nUse -h or --help for usage information"
   );
}

bool CliParser::isValidOption(const std::string& option)
{
   return VALID_OPTIONS.find(option) != VALID_OPTIONS.end();
}

/// <summary>
/// All necessary argument are parsed here and validated, should not put
/// validation in execution function
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
CliOptions CliParser::parse(int argc, char* argv[])
{

   CliOptions options;

   initializeValidOptions();

   // No arguments - show help
   if(argc == 1)
   {
      options.setCommandType(CliOptions::CommandType::HELP);
      return options;
   }


   std::vector<QC::CLI::CommandDefinition> commandsFound;
   for(int i = 1; i < argc; i++)
   {
      for(auto const& cmd: CliArgumentDefinitions::getCommandDefinitions())
      {
         CLI_ARGUMENT argument = extractOption(argv[i]);
         if(argument.first == cmd.name)
         {
            if(argument.second.empty())
            {
               commandsFound.push_back(cmd);
               break;
            }
            else
            {
               QC_THROW_CMDLINE_ERROR(QC::Common::Exception::INVALID_COMMAND, argument.first, argument.second);
            }
         }
      }
   }

   if(commandsFound.size() <= 0)
   {
      CFLOG_ERROR(
         "Unable to determine command. Use -h or --help for usage "
         "information.",
         false
      );
      QC_THROW_CMDLINE_ERROR(
         QC::Common::Exception::MISSING_REQUIRED_PARAMETER,
         "command",
         "Unable to determine command"
      );
   }
   else if(commandsFound.size() > 1)
   {
      std::string cmdsError = "Too many commands: ";
      for(auto const& cmd: commandsFound)
      {
         cmdsError += ("'" + cmd.name + "' ");
      }
      cmdsError += "Use -h or --help for usage information.";
      CFLOG_ERROR(cmdsError, false);
      QC_THROW_CMDLINE_ERROR(QC::Common::Exception::INVALID_PARAMETERS, "command", "Too many commands");
   }

   CommandDefinition command = commandsFound.at(0);
   options.setCommandType(command.commandType);
   if(command.setter)
   {
      command.setter(options);
   }
   if(command.commandType == CliOptions::CommandType::HELP)
   {
      return options;
   }
   // Parse all arguments
   std::map<std::string, std::string> parsedArgs;
   for(int i = 1; i < argc; i++)
   {
      CLI_ARGUMENT argument = extractOption(argv[i]);
      std::string arg = argv[i];
      if(parsedArgs.find(argument.first) != parsedArgs.end())
      {
         if(parsedArgs[argument.first] != argument.second)
         {
            QC_THROW_CMDLINE_ERROR(
               QC::Common::Exception::CONFLICTING_VALUE,
               argument.first,
               "Duplicate parameter: " + argument.first +
                  " with conflicting value found. Previous "
                  "value: " +
                  parsedArgs[argument.first] + " New value: " + argument.second
            );
         }
      }
      parsedArgs[argument.first] = argument.second;
   }

   std::vector<std::string> usedArgs;
   for(auto& requiredArgs: command.requiredArgs)
   {
      auto currentArg = parsedArgs.find(requiredArgs.name);
      if(currentArg == parsedArgs.end())
      {
         QC_THROW_CMDLINE_ERROR(
            QC::Common::Exception::MISSING_REQUIRED_PARAMETER,
            requiredArgs.name,
            "Missing argument: --" + requiredArgs.name + " for command: --" + command.name +
               ",\nUse -h or --help for usage information"
         );
      }
      usedArgs.push_back(requiredArgs.name);
      const auto* def = CliArgumentDefinitions::getArgumentDefinition(requiredArgs.name);
      if(def)
      {
         switch(def->type)
         {
            case ArgumentType::FLAG:
               CHECK_IF_NO_VALUE;
               break;
            case ArgumentType::BOOLEAN:
            case ArgumentType::STRING:
            case ArgumentType::INTEGER:
            case ArgumentType::STRING_LIST:
            case ArgumentType::PATH:
               CHECK_IF_VALUE_EXISTS;
               break;
         }

         if(def->setter)
         {
            try
            {
               def->setter(options, currentArg->second);
            }
            catch(const QC::Common::CommandLineException&)
            {
               throw; // Re-throw RCA exceptions
            }
            catch(const std::exception& e)
            {
               QC_THROW_CMDLINE_ERROR(
                  QC::Common::Exception::INVALID_PARAMETERS,
                  currentArg->first,
                  "Invalid value for '--" + currentArg->first + "': " + e.what()
               );
            }
         }
      }
      else
      {
         QC_THROW_CMDLINE_ERROR(
            QC::Common::Exception::INVALID_PARAMETERS,
            currentArg->first,
            "Unknown argument: " + currentArg->first
         );
      }
   }

   for(auto& optionalArgs: command.optionalArgs)
   {
      auto currentArg = parsedArgs.find(optionalArgs.name);
      if(currentArg != parsedArgs.end())
      {
         usedArgs.push_back(optionalArgs.name);
         const auto* def = CliArgumentDefinitions::getArgumentDefinition(optionalArgs.name);
         if(def)
         {
            switch(def->type)
            {
               case ArgumentType::FLAG:
                  CHECK_IF_NO_VALUE;
                  break;
               case ArgumentType::BOOLEAN:
               case ArgumentType::STRING:
               case ArgumentType::INTEGER:
               case ArgumentType::STRING_LIST:
               case ArgumentType::PATH:
                  CHECK_IF_VALUE_EXISTS;
                  break;
            }

            if(def->setter)
            {
               try
               {
                  def->setter(options, currentArg->second);
               }
               catch(const QC::Common::CommandLineException&)
               {
                  throw; // Re-throw RCA exceptions
               }
               catch(const std::exception& e)
               {
                  QC_THROW_CMDLINE_ERROR(
                     QC::Common::Exception::INVALID_PARAMETERS,
                     currentArg->first,
                     "Invalid value for '--" + currentArg->first + "': " + e.what()
                  );
               }
            }
         }
         else
         {
            QC_THROW_CMDLINE_ERROR(
               QC::Common::Exception::INVALID_PARAMETERS,
               currentArg->first,
               "Unknown argument: " + currentArg->first
            );
         }
      }
   }

   // List ignored parameters
   for(auto& arg: parsedArgs)
   {
      if(arg.first != command.name)
      {
         if(std::find(usedArgs.begin(), usedArgs.end(), arg.first) == usedArgs.end())
         {
            QC_THROW_CMDLINE_ERROR(
               QC::Common::Exception::INVALID_PARAMETERS,
               arg.first,
               "Invalid argument '" + arg.first + "' for command " + "'" + command.name
            );
         }
      }
   }
   // Auto-detect files for download build command if not explicitly provided
   // NOTE:  Only included command because that contains --build
   // rawprogram/patch XMLs
   if((options.command == CliOptions::CommandType::DOWNLOAD_BUILD ||
       options.command == CliOptions::CommandType::READ_IMAGES ||
       options.command == CliOptions::CommandType::CREATE_VIP_DIGEST ||
       options.command == CliOptions::CommandType::CREATE_VALIDATION_DIGEST) &&
      !options.buildPath.empty())
   {
      // Auto-detect patch XML files if no XML is provided, find patch first for
      // logic reason
      if(!options.downloadBuildOptions.__isset.patchXmlList && !options.downloadBuildOptions.__isset.rawXmlList)
      {
         std::vector<std::string> patchXmls = FileSystem::findPatchProgramXmls(options.buildPath);
         if(!patchXmls.empty())
         {
            options.downloadBuildOptions.__set_patchXmlList(patchXmls);
         }
      }

      // Auto-detect raw XML files if not provided
      if(!options.downloadBuildOptions.__isset.rawXmlList)
      {
         std::vector<std::string> rawXmls = FileSystem::findRawProgramXmls(options.buildPath);
         if(!rawXmls.empty())
         {
            options.downloadBuildOptions.__set_rawXmlList(rawXmls);
         }
      }

      // Process CDT binary if provided
      if(!options.cdtBinaryPath.empty())
      {
         // Validate CDT binary exists
         if(!FileSystem::fileExists(options.cdtBinaryPath))
         {
            QC_THROW_FILE_ERROR(
               QC::Common::Exception::FILE_NOT_FOUND,
               options.cdtBinaryPath,
               "CDT binary file validation"
            );
         }

         // Get current raw XML list for CDT processing
         std::vector<std::string> currentRawXmls;
         if(options.downloadBuildOptions.__isset.rawXmlList)
         {
            currentRawXmls = options.downloadBuildOptions.rawXmlList;
         }

         if(!currentRawXmls.empty())
         {
            // Process CDT modification
            Cdt::CdtResult cdtResult =
               Cdt::modifyCdtRawprogramXml(options.cdtBinaryPath, options.buildPath, currentRawXmls);

            if(!cdtResult.success)
            {
               QC_THROW(
                  QC::Common::Exception::VALIDATION_ERROR,
                  "CDT processing failed: " + cdtResult.error,
                  "CDT modification"
               );
            }

            // Update raw XML list with modified files
            options.downloadBuildOptions.__set_rawXmlList(cdtResult.updatedRawXmlList);

            // Store CDT processing information for potential logging
            if(options.verbose && !cdtResult.modifiedXmlPath.empty())
            {
               // Note: In a real implementation, you might want to log this
               // information For now, we'll just ensure the processing
               // completed successfully
            }
         }
      }

      if(options.command != CliOptions::CommandType::CREATE_VIP_DIGEST &&
         options.command != CliOptions::CommandType::CREATE_VALIDATION_DIGEST)
      {
         // If auto find programmer always use saharaImageList approach - never
         // set firehoseProgPath
         if(!options.downloadBuildOptions.__isset.firehoseProgPath &&
            !options.downloadBuildOptions.__isset.saharaImageList)
         {
            // Use the unified function that handles priority order
            std::map<int, std::string> saharaImages = FileSystem::findDeviceProgrammer(options.buildPath);

            if(saharaImages.empty())
            {
               QC_THROW_FILE_ERROR(
                  QC::Common::Exception::FILE_NOT_FOUND,
                  options.buildPath,
                  "Unable to find device programmer in build path: " + options.buildPath +
                     "\nSearched for: qsahara_device_programmer.xml, "
                     "xbl_s_devprg_ns.melf/.elf, " +
                     "prog_firehose_ddr.mbn/.elf, prog_firehose_lite.mbn/.elf"
               );
            }

            // Validate that all files in the sahara image list exist
            for(const auto& saharaImage: saharaImages)
            {
               if(!FileSystem::isFile(saharaImage.second))
               {
                  QC_THROW_FILE_ERROR(
                     QC::Common::Exception::FILE_NOT_FOUND,
                     saharaImage.second,
                     "sahara image list file validation"
                  );
               }
            }
            options.downloadBuildOptions.__set_saharaImageList(saharaImages);
         }
      }
   }
   options.validate();
   return options;
}

} // namespace CLI
} // namespace QC

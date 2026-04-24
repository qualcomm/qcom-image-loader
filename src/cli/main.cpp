// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "CliCommands.h"
#include "CliHelp.h"
#include "CliParser.h"
#include "DeviceDiscovery.h"
#include "ExceptionHandler.h"
#include "FileSystem.h"

#include <exception>
#include <iostream>
#include <KL/kLogger.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

std::string getVersionString()
{
   return std::string(QC::DeviceDiscovery::getDLLVersion());
}

std::string getCurrentTimeString()
{
   char buffer[32];
   time_t t = time(nullptr);
   strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", localtime(&t));
   return std::string(buffer);
}

void printCurrentCommandLine(int argc, char* argv[])
{
   std::ostringstream oss;
   oss << "Current command: " << std::endl;

   for(int i = 1; i < argc; ++i)
   {
      oss << " " << argv[i];
   }
   FLOG_INFO(oss.str());
}

bool createLogDirectory()
{
   try
   {
      return QC::CLI::FileSystem::createDirectoryReclusively(TMP_DIR);
   }
   catch(const std::exception& e)
   {
      throw std::runtime_error(e.what());
   }
}

bool createTempDirectory()
{
   try
   {
      return QC::CLI::FileSystem::createDirectoryReclusively(TMP_DIR);
   }
   catch(const std::exception& e)
   {
      throw std::runtime_error(e.what());
   }
}


int main(int argc, char* argv[])
{
   // Configure exception handler based on command line arguments
   bool verboseMode = false;
   bool jsonMode = false;

   // Quick scan for verbose/json flags before full parsing
   for(int i = 1; i < argc; ++i)
   {
      std::string arg = argv[i];
      if(arg == "--verbose" || arg == "-v")
      {
         verboseMode = true;
      }
      else if(arg == "--json")
      {
         jsonMode = true;
      }
   }



   QC::Common::ExceptionHandler::setVerboseMode(verboseMode);
   QC::Common::ExceptionHandler::setJsonMode(jsonMode);
   QC::Common::ExceptionHandler::setLogLevel(QC::Common::ExceptionHandler::LEVEL_ERROR);

   int exitCode = 0;

   try
   {
      if(createLogDirectory())
      {
        // Initialize logger with log directory and size limit of 100MB
        KL::Logger::get_instance().init(LOG_DIR, 0, KL::MAX_LOG_FILE_SIZE, getVersionString());
        FLOG_INFO("Successfully created LOG directory: " + std::string(LOG_DIR));
      }
      else {
        throw std::runtime_error("Failed to create LOG directory:  " + std::string(LOG_DIR));
      }

      if (createTempDirectory())
      {
        FLOG_INFO("Successfully created TMP directory: " + std::string(TMP_DIR));

      }
      else {
            throw std::runtime_error("Failed to create TMP directory:  " + std::string(TMP_DIR));
      };

      printCurrentCommandLine(argc, argv);

      // Parse command-line arguments
      QC::CLI::CliOptions options = QC::CLI::CliParser::parse(argc, argv);

      // Execute command
      exitCode = QC::CLI::CliCommands::execute(options);
   }
   catch(const QC::Common::Exception& ex)
   {
      // Handle CLI-specific exceptions
      auto result = QC::Common::ExceptionHandler::handleException(ex, "main application execution");
      if(!result.message.empty())
      {
         std::cerr << result.message << std::endl;
      }
      exitCode = result.exitCode;
   }
   catch(const std::runtime_error& ex)
   {
      // Handle runtime errors from DLL (converted from TABL exceptions)
      std::cerr << "Runtime Error: " << ex.what() << std::endl;
      exitCode = 1;
   }
   catch(const std::exception& ex)
   {
      // Handle other standard exceptions
      std::cerr << "Error: " << ex.what() << std::endl;
      exitCode = 1;
   }
   catch(...)
   {
      // Handle unknown exceptions
      std::cerr << "Unknown error occurred" << std::endl;
      exitCode = 1;
   }

   // Flush all pending log entries and shut down the logger worker thread
   // before returning. Without this, queued log entries may be lost because
   // the async worker thread is still running when main() returns, and the
   // singleton destructor fires during static cleanup — too late for
   // reliable I/O and prone to segfaults.
   KL::Logger::get_instance().flush_and_shutdown();

   return exitCode;
}

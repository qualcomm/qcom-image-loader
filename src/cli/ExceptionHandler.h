// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "Definitions.h"
#include "Exception.h"

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>

namespace QC {
namespace Common {

// --------------------------------------------------------------------------
// Exception Handler Class
//
/// Provides centralized exception handling, logging, and recovery mechanisms
// --------------------------------------------------------------------------
class ExceptionHandler
{
public:
   /// Logging levels for exception reporting
   enum LogLevel
   {
      LEVEL_DEBUG = 0,
      LEVEL_INFO = 1,
      LEVEL_WARNING = 2,
      LEVEL_ERROR = 3,
      LEVEL_CRITICAL = 4
   };

   /// Exception handling result
   struct HandleResult
   {
      bool shouldContinue = false;
      bool shouldRetry = false;
      int exitCode = 1;
      std::string message;
   };

   /// Set global log level
   static void setLogLevel(LogLevel level)
   {
      logLevel = level;
   }

   /// Set verbose mode for detailed error reporting
   static void setVerboseMode(bool verbose)
   {
      verboseMode = verbose;
   }

   /// Set JSON output mode
   static void setJsonMode(bool jsonModeParam)
   {
      jsonMode = jsonModeParam;
   }

   /// Handle exception with appropriate logging and user feedback
   static HandleResult handleException(const Exception& ex, const std::string& operation = "")
   {
      HandleResult result;

      // Determine log level based on exception severity
      LogLevel level = getLogLevelFromSeverity(ex.getSeverity());

      // Log the exception if appropriate
      if(level >= logLevel)
      {
         logException(ex, operation, level);
      }

      // Determine handling strategy
      if(ex.isCritical())
      {
         result.shouldContinue = false;
         result.exitCode = 2; // Critical error exit code
      }
      else if(ex.isRecoverable())
      {
         result.shouldRetry = true;
         result.shouldContinue = true;
         result.exitCode = 0;
      }
      else
      {
         result.shouldContinue = false;
         result.exitCode = 1; // Standard error exit code
      }

      // Format user message
      if(jsonMode)
      {
         result.message = ex.toJson();
      }
      else
      {
         // Always show formatted message with suggestions
         result.message = ex.getFormattedMessage();
      }

      return result;
   }

   /// Get current timestamp for logging
   static std::string getCurrentTimestamp()
   {
      auto now = std::chrono::system_clock::now();
      auto timeValue = std::chrono::system_clock::to_time_t(now);
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

      std::ostringstream oss;
      oss << std::put_time(std::localtime(&timeValue), "%Y-%m-%d %H:%M:%S");
      oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
      return oss.str();
   }

private:
   static LogLevel logLevel;
   static bool verboseMode;
   static bool jsonMode;

   static LogLevel getLogLevelFromSeverity(Exception::Severity severity)
   {
      switch(severity)
      {
         case Exception::TOOLS_INFO:
            return LEVEL_INFO;
         case Exception::TOOLS_WARNING:
            return LEVEL_WARNING;
         case Exception::TOOLS_ERROR:
            return LEVEL_ERROR;
         case Exception::TOOLS_CRITICAL:
            return LEVEL_CRITICAL;
         default:
            return LEVEL_ERROR;
      }
   }

   static void logException(const Exception& ex, const std::string& operation, LogLevel level)
   {
      std::string levelStr = getLogLevelString(level);
      std::string timestamp = getCurrentTimestamp();

      if(jsonMode)
      {
         std::cerr << "{" << "\"timestamp\":\"" << timestamp << "\"," << "\"level\":\"" << levelStr << "\","
                   << "\"operation\":\"" << operation << "\"," << "\"exception\":" << ex.toJson() << "}" << std::endl;
      }
      else
      {
         std::cerr << "[" << timestamp << "] [" << levelStr << "]";
         if(!operation.empty())
         {
            std::cerr << " [" << operation << "]";
         }
         std::cerr << " " << (verboseMode ? ex.getFormattedMessage() : ex.getMessage()) << std::endl;
      }
   }

   static void logRetry(int retryCount, int maxRetries, const std::string& operation)
   {
      if(logLevel <= LEVEL_INFO)
      {
         std::string timestamp = getCurrentTimestamp();
         std::cerr << "[" << timestamp << "] [INFO] Retrying operation";
         if(!operation.empty())
         {
            std::cerr << " '" << operation << "'";
         }
         std::cerr << " (attempt " << retryCount << "/" << maxRetries << ")" << std::endl;
      }
   }

   static std::string getLogLevelString(LogLevel level)
   {
      switch(level)
      {
         case LEVEL_DEBUG:
            return "DEBUG";
         case LEVEL_INFO:
            return "INFO";
         case LEVEL_WARNING:
            return "WARNING";
         case LEVEL_ERROR:
            return "ERROR";
         case LEVEL_CRITICAL:
            return "CRITICAL";
         default:
            return "UNKNOWN";
      }
   }
};

} // namespace Common
} // namespace QC

/// Macro for throwing exceptions with context
#define QC_THROW(errorCode, message, context) throw QC::Common::Exception(errorCode, message, context)

/// Macro for throwing file system exceptions (suggestion is optional)
#define QC_THROW_FILE_ERROR(errorCode, filePath, operation, ...)                                                       \
   throw QC::Common::FileSystemException(errorCode, filePath, operation, ##__VA_ARGS__)

/// Macro for throwing device exceptions (suggestion is optional)
#define QC_THROW_DEVICE_ERROR(errorCode, deviceInfo, operation, ...)                                                   \
   throw QC::Common::DeviceException(errorCode, deviceInfo, operation, ##__VA_ARGS__)

/// Macro for throwing command line exceptions (suggestion is optional)
#define QC_THROW_CMDLINE_ERROR(errorCode, parameter, value, ...)                                                       \
   throw QC::Common::CommandLineException(errorCode, parameter, value, ##__VA_ARGS__)

/// Macro for validating required parameters
#define QC_REQUIRE_PARAM(condition, parameter)                                                                         \
   if(!(condition))                                                                                                    \
   {                                                                                                                   \
      QC_THROW_CMDLINE_ERROR(QC::Common::Exception::MISSING_REQUIRED_PARAMETER, parameter, "");                        \
   }

/// Macro to convert ErrorType from DLL to RCA Exception and throw
#define THROW_FROM_ERROR_TYPE(errorType, operation) QC::Common::throwFromErrorTypeImpl(errorType, operation)

// Helper function for THROW_FROM_ERROR_TYPE macro (implemented in
// ExceptionHandler.cpp)
namespace QC {
namespace Common {
void throwFromErrorTypeImpl(const QC::ErrorType& errorType, const std::string& operation);
}
} // namespace QC

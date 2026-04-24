// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <exception>
#include <sstream>
#include <string>

namespace QC {
namespace Common {

// --------------------------------------------------------------------------
// Exception
//
/// Base exception class for operations with structured error reporting
/// and Root Cause Analysis (RCA) support
// --------------------------------------------------------------------------
class Exception : public std::exception
{
public:
   /// Error codes for different operation failures
   enum ErrorCode
   {
      TOOLS_NO_ERROR = 0,
      UNKNOWN_ERROR,
      INVALID_PARAMETERS,
      INVALID_COMMAND,
      MISSING_REQUIRED_PARAMETER,
      INVALID_FILE_PATH,
      FILE_NOT_FOUND,
      FILE_ACCESS_DENIED,
      FILE_READ_ERROR,
      FILE_WRITE_ERROR,
      DIRECTORY_NOT_FOUND,
      DIRECTORY_ACCESS_DENIED,
      DEVICE_NOT_FOUND,
      DEVICE_CONNECTION_FAILED,
      DEVICE_COMMUNICATION_ERROR,
      DEVICE_TIMEOUT,
      DEVICE_DISCONNECTED,
      PROTOCOL_ERROR,
      PROTOCOL_UNSUPPORTED,
      DOWNLOAD_FAILED,
      UPLOAD_FAILED,
      VERIFICATION_FAILED,
      CHECKSUM_MISMATCH,
      MEMORY_ALLOCATION_ERROR,
      CONFIGURATION_ERROR,
      PERMISSION_DENIED,
      OPERATION_CANCELLED,
      OPERATION_TIMEOUT,
      NETWORK_ERROR,
      PARSING_ERROR,
      VALIDATION_ERROR,
      INITIALIZATION_FAILED,
      CLEANUP_FAILED,
      RESOURCE_BUSY,
      RESOURCE_UNAVAILABLE,
      VERSION_MISMATCH,
      COMPATIBILITY_ERROR,
      INTERNAL_ERROR,
      CONFLICTING_VALUE
   };

   /// Constructor with error code and message
   Exception(ErrorCode errorCode, const std::string& message = "", const std::string& context = "")
   : m_errorCode(errorCode)
   , m_message(message.empty() ? getErrorCodeString(errorCode) : message)
   , m_context(context)
   {
      updateWhatMessage();
   }

   /// Constructor with error code, message, and additional details
   Exception(ErrorCode errorCode, const std::string& message, const std::string& context, const std::string& suggestion)
   : m_errorCode(errorCode)
   , m_message(message.empty() ? getErrorCodeString(errorCode) : message)
   , m_context(context)
   , m_suggestion(suggestion)
   {
      updateWhatMessage();
   }

   /// Copy constructor
   Exception(const Exception& other)
   : m_errorCode(other.m_errorCode)
   , m_message(other.m_message)
   , m_context(other.m_context)
   , m_suggestion(other.m_suggestion)
   , m_whatMessage(other.m_whatMessage)
   {
   }

   /// Assignment operator
   Exception& operator=(const Exception& other)
   {
      if(this != &other)
      {
         m_errorCode = other.m_errorCode;
         m_message = other.m_message;
         m_context = other.m_context;
         m_suggestion = other.m_suggestion;
         m_whatMessage = other.m_whatMessage;
      }
      return *this;
   }

   /// Destructor
   virtual ~Exception() noexcept = default;

   /// Get error code
   ErrorCode getErrorCode() const noexcept
   {
      return m_errorCode;
   }

   /// Get error message
   const std::string& getMessage() const noexcept
   {
      return m_message;
   }

   /// Get error context
   const std::string& getContext() const noexcept
   {
      return m_context;
   }

   /// Get suggestion for fixing the error
   const std::string& getSuggestion() const noexcept
   {
      return m_suggestion;
   }

   /// Override std::exception::what()
   const char* what() const noexcept override
   {
      return m_whatMessage.c_str();
   }

   /// Get formatted error information as JSON
   std::string toJson() const
   {
      std::ostringstream json;
      json << "{" << "\"errorCode\":" << static_cast<int>(m_errorCode) << "," << "\"errorName\":\""
           << getErrorCodeName(m_errorCode) << "\"," << "\"message\":\"" << escapeJsonString(m_message) << "\"";

      if(!m_context.empty())
      {
         json << ",\"context\":\"" << escapeJsonString(m_context) << "\"";
      }

      if(!m_suggestion.empty())
      {
         json << ",\"suggestion\":\"" << escapeJsonString(m_suggestion) << "\"";
      }

      json << "}";
      return json.str();
   }

   /// Get user-friendly formatted error message
   std::string getFormattedMessage() const
   {
      std::ostringstream formatted;
      formatted << "[" << getErrorCodeName(m_errorCode) << "] " << m_message;

      if(!m_context.empty())
      {
         formatted << "\nContext: " << m_context;
      }

      if(!m_suggestion.empty())
      {
         formatted << "\nSuggestion: " << m_suggestion;
      }

      return formatted.str();
   }

   /// Check if error is recoverable
   bool isRecoverable() const noexcept
   {
      switch(m_errorCode)
      {
         case DEVICE_TIMEOUT:
         case DEVICE_COMMUNICATION_ERROR:
         case NETWORK_ERROR:
         case RESOURCE_BUSY:
         case OPERATION_TIMEOUT:
            return true;
         default:
            return false;
      }
   }

   /// Check if error is critical (should terminate application)
   bool isCritical() const noexcept
   {
      switch(m_errorCode)
      {
         case MEMORY_ALLOCATION_ERROR:
         case INTERNAL_ERROR:
         case INITIALIZATION_FAILED:
            return true;
         default:
            return false;
      }
   }

   /// Get error severity level
   enum Severity
   {
      TOOLS_INFO,
      TOOLS_WARNING,
      TOOLS_ERROR,
      TOOLS_CRITICAL
   };
   Severity getSeverity() const noexcept
   {
      if(isCritical()) return TOOLS_CRITICAL;

      switch(m_errorCode)
      {
         case TOOLS_NO_ERROR:
            return TOOLS_INFO;
         case OPERATION_CANCELLED:
         case VERSION_MISMATCH:
            return TOOLS_WARNING;
         default:
            return TOOLS_ERROR;
      }
   }

   /// Static method to get error code string description
   static std::string getErrorCodeString(ErrorCode errorCode)
   {
      switch(errorCode)
      {
         case TOOLS_NO_ERROR:
            return "No error";
         case UNKNOWN_ERROR:
            return "Unknown error occurred";
         case INVALID_PARAMETERS:
            return "Invalid parameters provided";
         case INVALID_COMMAND:
            return "Invalid command specified";
         case MISSING_REQUIRED_PARAMETER:
            return "Missing required parameter";
         case INVALID_FILE_PATH:
            return "Invalid file path";
         case FILE_NOT_FOUND:
            return "File not found";
         case FILE_ACCESS_DENIED:
            return "File access denied";
         case FILE_READ_ERROR:
            return "File read error";
         case FILE_WRITE_ERROR:
            return "File write error";
         case DIRECTORY_NOT_FOUND:
            return "Directory not found";
         case DIRECTORY_ACCESS_DENIED:
            return "Directory access denied";
         case DEVICE_NOT_FOUND:
            return "Device not found";
         case DEVICE_CONNECTION_FAILED:
            return "Device connection failed";
         case DEVICE_COMMUNICATION_ERROR:
            return "Device communication error";
         case DEVICE_TIMEOUT:
            return "Device operation timeout";
         case DEVICE_DISCONNECTED:
            return "Device disconnected";
         case PROTOCOL_ERROR:
            return "Protocol error";
         case PROTOCOL_UNSUPPORTED:
            return "Protocol not supported";
         case DOWNLOAD_FAILED:
            return "Download operation failed";
         case UPLOAD_FAILED:
            return "Upload operation failed";
         case VERIFICATION_FAILED:
            return "Verification failed";
         case CHECKSUM_MISMATCH:
            return "Checksum mismatch";
         case MEMORY_ALLOCATION_ERROR:
            return "Memory allocation error";
         case CONFIGURATION_ERROR:
            return "Configuration error";
         case PERMISSION_DENIED:
            return "Permission denied";
         case OPERATION_CANCELLED:
            return "Operation cancelled";
         case OPERATION_TIMEOUT:
            return "Operation timeout";
         case NETWORK_ERROR:
            return "Network error";
         case PARSING_ERROR:
            return "Parsing error";
         case VALIDATION_ERROR:
            return "Validation error";
         case INITIALIZATION_FAILED:
            return "Initialization failed";
         case CLEANUP_FAILED:
            return "Cleanup failed";
         case RESOURCE_BUSY:
            return "Resource busy";
         case RESOURCE_UNAVAILABLE:
            return "Resource unavailable";
         case VERSION_MISMATCH:
            return "Version mismatch";
         case COMPATIBILITY_ERROR:
            return "Compatibility error";
         case INTERNAL_ERROR:
            return "Internal error";
         default:
            return "Unknown error code";
      }
   }

   /// Static method to get error code name
   static std::string getErrorCodeName(ErrorCode errorCode)
   {
      switch(errorCode)
      {
         case TOOLS_NO_ERROR:
            return "NO_ERROR";
         case UNKNOWN_ERROR:
            return "UNKNOWN_ERROR";
         case INVALID_PARAMETERS:
            return "INVALID_PARAMETERS";
         case INVALID_COMMAND:
            return "INVALID_COMMAND";
         case MISSING_REQUIRED_PARAMETER:
            return "MISSING_REQUIRED_PARAMETER";
         case INVALID_FILE_PATH:
            return "INVALID_FILE_PATH";
         case FILE_NOT_FOUND:
            return "FILE_NOT_FOUND";
         case FILE_ACCESS_DENIED:
            return "FILE_ACCESS_DENIED";
         case FILE_READ_ERROR:
            return "FILE_READ_ERROR";
         case FILE_WRITE_ERROR:
            return "FILE_WRITE_ERROR";
         case DIRECTORY_NOT_FOUND:
            return "DIRECTORY_NOT_FOUND";
         case DIRECTORY_ACCESS_DENIED:
            return "DIRECTORY_ACCESS_DENIED";
         case DEVICE_NOT_FOUND:
            return "DEVICE_NOT_FOUND";
         case DEVICE_CONNECTION_FAILED:
            return "DEVICE_CONNECTION_FAILED";
         case DEVICE_COMMUNICATION_ERROR:
            return "DEVICE_COMMUNICATION_ERROR";
         case DEVICE_TIMEOUT:
            return "DEVICE_TIMEOUT";
         case DEVICE_DISCONNECTED:
            return "DEVICE_DISCONNECTED";
         case PROTOCOL_ERROR:
            return "PROTOCOL_ERROR";
         case PROTOCOL_UNSUPPORTED:
            return "PROTOCOL_UNSUPPORTED";
         case DOWNLOAD_FAILED:
            return "DOWNLOAD_FAILED";
         case UPLOAD_FAILED:
            return "UPLOAD_FAILED";
         case VERIFICATION_FAILED:
            return "VERIFICATION_FAILED";
         case CHECKSUM_MISMATCH:
            return "CHECKSUM_MISMATCH";
         case MEMORY_ALLOCATION_ERROR:
            return "MEMORY_ALLOCATION_ERROR";
         case CONFIGURATION_ERROR:
            return "CONFIGURATION_ERROR";
         case PERMISSION_DENIED:
            return "PERMISSION_DENIED";
         case OPERATION_CANCELLED:
            return "OPERATION_CANCELLED";
         case OPERATION_TIMEOUT:
            return "OPERATION_TIMEOUT";
         case NETWORK_ERROR:
            return "NETWORK_ERROR";
         case PARSING_ERROR:
            return "PARSING_ERROR";
         case VALIDATION_ERROR:
            return "VALIDATION_ERROR";
         case INITIALIZATION_FAILED:
            return "INITIALIZATION_FAILED";
         case CLEANUP_FAILED:
            return "CLEANUP_FAILED";
         case RESOURCE_BUSY:
            return "RESOURCE_BUSY";
         case RESOURCE_UNAVAILABLE:
            return "RESOURCE_UNAVAILABLE";
         case VERSION_MISMATCH:
            return "VERSION_MISMATCH";
         case COMPATIBILITY_ERROR:
            return "COMPATIBILITY_ERROR";
         case INTERNAL_ERROR:
            return "INTERNAL_ERROR";
         case CONFLICTING_VALUE:
            return "CONFLICTING_VALUE";
         default:
            return "UNKNOWN_ERROR_CODE";
      }
   }

private:
   ErrorCode m_errorCode;
   std::string m_message;
   std::string m_context;
   std::string m_suggestion;
   std::string m_whatMessage;

   void updateWhatMessage()
   {
      std::ostringstream oss;
      oss << "[" << getErrorCodeName(m_errorCode) << "] " << m_message;
      if(!m_context.empty())
      {
         oss << " (Context: " << m_context << ")";
      }
      m_whatMessage = oss.str();
   }

   std::string escapeJsonString(const std::string& str) const
   {
      std::string escaped;
      for(char c: str)
      {
         switch(c)
         {
            case '"':
               escaped += "\\\"";
               break;
            case '\\':
               escaped += "\\\\";
               break;
            case '\b':
               escaped += "\\b";
               break;
            case '\f':
               escaped += "\\f";
               break;
            case '\n':
               escaped += "\\n";
               break;
            case '\r':
               escaped += "\\r";
               break;
            case '\t':
               escaped += "\\t";
               break;
            default:
               escaped += c;
               break;
         }
      }
      return escaped;
   }
};

// --------------------------------------------------------------------------
// Specialized Exception Classes
// --------------------------------------------------------------------------

/// File system related exceptions
class FileSystemException : public Exception
{
public:
   FileSystemException(
      ErrorCode errorCode,
      const std::string& filePath,
      const std::string& operation = "",
      const std::string& customSuggestion = ""
   )
   : Exception(
        errorCode,
        getFileSystemMessage(errorCode, operation),
        filePath,
        customSuggestion.empty() ? getFileSystemSuggestion(errorCode) : customSuggestion
     )
   {
   }

private:
   static std::string getFileSystemMessage(ErrorCode errorCode, const std::string& operation)
   {
      std::string msg = getErrorCodeString(errorCode);
      if(!operation.empty())
      {
         msg += " during " + operation;
      }
      return msg;
   }

   static std::string getFileSystemSuggestion(ErrorCode errorCode)
   {
      switch(errorCode)
      {
         case FILE_NOT_FOUND:
            return "Check if the file path is correct and the file exists";
         case FILE_ACCESS_DENIED:
            return "Check file permissions or run with appropriate privileges";
         case DIRECTORY_NOT_FOUND:
            return "Verify the directory path exists or create it first";
         case DIRECTORY_ACCESS_DENIED:
            return "Check directory permissions or run with appropriate "
                   "privileges";
         default:
            return "";
      }
   }
};

/// Device communication related exceptions
class DeviceException : public Exception
{
public:
   DeviceException(
      ErrorCode errorCode,
      const std::string& deviceInfo = "",
      const std::string& operation = "",
      const std::string& customSuggestion = ""
   )
   : Exception(
        errorCode,
        getDeviceMessage(errorCode, operation),
        deviceInfo,
        customSuggestion.empty() ? getDeviceSuggestion(errorCode) : customSuggestion
     )
   {
   }

private:
   static std::string getDeviceMessage(ErrorCode errorCode, const std::string& operation)
   {
      std::string msg = getErrorCodeString(errorCode);
      if(!operation.empty())
      {
         msg += " during " + operation;
      }
      return msg;
   }

   static std::string getDeviceSuggestion(ErrorCode errorCode)
   {
      switch(errorCode)
      {
         case DEVICE_NOT_FOUND:
            return "Check device connection and ensure drivers are installed";
         case DEVICE_CONNECTION_FAILED:
            return "Verify device is in correct mode and try reconnecting";
         case DEVICE_TIMEOUT:
            return "Check device responsiveness and try increasing timeout";
         case DEVICE_COMMUNICATION_ERROR:
            return "Verify cable connection and device compatibility";
         default:
            return "";
      }
   }
};

/// Command line parsing related exceptions
class CommandLineException : public Exception
{
public:
   CommandLineException(
      ErrorCode errorCode,
      const std::string& parameter = "",
      const std::string& value = "",
      const std::string& customSuggestion = ""
   )
   : Exception(
        errorCode,
        getCommandLineMessage(errorCode, parameter, value),
        parameter,
        customSuggestion.empty() ? getCommandLineSuggestion(errorCode) : customSuggestion
     )
   {
   }

private:
   static std::string getCommandLineMessage(ErrorCode errorCode, const std::string& parameter, const std::string& value)
   {
      std::string msg = getErrorCodeString(errorCode);
      if(!parameter.empty())
      {
         msg += " for parameter '" + parameter + "'";
         if(!value.empty())
         {
            msg += " with value '" + value + "'";
         }
      }
      return msg;
   }

   static std::string getCommandLineSuggestion(ErrorCode errorCode)
   {
      switch(errorCode)
      {
         case INVALID_COMMAND:
            return "Use --help to see available commands";
         case MISSING_REQUIRED_PARAMETER:
            return "Check command syntax and provide all required parameters";
         case INVALID_PARAMETERS:
            return "Verify parameter format and allowed values";
         case CONFLICTING_VALUE:
            return "Please specify a single value for each parameter";
         default:
            return "";
      }
   }
};

} // namespace Common
} // namespace QC

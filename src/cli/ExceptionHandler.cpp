// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "ExceptionHandler.h"

namespace QC {
namespace Common {

// Initialize static members
ExceptionHandler::LogLevel ExceptionHandler::logLevel = ExceptionHandler::LEVEL_ERROR;
bool ExceptionHandler::verboseMode = false;
bool ExceptionHandler::jsonMode = false;

// Helper function to check if string is JSON
static bool isJsonString(const std::string& str)
{
   if(str.empty()) return false;

   // Trim whitespace
   size_t start = str.find_first_not_of(" \t\n\r");
   if(start == std::string::npos) return false;

   char firstChar = str[start];
   return (firstChar == '{' || firstChar == '[');
}

// Helper function to format JSON string as a table
static std::string formatJsonAsTable(const std::string& jsonStr, const std::string& errorType)
{
   std::ostringstream formatted;

   // Simple JSON parser for key-value pairs
   // Looks for "key": "value" or "key": value patterns
   formatted << "\n";
   formatted << "+-------------------------------------------------------------+\n";

   // Simple header - just "Error Details"
   // Total width: 61 chars, Border: "| " (2) + "|" (1) = 3, Content: 58
   formatted << "| Error Details                                               |\n";
   formatted << "+----------------------+--------------------------------------+\n";

   // Add error code as first row
   formatted << "| " << std::left << std::setw(20) << "error_code" << " | ";
   if(errorType.length() <= 36)
   {
      formatted << std::left << std::setw(36) << errorType << " |\n";
   }
   else
   {
      // Split long error codes
      size_t offset = 0;
      while(offset < errorType.length())
      {
         if(offset > 0)
         {
            formatted << "| " << std::setw(20) << "" << " | ";
         }
         size_t len = std::min(size_t(36), errorType.length() - offset);
         formatted << std::left << std::setw(36) << errorType.substr(offset, len) << " |\n";
         offset += len;
      }
   }

   size_t pos = 0;
   bool hasContent = true; // We already have error code row
   bool firstRow = false;  // Error code was first row

   while(pos < jsonStr.length())
   {
      // Find key
      size_t keyStart = jsonStr.find('"', pos);
      if(keyStart == std::string::npos) break;

      size_t keyEnd = jsonStr.find('"', keyStart + 1);
      if(keyEnd == std::string::npos) break;

      std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);

      // Find colon
      size_t colonPos = jsonStr.find(':', keyEnd);
      if(colonPos == std::string::npos) break;

      // Find value
      size_t valueStart = jsonStr.find_first_not_of(" \t\n\r", colonPos + 1);
      if(valueStart == std::string::npos) break;

      std::string value;
      if(jsonStr[valueStart] == '"')
      {
         // String value
         size_t valueEnd = jsonStr.find('"', valueStart + 1);
         if(valueEnd == std::string::npos) break;
         value = jsonStr.substr(valueStart + 1, valueEnd - valueStart - 1);
         pos = valueEnd + 1;
      }
      else
      {
         // Non-string value (number, boolean, etc.)
         size_t valueEnd = jsonStr.find_first_of(",}]", valueStart);
         if(valueEnd == std::string::npos) valueEnd = jsonStr.length();
         value = jsonStr.substr(valueStart, valueEnd - valueStart);
         // Trim whitespace
         size_t valueRealEnd = value.find_last_not_of(" \t\n\r");
         if(valueRealEnd != std::string::npos)
         {
            value = value.substr(0, valueRealEnd + 1);
         }
         pos = valueEnd;
      }

      // Add divider between rows (but not before first row)
      if(!firstRow)
      {
         formatted << "+----------------------+--------------------------------------+\n";
      }
      firstRow = false;

      // Format as table row
      formatted << "| " << std::left << std::setw(20) << key << " | ";

      // Wrap long values
      if(value.length() <= 36)
      {
         formatted << std::left << std::setw(36) << value << " |\n";
      }
      else
      {
         // Split long values
         size_t offset = 0;
         while(offset < value.length())
         {
            if(offset > 0)
            {
               formatted << "| " << std::setw(20) << "" << " | ";
            }
            size_t len = std::min(size_t(36), value.length() - offset);
            formatted << std::left << std::setw(36) << value.substr(offset, len) << " |\n";
            offset += len;
         }
      }

      hasContent = true;
   }

   if(!hasContent)
   {
      // If parsing failed, just return the original string
      return jsonStr;
   }

   formatted << "+----------------------+--------------------------------------+\n";
   return formatted.str();
}

// Helper function for THROW_FROM_ERROR_TYPE macro
void throwFromErrorTypeImpl(const QC::ErrorType& errorType, const std::string& operation)
{
   if(errorType.errorCode == QC::ErrorCode::DEVICE_NO_ERROR)
   {
      return; // No error, don't throw
   }

   // Map QC::ErrorCode to QC::Common::Exception::ErrorCode
   QC::Common::Exception::ErrorCode exceptionCode = QC::Common::Exception::UNKNOWN_ERROR;

   switch(errorType.errorCode)
   {
      case QC::ErrorCode::DEVICE_INVALID_DEVICE_HANDLE:
         exceptionCode = QC::Common::Exception::DEVICE_NOT_FOUND;
         break;
      case QC::ErrorCode::DEVICE_CONNECTION_LOCKED:
         exceptionCode = QC::Common::Exception::DEVICE_CONNECTION_FAILED;
         break;
      case QC::ErrorCode::DEVICE_DISCONNECTED:
         exceptionCode = QC::Common::Exception::DEVICE_DISCONNECTED;
         break;
      case QC::ErrorCode::DEVICE_PROTOCOL_INVALID:
      case QC::ErrorCode::DEVICE_PROTOCOL_DISCONNECTED:
      case QC::ErrorCode::DEVICE_PROTOCOL_UNRESPONSIVE:
      case QC::ErrorCode::DEVICE_RESPONSE_ERROR:
         exceptionCode = QC::Common::Exception::DEVICE_COMMUNICATION_ERROR;
         break;
      case QC::ErrorCode::DEVICE_TIMEOUT:
         exceptionCode = QC::Common::Exception::DEVICE_TIMEOUT;
         break;
      case QC::ErrorCode::DEVICE_INVALID_PARAMETERS:
         exceptionCode = QC::Common::Exception::INVALID_PARAMETERS;
         break;
      case QC::ErrorCode::DEVICE_PERMISSIONS_ERROR:
         exceptionCode = QC::Common::Exception::PERMISSION_DENIED;
         break;
      case QC::ErrorCode::DEVICE_SERVICE_NOT_INITIALIZED:
      case QC::ErrorCode::DEVICE_SERVICE_ALREADY_INITIALIZED:
         exceptionCode = QC::Common::Exception::INITIALIZATION_FAILED;
         break;
      case QC::ErrorCode::DEVICE_TX_CANCELLED:
         exceptionCode = QC::Common::Exception::OPERATION_CANCELLED;
         break;
      default:
         exceptionCode = QC::Common::Exception::INTERNAL_ERROR;
         break;
   }

   // Get error code name for display
   std::string errorCodeName;
   switch(errorType.errorCode)
   {
      case QC::ErrorCode::DEVICE_INVALID_DEVICE_HANDLE:
         errorCodeName = "DEVICE_INVALID_DEVICE_HANDLE";
         break;
      case QC::ErrorCode::DEVICE_CONNECTION_LOCKED:
         errorCodeName = "DEVICE_CONNECTION_LOCKED";
         break;
      case QC::ErrorCode::DEVICE_DISCONNECTED:
         errorCodeName = "DEVICE_DISCONNECTED";
         break;
      case QC::ErrorCode::DEVICE_PROTOCOL_INVALID:
         errorCodeName = "DEVICE_PROTOCOL_INVALID";
         break;
      case QC::ErrorCode::DEVICE_PROTOCOL_DISCONNECTED:
         errorCodeName = "DEVICE_PROTOCOL_DISCONNECTED";
         break;
      case QC::ErrorCode::DEVICE_PROTOCOL_UNRESPONSIVE:
         errorCodeName = "DEVICE_PROTOCOL_UNRESPONSIVE";
         break;
      case QC::ErrorCode::DEVICE_RESPONSE_ERROR:
         errorCodeName = "DEVICE_RESPONSE_ERROR";
         break;
      case QC::ErrorCode::DEVICE_TIMEOUT:
         errorCodeName = "DEVICE_TIMEOUT";
         break;
      case QC::ErrorCode::DEVICE_INVALID_PARAMETERS:
         errorCodeName = "DEVICE_INVALID_PARAMETERS";
         break;
      case QC::ErrorCode::DEVICE_PERMISSIONS_ERROR:
         errorCodeName = "DEVICE_PERMISSIONS_ERROR";
         break;
      case QC::ErrorCode::DEVICE_SERVICE_NOT_INITIALIZED:
         errorCodeName = "DEVICE_SERVICE_NOT_INITIALIZED";
         break;
      case QC::ErrorCode::DEVICE_SERVICE_ALREADY_INITIALIZED:
         errorCodeName = "DEVICE_SERVICE_ALREADY_INITIALIZED";
         break;
      case QC::ErrorCode::DEVICE_TX_CANCELLED:
         errorCodeName = "DEVICE_TX_CANCELLED";
         break;
      default:
         errorCodeName = "UNKNOWN_ERROR";
         break;
   }

   // Format error string - if it's JSON, format as table with error type
   std::string formattedErrorString = errorType.errorString;
   if(isJsonString(errorType.errorString))
   {
      formattedErrorString = formatJsonAsTable(errorType.errorString, errorCodeName);
   }

   // Throw DeviceException with the error information from DLL
   throw QC::Common::DeviceException(
      exceptionCode,
      "", // deviceInfo - empty, will use context
      operation,
      formattedErrorString // Use formatted error string (table
                           // if JSON, original otherwise)
   );
}

} // namespace Common
} // namespace QC

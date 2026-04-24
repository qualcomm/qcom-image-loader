// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "platform/Exception.h"

namespace Device {

// --------------------------------------------------------------------------
// Exception
//
/// Includes an error code regarding the exception that happened
// --------------------------------------------------------------------------
class Exception : public ToolException
{
public:
   static std::string getErrorJson(const std::string& error, const std::string& suggestion, const std::string& poc)
   {
      return "{ \"error\": \"" + error + "\", \"suggestion\": \"" + suggestion + "\", \"poc\": \"" + poc + "\"}";
   }
   enum ErrorCode
   {
      DEVICE_NO_ERROR = 0,
      DEVICE_UNKNOWN_ERROR,
      DEVICE_INVALID_PARAMETERS,
      DEVICE_PERMISSIONS_ERROR,
      DEVICE_INVALID_DEVICE_HANDLE,
      DEVICE_INVALID_PROTOCOL_HANDLE,
      DEVICE_INVALID_CONNECTION_HANDLE,
      DEVICE_CONNECTION_LOCKED,
      DEVICE_DISCONNECTED,
      DEVICE_PROTOCOL_INVALID,
      DEVICE_PROTOCOL_DISCONNECTED,
      DEVICE_PROTOCOL_UNRESPONSIVE,
      DEVICE_TX_CANCELLED,
      DEVICE_TIMEOUT,
      DEVICE_INVALID_PROCESSOR,
      DEVICE_INVALID_PACKET,
      DEVICE_RESPONSE_ERROR,
      DEVICE_INVALID_LOG_SESSION,
      DEVICE_SERVICE_NOT_INITIALIZED,
      DEVICE_TCP_PORT_FAILURE,
      DEVICE_SERVICE_ALREADY_INITIALIZED,
      DEVICE_LICENSE_ERROR,
      DEVICE_OTP_PROGRAMED,
      DEVICE_DECRYPTION_FAILED,
      DEVICE_SERVICE_LOCKED
      // Note: When adding to this the same values must be added in
      // Exception.h: std::string getErrorCodeString(ErrorCode errcode)
   };

   Exception(
      ErrorCode errorCode,
      const std::string& msg = ToolException::defaultMessage().c_str(),
      const std::string& location = ToolException::defaultLocation().c_str()
   )
   : ToolException(msg, location)
   , m_errorCode(errorCode)
   {
   }

   Exception(const Exception& src)
   : ToolException(src)
   , m_errorCode(src.m_errorCode)
   {
   }

   Exception& operator=(const Device::Exception& src)
   {
      if(this == &src)
      {
         return *this;
      }

      ToolException::operator=(src);
      return *this;
   }
   Exception& operator=(const ToolException& src)
   {
      if(this == &src)
      {
         return *this;
      }

      ToolException::operator=(src);
      return *this;
   }
   Exception& operator=(const std::exception& src)
   {
      if(this == &src)
      {
         return *this;
      }

      ToolException::operator=(src);

      m_errorCode = DEVICE_UNKNOWN_ERROR;
      return *this;
   }

   virtual ~Exception() throw()
   {
   }

   ErrorCode getErrorCode() const throw()
   {
      return m_errorCode;
   }

   std::string getErrorCodeString(ErrorCode errcode) const
   {
      switch(errcode)
      {
         case DEVICE_NO_ERROR:
            return "No error";
         case DEVICE_UNKNOWN_ERROR:
            return "UnKonown error";
         case DEVICE_INVALID_PARAMETERS:
            return "Invalid larameters";
         case DEVICE_PERMISSIONS_ERROR:
            return "Permission error";
         case DEVICE_INVALID_DEVICE_HANDLE:
            return "Invalid devie handle";
         case DEVICE_INVALID_PROTOCOL_HANDLE:
            return "Invalid protocol handle";
         case DEVICE_INVALID_CONNECTION_HANDLE:
            return "Invalid connection handle";
         case DEVICE_CONNECTION_LOCKED:
            return "Connection locked";
         case DEVICE_DISCONNECTED:
            return "Device disconnected";
         case DEVICE_PROTOCOL_INVALID:
            return "Invalid protocol";
         case DEVICE_PROTOCOL_DISCONNECTED:
            return "Protocol disconnected";
         case DEVICE_PROTOCOL_UNRESPONSIVE:
            return "Protocol unresponsive ";
         case DEVICE_TX_CANCELLED:
            return "Tx cancelled";
         case DEVICE_TIMEOUT:
            return "Device timeout";
         case DEVICE_INVALID_PROCESSOR:
            return "Invalid processor";
         case DEVICE_INVALID_PACKET:
            return "Invalid packet";
         case DEVICE_RESPONSE_ERROR:
            return "Responsive error";
         case DEVICE_INVALID_LOG_SESSION:
            return "Invalid log session";
         case DEVICE_SERVICE_NOT_INITIALIZED:
            return "Service not initialized";
         case DEVICE_TCP_PORT_FAILURE:
            return "TCP port failure";
         case DEVICE_SERVICE_ALREADY_INITIALIZED:
            return "Service already initialized";
         case DEVICE_LICENSE_ERROR:
            return "License error";
         case DEVICE_OTP_PROGRAMED:
            return "OTP programed";
         case DEVICE_DECRYPTION_FAILED:
            return "Decryption failed";
         case DEVICE_SERVICE_LOCKED:
            return "Service is locked";
         default:
            return "Unknown error";
      }
   }

private:
   ErrorCode m_errorCode;
};
} // namespace Device

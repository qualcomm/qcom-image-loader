// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <cstdint>
#include <list>
#include <string>

namespace QC {
#pragma pack(push, 1)

struct ErrorCode
{
   enum type
   {
      DEVICE_NO_ERROR = 0,
      DEVICE_UNKNOWN_ERROR = 1,
      DEVICE_INVALID_PARAMETERS = 2,
      DEVICE_PERMISSIONS_ERROR = 3,
      DEVICE_INVALID_DEVICE_HANDLE = 4,
      DEVICE_INVALID_PROTOCOL_HANDLE = 5,
      DEVICE_INVALID_CONNECTION_HANDLE = 6,
      DEVICE_CONNECTION_LOCKED = 7,
      DEVICE_DISCONNECTED = 8,
      DEVICE_PROTOCOL_INVALID = 9,
      DEVICE_PROTOCOL_DISCONNECTED = 10,
      DEVICE_PROTOCOL_UNRESPONSIVE = 11,
      DEVICE_TX_CANCELLED = 12,
      DEVICE_TIMEOUT = 13,
      DEVICE_INVALID_PROCESSOR = 14,
      DEVICE_INVALID_PACKET = 15,
      DEVICE_RESPONSE_ERROR = 16,
      DEVICE_INVALID_LOG_SESSION = 17,
      DEVICE_SERVICE_NOT_INITIALIZED = 18,
      DEVICE_TCP_PORT_FAILURE = 19,
      DEVICE_SERVICE_ALREADY_INITIALIZED = 20,
      DEVICE_LICENSE_ERROR = 21,
      DEVICE_OTP_PROGRAMED = 22,
      DEVICE_DECRYPTION_FAILED = 23,
      DEVICE_SERVICE_LOCKED = 24
   };
};

struct MessageLevel
{
   enum type
   {
      INFO = 0,
      WARNING = 1,
      EXCEPTION = 2,
      CRITICAL = 3
   };
};

enum DeviceMode
{
   DEVICE_MODE_NONE = 0x00000000,
   DEVICE_MODE_SAHARA_DOWNLOAD = 0x00000001,
   DEVICE_MODE_SAHARA_CRASH = 0x00000002,
   DEVICE_MODE_SAHARA_EFS_SYNC = 0x00000004
};

enum ConnectionType
{
   CONNECT_UNKNOWN = -1,
   CONNECT_USB = 0,
   CONNECT_MAX
};

enum ProtocolType
{
   PROT_UNKNOWN = -1,
   PROT_SAHARA,
   PROT_FIREHOSE,
   PROT_MAX
};

enum ProtocolState
{
   STATE_AVAILABLE = 0,
   STATE_DISCONNECTED = 1,
   STATE_UNRESPONSIVE = 2,
   STATE_INITIALIZING = 3
};

enum OpenProp
{
   OPEN_NONE = 0x0000,
   OPEN_READ = 0x0001,
   OPEN_WRITE = 0x0002,
   OPEN_READ_WRITE = 0x003
};

/**
 * @brief bit flags to identify device details
 *
 */
enum DeviceDetail
{
   NO_DEVICE_DETAIL = 0,
   QC_DEVICE = 1 << 0,
   SNAPDRAGON_DEVICE = 1 << 1,
   ADB_LA_DEVICE = 1 << 2,
   ADB_LE_DEVICE = 1 << 3,
   QNX_DEVICE = 1 << 4,
   TROUBLESHOOT_ADB = 1 << 5,
   GOOGLE_DEVICE = 1 << 6,
   ADB_HGY_DEVICE = 1 << 7,
   LOCALHOST_DEVICE = 1 << 8,
   USB_DEVICE = 1 << 9,
};

#pragma pack(pop)

// Structs with std::string or std::list must use default alignment
// to avoid C4315 warning (misaligned 'this' pointer)
struct ErrorType
{
   ErrorCode::type errorCode;
   std::string errorString;
};

struct SocInfo
{
   std::string chipName;
   std::string version;
};

struct ProtocolInfo
{
   uint64_t protocolHandle;
   uint64_t deviceHandle;
   std::string description;
   ProtocolType protocolType;
   ConnectionType connectionType;
   OpenProp connectionStatus;
   OpenProp shareStatus;
   ProtocolState protocolState;
   uint32_t localPort;
   uint32_t remotePort;
   std::string alternateDescription;
};

struct DeviceInfo
{
   uint64_t deviceHandle;
   std::string description;
   std::list<ProtocolInfo> protocols;

   std::string serialNumber;
   std::string adbSerialNumber;
   std::string location;
   std::string vid;
   std::string pid;
   std::string edlChipId;
   std::string devicePhysicalLocation;
   SocInfo socInfo;
};

}; // namespace QC

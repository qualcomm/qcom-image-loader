// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Connection.h"
#include "device/Fwd.h"
#include "device/Impl.h"
#include "protocol/Fwd.h"
#include "report/Fwd.h"
#include "util/AppEvent.h"
#include "util/AppMessage.h"
#include "util/SystemHelper.h"
#include "util/ThreadHelper.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
namespace Device {

static const int64_t NULL_DEVICE_HANDLE = 0;
static const uint64_t USB_DEFAULT_BAUD_RATE = 38400;

static const uint8_t HOST_PROFILING = 1;
static const uint8_t TARGET_OVER_TCP = 2;

static const std::string STRING_PATTERN_QCDM = ".*?QCDM|.*?qcdm";
static const std::string STRING_PATTERN_MHI = "mhi.*?";
enum DeviceMode
{
   DEVICE_MODE_NONE = 0,
   DEVICE_MODE_SAHARA_DOWNLOAD = 1,
   DEVICE_MODE_SAHARA_CRASH = 2,
   DEVICE_MODE_SAHARA_COMMAND = 3,
   DEVICE_MODE_SAHARA_EFS_SYNC = 4
};

// Event index used for issue identification
// Format:
// 0x xx .. .. .. Control bits (Default - 00)
// 0x .. xx .. .. Main category
// 0x .. .. xx .. Subcategory
// 0x .. .. .. xx Index
enum CriticalEventId
{
   EVENT_BASE = 0,

   // Main category - Communication (0x00)
   EVENT_COMMUNICATION_BASE = 0x00000000,
   /// Communication (0x00) subcategory - USB (0x00)
   EVENT_COMMUNICATION_USB_OPEN_FAILURE = 0x00000001,
   EVENT_COMMUNICATION_USB_SEND_FAILURE = 0x00000002,

   // Main category - Device (0x01)
   EVENT_DEVICE_BASE = 0x00010000,

   // Main category - Protocol (0x02)
   EVENT_PROTOCOL_BASE = 0x00020000,
   /// Protocol (0x02) subcategory - QDSS (0x01)
   EVENT_PROTOCOL_QDSS_ATB_DECODING_ERROR = 0x00020101,
   EVENT_PROTOCOL_QDSS_HIGH_MEMORY_DATA_DROP_ERROR = 0x00020102,
   EVENT_PROTOCOL_QDSS_ATB_FLOW_CONTROL_DATA_DROP_ERROR = 0x00020103,

   // Main category - System (0x03)
   EVENT_SYSTEM_BASE = 0x00030000
};

typedef std::map<CriticalEventId, std::string> CriticalEventMap;

// ----------------------------------------------------------------------------
// DeviceEvent
//
/// Event from a device being added or removed
// ----------------------------------------------------------------------------
template <size_t _EventType>
class DeviceEvent : public Util::AsyncEvent
{
   TOOLS_FORBID_COPY(DeviceEvent);

public:
   DeviceEvent(const ImplPtr& pDevice)
   : Util::AsyncEvent()
   , m_pDevice(pDevice)
   {
   }

   ImplPtr getDevice() const
   {
      return m_pDevice;
   }

private:
   ImplPtr m_pDevice; ///< Device the event happend to
};

typedef DeviceEvent<0> DeviceConnectEvent;
typedef DeviceEvent<1> DeviceDisconnectEvent;
typedef DeviceEvent<2> DeviceModeChangeEvent;

// ----------------------------------------------------------------------------
// ProtocolEvent
//
/// Event from a protocol being added or removed
// ----------------------------------------------------------------------------
template <size_t _EventType>
class ProtocolEvent : public Util::AsyncEvent
{
   TOOLS_FORBID_COPY(ProtocolEvent);

public:
   ProtocolEvent(const ImplPtr& pDevice, const Protocol::BasePtr& pProtocol)
   : Util::AsyncEvent()
   , m_pDevice(pDevice)
   , m_pProtocol(pProtocol)
   {
   }

   ImplPtr getDevice() const
   {
      return m_pDevice;
   }

   Protocol::BasePtr getProtocol() const
   {
      return m_pProtocol;
   }

private:
   ImplPtr m_pDevice;             ///< Device the event happend to
   Protocol::BasePtr m_pProtocol; ///< Protocol the event happened to
};


// ----------------------------------------------------------------------------
// GlobalServiceEvent
//
/// service Event
// ----------------------------------------------------------------------------
template <size_t _EventType>
class GlobalServiceEvent : public Util::AsyncEvent
{
   TOOLS_FORBID_COPY(GlobalServiceEvent);

public:
   GlobalServiceEvent(
      const std::string& serviceName,
      int64_t deviceHandle,
      int64_t protocolHandle,
      /*EventId*/ int64_t eventId,
      const std::string& eventDescription
   )
   : Util::AsyncEvent()
   , m_serviceName(serviceName)
   , m_deviceHandle(deviceHandle)
   , m_protocolHandle(protocolHandle)
   , m_eventId(eventId)
   , m_eventDescription(eventDescription)
   {
   }

   const std::string& getServiceName() const
   {
      return m_serviceName;
   }

   int64_t getDeviceHanle() const
   {
      return m_deviceHandle;
   }

   int64_t getProtocolHandle() const
   {
      return m_protocolHandle;
   }

   int64_t getEventId() const
   {
      return m_eventId;
   }

   std::string getEventDescription() const
   {
      return m_eventDescription;
   }

private:
   std::string m_serviceName;      ///< Service that invoked the event
   int64_t m_deviceHandle;         ///< Device Handle
   int64_t m_protocolHandle;       ///< Protocol Handle
   int64_t m_eventId;              ///< Event specific ID
   std::string m_eventDescription; ///< Optional description
};

typedef GlobalServiceEvent<0> ImageManagementServiceEvent;


// ----------------------------------------------------------------------------
// CriticalEvent
//
/// Event for critical message
// ----------------------------------------------------------------------------
class CriticalEvent : public Util::AsyncEvent
{
   TOOLS_FORBID_COPY(CriticalEvent);

public:
   CriticalEvent(CriticalEventId id, const std::string& location, const std::string& description)
   : Util::AsyncEvent()
   , m_location(location)
   , m_id(id)
   , m_description(description)
   {
   }

   Util::Message::Level getLevel() const
   {
      return Util::Message::Level::CRITICAL;
   }

   std::string getLocation() const
   {
      return m_location;
   }

   CriticalEventId getId() const
   {
      return m_id;
   }

   std::string getDescription() const
   {
      return m_description;
   }

private:
   std::string m_location;
   CriticalEventId m_id;
   std::string m_description;
};

// ----------------------------------------------------------------------------
// AccessibleFile
//
/// Wrapper class created for writting file to a network location, in order to
/// get around access permission problem when system mode access is not allowed.
///
/// Note: simple open/write/close usage only
// ----------------------------------------------------------------------------
class AccessibleFile
{
   TOOLS_FORBID_COPY(AccessibleFile);

public:
   AccessibleFile()
   : m_path()
   , m_tempPath()
   {
   }
   virtual void open(const std::filesystem::path& path, std::ios::openmode mode = std::ios::out | std::ios::binary);

   void close() throw();

   // Forward common stream operations
   std::fstream& stream()
   {
      return m_stream;
   }
   const std::fstream& stream() const
   {
      return m_stream;
   }

   bool is_open() const
   {
      return m_stream.is_open();
   }

   template <typename T>
   void write(const T* data, std::streamsize count)
   {
      m_stream.write(reinterpret_cast<const char*>(data), count);
   }

   template <typename T>
   void read(T* data, std::streamsize count)
   {
      m_stream.read(reinterpret_cast<char*>(data), count);
   }

   void flush()
   {
      m_stream.flush();
   }
   bool good() const
   {
      return m_stream.good();
   }
   bool fail() const
   {
      return m_stream.fail();
   }
   bool eof() const
   {
      return m_stream.eof();
   }

   // Seek operations
   void seek(std::streamoff offset, std::ios::seekdir dir = std::ios::beg)
   {
      m_stream.seekg(offset, dir);
      m_stream.seekp(offset, dir);
   }

   void seek(std::streamoff offset)
   {
      seek(offset, std::ios::beg);
   }

   std::fstream m_stream;

private:
   std::filesystem::path m_path;
   std::filesystem::path m_tempPath;
};
typedef std::shared_ptr<AccessibleFile> AccessibleFilePtr;

typedef ProtocolEvent<0> ProtocolAddedEvent;
typedef ProtocolEvent<1> ProtocolRemovedEvent;

class DeviceManagerHelper;

// ----------------------------------------------------------------------------
// Manager
//
/// Holds all available device connections
// ----------------------------------------------------------------------------
class Manager
: public Util::EventPublisher
, public Util::AsyncEventPublisher
, public std::enable_shared_from_this<Manager>
{
   TOOLS_FORBID_COPY(Manager);

public:
   struct ConnectionInfo
   {
      ConnectionPtr m_pConnection;
      size_t m_openCount;
      int32_t m_clientId;
   };

   typedef std::unordered_map<DeviceHandle, ImplPtr> DeviceList;
   typedef std::unordered_map<Protocol::Handle, Protocol::BasePtr> ProtocolList;

   typedef std::unordered_map<int64_t, ConnectionInfo> ConnectionList;
   typedef std::set<std::filesystem::path> FilePathSet;
   typedef std::unordered_map<int32_t, FilePathSet> ClientFilePathSetMap;
   static ManagerPtr getInstance();

   friend class Util::SharedPointer<Manager>;
   virtual ~Manager();

   void setApplicationInfo(
      const std::filesystem::path& tempFolder,
      const std::string& appName,
      uint16_t appMajorVer,
      uint16_t appMinorVer,
      const std::string& appBuildId,
      const std::filesystem::path& programDataFolder
   );

   void shutDown();

   uint16_t getAppMajorVersion();
   uint16_t getAppMinorVersion();
   std::string getAppBuildId();

   std::filesystem::path getProgramDataDirectory();
   std::filesystem::path getTempDirectory();

   DeviceList getDeviceList() const;

   ImplPtr getDeviceByHandle(DeviceHandle handle);
   Protocol::BasePtr getProtocolByHandle(Protocol::Handle handle);
   ProtocolType getProtocolType(Protocol::Handle handle);
   ProtocolType getProtocolType(const Protocol::BasePtr& pProtocol);
   ConnectionPtr openConnection(
      const Protocol::BasePtr& pProtocol,
      const Protocol::Base::Access& access,
      const Protocol::Base::Share& share,
      int32_t clientId,
      std::shared_ptr<Util::IMessagePublisher> pPublisher,
      bool& bNewConnection,
      uint16_t contextHandle = 0
   );

   void closeConnection(const ConnectionPtr& pConnection);
   bool isProtocolUsedByConnection(const Device::Protocol::BasePtr& pProtocol);

   void sendImageManagementServiceEvent(
      const std::string& serviceName,
      int64_t deviceHandle,
      int64_t protocolHandle,
      int64_t eventId,
      const std::string& description
   );
   void reportCriticalEvent(const CriticalEventId id, const std::string& location);

   void onProtocolStateChange(Protocol::StateChangeEvent* pEvent);

   void onCriticalEvent(Device::CriticalEvent* pEvent);

   std::filesystem::path getAccessiblePath(
      const std::filesystem::path& filePath,
      const std::filesystem::path& destFolder = std::filesystem::path(),
      const bool bIgnoreDirectories = false,
      const int32_t clientId = Protocol::Base::NO_CLIENT_ID,
      const bool bSearchCache = false
   );
   void saveFile(
      const std::filesystem::path& sourcePath,
      const std::filesystem::path& destinationPath,
      const bool bKeepSourceFile = false
   );

   void mhiForceEdl(int32_t instance, const std::string& programmerPath);

   bool isMemoryUsageCritical();
   bool isCpuUsageCritical();
   void removeConnectedDevice(const DeviceHandle deviceHandle);
   void addDevice(const ImplPtr& pDevice);

protected:
   void waitForInitialDeviceList() const;
   Manager();
   void initialize();
   void finalize();

private:
   friend class DeviceManagerHelper;

   typedef std::map<std::filesystem::path, std::filesystem::path> AccessibleFileMap;

   std::filesystem::path m_tempLogFolder;  ///< Where to put intermediate log files
   std::string m_appName;                  ///< Used when creating some log files
   uint16_t m_appMajorVer;                 ///< Used when creating some log files
   uint16_t m_appMinorVer;                 ///< Used when creating some log files
   std::string m_appBuildId;               ///< Used when creating some log files
   std::filesystem::path m_programDataDir; ///< Folder with persistent app data

   DeviceList m_devices;             ///< All connected devices
   DeviceList m_disconnectedDevices; ///< Devices no longer active
   ConnectionList m_connections;     ///< All open connections

   DeviceHandle m_nextDeviceHandle; ///< Next available device handle

   mutable std::recursive_mutex m_deviceManagerMutex;

   Report::StatusManagerPtr m_pStatusReportManager;

   std::shared_ptr<DeviceManagerHelper> m_pDiscoveryWork;
   std::shared_ptr<Util::StdThreadWrapper> m_pDiscoveryThread;

   ClientFilePathSetMap m_clientFilePath; ///< temp filePath list for each client
   mutable std::recursive_mutex m_clientFilePathMutex;

   mutable std::recursive_mutex m_cachedAccessibleFilesMutex;
   AccessibleFileMap m_cachedAccessibleFiles;
};

} // namespace Device

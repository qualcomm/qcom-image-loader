// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Fwd.h"
#include "protocol/Fwd.h"

#include <list>
#include <mutex>
#include <string>

// --------------------------------------------------------------------------
// TOOLS_FORBID_COPY
//
/// Prevent instances of a class from being copyable
// --------------------------------------------------------------------------
#define TOOLS_FORBID_COPY(Class)                                                                                       \
private:                                                                                                               \
   Class(const Class&) = delete;                                                                                       \
   Class& operator=(const Class&) = delete

namespace Device {

// ----------------------------------------------------------------------------
// Impl
//
/// Contains all the protocols for communicating with a device
// ----------------------------------------------------------------------------
class Impl : public std::enable_shared_from_this<Impl>
{
   TOOLS_FORBID_COPY(Impl);

public:
   typedef std::list<Protocol::BasePtr> ProtocolList;
   static const int64_t INVALID_DEVICE_HANDLE = -1;

   Impl(DeviceHandle handle, const std::string& description, const std::string& uniqueIdentifier);
   virtual ~Impl();

   std::string getHwId() const;
   void setHwId(const std::string& hwId);
   std::string getDescription() const;
   void setDescription(const std::string& description);
   std::string getUniqueIdentifier() const;
   void setUniqueIdentifier(const std::string& uniqueIdentifier);
   std::string getDevicePath() const;
   void setDevicePath(const std::string& uniqueIdentifier);
   std::string getSerialNumberMsm() const;
   void setSerialNumberMsm(const std::string& serialNumberMsm);
   std::string getSerialNumberAdb() const;
   void setSerialNumberAdb(const std::string& serialNumberAdb);
   std::string getSocVersion() const;
   void setSocVersion(const std::string& socVer);

   std::string getVid() const;
   void setVid(const std::string& vid);
   std::string getPid() const;
   void setPid(const std::string& pid);
   std::string getEdlChipId() const;
   void setEdlChipId(const std::string& edlChipId);
   std::string getDevicePhysicalLocation() const;
   void setDevicePhysicalLocation(const std::string& edlChipId);
   DeviceHandle getHandle() const;

   size_t getProtocolCount() const;
   ProtocolList getProtocolList() const;
   ProtocolList getUnavailableProtocolList() const;

   bool
   searchAvailableProtocolByDescriptionAndIdentifier(const std::string& description, const std::string& identifier);
   bool searchUnavailableProtocolByDescription(const std::string& description);

   void addProtocol(const Protocol::BasePtr& pProtocol);
   void removeProtocol(const Protocol::BasePtr& pProtocol);

   void moveProtocolIn(const Protocol::BasePtr& pProtocol, bool isConnected);
   void moveProtocolOut(const Protocol::BasePtr& pProtocol, bool isConnected);

private:
   friend class Manager;
   friend class DeviceManagerHelper;

   DeviceHandle m_handle;                      ///< Handle used to reference this device
   std::string m_description;                  ///< Describes the device
   std::string m_uniqueIdentifier;             ///< For grouping
   std::string m_devicePath;                   ///< For grouping
   std::string m_serialNumberMsm;              ///< For additional identification
   std::string m_serialNumberAdb;              ///< For additional identification
   std::string m_socVersion;                   ///< For additional identification
   std::string m_vid;                          ///< For additional identification
   std::string m_pid;                          ///< For additional identification
   std::string m_edlChipId;                    ///< For additional identification
   std::string m_devicePhysicalLocation;       ///< For additional identification
   ProtocolList m_protocols;                   ///< All available connections
   ProtocolList m_unavailableProtocols;        ///< Connections that disconnected
   Protocol::Handle m_nextProtocolHandle;      ///< Next avaialble handle
   mutable std::recursive_mutex m_mutex;

   std::string m_hwId;                      ///< Hardware Id of the device
};

} // namespace Device

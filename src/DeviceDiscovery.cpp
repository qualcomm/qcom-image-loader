// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "DeviceDiscovery.h"

#include "callback/ClientCallbackHandler.h"
#include "callback/Fwd.h"
#include "Definitions.h"
#include "device/Fwd.h"
#include "device/Impl.h"
#include "device/Manager.h"
#include "Globals.h"
#include "tracker/FunctionTracker.h"
#include "util/SystemHelper.h"



namespace QC {

// ----------------------------------------------------------------------------
// getProtocolInfo
//
/// @returns A set of protocol information regarding a given protocol
// ----------------------------------------------------------------------------
ProtocolInfo getProtocolInfo(Device::Protocol::Handle protocolHandle)
{
   Device::Protocol::BasePtr pProtocol = Device::Manager::getInstance()->getProtocolByHandle(protocolHandle);

   ProtocolInfo info;
   info.protocolHandle = pProtocol->getHandle();
   info.deviceHandle = pProtocol->getDevice()->getHandle();
   info.description = pProtocol->getDescription().c_str();

   info.protocolType =
      static_cast<QC::ProtocolType>(Device::Manager::getInstance()->getProtocolType(pProtocol->getHandle()));

   return info;
}

// ----------------------------------------------------------------------------
// fillProtocols
//
/// @returns A set of protocol information regarding the given device
// ----------------------------------------------------------------------------
void fillProtocols(const Device::ImplPtr& pDevice, std::list<QC::ProtocolInfo>& _return)
{
   Device::Impl::ProtocolList protocols = pDevice->getProtocolList();
   Device::Impl::ProtocolList::const_iterator it = protocols.begin();
   Device::Impl::ProtocolList::const_iterator end = protocols.end();
   for(; it != end; ++it)
   {
      Device::Protocol::BasePtr pProtocol = *it;

      _return.push_back(getProtocolInfo(pProtocol->getHandle()));
   }
}

// ----------------------------------------------------------------------------
// getDeviceInfo
//
/// @returns A RPC DeviceInfo object for the given device
// ----------------------------------------------------------------------------

DeviceInfo getDeviceInfo(const Device::ImplPtr& pDevice)
{
   DeviceInfo info;

   info.deviceHandle = pDevice->getHandle();
   info.description = pDevice->getDescription().c_str();
   fillProtocols(pDevice, info.protocols);
   info.location = pDevice->getUniqueIdentifier().c_str();

   info.serialNumber = pDevice->getSerialNumberMsm().c_str();
   info.adbSerialNumber = pDevice->getSerialNumberAdb().c_str();

   info.vid = pDevice->getVid().c_str();
   info.pid = pDevice->getPid().c_str();

   info.edlChipId = pDevice->getEdlChipId().c_str();
   info.devicePhysicalLocation = pDevice->getDevicePhysicalLocation().c_str();
   info.socInfo.version = pDevice->getSocVersion().c_str();
   info.socInfo.chipName = pDevice->getSocVersion().c_str();
   return info;
}

// ----------------------------------------------------------------------------
// DeviceDiscovery
//
/// DeviceDiscovery constructor
// ----------------------------------------------------------------------------
DeviceDiscovery::DeviceDiscovery()
{
}

// ----------------------------------------------------------------------------
// ~DeviceDiscovery
//
/// DeviceDiscovery destructor
// ----------------------------------------------------------------------------
DeviceDiscovery::~DeviceDiscovery()
{
   FLOG_INFO("DeviceDiscovery::~DeviceDiscovery(): Entered.");
}
// ----------------------------------------------------------------------------
// getDLLVersion
//
/// Gets the DLL Version
// ----------------------------------------------------------------------------
std::string DeviceDiscovery::getDLLVersion()
{
   return QC::getDLLVersion();
}
// ----------------------------------------------------------------------------
// setDeviceConnectedCallback
//
/// Sets the callback function for device connected events
// ----------------------------------------------------------------------------
void DeviceDiscovery::setDeviceConnectedCallback(const DeviceConnectedCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setDeviceConnectedCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setDeviceConnectedCallback(callback);
   FLOG_INFO("DeviceDiscovery::setDeviceConnectedCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// setMessageCallback
//
/// Sets the callback function for giving a message
// ----------------------------------------------------------------------------
void DeviceDiscovery::setMessageCallback(const MessageCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setMessageCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setMessageCallback(callback);
   FLOG_INFO("DeviceDiscovery::setMessageCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// setDeviceDisconnectedCallback
//
/// Sets the callback function for device disconnected events
// ----------------------------------------------------------------------------
void DeviceDiscovery::setDeviceDisconnectedCallback(const DeviceConnectedCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setDeviceDisconnectedCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setDeviceDisconnectedCallback(callback);
   FLOG_INFO("DeviceDiscovery::setDeviceDisconnectedCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// setProtocolAddedCallback
//
/// Sets the callback function for protocol added events
// ----------------------------------------------------------------------------
void DeviceDiscovery::setProtocolAddedCallback(const ProtocolConnectedCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setProtocolAddedCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setProtocolAddedCallback(callback);
   FLOG_INFO("DeviceDiscovery::setProtocolAddedCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// setProtocolRemovedCallback
//
/// Sets the callback function for protocol removed events
// ----------------------------------------------------------------------------
void DeviceDiscovery::setProtocolRemovedCallback(const ProtocolConnectedCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setProtocolRemovedCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setProtocolRemovedCallback(callback);
   FLOG_INFO("DeviceDiscovery::setProtocolRemovedCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// setProtocolStateChangeCallback
//
/// Sets the callback function for protocol state change events
// ----------------------------------------------------------------------------
void DeviceDiscovery::setProtocolStateChangeCallback(const ProtocolStateCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setProtocolStateChangeCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setProtocolStateChangeCallback(callback);
   FLOG_INFO("DeviceDiscovery::setProtocolStateChangeCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// setDeviceModeChangeCallback
//
/// Sets the callback function for device mode change events
// ----------------------------------------------------------------------------
void DeviceDiscovery::setDeviceModeChangeCallback(const DeviceModeCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setDeviceModeChangeCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setDeviceModeChangeCallback(callback);
   FLOG_INFO("DeviceDiscovery::setDeviceModeChangeCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// setServiceEventCallback
//
/// Sets the callback function for service events
// ----------------------------------------------------------------------------
void DeviceDiscovery::setServiceEventCallback(const ServiceEventCallback& callback)
{
   FLOG_INFO("DeviceDiscovery::setServiceEventCallback() : Entered.");
   ClientCallbackHandler::getInstance()->setServiceEventCallback(callback);
   FLOG_INFO("DeviceDiscovery::setServiceEventCallback() : Exited.");
}

// ----------------------------------------------------------------------------
// startMonitoring
//
/// Allocate resources and start monitoring.
// ----------------------------------------------------------------------------
void DeviceDiscovery::startMonitoring()
{
   Device::Manager::getInstance()->setApplicationInfo(
      QC::getTempDirectory(),
      QC::getName(),
      QC::getAppMajorVersion(),
      QC::getAppMinorVersion(),
      QC::getAppBuildId(),
      QC::getAllUsersDirectory()
   );
}

// ----------------------------------------------------------------------------
// stopMonitoring
//
/// clean up the resources and stop monitoring
// ----------------------------------------------------------------------------
void DeviceDiscovery::stopMonitoring()
{
   TOOLS_IGNORE_EXCEPTIONS(Device::Manager::getInstance()->shutDown());
}

// ----------------------------------------------------------------------------
// getDeviceList
//
/// @returns Return the list of devcies detected
// ----------------------------------------------------------------------------
std::list<DeviceInfo> DeviceDiscovery::getDeviceList()
{
   FLOG_INFO("DeviceDiscovery::getDeviceList() : Entered.");

   std::list<DeviceInfo> deviceLists;

   {
      Device::Manager::DeviceList devices = Device::Manager::getInstance()->getDeviceList();
      Device::Manager::DeviceList::const_iterator it = devices.begin();
      Device::Manager::DeviceList::const_iterator end = devices.end();
      for(; it != end; ++it)
      {
         deviceLists.push_back(getDeviceInfo(it->second));
      }
   }

   FLOG_INFO("DeviceDiscovery::getDeviceList() : Exited.");
   return deviceLists;
}

// ----------------------------------------------------------------------------
// getProtocolList
//
/// @returns Return the list of protocols for a device detected
// ----------------------------------------------------------------------------
std::list<ProtocolInfo> DeviceDiscovery::getProtocolList(const int64_t deviceHandle)
{
   std::list<QC::ProtocolInfo> protocolList;
   FLOG_INFO("DeviceDiscovery::getProtocolList() : Entered.");

   {
      Device::ImplPtr pDevice = Device::Manager::getInstance()->getDeviceByHandle(deviceHandle);

      fillProtocols(pDevice, protocolList);
   }
   // DEVICE_RPC_CATCH;
   FLOG_INFO("DeviceDiscovery::getProtocolList() : Exited.");

   return protocolList;
}

// ----------------------------------------------------------------------------
// getDeviceDetails
//
/// @returns Return the bit mask of DeviceDetail for a device detected
// ----------------------------------------------------------------------------
uint64_t DeviceDiscovery::getDeviceDetails(const int64_t deviceHandle)
{
   (void)deviceHandle; // Suppress unused parameter warning
   FLOG_INFO("DeviceDiscovery::getDeviceDetails() : Entered.");
   uint64_t details = QC::DeviceDetail::NO_DEVICE_DETAIL;

   FLOG_INFO("DeviceDiscovery::getDeviceDetails() : Exited.");

   return details;
}
} // namespace QC

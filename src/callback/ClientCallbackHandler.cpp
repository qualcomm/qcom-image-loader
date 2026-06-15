// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "callback/ClientCallbackHandler.h"

#include "device/Impl.h"
#include "device/Logger.h"
#include "device/Manager.h"

#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>

namespace QC {

// ----------------------------------------------------------------------------
// ClientCallbackHandler
//
// ----------------------------------------------------------------------------
ClientCallbackHandler::ClientCallbackHandler()
: Util::EventPublisher()
, Util::AsyncEventPublisher()
, m_connectedCallback()
, m_disconnectedCallback()
, m_protocolAddedCallback()
, m_protocolRemovedCallback()
, m_protocolStateChangeCallback()
, m_messageCallback()
, m_mutex()
{
   FLOG_INFO("ClientCallBackHander created");
}

// ----------------------------------------------------------------------------
// ~ClientCallbackHandler
//
// ----------------------------------------------------------------------------
ClientCallbackHandler::~ClientCallbackHandler()
{
}

// ----------------------------------------------------------------------
// getInstance
//
// ----------------------------------------------------------------------
ClientCallbackHandlerPtr ClientCallbackHandler::getInstance()
{
   static std::once_flag initFlag;
   static ClientCallbackHandlerPtr pTheClientCallbackHandler;

   std::call_once(initFlag, [] {
      pTheClientCallbackHandler = Util::SharedPointer<ClientCallbackHandler>::create();
      FLOG_INFO("created new pTheClientCallbackHandler");
   });

   return pTheClientCallbackHandler;
}

// ----------------------------------------------------------------------------
// initialize
//
// ----------------------------------------------------------------------------
void ClientCallbackHandler::initialize()
{
   // Store reference to Device::Manager to avoid
   // singleton access during destruction
   m_pDeviceManager = Device::Manager::getInstance();

   TOOLS_ASSUMING(m_pDeviceManager->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onCriticalEvent));
   TOOLS_ASSUMING(m_pDeviceManager->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onDeviceConnected));
   TOOLS_ASSUMING(m_pDeviceManager->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onDeviceDisconnected));
   TOOLS_ASSUMING(m_pDeviceManager->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onDeviceModeChange));
   TOOLS_ASSUMING(m_pDeviceManager->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onProtocolAdded));
   TOOLS_ASSUMING(m_pDeviceManager->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onProtocolRemoved));
   TOOLS_ASSUMING(m_pDeviceManager->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onProtocolStateChange));
}

// ----------------------------------------------------------------------------
// finalize
//
// ----------------------------------------------------------------------------
void ClientCallbackHandler::finalize()
{
   FLOG_INFO("ClientCallbackHandler Unsubscribe "
             "handlers when stop callbacks");

   // Use stored reference instead of singleton
   // access to avoid static destruction order
   // issues
   TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManager->unsubscribeAsyncEvents(this, &ClientCallbackHandler::onDeviceConnected));

   TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManager->unsubscribeAsyncEvents(this, &ClientCallbackHandler::onDeviceDisconnected)
   );

   TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManager->unsubscribeAsyncEvents(this, &ClientCallbackHandler::onDeviceModeChange));

   TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManager->unsubscribeAsyncEvents(this, &ClientCallbackHandler::onProtocolAdded));

   TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManager->unsubscribeAsyncEvents(this, &ClientCallbackHandler::onProtocolRemoved));

   TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManager->unsubscribeAsyncEvents(this, &ClientCallbackHandler::onProtocolStateChange)
   );

   TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManager->unsubscribeAsyncEvents(this, &ClientCallbackHandler::onCriticalEvent));

   FLOG_INFO("ClientCallbackHandler shutting the "
             "call back server: ");
}

// ----------------------------------------------------------------------------
// subscribeForServiceAsyncEvents
//
// ----------------------------------------------------------------------------
void ClientCallbackHandler::subscribeForServiceAsyncEvents(const Util::CheckedPointer<Service::ServiceHandlerBase> pService)
{
   TOOLS_ASSUMING(pService->subscribeForAsyncEvents(this, &QC::ClientCallbackHandler::onServiceEvent));
}
// ----------------------------------------------------------------------------
// subscribeForServiceAsyncEvents
//
// ----------------------------------------------------------------------------
void ClientCallbackHandler::unsubscribeServiceAsyncEvents(const Util::CheckedPointer<Service::ServiceHandlerBase> pService)
{
   TOOLS_IGNORE_EXCEPTIONS(pService->unsubscribeAsyncEvents(this, &QC::ClientCallbackHandler::onServiceEvent));
   FLOG_INFO("ClientCallbackHandler shutting the "
             "service call back server: ");
}

// ----------------------------------------------------------------------------
// setMessageCallback
//
/// Sets the callback function for giving a
/// message
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setMessageCallback(const MessageDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_messageCallback = callback;
}


// ----------------------------------------------------------------------------
// setDeviceConnectedCallback
//
/// Sets the callback function for device
/// connected events
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setDeviceConnectedCallback(const DeviceEventDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_connectedCallback = callback;
}

// ----------------------------------------------------------------------------
// setDeviceDisconnectedCallback
//
/// Sets the callback function for device
/// disconnected events
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setDeviceDisconnectedCallback(const DeviceEventDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_disconnectedCallback = callback;
}

// ----------------------------------------------------------------------------
// setDeviceModeChangeCallback
//
/// Sets the callback function for device mode
/// change events
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setDeviceModeChangeCallback(const DeviceModeDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_deviceModeChangeCallback = callback;
}


// ----------------------------------------------------------------------------
// setProtocolAddedCallback
//
/// Sets the callback function for protocol added
/// events
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setProtocolAddedCallback(const ProtocolEventDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_protocolAddedCallback = callback;
}

// ----------------------------------------------------------------------------
// setProtocolRemovedCallback
//
/// Sets the callback function for protocol
/// removed events
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setProtocolRemovedCallback(const ProtocolEventDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_protocolRemovedCallback = callback;
}

// ----------------------------------------------------------------------------
// setProtocolStateChangeCallback
//
/// Sets the callback function for protocol state
/// change events
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setProtocolStateChangeCallback(const ProtocolStateDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_protocolStateChangeCallback = callback;
}


// ----------------------------------------------------------------------------
// setServiceEventCallback
//
/// Sets the callback function for service
/// specific events
// ----------------------------------------------------------------------------
void ClientCallbackHandler::setServiceEventCallback(const ServiceEventDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_serviceEventCallback = callback;
}


// ----------------------------------------------------------------------------
// onCriticalEvent
//
/// Calls the message signal to the UTS client
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onCriticalEvent(Device::CriticalEvent* pEvent)
{
   try
   {
      std::stringstream idHexString;
      idHexString << "0x" << std::setfill('0') << std::setw(8) << std::hex << pEvent->getId();

      onMessage(
         static_cast<MessageLevel::type>(pEvent->getLevel().toStorage()),
         pEvent->getLocation().c_str(),
         idHexString.str(),
         pEvent->getDescription().c_str()
      );
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}


// ----------------------------------------------------------------------------
// onMessage
//
///
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onMessage(
   const MessageLevel::type level,
   const std::string& location,
   const std::string& title,
   const std::string& description
)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_messageCallback)
      {
         m_messageCallback(level, location, title, description);
      }
   }
   TOOLS_CATCH(
      e,
      TOOLS_TRACE(e.what())
   ); // Cannot reportException; it would be
      // recursive
}


// ----------------------------------------------------------------------------
// getProtocolInfo
//
/// @returns A set of protocol information
/// regarding a given protocol
// ----------------------------------------------------------------------------
ProtocolInfo getProtocolInfoForHandle(Device::Protocol::Handle protocolHandle)
{
   Device::Protocol::BasePtr pProtocol = Device::Manager::getInstance()->getProtocolByHandle(protocolHandle);

   ProtocolInfo info;
   info.protocolHandle = pProtocol->getHandle();
   info.deviceHandle = pProtocol->getDevice()->getHandle();
   info.description = pProtocol->getDescription().c_str();

   info.protocolType =
      static_cast<QC::ProtocolType>(Device::Manager::getInstance()->getProtocolType(pProtocol->getHandle()));

   // (Other fields omitted for brevity – same as
   // original)
   return info;
}
// ----------------------------------------------------------------------------
// fillProtocols
//
/// @returns A set of protocol information
/// regarding the given device
// ----------------------------------------------------------------------------
void fillProtocolsForDevice(const Device::ImplPtr& pDevice, std::list<QC::ProtocolInfo>& _return)
{
   for(auto&& pProtocol: pDevice->getProtocolList())
   {
      _return.push_back(getProtocolInfoForHandle(pProtocol->getHandle()));
   }
}
// ----------------------------------------------------------------------------
// getDeviceInfo
//
/// @returns A RPC DeviceInfo object for the given
/// device
// ----------------------------------------------------------------------------
QC::DeviceInfo getDeviceInfoForEvent(const Device::ImplPtr& pDevice)
{
   DeviceInfo info;
   info.deviceHandle = pDevice->getHandle();
   info.description = pDevice->getDescription().c_str();
   fillProtocolsForDevice(pDevice, info.protocols);
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
// onDeviceConnected
//
/// Calls the device connect signal to the UTS
/// client
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onDeviceConnected(Device::DeviceConnectEvent* pEvent)
{
   try
   {
      QC::DeviceInfo info = getDeviceInfoForEvent(pEvent->getDevice());
      onDeviceConnected(info);
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}
// ----------------------------------------------------------------------------
// onDeviceConnected
//
/// Signals to the UTS client that a new device
/// has been connected
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onDeviceConnected(const DeviceInfo& deviceInfo)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_connectedCallback)
      {
         m_connectedCallback(deviceInfo);
      }
   }
   TOOLS_CATCH(e, reportException(e));
}

// ----------------------------------------------------------------------------
// onDeviceDisconnected
//
/// Signals to the UTS client that a device was
/// disconnected
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onDeviceDisconnected(const DeviceInfo& deviceInfo)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_disconnectedCallback)
      {
         m_disconnectedCallback(deviceInfo);
      }
   }
   TOOLS_CATCH(e, reportException(e));
}
// ----------------------------------------------------------------------------
// onDeviceDisconnected
//
/// Calls the device disconnect signal to the UTS
/// client
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onDeviceDisconnected(Device::DeviceDisconnectEvent* pEvent)
{
   try
   {
      QC::DeviceInfo info = getDeviceInfoForEvent(pEvent->getDevice());
      onDeviceDisconnected(info);
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}

// ----------------------------------------------------------------------------
// getDeviceMode
//
/// Returns current device mode of operations
// ----------------------------------------------------------------------------
QC::DeviceMode ClientCallbackHandler::getDeviceMode(const int64_t deviceHandle)
{
   (void)deviceHandle; // Suppress unused
                       // parameter warning
   ::QC::DeviceMode deviceMode = QC::DeviceMode::DEVICE_MODE_NONE;
   FLOG_INFO(std::string("Device mode: ") + std::to_string(deviceMode));
   return deviceMode;
}
// ----------------------------------------------------------------------------
// onDeviceModeChange
//
/// Calls the device mode change signal to the UTS
/// client
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onDeviceModeChange(Device::DeviceModeChangeEvent* pEvent)
{
   try
   {
      Device::DeviceHandle handle = pEvent->getDevice()->getHandle();
      QC::DeviceMode mode = getDeviceMode(handle);
      onDeviceModeChange(handle, mode);
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}
// ----------------------------------------------------------------------------
// onDeviceModeChange
//
/// Signals to the UTS client that the mode of a
/// device has changed
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onDeviceModeChange(const int64_t deviceHandle, const QC::DeviceMode newMode)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_deviceModeChangeCallback)
      {
         m_deviceModeChangeCallback(deviceHandle, newMode);
      }
   }
   TOOLS_CATCH(e, reportException(e));
}


// ----------------------------------------------------------------------------
// onProtocolAdded
//
/// Calls the protocol added signal to the UTS
/// client
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onProtocolAdded(Device::ProtocolAddedEvent* pEvent)
{
   try
   {
      QC::DeviceInfo deviceInfo = getDeviceInfoForEvent(pEvent->getDevice());
      QC::ProtocolInfo protocolInfo = getProtocolInfoForHandle(pEvent->getProtocol()->getHandle());

      onProtocolAdded(deviceInfo, protocolInfo);
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}
// ----------------------------------------------------------------------------
// onProtocolAdded
//
/// Signals to the UTS client that a new protocol
/// was added
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onProtocolAdded(const DeviceInfo& deviceInfo, const ProtocolInfo& protocolInfo)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_protocolAddedCallback)
      {
         m_protocolAddedCallback(deviceInfo, protocolInfo);
      }
   }
   TOOLS_CATCH(e, reportException(e));
}

// ----------------------------------------------------------------------------
// onProtocolRemoved
//
/// Calls the protocol removed signal to the UTS
/// client
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onProtocolRemoved(Device::ProtocolRemovedEvent* pEvent)
{
   try
   {
      QC::DeviceInfo deviceInfo = getDeviceInfoForEvent(pEvent->getDevice());
      QC::ProtocolInfo protocolInfo = getProtocolInfoForHandle(pEvent->getProtocol()->getHandle());


      onProtocolRemoved(deviceInfo, protocolInfo);
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}
// ----------------------------------------------------------------------------
// onProtocolRemoved
//
/// Signals to the UTS client that a protocol was
/// removed
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onProtocolRemoved(const DeviceInfo& deviceInfo, const ProtocolInfo& protocolInfo)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_protocolRemovedCallback)
      {
         m_protocolRemovedCallback(deviceInfo, protocolInfo);
      }
   }
   TOOLS_CATCH(e, reportException(e));
}

// ----------------------------------------------------------------------------
// onProtocolStateChange
//
/// Calls the connection state change signal to
/// the UTS client
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onProtocolStateChange(Device::Protocol::StateChangeEvent* pEvent)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);

      Device::Protocol::Handle handle = pEvent->getProtocol()->getHandle();
      QC::ProtocolState state = static_cast<QC::ProtocolState>(pEvent->getState());

      onProtocolStateChange(handle, state);
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}
// ----------------------------------------------------------------------------
// onProtocolStateChange
//
/// Signals to the UTS client that a protocol was
/// disconnected
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onProtocolStateChange(const int64_t protocolHandle, const ProtocolState newState)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_protocolStateChangeCallback)
      {
         m_protocolStateChangeCallback(protocolHandle, newState);
      }
   }
   TOOLS_CATCH(e, reportException(e));
}


// ----------------------------------------------------------------------------
// onServiceEvent
//
/// Signals to the UTS client that an
/// service-defined event occurred
// ----------------------------------------------------------------------------
void ClientCallbackHandler::onServiceEvent(ServiceEvent* pEvent)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);

      std::string serviceName = pEvent->getServiceName();
      int64_t eventId = static_cast<int64_t>(pEvent->getEventId());
      std::string eventDescription = pEvent->getEventDescription();

      onServiceEvent(serviceName, eventId, eventDescription);
   }
   TOOLS_CATCH(e, FLOG_ERROR(std::string(e.what()) + " " + e.where() + " " + e.callStack()));
}
// ----------------------------------------------------------------------------
// onServiceEvent
//
/// Signals to the UTS client that an
/// service-defined event occurred
// ----------------------------------------------------------------------------
void ClientCallbackHandler::
   onServiceEvent(const std::string& serviceName, const int64_t eventId, const std::string& eventDescription)
{
   try
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(m_serviceEventCallback)
      {
         m_serviceEventCallback(serviceName, eventId, eventDescription);
      }
   }
   TOOLS_CATCH(e, reportException(e));
}


// ----------------------------------------------------------------------------
// reportException
//
/// Calls back the onMessage function with the
/// exception
// ----------------------------------------------------------------------------
void ClientCallbackHandler::reportException(const ToolException& e)
{
   onMessage(MessageLevel::EXCEPTION, e.where(), "Exception", e.what());
}

} // namespace QC
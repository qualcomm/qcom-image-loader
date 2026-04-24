// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "callback/Fwd.h"
#include "Definitions.h"
#include "device/Fwd.h"
#include "device/Manager.h"
#include "rpc/DeviceManagerHandler.h"
#include "rpc/Service.h"
#include "util/AppEvent.h"
#include "util/AppMessage.h"
#include "util/MemoryHelper.h"

#include <mutex>
namespace QC {


// ----------------------------------------------------------------------------
// ClientCallbackHandler
//
/// Implements the functions for the ClientCallback RPC server
// ----------------------------------------------------------------------------
class ClientCallbackHandler
: public Util::EventPublisher
, public Util::AsyncEventPublisher
, public std::enable_shared_from_this<ClientCallbackHandler>
{
   TOOLS_FORBID_COPY(ClientCallbackHandler);

public:
   typedef std::function<void(
      MessageLevel::type level,
      const std::string& location,
      const std::string& title,
      const std::string& description
   )>
      MessageDelegate;

   typedef std::function<void(const DeviceInfo&)> DeviceEventDelegate;
   typedef std::function<void(int64_t deviceHandle, const DeviceMode newMode)> DeviceModeDelegate;

   typedef std::function<void(const DeviceInfo&, const ProtocolInfo&)> ProtocolEventDelegate;
   typedef std::function<void(int64_t, ProtocolState)> ProtocolStateDelegate;

   typedef std::function<void(const std::string& serviceName, int64_t eventId, const std::string& eventDescription)>
      ServiceEventDelegate;

   friend class Util::SharedPointer<ClientCallbackHandler>;
   virtual ~ClientCallbackHandler();

   static ClientCallbackHandlerPtr getInstance();

   void setMessageCallback(const MessageDelegate& callback);
   void setDeviceConnectedCallback(const DeviceEventDelegate& callback);
   void setDeviceDisconnectedCallback(const DeviceEventDelegate& callback);
   void setDeviceModeChangeCallback(const DeviceModeDelegate& callback);
   void setProtocolAddedCallback(const ProtocolEventDelegate& callback);
   void setProtocolRemovedCallback(const ProtocolEventDelegate& callback);
   void setProtocolStateChangeCallback(const ProtocolStateDelegate& callback);
   void setServiceEventCallback(const ServiceEventDelegate& callback);

   // Functions from ClientCallback.thrift
   void onMessage(
      const MessageLevel::type level,
      const std::string& location,
      const std::string& title,
      const std::string& description
   );

   void onDeviceConnected(const DeviceInfo& deviceInfo);
   void onDeviceDisconnected(const DeviceInfo& deviceInfo);
   void onDeviceModeChange(const int64_t deviceHandle, const DeviceMode newMode);

   void onProtocolAdded(const DeviceInfo& deviceInfo, const ProtocolInfo& protocolInfo);
   void onProtocolRemoved(const DeviceInfo& deviceInfo, const ProtocolInfo& protocolInfo);
   void onProtocolStateChange(const int64_t protocolHandle, const ProtocolState newState);

   void onServiceEvent(const std::string& serviceName, const int64_t eventId, const std::string& eventDescription);

   void reportException(const ToolException& e);

   void subscribeForServiceAsyncEvents(const Util::CheckedPointer<Rpc::ServiceHandlerBase> pService);
   void unsubscribeServiceAsyncEvents(const Util::CheckedPointer<Rpc::ServiceHandlerBase> pService);

protected:
   ClientCallbackHandler();
   virtual void initialize();
   virtual void finalize();

private:
   DeviceEventDelegate m_connectedCallback;
   DeviceEventDelegate m_disconnectedCallback;
   ProtocolEventDelegate m_protocolAddedCallback;
   ProtocolEventDelegate m_protocolRemovedCallback;
   ProtocolStateDelegate m_protocolStateChangeCallback;
   MessageDelegate m_messageCallback;
   ServiceEventDelegate m_serviceEventCallback;
   DeviceModeDelegate m_deviceModeChangeCallback;
   std::recursive_mutex m_mutex;
   // Store reference to Device::Manager to avoid singleton access during
   // destruction
   Device::ManagerPtr m_pDeviceManager;

   static std::recursive_mutex g_callbackMutex;

   void onCriticalEvent(Device::CriticalEvent* pEvent);

   void onDeviceConnected(Device::DeviceConnectEvent* pEvent);

   void onDeviceDisconnected(Device::DeviceDisconnectEvent* pEvent);

   void onDeviceModeChange(Device::DeviceModeChangeEvent* pEvent);

   QC::DeviceMode getDeviceMode(const int64_t deviceHandle);

   void onProtocolAdded(Device::ProtocolAddedEvent* pEvent);

   void onProtocolRemoved(Device::ProtocolRemovedEvent* pEvent);

   void onProtocolStateChange(Device::Protocol::StateChangeEvent* pEvent);

   void onServiceEvent(ServiceEvent* pEvent);
};

} // namespace QC

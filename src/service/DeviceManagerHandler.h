// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Connection.h"
#include "protocol/Base.h"
#include "service/Fwd.h"
#include "util/MemoryHelper.h"

namespace Service {

class DummyMessagePublisher : public Util::IMessagePublisher
{
   TOOLS_FORBID_COPY(DummyMessagePublisher);

public:
   DummyMessagePublisher()
   {
   }

   virtual ~DummyMessagePublisher()
   {
   }

   void send(const Util::MessagePtr& pMessage) override
   {
      (void)pMessage; // Suppress unused parameter warning
   }

   virtual void send(
      const Util::Message::Level& level,
      const std::string& location,
      const std::string& title,
      const std::string& description = {}
   ) override
   {
      (void)level; // Suppress unused parameter warnings
      (void)location;
      (void)title;
      (void)description;
   }

   virtual void report(const ToolException& e, const std::string& catchLocation) override
   {
      (void)e; // Suppress unused parameter warnings
      (void)catchLocation;
   }
};

typedef std::map<Device::ConnectionPtr, size_t> ConnectionList;
// ----------------------------------------------------------------------------
// DeviceManagerHandler
//
/// Implements the functions for the DeviceManager RPC server
// ----------------------------------------------------------------------------
class DeviceManagerHandler
{
   TOOLS_FORBID_COPY(DeviceManagerHandler);

public:
   friend class Util::SharedPointer<DeviceManagerHandler>;
   virtual ~DeviceManagerHandler();
   QC::ErrorCode::type attachService(const int64_t deviceHandle, Service::ServiceHandlerBasePtr pService);

   // Actual implementation so that it can also be called by other services
   Device::ConnectionPtr createConnection(
      const Device::Protocol::BasePtr& pProtocol,
      const Device::Protocol::Base::Access& access,
      const Device::Protocol::Base::Share& share
   );
   void closeConnection(const Device::ConnectionPtr& pConnection);

   void finishAllServices();

   static std::shared_ptr<DeviceManagerHandler> getInstance();
   std::shared_ptr<Util::IMessagePublisher> getPublisher()
   {
      return m_pPublisher;
   }

protected:
   DeviceManagerHandler();
   virtual void initialize();
   virtual void finalize();

private:
   std::shared_ptr<Util::IMessagePublisher> m_pPublisher; ///< Place to publish messages
   ConnectionList m_connections;                          ///< Connections opened through this server
   std::recursive_mutex m_connectionsMutex;

   std::recursive_mutex m_activeServicesMutex;
   std::unordered_map<std::string, Service::ServiceHandlerBasePtr>
      m_activeServices; // active list of services created by current client
};
} // namespace Service

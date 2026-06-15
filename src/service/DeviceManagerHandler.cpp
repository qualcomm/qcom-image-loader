// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "service/DeviceManagerHandler.h"

#include "callback/ClientCallbackHandler.h"
#include "device/Exception.h"
#include "device/Manager.h"
#include "service/ImageManagementServiceHandler.h"

namespace Service {

DeviceManagerHandler::DeviceManagerHandler()
: m_pPublisher(std::make_shared<DummyMessagePublisher>())
{
}

DeviceManagerHandler::~DeviceManagerHandler()
{
   FLOG_INFO("Start ~DeviceManagerHandler.");
   FLOG_INFO("Completed ~DeviceManagerHandler.");
}

std::shared_ptr<DeviceManagerHandler> DeviceManagerHandler::getInstance()
{
   static std::once_flag initFlag;
   static Util::SharedPointer<DeviceManagerHandler> pTheDeviceManagerHandler;

   std::call_once(initFlag, [] {
      pTheDeviceManagerHandler = Util::SharedPointer<DeviceManagerHandler>::create();
      FLOG_INFO("created new pTheDeviceManagerHandler");
   });

   return pTheDeviceManagerHandler;
}

void DeviceManagerHandler::initialize()
{
}
void DeviceManagerHandler::finalize()
{
   TOOLS_IGNORE_EXCEPTIONS(finishAllServices());
   m_pPublisher = nullptr;
}

void DeviceManagerHandler::finishAllServices()
{
   FLOG_INFO(("connection count: " + std::to_string(m_connections.size())).c_str());
   while(!m_connections.empty())
   {
      closeConnection(m_connections.begin()->first);
   }

   FLOG_INFO("Remove logfile manager client.");

   {
      std::lock_guard<std::recursive_mutex> lock(m_activeServicesMutex);
      // cleanup all active services for the current client
      std::unordered_map<std::string, Service::ServiceHandlerBasePtr>::iterator it = m_activeServices.begin();
      for(; m_activeServices.end() != it; ++it)
      {
         QC::ClientCallbackHandler::getInstance()->unsubscribeServiceAsyncEvents(it->second);
         (it->second)->destroyService();
         it->second = nullptr;
      }
      m_activeServices.clear();
   }
}

QC::ErrorCode::type DeviceManagerHandler::attachService(const int64_t deviceHandle, Service::ServiceHandlerBasePtr pService)
{
   Device::ImplPtr pDevice = nullptr;

   if(::Device::Impl::INVALID_DEVICE_HANDLE != deviceHandle)
   {
      pDevice = Device::Manager::getInstance()->getDeviceByHandle(deviceHandle);
   }
   std::string serviceName = pService->getName();
   m_activeServices.insert(std::pair<std::string, Service::ServiceHandlerBasePtr>(serviceName, pService));
   QC::ClientCallbackHandler::getInstance()->subscribeForServiceAsyncEvents(pService);

   if(auto other = std::static_pointer_cast<Service::ImageManagementServiceHandler>(pService))
   {
      return other->initializeService(pDevice); // Safe to call
   }
   return QC::ErrorCode::DEVICE_INVALID_PARAMETERS;
}

// ----------------------------------------------------------------------------
// createConnection
//
/// Creates a connection internally
// ----------------------------------------------------------------------------
Device::ConnectionPtr DeviceManagerHandler::createConnection(
   const Device::Protocol::BasePtr& pProtocol,
   const Device::Protocol::Base::Access& access,
   const Device::Protocol::Base::Share& share
)
{
   bool bNewConnection = true;
   Device::ConnectionPtr pConnection;
   try
   {
      pConnection =
         Device::Manager::getInstance()
            ->openConnection(pProtocol, access, share, 0, m_pPublisher, bNewConnection);
   }
   catch(const Device::Exception& e)
   {
      try
      {
         if(Device::Exception::DEVICE_CONNECTION_LOCKED == e.getErrorCode())
         {
            // If it failed to create the connection because of sharing rules,
            // try connecting as shared (if bAllowConnectToShared)
            pConnection = Device::Manager::getInstance()->openConnection(
               pProtocol,
               access,
               Device::Protocol::Base::Share::READ_WRITE,
               0,
               m_pPublisher,
               bNewConnection
            );

            if(pConnection != nullptr)
            {
               // sendMessage(
               //    Util::Message::Level::WARNING,
               //    getName(),
               //    "Connection shared between clients",
               //    "A connection is being used by multiple clients, which
               //    could lead to inconsistent behavior: "
               //    + pProtocol->getDescription()
               //);
            }
         }
      }
      catch(...)
      {
      }

      if(pConnection == nullptr)
      {
         // If it still wasn't able to create the connection, re-throw
         throw;
      }
   }

   if(bNewConnection)
   {
#if defined(FEATURE_PROFILING_TCP) || defined(FEATURE_PROFILING_HOST)
      if(Device::ProtocolType::PROT_QSPS == Device::Manager::getInstance()->getProtocolType(pProtocol))
      {
         auto pQspsProtocol = pProtocol->getOverrideProtocol().dynamicCast<Device::Protocol::Qsps>();
         TOOLS_ASSUMING(pQspsProtocol->subscribeForAsyncEvents(this, &DeviceManagerHandler::onQspsMessage));
      }
#endif

      std::lock_guard<std::recursive_mutex> lock(m_connectionsMutex);
      m_connections[pConnection] = 1;
   }
   else
   {
      std::lock_guard<std::recursive_mutex> lock(m_connectionsMutex);
      ++m_connections[pConnection];
   }

   return pConnection;
}

// ----------------------------------------------------------------------------
// closeConnection
//
/// Closes the connection to the given device
// ----------------------------------------------------------------------------
void DeviceManagerHandler::closeConnection(const Device::ConnectionPtr& pConnection)
{
   FLOG_INFO("closeConnection: " + pConnection->getProtocol()->getDescription());

   std::lock_guard<std::recursive_mutex> lock(m_connectionsMutex);
   FLOG_INFO("connection count: " + std::to_string(m_connections.size()));

   ConnectionList::iterator it = m_connections.find(pConnection);
   TOOLS_ASSERT_OR_RETURN(m_connections.end() != it, TOOLS_VOID);

   FLOG_INFO("Found connection it->second = " + std::to_string(it->second));

   // else it->second could end up in SIZE_MAX -1
   if(0 == it->second)
   {
      m_connections.erase(it);
      FLOG_INFO("Erase closeConnection.");
      return;
   }
   --it->second;

   FLOG_INFO("Updated it->second = " + std::to_string(it->second));
   if(0 == it->second)
   {
      FLOG_INFO("unsubscribe eventhandlers.");

      FLOG_INFO(
         "Protocol Type : " +
         std::to_string(static_cast<int32_t>(Device::Manager::getInstance()->getProtocolType(pConnection->getProtocol())
         ))
      );
   }

   FLOG_INFO("Device Manager closeConnection.");
   Device::Manager::getInstance()->closeConnection(pConnection);
   FLOG_INFO("Device Manager completed closeConnection.");
   if(0 == it->second)
   {
      m_connections.erase(it);
      FLOG_INFO("Erase closeConnection.");
   }
   FLOG_INFO("closeConnection completed.");
}

} // namespace Service

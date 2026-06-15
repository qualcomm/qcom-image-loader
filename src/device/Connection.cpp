// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "device/Connection.h"

#include "device/Buffer.h"
#include "device/DataPacket.h"
#include "device/Exception.h"
#include "device/Impl.h"
#include "device/Manager.h"
#include "util/ThisThread.h"

#include <functional>
#include <mutex>

namespace Device {

static const size_t MAX_ASYNC_REQUEST_TRACKING = 10000;

// ----------------------------------------------------------------------------
// AsyncResponseEvent
//
// ----------------------------------------------------------------------------
Connection::AsyncResponseEvent::
   AsyncResponseEvent(const ConnectionPtr& pConnection, Protocol::Base::TransactionId transactionId)
: Util::Event()
, m_pConnection(pConnection)
, m_transactionId(transactionId)
{
}

// ----------------------------------------------------------------------------
// ~AsyncResponseEvent
//
// ----------------------------------------------------------------------------
Connection::AsyncResponseEvent::~AsyncResponseEvent()
{
}

// ----------------------------------------------------------------------------
// getConnection
//
// ----------------------------------------------------------------------------
ConnectionPtr Connection::AsyncResponseEvent::getConnection() const
{
   return m_pConnection;
}

// ----------------------------------------------------------------------------
// getTransactionId
//
// ----------------------------------------------------------------------------
Protocol::Base::TransactionId Connection::AsyncResponseEvent::getTransactionId() const
{
   return m_transactionId;
}

// ----------------------------------------------------------------------------
// Connection
//
// ----------------------------------------------------------------------------
Connection::Connection(
   const Protocol::BasePtr& pProtocol,
   const Protocol::Base::Access& access,
   const Protocol::Base::Share& share,
   int32_t clientId,
   std::shared_ptr<Util::IMessagePublisher> pPublisher
)
: Util::EventPublisher()
, Util::AsyncEventPublisher()
, m_clientId(clientId)
, m_pProtocol(pProtocol)
, m_access(access)
, m_share(share)
, m_bConnected(false)
, m_pPublisher(pPublisher)
, m_bRegisteredAsync(false)
, m_asyncRequests()
, m_asyncRequestMutex()
, m_mutex()
{
}

// ----------------------------------------------------------------------------
// ~Connection
//
// ----------------------------------------------------------------------------
Connection::~Connection()
{
}

// ----------------------------------------------------------------------------
// getClientId
//
/// @returns The client that this connection belongs to
// ----------------------------------------------------------------------------
int32_t Connection::getClientId() const
{
   return m_clientId;
}

// ----------------------------------------------------------------------------
// getProtocol
//
/// @returns The protocol this connection uses
// ----------------------------------------------------------------------------
Protocol::BasePtr Connection::getProtocol() const
{
   return m_pProtocol;
}

// ----------------------------------------------------------------------------
// getDevice
//
/// @returns The device this connection is on
// ----------------------------------------------------------------------------
ImplPtr Connection::getDevice() const
{
   return m_pProtocol->getDevice();
}

// ----------------------------------------------------------------------------
// getAccess
//
/// @returns The access rights this permission was opened with
// ----------------------------------------------------------------------------
Protocol::Base::Access Connection::getAccess() const
{
   return m_access;
}

// ----------------------------------------------------------------------------
// getShare
//
/// @returns The share permissions this connection was opened with
// ----------------------------------------------------------------------------
Protocol::Base::Share Connection::getShare() const
{
   return m_share;
}

// ----------------------------------------------------------------------------
// connect
//
/// Virtual function to open the connection so it can be overridden
// ----------------------------------------------------------------------------
void Connection::connect()
{
   if(Protocol::Base::INVALID_HANDLE != getProtocol()->getHandle())
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(!m_bConnected)
      {
         doConnect();
         m_bConnected = true;
      }
   }
}

// ----------------------------------------------------------------------------
// disconnect
//
/// Removes this connection from the protocol
// ----------------------------------------------------------------------------
void Connection::disconnect()
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   if(m_bConnected)
   {
      if(m_bRegisteredAsync)
      {
         m_pProtocol->unregisterReceiveAsync(std::bind(
            &Connection::onReceiveAsync,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
         ));
         m_bRegisteredAsync = false;
      }

      m_bConnected = false;
      doDisconnect();
   }
}

// ----------------------------------------------------------------------------
// sendSync
//
/// Sends on the underlying protocol
/// @returns The responses buffer
// ----------------------------------------------------------------------------
DataPacketPtr Connection::sendSync(
   const Device::SharedByteBufferPtr& pBuffer,
   const std::optional<std::chrono::milliseconds>& timeout,
   bool bPriority
)
{
   return sendSyncWithKey(Device::Protocol::PROTOCOL_NO_KEY, pBuffer, timeout, bPriority);
}

// ----------------------------------------------------------------------------
// sendSyncWithKey
//
/// Sends on the underlying protocol with lock key
/// @returns The responses buffer
// ----------------------------------------------------------------------------
DataPacketPtr Connection::sendSyncWithKey(
   const Device::Protocol::ProtocolLockKey& key,
   const Device::SharedByteBufferPtr& pBuffer,
   const std::optional<std::chrono::milliseconds>& timeout,
   bool bPriority
)
{
   TOOLS_ASSERT_OR_THROW(
      pBuffer != nullptr,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "Attempting to send NULL packet: " + getProtocol()->getDescription()
      )
   );

   TOOLS_ASSERT_OR_THROW(
      hasWriteAccess(),
      Device::Exception(
         Device::Exception::DEVICE_PERMISSIONS_ERROR,
         "Cannot sendSync; no write access on protocol " + getProtocol()->getDescription()
      )
   );

   DataPacketPtr pPacket;
   if(Device::Protocol::PROTOCOL_NO_KEY == key)
   {
      pPacket = m_pProtocol->sendSync(pBuffer, timeout, bPriority);
   }
   else
   {
      pPacket = m_pProtocol->sendSyncWithKey(key, pBuffer, timeout, bPriority);
   }
   if(pPacket != nullptr && !pPacket->isFinalResponse())
   {
      std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
      TOOLS_ASSUMING(m_asyncRequests
                        .insert(AsyncRespMap::value_type(pPacket->getTransactionId(), std::make_shared<ResponseData>()))
                        .second);
   }
   return pPacket;
}

// ----------------------------------------------------------------------------
// sendAsync
//
/// Sends on the underlying protocol
/// @returns The transaction ID used to track for the async response
// ----------------------------------------------------------------------------
Protocol::Base::TransactionId Connection::sendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority)
{
   return sendAsyncWithKey(Device::Protocol::PROTOCOL_NO_KEY, pBuffer, bPriority);
}

// ----------------------------------------------------------------------------
// sendAsyncWithKey
//
/// Sends on the underlying protocol with lock key
/// @returns The transaction ID used to track for the async response
// ----------------------------------------------------------------------------
Protocol::Base::TransactionId Connection::sendAsyncWithKey(
   const Device::Protocol::ProtocolLockKey& key,
   const Device::SharedByteBufferPtr& pBuffer,
   bool bPriority
)
{
   TOOLS_ASSERT_OR_THROW(
      pBuffer != nullptr,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "Attempting to send NULL packet: " + getProtocol()->getDescription()
      )
   );

   TOOLS_ASSERT_OR_THROW(
      hasWriteAccess(),
      Device::Exception(
         Device::Exception::DEVICE_PERMISSIONS_ERROR,
         "Cannot sendAsync; no write access on protocol " + getProtocol()->getDescription()
      )
   );

   if(!m_bRegisteredAsync)
   {
      std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
      if(!m_bRegisteredAsync)
      {
         m_pProtocol->registerReceiveAsync(std::bind(
            &Connection::onReceiveAsync,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
         ));
         m_bRegisteredAsync = true;
      }
   }

   Protocol::Base::TransactionId id;

   std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
   if(Device::Protocol::PROTOCOL_NO_KEY == key)
   {
      id = doSendAsync(pBuffer, bPriority);
   }
   else
   {
      id = m_pProtocol->sendAsyncWithKey(key, pBuffer, bPriority);
   }

   TOOLS_ASSUMING(m_asyncRequests.insert(AsyncRespMap::value_type(id, std::make_shared<ResponseData>())).second);

   while(m_asyncRequests.size() > MAX_ASYNC_REQUEST_TRACKING)
   {
      // If the queue isn't being serviced, remove old ones so the map doesn't
      // become too big
      m_asyncRequests.erase(m_asyncRequests.begin());
   }

   return id;
}

// ----------------------------------------------------------------------------
// getAsyncResponse
//
/// @returns The async response payload based on the transaction ID
// ----------------------------------------------------------------------------
DataPacketPtr Connection::getAsyncResponse(
   Protocol::Base::TransactionId transactionId,
   const std::optional<std::chrono::milliseconds>& timeout
)
{
   ResponseDataPtr pResponseData;
   {
      std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
      AsyncRespMap::iterator it = m_asyncRequests.find(transactionId);

      TOOLS_ASSERT_OR_THROW(
         m_asyncRequests.end() != it,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "getAsyncResponse Error: Transaction id  = " + std::to_string(transactionId) +
               " doesn't match any corresponding async request "
               "txid. " +
               getProtocol()->getDescription()
         )
      );

      pResponseData = it->second;

      if(pResponseData->m_responses.empty() && timeout.has_value() && timeout.value() == std::chrono::milliseconds(0))
      {
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_RESPONSE_ERROR,
            "getAsyncResponse Error: No async response "
            "received for Transaction id  = " +
               std::to_string(transactionId) + ", " + getProtocol()->getDescription()
         ));
      }
   }

   if(pResponseData->m_responses.empty())
   {
      Util::ThisThread::waitForEvent(&pResponseData->m_receiveEvent, timeout);
   }

   {
      std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
      pResponseData->m_receiveEvent.reset();
      if(pResponseData->m_responses.empty())
      {
         return DataPacketPtr();
      }

      DataPacketPtr pResponse = pResponseData->m_responses.front();
      pResponseData->m_responses.pop_front();
      if(pResponseData->m_responses.empty() && pResponseData->m_bFinished)
      {
         m_asyncRequests.erase(m_asyncRequests.find(transactionId));
      }
      return pResponse;
   }
}

// ----------------------------------------------------------------------------
// isAsyncRequestFinished
//
/// @returns True if all expected async responses have been received already
// ----------------------------------------------------------------------------
bool Connection::isAsyncRequestFinished(Protocol::Base::TransactionId transactionId) const
{
   std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
   return m_asyncRequests.end() == m_asyncRequests.find(transactionId);
}

// ----------------------------------------------------------------------------
// cancelTx
//
/// Cancels a transaction on the underlying protocol
// ----------------------------------------------------------------------------
bool Connection::cancelTx(Protocol::Base::TransactionId transactionId)
{
   TOOLS_ASSERT_OR_THROW(
      hasWriteAccess(),
      Device::Exception(
         Device::Exception::DEVICE_PERMISSIONS_ERROR,
         "Cannot cancelTx; no write access on protocol " + getProtocol()->getDescription()
      )
   );

   bool bCanceled = m_pProtocol->cancelTx(transactionId);

   std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
   AsyncRespMap::iterator it = m_asyncRequests.find(transactionId);

   if(m_asyncRequests.end() != it)
   {
      m_asyncRequests.erase(it);
   }

   return bCanceled;
}

// ----------------------------------------------------------------------------
// elevateAccess
//
/// Elevates access priveledges if they are higher
// ----------------------------------------------------------------------------
void Connection::elevateAccess(const Protocol::Base::Access& access, const Protocol::Base::Share& share)
{
   m_access = static_cast<Protocol::Base::Access>(m_access | access);
   m_share = static_cast<Protocol::Base::Share>(m_share & share);
}

// ----------------------------------------------------------------------------
// hasWriteAccess
//
/// @returns True when the connection was opened with write (transmit) access
/// rights
// ----------------------------------------------------------------------------
bool Connection::hasWriteAccess() const
{
   return Protocol::Base::Access::WRITE == (m_access & Protocol::Base::Access::WRITE);
}

// ----------------------------------------------------------------------------
// doConnect
//
/// Actually connect the connection
// ----------------------------------------------------------------------------
void Connection::doConnect()
{
   m_pProtocol->connect(m_clientId);
}

// ----------------------------------------------------------------------------
// doConnect
//
/// Actually disconnect the connection
// ----------------------------------------------------------------------------
void Connection::doDisconnect()
{
   m_pProtocol->disconnect(m_clientId);
}

// ----------------------------------------------------------------------------
// lock
//
/// Lock the protocol
// ----------------------------------------------------------------------------
void Connection::lock(const Device::Protocol::ProtocolLockKey& key, const std::string& reason)
{
   m_pProtocol->lock(m_clientId, key, reason);
}

// ----------------------------------------------------------------------------
// unlock
//
/// Unlock the protocol
// ----------------------------------------------------------------------------
void Connection::unlock(const Device::Protocol::ProtocolLockKey& key)
{
   m_pProtocol->unlock(m_clientId, key);
}

// ----------------------------------------------------------------------------
// onReceiveAsync
//
/// Checks if the async message matches one sent by this connection
// ----------------------------------------------------------------------------
void Connection::
   onReceiveAsync(const DataPacketPtr& pPayload, Protocol::Base::TransactionId transactionId, bool bFinalResponse)
{
   ResponseDataPtr pResponseData;
   {
      std::lock_guard<std::recursive_mutex> lock(m_asyncRequestMutex);
      AsyncRespMap::iterator it = m_asyncRequests.find(transactionId);

      if(m_asyncRequests.end() != it)
      {
         pResponseData = it->second;
         pResponseData->m_responses.push_back(pPayload);
         pResponseData->m_bFinished = bFinalResponse;
      }
      if(pResponseData != nullptr)
      {
         pResponseData->m_receiveEvent.signal();
      }
   }
   if(pResponseData != nullptr)
   {
      notify(std::make_shared<AsyncResponseEvent>((ConnectionPtr)shared_from_this(), transactionId));
      FLOG_DEBUG(
         "Connection::onReceiveAsync: notify,  transid = " +
         toString(transactionId) + ", bFinalResponse = " +
         toString((uint32_t)bFinalResponse)
      );
   }
   else
   {
      FLOG_DEBUG(
         "Connection::onReceiveAsync: pResponseData == NULL, transid = " +
         toString(transactionId) + ", bFinalResponse = " +
         toString((uint32_t)bFinalResponse)
      );
   }
}

// ----------------------------------------------------------------------------
// doSendAsync
//
/// Actually sends the async command on the protocol
// ----------------------------------------------------------------------------
Protocol::Base::TransactionId Connection::doSendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority)
{
   return m_pProtocol->sendAsync(pBuffer, bPriority);
}

// ----------------------------------------------------------------------------
// initialize
//
/// Does nothing.  Intended to be overridden
// ----------------------------------------------------------------------------
void Connection::initialize()
{
   TOOLS_ASSUMING(m_pProtocol->subscribeForAsyncEvents(this, &Connection::onStateChange));
   TOOLS_ASSUMING(m_pProtocol->subscribe(this, &Connection::onMessage));
}

// ----------------------------------------------------------------------------
// finalize
//
/// Removes all subscriptions before final deletion
// ----------------------------------------------------------------------------
void Connection::finalize()
{
   if(m_bRegisteredAsync)
   {
      TOOLS_IGNORE_EXCEPTIONS(m_pProtocol->unregisterReceiveAsync(std::bind(
         &Connection::onReceiveAsync,
         shared_from_this(),
         std::placeholders::_1,
         std::placeholders::_2,
         std::placeholders::_3
      )));
      m_bRegisteredAsync = false;
   }

   TOOLS_IGNORE_EXCEPTIONS(disconnect());


   TOOLS_IGNORE_EXCEPTIONS(m_pProtocol->unsubscribeAsyncEvents(this, &Connection::onStateChange));
   TOOLS_IGNORE_EXCEPTIONS(m_pProtocol->unsubscribe(this, &Connection::onMessage));
}

// ----------------------------------------------------------------------------
// onStateChange
//
/// Protocol state change.
// ----------------------------------------------------------------------------
void Connection::onStateChange(Protocol::StateChangeEvent* pEvent)
{
   TOOLS_ASSERT(pEvent->getProtocol() == m_pProtocol);
   notifyAsync(std::shared_ptr<Protocol::StateChangeEvent>(pEvent, [](Protocol::StateChangeEvent*) {}));
}

// ----------------------------------------------------------------------------
// onMessage
//
/// Sends the message back to the RPC client
// ----------------------------------------------------------------------------
void Connection::onMessage(Util::Message* pMessage)
{
   if(NULL != m_pPublisher)
   {
      m_pPublisher->send(std::shared_ptr<Util::Message>(pMessage, [](Util::Message*) {}));
   }
}

} // namespace Device

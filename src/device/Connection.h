// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Fwd.h"
#include "protocol/Base.h"
#include "util/AppEvent.h"
#include "util/AppMessage.h"
#include "util/Event.h"

#include <chrono>
#include <mutex>
#include <optional>
namespace Device {

// ----------------------------------------------------------------------------
// Connection
//
/// Wraps up a connection (the protocol, device, and communication used)
// ----------------------------------------------------------------------------
class Connection
: public Util::EventPublisher
, public Util::AsyncEventPublisher
, public std::enable_shared_from_this<Connection>
{
   TOOLS_FORBID_COPY(Connection);

public:
   class AsyncResponseEvent : public Util::Event
   {
      TOOLS_FORBID_COPY(AsyncResponseEvent);

   public:
      AsyncResponseEvent(const ConnectionPtr& pConnection, Protocol::Base::TransactionId transactionId);
      virtual ~AsyncResponseEvent();

      ConnectionPtr getConnection() const;
      Protocol::Base::TransactionId getTransactionId() const;

   private:
      ConnectionPtr m_pConnection; ///< Connection that was disconnected
      Protocol::Base::TransactionId m_transactionId;
   };

   friend class Util::SharedPointer<Connection>;

   Connection(
      const Protocol::BasePtr& pProtocol,
      const Protocol::Base::Access& access,
      const Protocol::Base::Share& share,
      int32_t clientId,
      std::shared_ptr<Util::IMessagePublisher> pPublisher
   );
   virtual ~Connection();

   int32_t getClientId() const;
   Protocol::BasePtr getProtocol() const;
   ImplPtr getDevice() const;
   Protocol::Base::Access getAccess() const;
   Protocol::Base::Share getShare() const;
   bool cancelTx(Protocol::Base::TransactionId transactionId);

   void elevateAccess(const Protocol::Base::Access& access, const Protocol::Base::Share& share);

   bool hasReadAccess() const;
   bool hasWriteAccess() const;

   virtual void connect();
   virtual void disconnect();
   virtual DataPacketPtr sendSync(
      const Device::SharedByteBufferPtr& pBuffer,
      const std::optional<std::chrono::milliseconds>& timeout = std::nullopt,
      bool bPriority = false
   );
   virtual Protocol::Base::TransactionId sendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority = false);
   virtual DataPacketPtr sendSyncWithKey(
      const Device::Protocol::ProtocolLockKey& key,
      const Device::SharedByteBufferPtr& pBuffer,
      const std::optional<std::chrono::milliseconds>& timeout = std::nullopt,
      bool bPriority = false
   );
   virtual Protocol::Base::TransactionId sendAsyncWithKey(
      const Device::Protocol::ProtocolLockKey& key,
      const Device::SharedByteBufferPtr& pBuffer,
      bool bPriority = false
   );

   DataPacketPtr getAsyncResponse(
      Protocol::Base::TransactionId transactionId,
      const std::optional<std::chrono::milliseconds>& timeout = std::chrono::milliseconds(0)
   );

   bool isAsyncRequestFinished(Protocol::Base::TransactionId transactionId) const;

   // DataProcessorPtr getDataProcessor(const std::string& name);

   std::shared_ptr<Util::IMessagePublisher> getPublisher() const
   {
      return m_pPublisher;
   }

   void lock(const Device::Protocol::ProtocolLockKey& key, const std::string& reason);
   void unlock(const Device::Protocol::ProtocolLockKey& key);
   std::recursive_mutex& getMutex() const noexcept
   {
      return m_mutex;
   }


protected:
   // typedef std::unordered_map<std::string, DataProcessorPtr>
   // DataProcessorMap;
   typedef std::list<DataPacketPtr> ResponseList;

   class ResponseData
   {
      TOOLS_FORBID_COPY(ResponseData);

   public:
      ResponseData()
      : m_responses()
      , m_bFinished(false)
      , m_receiveEvent()
      {
      }

      ResponseList m_responses;
      bool m_bFinished;

      Util::Event m_receiveEvent;
   };
   typedef std::shared_ptr<ResponseData> ResponseDataPtr;
   typedef std::unordered_map<uint64_t, ResponseDataPtr> AsyncRespMap;

   virtual void doConnect();
   virtual void doDisconnect();

   void onReceiveAsync(const DataPacketPtr& pPayload, Protocol::Base::TransactionId transactionId, bool bFinalResponse);

   virtual Protocol::Base::TransactionId doSendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority);

   virtual void initialize();
   virtual void finalize();


   void onStateChange(Protocol::StateChangeEvent* pEvent);
   void onMessage(Util::Message* pMessage);

   Protocol::BasePtr m_pProtocol;   ///< Protocol connection is on
   volatile int32_t m_clientId;     ///< Client to which this connection belongss
   Protocol::Base::Access m_access; ///< Access rights of connection
   Protocol::Base::Share m_share;   ///< Share permissions
   bool m_bConnected;               ///< Whether disconnect() haws been called

   std::shared_ptr<Util::IMessagePublisher> m_pPublisher; ///< For propagating messages


   bool m_bRegisteredAsync;                          ///< Whether has registered async receive
   AsyncRespMap m_asyncRequests;                     ///< All async requests sent
   mutable std::recursive_mutex m_asyncRequestMutex; ///< Protects m_asyncRequests

   mutable std::recursive_mutex m_mutex;
};

// ----------------------------------------------------------------------------
// ConnectionScopeLock
//
/// Wraps up a connection (the protocol, device, and communication used)
// ----------------------------------------------------------------------------
class ConnectionScopeLock
{
   TOOLS_FORBID_COPY(ConnectionScopeLock);

public:
   ConnectionScopeLock(
      const ConnectionPtr& connection,
      const Device::Protocol::ProtocolLockKey& key,
      const std::string& reason
   )
   : m_pConnection(connection)
   , m_key(key)
   , m_guard(connection->getMutex())
   {
      m_pConnection = connection;
      m_pConnection->lock(key, reason);
   }

   ~ConnectionScopeLock()
   {
      m_pConnection->unlock(m_key);
   }

private:
   ConnectionPtr m_pConnection;
   Device::Protocol::ProtocolLockKey m_key;
   std::unique_lock<std::recursive_mutex> m_guard;
};

#define CONNECTION_LOCK(connection, key, reason)                                                                       \
   Device::ConnectionScopeLock TOOLS_ANONYMOUS_IDENTIFIER(connectionScopeLock)(connection, key, reason)
} // namespace Device

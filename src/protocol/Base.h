// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "communication/Fwd.h"
#include "device/DataPacket.h"
#include "device/Exception.h"
#include "device/Fwd.h"
#include "device/Logger.h"
#include "protocol/FlowControl.h"
#include "util/AppEvent.h"
#include "util/AppMessage.h"
#include "util/AppUser.h"

#include <chrono>
#include <mutex>
#include <optional>
#include <set>

namespace Device {
namespace Protocol {

typedef std::string ProtocolLockKey;
static const ProtocolLockKey PROTOCOL_NO_KEY = "";

// ----------------------------------------------------------------------------
// Base
//
/// Interface for all protocols to inherit from
// ----------------------------------------------------------------------------
class Base
: public Util::EventPublisher
, public Util::AsyncEventPublisher
, public Util::IMessagePublisher
, public FlowControl
, public std::enable_shared_from_this<Base>
{
   TOOLS_FORBID_COPY(Base);

public:
   static const Handle INVALID_HANDLE = -1;

   // Must lock from a connection
   friend class Connection;
   friend class Util::SharedPointer<Base>;

   virtual ~Base();

   enum Access
   {
      NONE = 0,                  ///< Process has no access to the file
      READ = 1,                  ///< Process can read the file
      WRITE = 2,                 ///< Process can write the file
      READ_WRITE = READ | WRITE, ///< Process can read and write
   };

   typedef Protocol::Base::Access Share;

   typedef int32_t TransactionId;
   static const TransactionId NULL_TRANSACTION_ID = 0;
   static const int32_t NO_CLIENT_ID = -1;
   typedef std::function<void(const DataPacketPtr&, TransactionId, bool)> ReceiveDelegate;

   enum Direction
   {
      DIR_RX = 0x1,
      DIR_TX = 0x2,
      DIR_BOTH = DIR_RX | DIR_TX
   };

   enum State
   {
      // Basic state
      STATE_AVAILABLE,
      STATE_DISCONNECTED,
      STATE_UNRESPONSIVE,
      STATE_INITIALIZING
   };

   enum LockState
   {
      LOCK_STATE_OFF,
      LOCK_STATE_ON
   };

   struct LockStatus
   {
      LockState lockState;
      int32_t clientId;
      std::string reason;
   };

   Handle getHandle() const;
   virtual void setHandle(const ImplPtr& pDevice, Handle handle);
   std::string getCommonIoDescription() const;
   std::string getCommonIoIdentifier() const;
   ImplPtr getDevice() const;

   /// Helper to get Util::SharedPointer from this object
   inline BasePtr getSharedPtr()
   {
      return BasePtr(shared_from_this());
   }
   Communication::CommonIoPtr getCommonIo() const;

   virtual State getState() const;
   virtual void setState(State newState);
   virtual void flowControlCallback(FlowControl::FlowControlStatus status);
   void registerReceiveAsync(const ReceiveDelegate& callback);
   void unregisterReceiveAsync(const ReceiveDelegate& callback);

   // To be overridden
   virtual std::string getSerialNumber() const;
   virtual std::string getDescription() const;
   virtual void connect(const int32_t clientId = NO_CLIENT_ID);
   virtual void disconnect(const int32_t clientId = NO_CLIENT_ID);
   virtual void reset(Direction dir);
   virtual DataPacketPtr sendSync(
      const Device::SharedByteBufferPtr& pBuffer,
      const std::optional<std::chrono::milliseconds>& timeout = std::nullopt,
      bool bPriority = false
   );
   virtual Device::DataPacketPtr sendSyncWithKey(
      const Device::Protocol::ProtocolLockKey& key,
      const Device::SharedByteBufferPtr& pBuffer,
      const std::optional<std::chrono::milliseconds>& timeout = std::nullopt,
      bool bPriority = false
   );
   virtual TransactionId sendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority = false);
   virtual TransactionId sendAsyncWithKey(
      const Device::Protocol::ProtocolLockKey& key,
      const Device::SharedByteBufferPtr& pBuffer,
      bool bPriority = false
   );
   virtual bool cancelTx(TransactionId transactionId);
   virtual void forceDisconnect();

   virtual BasePtr getOverrideProtocol();
   virtual ConnectionPtr createConnection(
      const Protocol::Base::Access& access,
      const Protocol::Base::Share& share,
      int32_t clientId,
      std::shared_ptr<Util::IMessagePublisher> pPublisher
   );

   // Overridden from IMessagePublisher
   virtual void send(const Util::MessagePtr& pMessage);
   virtual void send(
      const Util::Message::Level& level,
      const std::string& location,
      const std::string& title,
      const std::string& description = std::string()
   );
   virtual void report(const ToolException& e, const std::string& catchLocation);

   virtual void onDrop();
   virtual void onAdd();

   // lock protocol for override
   virtual void lock(const int32_t clientId, const Device::Protocol::ProtocolLockKey& key, const std::string& reason);
   virtual bool getLock(const Device::Protocol::ProtocolLockKey& key, std::string& reason);
   virtual void unlock(const int32_t clientId, const Device::Protocol::ProtocolLockKey& key);

protected:
   typedef std::vector<ReceiveDelegate> CallbackList;
   Base(
      const Communication::CommonIoPtr& pIo,
      const std::string& description = std::string() //,
   );
   virtual void initialize() {};
   virtual void finalize() {};

   Util::CheckedPointer<Impl> m_pDevice; ///< Parent device
   Communication::CommonIoPtr m_pIo;     ///< Actual communication layer
   std::string m_description;            ///< Description of the instance
   Protocol::Handle m_handle;            ///< Handle for RPC layer
   State m_state;                        ///< State of the connection

   Util::UserPtr m_pUser; ///< User handle for this protocol

   CallbackList m_asyncCallbacks; ///< All registered for async
   mutable std::recursive_mutex m_mutex;

   std::recursive_mutex m_receivedPacketMutex;

   std::recursive_mutex m_lockMutex; ///< Protects lock configuration
   std::string m_lockReason;
   volatile int32_t m_lockClientId;
   ProtocolLockKey m_lockKey;
};

// ----------------------------------------------------------------------------
// StateChangeEvent
//
/// Event from a protocol changing state
// ----------------------------------------------------------------------------
class StateChangeEvent : public Util::AsyncEvent
{
   TOOLS_FORBID_COPY(StateChangeEvent);

public:
   StateChangeEvent(const BasePtr& pProtocol, Base::State newState)
   : Util::AsyncEvent()
   , m_pProtocol(pProtocol)
   , m_state(newState)
   {
   }

   BasePtr getProtocol() const
   {
      return m_pProtocol;
   }

   Base::State getState() const
   {
      return m_state;
   }

private:
   BasePtr m_pProtocol; ///< Protocol that disconnected
   Base::State m_state; ///< The state the protocol is now in
};

// ----------------------------------------------------------------------------
// FlowControlStatusChangeEvent
//
/// Event from a protocol changing flow control status
// ----------------------------------------------------------------------------
class FlowControlStatusChangeEvent : public Util::Event
{
   TOOLS_FORBID_COPY(FlowControlStatusChangeEvent);

public:
   FlowControlStatusChangeEvent(
      const BasePtr& pProtocol,
      Base::Direction dir,
      FlowControl::FlowControlStatus flowControlStatus
   )
   : Util::Event()
   , m_pProtocol(pProtocol)
   , m_direction(dir)
   , m_flowControlStatus(flowControlStatus)
   {
   }

   BasePtr getProtocol() const
   {
      return m_pProtocol;
   }

   Base::Direction getDirection() const
   {
      return m_direction;
   }

   FlowControl::FlowControlStatus getStatus() const
   {
      return m_flowControlStatus;
   }

private:
   BasePtr m_pProtocol;                                ///< Protocol that disconnected
   Base::Direction m_direction;                        ///< Direction of flow control
   FlowControl::FlowControlStatus m_flowControlStatus; ///< The current flow control status for protocol
};


} // namespace Protocol
} // namespace Device

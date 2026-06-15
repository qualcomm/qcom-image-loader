// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "protocol/Base.h"

#include "communication/CommonIO.h"
#include "device/Connection.h"
#include "device/DataPacket.h"
#include "device/Exception.h"
#include "device/Impl.h"
#include "device/Manager.h"
#include "report/StatusManager.h"
#include "util/AppUser.h"
#include "util/FunctionHelper.h"

namespace Device {
namespace Protocol {

// ----------------------------------------------------------------------------
// virtual base function to be implemented
//
// ----------------------------------------------------------------------------
void Base::connect(const int32_t clientId)
{
   (void)clientId; // Suppress unused parameter warning
}
void Base::disconnect(const int32_t clientId)
{
   (void)clientId; // Suppress unused parameter warning
}
void Base::reset(Direction dir)
{
   (void)dir; // Suppress unused parameter warning
}
bool Base::cancelTx(TransactionId transactionId)
{
   (void)transactionId; // Suppress unused parameter warning
   return false;
}
void Base::forceDisconnect()
{
}
DataPacketPtr Base::sendSync(
   const Device::SharedByteBufferPtr& pBuffer,
   const std::optional<std::chrono::milliseconds>& timeout,
   bool bPriority
)
{
   (void)pBuffer;   // Suppress unused parameter warning
   (void)timeout;   // Suppress unused parameter warning
   (void)bPriority; // Suppress unused parameter warning
   return DataPacketPtr();
}
Base::TransactionId Base::sendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority)
{
   (void)pBuffer;   // Suppress unused parameter warning
   (void)bPriority; // Suppress unused parameter warning
   return -1;
}
// ----------------------------------------------------------------------------
// Base
//
// ----------------------------------------------------------------------------
Base::Base(const Communication::CommonIoPtr& pIo, const std::string& description)
: Util::EventPublisher()
, Util::IMessagePublisher()
, FlowControl()
, m_pDevice()
, m_pIo(pIo)
, m_description(description)
, m_handle(INVALID_HANDLE)
, m_state(STATE_DISCONNECTED)
, m_pUser(std::make_shared<Util::User>())
, m_mutex()
, m_receivedPacketMutex()
, m_lockClientId(NO_CLIENT_ID)
, m_lockReason()
, m_lockKey(PROTOCOL_NO_KEY)
{
}

// ----------------------------------------------------------------------------
// ~Base
//
// ----------------------------------------------------------------------------
Base::~Base()
{
}

// ----------------------------------------------------------------------------
// getHandle
//
/// @returns The handle used to reference this protocol instance
// ----------------------------------------------------------------------------
Handle Base::getHandle() const
{
   return m_handle;
}

// ----------------------------------------------------------------------------
// setHandle
//
/// Sets the handle to be used to reference this protocol instance
// ----------------------------------------------------------------------------
void Base::setHandle(const ImplPtr& pDevice, Handle handle)
{
   m_pDevice = pDevice;
   m_handle = handle;
}

// ----------------------------------------------------------------------------
// getSerialNumber
//
/// @returns The serial number as discovered by the I/O layer
// ----------------------------------------------------------------------------
std::string Base::getSerialNumber() const
{
   if(m_pIo != nullptr)
   {
      return m_pIo->getSerialNumber();
   }
   else
   {
      return std::string();
   }
}

// ----------------------------------------------------------------------------
// getCommonIoDescription
//
/// @returns The description of the communication layer this protocol uses
// ----------------------------------------------------------------------------
std::string Base::getCommonIoDescription() const
{
   return m_pIo->getDescription();
}

// ----------------------------------------------------------------------------
// getCommonIoIdentifier
//
/// @returns The identifier of the communication layer this protocol uses
// ----------------------------------------------------------------------------
std::string Base::getCommonIoIdentifier() const
{
   return m_pIo->getIdentifier();
}

// ----------------------------------------------------------------------------
// getDescription
//
/// @returns The description of the interface
// ----------------------------------------------------------------------------
std::string Base::getDescription() const
{
   if(m_description.empty() && m_pIo != nullptr)
   {
      return m_pIo->getDescription();
   }

   return m_description;
}

// ----------------------------------------------------------------------------
// getDevice
//
/// @returns The device this protocol lives on
// ----------------------------------------------------------------------------
ImplPtr Base::getDevice() const
{
   return m_pDevice.lock();
}

// ----------------------------------------------------------------------------
// getCommonIo
//
/// @returns The communication layer this protocol uses
// ----------------------------------------------------------------------------
Communication::CommonIoPtr Base::getCommonIo() const
{
   return m_pIo;
}


// ----------------------------------------------------------------------------
// getState
//
/// @returns The state the protocol is in relative to its connection
// ----------------------------------------------------------------------------
Base::State Base::getState() const
{
   return m_state;
}

// ----------------------------------------------------------------------------
// stateToString
//
/// File-local helper to convert State enum to string for logging
// ----------------------------------------------------------------------------
static std::string stateToString(Base::State state)
{
   switch(state)
   {
      case Base::STATE_AVAILABLE:
         return "STATE_AVAILABLE";
      case Base::STATE_DISCONNECTED:
         return "STATE_DISCONNECTED";
      case Base::STATE_UNRESPONSIVE:
         return "STATE_UNRESPONSIVE";
      case Base::STATE_INITIALIZING:
         return "STATE_INITIALIZING";
      default:
         return "STATE_UNKNOWN";
   }
}

// ----------------------------------------------------------------------------
// setState
//
/// Sets the state of the protocol and notifies the change
// ----------------------------------------------------------------------------
void Base::setState(Base::State newState)
{
   if(m_state == newState)
   {
      return;
   }

   FLOG_INFO(
      "Protocol state changed from old state: " + stateToString(m_state) + ", newState: " + stateToString(newState)
   );

   m_state = newState;
   notifyAsync(std::make_shared<Protocol::StateChangeEvent>(getSharedPtr(), m_state));
}

// -------------------------------------------------------------------------
// flowControlLevelCallback
//
/// Callback handler to receive flow control status update
// -------------------------------------------------------------------------
void Base::flowControlCallback(FlowControl::FlowControlStatus status)
{
   notify(std::make_shared<Protocol::FlowControlStatusChangeEvent>(getSharedPtr(), Base::Direction::DIR_RX, status));
}

// ----------------------------------------------------------------------------
// registerReceiveAsync
//
/// Adds a callback for async data receive
// ----------------------------------------------------------------------------
void Base::registerReceiveAsync(const ReceiveDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   m_asyncCallbacks.push_back(callback);
}

// ----------------------------------------------------------------------------
// unregisterReceiveAsync
//
/// Removes a callback for async data receive
// ----------------------------------------------------------------------------
void Base::unregisterReceiveAsync(const ReceiveDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   CallbackList::iterator it =
      std::find_if(m_asyncCallbacks.begin(), m_asyncCallbacks.end(), [&callback](const ReceiveDelegate& registered) {
         return Util::isSameFunction(registered, callback);
      });

   TOOLS_ASSERT_OR_RETURN(m_asyncCallbacks.end() != it, TOOLS_VOID);

   m_asyncCallbacks.erase(it);
}

// ----------------------------------------------------------------------------
// lock
//
/// Locks the protocol
// ----------------------------------------------------------------------------
void Base::lock(const int32_t clientId, const Device::Protocol::ProtocolLockKey& key, const std::string& reason)
{
   TOOLS_UNUSED_PARAMETER(clientId);
   TOOLS_UNUSED_PARAMETER(key);
   TOOLS_UNUSED_PARAMETER(reason);
   TOOLS_THROW(ToolException("Lock feature is not supported"));
}

// ----------------------------------------------------------------------------
// unlock
//
/// Unlock the protocol
// ----------------------------------------------------------------------------
void Base::unlock(const int32_t clientId, const Device::Protocol::ProtocolLockKey& key)
{
   TOOLS_UNUSED_PARAMETER(clientId);
   TOOLS_UNUSED_PARAMETER(key);
   TOOLS_THROW(ToolException("Unlock feature is not supported"));
}

// ----------------------------------------------------------------------------
// getLock
//
/// test if the protocol is reachable with the key
// ----------------------------------------------------------------------------
bool Base::getLock(const Device::Protocol::ProtocolLockKey& key, std::string& reason)
{
   std::lock_guard<std::recursive_mutex> lock(m_lockMutex);
   if(NO_CLIENT_ID == m_lockClientId || PROTOCOL_NO_KEY == m_lockKey)
   {
      return true;
   }
   if(key == m_lockKey)
   {
      return true;
   }
   reason = m_lockReason;
   return false;
}

// ----------------------------------------------------------------------------
// sendSyncWithKey
//
/// send command in lock
// ----------------------------------------------------------------------------
Device::DataPacketPtr Base::sendSyncWithKey(
   const Device::Protocol::ProtocolLockKey& key,
   const Device::SharedByteBufferPtr& pBuffer,
   const std::optional<std::chrono::milliseconds>& timeout,
   bool bPriority
)
{
   TOOLS_UNUSED_PARAMETER(key);
   TOOLS_UNUSED_PARAMETER(pBuffer);
   TOOLS_UNUSED_PARAMETER(timeout);
   TOOLS_UNUSED_PARAMETER(bPriority);
   TOOLS_THROW(ToolException("Send sync with key feature is not supported"));
}

// ----------------------------------------------------------------------------
// sendAsyncWithKey
//
/// send async command in lock
// ----------------------------------------------------------------------------
Base::TransactionId Base::sendAsyncWithKey(
   const Device::Protocol::ProtocolLockKey& key,
   const Device::SharedByteBufferPtr& pBuffer,
   bool bPriority
)
{
   TOOLS_UNUSED_PARAMETER(key);
   TOOLS_UNUSED_PARAMETER(pBuffer);
   TOOLS_UNUSED_PARAMETER(bPriority);
   TOOLS_THROW(ToolException("Send async with key feature is not supported"));
}
// ----------------------------------------------------------------------------
// getOverrideProtocol
//
/// Allows the protocol to delegate to an underlying protocol.  Useful for
/// "DiagManager" to return the actual Diag or Sahara protocol used, or for
/// "Unknown" to return an overriding protocol
/// @returns The default override, which is this protocol (no override)
// ----------------------------------------------------------------------------
BasePtr Base::getOverrideProtocol()
{
   return getSharedPtr();
}

// ----------------------------------------------------------------------------
// createConnection
//
/// Creates a connection object for the given protocol.  Allows overriding so
/// that protocols can add case specific info (like user db locations for Diag)
/// @returns The created connection object
// ----------------------------------------------------------------------------
ConnectionPtr Base::createConnection(
   const Protocol::Base::Access& access,
   const Protocol::Base::Share& share,
   int32_t clientId,
   std::shared_ptr<Util::IMessagePublisher> pPublisher
)
{
   return Util::SharedPointer<Connection>::create(getSharedPtr(), access, share, clientId, pPublisher);
}


// ----------------------------------------------------------------------------
// send
//
/// Sends the notification to those subscribed
// ----------------------------------------------------------------------------
void Base::send(const Util::MessagePtr& pMessage)
{
   if(0 < getSubscriberCount())
   {
      notify(pMessage);
   }
}

// ----------------------------------------------------------------------------
// send
//
/// Sends the notification to those subscribed
// ----------------------------------------------------------------------------
void Base::send(
   const Util::Message::Level& level,
   const std::string& location,
   const std::string& title,
   const std::string& description
)
{
   if(0 < getSubscriberCount())
   {
      send(Util::Message::create(level, location, title, description));
   }
}

// ----------------------------------------------------------------------------
// report
//
/// Sends the notification to those subscribed
// ----------------------------------------------------------------------------
void Base::report(const ToolException& e, const std::string& catchLocation)
{
   if(0 < getSubscriberCount())
   {
      send(
         Util::Message::Level::EXCEPTION,
         e.where(),
         e.what(),
         "Caught at: " + catchLocation + ", Call Stack: " + e.callStack()
      );
   }
}


// ----------------------------------------------------------------------------
// onDrop
//
/// Handle when device is dropped.
// ----------------------------------------------------------------------------
void Base::onDrop()
{
   // For inheritance
}

// ----------------------------------------------------------------------------
// onAdd
//
/// Handle when device is added.
// ----------------------------------------------------------------------------
void Base::onAdd()
{
   // Most protocols are immediately available.  To change this, onAdd must
   // be overridden
   setState(STATE_AVAILABLE);
}


} // namespace Protocol
} // namespace Device

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Buffer.h"
#include "device/DataPacket.h"
#include "device/Manager.h"
#include "protocol/Base.h"
#include "report/DataContainer.h"
#include "util/Event.h"
#include "util/ThreadHelper.h"

#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>

namespace Device {
namespace Protocol {
static constexpr auto SAHARA_RX_WAIT_INTERVAL = std::chrono::milliseconds(100);
class SaharaRxWorker;

// ----------------------------------------------------------------------------
// Sahara Protocol
//
/// Interface for Sahara protocol
// ----------------------------------------------------------------------------
class Sahara
: public Base
, public Report::DataContainer
{
   TOOLS_FORBID_COPY(Sahara);

public:
   enum Version
   {
      SUPPORTED_HIGHEST_VERSION = 3,
      SUPPORTED_DEFAULT_VERSION = 2,
      SUPPORTED_LOWEST_VERSION = 1
   };

   enum Status
   {
      STATUS_SUCCESS = 0
   };

   enum Mode
   {
      MODE_IMAGE_TX_PENDING = 0,
      MODE_IMAGE_TX_COMPLETE = 1,
      MODE_MEMORY_DEBUG = 2,
      MODE_COMMAND = 3,
      MODE_EFS_SYNC = 0xFE,
      MODE_UNKNOWN = 0xFF
   };

   enum ImageId
   {
      IMAGE_ID_AMSS = 2,
      IMAGE_ID_APPS = 6,
      IMAGE_ID_DSP1 = 8,
      IMAGE_ID_DBL = 10,
      IMAGE_ID_OSBL = 11,
      IMAGE_ID_DSP2 = 12,
      IMAGE_ID_EDL_PROGRAMMER = 13,
      IMAGE_ID_EFS1 = 16,
      IMAGE_ID_EFS2 = 17,
      IMAGE_ID_EFS3 = 20,
      IMAGE_ID_SBL1 = 21,
      IMAGE_ID_SBL2 = 22,
      IMAGE_ID_RPM = 23,
      IMAGE_ID_TZ = 25,
      IMAGE_ID_DSP3 = 28,
      IMAGE_ID_ACDB = 29,
      IMAGE_ID_WDT = 30,
      IMAGE_ID_MBA = 31,
      IMAGE_ID_MDMDDR = 34,
      IMAGE_ID_XBLCONFIG = 38,
   };
   enum ClientCommandID
   {
      CMD_NOP = 0,
      CMD_SN_READ = 0x01,
      CMD_HWID_READ = 0x02,
      CMD_PKHASH_READ = 0x03,
      CMD_DEBUG_READ = 0x06,
      CMD_GET_SBL_VER = 0x07,
      CMD_GET_CMDID_LIST = 0x08,
      CMD_GET_TRAINING_DATA = 0x09,
      CMD_GET_FUSE_INFO = 0x0A,
      CMD_MAX = 0x7F
   };

   friend class Util::SharedPointer<Sahara>;
   virtual ~Sahara();

   virtual void connect(const int32_t clientId = 0);
   virtual void disconnect(const int32_t clientId = 0);
   virtual void reset(Direction dir);
   virtual Device::DataPacketPtr sendSync(
      const Device::SharedByteBufferPtr& pBuffer,
      const std::optional<std::chrono::milliseconds>& timeout = std::nullopt,
      bool bPriority = false
   );
   virtual TransactionId sendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority = false);
   virtual bool cancelTx(TransactionId transactionId);
   virtual void forceDisconnect();

   virtual void onAdd();
   virtual void onDrop();

   Mode getMode() const;
   void setMode(Mode mode, bool bNotify = true);
   Device::DeviceMode toDeviceMode(Mode mode);
   Device::SharedByteBufferPtr
   getFrame(const std::optional<std::chrono::milliseconds>& timeout = std::chrono::milliseconds(10000));

   /// Helper to get Util::SharedPointer<Sahara> from this object
   inline SaharaPtr getSaharaSharedPtr()
   {
      return SaharaPtr(std::static_pointer_cast<Sahara>(shared_from_this()));
   }
   void setFirehoseModeEnabled(const bool enabled);
   bool isFirehoseModeEnabled() const;
   void setCommandModeEnabled(const bool enabled);
   bool isCommandModeEnabled() const;
   bool isSaharaV3WarmResetEnabled() const;
   void setSaharaVersion(const uint32_t version);
   uint32_t getSaharaVersion() const;

   void doDisconnect();

protected:
   Sahara(const Communication::CommonIoPtr& pIo, Mode mode);
   virtual void initialize();
   virtual void finalize();

private:
   friend class SaharaRxWorker;
   typedef std::list<Device::SharedByteBufferPtr> ResponseQueue;

   volatile Mode m_initialMode;  ///< Initial sahara mode
   volatile Mode m_mode;         ///< Target operation mode
   ResponseQueue m_responses;    ///< Data read from CommonIo
   std::recursive_mutex m_mutex; ///< Protects m_responses
   Util::Event m_rxDataEvent;    ///< Event from CommonIo indicating data ready

   std::recursive_mutex m_connectMutex; ///< Mutex for connect/disconnect
   volatile bool m_bConnected;          ///< Whether the protocol is connected
   uint64_t m_dataLeftSend;             ///< How much left to send

   std::shared_ptr<SaharaRxWorker> m_pRxWork;
   std::shared_ptr<Util::StdThreadWrapper> m_pRxThread;

   volatile bool m_bFirehoseMode;
   volatile bool m_bCommandMode;
   volatile uint32_t m_saharaResetTimes;

   volatile uint32_t m_currentVersion; ///< Current Sahara Protocol Version
public:
   enum CommandId
   {
      SAHARA_INVALID = 0,
      SAHARA_HELLO,
      SAHARA_HELLO_RESP,
      SAHARA_READ_DATA,
      SAHARA_END_OF_IMAGE_TRANS,
      SAHARA_DONE,
      SAHARA_DONE_RESP,
      SAHARA_RESET,
      SAHARA_RESET_RESP,
      SAHARA_MEMORY_DEBUG,
      SAHARA_MEMORY_READ,
      SAHARA_COMMAND_READY,
      SAHARA_COMMAND_SWITCH_MODE,
      SAHARA_COMMAND_EXECUTE,
      SAHARA_COMMAND_EXECUTE_RESP,
      SAHARA_COMMAND_EXECUTE_DATA,
      SAHARA_64_BIT_MEMORY_DEBUG,
      SAHARA_64_BIT_MEMORY_READ,
      SAHARA_64_BIT_READ_DATA,
      SAHARA_RESET_SAHARA_STATE_MACHINE
   };

   struct FrameHeader
   {
      uint32_t m_command;
      uint32_t m_length;
   };
   TOOLS_STATIC_ASSERT(8 == TOOLS_SIZEOF(Sahara::FrameHeader));

   // -------------------------------------------------------------------------
   // Hello
   //
   // Payload for SAHARA_HELLO (Command 0x01)
   // -------------------------------------------------------------------------
   struct Hello
   {
      Sahara::FrameHeader m_header;
      uint32_t m_versionNumber;
      uint32_t m_versionCompatible;
      uint32_t m_commandPacketLength;
      uint32_t m_mode;
      uint32_t m_reserved[6];
   };
   TOOLS_STATIC_ASSERT(48 == TOOLS_SIZEOF(Hello));

   // -------------------------------------------------------------------------
   // HelloResponse
   //
   // Payload for SAHARA_HELLO_RESP (Command 0x02)
   // -------------------------------------------------------------------------
   struct HelloResponse
   {
      Sahara::FrameHeader m_header;
      uint32_t m_versionNumber;
      uint32_t m_versionCompatible;
      uint32_t m_status;
      uint32_t m_mode;
      uint32_t m_reserved[6];
   };
   TOOLS_STATIC_ASSERT(48 == TOOLS_SIZEOF(HelloResponse));

   // -------------------------------------------------------------------------
   // ReadData
   //
   // Payload for SAHARA_READ_DATA (Command 0x03)
   // -------------------------------------------------------------------------
   struct ReadData
   {
      Sahara::FrameHeader m_header;
      uint32_t m_imageId;
      uint32_t m_dataOffset;
      uint32_t m_dataLength;
   };
   TOOLS_STATIC_ASSERT(20 == TOOLS_SIZEOF(ReadData));

   // -------------------------------------------------------------------------
   // EndOfImageTransfer
   //
   // Payload for SAHARA_END_OF_IMAGE_TRANS (Command 0x04)
   // -------------------------------------------------------------------------
   struct EndOfImageTransfer
   {
      Sahara::FrameHeader m_header;
      uint32_t m_imageId;
      uint32_t m_status;
   };
   TOOLS_STATIC_ASSERT(16 == TOOLS_SIZEOF(EndOfImageTransfer));

   // -------------------------------------------------------------------------
   // Done
   //
   // Payload for SAHARA_DONE (Command 0x05)
   // -------------------------------------------------------------------------
   struct Done
   {
      Sahara::FrameHeader m_header;
   };
   TOOLS_STATIC_ASSERT(8 == TOOLS_SIZEOF(Done));

   // -------------------------------------------------------------------------
   // DoneResponse
   //
   // Payload for SAHARA_DONE_RESP (Command 0x06)
   // -------------------------------------------------------------------------
   struct DoneResponse
   {
      Sahara::FrameHeader m_header;
      uint32_t m_imageTransferStatus;
   };
   TOOLS_STATIC_ASSERT(12 == TOOLS_SIZEOF(DoneResponse));

   // -------------------------------------------------------------------------
   // Reset
   //
   // Payload for SAHARA_RESET (Command 0x07)
   // -------------------------------------------------------------------------
   struct Reset
   {
      Sahara::FrameHeader m_header;
   };
   TOOLS_STATIC_ASSERT(8 == TOOLS_SIZEOF(Reset));

   // -------------------------------------------------------------------------
   // ResetResponse
   //
   // Payload for SAHARA_RESET_RESP (Command 0x08)
   // -------------------------------------------------------------------------
   struct ResetResponse
   {
      Sahara::FrameHeader m_header;
   };
   TOOLS_STATIC_ASSERT(8 == TOOLS_SIZEOF(ResetResponse));

   // -------------------------------------------------------------------------
   // MemoryDebug
   //
   // Payload for SAHARA_MEMORY_DEBUG (Command 0x09)
   // -------------------------------------------------------------------------
   struct MemoryDebug
   {
      Sahara::FrameHeader m_header;
      uint32_t m_memoryTableAddress;
      uint32_t m_memoryTableLength;
   };
   TOOLS_STATIC_ASSERT(16 == TOOLS_SIZEOF(MemoryDebug));

   // -------------------------------------------------------------------------
   // MemoryRead
   //
   // Payload for SAHARA_MEMORY_READ (Command 0x0A)
   // -------------------------------------------------------------------------
   struct MemoryRead
   {
      Sahara::FrameHeader m_header;
      uint32_t m_memoryAddress;
      uint32_t m_memoryLength;
   };
   TOOLS_STATIC_ASSERT(16 == TOOLS_SIZEOF(MemoryRead));

   // -------------------------------------------------------------------------
   // CommandReady
   //
   // Payload for SAHARA_COMMAND_READY (Command 0x0B)
   // -------------------------------------------------------------------------
   struct CommandReady
   {
      Sahara::FrameHeader m_header;
   };
   TOOLS_STATIC_ASSERT(8 == TOOLS_SIZEOF(CommandReady));

   // -------------------------------------------------------------------------
   // CommandSwitchMode
   //
   // Payload for SAHARA_COMMAND_SWITCH_MODE (Command 0x0C)
   // -------------------------------------------------------------------------
   struct CommandSwitchMode
   {
      Sahara::FrameHeader m_header;
      uint32_t m_mode;
   };
   TOOLS_STATIC_ASSERT(12 == TOOLS_SIZEOF(CommandSwitchMode));

   // -------------------------------------------------------------------------
   // CommandExecute
   //
   // Payload for SAHARA_COMMAND_EXECUTE (Command 0x0D)
   // -------------------------------------------------------------------------
   struct CommandExecute
   {
      Sahara::FrameHeader m_header;
      uint32_t m_clientCommand;
   };
   TOOLS_STATIC_ASSERT(12 == TOOLS_SIZEOF(CommandExecute));

   // -------------------------------------------------------------------------
   // CommandExecuteResp
   //
   // Payload for SAHARA_COMMAND_EXECUTE_RESP (Command 0x0E)
   // -------------------------------------------------------------------------
   struct CommandExecuteResp
   {
      Sahara::FrameHeader m_header;
      uint32_t m_clientCommand;
      uint32_t m_responseLength;
   };
   TOOLS_STATIC_ASSERT(16 == TOOLS_SIZEOF(CommandExecuteResp));

   // -------------------------------------------------------------------------
   // CommandExecuteData
   //
   // Payload for SAHARA_COMMAND_EXECUTE_DATA (Command 0x0F)
   // -------------------------------------------------------------------------
   struct CommandExecuteData
   {
      Sahara::FrameHeader m_header;
      uint32_t m_clientCommand;
   };
   TOOLS_STATIC_ASSERT(12 == TOOLS_SIZEOF(CommandExecuteData));

   // -------------------------------------------------------------------------
   // MemoryDebug64Bit
   //
   // Payload for SAHARA_64_BIT_MEMORY_DEBUG (Command 0x10)
   // -------------------------------------------------------------------------
   struct MemoryDebug64Bit
   {
      Sahara::FrameHeader m_header;
      uint64_t m_memoryTableAddress;
      uint64_t m_memoryTableLength;
   };
   TOOLS_STATIC_ASSERT(24 == TOOLS_SIZEOF(MemoryDebug64Bit));

   // -------------------------------------------------------------------------
   // MemoryRead64Bit
   //
   // Payload for SAHARA_64_BIT_MEMORY_READ (Command 0x11)
   // ------------------------------------------------------------------------
   struct MemoryRead64Bit
   {
      Sahara::FrameHeader m_header;
      uint64_t m_memoryAddress;
      uint64_t m_memoryLength;
   };
   TOOLS_STATIC_ASSERT(24 == TOOLS_SIZEOF(MemoryRead64Bit));

   // -------------------------------------------------------------------------
   // ReadData64Bit
   //
   // Payload for SAHARA_64_BIT_READ_DATA (Command 0x012)
   // -------------------------------------------------------------------------
   struct ReadData64Bit
   {
      Sahara::FrameHeader m_header;
      uint64_t m_imageId;
      uint64_t m_dataOffset;
      uint64_t m_dataLength;
   };
   TOOLS_STATIC_ASSERT(32 == TOOLS_SIZEOF(ReadData64Bit));

   // -------------------------------------------------------------------------
   // ResetStateMachine
   //
   // Payload for SAHARA_RESET_SAHARA_STATE_MACHINE (Command 0x13)
   // -------------------------------------------------------------------------
   struct ResetStateMachine
   {
      Sahara::FrameHeader m_header;
   };
   TOOLS_STATIC_ASSERT(8 == TOOLS_SIZEOF(ResetStateMachine));

   template <typename _Type>
   Device::SharedByteBufferPtr createCommand(Device::Protocol::Sahara::CommandId commandId) const
   {
      Device::SharedByteBufferPtr pBuffer = Device::Buffer::createBuffer(TOOLS_SIZEOF(_Type));
      _Type* pCommand = Util::buffer_cast<_Type*>(pBuffer->begin(), pBuffer->size());
      pCommand->m_header.m_command = commandId;
      pCommand->m_header.m_length = TOOLS_SIZEOF(_Type);

      return pBuffer;
   }
};

} // namespace Protocol
} // namespace Device

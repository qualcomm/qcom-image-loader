// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/DataPacket.h"
#include "protocol/Base.h"
#include "report/DataContainer.h"
#include "util/Event.h"

#include <chrono>
#include <deque>
#include <mutex>
#include <optional>

namespace Device {
namespace Protocol {

// ----------------------------------------------------------------------------
// FirehoseEvent
//
/// Notifies differnt stages of the firehose transfer
// ----------------------------------------------------------------------------
class FirehoseEvent : public Util::Event
{
   TOOLS_FORBID_COPY(FirehoseEvent);

public:
   enum EventId
   {
      INITIALIZE = 0,
      FIREHOSE_PROGRESSION,
      FIREHOSE_TRANSFER_RAW_PROGRAMMER,
      FIREHOSE_TRANSFER_PATCH_PROGRAMMER,
      FIREHOSE_UFS_PROVISIONING,
      FIREHOSE_SELECT_PARTITION,
      FIREHOSE_RESETTING,
      FIREHOSE_ERASE,
      FIREHOSE_READ_IMAGE_FILES,
      FIREHOSE_GET_STORAGE_INFO,
      FIREHOSE_CREATE_DIGESTS,
      FIREHOSE_VIP_DOWNLOAD,
      FIREHOSE_THROUGHPUT,

      // below event are to be added as informatiom
      FIREHOSE_TARGET_SAID = 0x80,
   };

   FirehoseEvent(EventId eventId, const std::string& description);
   virtual ~FirehoseEvent();

   EventId getEventId() const;
   std::string getDescription() const;

private:
   EventId m_eventId;
   std::string m_description;
};

// ----------------------------------------------------------------------------
// Firehose Protocol
//
/// Interface for Firehose protocol
// ----------------------------------------------------------------------------
class Firehose
: public Base
, public Report::DataContainer
{
   TOOLS_FORBID_COPY(Firehose);

public:
   typedef std::vector<std::string> FirehoseCommandType;

   typedef struct
   {
      std::string totalBlocks;
      std::string blockSize;
      std::string pageSize;
      std::string numPhysicalPartitions;
      std::string manufacturerId;
      std::string serialNum;
      std::string fwVersion;
      std::string memType;
      std::string prodName;
      std::string specVersion;
      bool isSet;

   } StorageInfoType;

   friend class Util::SharedPointer<Firehose>;
   virtual ~Firehose();

   void setIo(const Communication::CommonIoPtr& pIo);
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

   void getBytes(uint8_t* pDestination, uint32_t bytesToRead, uint32_t& bytesRead);

   void processCommand(
      FirehoseCommandType& pParameters,
      const std::filesystem::path& readImagePath = std::filesystem::path(),
      const std::optional<std::chrono::milliseconds>& rxTimeout = std::chrono::milliseconds(0)
   );
   void getStorageInfo(StorageInfoType& info);
   void handleFirehoseLoaderError(void);
   bool waitForData(const std::optional<std::chrono::milliseconds>& timeout);

   /// Helper to get Util::SharedPointer<Firehose> from this object
   inline FirehosePtr getFirehoseSharedPtr()
   {
      return FirehosePtr(std::static_pointer_cast<Firehose>(shared_from_this()));
   }

protected:
   Firehose(const Communication::CommonIoPtr& pIo, Device::DeviceHandle handle);
   virtual void initialize();

private:
   void onFirehoseEvent(Device::Protocol::FirehoseEvent* pEvent);

   typedef std::list<Device::SharedByteBufferPtr> ResponseQueue;

   void receiveData(const Device::SharedByteBufferPtr& pBuffer);

   ResponseQueue m_responses;    ///< Data read from CommonIo
   uint32_t m_responseOffset;    ///< Offset of partially read response
   std::recursive_mutex m_mutex; ///< Protects m_responses
   Util::Event m_rxDataEvent;    ///< Event from CommonIo indicating data ready

   std::recursive_mutex m_connectMutex; ///< Mutex for connect/disconnect
   bool m_bConnected;                   ///< Whether the protocol is connected
   uint64_t m_dataLeftSend;             ///< How much left to send
   uint64_t m_dataLeftReceive;          ///< How much left to receive

   FirehoseLoaderPtr m_pFirehoseLoader; ///< Firehose Loader to be reused

   Util::Event m_flowControlEvent;
};

} // namespace Protocol
} // namespace Device

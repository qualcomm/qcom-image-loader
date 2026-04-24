// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Fwd.h"
#include "protocol/Fwd.h"

#include <chrono>
#include <string>

namespace Device {

class DataPacket;
typedef std::shared_ptr<DataPacket> DataPacketPtr;

// ----------------------------------------------------------------------------
// DataPacket
//
/// A wrapper for the buffer that includes a use case specific ID number and
/// index
// ----------------------------------------------------------------------------
class DataPacket
{
   TOOLS_FORBID_COPY(DataPacket);

public:
   enum Direction
   {
      DIR_TX = 0,
      DIR_RX,
   };

   DataPacket(
      const Device::SharedByteBufferPtr& pBuffer,
      uint64_t id,
      size_t protocolIndex,
      size_t globalIndex,
      const std::chrono::system_clock::time_point& receiveTime,
      Protocol::Handle protocolHandle,
      const ProtocolType& protocolType,
      const Device::DataPacket::Direction& direction
   );

   virtual ~DataPacket();

   virtual Device::SharedByteBufferPtr getPayload() const;
   virtual size_t getPayloadSize() const;
   virtual std::chrono::system_clock::time_point getReceiveTime() const;
   virtual Device::DataPacket::Direction getDirection() const;
   virtual size_t getTimeSortedIndex() const;


   virtual uint64_t getId() const
   {
      return m_id;
   }

   inline size_t getProtocolIndex() const
   {
      return m_index;
   }

   inline size_t getGlobalIndex() const
   {
      return m_globalIndex;
   }

   inline Protocol::Handle getProtocolHandle() const
   {
      return m_protocolHandle;
   }

   inline ProtocolType getProtocolType() const
   {
      return m_protocolType;
   }

   inline void setTransactionId(uint64_t transactionId, bool bFinalResponse = true)
   {
      m_transactionId = transactionId;
      m_bFinalResponse = bFinalResponse;
   }

   inline uint64_t getTransactionId()
   {
      return m_transactionId;
   }

   bool isFinalResponse()
   {
      return m_bFinalResponse;
   }


protected:
   mutable Device::SharedByteBufferPtr m_pPayload;      ///< Actual payload
   uint64_t m_id;                                       ///<  Packet id
   size_t m_index;                                      ///< Index it was received relative to this protocol
   size_t m_globalIndex;                                ///<  Index it was received across all protocols
   std::chrono::system_clock::time_point m_receiveTime; ///< Time the packet arrived on the host
   Protocol::Handle m_protocolHandle;                   ///< Protocol the packet came from
   ProtocolType m_protocolType;                         ///< Type of protocol m_protocolHandle is
   Device::DataPacket::Direction m_direction;           ///< Packet direction.
   uint64_t m_transactionId;                            ///< Transaction id of the packet
   bool m_bFinalResponse;                               ///< Whether this is the last packet
};

} // namespace Device

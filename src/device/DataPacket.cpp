// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "device/DataPacket.h"

#include "device/Buffer.h"
#include "device/Exception.h"
#include "device/Manager.h"

#include <string>

namespace Device {

// ----------------------------------------------------------------------------
// DataPacket
//
// ----------------------------------------------------------------------------
DataPacket::DataPacket(
   const Device::SharedByteBufferPtr& pBuffer,
   uint64_t id,
   size_t protocolIndex,
   size_t globalIndex,
   const std::chrono::system_clock::time_point& receiveTime,
   Device::Protocol::Handle protocolHandle,
   const ProtocolType& protocolType,
   const Device::DataPacket::Direction& direction
)
: m_pPayload(pBuffer)
, m_id(id)
, m_index(protocolIndex)
, m_globalIndex(globalIndex)
, m_receiveTime(receiveTime)
, m_protocolHandle(protocolHandle)
, m_protocolType(protocolType)
, m_direction(direction)
, m_transactionId(Device::Protocol::Base::NULL_TRANSACTION_ID)
, m_bFinalResponse(true)
{
}

// ----------------------------------------------------------------------------
// ~DataPacket
//
// ----------------------------------------------------------------------------
DataPacket::~DataPacket()
{
}

// ----------------------------------------------------------------------------
// getDirection
//
/// @returns Tx or Rx
// ----------------------------------------------------------------------------
Device::DataPacket::Direction DataPacket::getDirection() const
{
   return m_direction;
}

} // namespace Device

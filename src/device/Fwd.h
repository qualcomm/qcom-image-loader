// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "Definitions.h"
#include "util/MemoryHelper.h"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// ----------------------------------------------------------------------------
// Device
//
/// The Device library contains the communication layers and protocols for
/// connecting to a device
// ----------------------------------------------------------------------------
namespace Device {

class SharedByteBuffer;
typedef std::shared_ptr<SharedByteBuffer> SharedByteBufferPtr;

class Buffer;
typedef std::shared_ptr<Buffer> BufferPtr;

class Connection;
typedef Util::SharedPointer<Connection> ConnectionPtr;

typedef QC::ConnectionType ConnectionType;

class DataPacket;
typedef std::shared_ptr<DataPacket> DataPacketPtr;

class Impl;
typedef std::shared_ptr<Impl> ImplPtr;

typedef int64_t DeviceHandle;

class Manager;
typedef Util::SharedPointer<Manager> ManagerPtr;

typedef QC::ProtocolType ProtocolType;

} // namespace Device

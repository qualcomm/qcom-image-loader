// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "util/MemoryHelper.h"

#include <memory>

namespace Device {

// ----------------------------------------------------------------------------
// Protocol
//
/// Contains the impelementation for different data protocols used in
/// communicating with the device
// ----------------------------------------------------------------------------
namespace Protocol {

class Base;
typedef Util::SharedPointer<Base> BasePtr;

typedef int64_t Handle;

class Firehose;
typedef Util::SharedPointer<Firehose> FirehosePtr;

class FirehoseLoader;
typedef std::shared_ptr<FirehoseLoader> FirehoseLoaderPtr;

class Sahara;
typedef Util::SharedPointer<Sahara> SaharaPtr;

} // namespace Protocol
} // namespace Device

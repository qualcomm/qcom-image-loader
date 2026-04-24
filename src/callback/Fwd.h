// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "callback/ClientCallbackHandler.h"
#include "util/MemoryHelper.h"

// ----------------------------------------------------------------------------
// Device
//
/// The Device library contains the communication layers and protocols for
/// connecting to a device
// ----------------------------------------------------------------------------
namespace QC {

class ClientCallbackHandler;
typedef Util::SharedPointer<ClientCallbackHandler> ClientCallbackHandlerPtr;

} // namespace QC

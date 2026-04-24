// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <memory>

// ----------------------------------------------------------------------------
// Function
//
/// The Function library contains the definition for functional level
/// capabilities in communicating with a device
// ----------------------------------------------------------------------------
namespace Function {

class ImageTransfer;
typedef std::shared_ptr<ImageTransfer> ImageTransferPtr;

class ImageTransferEvent;

} // namespace Function

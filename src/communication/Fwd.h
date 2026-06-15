// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <memory>

namespace Device {

// ----------------------------------------------------------------------------
// Communication
//
/// Contains the impelementation for different physical communication mechanisms
// ----------------------------------------------------------------------------
namespace Communication {

class CommonIo;
typedef std::shared_ptr<CommonIo> CommonIoPtr;

class Usb;
typedef std::shared_ptr<Usb> UsbPtr;

} // namespace Communication
} // namespace Device

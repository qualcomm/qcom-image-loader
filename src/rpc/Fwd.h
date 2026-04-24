// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <memory>

namespace Rpc {

class DeviceManagerHandler;

class ServiceHandlerBase;
typedef std::shared_ptr<ServiceHandlerBase> ServiceHandlerBasePtr;

} // namespace Rpc

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Fwd.h"
#include "device/Logger.h"

#include <memory>

namespace Device {

// ----------------------------------------------------------------------------
// Report
//
/// Contains the declaration of all items used for reporting application health
// ----------------------------------------------------------------------------
namespace Report {


class DataContainer;

class StatusManager;
typedef std::shared_ptr<StatusManager> StatusManagerPtr;

namespace Thread {
class Work;
}

} // namespace Report
} // namespace Device

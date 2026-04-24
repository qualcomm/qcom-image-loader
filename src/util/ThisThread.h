// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "Event.h"

#include <chrono>
#include <optional>
#include <vector>

namespace Util {

using ThreadId = unsigned int;
using EventList = std::vector<Util::Event*>;
// ----------------------------------------------------------------------------
// ThisThread
// ----------------------------------------------------------------------------
class ThisThread
{
public:
   enum WaitType
   {
      WAIT_FOR_ANY = 0,
      WAIT_FOR_ALL = 1
   };

   enum WaitResult
   {
      WAIT_RESULT_SUCCESS = 0,
      WAIT_RESULT_UNKNOWN = 0xFFFFFFFF
   };

   static ThreadId getId();
   static void sleep(const std::optional<std::chrono::milliseconds>& duration);
   static bool
   waitForEvent(const Util::Event* pEvent, const std::optional<std::chrono::milliseconds>& timeout = std::nullopt);
   static void waitForMultipleEvents(
      const EventList& events,
      const WaitType waitType = WAIT_FOR_ANY,
      const std::optional<std::chrono::milliseconds>& timeout = std::nullopt
   );
};

} // namespace Util

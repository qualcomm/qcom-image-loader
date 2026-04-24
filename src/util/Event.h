// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>

namespace Util {

// --------------------------------------------------------------------------
// Event
//
/// System even that allows efficient synchronization between threads
// --------------------------------------------------------------------------
class Event
{
   Event(const Event&) = delete;
   Event& operator=(const Event&) = delete;

public:
   using Handle = void*;

   explicit Event(bool bManualReset = false, bool bInitialState = false);
   virtual ~Event();

   void signal();
   void reset();
   bool wait(const std::optional<std::chrono::milliseconds>& timeout = std::nullopt) const;
   bool isSignaled() const noexcept
   {
      return m_signaled;
   }


   void registerNotifier(std::condition_variable_any* cv) const;
   void unregisterNotifier(std::condition_variable_any* cv) const;

private:
   mutable std::mutex m_mtx;
   mutable std::condition_variable_any m_cv;
   bool m_manualReset;
   bool m_signaled;

   mutable std::vector<std::condition_variable_any*> m_notifiers;
};
} // namespace Util

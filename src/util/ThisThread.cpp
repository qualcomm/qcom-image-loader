// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "ThisThread.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <thread>


namespace Util {

// --------------------------------------------------------------------------
// getId
//
// --------------------------------------------------------------------------
ThreadId ThisThread::getId()
{
   std::hash<std::thread::id> hasher;
   return static_cast<ThreadId>(hasher(std::this_thread::get_id()));
}

// --------------------------------------------------------------------------
// sleep
//
/// Tells this thread to sleep for a given duration
// --------------------------------------------------------------------------
void ThisThread::sleep(const std::optional<std::chrono::milliseconds>& duration)
{
   if(!duration.has_value()) return; // nothing to do
   std::this_thread::sleep_for(duration.value());
}

// --------------------------------------------------------------------------
// waitForEvent
//
/// Waits for the given event to signal
/// Return true if event is signaled
// --------------------------------------------------------------------------
bool ThisThread::waitForEvent(const Util::Event* pEvent, const std::optional<std::chrono::milliseconds>& timeout)
{
   if(!pEvent)
   {
      throw std::invalid_argument("CurrentThread::waitForEvent: pEvent is nullptr");
   }
   return pEvent->wait(timeout) ? WAIT_RESULT_SUCCESS : WAIT_RESULT_UNKNOWN;
}
// --------------------------------------------------------------------------
// waitForMultipleEvents
//
/// Waits for multiple events to signal
// --------------------------------------------------------------------------
void ThisThread::waitForMultipleEvents(
   const EventList& events,
   WaitType waitType,
   const std::optional<std::chrono::milliseconds>& timeout
)
{
   if(events.empty()) return;

   auto anySignaled = [&events]() -> bool {
      return std::any_of(events.begin(), events.end(), [](const Util::Event* ev) { return ev->isSignaled(); });
   };

   auto allSignaled = [&events]() -> bool {
      return std::all_of(events.begin(), events.end(), [](const Util::Event* ev) { return ev->isSignaled(); });
   };

   static std::mutex sharedMtx;
   static std::condition_variable_any sharedCv;

   for(auto* ev: events)
      ev->registerNotifier(&sharedCv);

   std::unique_lock<std::mutex> lk(sharedMtx);

   std::function<bool()> predicate =
      (waitType == WAIT_FOR_ANY) ? std::function<bool()>(anySignaled) : std::function<bool()>(allSignaled);

   if(timeout.has_value())
      sharedCv.wait_for(lk, timeout.value(), predicate);
   else
      sharedCv.wait(lk, predicate);

   for(auto* ev: events)
      ev->unregisterNotifier(&sharedCv);
}

} // namespace Util

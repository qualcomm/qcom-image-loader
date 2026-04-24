// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "Event.h"

#include <algorithm>
#include <stdexcept>

namespace Util {

Event::Event(bool bManualReset, bool bInitialState)
: m_manualReset(bManualReset)
, m_signaled(bInitialState)
{
}

Event::~Event() = default;

/*--------------------------------------------------------------
 *  signal / reset – standard condition‑variable semantics
 *--------------------------------------------------------------*/
void Event::signal()
{
   std::lock_guard<std::mutex> lk(m_mtx);
   m_signaled = true;
   m_cv.notify_all();

   // Notify any external notifiers (used by waitForMultipleEvents)
   for(auto* extCv: m_notifiers)
      extCv->notify_all();
}

void Event::reset()
{
   std::lock_guard<std::mutex> lk(m_mtx);
   m_signaled = false;
}

/*--------------------------------------------------------------
 *  wait – blocks until signaled or timeout expires
 *--------------------------------------------------------------*/
bool Event::wait(const std::optional<std::chrono::milliseconds>& timeout) const
{
   std::unique_lock<std::mutex> lk(m_mtx);
   if(timeout.has_value())
   {
      return m_cv.wait_for(lk, timeout.value(), [this] { return m_signaled; });
   }
   else
   {
      m_cv.wait(lk, [this] { return m_signaled; });
      return true;
   }
}

void Event::registerNotifier(std::condition_variable_any* cv) const
{
   if(!cv) return;
   std::lock_guard<std::mutex> lk(m_mtx);
   m_notifiers.push_back(cv);
}

void Event::unregisterNotifier(std::condition_variable_any* cv) const
{
   if(!cv) return;
   std::lock_guard<std::mutex> lk(m_mtx);
   m_notifiers.erase(std::remove(m_notifiers.begin(), m_notifiers.end(), cv), m_notifiers.end());
}

} // namespace Util

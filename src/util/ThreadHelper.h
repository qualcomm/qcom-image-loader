// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <atomic>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "Event.h"

namespace Util {
// Forward declaration
class StdThreadWrapper;

// ----------------------------------------------------------------------------
// StdThreadWrapper
//
/// Wrapper to manage std::thread with StdThreadWork
// ----------------------------------------------------------------------------
class StdThreadWrapper : public std::enable_shared_from_this<StdThreadWrapper>
{
public:
   enum State
   {
      CREATED = 0,
      RUNNING = 1,
      SUSPENDED = 2,
      STOPPING = 3,
      STOPPED = 4
   };

   enum class HealthMonitorPriority
   {
      Low = 0,
      Normal = 1,
      Medium = 2,
      Critical = 3
   };

   // ----------------------------------------------------------------------------
   // StdThreadWork
   //
   // ----------------------------------------------------------------------------
   class Work
   {
   public:
      Work()
      : m_pThread(NULL)
      , m_bStop(false)
      {
      }

      Work(
         const std::string& threadName,
         const HealthMonitorPriority healthMonitorPriority = HealthMonitorPriority::Normal
      )
      : m_pThread(NULL)
      , m_bStop(false)
      {
         (void)threadName;            // Suppress unused parameter warning
         (void)healthMonitorPriority; // Suppress unused parameter warning
      }

      virtual ~Work()
      {
         int a = 1;
         a++;
      }

      std::shared_ptr<StdThreadWrapper> getThread() const
      {
         return m_pThread;
      }
      virtual bool isStopSignaled() const
      {
         return m_bStop.load();
      }

      void stop()
      {
         m_bStop.store(true);
         onStop();
      }

      virtual void onStop()
      {
         // Virtual method can be overridden for custom actions when stop is
         // called
      }

      // Called by thread wrapper to run the work
      virtual void run(std::shared_ptr<StdThreadWrapper> pThread)
      {
         m_pThread = pThread;

         try
         {
            setState(StdThreadWrapper::RUNNING);
            // Notify that thread has started and m_pThread is initialized
            m_pThread->notifyThreadStarted();
            onRun();
         }
         catch(...)
         {
            setState(StdThreadWrapper::STOPPED);
            throw;
         }

         setState(StdThreadWrapper::STOPPED);
      }

      State getState() const
      {
         return m_pThread->getState();
      }

   protected:
      friend class StdThreadWrapper;

      virtual void onRun() = 0;

      void setState(StdThreadWrapper::State state)
      {
         std::lock_guard<std::mutex> lock(m_pThread->m_stateMutex);
         m_pThread->m_state = state;
      }

      std::shared_ptr<StdThreadWrapper> m_pThread;

   private:
      std::atomic<bool> m_bStop;
   };
   // ----------------------------------------------------------------------------
   // end StdThreadWork
   //
   // ----------------------------------------------------------------------------


   StdThreadWrapper(std::shared_ptr<Work> pWork)
   : m_pWork(pWork)
   , m_pThread(nullptr)
   , m_state(CREATED)
   , m_id(UINT32_MAX) // Use UINT32_MAX instead of -1 to avoid signed/unsigned
   , m_startEvent(false, false) // auto-reset, not signaled initially
   {
   }

   ~StdThreadWrapper()
   {
      if(m_pThread)
      {
         if(m_pThread->joinable())
         {
            m_pThread->join();
         }
         m_pThread = nullptr;
      }
   }
   void start()
   {
      if(m_pWork)
      {
         m_pWork->m_bStop = false;
      }

      // Reset the event before starting the thread
      m_startEvent.reset();

      // Capture shared_ptr to this object before creating the thread
      auto self = shared_from_this();
      m_pThread = std::make_shared<std::thread>([this, self]() { threadProcessor(self); });

      // Wait for thread to actually start
      m_startEvent.wait();
   }

   void threadProcessor(std::shared_ptr<StdThreadWrapper> self)
   {
      m_pWork->run(self);
   }

   void stop()
   {
      if(!m_pWork)
      {
         return;
      }

      // Check if already stopped or stopping
      State currentState = getState();
      if(currentState == STOPPED || currentState == STOPPING)
      {
         return;
      }

      // Set state to STOPPING
      m_pWork->setState(STOPPING);

      // Call work's stop method
      m_pWork->stop();
   }

   void waitForStop()
   {
      if(m_pThread && m_pThread->joinable())
      {
         m_pThread->join();
      }
   }

   State getState() const
   {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      return m_state;
   }

   uint32_t getId() const
   {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      return m_id;
   }

   void notifyThreadStarted()
   {
      m_startEvent.signal();
   }

private:
   std::shared_ptr<Work> m_pWork;
   uint32_t m_id;
   std::shared_ptr<std::thread> m_pThread;
   volatile State m_state;
   mutable std::mutex m_stateMutex;
   Event m_startEvent;
};

} // namespace Util
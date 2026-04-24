// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <algorithm>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <typeindex>
#include <util/Event.h>
#include <vector>
namespace Util {

class AsyncEvent : public std::enable_shared_from_this<AsyncEvent>
{
protected:
   AsyncEvent() = default;
   AsyncEvent(const AsyncEvent&) = default;
   AsyncEvent& operator=(const AsyncEvent&)
   {
      return *this;
   }
   virtual ~AsyncEvent() = default;
};

struct DelegateKey
{
   void* obj = nullptr;
   std::vector<uint8_t> funcBytes;

   bool operator==(const DelegateKey& o) const noexcept
   {
      return obj == o.obj && funcBytes == o.funcBytes;
   }
   bool operator!=(const DelegateKey& o) const noexcept
   {
      return !(*this == o);
   }
};

template <typename C, typename E>
inline DelegateKey makeDelegateKey(C* obj, void (C::*method)(E*))
{
   DelegateKey key;
   key.obj = static_cast<void*>(obj);
   key.funcBytes.resize(sizeof(method));
   std::memcpy(key.funcBytes.data(), &method, sizeof(method));
   return key;
}

template <typename C, typename E>
inline DelegateKey makeDelegateKey(const C* obj, void (C::*method)(E*) const)
{
   DelegateKey key;
   key.obj = const_cast<void*>(static_cast<const void*>(obj));
   key.funcBytes.resize(sizeof(method));
   std::memcpy(key.funcBytes.data(), &method, sizeof(method));
   return key;
}

template <typename EventT = Event>
class Publisher
{
public:
   using EventPtr = std::shared_ptr<EventT>;

   explicit Publisher(bool bMulti = true)
   : m_bMulti(bMulti)
   {
   }
   virtual ~Publisher() = default;

   // ── Subscribe ────────────────────────────────────────────────────────
   template <typename R, typename E, typename C>
   bool subscribe(R* pObj, void (C::*method)(E*))
   {
      auto key = makeDelegateKey(pObj, method);
      auto wrapper = [pObj, method](EventT* pEvent) {
         if(auto* pE = dynamic_cast<E*>(pEvent))
         {
            (pObj->*method)(pE);
         }
      };
      return doSubscribe(std::type_index(typeid(E)), key, std::move(wrapper));
   }

   template <typename R, typename E, typename C>
   bool subscribe(const R* pObj, void (C::*method)(E*) const)
   {
      auto key = makeDelegateKey(pObj, method);
      auto wrapper = [pObj, method](EventT* pEvent) {
         if(auto* pE = dynamic_cast<E*>(pEvent))
         {
            (pObj->*method)(pE);
         }
      };
      return doSubscribe(std::type_index(typeid(E)), key, std::move(wrapper));
   }

   // ── Unsubscribe ──────────────────────────────────────────────────────
   template <typename R, typename E, typename C>
   void unsubscribe(R* pObj, void (C::*method)(E*))
   {
      doUnsubscribe(std::type_index(typeid(E)), makeDelegateKey(pObj, method));
   }

   template <typename R, typename E, typename C>
   void unsubscribe(const R* pObj, void (C::*method)(E*) const)
   {
      doUnsubscribe(std::type_index(typeid(E)), makeDelegateKey(pObj, method));
   }

   void notify(const EventPtr& pEvent)
   {
      std::vector<std::function<void(EventT*)>> toCall;
      {
         std::lock_guard<std::mutex> lock(m_mutex);
         auto it = m_subscribers.find(std::type_index(typeid(*pEvent)));
         if(it != m_subscribers.end())
            for(const auto& h: it->second)
               toCall.push_back(h.second);
      }
      for(auto& fn: toCall)
         fn(pEvent.get());
   }

   size_t getSubscriberCount() const
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      size_t n = 0;
      for(const auto& [ti, v]: m_subscribers)
         n += v.size();
      return n;
   }

protected:
   bool m_bRunning = false; ///< kept for API compatibility with original

private:
   using Handler = std::pair<DelegateKey, std::function<void(EventT*)>>;

   bool doSubscribe(std::type_index ti, const DelegateKey& key, std::function<void(EventT*)> wrapper)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto& handlers = m_subscribers[ti];
      for(const auto& h: handlers)
         if(h.first == key) return true; // already subscribed
      if(!m_bMulti && !handlers.empty()) return false;
      handlers.push_back({key, std::move(wrapper)});
      return true;
   }

   void doUnsubscribe(std::type_index ti, const DelegateKey& key)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = m_subscribers.find(ti);
      if(it == m_subscribers.end()) return;
      auto& handlers = it->second;
      handlers.erase(
         std::remove_if(handlers.begin(), handlers.end(), [&key](const Handler& h) { return h.first == key; }),
         handlers.end()
      );
   }

   bool m_bMulti;
   std::map<std::type_index, std::vector<Handler>> m_subscribers;
   mutable std::mutex m_mutex;
};

/// Matches the original Util::EventPublisher typedef
using EventPublisher = Publisher<Event>;

template <typename EventT = AsyncEvent>
class AsyncPublisher
{
public:
   using EventPtr = std::shared_ptr<EventT>;

   explicit AsyncPublisher(bool bMulti = true)
   : m_bMulti(bMulti)
   {
   }
   virtual ~AsyncPublisher() = default;

   // ── Subscribe ────────────────────────────────────────────────────────
   template <typename R, typename E, typename C>
   bool subscribeForAsyncEvents(R* pObj, void (C::*method)(E*))
   {
      auto key = makeDelegateKey(pObj, method);
      auto wrapper = [pObj, method](EventT* pEvent) {
         if(auto* pE = dynamic_cast<E*>(pEvent))
         {
            (pObj->*method)(pE);
         }
      };
      return doSubscribe(std::type_index(typeid(E)), key, std::move(wrapper));
   }

   template <typename R, typename E, typename C>
   bool subscribeForAsyncEvents(const R* pObj, void (C::*method)(E*) const)
   {
      auto key = makeDelegateKey(pObj, method);
      auto wrapper = [pObj, method](EventT* pEvent) {
         if(auto* pE = dynamic_cast<E*>(pEvent))
         {
            (pObj->*method)(pE);
         }
      };
      return doSubscribe(std::type_index(typeid(E)), key, std::move(wrapper));
   }

   // ── Unsubscribe ──────────────────────────────────────────────────────
   template <typename R, typename E, typename C>
   void unsubscribeAsyncEvents(R* pObj, void (C::*method)(E*))
   {
      doUnsubscribe(std::type_index(typeid(E)), makeDelegateKey(pObj, method));
   }

   template <typename R, typename E, typename C>
   void unsubscribeAsyncEvents(const R* pObj, void (C::*method)(E*) const)
   {
      doUnsubscribe(std::type_index(typeid(E)), makeDelegateKey(pObj, method));
   }

   // ── Dispatch ─────────────────────────────────────────────────────────
   void notifyAsync(const EventPtr& pEvent)
   {
      std::vector<std::function<void(EventT*)>> toCall;
      {
         std::lock_guard<std::mutex> lock(m_mutex);
         auto it = m_subscribers.find(std::type_index(typeid(*pEvent)));
         if(it != m_subscribers.end())
            for(const auto& h: it->second)
               toCall.push_back(h.second);
      }
      for(auto& fn: toCall)
         fn(pEvent.get());
   }

   size_t getAsyncSubscriberCount() const
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      size_t n = 0;
      for(const auto& [ti, v]: m_subscribers)
         n += v.size();
      return n;
   }

protected:
   bool m_bRunning = false;

private:
   using Handler = std::pair<DelegateKey, std::function<void(EventT*)>>;

   bool doSubscribe(std::type_index ti, const DelegateKey& key, std::function<void(EventT*)> wrapper)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto& handlers = m_subscribers[ti];
      for(const auto& h: handlers)
         if(h.first == key) return true;
      if(!m_bMulti && !handlers.empty()) return false;
      handlers.push_back({key, std::move(wrapper)});
      return true;
   }

   void doUnsubscribe(std::type_index ti, const DelegateKey& key)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = m_subscribers.find(ti);
      if(it == m_subscribers.end()) return;
      auto& handlers = it->second;
      handlers.erase(
         std::remove_if(handlers.begin(), handlers.end(), [&key](const Handler& h) { return h.first == key; }),
         handlers.end()
      );
   }

   bool m_bMulti;
   std::map<std::type_index, std::vector<Handler>> m_subscribers;
   mutable std::mutex m_mutex;
};

/// Matches the original Util::AsyncEventPublisher typedef
using AsyncEventPublisher = AsyncPublisher<AsyncEvent>;

} // namespace Util

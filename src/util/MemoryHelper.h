// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "platform/Assertions.h"

#include <atomic>
#include <cstdint>
#include <iomanip>
#include <KL/kLogger.h>
#include <memory>
#include <sstream>
#include <type_traits>
#include <typeinfo>

namespace Util {
// --------------------------------------------------------------------------
// SharedPointer
//
/// A smart pointer that enforces initialize/finalize lifecycle pattern.
///
/// This class wraps std::shared_ptr and ensures that:
/// - Objects are always created via create() methods
/// - initialize() is called after construction
/// - finalize() is called before destruction
///
/// Requirements for type T:
/// - Must have void initialize() method
/// - Must have void finalize() method
///
/// Typical usage:
///   auto ptr = Util::SharedPointer<MyClass>::create(arg1, arg2);
///   ptr->someMethod();  // Object is already initialized
///   // finalize() will be called automatically when last reference is
///   destroyed
// --------------------------------------------------------------------------
template <typename T>
class SharedPointer
{
private:
   // Helper class to access protected initialize/finalize methods
   class LifecycleHelper : public T
   {
   public:
      static void callInitialize(T* ptr)
      {
         // keep log for debug
         std::ostringstream oss;
         oss << "[SharedPointer] Calling initialize() for " << typeid(T).name() << " at address 0x" << std::hex
             << std::setw(16) << std::setfill('0') << reinterpret_cast<std::uintptr_t>(ptr);
         FLOG_INFO(oss.str().c_str());
         static_cast<LifecycleHelper*>(ptr)->initialize();
         oss.str("");
         oss.clear();
         oss << "[SharedPointer] Initialize() completed for " << typeid(T).name() << " at address 0x" << std::hex
             << std::setw(16) << std::setfill('0') << reinterpret_cast<std::uintptr_t>(ptr);
         FLOG_INFO(oss.str().c_str());
      }

      static void callFinalize(T* ptr)
      {
         // keep log for debug
         std::ostringstream oss;
         oss << "[SharedPointer] Calling finalize() for " << typeid(T).name() << " at address 0x" << std::hex
             << std::setw(16) << std::setfill('0') << reinterpret_cast<std::uintptr_t>(ptr);
         FLOG_INFO(oss.str().c_str());
         static_cast<LifecycleHelper*>(ptr)->finalize();
         oss.str("");
         oss.clear();
         oss << "[SharedPointer] Finalize() completed for " << typeid(T).name() << " at address 0x" << std::hex
             << std::setw(16) << std::setfill('0') << reinterpret_cast<std::uintptr_t>(ptr);
         FLOG_INFO(oss.str().c_str());
      }
   };

public:
   typedef SharedPointer<T> MyType;

   SharedPointer()
   : m_ptr(nullptr)
   {
   }

   SharedPointer(const MyType& other)
   : m_ptr(other.m_ptr)
   {
   }

   SharedPointer(MyType&& other)
   : m_ptr(std::move(other.m_ptr))
   {
   }

   SharedPointer(std::nullptr_t)
   : m_ptr(nullptr)
   {
   }

   // Constructor from std::shared_ptr (does NOT call initialize/finalize)
   explicit SharedPointer(const std::shared_ptr<T>& ptr)
   : m_ptr(ptr)
   {
   }

   // Constructor from std::shared_ptr (move version, does NOT call
   // initialize/finalize)
   explicit SharedPointer(std::shared_ptr<T>&& ptr)
   : m_ptr(std::move(ptr))
   {
   }

   // Conversion constructor for compatible types
   // Supports: derived to base, T to const T
   // Disabled when U == T to avoid conflicts with copy constructor
   template <typename U, typename = typename std::enable_if<!std::is_same<U, T>::value>::type>
   SharedPointer(const SharedPointer<U>& other)
   : m_ptr(other.m_ptr)
   {
   }

   // Conversion constructor for compatible types (move version)
   // Supports: derived to base, T to const T
   // Disabled when U == T to avoid conflicts with move constructor
   template <typename U, typename = typename std::enable_if<!std::is_same<U, T>::value>::type>
   SharedPointer(SharedPointer<U>&& other)
   : m_ptr(std::move(other.m_ptr))
   {
   }

   ~SharedPointer()
   {
   }

   inline MyType& operator=(const MyType& other)
   {
      m_ptr = other.m_ptr;
      return *this;
   }

   inline MyType& operator=(MyType&& other)
   {
      m_ptr = std::move(other.m_ptr);
      return *this;
   }

   // Conversion assignment for compatible types
   // Supports: derived to base, T to const T
   // Disabled when U == T to avoid conflicts with copy assignment
   template <typename U>
   inline typename std::enable_if<!std::is_same<U, T>::value, MyType&>::type operator=(const SharedPointer<U>& other)
   {
      m_ptr = other.m_ptr;
      return *this;
   }

   // Conversion assignment for compatible types (move version)
   // Supports: derived to base, T to const T
   // Disabled when U == T to avoid conflicts with move assignment
   template <typename U>
   inline typename std::enable_if<!std::is_same<U, T>::value, MyType&>::type operator=(SharedPointer<U>&& other)
   {
      m_ptr = std::move(other.m_ptr);
      return *this;
   }

   inline MyType& operator=(std::nullptr_t)
   {
      m_ptr = nullptr;
      return *this;
   }

   inline T* get() const
   {
      return m_ptr.get();
   }

   inline bool isNull() const
   {
      return m_ptr == nullptr;
   }

   inline operator bool() const
   {
      return m_ptr != nullptr;
   }

   inline T* operator->() const
   {
      TOOLS_ASSERT_OR_THROW(m_ptr != nullptr, Tool::NullPointerException(typeid(T)));
      return m_ptr.get();
   }

   inline T& operator*() const
   {
      TOOLS_ASSERT_OR_THROW(m_ptr != nullptr, Tool::NullPointerException(typeid(T)));
      return *m_ptr;
   }

   inline operator std::shared_ptr<T>() const
   {
      return m_ptr;
   }

   inline std::shared_ptr<T> toStdSharedPtr() const
   {
      return m_ptr;
   }

   // Type conversion methods

   /// Static cast - no runtime type checking
   /// Undefined behavior if types are incompatible
   template <typename U>
   SharedPointer<U> staticCast() const
   {
      SharedPointer<U> result;
      result.m_ptr = std::static_pointer_cast<U>(m_ptr);
      return result;
   }

   /// Dynamic cast with runtime type checking
   /// Returns nullptr if cast fails (safe)
   template <typename U>
   SharedPointer<U> dynamicCast() const
   {
      SharedPointer<U> result;
      result.m_ptr = std::dynamic_pointer_cast<U>(m_ptr);
      // Returns nullptr (result.m_ptr == nullptr) if cast fails
      return result;
   }

   /// Const cast - add or remove const qualifier
   template <typename U>
   SharedPointer<U> constCast() const
   {
      SharedPointer<U> result;
      result.m_ptr = std::const_pointer_cast<U>(m_ptr);
      return result;
   }

   /// Convert to const pointer (safe, always succeeds)
   SharedPointer<const T> toConst() const
   {
      SharedPointer<const T> result;
      result.m_ptr = m_ptr; // std::shared_ptr<T> implicitly converts to
                            // std::shared_ptr<const T>
      return result;
   }

   /// Remove const qualifier (use with caution)
   /// Returns nullptr if T is not const
   SharedPointer<typename std::remove_const<T>::type> removeConst() const
   {
      return constCast<typename std::remove_const<T>::type>();
   }

   // Static create methods
   static MyType create()
   {
      MyType ptr;
      T* rawPtr = new T();

      // Deleter state that holds a shared_ptr to a dummy object sharing the
      // same control block This keepalive shared_ptr prevents the control block
      // from being fully destroyed
      struct DeleterState
      {
         std::shared_ptr<void> keepalive;     // Shares control block, keeps it alive
         std::atomic<bool> isDeleting{false}; // Reentrancy guard
      };
      auto state = std::make_shared<DeleterState>();

      ptr.m_ptr = std::shared_ptr<T>(rawPtr, [state](T* p) {
         if(!p) return;

         // Check for reentrant deletion
         bool expected = false;
         if(!state->isDeleting.compare_exchange_strong(expected, true))
         {
            // Already deleting, don't recurse
            return;
         }

         // The keepalive shared_ptr shares the same control block as the
         // original This means during finalize(), weak_ptrs can still lock()
         // successfully because the control block shows use_count > 0 (due to
         // keepalive)

         // Call finalize while keepalive keeps the control block reference
         // count > 0
         LifecycleHelper::callFinalize(p);

         // Release the keepalive reference
         state->keepalive.reset();
         state->isDeleting.store(false);
         delete p;
      });

      // Create a keepalive shared_ptr that shares the same control block using
      // aliasing constructor This shared_ptr doesn't manage the lifetime of T,
      // but it does keep the control block alive
      state->keepalive = std::shared_ptr<void>(ptr.m_ptr, (void*)rawPtr);

      if(ptr.m_ptr)
      {
         LifecycleHelper::callInitialize(ptr.m_ptr.get());
      }
      return ptr;
   }

   template <typename T1>
   static MyType create(const T1& arg1)
   {
      MyType ptr;
      T* rawPtr = new T(arg1);

      struct DeleterState
      {
         std::shared_ptr<void> keepalive;
         std::atomic<bool> isDeleting{false};
      };
      auto state = std::make_shared<DeleterState>();

      ptr.m_ptr = std::shared_ptr<T>(rawPtr, [state](T* p) {
         if(!p) return;
         bool expected = false;
         if(!state->isDeleting.compare_exchange_strong(expected, true)) return;
         LifecycleHelper::callFinalize(p);
         state->keepalive.reset();
         state->isDeleting.store(false);
         delete p;
      });

      state->keepalive = std::shared_ptr<void>(ptr.m_ptr, (void*)rawPtr);

      if(ptr.m_ptr)
      {
         LifecycleHelper::callInitialize(ptr.m_ptr.get());
      }
      return ptr;
   }

   template <typename T1, typename T2>
   static MyType create(const T1& arg1, const T2& arg2)
   {
      MyType ptr;
      T* rawPtr = new T(arg1, arg2);

      struct DeleterState
      {
         std::shared_ptr<void> keepalive;
         std::atomic<bool> isDeleting{false};
      };
      auto state = std::make_shared<DeleterState>();

      ptr.m_ptr = std::shared_ptr<T>(rawPtr, [state](T* p) {
         if(!p) return;
         bool expected = false;
         if(!state->isDeleting.compare_exchange_strong(expected, true)) return;
         LifecycleHelper::callFinalize(p);
         state->keepalive.reset();
         state->isDeleting.store(false);
         delete p;
      });

      state->keepalive = std::shared_ptr<void>(ptr.m_ptr, (void*)rawPtr);

      if(ptr.m_ptr)
      {
         LifecycleHelper::callInitialize(ptr.m_ptr.get());
      }
      return ptr;
   }

   template <typename T1, typename T2, typename T3>
   static MyType create(const T1& arg1, const T2& arg2, const T3& arg3)
   {
      MyType ptr;
      T* rawPtr = new T(arg1, arg2, arg3);

      struct DeleterState
      {
         std::shared_ptr<void> keepalive;
         std::atomic<bool> isDeleting{false};
      };
      auto state = std::make_shared<DeleterState>();

      ptr.m_ptr = std::shared_ptr<T>(rawPtr, [state](T* p) {
         if(!p) return;
         bool expected = false;
         if(!state->isDeleting.compare_exchange_strong(expected, true)) return;
         LifecycleHelper::callFinalize(p);
         state->keepalive.reset();
         state->isDeleting.store(false);
         delete p;
      });

      state->keepalive = std::shared_ptr<void>(ptr.m_ptr, (void*)rawPtr);

      if(ptr.m_ptr)
      {
         LifecycleHelper::callInitialize(ptr.m_ptr.get());
      }
      return ptr;
   }

   template <typename T1, typename T2, typename T3, typename T4>
   static MyType create(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
   {
      MyType ptr;
      T* rawPtr = new T(arg1, arg2, arg3, arg4);

      struct DeleterState
      {
         std::shared_ptr<void> keepalive;
         std::atomic<bool> isDeleting{false};
      };
      auto state = std::make_shared<DeleterState>();

      ptr.m_ptr = std::shared_ptr<T>(rawPtr, [state](T* p) {
         if(!p) return;
         bool expected = false;
         if(!state->isDeleting.compare_exchange_strong(expected, true)) return;
         LifecycleHelper::callFinalize(p);
         state->keepalive.reset();
         state->isDeleting.store(false);
         delete p;
      });

      state->keepalive = std::shared_ptr<void>(ptr.m_ptr, (void*)rawPtr);

      if(ptr.m_ptr)
      {
         LifecycleHelper::callInitialize(ptr.m_ptr.get());
      }
      return ptr;
   }

   template <typename T1, typename T2, typename T3, typename T4, typename T5>
   static MyType create(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5)
   {
      MyType ptr;
      T* rawPtr = new T(arg1, arg2, arg3, arg4, arg5);

      struct DeleterState
      {
         std::shared_ptr<void> keepalive;
         std::atomic<bool> isDeleting{false};
      };
      auto state = std::make_shared<DeleterState>();

      ptr.m_ptr = std::shared_ptr<T>(rawPtr, [state](T* p) {
         if(!p) return;
         bool expected = false;
         if(!state->isDeleting.compare_exchange_strong(expected, true)) return;
         LifecycleHelper::callFinalize(p);
         state->keepalive.reset();
         state->isDeleting.store(false);
         delete p;
      });

      state->keepalive = std::shared_ptr<void>(ptr.m_ptr, (void*)rawPtr);

      if(ptr.m_ptr)
      {
         LifecycleHelper::callInitialize(ptr.m_ptr.get());
      }
      return ptr;
   }

private:
   template <typename U>
   friend class SharedPointer;

   std::shared_ptr<T> m_ptr;
};

template <typename T>
inline bool operator==(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs)
{
   return lhs.get() == rhs.get();
}

template <typename T>
inline bool operator!=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs)
{
   return !operator==(lhs, rhs);
}

template <typename T>
inline bool operator<(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs)
{
   return lhs.get() < rhs.get();
}

template <typename T>
inline bool operator>(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs)
{
   return lhs.get() > rhs.get();
}

template <typename T>
inline bool operator<=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs)
{
   return !operator>(lhs, rhs);
}

template <typename T>
inline bool operator>=(const SharedPointer<T>& lhs, const SharedPointer<T>& rhs)
{
   return !operator<(lhs, rhs);
}

// Comparison with nullptr
template <typename T>
inline bool operator==(const SharedPointer<T>& lhs, std::nullptr_t)
{
   return lhs.get() == nullptr;
}

template <typename T>
inline bool operator==(std::nullptr_t, const SharedPointer<T>& rhs)
{
   return rhs.get() == nullptr;
}

template <typename T>
inline bool operator!=(const SharedPointer<T>& lhs, std::nullptr_t)
{
   return lhs.get() != nullptr;
}

template <typename T>
inline bool operator!=(std::nullptr_t, const SharedPointer<T>& rhs)
{
   return rhs.get() != nullptr;
}

// --------------------------------------------------------------------------
// CheckedPointer
//
/// A smart pointer wrapper that performs runtime checks on weak pointer access.
///
/// This class wraps a std::weak_ptr and automatically checks if the pointer
/// is still valid (not expired) every time the -> operator is used.
/// If the pointer has expired, it throws a Tool::NullPointerException.
///
/// Typical usage:
///   Util::CheckedPointer<MyClass> checkedPtr(sharedPtr);
///   checkedPtr->someMethod();  // Automatically checks validity before access
// --------------------------------------------------------------------------
template <typename T>
class CheckedPointer
{
public:
   typedef CheckedPointer<T> MyType;

   CheckedPointer()
   : m_weakPtr()
   {
   }

   CheckedPointer(const std::shared_ptr<T>& sharedPtr)
   : m_weakPtr(sharedPtr)
   {
   }

   CheckedPointer(const std::weak_ptr<T>& weakPtr)
   : m_weakPtr(weakPtr)
   {
   }

   CheckedPointer(const MyType& other)
   : m_weakPtr(other.m_weakPtr)
   {
   }

   ~CheckedPointer()
   {
   }

   inline MyType& operator=(const std::shared_ptr<T>& sharedPtr)
   {
      m_weakPtr = sharedPtr;
      return *this;
   }

   inline MyType& operator=(const std::weak_ptr<T>& weakPtr)
   {
      m_weakPtr = weakPtr;
      return *this;
   }

   inline MyType& operator=(const MyType& other)
   {
      m_weakPtr = other.m_weakPtr;
      return *this;
   }

   inline bool isExpired() const
   {
      return m_weakPtr.expired();
   }

   inline bool isAlive() const
   {
      return !m_weakPtr.expired();
   }

   inline std::shared_ptr<T> lock() const
   {
      return m_weakPtr.lock();
   }

   inline T* get() const
   {
      std::shared_ptr<T> ptr = m_weakPtr.lock();
      TOOLS_ASSERT_OR_THROW(ptr != nullptr, Tool::NullPointerException(typeid(T)));
      return ptr.get();
   }

   inline T* operator->() const
   {
      std::shared_ptr<T> ptr = m_weakPtr.lock();
      TOOLS_ASSERT_OR_THROW(ptr != nullptr, Tool::NullPointerException(typeid(T)));
      return ptr.get();
   }

   inline T& operator*() const
   {
      std::shared_ptr<T> ptr = m_weakPtr.lock();
      TOOLS_ASSERT_OR_THROW(ptr != nullptr, Tool::NullPointerException(typeid(T)));
      return *ptr;
   }

   inline operator bool() const
   {
      return !m_weakPtr.expired();
   }

private:
   std::weak_ptr<T> m_weakPtr;
};

} // namespace Util
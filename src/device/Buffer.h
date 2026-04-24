// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Exception.h"
#include "device/Fwd.h"

// #define ENABLE_BUFFER_POOL    // Whether to have the buffer pool or not

namespace Device {
// ----------------------------------------------------------------------------
// SharedByteBuffer
//
/// A ref-counted, dynamically allocated byte buffer
// ----------------------------------------------------------------------------
class SharedByteBuffer
{
   TOOLS_FORBID_COPY(SharedByteBuffer);

public:
   typedef uint8_t* Iterator;
   typedef const uint8_t* const_Iterator;

   // -------------------------------------------------------------------------
   // SharedByteBuffer
   // -------------------------------------------------------------------------
   SharedByteBuffer()
   : m_pElements(nullptr)
   , m_nElements(0)
   , m_nCapacity(0)
   , m_bOwn(true)
   {
   }

   // -------------------------------------------------------------------------
   // SharedByteBuffer
   //
   /// Constructs a shared buffer pointing at the given raw buffer (no-copy)
   // -------------------------------------------------------------------------
   SharedByteBuffer(uint8_t* pElements, size_t nElements, bool bOwn)
   : m_pElements(pElements)
   , m_nElements(nElements)
   , m_nCapacity(nElements)
   , m_bOwn(bOwn)
   {
   }

   // -------------------------------------------------------------------------
   // SharedByteBuffer
   //
   /// Constructs a shared buffer pointing at a copy of the given raw buffer
   // -------------------------------------------------------------------------
   SharedByteBuffer(const uint8_t* pElements, size_t nElements)
   : m_pElements(nullptr)
   , m_nElements(0)
   , m_nCapacity(0)
   , m_bOwn(true)
   {
      assign(pElements, nElements);
   }

   // -------------------------------------------------------------------------
   // ~SharedByteBuffer
   //
   /// If this object owns the buffer, destroy it
   // -------------------------------------------------------------------------
   virtual ~SharedByteBuffer()
   {
      clear(true);
   }

   // -------------------------------------------------------------------------
   // capacity
   //
   /// @returns the number of elements that can be stored without reallocation
   // -------------------------------------------------------------------------
   inline size_t capacity() const
   {
      return m_nCapacity;
   }

   // -------------------------------------------------------------------------
   // reserve
   //
   /// Reserve space for a number of elements
   // -------------------------------------------------------------------------
   void reserve(size_t nElements);

   // -------------------------------------------------------------------------
   // empty
   //
   /// @returns true when no elements are stored
   // -------------------------------------------------------------------------
   inline bool empty() const
   {
      return 0 == size();
   }

   // -------------------------------------------------------------------------
   // size
   //
   /// @returns the number of elements stored in the buffer
   // -------------------------------------------------------------------------
   inline size_t size() const
   {
      return m_nElements;
   }

   // -------------------------------------------------------------------------
   // resize
   //
   /// Grow or shrink the buffer to the requested size
   // -------------------------------------------------------------------------
   void resize(size_t nElements, size_t nAutoIncrementSize = 0);

   // -------------------------------------------------------------------------
   // clear
   //
   /// Remove all elements from the buffer
   // -------------------------------------------------------------------------
   void clear(bool bReleaseMemory = false);

   // -------------------------------------------------------------------------
   // erase
   //
   /// Remove elements from the buffer within a given range
   // -------------------------------------------------------------------------
   void erase(Iterator first, Iterator last);

   // -------------------------------------------------------------------------
   // append
   //
   /// Append the given elements to the end of the container
   // -------------------------------------------------------------------------
   void append(const uint8_t* pElements, size_t nElements);

   // -------------------------------------------------------------------------
   // append
   //
   /// Append the given elements to the end of the container
   // -------------------------------------------------------------------------
   inline void append(const uint8_t* pElements, const uint8_t* pEnd)
   {
      append(pElements, static_cast<size_t>(pEnd - pElements));
   }

   // -------------------------------------------------------------------------
   // append
   //
   /// Append a basic type value to the buffer
   // -------------------------------------------------------------------------
   template <typename _BasicType>
   inline void append(_BasicType value);

   // -------------------------------------------------------------------------
   // assign
   //
   /// Replace the current contents with the given elements
   // -------------------------------------------------------------------------
   void assign(const uint8_t* pElements, size_t nElements);

   // -------------------------------------------------------------------------
   // at
   //
   /// @returns a reference to the element at index n
   // -------------------------------------------------------------------------
   inline uint8_t& at(size_t n)
   {
      return m_pElements[n];
   }

   // -------------------------------------------------------------------------
   // at
   //
   /// @returns a reference to the element at index n
   // -------------------------------------------------------------------------
   inline const uint8_t& at(size_t n) const
   {
      return m_pElements[n];
   }

   // -------------------------------------------------------------------------
   // begin
   //
   /// @returns an iterator on the first element in the buffer
   // -------------------------------------------------------------------------
   inline Iterator begin()
   {
      return m_pElements;
   }
   inline const_Iterator begin() const
   {
      return m_pElements;
   }

   // -------------------------------------------------------------------------
   // end
   //
   /// @returns an iterator set 1 past the last element in the buffer
   // -------------------------------------------------------------------------
   inline Iterator end()
   {
      return m_pElements + m_nElements;
   }
   inline const_Iterator end() const
   {
      return m_pElements + m_nElements;
   }

   uint8_t* m_pElements; ///< The buffer
   size_t m_nElements;   ///< Number of constructed elements in the buffer
   size_t m_nCapacity;   ///< Length of the buffer (in elements)
   bool m_bOwn;          ///< Does this object own the buffer

   friend class Buffer;
};

// ----------------------------------------------------------------------------
// Buffer
//
/// A shared buffer object that releases back into a pool
// ----------------------------------------------------------------------------
class Buffer : public SharedByteBuffer
{
   TOOLS_FORBID_COPY(Buffer);

public:
   // -------------------------------------------------------------------------
   // Buffer
   // -------------------------------------------------------------------------
   Buffer()
   : Device::SharedByteBuffer()
   {
   }

   // -------------------------------------------------------------------------
   // Buffer
   //
   /// Constructs a buffer pointing at the given raw buffer
   // -------------------------------------------------------------------------
   Buffer(uint8_t* pElements, size_t nElements, bool bOwn)
   : Device::SharedByteBuffer(pElements, nElements, bOwn)
   {
   }

   // -------------------------------------------------------------------------
   // createBuffer
   //
   /// Creates a shared buffer pointing to the given raw buffer (no copy)
   /// @returns the newly allocated shared buffer object
   // -------------------------------------------------------------------------
   inline static BufferPtr createBuffer(size_t nElements = 0)
   {
#ifdef ENABLE_BUFFER_POOL
      return BufferPool::getInstance().createBuffer(nElements);
#else
      BufferPtr pBuffer = std::make_shared<Buffer>();
      pBuffer->resize(nElements);
      return pBuffer;
#endif
   }

   // -------------------------------------------------------------------------
   // createCopy
   //
   /// Creates a shared buffer pointing to a copy of the given raw buffer
   /// @returns the newly allocated shared buffer object
   // -------------------------------------------------------------------------
   inline static BufferPtr createCopy(const uint8_t* pElements, size_t nElements)
   {
#ifdef ENABLE_BUFFER_POOL
      return BufferPool::getInstance().createCopy(pElements, nElements);
#else
      BufferPtr pBuffer = std::make_shared<Buffer>();
      pBuffer->assign(pElements, nElements);
      return pBuffer;
#endif
   }

   // -------------------------------------------------------------------------
   // createCopy
   //
   /// Creates a shared buffer pointing to a copy of the given raw buffer
   /// @returns the newly allocated shared buffer object
   // -------------------------------------------------------------------------
   inline static BufferPtr createCopy(const Device::SharedByteBufferPtr& pToCopy)
   {
      return createCopy(pToCopy->begin(), pToCopy->size());
   }

   virtual ~Buffer()
   {
#ifdef ENABLE_BUFFER_POOL
      // Check m_bExiting in here instead of in BufferPool in case
      // the BufferPool is already destroyed
      if(!BufferPool::m_bExiting)
      {
         BufferPool::getInstance().addBuffer(this);
      }
#endif
   }

   friend class BufferPool;
};

} // namespace Device

// ----------------------------------------------------------------------------
// Util namespace
//
/// Utility functions for buffer operations
// ----------------------------------------------------------------------------
namespace Util {

// ----------------------------------------------------------------------------
// buffer_cast
//
/// Provides a safe cast from byte buffer to a specific type with size checking
/// @returns The buffer pointer cast to the given type
// ----------------------------------------------------------------------------
template <typename _OutT>
inline _OutT buffer_cast(
   uint8_t* pBuffer, ///< Pointer to the buffer
   size_t length     ///< Length of the buffer (in bytes)
)
{
   if(nullptr == pBuffer)
   {
      throw Device::Exception(Device::Exception::DEVICE_INVALID_PACKET, "pBuffer is NULL");
   }

   _OutT out = reinterpret_cast<_OutT>(reinterpret_cast<void*>(pBuffer));

   if(sizeof(*out) > length)
   {
      throw Device::Exception(Device::Exception::DEVICE_INVALID_PACKET, "Insufficient bytes for cast to type");
   }

   return out;
}

// ----------------------------------------------------------------------------
// buffer_cast
//
/// Const version - provides a safe cast from byte buffer to a specific type
/// @returns The buffer pointer cast to the given type
// ----------------------------------------------------------------------------
template <typename _OutT>
inline _OutT buffer_cast(
   const uint8_t* pBuffer, ///< Pointer to the buffer
   size_t length           ///< Length of the buffer (in bytes)
)
{
   if(nullptr == pBuffer)
   {
      throw Device::Exception(Device::Exception::DEVICE_INVALID_PACKET, "pBuffer is NULL");
   }

   _OutT out = reinterpret_cast<_OutT>(reinterpret_cast<const void*>(pBuffer));

   if(sizeof(*out) > length)
   {
      throw Device::Exception(Device::Exception::DEVICE_INVALID_PACKET, "Insufficient bytes for cast to type");
   }

   return out;
}

// ----------------------------------------------------------------------------
// buffer_cast
//
/// Convenience overload that takes SharedByteBufferPtr directly
/// @returns The buffer pointer cast to the given type
// ----------------------------------------------------------------------------
template <typename _OutT>
inline _OutT buffer_cast(const Device::SharedByteBufferPtr& pBuffer ///< SharedByteBuffer
                                                                    ///< pointer
)
{
   if(pBuffer == nullptr)
   {
      throw Device::Exception(Device::Exception::DEVICE_INVALID_PACKET, "SharedByteBuffer is NULL");
   }

   return buffer_cast<_OutT>(const_cast<uint8_t*>(pBuffer->begin()), pBuffer->size());
}

} // namespace Util

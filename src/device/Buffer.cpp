// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "device/Buffer.h"

namespace Device {
#ifdef ENABLE_BUFFER_POOL

// ----------------------------------------------------------------------------
// BufferPoolInitializer
//
/// Helper class to ensure that the buffer pool is initialized before
/// multiple threads are created.  The global of this will create the pool,
/// this way the BufferPool::getInstance() function does not need expensive
/// mutex locks or instance checks during run time.
// ----------------------------------------------------------------------------
class BufferPoolInitializer
{
   TOOLS_FORBID_COPY(BufferPoolInitializer);

public:
   BufferPoolInitializer()
   {
      BufferPool::getInstance();
   }
   virtual ~BufferPoolInitializer()
   {
   }
};
static const BufferPoolInitializer g_poolInitializer;

// m_bExiting is static so it will persist even if BufferPool is destroyed.
// This will ensure any Buffer objects trying to check m_bExiting in their
// destructor will still be able to
bool BufferPool::m_bExiting(false);


// ----------------------------------------------------------------------------
// ~BufferPool
//
// ----------------------------------------------------------------------------
BufferPool::~BufferPool()
{
   m_bExiting = true;
}

// ----------------------------------------------------------------------------
// getInstance
//
/// @returns The singleton instance of the pool
// ----------------------------------------------------------------------------
BufferPool& BufferPool::getInstance()
{
   static BufferPool theInstance;
   return theInstance;
}

// ----------------------------------------------------------------------------
// getCacheHits
//
/// @returns The number of buffer allocations fulfilled by the pool
// ----------------------------------------------------------------------------
uint64_t BufferPool::getCacheHits() const
{
   return m_bin128.m_cacheHits + m_bin1024.m_cacheHits + m_bin4096.m_cacheHits + m_bin16384.m_cacheHits +
          m_bin1Mb.m_cacheHits;
}

// ----------------------------------------------------------------------------
// getCacheMisses
//
/// @returns The number of buffer allocations that had to be allocated from
/// the system
// ----------------------------------------------------------------------------
uint64_t BufferPool::getCacheMisses() const
{
   return m_bin128.m_cacheMisses + m_bin1024.m_cacheMisses + m_bin4096.m_cacheMisses + m_bin16384.m_cacheMisses +
          m_bin1Mb.m_cacheMisses;
}

// ----------------------------------------------------------------------------
// BufferPool
//
// ----------------------------------------------------------------------------
BufferPool::BufferPool()
: m_bin128()
, m_bin1024()
, m_bin4096()
, m_bin16384()
, m_bin1Mb()
{
}

// ----------------------------------------------------------------------------
// createBuffer
//
/// @returns A buffer from the pool
// ----------------------------------------------------------------------------
BufferPtr BufferPool::createBuffer(size_t nElements)
{
   BufferPtr pBuffer;
   if(nElements <= m_bin128.ITEM_BUFFER_SIZE)
   {
      pBuffer = m_bin128.getBuffer();
   }
   else if(nElements <= m_bin1024.ITEM_BUFFER_SIZE)
   {
      pBuffer = m_bin1024.getBuffer();
   }
   else if(nElements <= m_bin4096.ITEM_BUFFER_SIZE)
   {
      pBuffer = m_bin4096.getBuffer();
   }
   else if(nElements <= m_bin16384.ITEM_BUFFER_SIZE)
   {
      pBuffer = m_bin16384.getBuffer();
   }
   else if(nElements <= m_bin1Mb.ITEM_BUFFER_SIZE)
   {
      pBuffer = m_bin1Mb.getBuffer();
   }
   else
   {
      // If it's too big we want to release it at the end
      pBuffer = std::make_shared<Buffer>();
   }


   pBuffer->resize(nElements);
   return pBuffer;
}

// ----------------------------------------------------------------------------
// createCopy
//
/// Creates a copy of the memory from an item in the pool
// ----------------------------------------------------------------------------
BufferPtr BufferPool::createCopy(const uint8_t* pElements, size_t nElements)
{
   BufferPtr pBuffer = createBuffer(nElements);

   pBuffer->assign(pElements, nElements);

   return pBuffer;
}

// ----------------------------------------------------------------------------
// addBuffer
//
/// Adds a buffer back into the pool
// ----------------------------------------------------------------------------
void BufferPool::addBuffer(Buffer* pBuffer)
{
   if(!pBuffer->m_bOwn)
   {
      return;
   }

   size_t capacity = pBuffer->capacity();

   switch(capacity)
   {
         // For the common case, the capacity shouldn't have changed from
         // when it was allocated from the pool, so it should fit one of the
         // standard values.
      case 128:
         m_bin128.addBuffer(pBuffer);
         break;
      case 1024:
         m_bin1024.addBuffer(pBuffer);
         break;
      case 4096:
         m_bin4096.addBuffer(pBuffer);
         break;
      case 16384:
         m_bin16384.addBuffer(pBuffer);
         break;
      case 1024 * 1024:
         m_bin1Mb.addBuffer(pBuffer);
         break;
      default:
         // Only keep buffers with standard sizes
         break;
   }
}

// ----------------------------------------------------------------------------
// allocateBuffer
//
/// Allocate new buffer of the buffer size.
// ----------------------------------------------------------------------------
BufferPtr BufferPool::allocateBuffer(size_t bufferSize)
{
   BufferPtr pBuffer = std::make_shared<Buffer>();
   pBuffer->reserve(bufferSize);
   return pBuffer;
}

#endif

// ----------------------------------------------------------------------------
// SharedByteBuffer implementation
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// reserve
//
/// Reserve space for a number of elements
// ----------------------------------------------------------------------------
void SharedByteBuffer::reserve(size_t nElements)
{
   // Capacity already exceeds request? Nothing to do.
   if(nElements <= capacity())
   {
      return;
   }

   // Capacity needs to be increased? Must own the buffer
   if(!m_bOwn)
   {
      throw std::runtime_error("Attempted expansion of unowned buffer");
   }

   // Allocate expanded buffer
   uint8_t* pNew = static_cast<uint8_t*>(malloc(nElements));
   if(nullptr == pNew)
   {
      throw std::bad_alloc();
   }

   // Copy existing elements to expanded buffer
   if(m_pElements && m_nElements > 0)
   {
      memcpy(pNew, m_pElements, m_nElements);
   }

   // Deallocate old buffer
   if(m_pElements && m_bOwn)
   {
      free(m_pElements);
   }

   // Set internal info
   m_pElements = pNew;
   m_nCapacity = nElements;
}

// ----------------------------------------------------------------------------
// resize
//
/// Grow or shrink the buffer to the requested size
// ----------------------------------------------------------------------------
void SharedByteBuffer::resize(size_t nElements, size_t nAutoIncrementSize)
{
   // Nothing to do
   if(m_nElements == nElements)
   {
      return;
   }

   // Can't change the size of a borrowed buffer
   if(!m_bOwn)
   {
      throw std::runtime_error("Attempted resize of unowned buffer");
   }

   // Make room if necessary
   if(nElements > capacity())
   {
      // If auto increment was preferred by users use auto increment size
      size_t reserveSize = nElements;
      if(0 < nAutoIncrementSize)
      {
         reserveSize = nElements + nAutoIncrementSize;
      }
      reserve(reserveSize);
   }

   // POD type (uint8_t) doesn't need construction/destruction
   m_nElements = nElements;
}

// ----------------------------------------------------------------------------
// clear
//
/// Remove all elements from the buffer
// ----------------------------------------------------------------------------
void SharedByteBuffer::clear(bool bReleaseMemory)
{
   if(m_bOwn)
   {
      if(bReleaseMemory && m_pElements)
      {
         free(m_pElements);
         m_pElements = nullptr;
         m_nCapacity = 0;
      }
      m_nElements = 0;
   }
   else
   {
      m_pElements = nullptr;
      m_nElements = 0;
      m_nCapacity = 0;
      m_bOwn = true;
   }
}

// ----------------------------------------------------------------------------
// erase
//
/// Remove elements from the buffer within a given range
// ----------------------------------------------------------------------------
void SharedByteBuffer::erase(Iterator first, Iterator last)
{
   if(first > last)
   {
      throw std::runtime_error("Invalid range passed to erase");
   }

   // Can't change the size of a borrowed buffer
   if(!m_bOwn)
   {
      throw std::runtime_error("Attempted erase of unowned buffer");
   }

   size_t nRemove = static_cast<size_t>(last - first);

   // Shift trailing items down
   if(last < end())
   {
      memmove(first, last, static_cast<size_t>(end() - last));
   }

   m_nElements -= nRemove;
}

// ----------------------------------------------------------------------------
// append
//
/// Append the given elements to the end of the container
// ----------------------------------------------------------------------------
void SharedByteBuffer::append(const uint8_t* pElements, size_t nElements)
{
   size_t newSize = nElements + size();

   // Make sure there is enough buffer
   if(capacity() < newSize)
   {
      size_t nCapacity = capacity();
      if(0 == nCapacity)
      {
         nCapacity = nElements;
      }
      else
      {
         do
         {
            // Double the capacity until new elements fit
            nCapacity *= 2;

            // Watch out for wrap-around
         } while((nCapacity < newSize) && (0 != nCapacity));
      }

      // Reserve the new elements
      reserve(nCapacity);
   }

   // Copy source elements
   memcpy(m_pElements + m_nElements, pElements, nElements);

   // Update the element count
   m_nElements += nElements;
}

// ----------------------------------------------------------------------------
// append (template specialization)
//
/// Append a basic type value to the buffer
// ----------------------------------------------------------------------------
template <typename _BasicType>
void SharedByteBuffer::append(_BasicType value)
{
   size_t newSize = sizeof(_BasicType) + size();

   // Make sure there is enough buffer
   if(capacity() < newSize)
   {
      size_t nCapacity = capacity();
      if(0 == nCapacity)
      {
         nCapacity = sizeof(_BasicType);
      }
      else
      {
         do
         {
            // Double the capacity until new elements fit
            nCapacity *= 2;

            // Watch out for wrap-around
         } while((nCapacity < newSize) && (0 != nCapacity));
      }

      // Reserve the new elements
      reserve(nCapacity);
   }

   *reinterpret_cast<_BasicType*>(m_pElements + m_nElements) = value;

   // Update the element count
   m_nElements = newSize;
}

// Explicit template instantiations for common types
template void SharedByteBuffer::append<uint8_t>(uint8_t value);
template void SharedByteBuffer::append<uint16_t>(uint16_t value);
template void SharedByteBuffer::append<uint32_t>(uint32_t value);
template void SharedByteBuffer::append<uint64_t>(uint64_t value);
template void SharedByteBuffer::append<int8_t>(int8_t value);
template void SharedByteBuffer::append<int16_t>(int16_t value);
template void SharedByteBuffer::append<int32_t>(int32_t value);
template void SharedByteBuffer::append<int64_t>(int64_t value);

// ----------------------------------------------------------------------------
// assign
//
/// Replace the current contents with the given elements
// ----------------------------------------------------------------------------
void SharedByteBuffer::assign(const uint8_t* pElements, size_t nElements)
{
   if(!m_bOwn && (nElements != capacity()))
   {
      throw std::runtime_error("Operation requires resizing borrowed buffer");
   }

   // Clear existing elements
   m_nElements = 0;

   // More elements than are currently allocated
   if(nElements > capacity())
   {
      // Deallocate the existing buffer
      if(m_bOwn && m_pElements)
      {
         free(m_pElements);
      }

      // Allocate a new buffer
      m_pElements = static_cast<uint8_t*>(malloc(nElements));
      if(nullptr == m_pElements)
      {
         throw std::bad_alloc();
      }
      m_nCapacity = nElements;
      m_bOwn = true;
   }

   // Copy in the new elements
   if(pElements && nElements > 0)
   {
      memcpy(m_pElements, pElements, nElements);
   }

   m_nElements = nElements;
}

} // namespace Device

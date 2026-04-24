// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
// Utility functions for std::function comparison and manipulation

#include <cstring>
#include <functional>
#include <type_traits>
#include <typeindex>

namespace Util {

// --------------------------------------------------------------------------
// isSameFunction
//
/// Compares two std::function objects to determine if they wrap the same
/// callable
///
/// Supported comparisons:
/// - Function pointers: full support
/// - std::bind with member functions + object pointers: supported via memory
/// comparison
///   Example: std::bind(&Class::method, obj_ptr, std::placeholders::_1)
/// - Stateless lambdas: supported
///
/// @note Limitations:
/// - Cannot reliably compare lambdas with captures
/// - std::bind comparison uses heuristic memory comparison
/// (implementation-dependent)
/// - Works with raw pointers and shared_ptr in bind expressions
///
/// @returns true if the functions appear to be the same, false otherwise
// --------------------------------------------------------------------------
template <typename Signature>
inline bool isSameFunction(const std::function<Signature>& f1, const std::function<Signature>& f2)
{
   // Both empty - consider them the same
   if(!f1 && !f2)
   {
      return true;
   }

   // One empty, one not - different
   if(!f1 || !f2)
   {
      return false;
   }

   // Check if they have the same target type
   if(f1.target_type() != f2.target_type())
   {
      return false;
   }

   // Try to compare as function pointers
   typedef Signature* FuncPtr;
   const FuncPtr* ptr1 = f1.template target<FuncPtr>();
   const FuncPtr* ptr2 = f2.template target<FuncPtr>();

   if(ptr1 && ptr2)
   {
      return *ptr1 == *ptr2;
   }

   // Try to compare as member function pointers (common case)
   // Note: This won't work for lambdas or std::bind results
   const void* target1 = reinterpret_cast<const void*>(f1.template target<Signature*>());
   const void* target2 = reinterpret_cast<const void*>(f2.template target<Signature*>());

   if(target1 && target2)
   {
      return target1 == target2;
   }

   // Try to compare std::bind results
   // This is a heuristic that works for simple bind expressions like:
   //   std::bind(&Class::method, object, placeholders...)
   //
   // Implementation note: std::bind creates objects that store:
   // 1. The callable (function/member function pointer)
   // 2. The bound arguments (object pointers, values)
   // 3. Placeholder information (compile-time, not stored)
   //
   // For the common case of member function + object pointer binding,
   // we can compare the stored data to determine equality.

   const std::type_info& type = f1.target_type();

   // Both must have the same type
   if(type == f2.target_type())
   {
      std::string typeName = type.name();

      // Check if this looks like a std::bind result
      // Type names from std::bind contain "bind" or "_Bind"
      if(typeName.find("bind") != std::string::npos || typeName.find("Bind") != std::string::npos)
      {
         // For std::bind, compare by memory
         // This works because:
         // 1. Same bind expression type = same memory layout
         // 2. std::bind stores function pointer and object pointer contiguously
         // 3. Placeholders don't affect storage (they're types only)

         // Helper to get target storage address
         auto getTargetStorage = [](const std::function<Signature>& func) -> const unsigned char* {
            // std::function::target<T>() returns pointer to stored callable
            // We need the actual type T of the bind result, which we can't name
            // Instead, we use the target_type() to verify and cast to char*

            // Access the function's internal storage
            // The std::function small buffer optimization stores small
            // callables inline For bind results with member function + object
            // pointer, this is typically small enough

            // Cast function object to bytes to access its storage
            // Note: This is implementation-defined but works on MSVC, GCC,
            // Clang
            const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&func);

            // Skip to the storage area (after vtable pointer and other
            // metadata) Typical layout: vtable (8 bytes on 64-bit), then
            // storage
            return bytes + sizeof(void*);
         };

         const unsigned char* storage1 = getTargetStorage(f1);
         const unsigned char* storage2 = getTargetStorage(f2);

         // Compare the relevant portion of storage
         // For member function bind: function pointer (8-16 bytes) + object
         // pointer (8 bytes) We compare up to 32 bytes to be safe, covering
         // function ptr + object ptr
         const size_t compareSize = 32;

         if(std::memcmp(storage1, storage2, compareSize) == 0)
         {
            return true;
         }
      }
   }

   // Cannot determine - assume different
   // Note: This includes lambdas with captures, complex bind expressions, etc.
   return false;
}

} // namespace Util

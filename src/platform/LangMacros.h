// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "platform/Compiler.h"
#include "platform/Target.h"

#include <algorithm>
#include <string>


// --------------------------------------------------------------------------
//
// LANG PLATFORM
//
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// TOOLS_CONCATENATE
//
/// Concatenate two tokens
// --------------------------------------------------------------------------
#define TOOLS_CONCATENATE(x, y) x##y

// --------------------------------------------------------------------------
// TOOLS_I64/TOOLS_UI64
//
/// make an integer constant a 64-bit integer
// --------------------------------------------------------------------------
#if defined TOOLS_COMPILER_MSVC
#define TOOLS_I64(c) TOOLS_CONCATENATE(c, I64)
#define TOOLS_UI64(c) TOOLS_CONCATENATE(c, UI64)
#if defined TOOLS_ARCH_32BIT
#define TOOLS_SIZE(i) TOOLS_CONCATENATE(i, U)
#elif defined TOOLS_ARCH_64BIT
#define TOOLS_SIZE(i) TOOLS_UI64(i)
#else
#error Unkonwn architecture type
#endif
#elif defined TOOLS_COMPILER_GNUC
#define TOOLS_I64(c) TOOLS_CONCATENATE(c, LL)
#define TOOLS_UI64(c) TOOLS_CONCATENATE(c, ULL)
#if defined TOOLS_ARCH_32BIT
#define TOOLS_SIZE(i) TOOLS_CONCATENATE(i, U)
#elif defined TOOLS_ARCH_64BIT
#define TOOLS_SIZE(i) TOOLS_UI64(i)
#else
#error Unkonwn architecture type
#endif
#elif defined TOOLS_COMPILER_CLANG
#define TOOLS_I64(c) TOOLS_CONCATENATE(c, LL)
#define TOOLS_UI64(c) TOOLS_CONCATENATE(c, ULL)
#if defined TOOLS_ARCH_32BIT
#define TOOLS_SIZE(i) TOOLS_CONCATENATE(i, U)
#elif defined TOOLS_ARCH_64BIT
#define TOOLS_SIZE(i) TOOLS_UI64(i)
#else
#error Unkonwn architecture type
#endif
#else
#error Unknown compiler
#endif


// --------------------------------------------------------------------------
//
// LANG MARCOS
//
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// TOOLS_ANONYMOUS_IDENTIFIER
//
/// Fabricate an identifier from a prefix and a line number
// --------------------------------------------------------------------------
#define TOOLS_ANONYMOUS_IDENTIFIER(prefix)                                                                             \
   TOOLS_ANONYMOUS_IDENTIFIER2(TOOLS_CONCATENATE(__an0nym0us__, prefix), __LINE__)

#define TOOLS_ANONYMOUS_IDENTIFIER2(prefix, line) TOOLS_CONCATENATE(prefix, line)

// --------------------------------------------------------------------------
// TOOLS_ARRAY_SIZE
//
/// Get the number of elements in an array
// --------------------------------------------------------------------------
#define TOOLS_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// --------------------------------------------------------------------------
// TOOLS_COMPOUND_ZERO_INIT
//
/// Initialize an array, union or struct to zero
// --------------------------------------------------------------------------
#define TOOLS_COMPOUND_ZERO_INIT()                                                                                     \
   {                                                                                                                   \
      0                                                                                                                \
   }

// --------------------------------------------------------------------------
// TOOLS_FALSE
//
/// Helper to serve as a false value and avoid compiler warnings about
/// constant value conditions.
// --------------------------------------------------------------------------
inline bool TOOLS_FALSE() noexcept
{
   return false;
}

// --------------------------------------------------------------------------
// TOOLS_TRUE
//
/// Helper to serve as a true value and avoid compiler warnings about
/// constant value conditions.
// --------------------------------------------------------------------------
inline bool TOOLS_TRUE() noexcept
{
   return true;
}

// --------------------------------------------------------------------------
// TOOLS_FALLTHROUGH
//
/// Allows fallthrough in case statments without lint warnings
// --------------------------------------------------------------------------
#define TOOLS_FALLTHROUGH /*lint -e(616, 825)*/ /* FALLTHROUGH */ ;

// --------------------------------------------------------------------------
// TOOLS_FIELD_SIZE
//
/// Calculate the size of a struct field in bytes
// --------------------------------------------------------------------------
#define TOOLS_FIELD_SIZE(s, f) TOOLS_SIZEOF((reinterpret_cast<s*>(0))->f)

// --------------------------------------------------------------------------
// TOOLS_FORBID_COPY
//
/// Prevent instances of a class from being copyable
// --------------------------------------------------------------------------
#define TOOLS_FORBID_COPY(Class)                                                                                       \
private:                                                                                                               \
   Class(const Class&) = delete;                                                                                       \
   Class& operator=(const Class&) = delete

// --------------------------------------------------------------------------
// TOOLS_SIZEOF
//
/// Cast the sizeof to Uint32 to remove 64 bit warning
// --------------------------------------------------------------------------
#define TOOLS_SIZEOF(x) (static_cast<uint64_t>(sizeof(x)))

// --------------------------------------------------------------------------
// TOOLS_STR
//
/// Why 2 STR macros? Doing it this way ensures that symbols like __LINE__
/// are properly stringized
// --------------------------------------------------------------------------
#define TOOLS_STR(x) TOOLS_STR2(x)
#define TOOLS_STR2(x) #x

// --------------------------------------------------------------------------
// _T
//
/// Deprecated.  Always default compile as 8 bit string literals
// --------------------------------------------------------------------------
#undef _T
#define _T(x) x

// --------------------------------------------------------------------------
// TOOLS_FUNCTION_NAME
//
/// Name of the current function
///
/// GCC defines the __FUNCTION__ symbol as a local char array rather than
/// a free string literal -- so it cannot be automatically concatenated with
/// other string literals by the pre-processor
/// Known issues:
/// 1.This is not thread safe
/// 2.The 256 may not be long enough if a method name is too long, we may run
/// over the boundary.
/// As this is only for Debugging, don't want to make it too complex to add the
/// thread safe and bounary checking. We can revisit it later if there is
/// problem.
// --------------------------------------------------------------------------
#if defined TOOLS_COMPILER_MSVC
#define TOOLS_FUNCTION_NAME _T(__FUNCTION__)
#elif defined TOOLS_COMPILER_GNUC || defined TOOLS_COMPILER_CLANG
#define TOOLS_FUNCTION_NAME __FUNCTION__
#else
#error Unknown compiler
#endif

// --------------------------------------------------------------------------
// TOOLS_SOURCE_LOCATION
//
/// Convenient location string
// --------------------------------------------------------------------------
#if defined TOOLS_COMPILER_MSVC
#define TOOLS_SOURCE_LOCATION()                                                                                        \
   _T(__FILE__)                                                                                                        \
   _T("(") _T(TOOLS_STR(__LINE__)) _T(") : ") TOOLS_FUNCTION_NAME
#elif defined TOOLS_COMPILER_GNUC || defined TOOLS_COMPILER_CLANG
inline const char* toolsMakeSourceLocation(const char* const pFileString, const char* const pFunctionName)
{
   static thread_local char TOOLS_SOURCE_LOCATION_STRING[512];
   std::string temp = std::string(pFileString) + pFunctionName;
   size_t len = std::min(temp.size(), sizeof(TOOLS_SOURCE_LOCATION_STRING) - 1);
   temp.copy(TOOLS_SOURCE_LOCATION_STRING, len);
   TOOLS_SOURCE_LOCATION_STRING[len] = '\0';
   return TOOLS_SOURCE_LOCATION_STRING;
}
#define TOOLS_SOURCE_LOCATION()                                                                                        \
   toolsMakeSourceLocation(_T(__FILE__) _T("(") _T(TOOLS_STR(__LINE__)) _T(") : "), TOOLS_FUNCTION_NAME)
#else
#error Unknown compiler
#endif

// --------------------------------------------------------------------------
// TOOLS_UNUSED_PARAMETER
//
/// Explicitly mark a parameter as unused (removes warning)
// --------------------------------------------------------------------------
template <typename __X>
inline void TOOLS_UNUSED_PARAMETER(const __X&) noexcept
{
}

// --------------------------------------------------------------------------
// TOOLS_UNUSED_PARAMETER
//
/// Special version for integers that will convert to 64-bit on
/// 64-bit architectures
// --------------------------------------------------------------------------
// template<>
// inline void TOOLS_UNUSED_PARAMETER(const TOOLS_W64 long&) throw() { }
//
// template<>
// inline void TOOLS_UNUSED_PARAMETER(const TOOLS_W64 unsigned long&) throw() {
// }
//
// template<>
// inline void TOOLS_UNUSED_PARAMETER(const Lang::Platform::Size&) throw() { }
//
// template<>
// inline void TOOLS_UNUSED_PARAMETER(const Lang::Platform::Offset&) throw() { }

// --------------------------------------------------------------------------
// TOOLS_UNUSED_RETURN
//
/// Explicitly ignore a return value
// --------------------------------------------------------------------------
#define TOOLS_UNUSED_RETURN(r) TOOLS_UNUSED_PARAMETER(r)

// --------------------------------------------------------------------------
// TOOLS_VOID
//
/// An empty definition to use as an empty statement
// --------------------------------------------------------------------------
#define TOOLS_VOID

// --------------------------------------------------------------------------
// TOOLS_WIDE
//
/// Make a string literal into a wide-string literal
// --------------------------------------------------------------------------
#define TOOLS_WIDE(x) TOOLS_CONCATENATE(L, x)

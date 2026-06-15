// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <assert.h>
#include <platform/Exception.h>

// --------------------------------------------------------------------------
// TOOLS_ASSERT
//
/// In debug mode, trigger an assertion breakpoint when the condition is false
/// In release mode, no-op
// --------------------------------------------------------------------------
#ifdef TOOLS_MODE_DEBUG
#define TOOLS_ASSERT(exp)                                                                                              \
   do                                                                                                                  \
   {                                                                                                                   \
      if(!(exp))                                                                                                       \
      {                                                                                                                \
         TOOLS_ASSERT_MESSAGE(#exp);                                                                                   \
      }                                                                                                                \
   } while(TOOLS_FALSE())
#else
#define TOOLS_ASSERT(exp)
#endif

// --------------------------------------------------------------------------
// TOOLS_ASSERT_MESSAGE
//
/// In debug mode, trigger an assetion message and breakpoint
/// In release mode, no-op
// --------------------------------------------------------------------------
#ifdef TOOLS_MODE_DEBUG
#define TOOLS_ASSERT_MESSAGE(exp) Tool::assertion(_T(exp), _T(__FILE__), __LINE__, TOOLS_FUNCTION_NAME)
#else
#define TOOLS_ASSERT_MESSAGE(exp)
#endif

// --------------------------------------------------------------------------
// TOOLS_ASSERT_OR_RETURN
//
/// In debug mode, asserts that the condition is true.
/// If the condition is not true (in debug or release mode),
///   execute a return statement.
// --------------------------------------------------------------------------
#define TOOLS_ASSERT_OR_RETURN(exp, rval)                                                                              \
   do                                                                                                                  \
   {                                                                                                                   \
      if(!(exp))                                                                                                       \
      {                                                                                                                \
         TOOLS_ASSERT_MESSAGE(#exp);                                                                                   \
         return rval;                                                                                                  \
      }                                                                                                                \
   } while(TOOLS_FALSE())

// --------------------------------------------------------------------------
// TOOLS_ASSERT_OR_THROW
//
/// In debug mode, asserts that the condition is true.
/// If the condition is not true (in debug or release mode),
///   throw the given exception.
// --------------------------------------------------------------------------
#define TOOLS_ASSERT_OR_THROW(exp, except)                                                                             \
   do                                                                                                                  \
   {                                                                                                                   \
      if(!(exp))                                                                                                       \
      {                                                                                                                \
         TOOLS_ASSERT_MESSAGE(#exp);                                                                                   \
         TOOLS_THROW(except);                                                                                          \
      }                                                                                                                \
   } while(TOOLS_FALSE())

// --------------------------------------------------------------------------
// TOOLS_ASSUMING
//
/// In debug mode, trigger an assertion when the condition is false
/// In release mode, execute the statement unchecked
// --------------------------------------------------------------------------
#ifdef TOOLS_MODE_DEBUG
#define TOOLS_ASSUMING(x) TOOLS_ASSERT(x)
#else
#define TOOLS_ASSUMING(x) TOOLS_UNUSED_RETURN(x)
#endif

// --------------------------------------------------------------------------
// TOOLS_STATIC_ASSERT
//
/// Check a boolean condition at compile-time
// --------------------------------------------------------------------------
#define TOOLS_STATIC_ASSERT(...) static_assert((__VA_ARGS__), "Failed static assertion!");

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

// Which target O/S are we building for?
#if defined __linux__

#ifndef TOOLS_TARGET_LINUX
#define TOOLS_TARGET_LINUX
#endif


// Check for 64-bit architecture using multiple common defines
#if defined(__x86_64__) || defined(__X86_64__) || defined(__amd64__) || defined(__aarch64__) || defined(__LP64__) ||   \
   defined(_LP64)
#define TOOLS_ARCH_64BIT
#else
#define TOOLS_ARCH_32BIT
#endif

#elif defined __APPLE__
#ifndef TOOLS_TARGET_OSX
#define TOOLS_TARGET_OSX
#endif
#define TOOLS_ARCH_64BIT


#elif defined _M_X64

#ifndef WIN64
#define WIN64
#endif

#ifndef _WIN64
#define _WIN64
#endif

#ifndef TOOLS_TARGET_WINDOWS
#define TOOLS_TARGET_WINDOWS
#endif

#ifndef NOMINMAX
#define NOMINMAX // Windows min/max macros interfere with STL
#endif

// Modify the following defines if you have to target a platform prior to the
// ones specified below. Refer to MSDN for the latest info on corresponding
// values for different platforms.
#ifndef WINVER // Allow use of features specific to Windows XP or later.
#define WINVER                                                                                                         \
   0x0600 // Change this to the appropriate value to target other versions of
          // Windows.
#endif

#ifndef TOOLS_ARCH_64BIT
#define TOOLS_ARCH_64BIT
#endif

#elif defined _WIN32 // _Win32 is ALWAYS define, even on 64 bit, so test for 64
                     // bit first

#ifndef WIN32
#define WIN32
#endif
#ifndef TOOLS_TARGET_WINDOWS
#define TOOLS_TARGET_WINDOWS
#endif

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used windows stuff

#ifndef NOMINMAX
#define NOMINMAX // Windows min/max macros interfere with STL
#endif

// Modify the following defines if you have to target a platform prior to the
// ones specified below. Refer to MSDN for the latest info on corresponding
// values for different platforms.
#ifndef WINVER // Allow use of features specific to Windows XP or later.
#define WINVER                                                                                                         \
   0x0601 // Change this to the appropriate value to target other versions of
          // Windows.
#endif

#ifndef _WIN32_WINNT // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT                                                                                                   \
   0x0601 // Change this to the appropriate value to target other versions of
          // Windows.
#endif

#ifndef _WIN32_WINDOWS // Allow use of features specific to Windows 98 or
                       // later.
#define _WIN32_WINDOWS                                                                                                 \
   0x0410 // Change this to the appropriate value to target Windows Me or
          // later.
#endif

#ifndef _WIN32_IE // Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE                                                                                                      \
   0x0600 // Change this to the appropriate value to target other versions of
          // IE.
#endif

#ifndef TOOLS_ARCH_32BIT
#define TOOLS_ARCH_32BIT
#endif

#else
#error Unknown Target
#endif


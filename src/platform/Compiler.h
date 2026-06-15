// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#if defined __APPLE_CC__ // Must be first, APPLE compiler also defines __GNUC__
#define TOOLS_COMPILER_CLANG
#elif defined __GNUC__
#define TOOLS_COMPILER_GNUC
#elif defined _MSC_VER
#define TOOLS_COMPILER_MSVC
#pragma warning(disable : 4351) // New behavior, arrays can be default init-ed
#else
#error Unknown Compiler
#endif

#if defined _DEBUG
#define TOOLS_MODE_DEBUG
#endif

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

// Shared export definitions for the C and C++ SDK headers.
// QIL_STATIC: building or consuming the static library.
// QIL_DLL_EXPORTS: building the shared library; consumers leave it undefined.
// SOFTWAREDOWNLOADLIB_* names remain supported for existing C API users.
#if defined(QIL_STATIC) || defined(SOFTWAREDOWNLOADLIB_STATIC)
#  define QIL_API
#elif defined(_WIN32)
#  if defined(QIL_DLL_EXPORTS) || defined(SOFTWAREDOWNLOADLIB_EXPORTS)
#    define QIL_API __declspec(dllexport)
#  else
#    define QIL_API __declspec(dllimport)
#  endif
#else
#  define QIL_API __attribute__((visibility("default")))
#endif

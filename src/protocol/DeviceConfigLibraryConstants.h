// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#ifdef _WIN_SOURCE

#ifdef _MAKE_DCL_DLL
#define DCL_EXPORT __declspec(dllexport)
#elif _MAKE_DCL_STATIC_LIB
#define DCL_EXPORT
#else
#define DCL_EXPORT __declspec(dllimport)
#endif

#else // _WIN_SOURCE
#define DCL_EXPORT
#endif

enum DCL_RESULT_CODE
{
   DCL_SUCCESS = 0,
   DCL_UNKNOWN_ERROR,
   DCL_NOT_FOUND,
   DCL_BAD_INPUT,
   DCL_NOT_SUPPORTED,
   DCL_DB_NOT_OPEN,
   DCL_DB_ALREADY_OPEN,
   DCL_UNSUPPORTED_CHIPSET
};

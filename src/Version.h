// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#define STRINGIZE2(S) #S
#define STRINGIZE(S) STRINGIZE2(S)

#define MAJOR_VERSION 1
#define MINOR_VERSION 3
#define PATCH_VERSION 1

#define VERSION_STRING "v" STRINGIZE(MAJOR_VERSION) "." STRINGIZE(MINOR_VERSION) "." STRINGIZE(PATCH_VERSION)
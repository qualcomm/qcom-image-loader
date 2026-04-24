// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "Definitions.h"
#include "ImageManagementDefinitions.h"

#include <map>

namespace QC {
#ifdef TOOLS_TARGET_WINDOWS

#elif defined TOOLS_TARGET_LINUX

#endif
class SoftwareDownloadUtility
{
public:
   SoftwareDownloadUtility();
   virtual ~SoftwareDownloadUtility();

   // Create VIP Digest
   static ErrorType
   createDigestsForVipDownload(std::string buildPath, DownloadBuildOptions options, std::string outputPath);
   // Create Build Validation Digest
   static ErrorType
   createDigestsForBuildValidation(std::string buildPath, DownloadBuildOptions options, std::string outputPath);
   static void clean();
};
} // namespace QC

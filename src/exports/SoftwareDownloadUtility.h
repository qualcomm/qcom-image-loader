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
class QIL_API SoftwareDownloadUtility
{
public:
   // Defined inline: this class only holds static methods, and an exported
   // class instantiates every member, so an out-of-line ctor/dtor would need a
   // definition that no translation unit provides.
   SoftwareDownloadUtility() = default;
   virtual ~SoftwareDownloadUtility() = default;

   // Create VIP Digest
   static ErrorType
   createDigestsForVipDownload(std::string buildPath, DownloadBuildOptions options, std::string outputPath);
   // Create Build Validation Digest
   static ErrorType
   createDigestsForBuildValidation(std::string buildPath, DownloadBuildOptions options, std::string outputPath);
   // Flatten Meta-Build
   static ErrorType
   flattenMeta(const std::string& buildPath, const FlattenMetaBuildOptions& options);
   static void clean();
};
} // namespace QC

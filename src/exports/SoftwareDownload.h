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
class SoftwareDownload
{
public:
   SoftwareDownload(DeviceInfo deviceInfo);
   virtual ~SoftwareDownload();

   ErrorType initializeService();
   ErrorType destroyService();

   ErrorType downloadBuild(const std::string& buildPath, const DownloadBuildOptions& options);
   ErrorType setDdrStorePath(const std::string& path);
   ErrorType resetDevice(const int32_t timeout, const bool skipSahra);
   ErrorType erasePartition(const DownloadBuildOptions& options);
   ErrorType getFlashInfo(std::vector<FlashInfo>& flashInfo, const DownloadBuildOptions& options);

private:
   uint64_t m_deviceHandle;
};
} // namespace QC

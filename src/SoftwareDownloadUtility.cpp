// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "SoftwareDownloadUtility.h"

#include "Globals.h"
#include "service/ImageManagementServiceHandler.h"
#include "service/UtilityServiceHandler.h"
#include "Version.h"

#include <iostream>

namespace QC {
ErrorType SoftwareDownloadUtility::
   createDigestsForVipDownload(std::string buildPath, DownloadBuildOptions options, std::string outputPath)
{
   QC::ErrorType result;
   std::shared_ptr<Service::UtilityServiceHandler> pUtility;
   try
   {
      pUtility = std::make_shared<Service::UtilityServiceHandler>();
      result.errorCode = pUtility->initializeService();
      if(result.errorCode == QC::ErrorCode::DEVICE_NO_ERROR)
      {
         result.errorCode = pUtility->createDigestsForVipDownload(buildPath, options, outputPath);
      }
   }
   LIB_CATCH
   if(pUtility != nullptr)
   {
      pUtility->destroyService();
   }
   return result;
}

ErrorType SoftwareDownloadUtility::
   createDigestsForBuildValidation(std::string buildPath, DownloadBuildOptions options, std::string outputPath)
{
   QC::ErrorType result;
   std::shared_ptr<Service::UtilityServiceHandler> pUtility;
   try
   {
      pUtility = std::make_shared<Service::UtilityServiceHandler>();
      result.errorCode = pUtility->initializeService();
      if(result.errorCode == QC::ErrorCode::DEVICE_NO_ERROR)
      {
         result.errorCode = pUtility->createDigestsForBuildValidation(buildPath, options, outputPath);
      }
   }
   LIB_CATCH
   if(pUtility != nullptr)
   {
      pUtility->destroyService();
   }
   return result;
}

void SoftwareDownloadUtility::clean()
{
}
} // namespace QC

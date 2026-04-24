// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "SoftwareDownload.h"

#include "Globals.h"
#include "rpc/ImageManagementServiceHandler.h"
#include "tracker/FunctionTracker.h"

typedef std::pair<uint64_t, std::shared_ptr<Rpc::ImageManagementServiceHandler>> GLOBAL_HANDLER;
typedef std::unordered_map<uint64_t, std::shared_ptr<Rpc::ImageManagementServiceHandler>> GLOBAL_HANDLER_MAP;
static GLOBAL_HANDLER_MAP g_handlers;

namespace QC {

void PrintMessage(std::string message)
{
#ifdef TOOLS_TARGET_WINDOWS
#elif defined TOOLS_TARGET_LINUX
#endif
}

SoftwareDownload::SoftwareDownload(DeviceInfo deviceInfo)
{
   if(g_handlers.find(deviceInfo.deviceHandle) == g_handlers.end())
   {
      m_deviceHandle = deviceInfo.deviceHandle;
      std::string serviceName = "SoftwareDownload_" + std::to_string(m_deviceHandle);
      g_handlers.insert(
         GLOBAL_HANDLER(deviceInfo.deviceHandle, std::make_shared<Rpc::ImageManagementServiceHandler>(serviceName))
      );
   }
}
SoftwareDownload::~SoftwareDownload()
{
   g_handlers.erase(m_deviceHandle);
}

ErrorType SoftwareDownload::initializeService()
{
   QC::ErrorType result;
   try
   {
      QFS_PRINT_FUNCTION
      result.errorCode =
         Rpc::DeviceManagerHandler::getInstance()->attachService(m_deviceHandle, g_handlers[m_deviceHandle]);
   }
   LIB_CATCH
   return result;
}
ErrorType SoftwareDownload::destroyService()
{
   QC::ErrorType result;
   try
   {
      QFS_CHECK_INITIALIZE
      result.errorCode = g_handlers[m_deviceHandle]->destroyService();
   }
   LIB_CATCH
   return result;
}

ErrorType SoftwareDownload::downloadBuild(const std::string& buildPath, const DownloadBuildOptions& options)
{
   QC::ErrorType result;
   try
   {
      QFS_CHECK_INITIALIZE
      result.errorCode = g_handlers[m_deviceHandle]->downloadBuild(buildPath, options);
   }
   LIB_CATCH
   return result;
}
ErrorType SoftwareDownload::setDdrStorePath(const std::string& path)
{
   QC::ErrorType result;
   try
   {
      QFS_CHECK_INITIALIZE
      result.errorCode = g_handlers[m_deviceHandle]->setDdrStorePath(path);
   }
   LIB_CATCH
   return result;
}
ErrorType SoftwareDownload::resetDevice(const int32_t timeout, const bool skipSahara)
{
   QC::ErrorType result;
   try
   {
      QFS_CHECK_INITIALIZE
      result.errorCode = g_handlers[m_deviceHandle]->resetDevice(timeout, skipSahara);
   }
   LIB_CATCH
   return result;
}
ErrorType SoftwareDownload::erasePartition(const DownloadBuildOptions& options)
{
   QC::ErrorType result;
   try
   {
      QFS_CHECK_INITIALIZE
      result.errorCode = g_handlers[m_deviceHandle]->erasePartition(options);
   }
   LIB_CATCH
   return result;
}

ErrorType SoftwareDownload::getFlashInfo(std::vector<FlashInfo>& flashInfo, const DownloadBuildOptions& options)
{
   QC::ErrorType result;
   try
   {
      QFS_CHECK_INITIALIZE
      g_handlers[m_deviceHandle]->getFlashInfo(flashInfo, options);
      result.errorCode = QC::ErrorCode::DEVICE_NO_ERROR;
   }
   LIB_CATCH
   return result;
}
} // namespace QC

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "service/UtilityServiceHandler.h"

#include "callback/ClientCallbackHandler.h"
#include "function/ImageTransfer.h"
#include "tracker/FunctionTracker.h"

namespace Service {

// ----------------------------------------------------------------------------
// UtilityServiceHandler
//
// ----------------------------------------------------------------------------
UtilityServiceHandler::UtilityServiceHandler()
: Service::ServiceHandlerBase("UtilityService")
{
   m_pPublisher = std::make_shared<DummyMessagePublisher>();
}

// ----------------------------------------------------------------------------
// ~UtilityServiceHandler
//
// ----------------------------------------------------------------------------
UtilityServiceHandler::~UtilityServiceHandler()
{
   m_pPublisher = nullptr;
}

QC::ErrorCode::type UtilityServiceHandler::initializeService()
{
   Device::Manager::getInstance()->setApplicationInfo(
      QC::getTempDirectory(),
      QC::getName(),
      QC::getAppMajorVersion(),
      QC::getAppMinorVersion(),
      QC::getAppBuildId(),
      QC::getAllUsersDirectory()
   );

   // Store reference to ClientCallbackHandler to avoid singleton access during
   // destruction
   m_pClientCallbackHandler = QC::ClientCallbackHandler::getInstance();

   m_pClientCallbackHandler->subscribeForServiceAsyncEvents(getServiceHandlerPtr());
   setInitialized();
   return QC::ErrorCode::DEVICE_NO_ERROR;
}

QC::ErrorCode::type UtilityServiceHandler::destroyService()
{
   // Use stored reference instead of singleton access to avoid static
   // destruction order issues
   if(m_pClientCallbackHandler.isAlive())
   {
      m_pClientCallbackHandler->unsubscribeServiceAsyncEvents(getServiceHandlerPtr());
      m_pClientCallbackHandler = nullptr; // Clear the reference
   }
   m_bInitialized = false;
   return QC::ErrorCode::DEVICE_NO_ERROR;
}

// ----------------------------------------------------------------------------
// sendEvent
//
// ----------------------------------------------------------------------------
void UtilityServiceHandler::sendEvent(EventId eventId, const std::string& description)
{
   notifyAsync(std::make_shared<ServiceEvent>(getName(), eventId, description));
}

// ----------------------------------------------------------------------------
// onImageTransferEvent
//
/// Pushes the event to the RPC client
// ----------------------------------------------------------------------------
void UtilityServiceHandler::onImageTransferEvent(Function::ImageTransferEvent* pEvent)
{
   sendEvent(pEvent->getEventId(), pEvent->getDescription());
}

// ----------------------------------------------------------------------------
// offlineFirehoseProcess
//
/// Run a offline firehose process
// ----------------------------------------------------------------------------
void UtilityServiceHandler::offlineFirehoseProcess(
   const int32_t process,
   const std::string& buildPath,
   const QC::DownloadBuildOptions& options,
   const std::string& outputPath
)
{
   {
      Device::ConnectionPtr pSaharaConnection;
      Device::ConnectionPtr pFirehoseConnection;
      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(pSaharaConnection, pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &UtilityServiceHandler::onImageTransferEvent));

      TOOLS_ASSERT_OR_THROW(
         (QC::MemoryType::MEMORY_TYPE_UNKNOWN > options.memoryType),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Unsupported memory type : " + std::to_string(static_cast<int32_t>(options.memoryType))
         )
      );
      // Select memory type
      pImageTransfer->firehoseSetMemoryType(static_cast<Function::ImageTransfer::MemoryType>(options.memoryType));
      // Select slot
      if(options.__isset.slot)
      {
         pImageTransfer->firehoseSetSlot(options.slot);
      }
      // Active partition
      if(options.__isset.activePartition)
      {
         pImageTransfer->firehoseSetActivePartition(options.activePartition);
      }
      // ZLP aware host
      if(options.__isset.zlpAwareHost)
      {
         pImageTransfer->firehoseSetZlpAwareHost(options.zlpAwareHost ? Function::ImageTransfer::ZLP_AWARE_HOST_ENABLE
                                                             : Function::ImageTransfer::ZLP_AWARE_HOST_DISABLE);
      }
      // vip download
      // if (options.__isset.signedDigestsPath &&
      // !options.signedDigestsPath.empty())
      //{
      //    std::filesystem::path digestsPath =
      //    Device::Manager::getInstance()->getAccessiblePath(
      //       options.signedDigestsPath.c_str()
      //       );
      //    imageTransfer->firehoseSetSignedDigestsFile(digestsPath);
      // }
      // if (options.__isset.chainedDigestsPath &&
      // !options.chainedDigestsPath.empty())
      //{
      //    std::filesystem::path digestsPath =
      //    Device::Manager::getInstance()->getAccessiblePath(
      //       options.chainedDigestsPath.c_str()
      //       );
      //    imageTransfer->firehoseSetChainedDigestsFile(digestsPath);
      // }

      if(options.__isset.maxPayloadSize)
      {
         pImageTransfer->firehoseSetMaxPayloadSize(options.maxPayloadSize);
      }

      if(options.__isset.readImagesPath && !options.readImagesPath.empty())
      {
         // Read Binary: Read images from device if readImagesPath is present,
         // otherwise start firehose download
         std::filesystem::path availableOutputPath;
         availableOutputPath =
            Device::Manager::getInstance()
               ->getAccessiblePath(std::filesystem::path(options.readImagesPath), std::filesystem::path(), true);
         pImageTransfer->firehoseSetOutpuDir(availableOutputPath);
      }

      if(options.__isset.firmwarePath && !options.firmwarePath.empty())
      {
         // write firmware
         std::filesystem::path availableFirmwarePath;
         availableFirmwarePath =
            Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(options.firmwarePath));
         pImageTransfer->firehoseSetFirmwarePath(availableFirmwarePath.wstring());
      }
      else if(options.__isset.ufsProvisioningPath && !options.ufsProvisioningPath.empty())
      {
         // ufs provision
         std::filesystem::path availableProvisionPath;
         availableProvisionPath =
            Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(options.ufsProvisioningPath));
         pImageTransfer->firehoseSetProvisionPath(availableProvisionPath.wstring());
      }
      else
      {
         if(options.__isset.readImages && true == options.readImages)
         {
            // Read Binary: Read images from device is the flag is set to true,
            // otherwise start firehose download
            pImageTransfer->firehoseSetReadImages(true);
         }
         else
         {
            // download
            // validation mode
            if(options.__isset.validationMode)
            {
               pImageTransfer->firehoseSetValidationMode(static_cast<
                                                         Function::ImageTransfer::ValidationMode>(options.validationMode
               ));
            }

            // Preserve partition setting
            TOOLS_ASSERT_OR_THROW(
               !(options.__isset.preservationOption && options.preservationOption.__isset.preservationMode &&
                 options.preservationOption.preservationMode != QC::PreservationMode::PRESERVATION_NONE &&
                 options.preservationOption.__isset.preservedPartitions &&
                 options.preservationOption.preservedPartitions.size() > 0),
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Preserve partition is not supported for VIP "
                  "process"
               )
            );

            // Prepare erase string to be sent to device
            // Erase flash, only availible in download
            if(options.__isset.erase && options.erase)
            {
               pImageTransfer->firehoseSetEnableEraseBeforeDownload(true);
            }
         }
         // download
         if(options.__isset.jsonPath && !options.jsonPath.empty())
         {
            // Json Download
            //  Use Json path for XMLs and images if given. Meta Build Download
            std::filesystem::path availableJsonPath;
            availableJsonPath =
               Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(options.jsonPath));
            pImageTransfer->firehoseSetJsonPath(availableJsonPath);
         }
         else
         {
            // Flat build Download
            std::vector<std::filesystem::path> rawXmlList;
            std::vector<std::filesystem::path> patchXmlList;
            if(options.__isset.rawXmlList && options.rawXmlList.size() > 0)
            {
               // Check if the file exists
               for(std::vector<std::string>::const_iterator it = options.rawXmlList.begin();
                   it != options.rawXmlList.end();
                   it++)
               {
                  std::string path = *it;
                  rawXmlList.push_back(std::filesystem::path(path.c_str()));
               }
               pImageTransfer->firehoseSetRawXmlList(rawXmlList);
            }
            if(options.__isset.patchXmlList && options.patchXmlList.size() > 0)
            {
               // Check if the file exists
               for(std::vector<std::string>::const_iterator it = options.patchXmlList.begin();
                   it != options.patchXmlList.end();
                   it++)
               {
                  std::string path = *it;
                  patchXmlList.push_back(std::filesystem::path(path.c_str()));
               }
               pImageTransfer->firehoseSetPatchXmlList(patchXmlList);
            }
         }
         // Check if we need to reset post download
         if(options.__isset.resetAfterDownload && options.resetAfterDownload)
         {
            pImageTransfer->firehoseSetEnableResetAfterDownload(true);
         }

         std::vector<int32_t> partitionIndexList;
         if(options.__isset.partitionIndexList && options.partitionIndexList.size() > 0)
         {
            pImageTransfer->firehoseSetPartitionIndexList(options.partitionIndexList);
         }
      }

      if(options.__isset.digestHeaderType)
      {
         pImageTransfer->firehoseSetDigestHeaderType(static_cast<
                                                     Function::ImageTransfer::DigestHeaderType>(options.digestHeaderType
         ));
      }

      std::filesystem::path availableBuildPath =
         Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(buildPath));
      std::filesystem::path availableOutputPath =
         Device::Manager::getInstance()
            ->getAccessiblePath(std::filesystem::path(outputPath), std::filesystem::path(), true);

      pImageTransfer->firehoseFormatDownloadBuildParameter(availableBuildPath);
      pImageTransfer->firehoseRunOfflineProcess(
         static_cast<Function::ImageTransfer::OfflineProcess>(process),
         availableBuildPath,
         availableOutputPath
      );

      pImageTransfer->unsubscribe(this, &UtilityServiceHandler::onImageTransferEvent);
   }
}

// ----------------------------------------------------------------------------
// createDigestsforVIPDownload
//
/// Create Digest files DigestsToSign.bin.mbn, ChainedTableOfDigests.bin
/// These will be created in the path specified by the --out parameter
/// passed in by the user.
/// Client needs to sign the DigestsToSign.bin.mbn separately before triggering
/// VIP download through downloadBuild()
/// Signing can be done offline using METABuild\common\sectools\sectools.py
// ----------------------------------------------------------------------------
::QC::ErrorCode::type UtilityServiceHandler::createDigestsForVipDownload(
   const std::string& buildPath,
   const QC::DownloadBuildOptions& options,
   const std::string& outputPath
)
{
   DEVICE_RPC_TRY(PARAMETER(buildPath) PARAMETER(options) PARAMETER(outputPath))
   {
      offlineFirehoseProcess(
         static_cast<int32_t>(Function::ImageTransfer::OfflineProcess::PROCESS_VIP_DIGEST),
         buildPath,
         options,
         outputPath
      );
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// createDigestsForBuildValidation
//
/// Create build validation digest file which canbe used for build validation
/// download through downloadBuild()
// ----------------------------------------------------------------------------
::QC::ErrorCode::type UtilityServiceHandler::createDigestsForBuildValidation(
   const std::string& buildPath,
   const QC::DownloadBuildOptions& options,
   const std::string& outputPath
)
{
   DEVICE_RPC_TRY(PARAMETER(buildPath) PARAMETER(options) PARAMETER(outputPath))
   {
      offlineFirehoseProcess(
         static_cast<int32_t>(Function::ImageTransfer::OfflineProcess::PROCESS_BUILD_VALIDATION_DIGEST),
         buildPath,
         options,
         outputPath
      );
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

} // namespace Service

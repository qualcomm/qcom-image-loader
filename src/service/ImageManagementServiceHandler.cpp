// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "service/ImageManagementServiceHandler.h"

#include "communication/CommonIO.h"
#include "device/Buffer.h"
#include "device/Impl.h"
#include "function/ImageTransfer.h"
#include "protocol/Fwd.h"
#include "protocol/Sahara.h"
#include "report/Thread.h"
#include "tracker/FunctionTracker.h"
#include "util/Event.h"
#include "util/MemoryHelper.h"
#include "util/StringHelper.h"
#include "util/ThisThread.h"

#include <chrono>
#include <regex>
DEFINE_ENUM_PARAMETER_LOGGING(DeviceImageMode);
DEFINE_ENUM_PARAMETER_LOGGING(MemoryType);

// To throw Exception since TOOLS_CATCH can not throw
#define IMGMGMT_CATCH(action)                                                                                          \
   catch(...)                                                                                                          \
   {                                                                                                                   \
      TOOLS_IGNORE_EXCEPTIONS(action);                                                                                 \
      throw;                                                                                                           \
   }

namespace Service {

TOOLS_STATIC_ASSERT(
   static_cast<int32_t>(Function::ImageTransfer::MEMORY_TYPE_EMMC
   ) == static_cast<int32_t>(QC::MemoryType::MEMORY_TYPE_EMMC),
   "MEMORY_TYPE_EMMC mismatch"
);
TOOLS_STATIC_ASSERT(
   static_cast<int32_t>(Function::ImageTransfer::MEMORY_TYPE_UFS
   ) == static_cast<int32_t>(QC::MemoryType::MEMORY_TYPE_UFS),
   "MEMORY_TYPE_UFS mismatch"
);
TOOLS_STATIC_ASSERT(
   static_cast<int32_t>(Function::ImageTransfer::MEMORY_TYPE_NAND
   ) == static_cast<int32_t>(QC::MemoryType::MEMORY_TYPE_NAND),
   "MEMORY_TYPE_NAND mismatch"
);
TOOLS_STATIC_ASSERT(
   static_cast<int32_t>(Function::ImageTransfer::MEMORY_TYPE_NVME
   ) == static_cast<int32_t>(QC::MemoryType::MEMORY_TYPE_NVME),
   "MEMORY_TYPE_NVME mismatch"
);
TOOLS_STATIC_ASSERT(
   static_cast<int32_t>(Function::ImageTransfer::MEMORY_TYPE_SPINOR
   ) == static_cast<int32_t>(QC::MemoryType::MEMORY_TYPE_SPINOR),
   "MEMORY_TYPE_SPINOR mismatch"
);
TOOLS_STATIC_ASSERT(
   static_cast<int32_t>(Function::ImageTransfer::MEMORY_TYPE_UNKNOWN
   ) == static_cast<int32_t>(QC::MemoryType::MEMORY_TYPE_UNKNOWN),
   "MEMORY_TYPE_UNKNOWN mismatch"
);

static constexpr auto SAHARA_EDL_SWITCH_WAIT_PERIOD = std::chrono::milliseconds(60000);

static constexpr auto FIREHOSE_READY_WAIT_PERIOD = std::chrono::seconds(3);

static constexpr auto CONNECTION_WORKER_PROTOCOL_ENUMERATION_WAIT_INTERVAL = std::chrono::seconds(4);

static constexpr auto CONNECTION_WORKER_READY_WAIT_PERIOD =
   std::chrono::seconds(1) + CONNECTION_WORKER_PROTOCOL_ENUMERATION_WAIT_INTERVAL;

static constexpr auto CONNECTION_WORKER_PROTOCOL_WAIT_INTERVAL = std::chrono::seconds(1);

static const std::string FIREHOSE_PROGRAMMER_FILTER = "*firehose*.*";

static const int32_t MAX_PARTITION_NUM = -1;
static const int32_t MAGIC_PARTITION_UFS_EMMC = 65210;
static const int32_t DEFAULT_PARTITION_NON_UFS_EMMC = 0;
static const std::string STRING_PATTERN_MHI = "mhi.*?";


// ----------------------------------------------------------------------------
// ImageManagementConnectionWorker
//
/// Maintains Diag and QMI connections during re-enumeration
// ----------------------------------------------------------------------------
class ImageManagementConnectionWorker : public Util::StdThreadWrapper::Work
{
   TOOLS_FORBID_COPY(ImageManagementConnectionWorker);

public:
   ImageManagementConnectionWorker(const std::shared_ptr<ImageManagementServiceHandler>& pParent)
   : Util::StdThreadWrapper::Work(
        QC::getClientName() + " Image Management Service Connection Thread",
        Util::StdThreadWrapper::HealthMonitorPriority::Low
     )
   , m_pParent(pParent)
   , m_protocolAddedEvent(true)
   {
   }

   virtual ~ImageManagementConnectionWorker()
   {
   }

   virtual void onRun()
   {
      try
      {
         while(!isStopSignaled())
         {
            FLOG_INFO(
               "ImageManagementConnectionWorker scan - mode: " +
               std::string(std::to_string(m_pParent->m_deviceImageMode))
            );


            Device::Impl::ProtocolList protocols = m_pParent->m_pDevice->getProtocolList();
            Device::Impl::ProtocolList::const_iterator it = protocols.begin();
            Device::Impl::ProtocolList::const_iterator end = protocols.end();
            m_pParent->m_bSaharaProtocolAdded = false;
            for(; !isStopSignaled() && it != end; ++it)
            {
               switch(Device::Manager::getInstance()->getProtocolType((*it)->getHandle()))
               {
                  case Device::ProtocolType::PROT_SAHARA: {
                     m_pParent->m_pSaharaProtocol =
                        ((*it)->getOverrideProtocol()).dynamicCast<Device::Protocol::Sahara>();
                     m_pParent->m_bSaharaProtocolAdded = true;
                     Device::Protocol::Sahara::Mode mode = m_pParent->m_pSaharaProtocol->getMode();
                     if(Device::Protocol::Sahara::Mode::MODE_MEMORY_DEBUG == mode)
                     {
                        m_pParent->m_deviceImageMode = QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_CRASH;
                     }
                     else if(Device::Protocol::Sahara::Mode::MODE_EFS_SYNC == mode)
                     {
                        m_pParent->m_deviceImageMode = QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_EFS_SYNC;
                     }
                     else if(Device::Protocol::Sahara::Mode::MODE_IMAGE_TX_PENDING == mode ||
                             Device::Protocol::Sahara::Mode::MODE_IMAGE_TX_COMPLETE == mode)
                     {
                        m_pParent->m_deviceImageMode = QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_DOWNLOAD;
                     }
                     else
                     {
                        m_pParent->m_deviceImageMode = QC::DeviceImageMode::DEVICE_IMAGE_MODE_NONE;
                     }

                     FLOG_INFO(
                        "ImageManagementConnectionWorker - sahara "
                        "available in mode: " +
                        std::string(std::to_string(m_pParent->m_deviceImageMode))
                     );

                     m_pParent->m_saharaReadyEvent.signal();
                     break;
                  }
               }
            }
            m_pParent->m_connectionWorkerEvent.signal();

            FLOG_INFO(
               "ImageManagementConnectionWorker wait for protocol - mode: " +
               std::string(std::to_string(m_pParent->m_deviceImageMode))
            );

            while(!isStopSignaled())
            {
               if(Util::ThisThread::waitForEvent(&m_protocolAddedEvent, CONNECTION_WORKER_PROTOCOL_WAIT_INTERVAL))
               {
                  m_pParent->m_connectionWorkerEvent.reset(); // reset the event and indicate a detection
                                                              // ongoing
                  // add a wait time here and expect add the protocol to be
                  // enumerated
                  Util::ThisThread::sleep(CONNECTION_WORKER_PROTOCOL_ENUMERATION_WAIT_INTERVAL);
                  m_protocolAddedEvent.reset();
                  break;
               }
            }
         }
      }
      TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
   }

private:
   friend class ImageManagementServiceHandler;

   Util::CheckedPointer<ImageManagementServiceHandler> m_pParent;
   Util::Event m_protocolAddedEvent; ///< Event from device manager indicating
                                     ///< the arrival of a protocol
};

// ----------------------------------------------------------------------------
// ImageManagementServiceHandler
//
// ----------------------------------------------------------------------------
ImageManagementServiceHandler::ImageManagementServiceHandler(std::string serviceName)
: Service::ServiceHandlerBase(serviceName)
, m_pSaharaProtocol()
, m_pSaharaConnection()
, m_pFirehoseProtocol()
, m_pFirehoseConnection()
, m_connectionWorkerEvent(true, false)
, m_saharaReadyEvent(true)
, m_deviceReconnectedEvent()
, m_ddrStorePath()
, m_deviceImageMode(QC::DeviceImageMode::DEVICE_IMAGE_MODE_NONE)
, m_bEntertedFirehoseMode()
, m_memoryType(QC::MemoryType::MEMORY_TYPE_UNKNOWN)
, m_validationMode(QC::ValidationMode::VALIDATION_MODE_NONE)
, m_availableOutputPath()
, m_numPartition()
, m_maxPayloadSize()
, m_preservationMode(QC::PreservationMode::PRESERVATION_NONE)
, m_preservedPartitions()
, m_pageSize(0)
, m_pagePerBlock(0)
, m_bSaharaProtocolAdded(false)
, m_serialNumber(0xFFFFFFFFU)
, m_msmHwId(0xFFFFFFFFU)
, m_mutex()
, m_slot(0)
{
   m_pDeviceManagerHandler = Service::DeviceManagerHandler::getInstance();
   m_pPublisher = m_pDeviceManagerHandler->getPublisher();
}

// ----------------------------------------------------------------------------
// ~ImageManagementServiceHandler
//
// ----------------------------------------------------------------------------
ImageManagementServiceHandler::~ImageManagementServiceHandler()
{
   TOOLS_IGNORE_EXCEPTIONS(doDestroy());
}

// ----------------------------------------------------------------------------
// isServiceLocked
//
// ----------------------------------------------------------------------------
bool ImageManagementServiceHandler::isServiceLocked()
{
   // LOG_TRACE(
   //     m_pLogger,
   //     "Checking if ImageManagementService is locked by any clients."
   //);
   //
   // QC::ServiceLockInfo lockInfo;
   // std::vector<QC::ServiceLockInfo> activeLocks;

   // lockInfo.__set_serviceName(QC::g_ImageManagementService_constants.IMAGE_MANAGEMENT_SERVICE_NAME);
   // lockInfo.__set_deviceHandle(m_pDevice->getHandle());
   // m_pDeviceManagerHandler->getServiceManager()->getServiceLockInfo(activeLocks,
   // lockInfo);

   // if (activeLocks.size() > 0)
   //{
   //     LOG_INFO(
   //         m_pLogger,
   //         "Active service lock count=" + std::to_string(activeLocks.size())
   //     );
   // }
   // return (0 < activeLocks.size());
   return false;
}

// ----------------------------------------------------------------------------
// initializeService
//
/// Opens the connection to the device.
// ----------------------------------------------------------------------------
QC::ErrorCode::type ImageManagementServiceHandler::initializeService(Device::ImplPtr pDevice)
{
   m_pDevice = pDevice;
   return initializeService();
}

QC::ErrorCode::type ImageManagementServiceHandler::initializeService()
{
   DEVICE_RPC_TRY_UNINITIALIZED(TOOLS_VOID)
   {
      setInitialized();

      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      m_pImageManagementConnectionWorker = std::make_shared<ImageManagementConnectionWorker>(shared_from_this());
      m_pImageManagementConnectionWorkerThread =
         std::make_shared<Util::StdThreadWrapper>(m_pImageManagementConnectionWorker);
      m_pImageManagementConnectionWorkerThread->start();
      Device::Manager::getInstance()
         ->subscribeForAsyncEvents(this, &ImageManagementServiceHandler::onProtocolAddedChange);
      Device::Manager::getInstance()
         ->subscribeForAsyncEvents(this, &ImageManagementServiceHandler::onProtocolRemovedChange);
   }
   DEVICE_RPC_CATCH;

   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// destroyService
//
/// Closes this service and shuts it down on the RPC server
// ----------------------------------------------------------------------------
QC::ErrorCode::type ImageManagementServiceHandler::destroyService()
{
   DEVICE_RPC_TRY_UNINITIALIZED(TOOLS_VOID)
   {
      doDestroy();
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// getDeviceImageMode
//
/// Returns current image management scheme
// ----------------------------------------------------------------------------
QC::DeviceImageMode::type ImageManagementServiceHandler::getDeviceImageMode()
{
   DEVICE_RPC_TRY(TOOLS_VOID)
   {
   }
   DEVICE_RPC_CATCH;
   return m_deviceImageMode;
}

// ----------------------------------------------------------------------------
// getDeviceInfo
//
/// Get related device info in EDL mode
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::getDeviceInfo(QC::EdlDeviceInfo& edlDeviceInfo)
{
   DEVICE_RPC_TRY(TOOLS_VOID)
   {
      if(isServiceLocked())
      {
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_SERVICE_LOCKED, "Image management service locked"));
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);
      // Constraint for EDL mode now
      TOOLS_ASSERT_OR_THROW(
         QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_DOWNLOAD == m_deviceImageMode,
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Download mode not available")
      );

      if(0xFFFFFFFFU != m_serialNumber && 0xFFFFFFFFU != m_msmHwId)
      {
         std::ostringstream serialNumber;
         serialNumber << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << m_serialNumber;
         edlDeviceInfo.serialNumber = serialNumber.str().c_str();
         std::ostringstream msmHwId;
         msmHwId << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << m_msmHwId;
         edlDeviceInfo.msmHwId = msmHwId.str().c_str();
         return;
      }
      doSaharaConnect();

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         Function::ImageTransfer::EdlDeviceInfo deviceInfo;
         pImageTransfer->getDeviceInfo(deviceInfo);
         // Reserve infomation until protocol removed
         m_serialNumber = deviceInfo.serialNumber;
         m_msmHwId = deviceInfo.msmHwId;
         std::ostringstream serialNumber;
         serialNumber << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << deviceInfo.serialNumber;
         edlDeviceInfo.serialNumber = serialNumber.str().c_str();
         std::ostringstream msmHwId;
         msmHwId << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << deviceInfo.msmHwId;
         edlDeviceInfo.msmHwId = msmHwId.str().c_str();
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
   }
   DEVICE_RPC_CATCH;
}

// ----------------------------------------------------------------------------
// transferImages
//
/// Send images to device from binary file list using Sahara protocol
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::
   transferImages(QC::TransferImageResult& transferImageResult, const std::map<int32_t, std::string>& imageList)
{
   DEVICE_RPC_TRY(PARAMETER(imageList))
   {
      if(isServiceLocked())
      {
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_SERVICE_LOCKED, "Image management service locked"));
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);
      TOOLS_ASSERT_OR_THROW(
         QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_DOWNLOAD == m_deviceImageMode,
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Download mode not available")
      );

      doSaharaConnect();

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         if(!m_ddrStorePath.empty()) // If user has called setDdrStorePath API to specify
                                     // path beforehand, use it, else use default path.
         {
            pImageTransfer->setDdrStorePath(m_ddrStorePath);
         }

         Function::ImageTransfer::BuildImageList buildImageList;
         std::map<int32_t, std::string>::const_iterator it = imageList.begin();
         std::map<int32_t, std::string>::const_iterator end = imageList.end();
         for(; it != end; ++it)
         {
            buildImageList
               .insert(std::make_pair(static_cast<uint32_t>(it->first), std::filesystem::path(it->second.c_str())));
         }

         Device::Protocol::Sahara::Mode mode;
         mode = pImageTransfer->transferImageList(buildImageList);

         if(Device::Protocol::Sahara::Mode::MODE_MEMORY_DEBUG == mode)
         {
            m_deviceImageMode = QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_CRASH;
         }
         else
         {
            m_deviceImageMode = QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_DOWNLOAD;
         }
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);

      transferImageResult.deviceImageMode = m_deviceImageMode;
   }
   DEVICE_RPC_CATCH;
   transferImageResult.errorCode = static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// doFirehoseGetFlashInfo
//
/// FireHose Process Get Flash or Storage information
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doFirehoseGetFlashInfo(
   QC::FlashInfo& flashInfo,
   const QC::MemoryType::type memoryType,
   int32_t partitionIndex,
   const Function::ImageTransferPtr& pImageTransfer,
   bool bReset
)
{
   // Select memory type
   updateMemorySettings(pImageTransfer);

   Device::Protocol::Firehose::StorageInfoType storageInfo;
   if(MAX_PARTITION_NUM == partitionIndex)
   {
      if((QC::MemoryType::MEMORY_TYPE_UFS == memoryType))
      {
         // The magic 65210 gets mapped to whole device
         partitionIndex = MAGIC_PARTITION_UFS_EMMC;
      }
      else
      {
         partitionIndex = DEFAULT_PARTITION_NON_UFS_EMMC;
      }
   }

   pImageTransfer->firehoseGetStorageInfo(partitionIndex, storageInfo, bReset);
   if(storageInfo.isSet)
   {
      if(!storageInfo.totalBlocks.empty())
      {
         flashInfo.__set_totalBlocks(storageInfo.totalBlocks);
      }
      if(!storageInfo.pageSize.empty())
      {
         if(QC::MemoryType::MEMORY_TYPE_NAND == m_memoryType)
         {
            TOOLS_ASSERT_OR_THROW(
               Util::isNumber(storageInfo.pageSize),
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Invalid page size for NAND: " + storageInfo.pageSize
               )
            );
            m_pageSize = static_cast<uint32_t>(std::stoul(std::string(storageInfo.pageSize)));
            TOOLS_ASSERT_OR_THROW(
               m_pageSize != 0,
               Device::Exception(
                  Device::Exception::DEVICE_UNKNOWN_ERROR,
                  "Converation error for page size: " + storageInfo.pageSize
               )
            );
            pImageTransfer->firehoseSetPageSize(m_pageSize);
         }
         flashInfo.__set_pageSize(storageInfo.pageSize);
      }
      if(!storageInfo.blockSize.empty())
      {
         if(QC::MemoryType::MEMORY_TYPE_NAND == m_memoryType)
         {
            TOOLS_ASSERT_OR_THROW(
               m_pageSize != 0,
               Device::Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Page size is Zero")
            );
            TOOLS_ASSERT_OR_THROW(
               Util::isNumber(storageInfo.blockSize),
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Invalid block size for NAND: " + storageInfo.blockSize
               )
            );
            m_pagePerBlock = static_cast<uint32_t>(std::stoul(storageInfo.blockSize)) / m_pageSize;
            TOOLS_ASSERT_OR_THROW(
               m_pagePerBlock != 0,
               Device::Exception(
                  Device::Exception::DEVICE_UNKNOWN_ERROR,
                  "Converation error for block size: " + storageInfo.blockSize
               )
            );
            pImageTransfer->firehoseSetPagePerBlock(m_pagePerBlock);
         }
         flashInfo.__set_blockSize(storageInfo.blockSize);
      }
      if(!storageInfo.numPhysicalPartitions.empty())
      {
         flashInfo.__set_numPhysicalPartitions(storageInfo.numPhysicalPartitions);
      }
      if(!storageInfo.manufacturerId.empty())
      {
         flashInfo.__set_manufacturerId(storageInfo.manufacturerId);
      }
      if(!storageInfo.serialNum.empty())
      {
         flashInfo.__set_serialNum(storageInfo.serialNum);
      }
      if(!storageInfo.fwVersion.empty())
      {
         flashInfo.__set_fwVersion(storageInfo.fwVersion);
      }
      if(!storageInfo.memType.empty())
      {
         flashInfo.__set_memType(storageInfo.memType);
      }
      if(!storageInfo.prodName.empty())
      {
         flashInfo.__set_prodName(storageInfo.prodName);
      }
      if(!storageInfo.specVersion.empty())
      {
         flashInfo.__set_specVersion(storageInfo.specVersion);
      }
   }
   else
   {
      FLOG_INFO(QC::getName() + "Flash Info not received from Device");
   }
}

// ----------------------------------------------------------------------------
// getFlashInfo
//
/// Get Flash or Storage information
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::
   getFlashInfo(std::vector<QC::FlashInfo>& flashInfo, const QC::DownloadBuildOptions& options)
{
   DEVICE_RPC_TRY(PARAMETER(options))
   {
      if(isServiceLocked())
      {
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_SERVICE_LOCKED, "Image management service locked"));
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);
      TOOLS_ASSERT_OR_THROW(
         isEdlModeAvailable(),
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Download mode not available")
      );
      m_memoryType = options.memoryType;
      TOOLS_ASSERT_OR_THROW(
         (QC::MemoryType::MEMORY_TYPE_UNKNOWN > m_memoryType),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Unsupported memory type : " + std::to_string(m_memoryType)
         )
      );

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         // Select slot
         if(options.__isset.slot)
         {
            pImageTransfer->firehoseSetSlot(options.slot);
         }
         // ZLP aware host
         if(options.__isset.zlpAwareHost)
         {
            pImageTransfer->firehoseSetZlpAwareHost(options.zlpAwareHost ? Function::ImageTransfer::ZLP_AWARE_HOST_ENABLE
                                                             : Function::ImageTransfer::ZLP_AWARE_HOST_DISABLE);
         }
         doFirehoseSwitch("", options, pImageTransfer);
         bool bReset = false;
         if(options.__isset.resetAfterDownload && options.resetAfterDownload)
         {
            bReset = true;
         }
         // Get the entire flash info first
         QC::FlashInfo entireFlashInfo;
         doFirehoseGetFlashInfo(entireFlashInfo, m_memoryType, MAX_PARTITION_NUM, pImageTransfer, bReset);
         int32_t maxPartition = atoi(entireFlashInfo.numPhysicalPartitions.c_str());
         flashInfo.push_back(entireFlashInfo);
         std::vector<int32_t>::const_iterator it = options.partitionIndexList.begin();
         std::vector<int32_t>::const_iterator end = options.partitionIndexList.end();
         for(; it != end; ++it)
         {
            QC::FlashInfo singlePartitionInfo;
            // Get the single partition info
            TOOLS_ASSERT_OR_THROW(
               (*it >= MAX_PARTITION_NUM),
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Invalid Partition Number:" + std::to_string(*it)
               )
            );
            if(*it < maxPartition)
            {
               doFirehoseGetFlashInfo(singlePartitionInfo, m_memoryType, *it, pImageTransfer);
               flashInfo.push_back(singlePartitionInfo);
            }
            else
            {
               FLOG_INFO(
                  QC::getName() +
                  std::string(
                     "Partition Number out of "
                     "bound and Ignored:" +
                     std::to_string(*it)
                  )
               );
            }
         }
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);

      if(options.__isset.resetAfterDownload && options.resetAfterDownload)
      {
         TOOLS_IGNORE_EXCEPTIONS(doSaharaCleanup());
         m_saharaReadyEvent.reset();
         m_pImageManagementConnectionWorker->m_protocolAddedEvent.signal();
      }
   }
   DEVICE_RPC_CATCH;
}


// ----------------------------------------------------------------------------
// erasePartition
//
/// Erase a specific physical partition or all at once
// ----------------------------------------------------------------------------

QC::ErrorCode::type ImageManagementServiceHandler::erasePartition(const QC::DownloadBuildOptions& options)
{
   DEVICE_RPC_TRY(PARAMETER(options))
   {
      if(isServiceLocked())
      {
         return QC::ErrorCode::DEVICE_SERVICE_LOCKED;
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);
      TOOLS_ASSERT_OR_THROW(
         isEdlModeAvailable(),
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Download mode not available")
      );
      m_memoryType = options.memoryType;
      TOOLS_ASSERT_OR_THROW(
         (QC::MemoryType::MEMORY_TYPE_UNKNOWN > m_memoryType),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Unsupported memory type : " + std::string(std::to_string(m_memoryType))
         )
      );

      TOOLS_ASSERT_OR_THROW(
         (options.__isset.partitionIndexList && !options.partitionIndexList.empty()),
         Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Erase Partition Number is not set")
      );

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         updateMemorySettings(pImageTransfer);
         // Select slot
         if(options.__isset.slot)
         {
            pImageTransfer->firehoseSetSlot(options.slot);
         }
         // ZLP aware host
         if(options.__isset.zlpAwareHost)
         {
            pImageTransfer->firehoseSetZlpAwareHost(options.zlpAwareHost ? Function::ImageTransfer::ZLP_AWARE_HOST_ENABLE
                                                             : Function::ImageTransfer::ZLP_AWARE_HOST_DISABLE);
         }
         doFirehoseSwitch("", options, pImageTransfer);


         // Get the entire flash info first
         QC::FlashInfo entireFlashInfo;
         doFirehoseGetFlashInfo(entireFlashInfo, m_memoryType, MAX_PARTITION_NUM, pImageTransfer);
         int32_t maxPartition = atoi(entireFlashInfo.numPhysicalPartitions.c_str());
         pImageTransfer->firehoseSetPartitionNumber(maxPartition);
         std::vector<int32_t> partitionIndexList;
         std::vector<int32_t>::const_iterator it = options.partitionIndexList.begin();
         std::vector<int32_t>::const_iterator end = options.partitionIndexList.end();
         for(; it != end; ++it)
         {
            TOOLS_ASSERT_OR_THROW(
               (*it >= 0),
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Invalid Partition Number:" + std::to_string(*it)
               )
            );
            partitionIndexList.push_back(*it);
         }
         pImageTransfer->firehoseSetPartitionIndexList(partitionIndexList);
         pImageTransfer->firehoseErase();
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}


bool isPatternFoundInDescription(const std::string& description, const std::string& pattern)
{
   std::smatch results;
   return std::regex_search(description, results, std::regex(pattern.c_str()));
}

// ----------------------------------------------------------------------------
// isMhiConnectedDevice
//
/// @returns True if the device is connected over MHI
// ----------------------------------------------------------------------------
bool ImageManagementServiceHandler::isMhiConnectedDevice(const Device::ImplPtr& pDevice)
{
   if(isPatternFoundInDescription(pDevice->getDescription(), STRING_PATTERN_MHI))
   {
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------
// updateMemorySettings
//
/// update memory related infomation
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::updateMemorySettings(const Function::ImageTransferPtr& pImageTransfer)
{
   pImageTransfer->firehoseSetMemoryType(static_cast<Function::ImageTransfer::MemoryType>(m_memoryType));
   pImageTransfer->firehoseSetPageSize(m_pageSize);
   pImageTransfer->firehoseSetPagePerBlock(m_pagePerBlock);
}
// ----------------------------------------------------------------------------
// doFirehosePreserve
//
/// Preserve the partition before download.
/// Will find the partition info  from both phone and build. Check the length
/// of the partition, preservaton only works for partition no length changed
/// Backup-Restore mode: backup(read) to PC and restore(write) after download.
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doFirehosePreserve(
   const std::filesystem::path& buildPath,
   const std::vector<QC::PreservedPartitionInfo>& preservedPartitions,
   const Function::ImageTransferPtr& pImageTransfer
)
{
   m_preservedPartitions.clear();
   if(QC::PreservationMode::type::PRESERVATION_NONE == m_preservationMode)
   {
      return;
   }
   // Get partition information from phone GPT
   std::vector<Function::ImageTransfer::PartitionEntry> partitionEntries;
   pImageTransfer->firehoseGetPartitionEntries(partitionEntries);
   // Get partition information from build GPT
   std::vector<Function::ImageTransfer::PartitionEntry> xmlPartitionEntries;
   pImageTransfer->getPartitionEntriesFromBuild(buildPath, xmlPartitionEntries);

   std::vector<Function::ImageTransfer::DataChunkOptions> preservedEntries;
   std::vector<QC::PreservedPartitionInfo>::const_iterator it = preservedPartitions.begin();
   std::vector<QC::PreservedPartitionInfo>::const_iterator end = preservedPartitions.end();
   for(; it != end; ++it)
   {
      // Check if requested partition existing in phone or not
      QC::PreservedPartitionInfo partition = *it;
      std::vector<Function::ImageTransfer::PartitionEntry>::const_iterator preservedEnd = partitionEntries.end();
      std::vector<Function::ImageTransfer::PartitionEntry>::const_iterator preservedIt = std::find_if(
         partitionEntries.begin(),
         partitionEntries.end(),
         [&partition](const Function::ImageTransfer::PartitionEntry& thisPartition) {
            if("X" != partition.partitionIndex &&
               thisPartition.partitionIndex != static_cast<uint32_t>(std::stoul(partition.partitionIndex)))
            {
               return false;
            }
            std::string findName = std::string(partition.name);
            if(thisPartition.partitionEntryName != findName)
            {
               return false;
            }
            return true;
         }
      );

      TOOLS_ASSERT_OR_THROW(
         preservedIt != preservedEnd,
         Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Can not find partition:" + partition.name)
      );

      Function::ImageTransfer::DataChunkOptions preservedDataChunk;
      preservedDataChunk.partitionIndex = preservedIt->partitionIndex;
      preservedDataChunk.startSector = std::to_string(preservedIt->partitionEntryParameters.startingLba);
      preservedDataChunk.sectorCount = std::to_string(
         preservedIt->partitionEntryParameters.endingLba - preservedIt->partitionEntryParameters.startingLba + 1
      );
      preservedDataChunk.imageFile = std::filesystem::path(preservedIt->partitionEntryName.c_str());

      // Check if requested partition existing in build or not. If yes, check
      // the size in sectors
      preservedEnd = xmlPartitionEntries.end();
      preservedIt = std::find_if(
         xmlPartitionEntries.begin(),
         xmlPartitionEntries.end(),
         [&preservedDataChunk](const Function::ImageTransfer::PartitionEntry& thisPartition) {
            if(thisPartition.partitionEntryName != preservedDataChunk.imageFile.string().c_str())
            {
               return false;
            }
            uint64_t sectorCount =
               thisPartition.partitionEntryParameters.endingLba - thisPartition.partitionEntryParameters.startingLba +
               1;
            if(sectorCount != std::stoull(preservedDataChunk.sectorCount))
            {
               return false;
            }
            return true;
         }
      );

      TOOLS_ASSERT_OR_THROW(
         preservedIt != preservedEnd,
         Device::
            Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Can not find partition in build:" + partition.name)
      );

      // Format the filename with GUID
      std::filesystem::path restorePath =
         (std::to_string(preservedDataChunk.partitionIndex) + "_" + preservedDataChunk.imageFile.string().c_str() +
          "_" + Util::generateUuid() + ".bin")
            .c_str();
      preservedDataChunk.imageFile = restorePath;
      preservedEntries.push_back(preservedDataChunk);

      QC::DataChunkOptions
         dataChunk(preservedIt->partitionIndex, std::to_string(preservedIt->partitionEntryParameters.startingLba));
      dataChunk.__set_sectorCount(std::to_string(
         preservedIt->partitionEntryParameters.endingLba - preservedIt->partitionEntryParameters.startingLba + 1
      ));
      dataChunk.__set_imagePath(preservedDataChunk.imageFile.string().c_str());

      // Push restore information into the vector
      m_preservedPartitions.push_back(dataChunk);
   }
   // Backup partition to local
   pImageTransfer->firehoseSavePartitionSectors(preservedEntries);
}

// ----------------------------------------------------------------------------
// doFirehoseRestore
//
/// Restore the partition after download
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doFirehoseRestore(const Function::ImageTransferPtr& pImageTransfer)
{
   std::vector<Function::ImageTransfer::DataChunkOptions> restoreEntries;
   std::vector<QC::DataChunkOptions>::const_iterator it = m_preservedPartitions.begin();
   std::vector<QC::DataChunkOptions>::const_iterator end = m_preservedPartitions.end();

   std::filesystem::path outputPath = m_availableOutputPath.empty()
      ? Device::Manager::getInstance()->getTempDirectory()
      : m_availableOutputPath;

   for(; it != end; ++it)
   {
      restoreEntries.push_back(
         {it->partitionIndex, it->startSector, it->sectorCount, outputPath / std::string(it->imagePath)}
      );
   }
   if(restoreEntries.size() > 0)
   {
      pImageTransfer->firehoseFlashPartitionSectors(restoreEntries);
   }
}

// ----------------------------------------------------------------------------
// downloadBuild
//
/// Download a meta build or flat build
// ----------------------------------------------------------------------------
QC::ErrorCode::type
ImageManagementServiceHandler::downloadBuild(const std::string& buildPath, const QC::DownloadBuildOptions& options)
{
   DEVICE_RPC_TRY(PARAMETER(std::string(buildPath)) PARAMETER(options))
   {
      if(isServiceLocked())
      {
         return QC::ErrorCode::DEVICE_SERVICE_LOCKED;
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);
      TOOLS_ASSERT_OR_THROW(
         isEdlModeAvailable(),
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Download mode not available")
      );
      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         m_memoryType = options.memoryType;
         TOOLS_ASSERT_OR_THROW(
            (QC::MemoryType::MEMORY_TYPE_UNKNOWN > m_memoryType),
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PARAMETERS,
               "Unsupported memory type : " + std::to_string(m_memoryType)
            )
         );
         // Select memory type
         updateMemorySettings(pImageTransfer);
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
         // RX timeout in millisecond
         if(options.__isset.downloadRxTimeoutInMs && options.downloadRxTimeoutInMs > 0)
         {
            pImageTransfer->firehoseSetDownloadRxTimeoutInMs(
               std::chrono::milliseconds(static_cast<uint32_t>(options.downloadRxTimeoutInMs))
            );
         }

         // ZLP aware host
         if(options.__isset.zlpAwareHost)
         {
            pImageTransfer->firehoseSetZlpAwareHost(options.zlpAwareHost ? Function::ImageTransfer::ZLP_AWARE_HOST_ENABLE
                                                             : Function::ImageTransfer::ZLP_AWARE_HOST_DISABLE);
         }

         // Validate image size before download
         if(options.__isset.validateImageSize)
         {
            pImageTransfer->firehoseSetValidateImageSize(options.validateImageSize);
         }

         bool bGetFlashInfo = false;
         bool bIsVipEnabled = false;
         // If user has called setDdrStorePath API to specify path beforehand,
         // use it, else use default path.
         if(!m_ddrStorePath.empty())
         {
            pImageTransfer->setDdrStorePath(m_ddrStorePath);
         }

         std::filesystem::path availableBuildPath =
            Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(buildPath));
         std::filesystem::path buildPathDir(availableBuildPath);

         if(options.__isset.readImagesPath && !options.readImagesPath.empty())
         {
            std::filesystem::path emptyPath;
            m_availableOutputPath =
               Device::Manager::getInstance()
                  ->getAccessiblePath(std::filesystem::path(options.readImagesPath), emptyPath, true);
            pImageTransfer->firehoseSetOutpuDir(m_availableOutputPath);
         }
         else
         {
            m_availableOutputPath.clear();
         }

         // vip download
         if(options.__isset.signedDigestsPath && !options.signedDigestsPath.empty())
         {
            std::filesystem::path digestsPath =
               Device::Manager::getInstance()->getAccessiblePath(options.signedDigestsPath.c_str());
            pImageTransfer->firehoseSetSignedDigestsFile(digestsPath);
            bIsVipEnabled = true;
         }
         if(options.__isset.chainedDigestsPath && !options.chainedDigestsPath.empty())
         {
            std::filesystem::path digestsPath =
               Device::Manager::getInstance()->getAccessiblePath(options.chainedDigestsPath.c_str());
            pImageTransfer->firehoseSetChainedDigestsFile(digestsPath);
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
         else if(options.__isset.edmaPath && !options.edmaPath.empty())
         {
            // EDMA process
            //  Use edma path for XMLs and images if given.
            std::filesystem::path availableEdmaPath;
            availableEdmaPath =
               Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(options.edmaPath));
            pImageTransfer->firehoseSetEdmaPath(availableEdmaPath);
         }
         else
         {
            if(options.__isset.readImages && true == options.readImages)
            {
               // Read Binary: Read images from device if the flag is set to
               // true, otherwise start firehose download
               pImageTransfer->firehoseSetReadImages(true);
            }
            else
            {
               // download
               // validation mode
               if(options.__isset.validationMode)
               {
                  m_validationMode = options.validationMode;
                  pImageTransfer
                     ->firehoseSetValidationMode(static_cast<Function::ImageTransfer::ValidationMode>(m_validationMode)
                     );
               }
               if(options.__isset.validationDigestsPath && !options.validationDigestsPath.empty())
               {
                  std::filesystem::path digestsPath =
                     Device::Manager::getInstance()->getAccessiblePath(options.validationDigestsPath.c_str());
                  pImageTransfer->firehoseSetBuildValidationDigests(digestsPath);
               }

               // Preserve partition setting
               m_preservationMode = QC::PreservationMode::type::PRESERVATION_NONE;
               if(options.__isset.preservationOption && options.preservationOption.__isset.preservationMode &&
                  options.preservationOption.preservationMode != m_preservationMode &&
                  options.preservationOption.__isset.preservedPartitions &&
                  options.preservationOption.preservedPartitions.size() > 0)
               {
                  TOOLS_ASSERT_OR_THROW(
                     (!bIsVipEnabled),
                     Device::Exception(
                        Device::Exception::DEVICE_INVALID_PARAMETERS,
                        "Preserve partition is not supported for VIP process"
                     )
                  );

                  TOOLS_ASSERT_OR_THROW(
                     (!options.__isset.singleImagePath || options.singleImagePath.empty()),
                     Device::Exception(
                        Device::Exception::DEVICE_INVALID_PARAMETERS,
                        "Preserve partition is not supported for single "
                        "image"
                     )
                  );
                  m_preservationMode = options.preservationOption.preservationMode;
               }

               // Prepare erase string to be sent to device
               // Erase flash, only availible in download
               if(options.__isset.erase && options.erase)
               {
                  pImageTransfer->firehoseSetEnableEraseBeforeDownload(true);
               }
               if(!bIsVipEnabled)
               {
                  bGetFlashInfo = true;
               }
            }

            if(options.__isset.singleImagePath && !options.singleImagePath.empty())
            {
               std::filesystem::path imagePath(options.singleImagePath.c_str());
               if("" == imagePath.parent_path())
               {
                  imagePath = availableBuildPath / imagePath;
               }
               imagePath = Device::Manager::getInstance()->getAccessiblePath(imagePath);
               pImageTransfer->firehoseSetSingleImagePath(imagePath);

               TOOLS_ASSERT_OR_THROW(
                  (options.__isset.lun && Function::ImageTransfer::INVALID_LUN < options.lun),
                  Device::Exception(
                     Device::Exception::DEVICE_INVALID_PARAMETERS,
                     "Provide valid LUN for the image: " + std::string(imagePath.string().c_str())
                  )
               );

               TOOLS_ASSERT_OR_THROW(
                  (options.__isset.startSector && Function::ImageTransfer::INVALID_START_SECTOR < options.startSector),
                  Device::Exception(
                     Device::Exception::DEVICE_INVALID_PARAMETERS,
                     "Provide valid start sector for the image: " + std::string(imagePath.string().c_str())
                  )
               );

               pImageTransfer->firehoseSetLun(options.lun);
               pImageTransfer->firehoseSetStartSector(static_cast<int32_t>(options.startSector));
            }
            else if(options.__isset.jsonPath && !options.jsonPath.empty())
            {
               // Json Download
               //  Use Json path for XMLs and images if given. Meta Build
               //  Download
               std::filesystem::path availableJsonPath;
               availableJsonPath =
                  Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(std::string(options.jsonPath))
                  );
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
                  std::vector<std::string>::const_iterator it = options.rawXmlList.begin();
                  std::vector<std::string>::const_iterator end = options.rawXmlList.end();
                  for(; it != end; ++it)
                  {
                     std::string path = *it;
                     rawXmlList.push_back(std::filesystem::path(path.c_str()));
                  }
                  pImageTransfer->firehoseSetRawXmlList(rawXmlList);
               }
               if(options.__isset.patchXmlList && options.patchXmlList.size() > 0)
               {
                  // Check if the file exists
                  std::vector<std::string>::const_iterator it = options.patchXmlList.begin();
                  std::vector<std::string>::const_iterator end = options.patchXmlList.end();
                  for(; it != end; ++it)
                  {
                     std::string path = *it;
                     patchXmlList.push_back(std::filesystem::path(path.c_str()));
                  }
                  pImageTransfer->firehoseSetPatchXmlList(patchXmlList);
               }
            }
            if(options.__isset.partitionIndexList && options.partitionIndexList.size() > 0)
            {
               pImageTransfer->firehoseSetPartitionIndexList(options.partitionIndexList);
            }
         }

         // Check if we need to reset after download
         bool bReset = false;
         if(options.__isset.resetAfterDownload && options.resetAfterDownload)
         {
            bReset = true;
         }
         pImageTransfer->firehoseFormatDownloadBuildParameter(buildPathDir, bGetFlashInfo);
         // Download Firehose programmer
         doFirehoseSwitch(buildPathDir, options, pImageTransfer);
         if(bGetFlashInfo)
         {
            QC::FlashInfo entireFlashInfo;
            doFirehoseGetFlashInfo(entireFlashInfo, m_memoryType, MAX_PARTITION_NUM, pImageTransfer);
            m_numPartition = atoi(entireFlashInfo.numPhysicalPartitions.c_str());
            pImageTransfer->firehoseSetPartitionNumber(m_numPartition);
         }
         else
         {
            m_numPartition = 0;
         }

         doFirehosePreserve(buildPathDir, options.preservationOption.preservedPartitions, pImageTransfer);
         if(QC::PreservationMode::type::PRESERVATION_BACKUP_RESTORE == m_preservationMode)
         {
            pImageTransfer->firehoseSetEnableResetAfterDownload(false);
         }
         else
         {
            pImageTransfer->firehoseSetEnableResetAfterDownload(bReset);
         }
         pImageTransfer->firehoseDoDownload(buildPathDir);
         doFirehoseRestore(pImageTransfer);
         if(QC::PreservationMode::type::PRESERVATION_BACKUP_RESTORE == m_preservationMode && bReset)
         {
            pImageTransfer->firehoseReset();
         }
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
      if(options.__isset.resetAfterDownload && options.resetAfterDownload)
      {
         TOOLS_IGNORE_EXCEPTIONS(doSaharaCleanup());
         m_saharaReadyEvent.reset();
         m_pImageManagementConnectionWorker->m_protocolAddedEvent.signal();
      }
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// setDdrStorePath
//
/// Set path to store DDR training data. This is only path without including
/// filename. Filename is always "mdmddr.mbn" for DDR store.
// ----------------------------------------------------------------------------
QC::ErrorCode::type ImageManagementServiceHandler::setDdrStorePath(const std::string& path)
{
   DEVICE_RPC_TRY(PARAMETER(std::string(path)))
   {
      if(isServiceLocked())
      {
         return QC::ErrorCode::DEVICE_SERVICE_LOCKED;
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      m_ddrStorePath = path.c_str();
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// resetDevice
//
/// Resets device and wait until device is re-enumerated or timeout
// ----------------------------------------------------------------------------
QC::ErrorCode::type ImageManagementServiceHandler::resetDevice(const int32_t timeout, bool skipSahara)
{
   DEVICE_RPC_TRY(PARAMETER(timeout))
   {
      if(isServiceLocked())
      {
         return QC::ErrorCode::DEVICE_SERVICE_LOCKED;
      }
      {
         std::lock_guard<std::recursive_mutex> lock(m_mutex);
         Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);
         Device::Manager::getInstance()
            ->subscribeForAsyncEvents(this, &ImageManagementServiceHandler::onDeviceConnected);

         if(!m_bEntertedFirehoseMode && !skipSahara)
         {
            doSaharaConnect();
            // Sahara only reset under DEVICE_IMAGE_MODE_SAHARA_CRASH mode
            TOOLS_ASSERT_OR_THROW(
               (m_deviceImageMode == QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_CRASH),
               Device::Exception(
                  Device::Exception::DEVICE_PROTOCOL_INVALID,
                  "Unsupported device image mode : " + std::to_string(m_deviceImageMode)
               )
            );
            // Send Sahara reset command directly
            Device::Protocol::SaharaPtr pSahara =
               (m_pSaharaConnection->getProtocol()).dynamicCast<Device::Protocol::Sahara>();
            Device::SharedByteBufferPtr pResetBuffer =
               pSahara->createCommand<Device::Protocol::Sahara::Reset>(Device::Protocol::Sahara::SAHARA_RESET);
            m_pSaharaConnection->sendSync(pResetBuffer);
            Util::ThisThread::sleep(std::chrono::seconds(1));
         }
         else
         {
            if(skipSahara)
            {
               doFirehoseAdd();
               doSaharaDisconnect();
               doFirehoseConnect();
            }

            Function::ImageTransferPtr pImageTransfer =
               std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
            TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
            try
            {
               pImageTransfer->firehoseReset();
            }
            IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
            pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
         }
         TOOLS_IGNORE_EXCEPTIONS(doSaharaCleanup());
         m_saharaReadyEvent.reset();
         m_pImageManagementConnectionWorker->m_protocolAddedEvent.signal();
      }

      if(timeout > 0)
      {
         Util::ThisThread::
            waitForEvent(&m_deviceReconnectedEvent, std::chrono::milliseconds(static_cast<uint32_t>(timeout)));
      }
      // Device sonmetimes may not re-enumerate after reset sequence, spoof a
      // protocol added signal to trigger sahara mode scan
      m_pImageManagementConnectionWorker->m_protocolAddedEvent.signal();
   }
   DEVICE_RPC_CATCH;
   TOOLS_IGNORE_EXCEPTIONS(Device::Manager::getInstance()
                              ->unsubscribeAsyncEvents(this, &ImageManagementServiceHandler::onDeviceConnected));
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// createGuidString
//
/// @returns GUID string based on partition table contents
// ----------------------------------------------------------------------------
std::string
ImageManagementServiceHandler::createGuidString(uint32_t data1, uint16_t data2, uint16_t data3, uint64_t data4)
{
   std::ostringstream guid;

   guid << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << data1;
   guid << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << data2;
   guid << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << data3;

   std::ostringstream revGuid4;
   revGuid4 << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << data4;

   // The last 8 bytes for some reason seem to be big endian. Changes endianness
   guid << revGuid4.str().substr(14, 2);
   guid << revGuid4.str().substr(12, 2);
   guid << revGuid4.str().substr(10, 2);
   guid << revGuid4.str().substr(8, 2);
   guid << revGuid4.str().substr(6, 2);
   guid << revGuid4.str().substr(4, 2);
   guid << revGuid4.str().substr(2, 2);
   guid << revGuid4.str().substr(0, 2);

   return guid.str();
}

// ----------------------------------------------------------------------------
// initPartitionTable
//
/// @returns A list of partition entries from a device
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::
   initPartitionTable(std::vector<QC::PartitionInfo>& _return, const QC::DownloadBuildOptions& options)
{
   DEVICE_RPC_TRY(PARAMETER(options))
   {
      if(isServiceLocked())
      {
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_SERVICE_LOCKED, "Image management service locked"));
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);
      TOOLS_ASSERT_OR_THROW(
         isEdlModeAvailable(),
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Download mode not available")
      );
      m_memoryType = options.memoryType;
      TOOLS_ASSERT_OR_THROW(
         (QC::MemoryType::MEMORY_TYPE_UNKNOWN > m_memoryType),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Unsupported memory type : " + std::to_string(m_memoryType)
         )
      );

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         // Select slot
         if(options.__isset.slot)
         {
            pImageTransfer->firehoseSetSlot(options.slot);
            m_slot = options.slot;
         }
         else
         {
            // When slot is not set, set m_slot defaultly to 0
            pImageTransfer->firehoseSetSlot(0);
            m_slot = 0;
         }

         if(options.__isset.partitionIndexList && options.partitionIndexList.size() > 0)
         {
            pImageTransfer->firehoseSetPartitionIndexList(options.partitionIndexList);
         }

         // validation mode
         if(options.__isset.validationMode)
         {
            m_validationMode = options.validationMode;
            pImageTransfer
               ->firehoseSetValidationMode(static_cast<Function::ImageTransfer::ValidationMode>(m_validationMode));
         }
         if(options.__isset.validationDigestsPath && !options.validationDigestsPath.empty())
         {
            std::filesystem::path digestsPath =
               Device::Manager::getInstance()->getAccessiblePath(options.validationDigestsPath.c_str());
            pImageTransfer->firehoseSetBuildValidationDigests(digestsPath);
         }

         if(options.__isset.readImagesPath && !options.readImagesPath.empty())
         {
            std::filesystem::path emptyPath;
            m_availableOutputPath =
               Device::Manager::getInstance()
                  ->getAccessiblePath(std::filesystem::path(options.readImagesPath), emptyPath, true);
            pImageTransfer->firehoseSetOutpuDir(m_availableOutputPath);
         }
         else
         {
            m_availableOutputPath.clear();
         }

         // ZLP aware host
         if(options.__isset.zlpAwareHost)
         {
            pImageTransfer->firehoseSetZlpAwareHost(options.zlpAwareHost ? Function::ImageTransfer::ZLP_AWARE_HOST_ENABLE
                                                             : Function::ImageTransfer::ZLP_AWARE_HOST_DISABLE);
         }

         doFirehoseSwitch("", options, pImageTransfer);

         // Select memory type
         updateMemorySettings(pImageTransfer);
         QC::FlashInfo entireFlashInfo;
         doFirehoseGetFlashInfo(entireFlashInfo, m_memoryType, MAX_PARTITION_NUM, pImageTransfer);
         m_numPartition = atoi(entireFlashInfo.numPhysicalPartitions.c_str());
         pImageTransfer->firehoseSetPartitionNumber(m_numPartition);

         std::vector<Function::ImageTransfer::PartitionEntry> partitionEntries;
         pImageTransfer->firehoseGetPartitionEntries(partitionEntries);
         std::vector<Function::ImageTransfer::PartitionEntry>::iterator it = partitionEntries.begin();
         std::vector<Function::ImageTransfer::PartitionEntry>::iterator end = partitionEntries.end();
         for(; it != end; ++it)
         {
            QC::PartitionInfo partitionInfo;

            partitionInfo.partitionTypeGuid = createGuidString(
               it->partitionEntryParameters.partitionTypeGuid1,
               it->partitionEntryParameters.partitionTypeGuid2,
               it->partitionEntryParameters.partitionTypeGuid3,
               it->partitionEntryParameters.partitionTypeGuid4
            );

            partitionInfo.uniquePartitionGuid = createGuidString(
               it->partitionEntryParameters.uniquePartitionGuid1,
               it->partitionEntryParameters.uniquePartitionGuid2,
               it->partitionEntryParameters.uniquePartitionGuid3,
               it->partitionEntryParameters.uniquePartitionGuid4
            );

            partitionInfo.lun = it->partitionIndex;
            partitionInfo.name.assign(it->partitionEntryName.c_str());
            partitionInfo.startingLba = it->partitionEntryParameters.startingLba;
            partitionInfo.endingLba = it->partitionEntryParameters.endingLba;
            partitionInfo.attributes = it->partitionEntryParameters.attributes;

            if(partitionInfo.endingLba != 0 && partitionInfo.endingLba >= partitionInfo.startingLba)
            {
               _return.push_back(partitionInfo);
            }
         }
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
   }
   DEVICE_RPC_CATCH;
}

// ----------------------------------------------------------------------------
// isValidPartitionDigits
//
/// Test if the string is a valid partition digits
// ----------------------------------------------------------------------------
std::string ImageManagementServiceHandler::getValidPartitionDigits(std::string partitionDigits)
{
   std::string str(partitionDigits);
   std::string::size_type pos = str.find_first_of('.');
   if(pos == str.length() - 1) //"last uint64_t is (.)
   {
      str = str.substr(0, pos);
   }
   std::string numDiskSectorsMinus = std::string("NUM_DISK_SECTORS-");
   if(str.compare(0, numDiskSectorsMinus.size(), numDiskSectorsMinus) != 0)
   {
      std::string numDiskSectors = std::string("NUM_DISK_SECTORS");
      if(str == numDiskSectors) return numDiskSectors + ".";
      if(Util::isNumber(str))
      {
         return str;
      }
   }
   else
   {
      std::string::size_type len = numDiskSectorsMinus.length();
      std::string num = str.substr(len);
      if(Util::isNumber(num))
      {
         return numDiskSectorsMinus + num + ".";
      }
   }
   return "";
}

// ----------------------------------------------------------------------------
// readPartitionData
//
/// Read data from partition sectors and save to file
// ----------------------------------------------------------------------------
QC::ErrorCode::type ImageManagementServiceHandler::readPartitionData(const std::vector<QC::DataChunkOptions>& options)
{
   DEVICE_RPC_TRY(TOOLS_VOID)
   {
      if(isServiceLocked())
      {
         return QC::ErrorCode::DEVICE_SERVICE_LOCKED;
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);

      TOOLS_ASSERT_OR_THROW(
         (QC::MemoryType::MEMORY_TYPE_UNKNOWN > m_memoryType),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Unsupported memory type : " + std::to_string(m_memoryType)
         )
      );
      std::vector<Function::ImageTransfer::DataChunkOptions> dataChunkOptions;
      std::vector<QC::DataChunkOptions>::const_iterator it = options.begin();
      std::vector<QC::DataChunkOptions>::const_iterator end = options.end();
      for(; it != end; ++it)
      {
         TOOLS_ASSERT_OR_THROW(
            it->__isset.startSector,
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Missing start sector")
         );
         std::string fmtStartSector = getValidPartitionDigits(it->startSector);
         TOOLS_ASSERT_OR_THROW(
            !fmtStartSector.empty(),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Invalid start sector:" + it->startSector)
         );
         TOOLS_ASSERT_OR_THROW(
            it->__isset.sectorCount,
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Missing sector number")
         );
         std::string fmtSectorCount = getValidPartitionDigits(it->sectorCount);
         TOOLS_ASSERT_OR_THROW(
            !fmtSectorCount.empty(),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Invalid sector number:" + it->sectorCount)
         );

         TOOLS_ASSERT_OR_THROW(
            (it->__isset.sectorCount && !it->imagePath.empty()),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Missing readback file path!")
         );
         dataChunkOptions.push_back({it->partitionIndex, fmtStartSector, fmtSectorCount, it->imagePath.c_str()});
      }
      TOOLS_ASSERT_OR_THROW(
         m_bEntertedFirehoseMode,
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Device has not entered firehose mode!")
      );

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         updateMemorySettings(pImageTransfer);
         if(!m_availableOutputPath.empty())
         {
            pImageTransfer->firehoseSetOutpuDir(m_availableOutputPath);
         }
         if(m_numPartition != 0)
         {
            pImageTransfer->firehoseSetPartitionNumber(m_numPartition);
         }
         if(m_maxPayloadSize != 0)
         {
            pImageTransfer->firehoseSetMaxPayloadSize(m_maxPayloadSize);
         }
         pImageTransfer->firehoseSavePartitionSectors(dataChunkOptions);
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// writePartitionData
//
/// Write data to partition sectors from a file
// ----------------------------------------------------------------------------
QC::ErrorCode::type ImageManagementServiceHandler::writePartitionData(const std::vector<QC::DataChunkOptions>& options)
{
   DEVICE_RPC_TRY(TOOLS_VOID)
   {
      if(isServiceLocked())
      {
         return QC::ErrorCode::DEVICE_SERVICE_LOCKED;
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);

      TOOLS_ASSERT_OR_THROW(
         (QC::MemoryType::MEMORY_TYPE_UNKNOWN > m_memoryType),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Unsupported memory type : " + std::to_string(m_memoryType)
         )
      );

      std::vector<Function::ImageTransfer::DataChunkOptions> dataChunkOptions;
      std::vector<QC::DataChunkOptions>::const_iterator it = options.begin();
      std::vector<QC::DataChunkOptions>::const_iterator end = options.end();
      for(; it != end; ++it)
      {
         TOOLS_ASSERT_OR_THROW(
            it->__isset.startSector,
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Missing start sector")
         );
         std::string fmtStartSector = getValidPartitionDigits(it->startSector);
         TOOLS_ASSERT_OR_THROW(
            !fmtStartSector.empty(),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Invalid start sector:" + it->startSector)
         );
         std::string fmtSectorCount = getValidPartitionDigits(it->sectorCount);
         TOOLS_ASSERT_OR_THROW(
            !fmtSectorCount.empty(),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Invalid sector number:" + it->sectorCount)
         );
         std::filesystem::path availableImagePath =
            Device::Manager::getInstance()->getAccessiblePath(std::filesystem::path(it->imagePath));
         TOOLS_ASSERT_OR_THROW(
            std::filesystem::exists(availableImagePath),
            Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Device has not entered firehose mode!")
         );
         dataChunkOptions.push_back({it->partitionIndex, fmtStartSector, fmtSectorCount, availableImagePath});
      }

      TOOLS_ASSERT_OR_THROW(
         m_bEntertedFirehoseMode,
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Device has not entered firehose mode!")
      );

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         updateMemorySettings(pImageTransfer);
         pImageTransfer->firehoseSetSlot(m_slot);
         if(!m_availableOutputPath.empty())
         {
            pImageTransfer->firehoseSetOutpuDir(m_availableOutputPath);
         }
         pImageTransfer->firehoseSetValidationMode(static_cast<Function::ImageTransfer::ValidationMode>(m_validationMode
         ));
         if(m_numPartition != 0)
         {
            pImageTransfer->firehoseSetPartitionNumber(m_numPartition);
         }
         if(m_maxPayloadSize != 0)
         {
            pImageTransfer->firehoseSetMaxPayloadSize(m_maxPayloadSize);
         }
         pImageTransfer->firehoseFlashPartitionSectors(dataChunkOptions);
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent););
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// erasePartitionData
//
/// Erase partition sector data
// ----------------------------------------------------------------------------
QC::ErrorCode::type ImageManagementServiceHandler::erasePartitionData(const std::vector<QC::DataChunkOptions>& options)
{
   DEVICE_RPC_TRY(TOOLS_VOID)
   {
      if(isServiceLocked())
      {
         return QC::ErrorCode::DEVICE_SERVICE_LOCKED;
      }
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      Util::ThisThread::waitForEvent(&m_connectionWorkerEvent, CONNECTION_WORKER_READY_WAIT_PERIOD);

      TOOLS_ASSERT_OR_THROW(
         (QC::MemoryType::MEMORY_TYPE_UNKNOWN > m_memoryType),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Unsupported memory type : " + std::to_string(m_memoryType)
         )
      );

      std::vector<Function::ImageTransfer::DataChunkOptions> dataChunkOptions;
      std::vector<QC::DataChunkOptions>::const_iterator it = options.begin();
      std::vector<QC::DataChunkOptions>::const_iterator end = options.end();
      for(; it != end; ++it)
      {
         TOOLS_ASSERT_OR_THROW(
            it->__isset.startSector,
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Missing start sector")
         );
         std::string fmtStartSector = getValidPartitionDigits(it->startSector);
         TOOLS_ASSERT_OR_THROW(
            !fmtStartSector.empty(),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Invalid start sector:" + it->startSector)
         );
         TOOLS_ASSERT_OR_THROW(
            it->__isset.sectorCount,
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Missing sector number")
         );
         std::string fmtSectorCount = getValidPartitionDigits(it->sectorCount);
         TOOLS_ASSERT_OR_THROW(
            !fmtSectorCount.empty(),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Invalid sector number:" + it->sectorCount)
         );

         dataChunkOptions.push_back({it->partitionIndex, fmtStartSector, fmtSectorCount, ""});
      }

      TOOLS_ASSERT_OR_THROW(
         m_bEntertedFirehoseMode,
         Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Device has not entered firehose mode!")
      );

      Function::ImageTransferPtr pImageTransfer =
         std::make_shared<Function::ImageTransfer>(m_pSaharaConnection, m_pFirehoseConnection);
      TOOLS_ASSUMING(pImageTransfer->subscribe(this, &ImageManagementServiceHandler::onImageTransferEvent));
      try
      {
         updateMemorySettings(pImageTransfer);
         pImageTransfer->firehoseSetSlot(m_slot);
         if(m_numPartition != 0)
         {
            pImageTransfer->firehoseSetPartitionNumber(m_numPartition);
         }
         pImageTransfer->firehoseErasePartitionSectors(dataChunkOptions);
      }
      IMGMGMT_CATCH(pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);

      );
      pImageTransfer->unsubscribe(this, &ImageManagementServiceHandler::onImageTransferEvent);
   }
   DEVICE_RPC_CATCH;
   return static_cast<QC::ErrorCode::type>(__functionError);
}

// ----------------------------------------------------------------------------
// onProtocolAddedChange
//
/// Notify ImageManagementConnectionWorker that a protocol is added
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::onProtocolAddedChange(Device::ProtocolAddedEvent* pEvent)
{
   FLOG_INFO("onProtocolAddedChange handle: " + std::string(std::to_string(pEvent->getProtocol()->getHandle())));

   // Don't care the prtotcol type, any interface change, will request a
   // re-enumerate of protocol
   if(pEvent->getDevice() == m_pDevice)
   {
      m_pImageManagementConnectionWorker->m_protocolAddedEvent.signal();
   }
}

// ----------------------------------------------------------------------------
// onProtocolRemovedChange
//
/// Notify ImageManagementConnectionWorker that a protocol is removed
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::onProtocolRemovedChange(Device::ProtocolRemovedEvent* pEvent)
{
   if(pEvent->getDevice() == m_pDevice)
   {
      // On removal also re-enumerate
      if((pEvent->getProtocol()->getOverrideProtocol()).dynamicCast<Device::Protocol::Sahara>() != nullptr)
      {
         TOOLS_IGNORE_EXCEPTIONS(doSaharaCleanup());
         m_saharaReadyEvent.reset();
      }
      if((pEvent->getProtocol()->getOverrideProtocol()).dynamicCast<Device::Protocol::Firehose>() != nullptr)
      {
         TOOLS_IGNORE_EXCEPTIONS(doSaharaCleanup());
         m_saharaReadyEvent.reset();
      }
      m_pImageManagementConnectionWorker->m_protocolAddedEvent.signal();
      m_serialNumber = 0xFFFFFFFFU;
      m_msmHwId = 0xFFFFFFFFU;
   }
}

// ----------------------------------------------------------------------------
// onImageTransferEvent
//
/// Pushes the event to the RPC client
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::onImageTransferEvent(Function::ImageTransferEvent* pEvent)
{
   sendEvent(pEvent->getEventId(), pEvent->getDescription());
   if(m_pDevice == nullptr || (m_bEntertedFirehoseMode && m_pFirehoseConnection == nullptr) ||
      (!m_bEntertedFirehoseMode && m_pSaharaConnection == nullptr))
   {
      return;
   }
   // send event to all clients
   Device::Manager::getInstance()->sendImageManagementServiceEvent(
      QC::getName(),
      m_pDevice->getHandle(),
      m_bEntertedFirehoseMode
         ? m_pFirehoseConnection->getProtocol()->getHandle()
         : m_pSaharaConnection->getProtocol()->getHandle(),
      pEvent->getEventId(),
      pEvent->getDescription()
   );
}

// ----------------------------------------------------------------------------
// onDeviceConnected
//
/// Calls the device connect signal to the UTS client
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::onDeviceConnected(Device::DeviceConnectEvent* pEvent)
{
   if(pEvent->getDevice() == m_pDevice)
   {
      m_deviceReconnectedEvent.signal();
   }
}

// ----------------------------------------------------------------------------
// doSaharaConnect
//
/// Creates sahara and firehose connections
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doSaharaConnect()
{
   if(m_pSaharaConnection != nullptr)
   {
      m_pDeviceManagerHandler->closeConnection(m_pSaharaConnection);
      m_pSaharaConnection = NULL;
   }

   Util::ThisThread::sleep(std::chrono::seconds(1));
   TOOLS_ASSERT_OR_THROW(
      m_pSaharaProtocol != nullptr,
      Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Sahara protocol not available")
   );
   m_pSaharaConnection = m_pDeviceManagerHandler->createConnection(
      m_pSaharaProtocol,
      Device::Protocol::Base::Access::READ_WRITE,
      Device::Protocol::Base::Share::NONE
   );
   TOOLS_ASSERT_OR_THROW(
      m_pSaharaConnection != nullptr,
      Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Sahara connection not available")
   );

   m_pSaharaConnection->connect();
}

// ----------------------------------------------------------------------------
// doSaharaDisconnect
//
/// Disconnect sahara connection
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doSaharaDisconnect()
{
   if(m_pSaharaConnection != nullptr)
   {
      m_pSaharaConnection->disconnect();
   }
}


// ----------------------------------------------------------------------------
// doEdlSwitch
//
/// Switch device to EDL mode using either ADB or Diag
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doEdlSwitch(const std::string& programmerPath)
{
   (void)programmerPath; // Suppress unused parameter warning
   if(QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_DOWNLOAD == m_deviceImageMode)
   {
      FLOG_INFO("Device already in Download Mode: " + std::string(std::to_string(m_deviceImageMode)));
      return;
   }
   else
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_DEVICE_HANDLE,
         "Invalid Device Image Mode:" + std::to_string(m_deviceImageMode)
      ));
   }
#ifdef TOOLS_TARGET_LINUX
   if(isMhiConnectedDevice(m_pDevice))
   {
      // Only support MHI0 for now
      Device::Manager::getInstance()->mhiForceEdl(0, programmerPath);

      // Wait for MHI0 to come up in download mode
      Util::ThisThread::waitForEvent(&m_saharaReadyEvent, SAHARA_EDL_SWITCH_WAIT_PERIOD);
      return;
   }
#endif
}

// ----------------------------------------------------------------------------
// doFirehoseSwitch
//
/// Switch device to firehose mode
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doFirehoseSwitch(
   const std::filesystem::path& buildPathDir,
   const QC::DownloadBuildOptions& options,
   const Function::ImageTransferPtr& pImageTransfer
)
{
   if(!m_bEntertedFirehoseMode)
   {
      // Download Firehose programmer
      std::filesystem::path firehoseProgrammer;
      if(options.__isset.skipSahara && options.skipSahara)
      {
         FLOG_INFO(QC::getName() + ": Skip Sahara, directly go to Firehose");
      }
      else if(options.__isset.firehoseProgPath && !options.firehoseProgPath.empty())
      {
         firehoseProgrammer = options.firehoseProgPath.c_str();
      }
      else if(!buildPathDir.empty() &&
              options.saharaImageList.end() ==
                 options.saharaImageList.find(Device::Protocol::Sahara::IMAGE_ID_EDL_PROGRAMMER))
      {
         std::filesystem::directory_iterator end;
         std::filesystem::directory_iterator firehoseProgrammerIt(buildPathDir);
         for(const auto& entry: std::filesystem::directory_iterator(buildPathDir))
         {
            if(entry.path().extension() == FIREHOSE_PROGRAMMER_FILTER)
            {
               firehoseProgrammer = firehoseProgrammerIt->path();
            }
         }
      }

      // If Sahara protocol is not present, switch device to EDL mode
      if(m_pSaharaProtocol == nullptr || Device::Protocol::Base::STATE_DISCONNECTED == m_pSaharaProtocol->getState())
      {
         doEdlSwitch(firehoseProgrammer.string().c_str());
      }

      doSaharaConnect();
      doFirehoseAdd();
      pImageTransfer->UpdateConnection(m_pSaharaConnection, m_pFirehoseConnection);
      // Connect Sahara and add Firehose protocol on top of it
      if((!m_pSaharaProtocol->isFirehoseModeEnabled()) && (!options.__isset.skipSahara || !options.skipSahara) &&
         !isMhiConnectedDevice(m_pDevice))
      {
         if(!options.__isset.memoryType)
         {
            Function::ImageTransfer::MemoryType memoryType = pImageTransfer->getDeviceMemoryType();
            TOOLS_ASSERT_OR_THROW(
               Function::ImageTransfer::MemoryType::MEMORY_TYPE_UNKNOWN != memoryType,
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Failed to retrive memory type from the device"
               )
            );

            m_memoryType = static_cast<QC::MemoryType::type>(memoryType);
            updateMemorySettings(pImageTransfer);
         }

         Function::ImageTransfer::BuildImageList buildImageList;
         std::map<int32_t, std::string>::const_iterator it = options.saharaImageList.begin();
         std::map<int32_t, std::string>::const_iterator end = options.saharaImageList.end();
         for(; it != end; ++it)
         {
            buildImageList
               .insert(std::make_pair(static_cast<uint32_t>(it->first), std::filesystem::path(it->second.c_str())));
         }
         pImageTransfer->transferFirehoseProgrammer(firehoseProgrammer, buildImageList);
      }
      // Manually disconnect Sahara and connect Firehose
      doSaharaDisconnect();
      FLOG_INFO("Wait few seconds for firehose Ready");
      if(options.__isset.firehoseInitializeTimeInMs)
      {
         Util::ThisThread::sleep(std::chrono::milliseconds(options.firehoseInitializeTimeInMs));
      }
      else
      {
         Util::ThisThread::sleep(FIREHOSE_READY_WAIT_PERIOD);
      }
      doFirehoseConnect();
      if(options.__isset.maxPayloadSize)
      {
         m_maxPayloadSize = options.maxPayloadSize;
         pImageTransfer->firehoseSetMaxPayloadSize(m_maxPayloadSize);
      }
      else
      {
         m_maxPayloadSize = 0;
         pImageTransfer->firehoseSetMaxPayloadSize(0);
      }

      if(options.__isset.maxReadPayloadSize)
      {
         pImageTransfer->firehoseSetMaxReadPayloadSize(options.maxReadPayloadSize);
      }
      else
      {
         pImageTransfer->firehoseSetMaxReadPayloadSize(0);
      }

      if(options.__isset.excludeErasePartitionInfo)
      {
         std::string excludeErasePartitionInfo("");
         for(std::vector<QC::PartitionInfo>::const_iterator it = options.excludeErasePartitionInfo.begin();
             it != options.excludeErasePartitionInfo.end();
             ++it)
         {
            excludeErasePartitionInfo.append(std::to_string(it->lun) + ":" + std::to_string(it->startingLba));
            if(options.excludeErasePartitionInfo.size() > 1)
            {
               excludeErasePartitionInfo.append(",");
            }
         }

         pImageTransfer->firehoseSetExcludeErasePartitionInfo(excludeErasePartitionInfo);
      }

      // Shall moving more initialization here
      m_pageSize = 0;
      m_pagePerBlock = 0;

      m_bEntertedFirehoseMode = true;
   }
}

// ----------------------------------------------------------------------------
// doFirehoseAdd
//
/// Creates firehose protocol and connection on top of Sahara protocol
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doFirehoseAdd()
{
   TOOLS_ASSERT_OR_THROW(
      m_pSaharaProtocol != nullptr,
      Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Sahara protocol not available")
   );

   // Create a parallel Firehose connection to use Firehose protocol without
   // tearing down Sahara
   if(m_pFirehoseProtocol == nullptr)
   {
      // Serach both available and unavailable protocol lists for Firehose
      // protocol to reuse
      Device::Impl::ProtocolList protocols = m_pDevice->getProtocolList();
      Device::Impl::ProtocolList::const_iterator it = protocols.begin();
      Device::Impl::ProtocolList::const_iterator end = protocols.end();

      for(; it != end; ++it)
      {
         if(Device::ProtocolType::PROT_FIREHOSE ==
               Device::Manager::getInstance()->getProtocolType((*it)->getHandle()) &&
            m_pSaharaProtocol->getCommonIo() == (*it)->getCommonIo())
         {
            m_pFirehoseProtocol = ((*it)->getOverrideProtocol()).dynamicCast<Device::Protocol::Firehose>();
            m_pFirehoseProtocol->setIo(m_pSaharaProtocol->getCommonIo());
            break;
         }
      }

      if(m_pFirehoseProtocol == nullptr)
      {
         Device::Impl::ProtocolList unavailableProtocols = m_pDevice->getUnavailableProtocolList();
         Device::Impl::ProtocolList::const_iterator itUnavailable = unavailableProtocols.begin();
         Device::Impl::ProtocolList::const_iterator endUnavailable = unavailableProtocols.end();

         for(; itUnavailable != endUnavailable; ++itUnavailable)
         {
            if(Device::ProtocolType::PROT_FIREHOSE ==
                  Device::Manager::getInstance()->getProtocolType((*itUnavailable)->getHandle()) &&
               m_pSaharaProtocol->getCommonIo() == (*itUnavailable)->getCommonIo())
            {
               m_pFirehoseProtocol =
                  ((*itUnavailable)->getOverrideProtocol()).dynamicCast<Device::Protocol::Firehose>();
               m_pDevice->addProtocol(m_pFirehoseProtocol);
               m_pFirehoseProtocol->setIo(m_pSaharaProtocol->getCommonIo());
               break;
            }
         }
      }

      if(m_pFirehoseProtocol != nullptr)
      {
         FLOG_INFO(QC::getName() + ": Reuse Firehose protocol " + std::to_string(m_pFirehoseProtocol->getHandle()));
      }
      else
      {
         FLOG_INFO(QC::getName() + ": Create new Firehose protocol");
         m_pFirehoseProtocol = Util::SharedPointer<Device::Protocol::Firehose>::
            create(m_pSaharaProtocol->getCommonIo(), m_pSaharaProtocol->getDevice()->getHandle());
         TOOLS_ASSERT_OR_THROW(
            m_pFirehoseProtocol != nullptr,
            Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Firehose protocol not available")
         );
         FLOG_INFO(
            QC::getName() + ": New Firehose protocol created " + std::to_string(m_pFirehoseProtocol->getHandle())
         );
         m_pSaharaProtocol->getDevice()->addProtocol(m_pFirehoseProtocol);
      }
   }
   else
   {
      FLOG_INFO(QC::getName() + ": Firehose protocol already added " + std::to_string(m_pFirehoseProtocol->getHandle()));
      m_pFirehoseProtocol->setIo(m_pSaharaProtocol->getCommonIo());
   }

   FLOG_INFO(QC::getName() + ": Enable Firehose protocol " + std::to_string(m_pFirehoseProtocol->getHandle()));
   m_pFirehoseProtocol->setState(Device::Protocol::Base::STATE_AVAILABLE);

   if(m_pFirehoseConnection == nullptr)
   {
      FLOG_INFO(
         QC::getName() + ": Create new Firehose connection from protocol " +
         std::to_string(m_pFirehoseProtocol->getHandle())
      );
      m_pFirehoseConnection = m_pDeviceManagerHandler->createConnection(
         m_pFirehoseProtocol,
         Device::Protocol::Base::Access::READ_WRITE,
         Device::Protocol::Base::Share::NONE
      );
      FLOG_INFO(
         QC::getName() + ": New Firehose connection created from protocol " +
         std::to_string(m_pFirehoseProtocol->getHandle())
      );
   }

   TOOLS_ASSERT_OR_THROW(
      m_pFirehoseConnection != nullptr,
      Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Firehose connection not available")
   );
}

// ----------------------------------------------------------------------------
// doFirehoseConnect
//
/// Creates firehose protocol and connection on top of Sahara protocol
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doFirehoseConnect()
{
   TOOLS_ASSERT_OR_THROW(
      m_pFirehoseProtocol != nullptr,
      Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Firehose protocol not available")
   );
   TOOLS_ASSERT_OR_THROW(
      m_pFirehoseConnection != nullptr,
      Device::Exception(Device::Exception::DEVICE_PROTOCOL_INVALID, "Firehose connection not available")
   );

   m_pFirehoseConnection->connect();
   m_pSaharaProtocol->setFirehoseModeEnabled(true);
}

// ----------------------------------------------------------------------------
// doFirehoseRemove
//
/// Tear down firehose protocol and connection
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doFirehoseRemove()
{
   FLOG_INFO(QC::getName() + ": Cleanup Firehose");
   m_bEntertedFirehoseMode = false;
   if(m_pFirehoseConnection != nullptr)
   {
      TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManagerHandler->closeConnection(m_pFirehoseConnection));
      m_pFirehoseConnection = NULL;
   }
   if(m_pFirehoseProtocol != nullptr)
   {
      TOOLS_IGNORE_EXCEPTIONS(m_pSaharaProtocol->getDevice()->removeProtocol(m_pFirehoseProtocol));
      m_pFirehoseProtocol->setState(Device::Protocol::Base::STATE_DISCONNECTED);
      m_pFirehoseProtocol = NULL;
   }
   // FLOG_INFO(QC::getName() + ": Cleanup Firehose Done");
}

// ----------------------------------------------------------------------------
// doSaharaCleanup
//
/// Clean up Sahara and Firehose
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doSaharaCleanup()
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   TOOLS_IGNORE_EXCEPTIONS(doFirehoseRemove());

   FLOG_INFO(QC::getName() + ": Cleanup Sahara");
   if(m_pSaharaConnection != nullptr)
   {
      TOOLS_IGNORE_EXCEPTIONS(m_pDeviceManagerHandler->closeConnection(m_pSaharaConnection));
      m_pSaharaConnection = NULL;
   }
   m_pSaharaProtocol = NULL;

   m_deviceImageMode = QC::DeviceImageMode::DEVICE_IMAGE_MODE_NONE;
   FLOG_INFO(QC::getName() + ": Cleanup Sahara Done");
}

// ----------------------------------------------------------------------------
// doDestroy
//
/// Cleans up subscriptions and threads
// ----------------------------------------------------------------------------
void ImageManagementServiceHandler::doDestroy()
{
   TOOLS_IGNORE_EXCEPTIONS(Device::Manager::getInstance()
                              ->unsubscribeAsyncEvents(this, &ImageManagementServiceHandler::onProtocolAddedChange));
   TOOLS_IGNORE_EXCEPTIONS(Device::Manager::getInstance()
                              ->unsubscribeAsyncEvents(this, &ImageManagementServiceHandler::onProtocolRemovedChange));

   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   if(m_pImageManagementConnectionWorkerThread != nullptr)
   {
      m_pImageManagementConnectionWorkerThread->stop();
      m_pImageManagementConnectionWorker->m_protocolAddedEvent.signal();
      m_pImageManagementConnectionWorkerThread->waitForStop();
      m_pImageManagementConnectionWorkerThread = NULL;
      m_pImageManagementConnectionWorker = NULL;
   }

   TOOLS_IGNORE_EXCEPTIONS(doSaharaCleanup());

   m_bInitialized = false;
}

// ----------------------------------------------------------------------------
// doDestroy
//
/// Check if EDL mode is ready or not
// ----------------------------------------------------------------------------
bool ImageManagementServiceHandler::isEdlModeAvailable()
{
   return (QC::DeviceImageMode::DEVICE_IMAGE_MODE_NONE == m_deviceImageMode && m_pFirehoseProtocol != nullptr) ||
          (QC::DeviceImageMode::DEVICE_IMAGE_MODE_SAHARA_DOWNLOAD == m_deviceImageMode && m_pSaharaProtocol != nullptr);
}
} // namespace Service

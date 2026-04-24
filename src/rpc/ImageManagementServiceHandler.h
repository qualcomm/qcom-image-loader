// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Exception.h"
#include "device/Fwd.h"
#include "function/ImageTransfer.h"
#include "ImageManagementDefinitions.h"
#include "rpc/DeviceManagerHandler.h"
#include "rpc/Service.h"
#include "util/Event.h"
#include "util/ThreadHelper.h"

#include <mutex>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4512)
#endif

namespace Rpc {

typedef int64_t EventId;

class ImageManagementConnectionWorker;
class StdThreadWrapper;

// ----------------------------------------------------------------------------
// SaharaServiceHandler
//
/// Implements the functions for the Sahara RPC server
// ----------------------------------------------------------------------------
class ImageManagementServiceHandler
: public Rpc::ServiceHandlerBase
, public std::enable_shared_from_this<ImageManagementServiceHandler>
{
   TOOLS_FORBID_COPY(ImageManagementServiceHandler);

public:
   ImageManagementServiceHandler(std::string serviceName);
   virtual ~ImageManagementServiceHandler();

   virtual QC::ErrorCode::type initializeService();
   virtual QC::ErrorCode::type initializeService(Device::ImplPtr pDevice);
   virtual QC::ErrorCode::type destroyService();

   bool isServiceLocked();
   QC::DeviceImageMode::type getDeviceImageMode();
   void transferImages(QC::TransferImageResult& transferImageResult, const std::map<int32_t, std::string>& imageList);
   void getDeviceInfo(QC::EdlDeviceInfo& edlDeviceInfo);
   QC::ErrorCode::type downloadBuild(const std::string& buildPath, const QC::DownloadBuildOptions& options);
   QC::ErrorCode::type setDdrStorePath(const std::string& path);
   QC::ErrorCode::type resetDevice(const int32_t timeout, bool skipSahara);
   QC::ErrorCode::type erasePartition(const QC::DownloadBuildOptions& options);

   void initPartitionTable(std::vector<QC::PartitionInfo>& _return, const QC::DownloadBuildOptions& options);
   QC::ErrorCode::type readPartitionData(const std::vector<QC::DataChunkOptions>& options);
   QC::ErrorCode::type writePartitionData(const std::vector<QC::DataChunkOptions>& options);
   QC::ErrorCode::type erasePartitionData(const std::vector<QC::DataChunkOptions>& options);
   void getFlashInfo(std::vector<QC::FlashInfo>& flashInfo, const QC::DownloadBuildOptions& options);

private:
   friend class ImageManagementConnectionWorker;

   bool isEdlModeAvailable();
   std::string getValidPartitionDigits(std::string partitionDigits);

   void onProtocolAddedChange(Device::ProtocolAddedEvent* pEvent);
   void onProtocolRemovedChange(Device::ProtocolRemovedEvent* pEvent);
   void onImageTransferEvent(Function::ImageTransferEvent* pEvent);
   void onDeviceConnected(Device::DeviceConnectEvent* pEvent);

   void doSaharaConnect();
   void doSaharaDisconnect();
   void doEdlSwitch(const std::string& programmerPath = std::string());
   void doFirehoseSwitch(
      const std::filesystem::path& buildPathDir,
      const QC::DownloadBuildOptions& options,
      const Function::ImageTransferPtr& pImageTransfer
   );
   void doFirehoseAdd();
   void doFirehoseConnect();
   void doFirehoseRemove();
   void doSaharaCleanup();
   void doFirehoseGetFlashInfo(
      QC::FlashInfo& flashInfo,
      const QC::MemoryType::type memoryType,
      int32_t partitionIndex,
      const Function::ImageTransferPtr& pImageTransfer,
      bool bReset = false
   );
   void doFirehosePreserve(
      const std::filesystem::path& buildPath,
      const std::vector<QC::PreservedPartitionInfo>& preservedPartitions,
      const Function::ImageTransferPtr& pImageTransfer
   );
   void doFirehoseRestore(const Function::ImageTransferPtr& pImageTransfer);
   void doDestroy();
   bool isMhiConnectedDevice(const Device::ImplPtr& pDevice);
   void updateMemorySettings(const Function::ImageTransferPtr& pImageTransfer);
   std::string createGuidString(uint32_t data1, uint16_t data2, uint16_t data3, uint64_t data4);

   std::shared_ptr<ImageManagementConnectionWorker> m_pImageManagementConnectionWorker;
   std::shared_ptr<Util::StdThreadWrapper> m_pImageManagementConnectionWorkerThread;
   Util::Event m_connectionWorkerEvent; ///< Event indicating connection worker is ready

   Device::Protocol::SaharaPtr m_pSaharaProtocol;
   volatile bool m_bSaharaProtocolAdded;
   Device::ConnectionPtr m_pSaharaConnection;

   Device::Protocol::FirehosePtr m_pFirehoseProtocol;
   Device::ConnectionPtr m_pFirehoseConnection;

   Util::Event m_saharaReadyEvent;   ///< Event from device manager indicating
                                     ///< Sahara interface ready
   Util::Event m_deviceReconnectedEvent;

   std::filesystem::path m_ddrStorePath;
   volatile QC::DeviceImageMode::type m_deviceImageMode;
   bool m_bEntertedFirehoseMode;

   // parameter stored for later usage in partition manager
   QC::MemoryType::type m_memoryType;
   std::filesystem::path m_availableOutputPath;
   QC::ValidationMode::type m_validationMode;
   int32_t m_numPartition; ///< Partition Number
   int32_t m_maxPayloadSize;
   QC::PreservationMode::type m_preservationMode;
   std::vector<QC::DataChunkOptions> m_preservedPartitions;

   uint32_t m_pageSize;     ///< NAND page size
   uint32_t m_pagePerBlock; ///< NAND pages per block

   uint32_t m_serialNumber;
   uint32_t m_msmHwId;
   int32_t m_slot;
   std::recursive_mutex m_mutex;

   // Moving from serviceBase
   Device::ImplPtr m_pDevice;
   Util::CheckedPointer<DeviceManagerHandler> m_pDeviceManagerHandler;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace Rpc

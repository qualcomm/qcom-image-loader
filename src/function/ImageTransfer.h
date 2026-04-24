// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "function/Fwd.h"
#include "protocol/firehose-loader/FirehoseLoader.h"
#include "protocol/Firehose.h"
#include "protocol/Sahara.h"
#include "util/AppEvent.h"
#include "util/Event.h"

#include <memory>
#include <string>
#include <vector>

namespace Function {

// ----------------------------------------------------------------------------
// ImageTransferEvent
//
/// Notifies differnt stages of the image transfer
// ----------------------------------------------------------------------------
class ImageTransferEvent : public Util::Event
{
   TOOLS_FORBID_COPY(ImageTransferEvent);

public:
   enum EventId
   {
      INITIALIZE = 0,
      SAHARA_TRANSFER_IMAGE,
      SAHARA_RESETTING,
      TRANSFER_FIREHOSE_PROGRAMMER,
      FIREHOSE_TRANSFER_RAW_PROGRAMMER,
      FIREHOSE_TRANSFER_PATCH_PROGRAMMER,
      FIREHOSE_UFS_PROVISIONING,
      FIREHOSE_SELECT_PARTITION,
      FIREHOSE_RESETTING,
      FIREHOSE_ERASE,
      FIREHOSE_READ_IMAGE_FILES,
      FIREHOSE_GET_STORAGE_INFO,
      FIREHOSE_CREATE_DIGESTS,
      FIREHOSE_VIP_DOWNLOAD,
      FIREHOSE_PROGRESSION = 254,
      FIREHOSE_INFORMATION,
   };

   ImageTransferEvent(EventId eventId, const std::string& description);
   virtual ~ImageTransferEvent();

   EventId getEventId() const;
   std::string getDescription() const;

private:
   EventId m_eventId;
   std::string m_description;
};

// ----------------------------------------------------------------------------
// ImageTransfer
//
/// Downloads build to device over the Sahara and Firehose protocol
// ----------------------------------------------------------------------------
class ImageTransfer : public Util::EventPublisher
{
   TOOLS_FORBID_COPY(ImageTransfer);

public:
   static const int32_t INVALID_LUN = -1;
   static const int32_t INVALID_START_SECTOR = -1;
   static const int32_t INVALID_SOC_HW_VERSION = -1;
   static const int32_t INVALID_BOOT_CONFIG = -1;
   enum MemoryType
   {
      MEMORY_TYPE_EMMC = 0,
      MEMORY_TYPE_UFS,
      MEMORY_TYPE_NAND,
      MEMORY_TYPE_NVME,
      MEMORY_TYPE_SPINOR,
      MEMORY_TYPE_UNKNOWN
   };

   enum ValidationMode
   {
      VALIDATION_MODE_NONE = 0,
      VALIDATION_MODE_BINARY_READBACK,
      VALIDATION_MODE_SHA256_READBACK,
      VALIDATION_MODE_BINARY_READBACK_WITH_DIGESTS_FILE,
      VALIDATION_MODE_SHA256_READBACK_WITH_DIGESTS_FILE,
   };

   enum OfflineProcess
   {
      PROCESS_VIP_DIGEST,
      PROCESS_BUILD_VALIDATION_DIGEST,
   };

   struct EdlDeviceInfo
   {
      uint32_t serialNumber;
      uint32_t msmHwId;
   };

   enum DigestHeaderType
   {
      DIGEST_HEADER_TYPE_NONE = 0,
      DIGEST_HEADER_TYPE_ELF,
      DIGEST_HEADER_TYPE_MBN,
   };

   enum ZlpAwareHostOption
   {
      ZLP_AWARE_HOST_NO_SET  = -1,
      ZLP_AWARE_HOST_DISABLE = 0,
      ZLP_AWARE_HOST_ENABLE  = 1,
   };

   struct DataChunkOptions
   {
      int32_t partitionIndex;
      std::string startSector;
      std::string sectorCount;
      std::filesystem::path imageFile;
   };

   struct PatchOptions
   {
      int32_t partitionIndex;
      std::string startSector;
      uint64_t byteOffset;
      uint32_t valueSizeInBytes;
      std::string value;
   };

   struct PartitionHeader
   {
      uint64_t signature;
      uint32_t revision;
      uint32_t headerSize;
      uint32_t headerCrc32;
      uint32_t reserved;
      uint64_t myLba;
      uint64_t alternateLba;
      uint64_t firstUsableLba;
      uint64_t lastUsableLba;
      uint64_t diskGuidLower;
      uint64_t diskGuidHigher;
      uint64_t partitionEntryLba;
      uint32_t numberOfPartitionEntries;
      uint32_t sizeOfPartitionEntry;
      uint32_t partitionEntryArrayCrc32;
   };

   struct PartitionEntryParameters
   {
      uint32_t partitionTypeGuid1;
      uint16_t partitionTypeGuid2;
      uint16_t partitionTypeGuid3;
      uint64_t partitionTypeGuid4;
      uint32_t uniquePartitionGuid1;
      uint16_t uniquePartitionGuid2;
      uint16_t uniquePartitionGuid3;
      uint64_t uniquePartitionGuid4;
      uint64_t startingLba;
      uint64_t endingLba;
      uint64_t attributes;
   };

   struct MibibHeader
   {
      uint32_t magicNumber1;
      uint32_t magicNumber2;
      uint32_t sizeOfPartitionEntry;
      uint32_t numberOfPartitionHeaderEntries;
   };

   struct MibibPartitionHeader
   {
      uint32_t magicNumber1;
      uint32_t magicNumber2;
      uint32_t sizeOfPartitionEntry;
      uint32_t numberOfPartitionEntries;
   };

   struct MibibPartitionEntryParameters
   {
      uint32_t startingLba;
      uint32_t lengthLba;
      uint32_t attributes;
   };

   struct PartitionEntry
   {
      uint32_t partitionIndex;
      PartitionEntryParameters partitionEntryParameters;
      std::string partitionEntryName;
   };

   typedef std::map<uint64_t, std::filesystem::path> BuildImageList;

   ImageTransfer(const Device::ConnectionPtr& pSaharaConnection, const Device::ConnectionPtr& pFirehoseConnection);

   virtual ~ImageTransfer();

   void
   UpdateConnection(const Device::ConnectionPtr& pSaharaConnection, const Device::ConnectionPtr& pFirehoseConnection);
   void getDeviceInfo(EdlDeviceInfo& deviceInfo);
   void transferFirehoseProgrammer(const std::filesystem::path& imageFile, BuildImageList& imageList);
   Device::Protocol::Sahara::Mode transferImageList(const BuildImageList& imageList);

   void setDdrStorePath(const std::filesystem::path& path);
   void firehoseSetMemoryType(const MemoryType type);
   void firehoseSetSlot(const int32_t slot);
   void firehoseSetSignedDigestsFile(const std::filesystem::path& signedDigestsFile);
   void firehoseSetChainedDigestsFile(const std::filesystem::path& chainedDigestsFile);
   void firehoseSetFirmwarePath(const std::filesystem::path& firmwareFile);
   void firehoseSetProvisionPath(const std::filesystem::path& provisionFile);
   void firehoseSetJsonPath(const std::filesystem::path& jsonFile);
   void firehoseSetSingleImagePath(const std::filesystem::path& jsonFile);
   void firehoseSetRawXmlList(const std::vector<std::filesystem::path>& xmlFiles);
   void firehoseSetPatchXmlList(const std::vector<std::filesystem::path>& xmlFiles);
   void firehoseSetPartitionIndexList(const std::vector<int32_t>& partitionIndexList);
   void firehoseSetPartitionNumber(const int32_t numPartition);
   void firehoseSetEnableResetAfterDownload(const bool bEnableResetAfterDownload);
   void firehoseSetEnableEraseBeforeDownload(const bool bEnableEraseBeforeDownload);
   void firehoseSetActivePartition(const int32_t activePartition);
   void firehoseSetDownloadRxTimeoutInMs(const std::chrono::milliseconds& downloadRxTimeoutInMs);
   void firehoseSetZlpAwareHost(ZlpAwareHostOption zlpAwareHost);
   void firehoseSetOutpuDir(const std::filesystem::path& outputDir);
   void firehoseSetReadImages(bool bEnableReadImages);
   void firehoseSetValidateImageSize(bool bValidateImageSize);
   void firehoseSetMaxPayloadSize(const int32_t maxPayloadSizeInByte);
   void firehoseSetMaxReadPayloadSize(const int32_t maxReadPayloadSizeInByte);
   void firehoseSetExcludeErasePartitionInfo(const std::string& excludeErasePartitionInfo);
   void firehoseSetValidationMode(const ValidationMode mode);
   void firehoseSetEdmaPath(const std::filesystem::path& edmaFile);
   void firehoseSetDigestHeaderType(const DigestHeaderType digestType);
   void firehoseSetBuildValidationDigests(const std::filesystem::path& digestsFile);
   void firehoseSetLun(const int32_t lun);
   void firehoseSetStartSector(const int32_t startSector);
   // NAND parameter
   void firehoseSetPageSize(const uint32_t pageSize);
   void firehoseSetPagePerBlock(const uint32_t pagePerBlock);

   void firehoseErase(void);
   void firehoseReset();

   void firehoseGetPartitionEntries(std::vector<PartitionEntry>& partitionEntries);
   void firehoseSavePartitionSectors(const std::vector<DataChunkOptions>& options);
   void firehoseFlashPartitionSectors(const std::vector<DataChunkOptions>& options);
   void firehoseErasePartitionSectors(const std::vector<DataChunkOptions>& options);
   void firehoseGetStorageInfo(
      const int32_t phyPartitionNum,
      Device::Protocol::Firehose::StorageInfoType& storageInfo,
      bool bReset
   );
   void firehoseRunOfflineProcess(
      const OfflineProcess process,
      const std::filesystem::path& buildPathDir,
      const std::filesystem::path& outputPathDir
   );
   void firehoseDoDownload(const std::filesystem::path& buildPathDir);

   void firehoseFormatDownloadBuildParameter(const std::filesystem::path& buildPathDir, bool bGetFlashInfo = false);

   uint64_t convertToSectorNumber(const std::string& sectorString, const uint64_t numDiskSectors) const;
   void
   getPartitionEntriesFromBuild(const std::filesystem::path& buildPath, std::vector<PartitionEntry>& partitionEntries);
   MemoryType getDeviceMemoryType();

private:
   void onFirehoseEvent(Device::Protocol::FirehoseEvent* pEvent);
   void saharaWarmReset();
   Device::SharedByteBufferPtr saharaWaitForHandshakeReady(
      const Device::Protocol::Sahara::Mode mode,
      const uint32_t leftoverPacketCount = 1,
      const std::chrono::milliseconds& timeout = std::chrono::milliseconds(10000)
   );
   Device::SharedByteBufferPtr saharaInitializeHelloRequest(
      const Device::Protocol::Sahara::Mode mode,
      const std::chrono::milliseconds& timeout = std::chrono::milliseconds(10000)
   );
   Device::Protocol::Sahara::Mode saharaWaitForNextImage(
      uint64_t& imageId,
      uint64_t& headerSize,
      bool& bMore,
      const std::chrono::milliseconds& timeout = std::chrono::milliseconds(10000)
   );
   void saharaTransferSingleImage(const std::filesystem::path& imageFile, uint64_t headerSize, bool& bMore);
   void saharaTransferImageRegion(const std::filesystem::path& imageFile, uint64_t offset, uint64_t length);
   void saharaReset();

   std::string firehoseGetMemoryName();

   std::string getSortedXmlFileString(const std::vector<std::string>& xmlPaths);
   void getPartitionHeaderFromFile(const std::filesystem::path& imageFile, PartitionHeader& partitionHeader);
   void getPartitionEntriesFromFile(
      const std::filesystem::path& imageFile,
      uint32_t partitionIndex,
      uint32_t entryCount,
      uint32_t entrySize,
      std::vector<PartitionEntry>& partitionEntryList
   );
   void getPartitionHeaderFromFileMibib(
      const std::filesystem::path& imageFile,
      uint32_t pageSize,
      uint32_t pagePerBlock,
      uint32_t& blockOffset,
      MibibPartitionHeader& partitionHeader
   );
   void getPartitionEntriesFromFileMibib(
      const std::filesystem::path& imageFile,
      uint32_t pageSize,
      uint32_t pagePerBlock,
      uint32_t blockOffset,
      uint32_t partitionIndex,
      uint32_t entryCount,
      std::vector<PartitionEntry>& partitionEntryList
   );
   uint32_t getPartitionDefaultSectorSize(MemoryType type);
   void getPartitionDefaultSectorParameterNand(uint32_t& pageSize, uint32_t& pagePerBlock);
   static bool isRunningOnWSL();
   void appendZlpAwareHostOption(Device::Protocol::Firehose::FirehoseCommandType& firehoseCommand);
   void prepareDownloadCmd(
      const std::filesystem::path& buildPathDir,
      Device::Protocol::Firehose::FirehoseCommandType& firehoseCommand,
      bool bIsOffline = false
   );
   std::list<MemoryType> retriveBootConfig(std::string& bootConfig);
   Device::Protocol::FirehoseLoaderPtr m_pOfflineFirehoseLoader;

   Device::ConnectionPtr m_pSaharaConnection;   ///< Connection to Sahara
   Device::ConnectionPtr m_pFirehoseConnection; ///< Connection to Firehose

   MemoryType m_memoryType;                           ///< Memory type used by
   int32_t m_slot;                                    ///< slot option
   std::filesystem::path m_signedDigestsFile;         ///< Signed Digests File used by VIP download
   std::filesystem::path m_chainedDigestsFile;        ///< Chained Digests File used by VIP download
   bool m_bResetAfterDownload;                        ///< Reset the device post download
   bool m_bEraseBeforeDownload;                       ///< Erase the device before download
   int32_t m_activePartition;                         ///< Active partition index
   std::filesystem::path m_firmwareFile;              ///< firmware File
   std::filesystem::path m_provisionFile;             ///< provision Xml File
   std::filesystem::path m_jsonFile;                  ///< Json file format used instead of sendXML
   std::filesystem::path m_singleImage;               ///< Single image path
   std::filesystem::path m_outputDir;                 ///< Readback output dir
   std::vector<std::filesystem::path> m_rawXmlList;   ///< rawprogram XML for sendXml
   std::vector<std::filesystem::path> m_patchXmlList; ///< patch XML for sendXml
   std::string m_xmlPathString;                       ///< XML string for sendXml
   std::string m_searchPathString;                    ///< XML string for sendXml
   std::vector<int32_t> m_partitionIndexList;         ///< Partition index list
   int32_t m_numPartition;                            ///< Partition Number
   int32_t m_maxPayloadSize;                          ///< Max payload size
   ValidationMode m_validationMode;                   ///< Build validation mode
   bool m_bReadImages;                                ///< Specify read image process
   bool m_bValidateImageSize;                         ///< Validate partition image size with partition
                                                      ///< raw xml value
   std::filesystem::path m_edmaPath;                  ///< Specify edma file
   std::string m_digestHeaderType;                    ///< Digest header type

   uint32_t m_pageSize;     ///< NAND page size
   uint32_t m_pagePerBlock; ///< NAND pages per block

   std::filesystem::path m_validationDigestsFile; ///< Build validation digest file

   std::chrono::milliseconds m_downloadRxTimeoutInMs; ///< RX timeout in millisecond
   ZlpAwareHostOption m_zlpAwareHost; ///< ZLP aware host option
   int32_t m_lun;                                     ///<  Logical unit number of storage device
   int32_t m_startSector;                             ///< Number of partition sectors
   int32_t m_socHwVersion;
   int32_t m_bootConfig;
   int32_t m_maxReadPayloadSize;            ///< Max read payload size
   std::string m_excludeErasePartitionInfo; ///< Max read payload size
};

} // namespace Function

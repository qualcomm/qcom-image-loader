// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "Definitions.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace QC {
#pragma pack(push, 1)

struct DeviceImageMode
{
   enum type
   {
      DEVICE_IMAGE_MODE_NONE = 0x00000000,
      DEVICE_IMAGE_MODE_SAHARA_DOWNLOAD = 0x00000001,
      DEVICE_IMAGE_MODE_SAHARA_CRASH = 0x00000002,
      DEVICE_IMAGE_MODE_SAHARA_EFS_SYNC = 0x00000004
   };
};

struct MemoryType
{
   enum type
   {
      MEMORY_TYPE_EMMC = 0,
      MEMORY_TYPE_UFS,
      MEMORY_TYPE_NAND,
      MEMORY_TYPE_NVME,
      MEMORY_TYPE_SPINOR,
      MEMORY_TYPE_UNKNOWN
   };
};

struct ValidationMode
{
   enum type
   {
      VALIDATION_MODE_NONE = 0,
      VALIDATION_MODE_BINARY_READBACK = 1,
      VALIDATION_MODE_SHA256_READBACK = 2,
      VALIDATION_MODE_BINARY_READBACK_WITH_DIGESTS_FILE = 3,
      VALIDATION_MODE_SHA256_READBACK_WITH_DIGESTS_FILE = 4,
   };
};

struct PreservationMode
{
   enum type
   {
      PRESERVATION_NONE = 0,
      PRESERVATION_BACKUP_RESTORE = 1
   };
};

struct DigestHeaderType
{
   enum type
   {
      DIGEST_HEADER_TYPE_NONE = 0,
      DIGEST_HEADER_TYPE_ELF,
      DIGEST_HEADER_TYPE_MBN,
   };
};

struct TransferImageResult
{
   ErrorCode::type errorCode;
   DeviceImageMode::type deviceImageMode;
};

typedef struct _PreservationOption__isset
{
   _PreservationOption__isset()
   : preservationMode(false)
   , preservedPartitions(false)
   {
   }
   bool preservationMode : 1;
   bool preservedPartitions : 1;
} _PreservationOption__isset;

typedef struct _DownloadBuildOptions__isset
{
   _DownloadBuildOptions__isset()
   : memoryType(false)
   , erase(false)
   , rawXmlList(false)
   , patchXmlList(false)
   , firehoseProgPath(false)
   , signedDigestsPath(false)
   , chainedDigestsPath(false)
   , skipSahara(false)
   , readImagesPath(false)
   , ufsProvisioningPath(false)
   , partitionIndexList(false)
   , jsonPath(false)
   , resetAfterDownload(false)
   , maxPayloadSize(false)
   , readImages(false)
   , validationMode(false)
   , edmaPath(false)
   , preservationOption(false)
   , singleImagePath(false)
   , slot(false)
   , digestHeaderType(false)
   , firehoseInitializeTimeInMs(false)
   , validationDigestsPath(false)
   , activePartition(false)
   , firmwarePath(false)
   , downloadRxTimeoutInMs(false)
   , zlpAwareHost(false)
   , lun(false)
   , startSector(false)
   , validateImageSize(false)
   , saharaImageList(false)
   , maxReadPayloadSize(false)
   , excludeErasePartitionInfo(false)
   {
   }
   bool memoryType : 1;
   bool erase : 1;
   bool rawXmlList : 1;
   bool patchXmlList : 1;
   bool firehoseProgPath : 1;
   bool signedDigestsPath : 1;
   bool chainedDigestsPath : 1;
   bool skipSahara : 1;
   bool readImagesPath : 1;
   bool ufsProvisioningPath : 1;
   bool partitionIndexList : 1;
   bool jsonPath : 1;
   bool resetAfterDownload : 1;
   bool maxPayloadSize : 1;
   bool readImages : 1;
   bool validationMode : 1;
   bool edmaPath : 1;
   bool preservationOption : 1;
   bool singleImagePath : 1;
   bool slot : 1;
   bool digestHeaderType : 1;
   bool firehoseInitializeTimeInMs : 1;
   bool validationDigestsPath : 1;
   bool activePartition : 1;
   bool firmwarePath : 1;
   bool downloadRxTimeoutInMs : 1;
   bool zlpAwareHost : 1;
   bool lun : 1;
   bool startSector : 1;
   bool validateImageSize : 1;
   bool saharaImageList : 1;
   bool maxReadPayloadSize : 1;
   bool excludeErasePartitionInfo : 1;
} _DownloadBuildOptions__isset;

typedef struct _FlashInfo__isset
{
   _FlashInfo__isset()
   : totalBlocks(false)
   , blockSize(false)
   , pageSize(false)
   , numPhysicalPartitions(false)
   , manufacturerId(false)
   , serialNum(false)
   , fwVersion(false)
   , memType(false)
   , prodName(false)
   , specVersion(false)
   {
   }
   bool totalBlocks : 1;
   bool blockSize : 1;
   bool pageSize : 1;
   bool numPhysicalPartitions : 1;
   bool manufacturerId : 1;
   bool serialNum : 1;
   bool fwVersion : 1;
   bool memType : 1;
   bool prodName : 1;
   bool specVersion : 1;
} _FlashInfo__isset;

typedef struct _DataChunkOptions__isset
{
   _DataChunkOptions__isset()
   : partitionIndex(false)
   , startSector(false)
   , sectorCount(false)
   , imagePath(false)
   {
   }
   bool partitionIndex : 1;
   bool startSector : 1;
   bool sectorCount : 1;
   bool imagePath : 1;
} _DataChunkOptions__isset;

#pragma pack(pop)

// Structs and classes with C++ objects (std::string, std::vector, std::map)
// must use default alignment

struct EdlDeviceInfo
{
   std::string serialNumber;
   std::string msmHwId;
};

struct PreservedPartitionInfo
{
   std::string partitionIndex;
   std::string name;
};


struct PartitionInfo
{
   int32_t lun;
   std::string name;
   std::string partitionTypeGuid;
   std::string uniquePartitionGuid;
   int64_t startingLba;
   int64_t endingLba;
   int64_t attributes;
};

class PreservationOption
{
public:
   PreservationOption(PreservationMode::type preservationMode);
   ~PreservationOption();

   _PreservationOption__isset __isset;

   PreservationMode::type preservationMode;
   /*optional*/ std::vector<PreservedPartitionInfo> preservedPartitions;
   void __set_preservationMode(const PreservationMode::type val);
   void __set_preservedPartitions(const std::vector<PreservedPartitionInfo>& val);

private:
   friend class DownloadBuildOptions;
   PreservationOption();
};

class DownloadBuildOptions
{
public:
   DownloadBuildOptions(MemoryType::type memoryType);
   ~DownloadBuildOptions();

   _DownloadBuildOptions__isset __isset;

   MemoryType::type memoryType;
   /*optional*/ bool erase;
   /*optional*/ std::vector<std::string> rawXmlList;
   /*optional*/ std::vector<std::string> patchXmlList;
   /*optional*/ std::string firehoseProgPath;
   /*optional*/ std::string signedDigestsPath;
   /*optional*/ std::string chainedDigestsPath;
   /*optional*/ bool skipSahara;
   /*optional*/ std::string readImagesPath;
   /*optional*/ std::string ufsProvisioningPath;
   /*optional*/ std::vector<int32_t> partitionIndexList;
   /*optional*/ std::string jsonPath;
   /*optional*/ bool resetAfterDownload;
   /*optional*/ int32_t maxPayloadSize;
   /*optional*/ bool readImages;
   /*optional*/ ValidationMode::type validationMode;
   /*optional*/ std::string edmaPath;
   /*optional*/ PreservationOption preservationOption;
   /*optional*/ std::string singleImagePath;
   /*optional*/ int32_t slot;
   /*optional*/ DigestHeaderType::type digestHeaderType;
   /*optional*/ int32_t firehoseInitializeTimeInMs;
   /*optional*/ std::string validationDigestsPath;
   /*optional*/ int32_t activePartition;
   /*optional*/ std::string firmwarePath;
   /*optional*/ int32_t downloadRxTimeoutInMs;
   /*optional*/ bool zlpAwareHost;
   /*optional*/ int32_t lun;
   /*optional*/ int64_t startSector;
   /*optional*/ bool validateImageSize;
   /*optional*/ std::map<int32_t, std::string> saharaImageList;
   /*optional*/ int32_t maxReadPayloadSize;
   /*optional*/ std::vector<PartitionInfo> excludeErasePartitionInfo;


   void __set_memoryType(const MemoryType::type val);
   void __set_erase(const bool val);
   void __set_rawXmlList(const std::vector<std::string>& val);
   void __set_patchXmlList(const std::vector<std::string>& val);
   void __set_firehoseProgPath(const std::string& val);
   void __set_signedDigestsPath(const std::string& val);
   void __set_chainedDigestsPath(const std::string& val);
   void __set_skipSahara(const bool val);
   void __set_readImagesPath(const std::string& val);
   void __set_ufsProvisioningPath(const std::string& val);
   void __set_partitionIndexList(const std::vector<int32_t>& val);
   void __set_jsonPath(const std::string& val);
   void __set_resetAfterDownload(const bool val);
   void __set_maxPayloadSize(const int32_t val);
   void __set_readImages(const bool val);
   void __set_validationMode(const ValidationMode::type val);
   void __set_edmaPath(const std::string& val);
   void __set_preservationOption(const PreservationOption& val);
   void __set_singleImagePath(const std::string& val);
   void __set_slot(const int32_t val);
   void __set_digestHeaderType(const DigestHeaderType::type val);
   void __set_firehoseInitializeTimeInMs(const int32_t val);
   void __set_validationDigestsPath(const std::string& val);
   void __set_activePartition(const int32_t val);
   void __set_firmwarePath(const std::string& val);
   void __set_downloadRxTimeoutInMs(const int32_t val);
   void __set_zlpAwareHost(const bool val);
   void __set_lun(const int32_t val);
   void __set_startSector(const int64_t val);
   void __set_validateImageSize(const bool val);
   void __set_saharaImageList(const std::map<int32_t, std::string>& val);
   void __set_maxReadPayloadSize(const int32_t val);
   void __set_excludeErasePartitionInfo(const std::vector<PartitionInfo>& val);
};

class FlashInfo
{
public:
   FlashInfo();
   ~FlashInfo();

   _FlashInfo__isset __isset;

   /*optional*/ std::string totalBlocks;
   /*optional*/ std::string blockSize;
   /*optional*/ std::string pageSize;
   /*optional*/ std::string numPhysicalPartitions;
   /*optional*/ std::string manufacturerId;
   /*optional*/ std::string serialNum;
   /*optional*/ std::string fwVersion;
   /*optional*/ std::string memType;
   /*optional*/ std::string prodName;
   /*optional*/ std::string specVersion;

   void __set_totalBlocks(const std::string& val);
   void __set_blockSize(const std::string& val);
   void __set_pageSize(const std::string& val);
   void __set_numPhysicalPartitions(const std::string& val);
   void __set_manufacturerId(const std::string& val);
   void __set_serialNum(const std::string& val);
   void __set_fwVersion(const std::string& val);
   void __set_memType(const std::string& val);
   void __set_prodName(const std::string& val);
   void __set_specVersion(const std::string& val);
};

class DataChunkOptions
{
public:
   DataChunkOptions(int32_t partitionIndex, std::string startSector);
   ~DataChunkOptions();
   _DataChunkOptions__isset __isset;
   int32_t partitionIndex;
   std::string startSector;
   /*optional*/ std::string sectorCount;
   /*optional*/ std::string imagePath;

   void __set_partitionIndex(const int32_t val);
   void __set_startSector(const std::string& val);
   void __set_sectorCount(const std::string& val);
   void __set_imagePath(const std::string& val);
};

}; // namespace QC

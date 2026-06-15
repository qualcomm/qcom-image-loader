// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "ImageManagementDefinitions.h"

namespace QC {


PreservationOption::~PreservationOption() noexcept
{
}
void PreservationOption::__set_preservationMode(const PreservationMode::type val)
{
   this->preservationMode = val;
}
void PreservationOption::__set_preservedPartitions(const std::vector<PreservedPartitionInfo>& val)
{
   this->preservedPartitions = val;
   __isset.preservedPartitions = true;
}
PreservationOption::PreservationOption(PreservationMode::type preservationMode)
{
   this->preservationMode = preservationMode;
   __isset.preservationMode = true;
}
PreservationOption::PreservationOption()
{
}


DownloadBuildOptions::~DownloadBuildOptions() noexcept
{
}
void DownloadBuildOptions::__set_memoryType(const MemoryType::type val)
{
   this->memoryType = val;
}
void DownloadBuildOptions::__set_erase(const bool val)
{
   this->erase = val;
   __isset.erase = true;
}
void DownloadBuildOptions::__set_rawXmlList(const std::vector<std::string>& val)
{
   this->rawXmlList = val;
   __isset.rawXmlList = true;
}
void DownloadBuildOptions::__set_patchXmlList(const std::vector<std::string>& val)
{
   this->patchXmlList = val;
   __isset.patchXmlList = true;
}
void DownloadBuildOptions::__set_firehoseProgPath(const std::string& val)
{
   this->firehoseProgPath = val;
   __isset.firehoseProgPath = true;
}
void DownloadBuildOptions::__set_signedDigestsPath(const std::string& val)
{
   this->signedDigestsPath = val;
   __isset.signedDigestsPath = true;
}
void DownloadBuildOptions::__set_chainedDigestsPath(const std::string& val)
{
   this->chainedDigestsPath = val;
   __isset.chainedDigestsPath = true;
}
void DownloadBuildOptions::__set_skipSahara(const bool val)
{
   this->skipSahara = val;
   __isset.skipSahara = true;
}
void DownloadBuildOptions::__set_readImagesPath(const std::string& val)
{
   this->readImagesPath = val;
   __isset.readImagesPath = true;
}

void DownloadBuildOptions::__set_ufsProvisioningPath(const std::string& val)
{
   this->ufsProvisioningPath = val;
   __isset.ufsProvisioningPath = true;
}
void DownloadBuildOptions::__set_partitionIndexList(const std::vector<int32_t>& val)
{
   this->partitionIndexList = val;
   __isset.partitionIndexList = true;
}
void DownloadBuildOptions::__set_jsonPath(const std::string& val)
{
   this->jsonPath = val;
   __isset.jsonPath = true;
}
void DownloadBuildOptions::__set_resetAfterDownload(const bool val)
{
   this->resetAfterDownload = val;
   __isset.resetAfterDownload = true;
}
void DownloadBuildOptions::__set_maxPayloadSize(const int32_t val)
{
   this->maxPayloadSize = val;
   __isset.maxPayloadSize = true;
}
void DownloadBuildOptions::__set_readImages(const bool val)
{
   this->readImages = val;
   __isset.readImages = true;
}
void DownloadBuildOptions::__set_validationMode(const ValidationMode::type val)
{
   this->validationMode = val;
   __isset.validationMode = true;
}
void DownloadBuildOptions::__set_edmaPath(const std::string& val)
{
   this->edmaPath = val;
   __isset.edmaPath = true;
}
void DownloadBuildOptions::__set_preservationOption(const PreservationOption& val)
{
   this->preservationOption = val;
   __isset.preservationOption = true;
}
void DownloadBuildOptions::__set_singleImagePath(const std::string& val)
{
   this->singleImagePath = val;
   __isset.singleImagePath = true;
}
void DownloadBuildOptions::__set_slot(const int32_t val)
{
   this->slot = val;
   __isset.slot = true;
}
void DownloadBuildOptions::__set_digestHeaderType(const DigestHeaderType::type val)
{
   this->digestHeaderType = val;
   __isset.digestHeaderType = true;
}
void DownloadBuildOptions::__set_firehoseInitializeTimeInMs(const int32_t val)
{
   this->firehoseInitializeTimeInMs = val;
   __isset.firehoseInitializeTimeInMs = true;
}
void DownloadBuildOptions::__set_validationDigestsPath(const std::string& val)
{
   this->validationDigestsPath = val;
   __isset.validationDigestsPath = true;
}
void DownloadBuildOptions::__set_activePartition(const int32_t val)
{
   this->activePartition = val;
   __isset.activePartition = true;
}
void DownloadBuildOptions::__set_firmwarePath(const std::string& val)
{
   this->firmwarePath = val;
   __isset.firmwarePath = true;
}

void DownloadBuildOptions::__set_downloadRxTimeoutInMs(const int32_t val)
{
   this->downloadRxTimeoutInMs = val;
   __isset.downloadRxTimeoutInMs = true;
}
void DownloadBuildOptions::__set_zlpAwareHost(const bool val)
{
   this->zlpAwareHost = val;
   __isset.zlpAwareHost = true;
}
void DownloadBuildOptions::__set_lun(const int32_t val)
{
   this->lun = val;
   __isset.lun = true;
}
void DownloadBuildOptions::__set_startSector(const int64_t val)
{
   this->startSector = val;
   __isset.startSector = true;
}
void DownloadBuildOptions::__set_validateImageSize(const bool val)
{
   this->validateImageSize = val;
   __isset.validateImageSize = true;
}
void DownloadBuildOptions::__set_saharaImageList(const std::map<int32_t, std::string>& val)
{
   this->saharaImageList = val;
   __isset.saharaImageList = true;
}
void DownloadBuildOptions::__set_maxReadPayloadSize(const int32_t val)
{
   this->maxReadPayloadSize = val;
   __isset.maxReadPayloadSize = true;
}
void DownloadBuildOptions::__set_excludeErasePartitionInfo(const std::vector<PartitionInfo>& val)
{
   this->excludeErasePartitionInfo = val;
   __isset.excludeErasePartitionInfo = true;
}
DownloadBuildOptions::DownloadBuildOptions(MemoryType::type memoryType)
{
   this->memoryType = memoryType;
   __isset.memoryType = true;
}


FlashInfo::~FlashInfo() noexcept
{
}
void FlashInfo::__set_totalBlocks(const std::string& val)
{
   this->totalBlocks = val;
   __isset.totalBlocks = true;
}
void FlashInfo::__set_blockSize(const std::string& val)
{
   this->blockSize = val;
   __isset.blockSize = true;
}
void FlashInfo::__set_pageSize(const std::string& val)
{
   this->pageSize = val;
   __isset.pageSize = true;
}
void FlashInfo::__set_numPhysicalPartitions(const std::string& val)
{
   this->numPhysicalPartitions = val;
   __isset.numPhysicalPartitions = true;
}
void FlashInfo::__set_manufacturerId(const std::string& val)
{
   this->manufacturerId = val;
   __isset.manufacturerId = true;
}
void FlashInfo::__set_serialNum(const std::string& val)
{
   this->serialNum = val;
   __isset.serialNum = true;
}
void FlashInfo::__set_fwVersion(const std::string& val)
{
   this->fwVersion = val;
   __isset.fwVersion = true;
}
void FlashInfo::__set_memType(const std::string& val)
{
   this->memType = val;
   __isset.memType = true;
}
void FlashInfo::__set_prodName(const std::string& val)
{
   this->prodName = val;
   __isset.prodName = true;
}
void FlashInfo::__set_specVersion(const std::string& val)
{
   this->specVersion = val;
   __isset.specVersion = true;
}
FlashInfo::FlashInfo()
{
}


DataChunkOptions::~DataChunkOptions() noexcept
{
}
void DataChunkOptions::__set_partitionIndex(const int32_t val)
{
   this->partitionIndex = val;
}
void DataChunkOptions::__set_startSector(const std::string& val)
{
   this->startSector = val;
}
void DataChunkOptions::__set_sectorCount(const std::string& val)
{
   this->sectorCount = val;
   __isset.sectorCount = true;
}
void DataChunkOptions::__set_imagePath(const std::string& val)
{
   this->imagePath = val;
   __isset.imagePath = true;
}
DataChunkOptions::DataChunkOptions(int32_t partitionIndex, std::string startSector)
{
   this->partitionIndex = partitionIndex;
   this->startSector = startSector;
   __isset.partitionIndex = true;
   __isset.startSector = true;
}


} // namespace QC

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
// Prevent ambiguous 'byte' symbol conflict between Windows headers and
// std::byte Must be defined before any Windows headers are included
#ifndef _HAS_STD_BYTE
#define _HAS_STD_BYTE 0
#endif
#include "function/ImageTransfer.h"

#include "device/Buffer.h"
#include "device/Connection.h"
#include "device/ErrorMessage.h"
#include "device/Exception.h"
#include "device/Impl.h"
#include "device/Manager.h"
#include "protocol/firehose-loader/FirehoseLoader.h"
#include "protocol/Firehose.h"
#include "util/StringHelper.h"
#include "util/ThisThread.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <algorithm>
#include <regex>
#include <string>

using namespace Device::Protocol;

namespace Function {

// ----------------------------------------------------------------------------
// isRunningOnWSL
//
/// Returns true if running under Windows Subsystem for Linux by checking
/// /proc/version for the strings "microsoft" or "wsl" (case-insensitive).
/// Always returns false on non-Linux platforms.
// ----------------------------------------------------------------------------
bool ImageTransfer::isRunningOnWSL()
{
#ifdef TOOLS_TARGET_LINUX
   std::ifstream procVersion("/proc/version");
   if(!procVersion.is_open())
   {
      return false;
   }
   std::string line;
   std::getline(procVersion, line);
   std::transform(line.begin(), line.end(), line.begin(), ::tolower);
   return line.find("microsoft") != std::string::npos || line.find("wsl") != std::string::npos;
#else
   return false;
#endif
}

// ----------------------------------------------------------------------------
// appendZlpAwareHostOption
//
/// Appends --zlpawarehost= to firehoseCommand if applicable.
/// On WSL, disables ZLP aware host by default unless the user explicitly set it.
// ----------------------------------------------------------------------------
void ImageTransfer::appendZlpAwareHostOption(Firehose::FirehoseCommandType& firehoseCommand)
{
   ZlpAwareHostOption effectiveZlpAwareHost =
      (m_zlpAwareHost == ZLP_AWARE_HOST_NO_SET && isRunningOnWSL()) ? ZLP_AWARE_HOST_DISABLE : m_zlpAwareHost;
   if(effectiveZlpAwareHost != ZLP_AWARE_HOST_NO_SET)
   {
      firehoseCommand.push_back(std::string("--zlpawarehost=") +
                                (effectiveZlpAwareHost == ZLP_AWARE_HOST_ENABLE ? "1" : "0"));
   }
}

static const uint64_t MAX_SAHARA_WRITE = 1024 * 1024;
static const uint32_t MAX_SAHARA_LEFTOVER_PACKETS_COUNT = 3;

static const uint32_t EDL_SERIAL_NUMBER_LENGTH = 4;

static const uint32_t PARTITION_HEADER_SECTOR_OFFSET = 1;
static const uint32_t PARTITION_HEADER_SECTOR_NUM = 1;
static const uint32_t PARTITION_HEADER_BLOCK_NUM_NAND = 32;
static const uint64_t PARTITION_HEADER_SIGNATURE = 0x5452415020494645;
static const uint32_t PARTITION_DEFAULT_SECTOR_SIZE = 512;
static const uint32_t PARTITION_DEFAULT_SECTOR_SIZE_4K = 4096;
static const uint32_t PARTITION_DEFAULT_PAGE_PER_BLOCK_SIZE_64K = 64;

static const std::string PROGRAM_TAG = "program";
static const std::string PATCH_TAG = "patch";

static const std::string SECTOR_SIZE_IN_BYTES = "SECTOR_SIZE_IN_BYTES";
static const std::string PHYSICAL_PARTITION_NUMBER = "physical_partition_number";
static const std::string START_SECTOR = "start_sector";
static const std::string NUM_PARTITION_SECTORS = "num_partition_sectors";
static const std::string FILE_SECTOR_OFFSET = "file_sector_offset";
static const std::string FILE_NAME = "filename";
static const std::string BYTE_OFFSET = "byte_offset";
static const std::string SIZE_IN_BYTES = "size_in_bytes";
static const std::string VALUE = "value";

// Mibib magic number
static const uint32_t AUTODETECT_PAGE_SIZE_MAGIC_NUM = 0x7D0B435A;
static const uint32_t MIBIB_MAGIC1 = 0xFE569FAC;
static const uint32_t MIBIB_MAGIC2 = 0xCD7F127A;
static const uint32_t MIBIB_PARAMETER_MAGIC1 = 0x55EE73AA;
static const uint32_t MIBIB_PARAMETER_MAGIC2 = 0xE35EBDDB;

static const uint32_t MIBIB_NUM_PARTITION_HEADER = 1;
static const uint32_t MIBIB_PARTITION_NAME_SIZE = 16;

static const int32_t ACTIVE_PARTITION_NO_SET = -1;
static const int32_t DEFAULT_UFS_BOOTABLE_PARTITION = 1;

// Boot config number
static const uint32_t SOC_HW_VERSION_OFFSET = 34;
static const uint32_t SOC_HW_VERSION_MIN_SIZE = 36;
static const uint32_t BOOT_CONFIG_MIN_SIZE = 1684;
static const uint32_t BOOT_CONFIG_OFFSET = 1680;

static const uint32_t SAHARA_COMMAND_READY_WAIT_ATTEMPTS = 5;

// ----------------------------------------------------------------------------
// XmlPartitionInfoData
//
// ----------------------------------------------------------------------------
class XmlPartitionInfo
{
   TOOLS_FORBID_COPY(XmlPartitionInfo);

public:
   XmlPartitionInfo()
   : m_dataChunks()
   , m_sectorSizeInBytes(0)
   , m_partitionIndex(0)
   , m_startSector()
   , m_sectorCount()
   , m_imageFile()
   , m_state(0)
   {
   }
   ~XmlPartitionInfo()
   {
   }

   uint64_t getSectorSizeInBytes() const
   {
      return m_sectorSizeInBytes;
   }
   void setSectorSizeInBytes(const uint64_t sectorSizeInByte)
   {
      m_sectorSizeInBytes = sectorSizeInByte;
   }
   void setPartitionIndex(const int32_t partitionIndex)
   {
      m_partitionIndex = partitionIndex;
      m_state |= PARTITION_INDEX_BIT;
      createNewDataChunk();
   }
   void setStartSector(const std::string& startSector)
   {
      m_startSector = startSector;
      m_state |= START_SECTOR_BIT;
      createNewDataChunk();
   }
   void setSectorCount(const std::string& sectorCount)
   {
      m_sectorCount = sectorCount;
      m_state |= SECTOR_COUNT_BIT;
      createNewDataChunk();
   }
   void setImageFile(const std::filesystem::path& imageFile)
   {
      m_imageFile = imageFile;
      m_state |= IMAGE_FILE_BIT;
      createNewDataChunk();
   }

   bool getPartitionTable(const int32_t partitionIndex, Function::ImageTransfer::DataChunkOptions& _return)
   {
      std::vector<Function::ImageTransfer::DataChunkOptions>::const_iterator it = std::find_if(
         m_dataChunks.begin(),
         m_dataChunks.end(),
         [&partitionIndex](const Function::ImageTransfer::DataChunkOptions& thisPartition) {
            if(thisPartition.partitionIndex != partitionIndex)
            {
               return false;
            }
            if("0" != thisPartition.startSector)
            {
               return false;
            }
            return true;
         }
      );
      if(it != m_dataChunks.end())
      {
         _return = *it;
         return true;
      }
      return false;
   }

private:
   static const uint32_t PARTITION_INDEX_BIT = 1;
   static const uint32_t START_SECTOR_BIT = 1 << 1;
   static const uint32_t SECTOR_COUNT_BIT = 1 << 2;
   static const uint32_t IMAGE_FILE_BIT = 1 << 3;
   static const uint32_t MAX_PARTITION_INFO_PARAMETER = (1 << 4) - 1;

   inline void createNewDataChunk()
   {
      if(MAX_PARTITION_INFO_PARAMETER != m_state)
      {
         return;
      }
      Function::ImageTransfer::DataChunkOptions options;
      options.partitionIndex = m_partitionIndex;
      options.startSector = m_startSector;
      options.sectorCount = m_sectorCount;
      options.imageFile = m_imageFile;
      m_dataChunks.push_back(options);
      m_state = 0;
   }

   uint32_t m_state = 0;
   int32_t m_partitionIndex;
   uint64_t m_sectorSizeInBytes;
   std::string m_startSector;
   std::string m_sectorCount;
   std::filesystem::path m_imageFile;

   std::vector<Function::ImageTransfer::DataChunkOptions> m_dataChunks;
};
typedef std::shared_ptr<XmlPartitionInfo> XmlPartitionInfoPtr;

// ----------------------------------------------------------------------------
// XmlPartitionInfoData
//
// ----------------------------------------------------------------------------
class XmlPatchInfo
{
   TOOLS_FORBID_COPY(XmlPatchInfo);

public:
   XmlPatchInfo()
   : m_patches()
   , m_sectorSizeInBytes()
   , m_partitionIndex(0)
   , m_startSector()
   , m_byteOffset(0)
   , m_valueSizeInBytes(0)
   , m_value()
   , m_imageFile()
   , m_state(0)
   {
   }
   ~XmlPatchInfo()
   {
   }

   uint64_t getSectorSizeInBytes() const
   {
      return m_sectorSizeInBytes;
   }
   void setSectorSizeInBytes(const uint64_t sectorSizeInByte)
   {
      m_sectorSizeInBytes = sectorSizeInByte;
   }
   void setPartitionIndex(const int32_t partitionIndex)
   {
      m_partitionIndex = partitionIndex;
      m_state |= PARTITION_INDEX_BIT;
      createNewPatchChunk();
   }
   void setStartSector(const std::string& startSector)
   {
      m_startSector = startSector;
      m_state |= START_SECTOR_BIT;
      createNewPatchChunk();
   }
   void setByteOffset(const uint64_t byteOffset)
   {
      m_byteOffset = byteOffset;
      m_state |= BYTE_OFFSET_BIT;
      createNewPatchChunk();
   }
   void setValueSizeInBytes(const uint32_t valueSizeInBytes)
   {
      m_valueSizeInBytes = valueSizeInBytes;
      m_state |= VALUE_SIZE_BIT;
      createNewPatchChunk();
   }
   void setValue(const std::string& valueString)
   {
      m_value = valueString;
      m_state |= VALUE_BIT;
      createNewPatchChunk();
   }
   void setImageFile(const std::filesystem::path& imageFile)
   {
      m_imageFile = imageFile;
      m_state |= IMAGE_FILE_BIT;
      createNewPatchChunk();
   }

   void getPartitionPatches(const int32_t partitionIndex, std::vector<Function::ImageTransfer::PatchOptions>& _return)
   {
      std::vector<Function::ImageTransfer::PatchOptions>::const_iterator it = m_patches.begin();
      std::vector<Function::ImageTransfer::PatchOptions>::const_iterator end = m_patches.end();
      for(; it != end; ++it)
      {
         if(it->partitionIndex == partitionIndex)
         {
            if(3 < it->value.size() && "CRC" == it->value.substr(0, 3))
            {
               // ignore CRC section
               continue;
            }
            Function::ImageTransfer::PatchOptions options(*it);
            _return.push_back(options);
         }
      }
   }

private:
   static const uint32_t PARTITION_INDEX_BIT = 1;
   static const uint32_t START_SECTOR_BIT = 1 << 1;
   static const uint32_t BYTE_OFFSET_BIT = 1 << 2;
   static const uint32_t VALUE_SIZE_BIT = 1 << 3;
   static const uint32_t VALUE_BIT = 1 << 4;
   static const uint32_t IMAGE_FILE_BIT = 1 << 5;
   static const uint32_t MAX_PARTITION_INFO_PARAMETER = (1 << 6) - 1;

   inline void createNewPatchChunk()
   {
      if(MAX_PARTITION_INFO_PARAMETER != m_state)
      {
         return;
      }
      if("DISK" != m_imageFile)
      {
         return;
      }
      Function::ImageTransfer::PatchOptions options;
      options.partitionIndex = m_partitionIndex;
      options.startSector = m_startSector;
      options.byteOffset = m_byteOffset;
      options.valueSizeInBytes = m_valueSizeInBytes;
      options.value = m_value;
      m_patches.push_back(options);
      m_state = 0;
   }

   uint32_t m_state = 0;
   uint64_t m_sectorSizeInBytes;

   int32_t m_partitionIndex;
   std::string m_startSector;
   uint64_t m_byteOffset;
   uint32_t m_valueSizeInBytes;
   std::string m_value;
   std::filesystem::path m_imageFile;

   std::vector<Function::ImageTransfer::PatchOptions> m_patches;
};
typedef std::shared_ptr<XmlPatchInfo> XmlPatchInfoPtr;

// ----------------------------------------------------------------------------
// parseXmlAttributes
//
// Parse XML file and invoke attrCallback(name, value) for each attribute
// of every element matching tagName.
// ----------------------------------------------------------------------------
static void parseXmlAttributes(
   const std::filesystem::path& xmlPath,
   const std::string& tagName,
   const std::function<void(const std::string&, const std::string&)>& attrCallback
)
{
   xmlDocPtr doc = xmlReadFile(xmlPath.string().c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
   if(!doc) return;
   xmlNodePtr root = xmlDocGetRootElement(doc);
   for(xmlNodePtr node = root ? root->children : nullptr; node; node = node->next)
   {
      if(node->type != XML_ELEMENT_NODE) continue;
      if(tagName != reinterpret_cast<const char*>(node->name)) continue;
      for(xmlAttrPtr attr = node->properties; attr; attr = attr->next)
      {
         xmlChar* val = xmlGetProp(node, attr->name);
         if(val)
         {
            attrCallback(reinterpret_cast<const char*>(attr->name), reinterpret_cast<const char*>(val));
            xmlFree(val);
         }
      }
   }
   xmlFreeDoc(doc);
}

// ----------------------------------------------------------------------------
// ImageTransferEvent
//
// ----------------------------------------------------------------------------
ImageTransferEvent::ImageTransferEvent(EventId eventId, const std::string& description)
: Util::Event()
, m_eventId(eventId)
, m_description(description)
{
}

// ----------------------------------------------------------------------------
// ~ImageTransferEvent
//
// ----------------------------------------------------------------------------
ImageTransferEvent::~ImageTransferEvent()
{
}

// ----------------------------------------------------------------------------
// getEventId
//
// ----------------------------------------------------------------------------
ImageTransferEvent::EventId ImageTransferEvent::getEventId() const
{
   return m_eventId;
}

// ----------------------------------------------------------------------------
// getDescription
//
// ----------------------------------------------------------------------------
std::string ImageTransferEvent::getDescription() const
{
   return m_description;
}

// ----------------------------------------------------------------------------
// ImageTransfer
//
// ----------------------------------------------------------------------------
ImageTransfer::
   ImageTransfer(const Device::ConnectionPtr& pSaharaConnection, const Device::ConnectionPtr& pFirehoseConnection)
: m_pSaharaConnection(pSaharaConnection)
, m_pFirehoseConnection(pFirehoseConnection)
, m_memoryType(MEMORY_TYPE_UNKNOWN)
, m_slot(0)
, m_signedDigestsFile()
, m_chainedDigestsFile()
, m_bResetAfterDownload(false)
, m_bEraseBeforeDownload(false)
, m_numPartition(0)
, m_activePartition(-1)
, m_firmwareFile()
, m_provisionFile()
, m_jsonFile()
, m_singleImage()
, m_xmlPathString()
, m_maxPayloadSize(0)
, m_validationMode(VALIDATION_MODE_NONE)
, m_bReadImages(false)
, m_outputDir(Device::Manager::getInstance()->getTempDirectory())
, m_digestHeaderType("mbn")
, m_pageSize(0)
, m_pagePerBlock(0)
, m_validationDigestsFile()
, m_downloadRxTimeoutInMs(std::chrono::milliseconds(0))
, m_zlpAwareHost(ZLP_AWARE_HOST_NO_SET)
, m_lun(INVALID_LUN)
, m_startSector(INVALID_START_SECTOR)
, m_bValidateImageSize(false)
, m_searchPathString()
, m_bootConfig(INVALID_BOOT_CONFIG)
, m_socHwVersion(INVALID_SOC_HW_VERSION)
, m_maxReadPayloadSize(0)
, m_excludeErasePartitionInfo()
{
   if(m_pFirehoseConnection != nullptr)
   {
      FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
      pFirehose->subscribe(this, &ImageTransfer::onFirehoseEvent);
   }
}

// ----------------------------------------------------------------------------
// ~ImageTransfer
//
// ----------------------------------------------------------------------------
ImageTransfer::~ImageTransfer()
{
   if(m_pFirehoseConnection != nullptr)
   {
      FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
      pFirehose->unsubscribe(this, &ImageTransfer::onFirehoseEvent);
   }
}

// ----------------------------------------------------------------------------
// UpdateConnection
//
/// Update the connection
// ----------------------------------------------------------------------------
void ImageTransfer::
   UpdateConnection(const Device::ConnectionPtr& pSaharaConnection, const Device::ConnectionPtr& pFirehoseConnection)
{
   m_pSaharaConnection = pSaharaConnection;
   m_pFirehoseConnection = pFirehoseConnection;
   if(m_pFirehoseConnection != nullptr)
   {
      FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
      pFirehose->unsubscribe(this, &ImageTransfer::onFirehoseEvent);
   }
   m_pFirehoseConnection = pFirehoseConnection;
   if(m_pFirehoseConnection != nullptr)
   {
      FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
      pFirehose->subscribe(this, &ImageTransfer::onFirehoseEvent);
   }
}

// ----------------------------------------------------------------------------
// saharaWaitForHandshakeReady
//
/// wait for a sahara mode ready
// ----------------------------------------------------------------------------
Device::SharedByteBufferPtr ImageTransfer::saharaWaitForHandshakeReady(
   const Device::Protocol::Sahara::Mode mode,
   const uint32_t leftoverPacketCount,
   const std::chrono::milliseconds& timeout
)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Device::Protocol::Sahara>();
   Device::SharedByteBufferPtr pBuffer;

   // Check if there is already a packet waiting. Skip leftover packets
   for(uint32_t i = 0; i < leftoverPacketCount; ++i)
   {
      if(0 == i)
      {
         pBuffer = pSahara->getFrame(timeout);
      }
      else
      {
         pBuffer = pSahara->getFrame();
      }
      if(pBuffer == nullptr)
      {
         // Timeout, break and try sending hello response later later
         FLOG_WARNING(
            "No packet received from device waiting for initial "
            "hello (image_tx_pending state) waitMode: " +
            std::to_string(mode) + " in Sahara protocol: " + pSahara->getDescription()
         );
         return nullptr;
      }
      else
      {
         const Sahara::FrameHeader* pHeader =
            Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());

         if(Sahara::SAHARA_HELLO == pHeader->m_command)
         {
            // valid command or hello, continue
            return pBuffer;
         }
         else if(Sahara::Mode::MODE_IMAGE_TX_PENDING == mode &&
                 (Sahara::CommandId::SAHARA_READ_DATA == pHeader->m_command ||
                  Sahara::CommandId::SAHARA_64_BIT_READ_DATA == pHeader->m_command))
         {
            return pBuffer;
         }
         else if(Sahara::Mode::MODE_COMMAND == mode && Sahara::CommandId::SAHARA_COMMAND_READY == pHeader->m_command)
         {
            return pBuffer;
         }
         else
         {
            // left over command, wait for next command
            FLOG_WARNING(
               "Leftover sahara packet from device waiting for initial hello "
               "(image_tx_pending state): " +
               std::string(std::to_string(pHeader->m_command)) +
               " size: " + std::string(std::to_string(pHeader->m_length)) + " waitMode: " + std::to_string(mode) +
               " in Sahara protocol: " + pSahara->getDescription()
            );
         }
      }
   }
   return nullptr;
}

// ----------------------------------------------------------------------------
// saharaInitializeHelloRequest
//
/// sahara wait for initialize hello handshake process
// ----------------------------------------------------------------------------
Device::SharedByteBufferPtr
ImageTransfer::saharaInitializeHelloRequest(const Sahara::Mode mode, const std::chrono::milliseconds& timeout)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Device::Protocol::Sahara>();
   Device::SharedByteBufferPtr pSendBuffer;
   Device::SharedByteBufferPtr pBuffer;
   Device::SharedByteBufferPtr pHelloResponseBuffer =
      pSahara->createCommand<Sahara::HelloResponse>(Sahara::SAHARA_HELLO_RESP);
   Sahara::HelloResponse* pHelloResp =
      Util::buffer_cast<Sahara::HelloResponse*>(pHelloResponseBuffer->begin(), pHelloResponseBuffer->size());

   pBuffer = saharaWaitForHandshakeReady(mode, MAX_SAHARA_LEFTOVER_PACKETS_COUNT, timeout);

   // If no incoming packet, send a hello response assuming hello packet has
   // been consumed
   if(pBuffer == nullptr)
   {
      FLOG_WARNING(
         "Unframed packet received from device waiting for initial hello "
         "(image_tx_pending state) try sending hello response with mode:" +
         std::to_string(mode) + " in Sahara protocol: " + pSahara->getDescription()
      );
      pHelloResp->m_versionNumber = pSahara->getSaharaVersion();
      pHelloResp->m_versionCompatible = Sahara::SUPPORTED_LOWEST_VERSION;
      pHelloResp->m_status = Sahara::STATUS_SUCCESS;
      pHelloResp->m_mode = mode;
      pHelloResp->m_reserved[0] = 1;
      pHelloResp->m_reserved[1] = 2;
      pHelloResp->m_reserved[2] = 3;
      pHelloResp->m_reserved[3] = 4;
      pHelloResp->m_reserved[4] = 5;
      pHelloResp->m_reserved[5] = 6;
      m_pSaharaConnection->sendSync(pHelloResponseBuffer);

      pBuffer = saharaWaitForHandshakeReady(mode, MAX_SAHARA_LEFTOVER_PACKETS_COUNT);
   }

   // If still no incoming packet, force a sahara state machine reset
   if(pBuffer == nullptr)
   {
      // FLOG_WARNING(
      //    "Unframed packet received from device waiting for next command
      //    (image_tx_pending state) try reset state machine. " +
      //    pSahara->getDescription());
      pSendBuffer = pSahara->createCommand<Sahara::ResetStateMachine>(Sahara::SAHARA_RESET_SAHARA_STATE_MACHINE);
      m_pSaharaConnection->sendSync(pSendBuffer);

      // Get a hello from the device
      pBuffer = saharaWaitForHandshakeReady(mode, MAX_SAHARA_LEFTOVER_PACKETS_COUNT);
   }

   TOOLS_ASSERT_OR_THROW(
      pBuffer != nullptr,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "No packet received from device waiting for reset "
         "state machine (image_tx_pending state)" +
            pSahara->getDescription()
      )
   );

   return pBuffer;
}

// ----------------------------------------------------------------------------
// saharaWarmReset
//
/// Issue a sahara warm reset
// ----------------------------------------------------------------------------
void ImageTransfer::saharaWarmReset()
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Device::Protocol::Sahara>();

   switch(pSahara->getSaharaVersion())
   {
      // V3
      case 3:
         TOOLS_ASSERT_OR_THROW(
            pSahara->isSaharaV3WarmResetEnabled(),
            Device::Exception(
               Device::Exception::DEVICE_UNKNOWN_ERROR,
               "Beyond max resetable cycle, please hard reset "
               "the phone"
            )
         );
         break;
      default:
         // Do nothing
         return;
   }
   // Send the reset command
   Device::SharedByteBufferPtr pResetBuffer = pSahara->createCommand<Sahara::Reset>(Sahara::SAHARA_RESET);

   m_pSaharaConnection->sendSync(pResetBuffer);

   // Get a CMD_EXEC_RESP from the device
   Device::SharedByteBufferPtr pBuffer = pSahara->getFrame();
   TOOLS_ASSERT_OR_THROW(
      pBuffer != nullptr,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "No packet received from device get info command "
         "execute (command state)" +
            pSahara->getDescription()
      )
   );

   const Sahara::FrameHeader* pHeader =
      Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());
   if(Sahara::SAHARA_RESET_RESP == pHeader->m_command)
   {
      pSahara->setCommandModeEnabled(false);
      pSahara->doDisconnect();
   }
   else
   {
      // Will naver comes here
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "Command execute response not received "
         "from Sahara protocol: " +
            pSahara->getDescription()
      ));
   }
}

// ----------------------------------------------------------------------------
// getDeviceInfo
//
/// Get EDL device information
// ----------------------------------------------------------------------------
void ImageTransfer::getDeviceInfo(EdlDeviceInfo& deviceInfo)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Device::Protocol::Sahara>();
   Device::SharedByteBufferPtr pSendBuffer;
   Device::SharedByteBufferPtr pBuffer;
   const Sahara::FrameHeader* pHeader = nullptr;
   deviceInfo.serialNumber = 0;
   deviceInfo.msmHwId = 0;
   // Returned buffer is never NULL, otherwise a exception generated in
   // saharaInitializeHelloRequest
   if(!pSahara->isCommandModeEnabled())
   {
      pBuffer = saharaInitializeHelloRequest(Sahara::Mode::MODE_COMMAND);

      pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());

      if(Sahara::SAHARA_HELLO == pHeader->m_command)
      {
         const Sahara::Hello* pHello = Util::buffer_cast<const Sahara::Hello*>(pBuffer->begin(), pBuffer->size());

         TOOLS_ASSERT_OR_THROW(
            Sahara::SAHARA_HELLO == pHello->m_header.m_command &&
               TOOLS_SIZEOF(Sahara::Hello) == pHello->m_header.m_length &&
               Sahara::Mode::MODE_IMAGE_TX_PENDING == pHello->m_mode, // Only availible for EDL
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "Command mode not available in Sahara protocol: " + pSahara->getDescription()
            )
         );

         pSahara->setMode(static_cast<Sahara::Mode>(pHello->m_mode));
         pSahara->setSaharaVersion(pHello->m_versionNumber);
         pSendBuffer = pSahara->createCommand<Sahara::HelloResponse>(Sahara::SAHARA_HELLO_RESP);
         Sahara::HelloResponse* pHelloResp =
            Util::buffer_cast<Sahara::HelloResponse*>(pSendBuffer->begin(), pSendBuffer->size());
         // if not Sahara::MODE_COMMAND, could be Sahara::MODE_IMAGE_TX_PENDING
         // or Sahara::MODE_IMAGE_TX_COMPLETE Send a hello response
         pHelloResp->m_versionNumber = pHello->m_versionNumber;
         pHelloResp->m_versionCompatible = pHello->m_versionNumber;
         pHelloResp->m_status = Sahara::STATUS_SUCCESS;
         pHelloResp->m_mode = Sahara::MODE_COMMAND;
         pHelloResp->m_reserved[0] = 1;
         pHelloResp->m_reserved[1] = 2;
         pHelloResp->m_reserved[2] = 3;
         pHelloResp->m_reserved[3] = 4;
         pHelloResp->m_reserved[4] = 5;
         pHelloResp->m_reserved[5] = 6;
         m_pSaharaConnection->sendSync(pSendBuffer);

         // Get a COMMAND_READY from the device
         pBuffer = pSahara->getFrame();
         TOOLS_ASSERT_OR_THROW(
            pBuffer != nullptr,
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "No packet received from device get info "
               "(image_tx_pending state)" +
                  pSahara->getDescription()
            )
         );
         pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());
      }

      if(Sahara::SAHARA_COMMAND_READY != pHeader->m_command)
      {
         // Will naver comes here
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Command ready not received from Sahara "
            "protocol: " +
               pSahara->getDescription()
         ));
      }
   }

   Sahara::ClientCommandID command = Sahara::CMD_SN_READ;
   while(command <= Sahara::CMD_HWID_READ) // get serial number and msm HW id,
                                           // max 2 parameter
   {
      pSendBuffer = pSahara->createCommand<Sahara::CommandExecute>(Sahara::SAHARA_COMMAND_EXECUTE);
      Sahara::CommandExecute* pCmdExecute =
         Util::buffer_cast<Sahara::CommandExecute*>(pSendBuffer->begin(), pSendBuffer->size());
      pCmdExecute->m_clientCommand = command;
      m_pSaharaConnection->sendSync(pSendBuffer);
      // Get a CMD_EXEC_RESP from the device
      pBuffer = pSahara->getFrame();
      TOOLS_ASSERT_OR_THROW(
         pBuffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "No packet received from device get info command "
            "execute (command state)" +
               pSahara->getDescription()
         )
      );
      pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());
      if(Sahara::SAHARA_COMMAND_EXECUTE_RESP == pHeader->m_command)
      {
         pSahara->setCommandModeEnabled(true);
         const Sahara::CommandExecuteResp* pCmdExecuteResp =
            Util::buffer_cast<const Sahara::CommandExecuteResp*>(pBuffer->begin(), pBuffer->size());
         TOOLS_ASSERT_OR_THROW(
            pBuffer != nullptr,
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "No packet received from device get info command "
               "execute (command state)" +
                  pSahara->getDescription()
            )
         );
         TOOLS_ASSERT_OR_THROW(
            EDL_SERIAL_NUMBER_LENGTH <= pCmdExecuteResp->m_responseLength, // Sahara 3.0
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "Invalid response length from device get info "
               "command execute (command state)" +
                  pSahara->getDescription()
            )
         );
         pSendBuffer = pSahara->createCommand<Sahara::CommandExecuteData>(Sahara::SAHARA_COMMAND_EXECUTE_DATA);
         Sahara::CommandExecuteData* pCmdExecuteData =
            Util::buffer_cast<Sahara::CommandExecuteData*>(pSendBuffer->begin(), pSendBuffer->size());
         pCmdExecuteData->m_clientCommand = command;
         m_pSaharaConnection->sendSync(pSendBuffer);
         // Get a data from the device
         pBuffer = pSahara->getFrame();
         TOOLS_ASSERT_OR_THROW(
            pBuffer != nullptr,
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "No packet received from device get info command "
               "execute data (command state)" +
                  pSahara->getDescription()
            )
         );
         TOOLS_ASSERT_OR_THROW(
            pCmdExecuteResp->m_responseLength == pBuffer->size(),
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "Invalid response length from device get info "
               "command execute data (command state)" +
                  pSahara->getDescription()
            )
         );

         uint32_t result = 0;
         if(Sahara::CMD_SN_READ == command) // 01 Serial number
         {
            std::memcpy(&result, pBuffer->begin(), EDL_SERIAL_NUMBER_LENGTH);
            deviceInfo.serialNumber = result;
            if(2 * EDL_SERIAL_NUMBER_LENGTH <= pCmdExecuteResp->m_responseLength)
            {
               // break on Sahara 3.0
               if(3 < pSahara->getSaharaVersion())
               {
                  // In case no Hello request missing, update version number
                  // here
                  pSahara->setSaharaVersion(3);
               }
               std::memcpy(&result, pBuffer->begin() + EDL_SERIAL_NUMBER_LENGTH, EDL_SERIAL_NUMBER_LENGTH);
               deviceInfo.msmHwId = result;
               break;
            }
         }
         else
         {
            std::memcpy(&result, pBuffer->begin() + EDL_SERIAL_NUMBER_LENGTH, EDL_SERIAL_NUMBER_LENGTH);
            deviceInfo.msmHwId = result;
         }
      }
      else
      {
         // Will naver comes here
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Command execute response not received "
            "from Sahara protocol: " +
               pSahara->getDescription()
         ));
      }
      if(Sahara::CMD_SN_READ == command)
      {
         command = Sahara::CMD_HWID_READ;
      }
      else
      {
         command = Sahara::CMD_MAX;
      }
   }
   saharaWarmReset();
}

// ----------------------------------------------------------------------------
// transferFirehoseProgrammer
//
/// Transfer a firehose programmer image to device
// ----------------------------------------------------------------------------
void ImageTransfer::transferFirehoseProgrammer(const std::filesystem::path& imageFile, BuildImageList& buildImageList)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Device::Protocol::Sahara>();
   if(imageFile.empty() && buildImageList.end() == buildImageList.find(Sahara::IMAGE_ID_EDL_PROGRAMMER))
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Invalid device programmer, provide either a \"firehoseProgPath\" or "
         "a \"saharaImageList\" option"
      ));
   }

   if(!imageFile.empty() && buildImageList.end() != buildImageList.find(Sahara::IMAGE_ID_EDL_PROGRAMMER) &&
      !buildImageList.at(Sahara::IMAGE_ID_EDL_PROGRAMMER).empty())
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Conflict in choosing device programmer, provide either a "
         "\"firehoseProgPath\" or \"saharaImageList\" option"
      ));
   }

   if(buildImageList.end() == buildImageList.find(Sahara::IMAGE_ID_EDL_PROGRAMMER) ||
      buildImageList.end() != buildImageList.find(Sahara::IMAGE_ID_EDL_PROGRAMMER) &&
         buildImageList.at(Sahara::IMAGE_ID_EDL_PROGRAMMER).empty())
   {
      if(imageFile.empty())
      {
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Invalid device programmer, provide either a \"firehoseProgPath\" "
            "or a \"saharaImageList\" option"
         ));
      }
      FLOG_INFO(
         "Device programmer not available in sahara image list, "
         "override it from firehoseProgPath: " +
         imageFile.string()
      );
      buildImageList.insert({Sahara::IMAGE_ID_EDL_PROGRAMMER, imageFile});
   }

   FLOG_INFO("Partition images to be tranfered, size: " + std::string(std::to_string(buildImageList.size())));

   if(buildImageList.size() <= 0)
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         Device::Exception::getErrorJson(ERR_FIREHOSE_PROGRAMMER_NOT_FOUND, SUGG_FILE_NOT_FOUND, POC(TARGET))
      ));
   }

   transferImageList(buildImageList);
}

// ----------------------------------------------------------------------------
// transferImageList
//
/// Transfer a list of images to device
// ----------------------------------------------------------------------------
Sahara::Mode ImageTransfer::transferImageList(const BuildImageList& imageList)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Device::Protocol::Sahara>();
   Sahara::Mode mode = Sahara::Mode::MODE_UNKNOWN;
   bool bMoreImage = true;
   uint64_t previousImageId = 0;
   while(bMoreImage)
   {
      uint64_t imageId = 0;
      uint64_t headerSize = 0;

      if(Sahara::IMAGE_ID_MDMDDR == previousImageId || Sahara::IMAGE_ID_XBLCONFIG == previousImageId)
      {
         FLOG_INFO(
            +"Previous image id: " + std::string(std::to_string(previousImageId)) +
            ", Wait a longer time for DDR training"
         );
         mode = saharaWaitForNextImage(imageId, headerSize, bMoreImage, std::chrono::milliseconds(30000));
      }
      else
      {
         mode = saharaWaitForNextImage(imageId, headerSize, bMoreImage);
      }

      FLOG_INFO(
         "Sahara mode: " + std::string(std::to_string(mode)) + " image id: " + std::string(std::to_string(imageId)) +
         " header size: " + std::string(std::to_string(headerSize)) +
         " more image: " + std::string(std::to_string(bMoreImage)) + " in Sahara protocol: " + pSahara->getDescription()
      );
      previousImageId = imageId;
      if(Sahara::MODE_IMAGE_TX_PENDING == mode || Sahara::MODE_IMAGE_TX_COMPLETE == mode)
      {
         TOOLS_ASSERT_OR_THROW(
            imageList.end() != imageList.find(imageId),
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               Device::Exception::getErrorJson(
                  ERR_SAHARA_IMAGE_NOT_FOUND(std::string(std::to_string(imageId))),
                  SUGG_SAHARA_IMAGE_NOT_FOUND,
                  POC(TARGET)
               )
            )
         );

         saharaTransferSingleImage(imageList.find(imageId)->second, headerSize, bMoreImage);
         if(bMoreImage && imageList.size() == 1 && imageList.end() != imageList.find(Sahara::IMAGE_ID_EDL_PROGRAMMER))
         {
            m_pSaharaConnection->getPublisher()->send(
               Util::Message::Level::WARNING,
               "Transfer firehose programmer",
               "Invalid transfer status: PENDING",
               "Behavior persist for all the Bear family chipsets, continue "
               "with SW download"
            );
            break;
         }
      }
      else if(Sahara::MODE_COMMAND == mode)
      {
         // DDR store command mode - no longer supported
         FLOG_WARNING("Sahara command mode (DDR store) encountered but DDR data collection has been removed");
         continue;
      }
      else if(Sahara::MODE_MEMORY_DEBUG == mode)
      {
         break;
      }
      else
      {
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Sahara protocol mode unknown: " + pSahara->getDescription()
         ));
      }
   }

   return mode;
}

// ----------------------------------------------------------------------------
// saharaWaitForNextImage
//
/// Wair for a new image transfer request from device
// ----------------------------------------------------------------------------
Sahara::Mode ImageTransfer::saharaWaitForNextImage(
   uint64_t& imageId,
   uint64_t& headerSize,
   bool& bMore,
   const std::chrono::milliseconds& timeout
)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Sahara>();
   Device::SharedByteBufferPtr pSendBuffer;
   Device::SharedByteBufferPtr pBuffer;
   const Sahara::FrameHeader* pHeader = nullptr;

   if(pSahara->isCommandModeEnabled())
   {
      pSendBuffer = pSahara->createCommand<Sahara::CommandSwitchMode>(Sahara::SAHARA_COMMAND_SWITCH_MODE);
      Sahara::CommandSwitchMode* pCmdSwitchMode =
         Util::buffer_cast<Sahara::CommandSwitchMode*>(pSendBuffer->begin(), pSendBuffer->size());
      pCmdSwitchMode->m_mode = Sahara::Mode::MODE_IMAGE_TX_PENDING;
      pSahara->sendSync(pSendBuffer);
   }
   // Returned buffer is never NULL, otherwise a exception generated in
   // saharaInitializeHelloRequest
   pBuffer = saharaInitializeHelloRequest(Sahara::Mode::MODE_IMAGE_TX_PENDING, timeout);

   pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());

   if(Sahara::SAHARA_HELLO == pHeader->m_command)
   {
      const Sahara::Hello* pHello = Util::buffer_cast<const Sahara::Hello*>(pBuffer->begin(), pBuffer->size());

      TOOLS_ASSERT_OR_THROW(
         Sahara::SAHARA_HELLO == pHello->m_header.m_command &&
            TOOLS_SIZEOF(Sahara::Hello) == pHello->m_header.m_length &&
            (Sahara::Mode::MODE_IMAGE_TX_PENDING == pHello->m_mode ||
             Sahara::Mode::MODE_COMMAND == pHello->m_mode              // mode_command for DDR store case
             || Sahara::Mode::MODE_IMAGE_TX_COMPLETE == pHello->m_mode // image_tx_complete means one last image to
                                                                       // be transferred and dont expect any further
             || Sahara::Mode::MODE_MEMORY_DEBUG == pHello->m_mode),    // Crash recovery after image transfer in
                                                                       // flashless mode
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Image transfer mode not available in Sahara protocol: " + pSahara->getDescription()
         )
      );

      pSahara->setMode(static_cast<Sahara::Mode>(pHello->m_mode));
      pSahara->setSaharaVersion(pHello->m_versionNumber);
      // The are two image transfer scenatios that requires special handling
      // 1. target initiated command_mode in between image_tx_pending mode, so
      // it means DDR store request
      // 2. Target enters memory debug mode after image transfer. This happens
      // when a flashless target crashes. Do not send HelloResponse here, leave
      // it to subsequent Sahara function
      if(Sahara::Mode::MODE_COMMAND == pHello->m_mode || Sahara::Mode::MODE_MEMORY_DEBUG == pHello->m_mode)
      {
         return pSahara->getMode();
      }

      // if not Sahara::MODE_COMMAND, could be Sahara::MODE_IMAGE_TX_PENDING or
      // Sahara::MODE_IMAGE_TX_COMPLETE Send a hello response
      Device::SharedByteBufferPtr pHelloResponseBuffer =
         pSahara->createCommand<Sahara::HelloResponse>(Sahara::SAHARA_HELLO_RESP);
      Sahara::HelloResponse* pHelloResp =
         Util::buffer_cast<Sahara::HelloResponse*>(pHelloResponseBuffer->begin(), pHelloResponseBuffer->size());
      pHelloResp->m_versionNumber = pHello->m_versionNumber;
      pHelloResp->m_versionCompatible = pHello->m_versionNumber;
      pHelloResp->m_status = Sahara::STATUS_SUCCESS;
      if(Sahara::Mode::MODE_IMAGE_TX_PENDING == pHello->m_mode)
      {
         pHelloResp->m_mode = Sahara::MODE_IMAGE_TX_PENDING;
      }
      if(Sahara::Mode::MODE_IMAGE_TX_COMPLETE == pHello->m_mode)
      {
         pHelloResp->m_mode = Sahara::MODE_IMAGE_TX_COMPLETE;
         bMore = false;
      }
      pHelloResp->m_reserved[0] = 1;
      pHelloResp->m_reserved[1] = 2;
      pHelloResp->m_reserved[2] = 3;
      pHelloResp->m_reserved[3] = 4;
      pHelloResp->m_reserved[4] = 5;
      pHelloResp->m_reserved[5] = 6;
      m_pSaharaConnection->sendSync(pHelloResponseBuffer);

      // Get a READ_DATA from the device
      pBuffer = pSahara->getFrame();
      TOOLS_ASSERT_OR_THROW(
         pBuffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "No packet received from device waiting for next "
            "image (image_tx_pending state)" +
               pSahara->getDescription()
         )
      );
      pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());
   }
   else if(Sahara::CommandId::SAHARA_READ_DATA == pHeader->m_command &&
           TOOLS_SIZEOF(Sahara::ReadData) == pHeader->m_length)
   {
      // On hello stage get read_data command
      pSahara->setMode(Sahara::Mode::MODE_IMAGE_TX_PENDING);
      pSahara->setSaharaVersion(Sahara::SUPPORTED_LOWEST_VERSION);
   }
   else if(Sahara::CommandId::SAHARA_64_BIT_READ_DATA == pHeader->m_command &&
           TOOLS_SIZEOF(Sahara::ReadData64Bit) == pHeader->m_length)
   {
      // On hello stage get read_data_64 command
      pSahara->setMode(Sahara::Mode::MODE_IMAGE_TX_PENDING);
      pSahara->setSaharaVersion(Sahara::SUPPORTED_LOWEST_VERSION);
   }

   if(Sahara::CommandId::SAHARA_READ_DATA == pHeader->m_command && TOOLS_SIZEOF(Sahara::ReadData) == pHeader->m_length)
   {
      const Sahara::ReadData* pReadData = Util::buffer_cast<const Sahara::ReadData*>(pBuffer->begin(), pBuffer->size());
      TOOLS_ASSERT_OR_THROW(
         Sahara::SAHARA_READ_DATA == pReadData->m_header.m_command &&
            TOOLS_SIZEOF(Sahara::ReadData) == pReadData->m_header.m_length && 0 == pReadData->m_dataOffset,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Read data not available from Sahara protocol: " + pSahara->getDescription()
         )
      );
      imageId = pReadData->m_imageId;
      headerSize = pReadData->m_dataLength;
   }
   else if(Sahara::CommandId::SAHARA_64_BIT_READ_DATA == pHeader->m_command &&
           TOOLS_SIZEOF(Sahara::ReadData64Bit) == pHeader->m_length)
   {
      const Sahara::ReadData64Bit* pReadData64Bit =
         Util::buffer_cast<const Sahara::ReadData64Bit*>(pBuffer->begin(), pBuffer->size());
      TOOLS_ASSERT_OR_THROW(
         Sahara::SAHARA_64_BIT_READ_DATA == pReadData64Bit->m_header.m_command &&
            TOOLS_SIZEOF(Sahara::ReadData64Bit) == pReadData64Bit->m_header.m_length &&
            0 == pReadData64Bit->m_dataOffset,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "64 bit Read data not available "
            "from Sahara protocol: " +
               pSahara->getDescription()
         )
      );
      imageId = pReadData64Bit->m_imageId;
      headerSize = pReadData64Bit->m_dataLength;
   }
   else
   {
      // Will naver comes here
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "Read data not received from Sahara protocol: " + pSahara->getDescription()
      ));
   }

   return pSahara->getMode();
}

// ----------------------------------------------------------------------------
// saharaTransferSingleImage
//
/// Sends a single image to device in Sahara image transfer mode
// ----------------------------------------------------------------------------
void ImageTransfer::saharaTransferSingleImage(const std::filesystem::path& imageFile, uint64_t headerSize, bool& bMore)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Sahara>();

   notify(std::make_shared<ImageTransferEvent>(ImageTransferEvent::SAHARA_TRANSFER_IMAGE, "Sahara transfer image"));

   // Transfer image header
   saharaTransferImageRegion(imageFile, 0, headerSize);

   bool bImageEnded = false;
   while(!bImageEnded)
   {
      // Get a response from the device
      auto rxStartTime = std::chrono::system_clock::now();
      Device::SharedByteBufferPtr pBuffer = pSahara->getFrame();
      auto rxTimeSpan = std::chrono::system_clock::now() - rxStartTime;
      TOOLS_ASSERT_OR_THROW(
         pBuffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "No packet received from device waiting for data transfer command. "
            "wait time(" +
               std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(rxTimeSpan).count()) +
               "ms): " + pSahara->getDescription()
         )
      );
      const Sahara::FrameHeader* pHeader =
         Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());

      if(Sahara::CommandId::SAHARA_READ_DATA == pHeader->m_command &&
         TOOLS_SIZEOF(Sahara::ReadData) == pHeader->m_length)
      {
         const Sahara::ReadData* pReadData =
            Util::buffer_cast<const Sahara::ReadData*>(pBuffer->begin(), pBuffer->size());
         TOOLS_ASSERT_OR_THROW(
            Sahara::SAHARA_READ_DATA == pReadData->m_header.m_command &&
               TOOLS_SIZEOF(Sahara::ReadData) == pReadData->m_header.m_length,
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "Read data not available from Sahara protocol: " + pSahara->getDescription()
            )
         );

         // Transfer an image segment
         saharaTransferImageRegion(imageFile, pReadData->m_dataOffset, pReadData->m_dataLength);
      }
      else if(Sahara::CommandId::SAHARA_64_BIT_READ_DATA == pHeader->m_command &&
              TOOLS_SIZEOF(Sahara::ReadData64Bit) == pHeader->m_length)
      {
         const Sahara::ReadData64Bit* pReadData64Bit =
            Util::buffer_cast<const Sahara::ReadData64Bit*>(pBuffer->begin(), pBuffer->size());
         TOOLS_ASSERT_OR_THROW(
            Sahara::SAHARA_64_BIT_READ_DATA == pReadData64Bit->m_header.m_command &&
               TOOLS_SIZEOF(Sahara::ReadData64Bit) == pReadData64Bit->m_header.m_length,
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "64 bit Read data not available from Sahara "
               "protocol: " +
                  pSahara->getDescription()
            )
         );

         // Transfer an image segment
         saharaTransferImageRegion(imageFile, pReadData64Bit->m_dataOffset, pReadData64Bit->m_dataLength);
      }
      else if(Sahara::CommandId::SAHARA_END_OF_IMAGE_TRANS == pHeader->m_command &&
              TOOLS_SIZEOF(Sahara::EndOfImageTransfer) == pHeader->m_length)
      {
         const Sahara::EndOfImageTransfer* pEndOfImageTransfer =
            Util::buffer_cast<const Sahara::EndOfImageTransfer*>(pBuffer->begin(), pBuffer->size());
         if(Sahara::Status::STATUS_SUCCESS != pEndOfImageTransfer->m_status)
         {
            saharaReset();
            Device::Exception ex(Device::Exception::DEVICE_INVALID_PACKET);
            TOOLS_THROW(Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               Device::Exception::getErrorJson(
                  ERR_SAHARA_PROTOCOL_RESET(
                     std::string(imageFile.filename().string().c_str()),
                     ex.getErrorCodeString(Device::Exception::DEVICE_INVALID_PACKET)
                  ),
                  SUGG_SAHARA_PROTOCOL_RESET,
                  std::string(POC(TARGET)) + " or " + std::string(POC(BOOT))
               )
            ));
         }
         else
         {
            bImageEnded = true;
         }
      }
      else if(Sahara::CommandId::SAHARA_HELLO == pHeader->m_command && TOOLS_SIZEOF(Sahara::Hello) == pHeader->m_length)
      {
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Image " + std::string(imageFile.string().c_str()) + " rejected by device: " + pSahara->getDescription()
         ));
      }
      else
      {
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Read data not received from Sahara "
            "protocol: " +
               pSahara->getDescription()
         ));
      }
   }

   // Send done
   Device::SharedByteBufferPtr pDoneBuffer = pSahara->createCommand<Sahara::Done>(Sahara::SAHARA_DONE);
   m_pSaharaConnection->sendSync(pDoneBuffer);

   // Get done response from the device
   Device::SharedByteBufferPtr pDoneResponseBuffer = pSahara->getFrame();
   TOOLS_ASSERT_OR_THROW(
      pDoneResponseBuffer != nullptr,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "No packet received from device waiting "
         "for done response:" +
            pSahara->getDescription()
      )
   );
   const Sahara::DoneResponse* pDoneResponse =
      Util::buffer_cast<const Sahara::DoneResponse*>(pDoneResponseBuffer->begin(), pDoneResponseBuffer->size());
   TOOLS_ASSERT_OR_THROW(
      Sahara::CommandId::SAHARA_DONE_RESP == pDoneResponse->m_header.m_command &&
         TOOLS_SIZEOF(Sahara::DoneResponse) == pDoneResponse->m_header.m_length,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "Image transfer done response not "
         "received from Sahara protocol: " +
            pSahara->getDescription()
      )
   );

   switch(pDoneResponse->m_imageTransferStatus)
   {
      case Sahara::MODE_IMAGE_TX_COMPLETE:
         bMore = false;
         break;

      case Sahara::MODE_IMAGE_TX_PENDING:
         bMore = true;
         break;

      default:
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Invalid image transfer status from "
            "Sahara protocol: " +
               pSahara->getDescription()
         ));
   }
}

// ----------------------------------------------------------------------------
// saharaTransferImageRegion
//
/// Transfer a chunk in Sahara image transfer mode
// ----------------------------------------------------------------------------
void ImageTransfer::
   saharaTransferImageRegion(const std::filesystem::path& imageFilePath, uint64_t offset, uint64_t length)
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Sahara>();

   auto imageFile = std::make_shared<std::fstream>();
   std::filesystem::path localPath =

      Device::Manager::getInstance()
         ->getAccessiblePath(imageFilePath, std::filesystem::path(), false, Device::Protocol::Base::NO_CLIENT_ID, true);
   imageFile->exceptions(std::ios::failbit | std::ios::badbit);
   imageFile->open(localPath.string().c_str(), std::ios::in | std::ios::binary);

   imageFile->seekg(offset, std::ios::beg);

   uint64_t bytesWrite = 0;
   while(bytesWrite < length)
   {
      uint64_t bytesToWrite = std::min<uint64_t>(length - bytesWrite, MAX_SAHARA_WRITE);
      Device::SharedByteBufferPtr pBuffer = Device::Buffer::createBuffer(static_cast<size_t>(bytesToWrite));

      imageFile->read(reinterpret_cast<char*>(pBuffer->begin()), pBuffer->size());
      TOOLS_ASSERT_OR_THROW(
         pBuffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Image transfer fail to read file: " + std::string(localPath.string().c_str()) + " offset " +
               std::to_string(offset) + " length " + std::to_string(length) + pSahara->getDescription()
         )
      );
      m_pSaharaConnection->sendSync(pBuffer);
      bytesWrite += bytesToWrite;
   }

   imageFile->close();
}

// ----------------------------------------------------------------------------
// saharaReset
//
/// Resets the device out of Sahara mode
// ----------------------------------------------------------------------------
void ImageTransfer::saharaReset()
{
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Sahara>();

   // Send the reset command
   Device::SharedByteBufferPtr pResetBuffer = pSahara->createCommand<Sahara::Reset>(Sahara::SAHARA_RESET);

   m_pSaharaConnection->sendSync(pResetBuffer);

   notify(std::make_shared<
          ImageTransferEvent>(ImageTransferEvent::SAHARA_RESETTING, "Resetting the device out of Sahara"));

   // Get the reset response but do not expect it will arrive
   pSahara->getFrame(std::chrono::seconds(1));
}

// ----------------------------------------------------------------------------
// firehoseGetMemoryName
//
/// Returns memory name string
// ----------------------------------------------------------------------------
std::string ImageTransfer::firehoseGetMemoryName()
{
   std::string memoryName;

   switch(m_memoryType)
   {
      case MemoryType::MEMORY_TYPE_EMMC:
         memoryName = "emmc";
         break;

      case MemoryType::MEMORY_TYPE_UFS:
         memoryName = "ufs";
         break;

      case MemoryType::MEMORY_TYPE_NAND:
         memoryName = "nand";
         break;

      case MemoryType::MEMORY_TYPE_NVME:
         memoryName = "nvme";
         break;

      case MemoryType::MEMORY_TYPE_SPINOR:
         memoryName = "spinor";
         break;

      default:
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Invalid device memory type: " + m_memoryType
         ));
   }

   return memoryName;
}

// ----------------------------------------------------------------------------
// firehoseSetMemoryType
//
/// Configures the storage memory type
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetMemoryType(const MemoryType type)
{
   m_memoryType = type;
}

// ----------------------------------------------------------------------------
// firehoseSetSlot
//
/// Configures the slot
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetSlot(const int32_t slot)
{
   m_slot = slot;
}

// ----------------------------------------------------------------------------
// firehoseSetRawXmlList
//
/// Configures the rawprogram xml path list for sendXml
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetRawXmlList(const std::vector<std::filesystem::path>& xmlFiles)
{
   m_rawXmlList = xmlFiles;
}
void ImageTransfer::firehoseSetPatchXmlList(const std::vector<std::filesystem::path>& xmlFiles)
{
   m_patchXmlList = xmlFiles;
}

// ----------------------------------------------------------------------------
// firehoseSetFirmwareWritePath
//
/// Configures the firmware path string for sendimage
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetFirmwarePath(const std::filesystem::path& firmwareFile)
{
   m_firmwareFile = firmwareFile;
}

// ----------------------------------------------------------------------------
// firehoseSetProvisionPath
//
/// Configures the provision xml path string for sendXml
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetProvisionPath(const std::filesystem::path& provisionFile)
{
   m_provisionFile = provisionFile; ///< provision Xml File
}

// ----------------------------------------------------------------------------
// firehoseSetJsonPath
//
/// Configures the Json path which has the xml and image paths
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetJsonPath(const std::filesystem::path& jsonFile)
{
   m_jsonFile = jsonFile;
}

// ----------------------------------------------------------------------------
// firehoseSetSingleImagePath
//
/// Configures the single image path
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetSingleImagePath(const std::filesystem::path& imageFile)
{
   m_singleImage = imageFile;
}

//----------------------------------------------------------------------------
// firehoseSetLun
//
/// Configures the LUN for the single image
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetLun(const int32_t lun)
{
   m_lun = lun;
}

//----------------------------------------------------------------------------
// firehoseSetStartSector
//
/// Configures the start sector for the single image
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetStartSector(const int32_t startSector)
{
   m_startSector = startSector;
}

// ----------------------------------------------------------------------------
// firehoseSetSignedDigestsFile
//
/// Configures signed digests used by VIP download
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetSignedDigestsFile(const std::filesystem::path& signedDigestsFile)
{
   m_signedDigestsFile = signedDigestsFile;
}

// ----------------------------------------------------------------------------
// firehoseSetChainedDigestsFile
//
/// Configures chained digests used by VIP download
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetChainedDigestsFile(const std::filesystem::path& chainedDigestsFile)
{
   m_chainedDigestsFile = chainedDigestsFile;
}

// ----------------------------------------------------------------------------
// firehoseSetPartitionIndexList
//
/// Configures partition index list
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetPartitionIndexList(const std::vector<int32_t>& partitionIndexList)
{
   m_partitionIndexList = partitionIndexList;
}

// ----------------------------------------------------------------------------
// firehoseSetPartitionNumber
//
/// Configures partition index size
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetPartitionNumber(const int32_t numPartition)
{
   m_numPartition = numPartition;
}

// ----------------------------------------------------------------------------
// firehoseSetEnableResetAfterDownload
//
/// Configures reset after download
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetEnableResetAfterDownload(const bool bEnableResetAfterDownload)
{
   m_bResetAfterDownload = bEnableResetAfterDownload;
}

// ----------------------------------------------------------------------------
// firehoseSetEnableEraseBeforeDownload
//
/// Configures erase before download
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetEnableEraseBeforeDownload(const bool bEnableEraseBeforeDownload)
{
   m_bEraseBeforeDownload = bEnableEraseBeforeDownload;
}

// ----------------------------------------------------------------------------
// firehoseSetActivePartition
//
/// Configures active partition
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetActivePartition(const int32_t activePartition)
{
   m_activePartition = activePartition;
}

// ----------------------------------------------------------------------------
// firehoseSetDownloadRxTimeoutInMs
//
/// Set download RX timeout
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetDownloadRxTimeoutInMs(const std::chrono::milliseconds& downloadRxTimeoutInMs)
{
   m_downloadRxTimeoutInMs = downloadRxTimeoutInMs;
}

// ----------------------------------------------------------------------------
// firehoseSetZlpAwareHost
//
/// Set ZLP aware host option
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetZlpAwareHost(ZlpAwareHostOption zlpAwareHost)
{
   m_zlpAwareHost = zlpAwareHost;
}

// ----------------------------------------------------------------------------
// firehoseSetOutpuDir
//
/// Set output dir
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetOutpuDir(const std::filesystem::path& outputDir)
{
   m_outputDir = outputDir;
}

// ----------------------------------------------------------------------------
// firehoseSetReadImages
//
/// Enable read images
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetReadImages(bool bEnableReadImages)
{
   m_bReadImages = bEnableReadImages;
}

// ----------------------------------------------------------------------------
// firehoseSetValidateImageSize
//
/// Set Validate image size
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetValidateImageSize(bool bValidateImageSize)
{
   m_bValidateImageSize = bValidateImageSize;
}

// ----------------------------------------------------------------------------
// firehoseSetMaxPayloadSize
//
/// Set max payload size
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetMaxPayloadSize(const int32_t maxPayloadSizeInByte)
{
   m_maxPayloadSize = maxPayloadSizeInByte;
}

// ----------------------------------------------------------------------------
// firehoseSetMaxReadPayloadSize
//
/// Set max read payload size
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetMaxReadPayloadSize(const int32_t maxReadPayloadSizeInByte)
{
   m_maxReadPayloadSize = maxReadPayloadSizeInByte;
}

// ----------------------------------------------------------------------------
// firehoseSetExcludeErasePartitionInfo
//
/// Set exclude partition information
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetExcludeErasePartitionInfo(const std::string& excludeErasePartitionInfo)
{
   m_excludeErasePartitionInfo = excludeErasePartitionInfo;
}


// ----------------------------------------------------------------------------
// firehoseSetValidationMode
//
/// Set build validation mode
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetValidationMode(const ValidationMode mode)
{
   m_validationMode = mode;
}

// ----------------------------------------------------------------------------
// firehoseSetEdmaPath
//
/// Set edma path
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetEdmaPath(const std::filesystem::path& edmaFile)
{
   m_edmaPath = edmaFile;
}

// ----------------------------------------------------------------------------
// firehoseSetDigestHeaderType
//
/// Set firehose Digest Header Type
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetDigestHeaderType(const DigestHeaderType digestHeaderType)
{
   switch(digestHeaderType)
   {
      case DIGEST_HEADER_TYPE_NONE:
         m_digestHeaderType = "none";
         break;
      case DIGEST_HEADER_TYPE_MBN:
         m_digestHeaderType = "mbn";
         break;
      case DIGEST_HEADER_TYPE_ELF:
         m_digestHeaderType = "elf";
         break;
      default:
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Invalid Digest Header Type"));
   }
}

// ----------------------------------------------------------------------------
// firehoseSetDigestHeaderType
//
/// Set firehose NAND page size
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetPageSize(const uint32_t pageSize)
{
   m_pageSize = pageSize;
}

// ----------------------------------------------------------------------------
// firehoseSetBuildValidationDigests
//
/// Set build validation digest file path
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetBuildValidationDigests(const std::filesystem::path& digestsFile)
{
   m_validationDigestsFile = digestsFile;
}

// ----------------------------------------------------------------------------
// firehoseSetDigestHeaderType
//
/// Set firehose NAND page per block
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSetPagePerBlock(const uint32_t pagePerBlock)
{
   m_pagePerBlock = pagePerBlock;
}

// ----------------------------------------------------------------------------
// firehoseErase
//
/// Erases chip
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseErase(void)
{
   notify(std::make_shared<ImageTransferEvent>(ImageTransferEvent::FIREHOSE_ERASE, "Erase Partitions"));

   FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Firehose>();
   Firehose::FirehoseCommandType firehoseCommand;

   firehoseCommand.push_back(std::string("Firehose"));
   firehoseCommand.push_back(std::string("--port=\\\\.\\COM"));
   if(0 != m_slot)
   {
      firehoseCommand.push_back(std::string("--slot=") + std::to_string(m_slot));
   }
   std::vector<int32_t>::iterator it = m_partitionIndexList.begin();
   std::vector<int32_t>::iterator end = m_partitionIndexList.end();
   for(; it != end; ++it)
   {
      if(m_numPartition == 0 || m_numPartition > *it)
      {
         firehoseCommand.push_back(std::string("--erase=") + std::to_string(*it));
      }
      else
      {
         notify(std::make_shared<ImageTransferEvent>(
            ImageTransferEvent::FIREHOSE_ERASE,
            "Partition Number out of bound and Ignored:" + std::to_string(*it)
         ));
      }
   }
   firehoseCommand.push_back(std::string("--noprompt"));
   firehoseCommand.push_back(std::string("--showpercentagecomplete"));
   firehoseCommand.push_back(std::string("--memoryname=" + firehoseGetMemoryName()));
   firehoseCommand.push_back(std::string("--noprompt"));

   notify(std::make_shared<ImageTransferEvent>(ImageTransferEvent::FIREHOSE_ERASE, "Partitions Erased"));

   pFirehose->processCommand(firehoseCommand, m_outputDir);
}

// ----------------------------------------------------------------------------
// getSortedXmlFileString
//
/// Create a comma separated XML file string
// ----------------------------------------------------------------------------
std::string ImageTransfer::getSortedXmlFileString(const std::vector<std::string>& xmlPaths)
{
   std::string finalXmlString = "";
   for(size_t i = 0; i < xmlPaths.size(); ++i)
   {
      if(i != 0)
      {
         finalXmlString += ",";
      }
      finalXmlString += xmlPaths.at(i);
   }
   return finalXmlString;
}

// ----------------------------------------------------------------------------
// formatDownloadBuildParameter
//
/// Download a meta build or flat build
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseFormatDownloadBuildParameter(const std::filesystem::path& buildPathDir, bool bGetFlashInfo)
{
   if(!m_chainedDigestsFile.empty())
   {
      TOOLS_ASSERT_OR_THROW(
         (!m_signedDigestsFile.empty()),
         Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Missing signed digests option")
      );
   }

   // download only
   if(m_firmwareFile.empty() && m_provisionFile.empty() && m_edmaPath.empty())
   {
      // Single Image
      bool bIsSingleImageSet = false;
      // Json Download
      bool bIsJsonPathSet = false;
      bool bIsFlatBuildSet = false;

      if(!m_singleImage.empty())
      {
         // Download single image
         bIsSingleImageSet = true;
      }
      else if(!m_jsonFile.empty())
      {
         // Use Json path for XMLs and images if given.
         // Meta Build Download
         bIsJsonPathSet = true;
      }
      else
      {
         // Search file
         if(m_rawXmlList.size() == 0 && (m_bReadImages || m_patchXmlList.size() == 0))
         {
            std::filesystem::directory_iterator end;
            const std::string FIREHOSE_RAW_PROGRAMMER_FILTER = "*rawprogram*.xml";
            const std::string FIREHOSE_PATCH_PROGRAMMER_FILTER = "*patch*.xml";

            // If no files set, search default from build path
            for(const auto& entry: std::filesystem::directory_iterator(buildPathDir))
            {
               if(entry.path().extension() == FIREHOSE_RAW_PROGRAMMER_FILTER)
               {
                  m_patchXmlList.push_back(entry.path());
               }
            }
            if(!m_bReadImages)
            {
               for(const auto& entry: std::filesystem::directory_iterator(buildPathDir))
               {
                  if(entry.path().extension() == FIREHOSE_PATCH_PROGRAMMER_FILTER)
                  {
                     m_patchXmlList.push_back(entry.path());
                  }
               }
            }
            // Sorting by file name before download
            std::sort(m_rawXmlList.begin(), m_rawXmlList.end());
            std::sort(m_patchXmlList.begin(), m_patchXmlList.end());
         }

         // Flat build Download
         std::string rawXmlStr = "";
         std::string patchXmlStr = "";
         std::vector<std::string> rawXmlList;
         std::vector<std::string> patchXmlList;
         std::vector<std::string> searchPaths = {};
         m_searchPathString.clear();
         // Format it
         if(m_rawXmlList.size() > 0)
         {
            // Check if the file exists
            std::vector<std::filesystem::path>::iterator it = m_rawXmlList.begin();
            std::vector<std::filesystem::path>::iterator end = m_rawXmlList.end();
            for(; it != end; ++it)
            {
               if(it->parent_path().empty())
               {
                  *it = buildPathDir / it->filename();
               }

               Device::Manager::getInstance()
                  ->getAccessiblePath(*it, std::filesystem::path(), false, Device::Protocol::Base::NO_CLIENT_ID, true);

               rawXmlList.push_back(it->filename().string().c_str());
               if(searchPaths.end() ==
                  std::find(searchPaths.begin(), searchPaths.end(), it->parent_path().string().c_str()))
               {
                  searchPaths.push_back(it->parent_path().string().c_str());
               }
            }
            rawXmlStr = getSortedXmlFileString(rawXmlList);
            bIsFlatBuildSet = true;
         }
         if(!m_bReadImages && m_patchXmlList.size() > 0)
         {
            // Check if the file exists
            std::vector<std::filesystem::path>::iterator it = m_patchXmlList.begin();
            std::vector<std::filesystem::path>::iterator end = m_patchXmlList.end();
            for(; it != end; ++it)
            {
               if(it->parent_path().empty())
               {
                  *it = buildPathDir / it->filename();
               }

               Device::Manager::getInstance()
                  ->getAccessiblePath(*it, std::filesystem::path(), false, Device::Protocol::Base::NO_CLIENT_ID, true);

               patchXmlList.push_back(it->filename().string().c_str());
               if(searchPaths.end() ==
                  std::find(searchPaths.begin(), searchPaths.end(), it->parent_path().string().c_str()))
               {
                  searchPaths.push_back(it->parent_path().string().c_str());
               }
            }
            patchXmlStr = getSortedXmlFileString(patchXmlList);
            bIsFlatBuildSet = true;
         }

         m_xmlPathString = rawXmlStr + ((rawXmlStr.empty() || patchXmlStr.empty()) ? "" : ",") + patchXmlStr;
         // Remove build path from searchPaths, we already adding above
         auto buildIt = find(searchPaths.begin(), searchPaths.end(), buildPathDir.string().c_str());
         if(searchPaths.end() != buildIt)
         {
            searchPaths.erase(buildIt);
         }
         m_searchPathString = getSortedXmlFileString(searchPaths);
      }

      TOOLS_ASSERT_OR_THROW(
         (bIsSingleImageSet || bIsJsonPathSet || bIsFlatBuildSet),
         Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "No Firehose program file available")
      );

      if(m_partitionIndexList.size() > 0)
      {
         std::vector<int32_t>::const_iterator it = m_partitionIndexList.begin();
         std::vector<int32_t>::const_iterator end = m_partitionIndexList.end();
         for(; it != end; ++it)
         {
            TOOLS_ASSERT_OR_THROW(
               (*it >= 0),
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Invalid Partition Number:" + std::to_string(*it)
               )
            );
         }
      }
      else
      {
         m_partitionIndexList.push_back(0);
         if(MemoryType::MEMORY_TYPE_UFS == m_memoryType)
         {
            m_partitionIndexList.push_back(1);
            m_partitionIndexList.push_back(2);
            // CDT is skipped
            m_partitionIndexList.push_back(4);
            m_partitionIndexList.push_back(5);
         }
      }

      // Some processes like VIP download & offline process are not going to get
      // partition number in runtime, in those situation, perdict it here
      if(!bGetFlashInfo)
      {
         m_numPartition = 0;
         std::vector<int32_t>::iterator it = m_partitionIndexList.begin();
         std::vector<int32_t>::iterator end = m_partitionIndexList.end();
         for(; it != end; ++it)
         {
            if(m_numPartition <= *it)
            {
               m_numPartition = *it + 1; // convert partitions from index to size
            }
         }
      }

      if(VALIDATION_MODE_BINARY_READBACK_WITH_DIGESTS_FILE == m_validationMode ||
         VALIDATION_MODE_SHA256_READBACK_WITH_DIGESTS_FILE == m_validationMode)
      {
         TOOLS_ASSERT_OR_THROW(
            (!m_validationDigestsFile.empty()),
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "No build validation file available")
         );
      }
   }
}

// ----------------------------------------------------------------------------
// prepareVipDownloadCmd
//
/// Prepare VIP download cmd to be sent to firehose
// ----------------------------------------------------------------------------
void ImageTransfer::prepareDownloadCmd(
   const std::filesystem::path& buildPathDir,
   Device::Protocol::Firehose::FirehoseCommandType& firehoseCommand,
   bool bIsOffline
)
{
   (void)bIsOffline; // Suppress unused parameter warning
   firehoseCommand.push_back(std::string("Firehose"));
   firehoseCommand.push_back(std::string("--port=\\\\.\\COM"));

   if(0 != m_slot)
   {
      firehoseCommand.push_back(std::string("--slot=") + std::to_string(m_slot));
   }
   if(!m_signedDigestsFile.empty())
   {
      firehoseCommand.push_back(std::string("--signeddigests=") + m_signedDigestsFile.string().c_str());
   }
   if(!m_chainedDigestsFile.empty())
   {
      firehoseCommand.push_back(std::string("--chaineddigests=") + m_chainedDigestsFile.string().c_str());
   }

   if(!m_firmwareFile.empty())
   {
      firehoseCommand.push_back(std::string("--firmwarewrite"));
      firehoseCommand.push_back(std::string("--sendimage=") + m_firmwareFile.string().c_str());
      firehoseCommand.push_back(std::string("--skipstorageinit"));
   }
   else if(!m_provisionFile.empty())
   {
      firehoseCommand.push_back(std::string("--sendxml=") + m_provisionFile.string().c_str());
      firehoseCommand.push_back(std::string("--skipstorageinit"));
   }
   else if(!m_edmaPath.empty())
   {
      firehoseCommand.push_back(std::string("--sendxml=") + m_edmaPath.string().c_str());
   }
   else
   {
      if(m_bEraseBeforeDownload)
      {
         std::vector<int32_t>::iterator it = m_partitionIndexList.begin();
         std::vector<int32_t>::iterator end = m_partitionIndexList.end();
         for(; it != end; ++it)
         {
            if(m_numPartition == 0 || m_numPartition > *it)
            {
               firehoseCommand.push_back(std::string("--erase=") + std::to_string(*it));
            }
            else
            {
               FLOG_WARNING("Partition Number out of bound and Ignored:" + std::to_string(*it));
            }
         }
      }

      // Append all search paths for FirehoseLoader
      firehoseCommand.push_back(
         std::string("--search_path=") +
         ((m_searchPathString.empty() ? "" : m_searchPathString + ",") + std::string(buildPathDir.string().c_str()))
      );
      if(m_bReadImages)
      {
         firehoseCommand.push_back(std::string("--convertprogram2read"));
      }
      if(!m_singleImage.empty())
      {
         firehoseCommand.push_back(std::string("--sendimage=") + m_singleImage.string());
         if(INVALID_LUN < m_lun)
         {
            firehoseCommand.push_back(std::string("--lun=") + std::to_string(m_lun));
         }
         if(INVALID_START_SECTOR < m_startSector)
         {
            firehoseCommand.push_back(std::string("--start_sector=") + std::to_string(m_startSector));
         }
         firehoseCommand.push_back(std::string("--showpercentagecomplete"));
      }
      else if(!m_jsonFile.empty())
      {
         firehoseCommand.push_back(std::string("--json_in=") + m_jsonFile.string());
         firehoseCommand.push_back(std::string("--showpercentagecomplete"));
      }
      else if(!m_xmlPathString.empty())
      {
         firehoseCommand.push_back(std::string("--sendxml=") + m_xmlPathString.c_str());
         firehoseCommand.push_back(std::string("--showpercentagecomplete"));
      }

      if(ACTIVE_PARTITION_NO_SET != m_activePartition)
      {
         if(m_numPartition != 0)
         {
            if(m_activePartition >= m_numPartition)
            {
               FLOG_WARNING(
                  "ImageTransfer::prepareDownloadCmd the number of "
                  "partition is: " +
                  std::to_string(m_numPartition) +
                  "and the activePartition has been ignored: " + std::to_string(m_activePartition)
               );
               m_activePartition = ACTIVE_PARTITION_NO_SET;
            }
         }
      }

      if(ACTIVE_PARTITION_NO_SET == m_activePartition)
      {
         if(MemoryType::MEMORY_TYPE_UFS == m_memoryType)
         {
            if(m_numPartition != 0)
            {
               if(1 >= m_numPartition)
               {
                  m_activePartition = 0;
                  FLOG_WARNING(
                     "ImageTransfer::prepareDownloadCmd under "
                     "MEMORY_TYPE_UFS the number of partition is: " +
                     std::to_string(m_numPartition) +
                     "and the activePartition has been set to: " + std::to_string(m_activePartition)
                  );
               }
               else
               {
                  m_activePartition = DEFAULT_UFS_BOOTABLE_PARTITION;
                  FLOG_INFO(
                     "ImageTransfer::prepareDownloadCmd under "
                     "MEMORY_TYPE_UFS "
                     "the activePartition has been set to: " +
                     std::to_string(m_activePartition)
                  );
               }
            }
         }
         else if(MemoryType::MEMORY_TYPE_EMMC == m_memoryType)
         {
            m_activePartition = 0;
            FLOG_INFO(
               "ImageTransfer::prepareDownloadCmd under "
               "MEMORY_TYPE_EMMC the activePartition has been set to: " +
               std::to_string(m_activePartition)
            );
         }
      }
      firehoseCommand.push_back(std::string("--setactivepartition=") + std::to_string(m_activePartition));

      if(0 != m_maxPayloadSize)
      {
         firehoseCommand.push_back(std::string("--maxpayloadsizeinbytes=") + std::to_string(m_maxPayloadSize).c_str());
      }

      if(0 != m_maxReadPayloadSize)
      {
         firehoseCommand
            .push_back(std::string("--maxreadpayloadsizeinbytes=") + std::to_string(m_maxReadPayloadSize).c_str());
      }

      appendZlpAwareHostOption(firehoseCommand);

      if(!m_excludeErasePartitionInfo.empty())
      {
         firehoseCommand.push_back(std::string("--excludeerasepartitioninfo=") + m_excludeErasePartitionInfo);
      }

      if(VALIDATION_MODE_BINARY_READBACK == m_validationMode)
      {
         firehoseCommand.push_back(std::string("--verify_programming"));
      }
      else if(VALIDATION_MODE_SHA256_READBACK == m_validationMode)
      {
         firehoseCommand.push_back(std::string("--verify_programming_getsha"));
      }
      else if(VALIDATION_MODE_BINARY_READBACK_WITH_DIGESTS_FILE == m_validationMode)
      {
         firehoseCommand.push_back(std::string("--verify_programming"));
         firehoseCommand.push_back(std::string("--verifysha256file"));
         firehoseCommand.push_back(std::string("--digestsperfilename=") + m_validationDigestsFile.string().c_str());
      }
      else if(VALIDATION_MODE_SHA256_READBACK_WITH_DIGESTS_FILE == m_validationMode)
      {
         firehoseCommand.push_back(std::string("--verify_programming_getsha"));
         firehoseCommand.push_back(std::string("--verifysha256file"));
         firehoseCommand.push_back(std::string("--digestsperfilename=") + m_validationDigestsFile.string().c_str());
      }

      if(m_bValidateImageSize)
      {
         firehoseCommand.push_back(std::string("--validate_image_size"));
      }
   }
   if(m_bResetAfterDownload)
   {
      firehoseCommand.push_back(std::string("--reset"));
   }
   firehoseCommand.push_back(std::string("--memoryname=" + firehoseGetMemoryName()));
   firehoseCommand.push_back(std::string("--noprompt"));
}

/// <summary>
///   Get device memory type from the device
/// </summary>
/// <returns>ImageTransfer::MemoryType</returns>
ImageTransfer::MemoryType ImageTransfer::getDeviceMemoryType()
{
   ImageTransfer::MemoryType memType = ImageTransfer::MemoryType::MEMORY_TYPE_UNKNOWN;
   SaharaPtr pSahara = (m_pSaharaConnection->getProtocol()).dynamicCast<Sahara>();
   Device::SharedByteBufferPtr pSendBuffer;
   Device::SharedByteBufferPtr pBuffer;
   const Sahara::FrameHeader* pHeader = nullptr;
   std::ostringstream socHw;
   FLOG_INFO(
      "Retrieving memory type from the device, Is command mode "
      "enabled: " +
      std::to_string(pSahara->isCommandModeEnabled())
   );

   if(!pSahara->isCommandModeEnabled())
   {
      pSendBuffer = pSahara->createCommand<Sahara::ResetStateMachine>(Sahara::SAHARA_RESET_SAHARA_STATE_MACHINE);
      m_pSaharaConnection->sendSync(pSendBuffer);

      pBuffer = saharaInitializeHelloRequest(Sahara::Mode::MODE_COMMAND);

      pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());

      if(Sahara::SAHARA_HELLO == pHeader->m_command)
      {
         const Sahara::Hello* pHello = Util::buffer_cast<const Sahara::Hello*>(pBuffer->begin(), pBuffer->size());

         TOOLS_ASSERT_OR_THROW(
            Sahara::SAHARA_HELLO == pHello->m_header.m_command &&
               TOOLS_SIZEOF(Sahara::Hello) == pHello->m_header.m_length &&
               Sahara::Mode::MODE_IMAGE_TX_PENDING == pHello->m_mode, // Only availible for EDL
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PACKET,
               "Command mode not available in Sahara protocol: " + pSahara->getDescription()
            )
         );

         pSahara->setMode(static_cast<Sahara::Mode>(pHello->m_mode));
         pSahara->setSaharaVersion(pHello->m_versionNumber);
         pSendBuffer = pSahara->createCommand<Sahara::HelloResponse>(Sahara::SAHARA_HELLO_RESP);
         Sahara::HelloResponse* pHelloResp =
            Util::buffer_cast<Sahara::HelloResponse*>(pSendBuffer->begin(), pSendBuffer->size());
         // if not Sahara::MODE_COMMAND, could be Sahara::MODE_IMAGE_TX_PENDING
         // or Sahara::MODE_IMAGE_TX_COMPLETE Send a hello response
         pHelloResp->m_versionNumber = pHello->m_versionNumber;
         pHelloResp->m_versionCompatible = pHello->m_versionNumber;
         pHelloResp->m_status = Sahara::STATUS_SUCCESS;
         pHelloResp->m_mode = Sahara::MODE_COMMAND;
         pHelloResp->m_reserved[0] = 1;
         pHelloResp->m_reserved[1] = 2;
         pHelloResp->m_reserved[2] = 3;
         pHelloResp->m_reserved[3] = 4;
         pHelloResp->m_reserved[4] = 5;
         pHelloResp->m_reserved[5] = 6;
         m_pSaharaConnection->sendSync(pSendBuffer);

         size_t maxAttemps = 0;
         do
         {
            // Discard all Hello responses which are out of sequence.
            FLOG_INFO(
               "Waiting for SAHARA_COMMAND_READY state from the device, "
               "Attempt: " +
               std::to_string(maxAttemps)
            );

            pBuffer = pSahara->getFrame();
            TOOLS_ASSERT_OR_THROW(
               pBuffer != nullptr,
               Device::Exception(
                  Device::Exception::DEVICE_RESPONSE_ERROR,
                  "No response received from the device, found "
                  "in invalid state, " +
                     pSahara->getDescription()
               )
            );

            pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());
            maxAttemps++;
         } while(Sahara::SAHARA_COMMAND_READY != pHeader->m_command && maxAttemps <= SAHARA_COMMAND_READY_WAIT_ATTEMPTS
         );
      }

      if(Sahara::SAHARA_COMMAND_READY != pHeader->m_command)
      {
         TOOLS_THROW(Device::Exception(
            Device::Exception::DEVICE_RESPONSE_ERROR,
            "Device found in invalid state, not "
            "ready to receive commands, " +
               pSahara->getDescription()
         ));
      }
   }

   FLOG_INFO("Sending sahara command: CMD_GET_FUSE_INFO");
   pSendBuffer = pSahara->createCommand<Sahara::CommandExecute>(Sahara::SAHARA_COMMAND_EXECUTE);
   Sahara::CommandExecute* pCmdExecute =
      Util::buffer_cast<Sahara::CommandExecute*>(pSendBuffer->begin(), pSendBuffer->size());
   pCmdExecute->m_clientCommand = Sahara::CMD_GET_FUSE_INFO;
   m_pSaharaConnection->sendSync(pSendBuffer);

   // Get a CMD_EXEC_RESP from the device
   pBuffer = pSahara->getFrame();
   TOOLS_ASSERT_OR_THROW(
      pBuffer != nullptr,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "No packet received from device sahara debug command "
         "execute (command state)" +
            pSahara->getDescription()
      )
   );
   pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());
   if(Sahara::SAHARA_COMMAND_EXECUTE_RESP == pHeader->m_command)
   {
      pSahara->setCommandModeEnabled(true);
      const Sahara::CommandExecuteResp* pCmdExecuteResp =
         Util::buffer_cast<const Sahara::CommandExecuteResp*>(pBuffer->begin(), pBuffer->size());
      TOOLS_ASSERT_OR_THROW(
         pBuffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "No packet received from device sahara debug "
            "command execute (command state)" +
               pSahara->getDescription()
         )
      );

      size_t commandDataLength = 0;
      pSendBuffer = pSahara->createCommand<Sahara::CommandExecuteData>(Sahara::SAHARA_COMMAND_EXECUTE_DATA);
      Sahara::CommandExecuteData* pCmdExecuteData =
         Util::buffer_cast<Sahara::CommandExecuteData*>(pSendBuffer->begin(), pSendBuffer->size());
      pCmdExecuteData->m_clientCommand = Sahara::CMD_GET_FUSE_INFO;
      m_pSaharaConnection->sendSync(pSendBuffer);

      Device::SharedByteBufferPtr pSaharaCmd10Buffer = std::make_shared<Device::SharedByteBuffer>();
      pSaharaCmd10Buffer->resize(pCmdExecuteResp->m_responseLength);
      while(commandDataLength < pCmdExecuteResp->m_responseLength)
      {
         Device::SharedByteBufferPtr pPartial = pSahara->getFrame();
         if(pPartial == nullptr)
         {
            TOOLS_THROW(Device::Exception(
               Device::Exception::DEVICE_RESPONSE_ERROR,
               "Error dumping file from " + pSahara->getDescription()
            ));
         };

         std::memcpy(pSaharaCmd10Buffer->begin() + commandDataLength, pPartial->begin(), pPartial->size());
         commandDataLength += pPartial->size();
      }

      TOOLS_ASSERT_OR_THROW(
         pSaharaCmd10Buffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "No data received from device for sahara command: "
            "Get Fuse Information, 0x0A"
         )
      );

      TOOLS_ASSERT_OR_THROW(
         SOC_HW_VERSION_MIN_SIZE < pSaharaCmd10Buffer->size(),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "SOC_HW_VERSION not supported by the device, failed "
            "to retrive "
            "storage or memory type from the device"
         )
      );

      int32_t result = 0;
      std::memcpy(&result, pSaharaCmd10Buffer->begin() + SOC_HW_VERSION_OFFSET, 2);
      pSaharaCmd10Buffer = NULL;
      m_socHwVersion = result;

      socHw << std::hex << std::uppercase << std::showbase << std::setw(4) << std::setfill('0')
            << static_cast<int32_t>(m_socHwVersion);

      FLOG_INFO("SOC_HW_VERSION: " + socHw.str());
   }
   else if(Sahara::SAHARA_END_OF_IMAGE_TRANS == pHeader->m_command)
   {
      const Sahara::EndOfImageTransfer* pEndOfImageTransfer =
         Util::buffer_cast<const Sahara::EndOfImageTransfer*>(pBuffer->begin(), pBuffer->size());
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_RESPONSE_ERROR,
         "Received end of image transfer from the device for sahara command: "
         "Get Fuse Information, 0x0A, status: " +
            std::to_string(pEndOfImageTransfer->m_status)
      ));
   }
   else
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "Command execute response not received "
         "from Sahara protocol: " +
            pSahara->getDescription()
      ));
   }

   FLOG_INFO("Sending sahara command: CMD_DEBUG_READ");
   pSendBuffer = pSahara->createCommand<Sahara::CommandExecute>(Sahara::SAHARA_COMMAND_EXECUTE);
   pCmdExecute = Util::buffer_cast<Sahara::CommandExecute*>(pSendBuffer->begin(), pSendBuffer->size());
   pCmdExecute->m_clientCommand = Sahara::CMD_DEBUG_READ;
   m_pSaharaConnection->sendSync(pSendBuffer);
   // Get a CMD_EXEC_RESP from the device
   pBuffer = pSahara->getFrame();
   TOOLS_ASSERT_OR_THROW(
      pBuffer != nullptr,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "No packet received from device sahara debug command "
         "execute (command state)" +
            pSahara->getDescription()
      )
   );
   pHeader = Util::buffer_cast<const Sahara::FrameHeader*>(pBuffer->begin(), pBuffer->size());
   if(Sahara::SAHARA_COMMAND_EXECUTE_RESP == pHeader->m_command)
   {
      pSahara->setCommandModeEnabled(true);
      const Sahara::CommandExecuteResp* pCmdExecuteResp =
         Util::buffer_cast<const Sahara::CommandExecuteResp*>(pBuffer->begin(), pBuffer->size());

      TOOLS_ASSERT_OR_THROW(
         pBuffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "No packet received from device sahara debug "
            "command execute (command state)" +
               pSahara->getDescription()
         )
      );

      size_t commandDataLength = 0;
      pSendBuffer = pSahara->createCommand<Sahara::CommandExecuteData>(Sahara::SAHARA_COMMAND_EXECUTE_DATA);
      Sahara::CommandExecuteData* pCmdExecuteData =
         Util::buffer_cast<Sahara::CommandExecuteData*>(pSendBuffer->begin(), pSendBuffer->size());
      pCmdExecuteData->m_clientCommand = Sahara::CMD_DEBUG_READ;
      m_pSaharaConnection->sendSync(pSendBuffer);

      Device::SharedByteBufferPtr pSaharaCmd6Buffer = std::make_shared<Device::SharedByteBuffer>();
      pSaharaCmd6Buffer->resize(pCmdExecuteResp->m_responseLength);
      while(commandDataLength < pCmdExecuteResp->m_responseLength)
      {
         Device::SharedByteBufferPtr pPartial = pSahara->getFrame();
         if(pPartial == nullptr)
         {
            TOOLS_THROW(Device::Exception(
               Device::Exception::DEVICE_RESPONSE_ERROR,
               "Error dumping file from " + pSahara->getDescription()
            ));
         };

         std::memcpy(pSaharaCmd6Buffer->begin() + commandDataLength, pPartial->begin(), pPartial->size());
         commandDataLength += pPartial->size();
      }

      TOOLS_ASSERT_OR_THROW(
         pSaharaCmd6Buffer != nullptr,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "No data received from device sahara command: Read "
            "Debug Data, 0x06"
         )
      );

      TOOLS_ASSERT_OR_THROW(
         BOOT_CONFIG_MIN_SIZE < pSaharaCmd6Buffer->size(),
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PACKET,
            "Boot config not supported by the device, failed to "
            "retrive storage or memory type from the device"
         )
      );

      int32_t result = INVALID_BOOT_CONFIG;
      std::memcpy(&result, pSaharaCmd6Buffer->begin() + BOOT_CONFIG_OFFSET, 4);
      pSaharaCmd6Buffer = NULL;
      m_bootConfig = result;

      std::ostringstream ss;
      ss << std::hex << std::uppercase << std::showbase << std::setw(2) << std::setfill('0')
         << static_cast<int32_t>(m_bootConfig);

      FLOG_INFO("Device boot config value: " + ss.str());
   }
   else if(Sahara::SAHARA_END_OF_IMAGE_TRANS == pHeader->m_command)
   {
      const Sahara::EndOfImageTransfer* pEndOfImageTransfer =
         Util::buffer_cast<const Sahara::EndOfImageTransfer*>(pBuffer->begin(), pBuffer->size());
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_RESPONSE_ERROR,
         "Received end of image transfer from the device for "
         "sahara command: Read Debug Data, 0x06, status: " +
            std::to_string(pEndOfImageTransfer->m_status)
      ));
   }
   else
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_INVALID_PACKET,
         "Command execute response not received "
         "from Sahara protocol: " +
            pSahara->getDescription()
      ));
   }

   // DclManager removed - boot config description retrieval disabled
   FLOG_INFO(("Boot config info: SOC_HW_VERSION: " + socHw.str() + ", Boot config: " + std::to_string(m_bootConfig)));

   std::string description = "DCL not available";
   std::string error = "";

   TOOLS_ASSERT_OR_THROW(
      !description.empty() || error.empty(),
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Failed to retrive storage or memory type from the DCL "
         "DB, SOC_HW_VERSION: " +
            socHw.str() + ", Error: " + error
      )
   );

   std::string bootDescription = description;
   std::list<ImageTransfer::MemoryType> memTypes = retriveBootConfig(bootDescription);
   if(memTypes.size() > 0)
   {
      memType = memTypes.front();
   }

   FLOG_INFO("Retrieved device memory type from the device, " /*+
                                                                 std::to_string(memType.c_str())*/
   );
   return memType;
}
/// <summary>
///   Retreive list of memory types from boot config description
/// </summary>
/// <param name="bootConfig"></param>
/// <returns>std::list<ImageTransfer::MemoryType></returns>
std::list<ImageTransfer::MemoryType> ImageTransfer::retriveBootConfig(std::string& bootConfig)
{
   std::list<ImageTransfer::MemoryType> memTypes;
   if(bootConfig.empty())
   {
      return memTypes;
   }

   std::smatch results;
   if(std::regex_search(bootConfig, results, std::regex("(UFS)", std::regex::icase)))
   {
      memTypes.push_back(ImageTransfer::MemoryType::MEMORY_TYPE_UFS);
   }
   if(std::regex_search(bootConfig, results, std::regex("(EMMC)", std::regex::icase)))
   {
      memTypes.push_back(ImageTransfer::MemoryType::MEMORY_TYPE_EMMC);
   }
   if(std::regex_search(bootConfig, results, std::regex("(NAND)", std::regex::icase)))
   {
      memTypes.push_back(ImageTransfer::MemoryType::MEMORY_TYPE_NAND);
   }
   if(std::regex_search(bootConfig, results, std::regex("(NVME)", std::regex::icase)))
   {
      memTypes.push_back(ImageTransfer::MemoryType::MEMORY_TYPE_NVME);
   }
   if(std::regex_search(bootConfig, results, std::regex("(SPI)", std::regex::icase)))
   {
      memTypes.push_back(ImageTransfer::MemoryType::MEMORY_TYPE_SPINOR);
   }
   return memTypes;
}

// ----------------------------------------------------------------------------
// firehoseRunOfflineProcess
//
/// 0. Create DigestsToSign.bin.mbn, ChainedTableOfDigests.bin
/// 1. Create build validation digest
/// Firehose cmd should match with the cmd formulated in firehoseDoVipDownload
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseRunOfflineProcess(
   const OfflineProcess process,
   const std::filesystem::path& buildPathDir,
   const std::filesystem::path& outputPathDir
)
{
   Firehose::FirehoseCommandType firehoseCommand;

   Device::Protocol::FirehosePtr pFirehoseProtocol;
   if(m_pOfflineFirehoseLoader == nullptr)
   {
      m_pOfflineFirehoseLoader = std::make_shared<Device::Protocol::FirehoseLoader>(pFirehoseProtocol);
   }
   TOOLS_ASSERT_OR_THROW(
      m_pOfflineFirehoseLoader != nullptr,
      Device::Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Firehose Loader not active  ")
   );

   prepareDownloadCmd(buildPathDir, firehoseCommand, true);

   switch(process)
   {
      case OfflineProcess::PROCESS_VIP_DIGEST:
         firehoseCommand.push_back(std::string("--createvipdigests=") + m_digestHeaderType.c_str());
         break;
      case OfflineProcess::PROCESS_BUILD_VALIDATION_DIGEST:
         firehoseCommand.push_back(std::string("--generatesha256file"));
         break;
      default:
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Invalid offline process"));
   }

   notify(std::make_shared<ImageTransferEvent>(ImageTransferEvent::FIREHOSE_CREATE_DIGESTS, "Create Digest files"));

   m_pOfflineFirehoseLoader->clearSettings();
   std::filesystem::path availibleOutputPathDir;
   if(outputPathDir.empty())
   {
      availibleOutputPathDir = Device::Manager::getInstance()->getTempDirectory();
   }
   else
   {
      availibleOutputPathDir = outputPathDir;
   }
   firehoseCommand.push_back(std::string("--mainoutputdir=") + availibleOutputPathDir.string().c_str());
   if(OfflineProcess::PROCESS_BUILD_VALIDATION_DIGEST == process)
   {
      std::filesystem::path availibleDigestFilePath =
         (availibleOutputPathDir.string() + static_cast<char>(std::filesystem::path::preferred_separator) +
          "BuildValidation.digest")
            .c_str();
      firehoseCommand.push_back(std::string("--digestsperfilename=") + availibleDigestFilePath.string().c_str());
   }

   int32_t errorCode = 0;
   std::string errorString;
   try
   {
      errorCode = m_pOfflineFirehoseLoader->executeCommand(firehoseCommand);
   }
   catch(const Device::Exception& e)
   {
      TOOLS_THROW(e);
   }
   catch(const ToolException& e)
   {
      TOOLS_THROW(e);
   }
   catch(...)
   {
      errorString = m_pOfflineFirehoseLoader->getDeviceErrorString();
      if(!errorString.empty())
      {
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_RESPONSE_ERROR, errorString));
      }
      else
      {
         errorString = m_pOfflineFirehoseLoader->getInternalErrorString();
         TOOLS_THROW(Device::
                        Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Firehose process failed: " + errorString));
      }
   }


   if(errorCode != 0)
   {
      errorString = m_pOfflineFirehoseLoader->getDeviceErrorString();
      if(!errorString.empty())
      {
         TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_RESPONSE_ERROR, errorString));
      }
      else
      {
         errorString = m_pOfflineFirehoseLoader->getInternalErrorString();
         TOOLS_THROW(Device::
                        Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Firehose process failed: " + errorString));
      }
   }
}


// ----------------------------------------------------------------------------
// firehoseDoVipDownload
//
/// Transfers Image through VIP download
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseDoDownload(const std::filesystem::path& buildPathDir)
{
   FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Firehose>();
   Firehose::FirehoseCommandType firehoseCommand;

   prepareDownloadCmd(buildPathDir, firehoseCommand);

   // These will not be set in case of CreateDigests
   if(!m_signedDigestsFile.empty())
   {
      notify(std::make_shared<ImageTransferEvent>(ImageTransferEvent::FIREHOSE_VIP_DOWNLOAD, "Perform VIP Download"));
   }

   if(!m_provisionFile.empty())
   {
      notify(std::make_shared<
             ImageTransferEvent>(ImageTransferEvent::FIREHOSE_UFS_PROVISIONING, "UFS Provision Started"));
      pFirehose->processCommand(firehoseCommand, m_outputDir);
   }
   else
   {
      if(m_bReadImages)
      {
         notify(std::make_shared<
                ImageTransferEvent>(ImageTransferEvent::FIREHOSE_READ_IMAGE_FILES, "Read Image Files Started"));
      }
      else
      {
         notify(std::make_shared<
                ImageTransferEvent>(ImageTransferEvent::TRANSFER_FIREHOSE_PROGRAMMER, "Download Started"));
      }
      pFirehose->processCommand(firehoseCommand, m_outputDir, m_downloadRxTimeoutInMs);
   }
   notify(std::make_shared<ImageTransferEvent>(ImageTransferEvent::TRANSFER_FIREHOSE_PROGRAMMER, "Process Finished"));
}

// ----------------------------------------------------------------------------
// firehoseReset
//
/// Resets device
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseReset()
{
   FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
   Firehose::FirehoseCommandType firehoseCommand;

   firehoseCommand.push_back(std::string("Firehose"));
   firehoseCommand.push_back(std::string("--port=\\\\.\\COM"));
   firehoseCommand.push_back(std::string("--reset"));
   firehoseCommand.push_back(std::string("--noprompt"));

   notify(std::make_shared<ImageTransferEvent>(ImageTransferEvent::FIREHOSE_RESETTING, "Firehose resetting"));

   pFirehose->processCommand(firehoseCommand, m_outputDir);
}

// ----------------------------------------------------------------------------
//  setDdrStorePath
//
/// Sets DDR store path
// ----------------------------------------------------------------------------
void ImageTransfer::setDdrStorePath(const std::filesystem::path& path)
{
   (void)path; // DDR store path is no longer used internally
}

// ----------------------------------------------------------------------------
// getPartitionHeaderFromFile
//
/// Read partition header structure from an image file
// ----------------------------------------------------------------------------
void ImageTransfer::getPartitionHeaderFromFile(const std::filesystem::path& imageFile, PartitionHeader& partitionHeader)
{
   auto outputFile = std::make_shared<std::fstream>();
   std::filesystem::path localImageFile = Device::Manager::getInstance()->getAccessiblePath(imageFile);
   outputFile->exceptions(std::ios::failbit | std::ios::badbit);

   outputFile->open(localImageFile.string().c_str(), std::ios::in | std::ios::binary);

   uint64_t fileSize = std::filesystem::file_size(localImageFile.string().c_str());
   TOOLS_ASSERT_OR_THROW(
      sizeof(PartitionHeader) <= fileSize,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Partition header file size too small: " + std::to_string(fileSize)
      )
   );

   outputFile->read(reinterpret_cast<char*>(&partitionHeader), sizeof(PartitionHeader));
   outputFile->close();

   TOOLS_ASSERT_OR_THROW(
      PARTITION_HEADER_SIGNATURE == partitionHeader.signature,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Partition header signature does not match: " + std::to_string(partitionHeader.signature)
      )
   );
}

// ----------------------------------------------------------------------------
// getPartitionEntriesFromFile
//
/// Read partition entries from an image file for GPT
// ----------------------------------------------------------------------------
void ImageTransfer::getPartitionEntriesFromFile(
   const std::filesystem::path& imageFile,
   uint32_t partitionIndex,
   uint32_t entryCount,
   uint32_t entrySize,
   std::vector<PartitionEntry>& partitionEntryList
)
{
   TOOLS_ASSERT_OR_THROW(
      sizeof(PartitionEntryParameters) <= entrySize,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Partition entry size invalid: " + std::to_string(entrySize)
      )
   );


   auto outputFile = std::make_shared<std::fstream>();
   std::filesystem::path localImageFile = Device::Manager::getInstance()->getAccessiblePath(imageFile);
   outputFile->exceptions(std::ios::failbit | std::ios::badbit);
   outputFile->open(localImageFile.string().c_str(), std::ios::in | std::ios::binary);

   uint64_t fileSize = std::filesystem::file_size(localImageFile.string().c_str());
   TOOLS_ASSERT_OR_THROW(
      entrySize * entryCount <= fileSize,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Partition entry file size too small: " + std::to_string(fileSize)
      )
   );

   for(uint32_t i = 0; i < entryCount; ++i)
   {
      PartitionEntry partitionEntry;
      partitionEntry.partitionIndex = partitionIndex;

      outputFile
         ->read(reinterpret_cast<char*>(&partitionEntry.partitionEntryParameters), sizeof(PartitionEntryParameters));
      std::wstring partitionEntryNameUtf16;
      partitionEntryNameUtf16.resize(entrySize - sizeof(PartitionEntryParameters));
      outputFile
         ->read(reinterpret_cast<char*>(&partitionEntryNameUtf16.at(0)), entrySize - sizeof(PartitionEntryParameters));
      partitionEntry.partitionEntryName = Util::fromWString(partitionEntryNameUtf16);
      partitionEntry.partitionEntryName.resize(Util::getStringLength(&partitionEntry.partitionEntryName.at(0)));
      partitionEntryList.push_back(partitionEntry);
   }

   outputFile->close();
}

// ----------------------------------------------------------------------------
// getPartitionHeaderFromFile
//
/// Read partition header structure from an image file
// ----------------------------------------------------------------------------
void ImageTransfer::getPartitionHeaderFromFileMibib(
   const std::filesystem::path& imageFile,
   uint32_t pageSize,
   uint32_t pagePerBlock,
   uint32_t& blockOffset,
   MibibPartitionHeader& partitionHeader
)
{
   auto outputFile = std::make_shared<std::fstream>();
   std::filesystem::path localImageFile = Device::Manager::getInstance()->getAccessiblePath(imageFile);
   outputFile->exceptions(std::ios::failbit | std::ios::badbit);

   outputFile->open(localImageFile.string().c_str(), std::ios::in | std::ios::binary);

   uint64_t fileSize = std::filesystem::file_size(localImageFile.string().c_str());
   TOOLS_ASSERT_OR_THROW(
      uint64_t(pageSize) * pagePerBlock * PARTITION_HEADER_BLOCK_NUM_NAND <= fileSize,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Partition entry file size too small: " + std::to_string(fileSize)
      )
   );

   // Find MibibHeader
   MibibHeader mibibHeader;
   for(uint32_t i = 0; i < PARTITION_HEADER_BLOCK_NUM_NAND; ++i)
   {
      outputFile->read(reinterpret_cast<char*>(&mibibHeader), sizeof(MibibHeader));
      if(mibibHeader.magicNumber1 == MIBIB_MAGIC1 && mibibHeader.magicNumber2 == MIBIB_MAGIC2)
      {
         TOOLS_ASSERT_OR_THROW(
            MIBIB_NUM_PARTITION_HEADER == mibibHeader.numberOfPartitionHeaderEntries,
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PARAMETERS,
               "Partition head number is not valid: " + std::to_string(mibibHeader.numberOfPartitionHeaderEntries)
            )
         );

         outputFile->seekg(pageSize - sizeof(MibibHeader), std::ios::beg);
         outputFile->read(reinterpret_cast<char*>(&partitionHeader), sizeof(MibibPartitionHeader));
         TOOLS_ASSERT_OR_THROW(
            partitionHeader.magicNumber1 == MIBIB_PARAMETER_MAGIC1 &&
               partitionHeader.magicNumber2 == MIBIB_PARAMETER_MAGIC2,
            Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Partition head magic is not valid")
         );
         TOOLS_ASSERT_OR_THROW(
            partitionHeader.sizeOfPartitionEntry == mibibHeader.sizeOfPartitionEntry,
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PARAMETERS,
               "Mibib partition size is not match with partition head" +
                  std::to_string(partitionHeader.sizeOfPartitionEntry)
            )
         );
         blockOffset = i;
         return;
      }
      else
      {
         outputFile->seekg(uint64_t(pageSize) * pagePerBlock - sizeof(MibibHeader), std::ios::beg);
      }
   }

   outputFile->close();

   TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Can not find mibib magic number"));
}

// ----------------------------------------------------------------------------
// getPartitionEntriesFromFileMibib
//
/// Read partition entries from an image file for Mibib
// ----------------------------------------------------------------------------
void ImageTransfer::getPartitionEntriesFromFileMibib(
   const std::filesystem::path& imageFile,
   uint32_t pageSize,
   uint32_t pagePerBlock,
   uint32_t blockOffset,
   uint32_t partitionIndex,
   uint32_t entryCount,
   std::vector<PartitionEntry>& partitionEntryList
)
{
   auto outputFile = std::make_shared<std::fstream>();
   std::filesystem::path localImageFile = Device::Manager::getInstance()->getAccessiblePath(imageFile);
   outputFile->exceptions(std::ios::failbit | std::ios::badbit);

   outputFile->open(localImageFile.string().c_str(), std::ios::in | std::ios::binary);
   uint32_t entrySize = sizeof(MibibPartitionEntryParameters) + MIBIB_PARTITION_NAME_SIZE;
   uint64_t fileSize = std::filesystem::file_size(localImageFile.string().c_str());
   uint64_t fileOffset = uint64_t(blockOffset) * pageSize * pagePerBlock + pageSize + sizeof(MibibPartitionHeader);
   TOOLS_ASSERT_OR_THROW(
      uint64_t(entryCount) * entrySize + fileOffset <= fileSize,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Partition entry file size too small: " + std::to_string(fileSize)
      )
   );
   outputFile->seekg(fileOffset, std::ios::beg);
   for(uint32_t i = 0; i < entryCount; ++i)
   {
      PartitionEntry partitionEntry;
      std::memset(&partitionEntry.partitionEntryParameters, 0, sizeof(PartitionEntryParameters));
      partitionEntry.partitionIndex = partitionIndex;

      std::string partitionEntryName;
      partitionEntryName.resize(MIBIB_PARTITION_NAME_SIZE);
      outputFile->read(reinterpret_cast<char*>(&partitionEntryName.at(0)), MIBIB_PARTITION_NAME_SIZE);
      partitionEntry.partitionEntryName = partitionEntryName;
      partitionEntry.partitionEntryName.resize(Util::getStringLength(&partitionEntry.partitionEntryName.at(0)));
      MibibPartitionEntryParameters partitionEntryParameters;
      outputFile->read(reinterpret_cast<char*>(&partitionEntryParameters), sizeof(MibibPartitionEntryParameters));
      partitionEntry.partitionEntryParameters.startingLba =
         uint64_t(partitionEntryParameters.startingLba) * pagePerBlock;
      partitionEntry.partitionEntryParameters.endingLba =
         uint64_t(partitionEntryParameters.startingLba + partitionEntryParameters.lengthLba) * pagePerBlock - 1;
      partitionEntry.partitionEntryParameters.attributes = partitionEntryParameters.attributes;
      partitionEntryList.push_back(partitionEntry);
   }

   outputFile->close();
}

// ----------------------------------------------------------------------------
// getPartitionDefaultSectorSize
//
/// Return partition sector size based on memory type
// ----------------------------------------------------------------------------
uint32_t ImageTransfer::getPartitionDefaultSectorSize(MemoryType type)
{
   uint32_t size;

   switch(type)
   {
      case MEMORY_TYPE_UFS:
      case MEMORY_TYPE_SPINOR:
         size = PARTITION_DEFAULT_SECTOR_SIZE_4K;
         break;

      case MEMORY_TYPE_EMMC:
      case MEMORY_TYPE_UNKNOWN:
      default:
         size = PARTITION_DEFAULT_SECTOR_SIZE;
         break;
   }

   return size;
}

// ----------------------------------------------------------------------------
// getPartitionDefaultSectorParameterNand
//
/// Return partition sector parameters for NAND
// ----------------------------------------------------------------------------
void ImageTransfer::getPartitionDefaultSectorParameterNand(uint32_t& pageSize, uint32_t& pagePerBlock)
{
   // Shall never fails here now, but still keep the check in case we have new
   // process\customization later
   TOOLS_ASSERT_OR_THROW(
      0 != m_pageSize && 0 != m_pagePerBlock,
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "NAND information is invalid, please run get flash "
         "information in advance"
      )
   );

   pageSize = m_pageSize;
   pagePerBlock = m_pagePerBlock;
}

// ----------------------------------------------------------------------------
// firehoseGetPartitionEntries
//
/// Get partition entry list from device
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseGetPartitionEntries(std::vector<PartitionEntry>& partitionEntries)
{
   if(m_partitionIndexList.size() > 0)
   {
      std::vector<int32_t>::const_iterator it = m_partitionIndexList.begin();
      std::vector<int32_t>::const_iterator end = m_partitionIndexList.end();
      for(; it != end; ++it)
      {
         TOOLS_ASSERT_OR_THROW(
            (*it >= 0),
            Device::Exception(
               Device::Exception::DEVICE_INVALID_PARAMETERS,
               "Invalid Partition Number:" + std::to_string(*it)
            )
         );
      }
   }
   else
   {
      m_partitionIndexList.push_back(0);
      if(MemoryType::MEMORY_TYPE_UFS == m_memoryType)
      {
         m_partitionIndexList.push_back(1);
         m_partitionIndexList.push_back(2);
         // CDT is skipped
         m_partitionIndexList.push_back(4);
         m_partitionIndexList.push_back(5);
      }
   }

   std::vector<int32_t>::iterator it = m_partitionIndexList.begin();
   std::vector<int32_t>::iterator end = m_partitionIndexList.end();
   for(; it != end; ++it)
   {
      if(m_numPartition == 0 || m_numPartition > *it)
      {
         std::filesystem::path outputDir = m_outputDir;
         if(outputDir.empty())
         {
            outputDir = Device::Manager::getInstance()->getTempDirectory();
         }
         std::filesystem::path tempFilePath = Util::createTempFileName(outputDir);
         int32_t i = *it;
         if(MEMORY_TYPE_NAND == m_memoryType)
         {
            // Mibib magic header partition header
            MibibPartitionHeader partitionHeader;
            uint32_t pageSize = 0;
            uint32_t pagePerBlock = 0;
            uint32_t blockOffset = 0;
            getPartitionDefaultSectorParameterNand(pageSize, pagePerBlock);
            std::vector<DataChunkOptions> options;
            DataChunkOptions headerOptions = {
               i,
               std::to_string(0),
               std::to_string(uint64_t(PARTITION_HEADER_BLOCK_NUM_NAND) * pagePerBlock),
               tempFilePath
            };
            options.push_back(headerOptions);
            firehoseSavePartitionSectors(options);
            getPartitionHeaderFromFileMibib(tempFilePath, pageSize, pagePerBlock, blockOffset, partitionHeader);
            if(blockOffset + partitionHeader.sizeOfPartitionEntry >= PARTITION_HEADER_BLOCK_NUM_NAND)
            {
               // No enough data, read again
               tempFilePath = Util::createTempFileName(outputDir);
               options.clear();
               DataChunkOptions dataOptions = {
                  i,
                  std::to_string(uint64_t(blockOffset) * pagePerBlock),
                  std::to_string(uint64_t(partitionHeader.sizeOfPartitionEntry) * pagePerBlock),
                  tempFilePath
               };
               options.push_back(dataOptions);
               firehoseSavePartitionSectors(options);
               blockOffset = 0;
            }
            getPartitionEntriesFromFileMibib(
               tempFilePath,
               pageSize,
               pagePerBlock,
               blockOffset,
               i,
               partitionHeader.numberOfPartitionEntries,
               partitionEntries
            );
         }
         else
         {
            // Get partition header
            PartitionHeader partitionHeader;
            std::vector<DataChunkOptions> options;
            DataChunkOptions headerOptions = {
               i,
               std::to_string(PARTITION_HEADER_SECTOR_OFFSET),
               std::to_string(PARTITION_HEADER_SECTOR_NUM),
               tempFilePath
            };
            options.push_back(headerOptions);
            firehoseSavePartitionSectors(options);
            getPartitionHeaderFromFile(tempFilePath, partitionHeader);

            tempFilePath = Util::createTempFileName(outputDir);

            auto tempFile = std::make_shared<std::fstream>();
            tempFile->exceptions(std::ios::failbit | std::ios::badbit);
            tempFile->open(tempFilePath.string().c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
            std::string emptyBuffer;
            emptyBuffer.resize(partitionHeader.numberOfPartitionEntries * partitionHeader.sizeOfPartitionEntry);
            tempFile->write(
               emptyBuffer.c_str(),
               partitionHeader.numberOfPartitionEntries * partitionHeader.sizeOfPartitionEntry
            );
            tempFile->close();
            uint32_t entrySectors = partitionHeader.sizeOfPartitionEntry * partitionHeader.numberOfPartitionEntries;
            uint32_t sectorSize = getPartitionDefaultSectorSize(m_memoryType);
            entrySectors = (entrySectors + sectorSize - 1) / sectorSize;
            options.clear();
            DataChunkOptions dataOptions =
               {i, std::to_string(partitionHeader.partitionEntryLba), std::to_string(entrySectors), tempFilePath};
            options.push_back(dataOptions);
            firehoseSavePartitionSectors(options);
            getPartitionEntriesFromFile(
               tempFilePath,
               i,
               partitionHeader.numberOfPartitionEntries,
               partitionHeader.sizeOfPartitionEntry,
               partitionEntries
            );
         }
      }
      else
      {
         notify(std::make_shared<ImageTransferEvent>(
            ImageTransferEvent::FIREHOSE_ERASE,
            "Partition Number out of bound and Ignored:" + std::to_string(*it)
         ));
      }
   }
}

// ----------------------------------------------------------------------------
// firehoseSavePartitionSectors
//
/// Read partition sectors from device and save to an image file
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseSavePartitionSectors(const std::vector<DataChunkOptions>& options)
{
   std::filesystem::path tempFilePath = Util::createTempFileName(Device::Manager::getInstance()->getTempDirectory());

   auto tempFile = std::make_shared<std::fstream>();
   tempFile->exceptions(std::ios::failbit | std::ios::badbit);
   tempFile->open(tempFilePath.string().c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
   std::string xmlBuffer;

   xmlBuffer.append("<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n");
   xmlBuffer.append("<data>\n");
   std::vector<DataChunkOptions>::const_iterator it = options.begin();
   std::vector<DataChunkOptions>::const_iterator end = options.end();
   std::filesystem::path imagePath;
   for(; it != end; ++it)
   {
      xmlBuffer.append("<program ");
      xmlBuffer.append("filename=\"");
      xmlBuffer.append(it->imageFile.filename().string().c_str());
      if(MEMORY_TYPE_NAND == m_memoryType)
      {
         uint32_t pageSize = 0;
         uint32_t pagePerBlock = 0;
         getPartitionDefaultSectorParameterNand(pageSize, pagePerBlock);
         xmlBuffer.append("\" PAGES_PER_BLOCK=\"");
         xmlBuffer.append(std::to_string(pagePerBlock));
         xmlBuffer.append("\" SECTOR_SIZE_IN_BYTES=\"");
         xmlBuffer.append(std::to_string(pageSize));
         xmlBuffer.append("\" num_partition_sectors=\"");
         xmlBuffer.append(it->sectorCount);
         xmlBuffer.append("\" start_sector=\"");
         xmlBuffer.append(it->startSector);
      }
      else
      {
         xmlBuffer.append("\" SECTOR_SIZE_IN_BYTES=\"");
         xmlBuffer.append(std::to_string(getPartitionDefaultSectorSize(m_memoryType)));
         xmlBuffer.append("\" num_partition_sectors=\"");
         xmlBuffer.append(it->sectorCount);
         xmlBuffer.append("\" start_sector=\"");
         xmlBuffer.append(it->startSector);
      }
      xmlBuffer.append("\" physical_partition_number=\"");
      xmlBuffer.append(std::to_string(it->partitionIndex));
      xmlBuffer.append("\"/>\n");
      imagePath = it->imageFile.parent_path();
   }
   xmlBuffer.append("</data>");

   tempFile->write(xmlBuffer.c_str(), xmlBuffer.size());
   tempFile->close();

   FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
   Firehose::FirehoseCommandType firehoseCommand;

   firehoseCommand.push_back(std::string("Firehose"));
   firehoseCommand.push_back(std::string("--port=\\\\.\\COM"));
   if(0 != m_slot)
   {
      firehoseCommand.push_back(std::string("--slot=") + std::to_string(m_slot));
   }
   firehoseCommand.push_back(std::string("--search_path=") + tempFilePath.parent_path().string().c_str());
   firehoseCommand.push_back(std::string("--sendxml=") + tempFilePath.string().c_str());
   firehoseCommand.push_back(std::string("--convertprogram2read"));
   firehoseCommand.push_back(std::string("--showpercentagecomplete"));
   firehoseCommand.push_back(std::string("--memoryname=" + firehoseGetMemoryName()));
   firehoseCommand.push_back(std::string("--noprompt"));
   if(0 != m_maxPayloadSize)
   {
      firehoseCommand.push_back(std::string("--maxpayloadsizeinbytes=") + std::to_string(m_maxPayloadSize).c_str());
   }

   if(0 != m_maxReadPayloadSize)
   {
      firehoseCommand
         .push_back(std::string("--maxreadpayloadsizeinbytes=") + std::to_string(m_maxReadPayloadSize).c_str());
   }

   appendZlpAwareHostOption(firehoseCommand);

   if(!imagePath.empty())
   {
      m_outputDir = imagePath;
   }
   pFirehose->processCommand(firehoseCommand, m_outputDir);
}

// ----------------------------------------------------------------------------
// firehoseFlashPartitionSectors
//
/// Flash partition sectors from an image file
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseFlashPartitionSectors(const std::vector<DataChunkOptions>& options)
{
   std::filesystem::path tempFilePath = Util::createTempFileName(Device::Manager::getInstance()->getTempDirectory());

   auto tempFile = std::make_shared<std::fstream>();
   tempFile->exceptions(std::ios::failbit | std::ios::badbit);
   tempFile->open(tempFilePath.string().c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
   std::string xmlBuffer;

   xmlBuffer.append("<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n");
   xmlBuffer.append("<data>\n");
   std::filesystem::path imagePath;
   std::vector<DataChunkOptions>::const_iterator it = options.begin();
   std::vector<DataChunkOptions>::const_iterator end = options.end();
   for(; it != end; ++it)
   {
      xmlBuffer.append("<program ");
      xmlBuffer.append("filename=\"");
      xmlBuffer.append(it->imageFile.filename().string().c_str());
      if(MEMORY_TYPE_NAND == m_memoryType)
      {
         uint32_t pageSize = 0;
         uint32_t pagePerBlock = 0;
         getPartitionDefaultSectorParameterNand(pageSize, pagePerBlock);
         xmlBuffer.append("\" PAGES_PER_BLOCK=\"");
         xmlBuffer.append(std::to_string(pagePerBlock));
         xmlBuffer.append("\" SECTOR_SIZE_IN_BYTES=\"");
         xmlBuffer.append(std::to_string(pageSize));
         xmlBuffer.append("\" num_partition_sectors=\"");
         xmlBuffer.append(it->sectorCount);
         xmlBuffer.append("\" start_sector=\"");
         xmlBuffer.append(it->startSector);
      }
      else
      {
         xmlBuffer.append("\" SECTOR_SIZE_IN_BYTES=\"");
         uint32_t sectorSize = getPartitionDefaultSectorSize(m_memoryType);
         xmlBuffer.append(std::to_string(sectorSize));
         xmlBuffer.append("\" num_partition_sectors=\"");
         uint64_t sectorCount = std::filesystem::file_size(it->imageFile);
         sectorCount = (sectorCount + sectorSize - 1) / sectorSize;
         xmlBuffer.append(std::to_string(sectorCount));
         xmlBuffer.append("\" start_sector=\"");
         xmlBuffer.append(it->startSector);
      }
      xmlBuffer.append("\" physical_partition_number=\"");
      xmlBuffer.append(std::to_string(it->partitionIndex));
      xmlBuffer.append("\"/>\n");
      imagePath = it->imageFile.parent_path();
   }
   xmlBuffer.append("</data>");


   tempFile->write(xmlBuffer.c_str(), xmlBuffer.size());
   tempFile->close();

   FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
   Firehose::FirehoseCommandType firehoseCommand;

   firehoseCommand.push_back(std::string("Firehose"));
   firehoseCommand.push_back(std::string("--port=\\\\.\\COM"));
   if(0 != m_slot)
   {
      firehoseCommand.push_back(std::string("--slot=") + std::to_string(m_slot));
   }
   firehoseCommand.push_back(std::string("--search_path=") + imagePath.string().c_str());
   firehoseCommand.push_back(std::string("--sendxml=") + tempFilePath.string().c_str());
   firehoseCommand.push_back(std::string("--showpercentagecomplete"));
   firehoseCommand.push_back(std::string("--memoryname=" + firehoseGetMemoryName()));
   firehoseCommand.push_back(std::string("--noprompt"));
   if(0 != m_maxPayloadSize)
   {
      firehoseCommand.push_back(std::string("--maxpayloadsizeinbytes=") + std::to_string(m_maxPayloadSize).c_str());
   }

   if(0 != m_maxReadPayloadSize)
   {
      firehoseCommand
         .push_back(std::string("--maxreadpayloadsizeinbytes=") + std::to_string(m_maxReadPayloadSize).c_str());
   }

   appendZlpAwareHostOption(firehoseCommand);

   if(VALIDATION_MODE_BINARY_READBACK == m_validationMode)
   {
      firehoseCommand.push_back(std::string("--verify_programming"));
   }
   else if(VALIDATION_MODE_SHA256_READBACK == m_validationMode)
   {
      firehoseCommand.push_back(std::string("--verify_programming_getsha"));
   }
   pFirehose->processCommand(firehoseCommand, m_outputDir);
}

// ----------------------------------------------------------------------------
// firehoseErasePartitionSectors
//
/// Erase partition sectors
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseErasePartitionSectors(const std::vector<DataChunkOptions>& options)
{
   TOOLS_ASSERT_OR_THROW(
      MEMORY_TYPE_UNKNOWN != m_memoryType,
      Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Memory type not set!")
   );

   std::filesystem::path tempFilePath = Util::createTempFileName(Device::Manager::getInstance()->getTempDirectory());

   auto tempFile = std::make_shared<std::fstream>();
   tempFile->exceptions(std::ios::failbit | std::ios::badbit);
   tempFile->open(tempFilePath.string().c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
   std::string xmlBuffer;

   xmlBuffer.append("<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n");
   xmlBuffer.append("<data>\n");
   std::vector<DataChunkOptions>::const_iterator it = options.begin();
   std::vector<DataChunkOptions>::const_iterator end = options.end();
   for(; it != end; ++it)
   {
      xmlBuffer.append("<erase ");
      if(MEMORY_TYPE_NAND == m_memoryType)
      {
         uint32_t pageSize = 0;
         uint32_t pagePerBlock = 0;
         getPartitionDefaultSectorParameterNand(pageSize, pagePerBlock);
         xmlBuffer.append("PAGES_PER_BLOCK=\"");
         xmlBuffer.append(std::to_string(pagePerBlock));
         xmlBuffer.append("\" SECTOR_SIZE_IN_BYTES=\"");
         xmlBuffer.append(std::to_string(pageSize));
         xmlBuffer.append("\" num_partition_sectors=\"");
         xmlBuffer.append(it->sectorCount);
         xmlBuffer.append("\" start_sector=\"");
         xmlBuffer.append(it->startSector);
      }
      else
      {
         xmlBuffer.append("SECTOR_SIZE_IN_BYTES=\"");
         xmlBuffer.append(std::to_string(getPartitionDefaultSectorSize(m_memoryType)));
         xmlBuffer.append("\" num_partition_sectors=\"");
         xmlBuffer.append(it->sectorCount);
         xmlBuffer.append("\" start_sector=\"");
         xmlBuffer.append(it->startSector);
      }
      xmlBuffer.append("\" physical_partition_number=\"");
      xmlBuffer.append(std::to_string(it->partitionIndex));
      xmlBuffer.append("\"/>\n");
   }
   xmlBuffer.append("</data>");

   tempFile->write(xmlBuffer.c_str(), xmlBuffer.size());
   tempFile->close();

   FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
   Firehose::FirehoseCommandType firehoseCommand;

   firehoseCommand.push_back(std::string("Firehose"));
   firehoseCommand.push_back(std::string("--port=\\\\.\\COM"));
   if(0 != m_slot)
   {
      firehoseCommand.push_back(std::string("--slot=") + std::to_string(m_slot));
   }
   firehoseCommand.push_back(std::string("--search_path=") + tempFilePath.parent_path().string().c_str());
   firehoseCommand.push_back(std::string("--sendxml=") + tempFilePath.filename().string().c_str());

   firehoseCommand.push_back(std::string("--showpercentagecomplete"));
   firehoseCommand.push_back(std::string("--memoryname=" + firehoseGetMemoryName()));
   firehoseCommand.push_back(std::string("--noprompt"));

   pFirehose->processCommand(firehoseCommand, m_outputDir);
}

// ----------------------------------------------------------------------------
// firehoseGetStorageInfo
//
/// Get Storage information form device
// ----------------------------------------------------------------------------
void ImageTransfer::firehoseGetStorageInfo(
   const int32_t phyPartitionNum,
   Device::Protocol::Firehose::StorageInfoType& storageInfo,
   bool bReset
)
{
   FirehosePtr pFirehose = (m_pFirehoseConnection->getProtocol()).dynamicCast<Device::Protocol::Firehose>();
   Firehose::FirehoseCommandType firehoseCommand;

   firehoseCommand.push_back(std::string("Firehose"));
   firehoseCommand.push_back(std::string("--port=\\\\.\\COM"));
   if(0 != m_slot)
   {
      firehoseCommand.push_back(std::string("--slot=") + std::to_string(m_slot));
   }
   firehoseCommand.push_back(std::string("--getstorageinfo=") + std::to_string(phyPartitionNum));
   firehoseCommand.push_back(std::string("--noprompt"));
   firehoseCommand.push_back(std::string("--memoryname=" + firehoseGetMemoryName()));
   // Do not using m_bResetAfterDownload to consider reset here
   if(bReset)
   {
      firehoseCommand.push_back(std::string("--reset"));
   }
   notify(std::make_shared<
          ImageTransferEvent>(ImageTransferEvent::FIREHOSE_GET_STORAGE_INFO, "Firehose get Storage Information"));
   pFirehose->processCommand(firehoseCommand, m_outputDir);
   pFirehose->getStorageInfo(storageInfo);
}

// ----------------------------------------------------------------------------
// onFirehoseEvent
//
// ----------------------------------------------------------------------------
void ImageTransfer::onFirehoseEvent(Device::Protocol::FirehoseEvent* pEvent)
{
   ImageTransferEvent::EventId iteEventId = ImageTransferEvent::INITIALIZE;
   Device::Protocol::FirehoseEvent::EventId eventId = pEvent->getEventId();
   if(eventId == Device::Protocol::FirehoseEvent::EventId::FIREHOSE_PROGRESSION)
   {
      iteEventId = ImageTransferEvent::FIREHOSE_PROGRESSION;
   }
   else if(eventId >= Device::Protocol::FirehoseEvent::EventId::FIREHOSE_TARGET_SAID)
   {
      iteEventId = ImageTransferEvent::FIREHOSE_INFORMATION;
   }
   else
   {
      iteEventId =
         static_cast<ImageTransferEvent::
                        EventId>(eventId + Device::Protocol::FirehoseEvent::EventId::FIREHOSE_TRANSFER_RAW_PROGRAMMER);
   }
   notify(std::make_shared<ImageTransferEvent>(iteEventId, pEvent->getDescription()));
}

// ----------------------------------------------------------------------------
// convertToSectorNumber
//
/// convert a sector string to number
// ----------------------------------------------------------------------------
uint64_t ImageTransfer::convertToSectorNumber(const std::string& sectorString, const uint64_t numDiskSectors) const
{
   if(sectorString.rfind("NUM_DISK_SECTORS", 0) == 0)
   {
      // parse suffix after "NUM_DISK_SECTORS"
      return static_cast<uint64_t>(static_cast<int64_t>(numDiskSectors) + std::stoll(sectorString.substr(16)));
   }
   else
   {
      return std::stoull(sectorString);
   }
}

// ----------------------------------------------------------------------------
// getPartitionEntriesFromBuild
//
// ----------------------------------------------------------------------------
void ImageTransfer::
   getPartitionEntriesFromBuild(const std::filesystem::path& buildPath, std::vector<PartitionEntry>& partitionEntries)
{
   TOOLS_ASSERT_OR_THROW(
      m_jsonFile.empty(),
      Device::Exception(Device::Exception::DEVICE_INVALID_PARAMETERS, "Preservation not supported for MetaBuild")
   );

   XmlPartitionInfoPtr pXmlPartitionInfo = std::make_shared<XmlPartitionInfo>();
   for(const auto& xmlPath: m_rawXmlList)
   {
      parseXmlAttributes(xmlPath, PROGRAM_TAG, [&pXmlPartitionInfo](const std::string& name, const std::string& value) {
         if(SECTOR_SIZE_IN_BYTES == name)
            pXmlPartitionInfo->setSectorSizeInBytes(std::stoull(value));
         else if(PHYSICAL_PARTITION_NUMBER == name)
            pXmlPartitionInfo->setPartitionIndex(std::stoi(value));
         else if(START_SECTOR == name)
            pXmlPartitionInfo->setStartSector(value);
         else if(NUM_PARTITION_SECTORS == name)
            pXmlPartitionInfo->setSectorCount(value);
         else if(FILE_NAME == name)
            pXmlPartitionInfo->setImageFile(value.c_str());
      });
   }
   XmlPatchInfoPtr pXmlPatchInfo = std::make_shared<XmlPatchInfo>();
   for(const auto& xmlPath: m_patchXmlList)
   {
      parseXmlAttributes(xmlPath, PATCH_TAG, [&pXmlPatchInfo](const std::string& name, const std::string& value) {
         if(SECTOR_SIZE_IN_BYTES == name)
            pXmlPatchInfo->setSectorSizeInBytes(std::stoull(value));
         else if(PHYSICAL_PARTITION_NUMBER == name)
            pXmlPatchInfo->setPartitionIndex(std::stoi(value));
         else if(START_SECTOR == name)
            pXmlPatchInfo->setStartSector(value);
         else if(FILE_NAME == name)
            pXmlPatchInfo->setImageFile(value.c_str());
         else if(BYTE_OFFSET == name)
            pXmlPatchInfo->setByteOffset(std::stoull(value));
         else if(SIZE_IN_BYTES == name)
            pXmlPatchInfo->setValueSizeInBytes(static_cast<uint32_t>(std::stoul(value)));
         else if(VALUE == name)
            pXmlPatchInfo->setValue(value);
      });
   }

   uint64_t sectorSizeInByte =
      0 == pXmlPatchInfo->getSectorSizeInBytes()
         ? pXmlPartitionInfo->getSectorSizeInBytes()
         : pXmlPatchInfo->getSectorSizeInBytes();
   TOOLS_ASSERT_OR_THROW(
      (pXmlPatchInfo->getSectorSizeInBytes() == 0 || pXmlPartitionInfo->getSectorSizeInBytes() == 0 ||
       pXmlPatchInfo->getSectorSizeInBytes() == pXmlPartitionInfo->getSectorSizeInBytes()),
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Sector size in bytes mismatch "
         "between partition and patch file"
      )
   );

   std::vector<int32_t>::iterator piit = m_partitionIndexList.begin();
   std::vector<int32_t>::iterator piend = m_partitionIndexList.end();
   for(; piit != piend; ++piit)
   {
      int32_t partitionIndex = *piit;
      DataChunkOptions options;
      bool findResult = false;
      TOOLS_IGNORE_EXCEPTIONS(findResult = pXmlPartitionInfo->getPartitionTable(partitionIndex, options));
      if(!findResult)
      {
         continue;
      }

      Device::Protocol::Firehose::StorageInfoType storageInfo;
      firehoseGetStorageInfo(partitionIndex, storageInfo, false);
      uint64_t numDiskSector = std::stoull(std::string(storageInfo.totalBlocks));

      std::filesystem::path fullPath = buildPath / options.imageFile;
      auto partitionTableFile = std::make_shared<std::fstream>();
      partitionTableFile->exceptions(std::ios::failbit | std::ios::badbit);

      partitionTableFile->open(fullPath.string().c_str(), std::ios::in | std::ios::binary);

      uint32_t fileSize = static_cast<uint32_t>(std::filesystem::file_size(fullPath.string().c_str())); // Assume size
                                                                                                        // is not so big
      Device::SharedByteBufferPtr partitionTableData = std::make_shared<Device::SharedByteBuffer>();
      partitionTableData->resize(fileSize);
      partitionTableFile->read(reinterpret_cast<char*>(partitionTableData->begin()), fileSize);
      partitionTableFile->close();

      // Patch before getting partition table
      std::vector<PatchOptions> patches;
      TOOLS_IGNORE_EXCEPTIONS(pXmlPatchInfo->getPartitionPatches(partitionIndex, patches));
      std::vector<PatchOptions>::iterator pit = patches.begin();
      std::vector<PatchOptions>::iterator pend = patches.end();
      for(; pit != pend; ++pit)
      {
         uint64_t patchStartSector = convertToSectorNumber(pit->startSector, numDiskSector);
         if(patchStartSector < convertToSectorNumber(options.sectorCount, numDiskSector))
         {
            // Apply the patch
            uint64_t requestOffset = sectorSizeInByte * patchStartSector + pit->byteOffset;
            uint64_t requestLength = requestOffset + pit->valueSizeInBytes;
            TOOLS_ASSERT_OR_THROW(
               requestLength <= fileSize,
               Device::Exception(
                  Device::Exception::DEVICE_INVALID_PARAMETERS,
                  "Partition file size too small for patch: " + std::to_string(fileSize)
               )
            );

            uint64_t valueUint64 = convertToSectorNumber(pit->value, numDiskSector);
            std::memcpy(partitionTableData->begin() + requestOffset, &valueUint64, pit->valueSizeInBytes);
         }
      }

      // Get partition infomation from the file
      PartitionHeader partitionHeader;
      TOOLS_ASSERT_OR_THROW(
         TOOLS_SIZEOF(PartitionHeader) + sectorSizeInByte <= fileSize,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Partition file size too small for header: " + std::to_string(fileSize)
         )
      );

      std::memcpy(
         reinterpret_cast<uint8_t*>(&partitionHeader),
         partitionTableData->begin() + sectorSizeInByte,
         TOOLS_SIZEOF(PartitionHeader)
      );

      uint64_t partitionStartOffset = partitionHeader.partitionEntryLba * sectorSizeInByte;
      uint32_t entryCount = partitionHeader.numberOfPartitionEntries;
      uint32_t entrySize = partitionHeader.sizeOfPartitionEntry;
      TOOLS_ASSERT_OR_THROW(
         entrySize * entryCount + partitionStartOffset <= fileSize,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_PARAMETERS,
            "Partition entry file size too small: " + std::to_string(fileSize)
         )
      );
      for(uint32_t i = 0; i < entryCount; ++i)
      {
         PartitionEntry partitionEntry;
         partitionEntry.partitionIndex = partitionIndex;

         std::memcpy(
            reinterpret_cast<uint8_t*>(&partitionEntry.partitionEntryParameters),
            partitionTableData->begin() + partitionStartOffset + entrySize * i,
            TOOLS_SIZEOF(PartitionEntryParameters)
         );
         std::wstring partitionEntryNameUtf16;
         partitionEntryNameUtf16.resize(entrySize - TOOLS_SIZEOF(PartitionEntryParameters));
         std::memcpy(
            reinterpret_cast<uint8_t*>(&partitionEntryNameUtf16.at(0)),
            partitionTableData->begin() + partitionStartOffset + entrySize * i + TOOLS_SIZEOF(PartitionEntryParameters),
            entrySize - TOOLS_SIZEOF(PartitionEntryParameters)
         );
         partitionEntry.partitionEntryName = Util::fromWString(partitionEntryNameUtf16);
         partitionEntry.partitionEntryName.resize(Util::getStringLength(&partitionEntry.partitionEntryName.at(0)));
         partitionEntries.push_back(partitionEntry);
      }
   }
}

} // namespace Function

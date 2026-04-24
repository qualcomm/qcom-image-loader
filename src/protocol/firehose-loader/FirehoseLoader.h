// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "protocol/Base.h"
#include "protocol/firehose-loader/FhLoaderSha.h"
#include "protocol/Firehose.h"
#include "protocol/Fwd.h"
#include "util/Event.h"

#ifdef TOOLS_TARGET_WINDOWS
#include <Windows.h>
#endif
#include <array>
#include <chrono>
#include <deque>
#include <optional>
#include <stdint.h>
#include <time.h>

namespace Device {
namespace Protocol {

// ----------------------------------------------------------------------------
// FirehoseLoader Protocol
//
/// Interface for FirehoseLoader protocol
// ----------------------------------------------------------------------------
class FirehoseLoader : public Util::EventPublisher
{
   TOOLS_FORBID_COPY(FirehoseLoader);

public:
   FirehoseLoader(const FirehosePtr& pFirehose);
   virtual ~FirehoseLoader();

   int32_t executeCommand(Device::Protocol::Firehose::FirehoseCommandType& pParameters);
   void setRxTimeout(const std::optional<std::chrono::milliseconds>& timeout);
   void setReadPath(const std::filesystem::path& readImagePath);
   void clearSettings();
   void getStorageInfo(Device::Protocol::Firehose::StorageInfoType& storageInfo)
   {
      storageInfo = m_storageInfo;
   }
   std::string& getInternalErrorString(void)
   {
      return m_interalErrorString;
   }
   std::string& getDeviceErrorString(void)
   {
      return m_errorFromDevice;
   }


private:
// defines
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define FH_MAX_DEVICE_DELAY_FOR_POWER_OP 601

#define SPARSE_HEADER_MAGIC 0xed26ff3a
#define CHUNK_TYPE_RAW 0xCAC1
#define CHUNK_TYPE_FILL 0xCAC2
#define CHUNK_TYPE_DONT_CARE 0xCAC3
#define SPARSE_HEADER_MAJOR_VER 1
#define MAX_FILES_IN_OPTIONS 128

#define ONE_MEGABYTE 1048576
#define MAX_STRING_SIZE 2048
#define MAX_PATH_SIZE 2048
#define MAX_XML_SIZE (4 * 1024)
#define MAX_STRING_PAIR_SIZE (2 * 4096)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define XML_HEADER_LENGTH 41 // 40 does NOT include null
#define XML_TAIL_LENGTH 9    // 9 DOES include null

#define FIREHOSE_TEMP_BUFFER_SIZE 512
#define MAX_ATTR_NAME_SIZE 64
#define MAX_ATTR_RAW_SIZE 64

#define MAX_TAG_NAME_LENGTH 128

#define MAX_CONTENTS_XML_ENTRIES 300

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#define MAX_XML_FILES 8000
#define MAX_FILES_TO_TRACK 2000
#define FIREHOSE_TX_BUFFER_SIZE (2 * ONE_MEGABYTE)

#define PORT_TRACE_LOG_BUFFER_SIZE (ONE_MEGABYTE) // 1MB buffered to file
// #define PORT_TRACE_LOG_BUFFER_SIZE (50)       // 1MB buffered to file
#define MAX_READ_BUFFER_SIZE ONE_MEGABYTE

#define TEMP_LOG_SIZE 2048
#define MAX_TAG_ATTR_LENGTH 4096

#define PARTITION_ARRAY_SECTOR 2
#define PARTITION_SIZE_IN_BYTES 128 // same for 4K
#define PARTITION_TYPE_OFFSET_IN_BYTES 0
#define PARTITION_GUID_OFFSET_IN_BYTES 16
#define PARTITION_FLBA_OFFSET_IN_BYTES 32
#define PARTITION_LLBA_OFFSET_IN_BYTES 40
#define PARTITION_NAME_OFFSET_IN_BYTES 56

#ifdef TOOLS_MODE_DEBUG
#define PRETTYPRINT(buffer, length, MaxLength) printBuffer(buffer, length, MaxLength, __FUNCTION__, __LINE__)
#else
#define PRETTYPRINT(buffer, length, MaxLength)
#endif

#ifdef _MSC_VER
#define dbg(log_level, fmt, ...) MyLog(log_level, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#else
#define dbg(log_level, fmt...) MyLog(log_level, __FUNCTION__, __LINE__, fmt)
#endif

#define ELF_HEADER_SIZE 232
   uint8_t elf_header_first_96bytes[96] = {
      0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   };


   // enums
   enum LOG_TYPES
   {
      LOG_DEBUG, // usually only goes to log, unless VerboseLevel==LOG_DEBUG
      LOG_INFO,
      LOG_WARN,
      LOG_ERROR,
      LOG_ALWAYS,
      LOG_ONLY   // always only goes to log, such as hex dump
   };

   enum fh_digest_header_type
   {
      FH_DIGEST_HEADER_NONE,
      FH_DIGEST_HEADER_ELF,
      FH_DIGEST_HEADER_MBN,
      FH_DIGEST_HEADER_MAX,
   };
   enum fh_digest_header_type digest_header_type = FH_DIGEST_HEADER_MBN;

   typedef enum firehose_error_t
   {
      FIREHOSE_SUCCESS,
      FIREHOSE_ERROR,
      FIREHOSE_TRANSPORT_ERROR,
      FIREHOSE_STORAGE_ERROR,
      FIREHOSE_VALIDATION_ERROR
   } firehose_error_t;

   // typedefs
   typedef int64_t SIZE_T;
   typedef uint8_t boolean;

   struct timeval
   {
      int64_t tv_sec;
      int64_t tv_usec;
   };

   typedef firehose_error_t (FirehoseLoader::*TagHandlerFunc)();
   typedef std::map<std::string, TagHandlerFunc> FirehoseTagDataT;

   struct sparse_handle_t
   {
      uint32_t total_groups;
      uint16_t chunk_hdr_sz;
      // array for chunks of raw data
      uint64_t* start_sector;
      uint64_t* bytes_remaining;  // how many bytes left in group
      int64_t* group_file_offset; // file position of a chunk header
      int32_t chunk_header_idx;   // index into above array
   };

   struct sparse_header_t
   {
      uint32_t magic;          /* 0xed26ff3a */
      uint16_t major_version;  /* (0x1) - reject images with higher major versions */
      uint16_t minor_version;  /* (0x0) - allow images with higer minor versions */
      uint16_t file_hdr_sz;    /* 28 bytes for first revision of the file format */
      uint16_t chunk_hdr_sz;   /* 12 bytes for first revision of the file format */
      uint32_t blk_sz;         /* block size in bytes, must be a multiple of 4 (4096) */
      uint32_t total_blks;     /* total blocks in the non-sparse output
                                  imasector_size_in_bytesge */
      uint32_t total_chunks;   /* total chunks in the sparse input image */
      uint32_t image_checksum; /* CRC32 checksum of the original data, counting
                                  "don't care" */
      /* as 0. Standard 802.3 polynomial, use a Public Domain */
      /* table implementation */
   };

   struct chunk_header_t
   {
      uint16_t chunk_type; /* 0xCAC1 -> raw; 0xCAC2 -> fill; 0xCAC3 -> don't care */
      uint16_t reserved1;
      uint32_t chunk_sz; /* in blocks in output image */
      uint32_t total_sz; /* in bytes of chunk input file including chunk header
                            and data */
   };

   struct sparse_read_info_t
   {
      struct chunk_header_t chunk_header;
      uint64_t chunk_remaining;
      uint64_t bytes_remaining;
      boolean get_fill_chunk_value;
      uint32_t fill_chunk_value;
   };

   struct __sechsh_ctx_s context;
   struct __sechsh_ctx_s context_per_packet;

   struct Attributes_Struct
   {
      char Name[MAX_ATTR_NAME_SIZE]; // i.e. "MaxPayloadSizeToTargetInBytes"
      char Raw[1024];                // On PC this handles log values i.e. "8192"
      // char    *Raw;   // On PC this handles log values i.e. "8192"
      void* pValue;
      SIZE_T Min;
      SIZE_T Max;
      SIZE_T MultipleOf;
      SIZE_T SizeOfpStr;
      char* pStr;
      char Type;     // i.e. "i" for integer, "s" for string, "x" for special, "t"
                     // for SIZE_T
      char Assigned; // i.e. Flag indicating if it was just assigned
   };

   struct ContentsXML_Struct
   {
      SIZE_T Address;
      char Filename[MAX_ATTR_NAME_SIZE];
      char Path[MAX_PATH_SIZE];
      char Flavor[MAX_ATTR_NAME_SIZE];
      char StorageType; // 0=unknown,'e'=emmc,'u'=ufs,'n'=nand
      char FileType;    // 0=unknown,'r'=<partition_file
                        // (rawprogram),'p'=<partition_patch_file (patch)
   };

   struct ufs_extras_type
   {
      char bNumberLU, bBootEnable, bDescrAccessEn, bInitPowerMode, bHighPriorityLUN, bSecureRemovalType,
         bInitActiveICCLevel, bConfigDescrLock;
      int16_t wPeriodicRTCUpdate;
   };

   struct storage_extras_type
   {
      struct ufs_extras_type ufs_extras;
   };

   struct UFS_LUN_Var_Struct_type
   {
      char LUNum, bLUEnable, bBootLunID, bLUWriteProtect, bMemoryType, bDataReliability, bLogicalBlockSize,
         bProvisioningType;
      int16_t wPeriodicRTCUpdate, wContextCapabilities;
      SIZE_T size_in_KB;
   };

   typedef struct
   {
      SIZE_T MaxPayloadSizeToTargetInBytes; // numeric attributes
      SIZE_T
      MaxPayloadSizeToTargetInBytesSupported; // DDR version would typically
                                              // have this set to 1MB
      SIZE_T MaxPayloadSizeFromTargetInBytes;
      SIZE_T MaxDigestTableSizeInBytes;
      SIZE_T AckRawDataEveryNumPackets;
      SIZE_T delayinseconds;
      SIZE_T address64;
      SIZE_T value64;
      SIZE_T storagedrive;
      SIZE_T SECTOR_SIZE_IN_BYTES;
      SIZE_T byte_offset;
      SIZE_T physical_partition_number;
      SIZE_T size_in_bytes;
      SIZE_T file_sector_offset;

      SIZE_T ZlpAwareHost; // bool attributes but made SIZE_T.
      SIZE_T SkipWrite;    // otherwise the generic function that assigns values
      SIZE_T BufferWrites;
      // SIZE_T AckRawData;
      // SIZE_T display;
      // SIZE_T read_back_verify;
      SIZE_T
      AlwaysValidate; // will possibly overwrite the next address in memory
      SIZE_T Verbose;
      SIZE_T commit;
      SIZE_T trials;
      // SIZE_T display;
      SIZE_T TestWritePerformance;
      SIZE_T TestReadPerformance;
      SIZE_T TestDigestPerformance;
      // SIZE_T read_back_verify;
      SIZE_T SkipStorageInit;
      SIZE_T SkipSettingMinClocks;
      SIZE_T SkipSettingMaxClocks;
      SIZE_T actual_size_in_bytes;

      // SIZE_T start_sector;    // special attributes
      char start_sector[MAX_STRING_SIZE]; // special attributes
      SIZE_T num_partition_sectors;
      char filename[MAX_STRING_SIZE];
      char value[1024 * 1024]; // On PC, this line handles log messages char
                               // value[MAX_STRING_SIZE];

      char MemoryName[MAX_STRING_SIZE];
      char TargetName[MAX_STRING_SIZE];
      char Commit[MAX_STRING_SIZE]; // We have some commit='true', which
                                    // eventually goes to set fh.attrs.commit=1
      char sha256_verify[32 * 2 + 1];

      SIZE_T slot;

      // these are stored into first in preParseAttributes(), then
      // parseAttributes() fills in start_sector and num_partition_sectors
      // char start_sector_value[64];
      // char num_partition_sectors_value[64];

      // hotplug_dev_type storage_type;  // attribute MemoryName={"eMMC" or
      // "ufs"}
   } firehose_attrs_t;

   typedef struct
   {
      firehose_attrs_t attrs;
   } firehose_protocol_t;

   struct file_path
   {
      char* path; /* Full Path */
      char* name; /* Pointer to the file portion in the path below. */
   };

   struct file_list
   {
      char in_use;            /* Is this structure in use or not. */
      size_t count;           /* Total number of elements in the array. */
      size_t index;           /* Index of the next available entry to use. */
      struct file_path* list; /* List of possible paths to use */
   };

   // Functions
   size_t memscpy(void* dst, size_t dst_size, const void* src, size_t src_size);

   void notifyMessage(const FirehoseEvent::EventId eventId,
                      const char* eventString);     // callback
   void notifyProgression(const float progression); // callback
   int32_t processCommand(int32_t argc, char* argv[]);

   void addSlotAttribute(void);

   SIZE_T CopyString(
      char* Dest,
      const char* Source,
      SIZE_T Dstart,
      SIZE_T Sstart,
      SIZE_T length,
      SIZE_T DestSize,
      SIZE_T SourceSize
   );
   char* RemoveBannedChars(char* p);
   boolean MyParseExpression(char* buffer, SIZE_T BufferLength, SIZE_T* result);
   firehose_error_t handleStorageExtras();
   SIZE_T ParseComplicatedAttributes(void);

   int32_t MyCopyFile(char* FileNameSource, char* FileNameDest);
   SIZE_T stringToNumber(const char* buffer, boolean* retval);
   void CleanseSearchPaths(void);
   void ModifyTags(void);
   void LoadConfigureIntoTXBuffer(void);
   void PossiblyShowContentsXMLDifferentFileFoundWarning(char* CurrentPathAndFilenameFound);
   void CheckContentsXMLCompleteFileAndPath(char* filename); // fills contents_full_filename_with_path
   void ParseContentsXML(char* FileAndPath);
   void FindPartitionByLabel(SIZE_T LUN, char* LabelToMatch, char* Filename);
   void ReturnSizeString(uint64_t size, char* sz, uint64_t Length);
   int32_t IsASCIIString(char* sz);
   void PrepareDigestTable(void);
   void ShowCommandLineOptions(void);
   int32_t PartOfCommandLineOptions(char* sz);

   std::shared_ptr<std::fstream> fh_fopen(const char* filename, const char* mode);
   void fh_fclose(std::shared_ptr<std::fstream> pFile);

   std::shared_ptr<std::fstream> ReturnFileHandle(const char* filename, int64_t MaxLength, const char* AccessMode);
   int32_t AlreadyHaveThisFile(char* filePath);
   int32_t AlreadyHaveThisPath(char* path);
   int32_t ThisFileIsInFilterFiles(char* filename_only);
   int32_t ThisFileIsInNotFilterFiles(char* filename_only);
   int32_t HasAPathCharacter(const char* sz, SIZE_T Length);
   int32_t IsARelativePath(char* sz, SIZE_T Length);
   void StripNewLines(char* Buffer, SIZE_T BufferLength, SIZE_T Start, SIZE_T Length);
   void PrettyPrintHexValueIntoTempBuffer(uint8_t* temp_hash_val, int32_t length, int32_t offset, int32_t MaxLength);
   void SendSignedDigestTable(char* SignedDigestTableFilename);
   void TestIfWeNeedToSendDigestTable(void);
   void parseConfigXML(SIZE_T XMLFileSize, char* xml_buffer, size_t xml_buffer_size);
   int32_t operatorHigherPrecedenceThan(char left, char right);
   void ExitAndShowLog(uint32_t ExitCode);
   void display_error();
   void display_warning();

   int32_t ReturnNumAttributes(void);
   int32_t ReturnAttributeLocation(char* NewAttrName);
   size_t replace_xml_escape_chars(char* data, size_t size);
   int32_t AssignAttribute(char* NewAttrName, char* NewAttrValue, SIZE_T NewAttrValueLen);
   char* RemoveEverythingButTags(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T MaxPacketSize);
   void ResetAllAttributeAssignFlags(void);
   SIZE_T ReturnFileSize(std::shared_ptr<std::fstream> fd);
   void print_usage();
   void InitAttributes(void);
   void InitLogging(void);
   void MyLog(int32_t log_level, const char* func_name, int32_t line_number, const char* format, ...);
   char* find_file(char* filename, char ShowToScreen);
   void SendXMLString(char* sz, SIZE_T MaxLength);
   void StoreXMLFile(char* FileWithPath);

   SIZE_T SplitStringOnCommas(
      char* sz,
      SIZE_T SizeOfString,
      char* szArray[],
      int32_t offset,
      SIZE_T ArraySize,
      SIZE_T StringSize
   );
   double ReturnTimeInSeconds(struct timeval* start_time, struct timeval* end_time);
   int32_t gettimeofday(struct timeval* t, void* tzp);
   void time_throughput_calculate(
      struct timeval* start_time,
      struct timeval* end_time,
      SIZE_T size_bytes,
      double NetworkElapsed,
      enum LOG_TYPES ThisLogType
   );
   int32_t timeval_subtract(struct timeval* result, struct timeval* x, struct timeval* y);
   SIZE_T IsEmptySpace(char c);
   int32_t hex_atoi(const char* ascii, uint64_t* value);
   bool isOptionInList(const char* option, const std::vector<std::string>& list);
   void parseStorageInfo(const std::string& storageInfoStr);
   void parseEdmaAddressInfo(const std::string& edmaAddressInfoStr);
   void parseEdmaDataInfo(const std::string& edmaDataInfoStr);
   void trimSubStr(std::string& srcStr, const std::string& eraseStr);

   firehose_error_t GetNextPacket(void);
   firehose_error_t handleProgram();
   firehose_error_t handleBenchmark();
   firehose_error_t handleFirmwareWrite();
   firehose_error_t handleRead();
   firehose_error_t handleConfigure();
   firehose_error_t handlePower();
   firehose_error_t handleSetBootableStorageDrive();
   firehose_error_t handleLog();
   firehose_error_t handleNop();
   firehose_error_t handleErase();
   firehose_error_t handleUnrecognized();
   firehose_error_t handleOptions();
   firehose_error_t handleFileMappings();
   void TrimTrailingWhiteSpaceFromStringVariable(char* sz);
   firehose_error_t handleSearchPaths();
   firehose_error_t handleRawProgram();
   firehose_error_t handlePatchProgram();

   firehose_error_t handleResponse();
   void InitBufferWithXMLHeader(char* MyBuffer, SIZE_T Length);
   void AppendToBuffer(char* MyBuffer, const char* buf, SIZE_T MaxBufferSize);
   uint32_t sendTransmitBuffer(void);
   uint32_t sendTransmitBufferBytes(SIZE_T Length);
   // void SendConfigure(void);
   void LoadConfigureIntoStringTable(void);
   void SendReset(void);
   void LoadResetIntoStringTable(void);
   void SendXmlFiles(void);
   void SortMyXmlFiles(void);
   void OpenAndStoreAllXMLFiles(void);
   void ReadSha256File(void);

   enum fh_reboot_options
   {
      FH_REBOOT_NONE,
      FH_REBOOT_POWER_OFF,
      FH_REBOOT_EDL,   /* Reboot to EDL */
      FH_REBOOT_RESET, /* Reset. */
      FH_REBOOT_MAX,
   };

   void LoadPowerOpIntoStringTable(enum fh_reboot_options option, uint32_t seconds);

   struct sparse_handle_t*
   sparse_open(std::shared_ptr<std::fstream> fd, uint64_t current_start_sector, uint64_t sector_size_in_bytes);
   size_t sparse_read(
      char* data,
      size_t num_bytes,
      std::shared_ptr<std::fstream> fd,
      struct sparse_read_info_t* sparse_read_struct,
      struct sparse_header_t sparse_header
   );
   void sparse_close(struct sparse_handle_t*);
   struct sparse_header_t read_sparse_header(std::shared_ptr<std::fstream> fd);
   boolean TestIfSparse(std::shared_ptr<std::fstream> fd);
   void read_chunk_header(std::shared_ptr<std::fstream> fd, uint16_t chunk_hdr_sz, struct chunk_header_t* chunk_header);

   struct sparse_header_t sparse_read_handle_init(
      std::shared_ptr<std::fstream> fd,
      SIZE_T file_sector_offset,
      SIZE_T FileSize,
      struct sparse_read_info_t* sparse_read_handle
   );
   void GenerateSparseXMLTags(
      std::shared_ptr<std::fstream> fd,
      char* filename,
      SIZE_T start_sector,
      SIZE_T SECTOR_SIZE_IN_BYTES,
      SIZE_T physical_partition
   );

   uint32_t OpenPort(char* pData);
   void ClosePort(void);
   uint32_t WritePort(uint8_t* pData, uint32_t length, uint32_t MaxLength, uint8_t RawData);
   uint32_t ReadPort(uint8_t* pData, uint32_t length, uint32_t MaxLength);
   // void printBuffer(uint8_t *buffer, uint32_t length);
   void
   printBuffer(const uint8_t* buffer, uint32_t length, uint32_t MaxLength, const char* func_name, int32_t line_number);

   // void MyLog(int32_t log_level, const char *func_name, int32_t line_number,
   // const char *format, ...);

   int32_t getoptarg(int32_t i, char* argv[], char* opt, SIZE_T SizeOfOpt, char* arg, SIZE_T SizeOfArg);
   SIZE_T
   DetermineTag(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T MaxPacketSize);
   SIZE_T DetermineAttributes(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T MaxPacketSize);
   SIZE_T
   GetStringFromXML(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T PacketLength);

   void PerformSHA256(uint8_t* inputPtr, uint32_t inputLen, uint8_t* outputPtr);
   void LoadBuffer(char* MyBuffer, SIZE_T Length);
   void get_build_month(char* month);


   struct file_list* file_path_init(void);
   int32_t file_path_grow(struct file_list* list);
   char* file_path_search(struct file_list* list, char* name);
   int32_t file_path_add_path(struct file_list* list, char* path);
   int32_t file_path_count(struct file_list* list);
   int32_t file_path_destroy(struct file_list* list);
   void string_replace(char* in, size_t len, const char* find, const char* newStr);
   int32_t json_move_to_char(char* json, char next_c);
   int32_t json_add_xml_to_list(char* start, char* end);
   int32_t json_add_file_to_list(char* start, char* end);
   void metacli_json_in(char* json_path);
   int32_t add_string_to_xml_file_table(char* buffer);

   // Variables
   std::vector<std::string> CommandLineOptions;
   uint64_t COPY_BUF_SIZE; // 4gb max copy buf size
   char xml_header[XML_HEADER_LENGTH];
   char ShowXMLFileInLog = 0; // Controls wether DetermineTag shows the XML
                              // file, because I want to show HOST to TARGET

   /* List of variables provided for documentation*/
   /* xml_tail[XML_TAIL_LENGTH]     = "\n</data>"; */
   /* OPENING_DATA_TAG[] = "<data>"; */
   /* CLOSING_DATA_TAG[] = "</data>"; */
   /* CLOSING_CONFIGURATION_TAG[] = "</configuration>"; */
   /* OPENING_PATCHES_TAG[] = "<patches>"; */
   /* CLOSING_PATCHES_TAG[] = "</patches>"; */
   /* CLOSING_TAG[] = "/>"; */

   char LOAD_RAW_PROGRAM_FILES = 1;
   char LOAD_PATCH_PROGRAM_FILES = 1;

   char UsingValidation; // flag that controls VIP, i.e. if we send Digest
                         // Tables or not
   char ShowDigestPerPacket = 0, createcommandtrace = 0, showpercentagecomplete = 0, ShowDigestPerFile = 0;
   char ConvertProgram2Firmware = 0, ConvertProgram2Read = 0, forcecontentsxmlpaths = 0, verify_build = 0;

   char ThereWereErrors;

   char RemoveCommentsFromXMLFiles = 1;
   char ParseAttributes = 1; // this changes from 0 to 1 during the program

   float ComPortOpenTimeout = 3.0;

   char* cwd;
   char* MainOutputDir;
   char* SignedDigestTableFilename;
   char* ChainedDigestTableFilename;
   char* InFileListFilename;
   char* flattenbuildto;
   char* flavor;
   char* flattenbuildvariant; // such as eMMC, UFS etc

   int64_t DeviceProgrammerIndex = -1;

   char* MyArg;
   char* MyOpt; // Command line args
   char* MyStringPairs;

   SIZE_T ThisXMLLength; // Updated by RemoveEverythingButTags()
   SIZE_T TotalTransferSize = 0,
          BuildSizeTransferSoFar = 0; // sum up all the transfers (reads and writes)
   float PercentageBuildLoaded = 0.0;
   struct timeval fh_loader_start, fh_loader_end;

   SIZE_T NumTries = 1000;

   // firehose_error_t GetNextPacket(void);

   char* ConfigXML; // used for --xml=firehose_config.xml
   char* port_name;
   SIZE_T SectorSizeInBytes = 512; // fh.attrs.SECTOR_SIZE_IN_BYTES updated on every RawProgram.xml
                                   // file, so compare to this
   char GotACK = 0, stresstest = 0;
   char AllowReset = 0; // user must specify --reset now
   char PromptUser, SortTags;

   char* XMLFileTable[MAX_XML_FILES];       // --sendxml=command1.xml,command2.xml,etc
   char* search_path[MAX_XML_FILES];        // --search_path=command1.xml,command2.xml,etc
   char* filter_files[MAX_XML_FILES];       // --files=sbl1.mbn,tz.mbn
   char* filter_not_files[MAX_XML_FILES];   // --notfiles=sbl1.mbn,tz.mbn
   char* XMLStringTable[MAX_XML_FILES];     // --sendxml=command1.xml,command2.xml,etc
   char* XMLStringTableTemp[MAX_XML_FILES]; // --sendxml=command1.xml,command2.xml,etc
   SIZE_T NumXMLFilesInTable = 1;           // start at 1 since <configure> will go at 0

   char* MaxFilesToTrack[MAX_FILES_TO_TRACK]; // --search_path=command1.xml,command2.xml,etc
   SIZE_T FileToTrackCount = 0;

   char* PortTraceName;
   char* DigestsPerFileName;
   char* CommandTraceName;
   char* ConfigXMLName;

   char* full_filename_with_path;
   char* contents_full_filename_with_path;
   SIZE_T num_search_paths = 0, num_xml_files_to_send = 0, num_filter_files = 0, num_filter_not_files = 0;

   std::shared_ptr<std::fstream> fp;  // for port_trace.txt
   std::shared_ptr<std::fstream> fc;  // for command_trace.txt
   std::shared_ptr<std::fstream> ft;  // for DIGEST_TABLE.bin
   std::shared_ptr<std::fstream> fg;  // for Config.xml from --createconfigxml
   std::shared_ptr<std::fstream> fdp; // for DigestsPerFile.txt

   char RawMode = 0; // 0 means XML files from target, 1 means RAW packets as
                     // in a <read> command
   char testvipimpact = 0;

   char* tx_buffer;
   char* tx_buffer_backup;
   char* temp_buffer;
   char* temp_buffer2;
   char* temp_buffer3;
   char* last_log_value;

   // char PortTraceLogBuffer[PORT_TRACE_LOG_BUFFER_SIZE];
   // SIZE_T PortTraceLogBufferStart = 0, PortTraceLogBufferEnd=0;

   // char tx_buffer2[FIREHOSE_TX_BUFFER_SIZE+FIREHOSE_TX_BUFFER_SIZE];

   SIZE_T BytesWritten = 0;
   SIZE_T BytesRead = 0; // How much USB data we read

   SIZE_T PacketsSent = 0;
   SIZE_T CurrentDigestLocation = 0;
   SIZE_T digest_file_offset = 0;
   SIZE_T NumDigestsFound = 0;
   SIZE_T DigestSizeInBytes = 32;
   SIZE_T MaxNumDigestsPerTable = 0;
   SIZE_T sha256_file_offset = 0;

   char verify_programming = 0;
   char verify_programming_sha256 = 0;
   char GenerateSha256File = 0;
   char VerifySha256File = 0;

   uint8_t temp_hash_value[32];
   uint8_t verify_hash_value[32];
   uint8_t last_hash_value[32];
   SIZE_T SizeOfDataFedToHashRoutine = 0;

   struct ContentsXML_Struct* ContensXML;
   struct ContentsXML_Struct* ContensXMLPath;
   SIZE_T NumContensXML = 0, NumContentsXMLPath = 0;

   struct storage_extras_type storage_extras;

   struct UFS_LUN_Var_Struct_type UFS_LUN_Var_Struct;

   firehose_protocol_t fh;

   // struct Attributes_Struct AllAttributes[54];
   Attributes_Struct* AllAttributes;

#ifdef _MSC_VER // i.e. if compiling under Windows
   HANDLE port_fd;
#else
   int32_t port_fd = 0;
#endif

   int32_t ret = 0;

   SIZE_T MaxBytesToReadFromUSB = MAX_READ_BUFFER_SIZE; // this is changed to
                                                        // fh.attrs.MaxPayloadSizeFromTargetInBytes after
                                                        // handleConfigure()

   char* ReadBuffer;
   int64_t CharsInBuffer = 0, PacketLoc = 0, PacketStart = 0;

   std::string WarningsBuffer;
   SIZE_T NumWarnings;

   uint8_t VerboseLevel, PrettyPrintRawPacketsToo, ReadBogusDataBeforeSendingConfigure;
   uint8_t Simulate, SimulateBack, Interactive, skipharddriveread, CreateDigests, Verbose, wipefirst, erasefirst,
      FlattenBuild, SimulateForFileSize, createconfigxml;
   uint8_t ForceOverwrite = 0;
   char* WipeFirstFileName = const_cast<char*>("Zeros16KB.bin");
   char* StressTestFileName = const_cast<char*>("Zeros1GB.bin");

   SIZE_T LastFindFileFileSize = 0;

   char NewTagName[MAX_TAG_NAME_LENGTH + 1];

   FirehoseTagDataT firehose_tag_data;
   TagHandlerFunc CurrentHandlerFunction; // this will always point to latest TAG function,
                                          // i.e. handleConfigure()

   Util::CheckedPointer<Firehose> m_pFirehose;
   int32_t m_errorCode;
   Device::Protocol::Firehose::StorageInfoType m_storageInfo;
   struct file_list* global_file_path_list = NULL;
   std::string m_interalErrorString;
   std::string m_errorFromDevice;
   std::string m_edmaFile;
   std::optional<std::chrono::milliseconds> m_rxTimeoutInMs;
   char ValidateImageSize = 0;


   float PrePercentageBuildLoaded = 0.0;
};

} // namespace Protocol
} // namespace Device

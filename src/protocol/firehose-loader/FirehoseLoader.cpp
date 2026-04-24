// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
/****************************************************************************
=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header:
//source/qcom/qct/core/storage/tools/fh_loader/vs2010/fh_loader/fh_loader.c#114
$ $DateTime: 2021/06/22 23:24:34 $ $Author: wkimberl $

when        who  what, where, why
----------  ---  ---------------------------------------------------------
2021-05-27  as   Add support for CHUNK_TYPE_FILL type for sparse images.
2021-05-25  wek  Make the read back verify code use output directory.
2021-03-10  wek  Include the slot when using verify_programming.
2021-03-10  wek  Look in the command line for the header to use for VIP.
2021-02-24  wek  Yield success when output directory does not exist beforehand.
2021-01-21  cm   Add logic to remove compile time warnings.
2021-01-07  wek  On contents.xml parsing fail gracefully if end of file_path.
2020-10-20  sk   Update the output directory with readback only on failure.
2020-09-28  wek  Delete define that re-maps memscpy.
2020-06-12  wek  Add ./ as the default main output directory.
2020-06-05  wek  Pad the first table of hashes to 54 regardless of size.
2020-05-30  wek  At digest creation compute the right number of tables.
2020-05-29  wek  Terminate XML string that checks for sparse image.
2020-05-27  wek  Remove handling of Windows FFU image.
2020-05-22  wek  Implement skip of VIP for transport layer when not using layer
2020-04-29  sk   Correct calculation of readback, overwriting original images
2020-01-29  wek  Include option to power off or reboot the device.
2020-01-28  gp   Add slot support to fh_loader command line.
2020-01-07  wek  Enhance fh_loader by making modules of transport layers.
2019-07-18  wek  Made read port not print error in linux on select timeout.
2019-06-07  wek  Make it possible to use on target SHA256 to verify flashing
2019-05-31  wek  Add deprecated messages to deprecated arguments.
2019-05-15  wek  Create a log header file to handle logs from all modules.
2019-05-02  wek  List NVMe as a new memory type.
2018-09-26  wek  Add initalization of file list array.
2018-04-04  wek  Increase the max number of supported XML entries.
2018-04-04  wek  Revise how throughput is calculated.
2018-02-16  wek  Enlarge the buffer sizes used for logs from target.
2018-02-14  wek  Verify that allocated pointers are properly casted.
2018-01-15  wek  Implement getting alternative build flavor.
2017-12-09  wek  Validate input parameter for the list of file helper.
2017-12-08  wek  Implement parsing meta_cli output JSON path data.
2017-11-16  wek  Add the root path to the file path when parsing contents.xml
2017-10-30  wek  New handling of invalid lun/labels to return error early.
2017-07-24  wek  Solve issue seen on VIP where file data was considered XML.
2017-07-21  wek  Add implementation of fixgpt.
2017-07-20  wek  Repair bug of trying to open cwd instead of the file.
2017-06-08  wek  Add checks when adding a command to make sure it fits
2017-04-24  wek  Write the handling logic for labels and notlabels.
2017-03-10  wek  Improve throughput printout for larger than 4GB builds.
2017-03-10  wek  Look for special character to allow absolute paths.
2016-05-25  wek  Lighten the memory load by adding a feature for debug code
2016-05-25  wek  Improve memory usage by freeing the memory in StoreXMLFile
2016-05-23  wek  Add a static base version to know the original source.
2016-05-23  wek  Make the port timeout 20ms to improve small transfer times.
2016-05-23  wek  Kill bug of keeping files open in handle program.
2016-05-05  wek  Introduce a fix to close file handle when calculating size.
2016-05-02  wek  Make sure all allocated memory in sparse open is free.
2016-04-13  wek  Bond the use of size_t format when printing hashes.
2016-04-12  wek  Enhance reading of data when buffer less than sector size
2016-04-07  wek  Remove bug on length for copy string in XML file path.
2016-04-06  wek  Last set of banned APIs were removed.
2016-04-05  wek  Yield to path given in search_path instead of contentsxml
2016-04-01  wek  Remove banned API replaced by snprintf.
2016-03-29  wek  Remove the need for an Entire XML buffer and its macros.
2016-03-10  wek  Handle XML escape characters in attribute values.
2016-03-10  wek  Support SPI-NOR storage type in contents.xml
2016-02-03  wek  Return a non zero value on error.
2016-01-29  wek  Skip over files that have ignore to true on contents.xml
2016-01-28  wek  Fix the number of hashes of the first VIP table to 54.
2016-01-27  wek  Increase the maximum size of the input XML file
2016-01-27  wek  Start sector from the end of the device using negative numbers
2016-01-27  wek  Report error on NAK for unrecognized command.
2016-01-14  wek  Move all the SHA functions to a spearate file.
2015-12-02  wek  Remove strtoull, not supported on VS2010.
2015-09-29  wek  Increase the max number of Commands/XMLs that can be sent.
2015-08-18  ky   flavor no longer necessary in contentsxml
2015-08-13  ky   Showpercentagecomplete works with flattenbuild
2015-08-10  ky   Don't copy over existing flattenbuild files unless
forceoverwrite 2015-08-07  ky   Redirect all output files to MainOutputDir
option 2015-08-06  ky   Added large file support 2015-07-21  ky   Added windows
FFU image support. 2015-07-02  wek  Fix error of opening NULL file for sparse
files. 2015-07-02  wek  Removing uncessesary memset. 2015-06-18  ky   Added
sparse image support. 2015-06-03  wek  Fix printing wrong build size for large
builds. 2015-06-02  wek  Accept upper case UFS from the command line argument.
2015-06-02  wek  Handle VIP tables that are multiple of 512 bytes.
2015-05-29  wek  Fix printing wrong percentage complete on large builds.
2015-05-27  wek  Fix the sector size for firmware update from the command line.
2015-05-26  ah   Do wipe first only for program tag.
2015-05-20  wek  Fix firmware update from the command line.
2015-05-20  wek  Fix double printing entries in the output digest file.
2015-05-15  wek  Print version, the version is same as date.
2015-05-15  wek  Print out the percentage when reading from the device.
2015-05-14  wek  Copy the device programmer binary when flattening build.
2015-05-06  wek  Change how the percentage completed is printed.
2015-05-05  wek  Move populating variable after the log name is known.
2015-05-04  wek  Run the soruce through a beautifier.
2015-04-22  ah   Build flattening improved
2015-04-12  ah   Build flattening based on flavor added
2015-03-27  ah   COM port hanging is corrected
2015-03-27  ah   Various features added
2015-03-24  ah   Various features added
2015-03-06  ah   Initial release

=============================================================================

O: maps to \\corebsp-tst-173\c$\preflight\builds\test_job_1\8994

Get driver working on Linux
sudo rmmod qcserial
sudo rmmod usbserial
sudo modprobe usbserial vendor=0x5c6 product=0x9008

After this
ahughes@ahughes-laptop-dell:~/programming/fh_loader$ ls /dev/ttyU*
/dev/ttyUSB0

Get DeviceProgrammer file over to Kickstart
./QSaharaServer -p /dev/ttyUSB0 -s 13:prog_firehose_ddr.bin

To build in Linux:
gcc fh_loader.c fh_loader_sha.c fh_cobs.c fh_crc.c fh_hsuart_packet.c
fh_transfer.c fh_transport.c fh_transport_com.c fh_transport_linux_pipe.c
fh_transport_hsuart.c stringl/memscpy.c stringl/memsmove.c stringl/strlcat.c
stringl/strlcpy.c -o fh_loader -lrt -I stringl/

./fh_loader --port=/dev/ttyUSB0 --sendimage=big.bin

****************************************************************************/

#include "protocol/firehose-loader/FirehoseLoader.h"

#include "device/Buffer.h"
#include "device/Manager.h"
#include "protocol/Base.h"
#include "protocol/Firehose.h"
#include "util/StringHelper.h"

#include <cstdio>
#include <iomanip>
#include <sstream>
#ifdef _MSC_VER // i.e. if compiling under Windows
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#define GETCWD _getcwd
#define ZLPAWAREHOST 1
#define SLASH '\\' // defined differently below for LINUX
#define SLASHSTR "\\"
#define WRONGSLASH '/'
#define O_RDWR _O_RDWR
#define O_SYNC _O_SEQUENTIAL
#define sleep(x) Sleep(x * 1000)

#define SIZE_T_FORMAT                                                                                                  \
   "lld" // Use in middle of string "Channel read " SIZE_T_FORMAT " bytes", at
         // end of string "num_physical_partitions="SIZE_T_FORMAT
#define SIZE_T_FORMAT4 ".4lld"
#define SIZE_T_FORMAT5 ".5lld"
#define fseek _fseeki64
#define ftell _ftelli64
#define strtok_x strtok_s
#define snprintf_x _snprintf
#define fileno _fileno

#else
#include <errno.h>
#include <fcntl.h> // for open
#include <termios.h>
#include <unistd.h> // for close
#define ZLPAWAREHOST 1
#define SLASH '/'
#define SLASHSTR "/"
#define WRONGSLASH '\\'
#define GETCWD getcwd
#define SIZE_T_FORMAT                                                                                                  \
   "ld" // Use in middle of string "Channel read " SIZE_T_FORMAT " bytes", at
        // end of string "num_physical_partitions="SIZE_T_FORMAT
#define SIZE_T_FORMAT4 ".4ld"
#define SIZE_T_FORMAT5 ".5ld"
#define _stat64 stat
#define strtok_x strtok_r
#define snprintf_x snprintf

#endif

#define BACKSLASH 92
#define FORWARDSLASH 47
#define COMMAND_TRACE_BYTES_TO_RECORD 128

#include "protocol/firehose-loader/FhLoaderSha.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE(msg) FLOG_ERROR(msg);

#define FIREHOSE_LOADER_PORT_TRACE_DEBUG_MESSAGE(msg) FLOG_DEBUG(msg);

#define FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE(msg) FLOG_WARNING(msg);

#define FIREHOSE_LOADER_PORT_TRACE_LOG_MESSAGE(msg) FLOG_INFO(msg);

// Added for storage info parsing. Not present in core firehose loader code
static const char FIREHOSE_PREFIX_STORAGE_LOG_STR[] = "\"storage_info\"";
static const char FIREHOSE_PREFIX_EDMA_ADDRESS_LOG_STR[] = "Using address ";
static const char FIREHOSE_PREFIX_EDMA_DATA_LOG_STR[] = "0x";
static const char FIREHOSE_DEVICE_ERROR_STR[] = "ERROR:";
static const float FLOAT32_EPSINON = 0.000001f;

namespace Device {
namespace Protocol {

static const uint64_t NUM_ATTRIBUTES = 55;

// ----------------------------------------------------------------------------
// FirehoseLoader
//
// ----------------------------------------------------------------------------
FirehoseLoader::FirehoseLoader(const FirehosePtr& pFirehose)
: m_pFirehose(pFirehose)
, m_errorCode(0)
, COPY_BUF_SIZE((4ULL * 1024 * 1024 * 1024) - 1)
, ShowXMLFileInLog(0)
, LOAD_RAW_PROGRAM_FILES(1)
, LOAD_PATCH_PROGRAM_FILES(1)
, UsingValidation(0)
, ShowDigestPerPacket(0)
, createcommandtrace(0)
, showpercentagecomplete(0)
, ShowDigestPerFile(0)
, ConvertProgram2Firmware(0)
, ConvertProgram2Read(0)
, forcecontentsxmlpaths(0)
, verify_build(0)
, ThereWereErrors(0)
, RemoveCommentsFromXMLFiles(1)
, ParseAttributes(1)
, ComPortOpenTimeout(3.0)
, cwd(new char[MAX_PATH_SIZE])
, MainOutputDir(new char[MAX_PATH_SIZE])
, SignedDigestTableFilename(new char[MAX_STRING_SIZE])
, ChainedDigestTableFilename(new char[MAX_STRING_SIZE])
, InFileListFilename(new char[MAX_STRING_SIZE])
, flattenbuildto(new char[MAX_STRING_SIZE])
, flavor(new char[MAX_STRING_SIZE])
, flattenbuildvariant(new char[MAX_STRING_SIZE])
, DeviceProgrammerIndex(-1)
, MyArg(new char[MAX_STRING_SIZE])
, MyOpt(new char[MAX_STRING_SIZE])
, MyStringPairs(new char[MAX_STRING_PAIR_SIZE])
, ThisXMLLength()
, TotalTransferSize(0)
, BuildSizeTransferSoFar(0)
, PercentageBuildLoaded(0.0)
, fh_loader_start()
, fh_loader_end()
, NumTries(1000)
, ConfigXML(new char[MAX_PATH_SIZE])
, port_name(new char[MAX_STRING_SIZE])
, SectorSizeInBytes(512)
, GotACK(0)
, stresstest(0)
, AllowReset(0)
, PromptUser(1)
, SortTags(1)
, XMLFileTable()
, search_path()
, filter_files()
, filter_not_files()
, XMLStringTable()
, XMLStringTableTemp()
, NumXMLFilesInTable(1)
, MaxFilesToTrack()
, FileToTrackCount(0)
, PortTraceName(new char[MAX_PATH_SIZE])
, DigestsPerFileName(new char[MAX_PATH_SIZE])
, CommandTraceName(new char[MAX_PATH_SIZE])
, ConfigXMLName(new char[MAX_PATH_SIZE])
, full_filename_with_path(new char[MAX_PATH_SIZE])
, contents_full_filename_with_path(new char[MAX_PATH_SIZE])
, num_search_paths(0)
, num_xml_files_to_send(0)
, num_filter_files(0)
, num_filter_not_files(0)
, fp(nullptr)
, fc(nullptr)
, ft(nullptr)
, fg(nullptr)
, fdp(nullptr)
, RawMode(0)
, testvipimpact(0)
, tx_buffer(new char[FIREHOSE_TX_BUFFER_SIZE])
, tx_buffer_backup(new char[FIREHOSE_TX_BUFFER_SIZE])
, temp_buffer(new char[FIREHOSE_TX_BUFFER_SIZE])
, temp_buffer2(new char[FIREHOSE_TX_BUFFER_SIZE])
, temp_buffer3(new char[FIREHOSE_TX_BUFFER_SIZE])
, last_log_value(new char[FIREHOSE_TX_BUFFER_SIZE])
, BytesWritten(0)
, BytesRead(0)
, PacketsSent(0)
, CurrentDigestLocation(0)
, digest_file_offset(0)
, NumDigestsFound(0)
, DigestSizeInBytes(32)
, MaxNumDigestsPerTable(0)
, verify_programming(0)
, verify_programming_sha256(0)
, temp_hash_value()
, verify_hash_value()
, last_hash_value()
, SizeOfDataFedToHashRoutine(0)
, ContensXML(new ContentsXML_Struct[MAX_CONTENTS_XML_ENTRIES])
, ContensXMLPath(new ContentsXML_Struct[MAX_CONTENTS_XML_ENTRIES])
, NumContensXML(0)
, NumContentsXMLPath(0)
, storage_extras()
, UFS_LUN_Var_Struct()
, fh()
, port_fd(0)
, ret(0)
, MaxBytesToReadFromUSB(MAX_READ_BUFFER_SIZE)
, ReadBuffer(new char[MAX_READ_BUFFER_SIZE])
, CharsInBuffer(0)
, PacketLoc(0)
, PacketStart(0)
, WarningsBuffer("")
, NumWarnings(0)
, VerboseLevel()
, PrettyPrintRawPacketsToo(0)
, ReadBogusDataBeforeSendingConfigure(0)
, Simulate(0)
, SimulateBack(0)
, Interactive(0)
, skipharddriveread(0)
, CreateDigests(0)
, Verbose(0)
, wipefirst(0)
, erasefirst(0)
, FlattenBuild(0)
, SimulateForFileSize(0)
, createconfigxml(0)
, ForceOverwrite(0)
, WipeFirstFileName(const_cast<char*>("Zeros16KB.bin"))
, StressTestFileName(const_cast<char*>("Zeros1GB.bin"))
, LastFindFileFileSize(0)
, NewTagName()
, CommandLineOptions(
     {"benchmarkddr",
      "benchmarkdigestperformance",
      "benchmarkreads",
      "benchmarkwrites",
      "createvipdigests=",
      "chaineddigests=",
      "contentsxml=",
      "convertprogram2read",
      "digestsperfilename=",
      "erase=",
      "files=",
      "firmwarewrite",
      "fixgpt=",
      "flattenbuildto=",
      "flavor=",
      "forcecontentsxmlpaths",
      "getstorageinfo=",
      "json_in=",
      "labels=",
      "loglevel=",
      "lun=",
      "mainoutputdir=",
      "maxpayloadsizeinbytes=",
      "memoryname=",
      "notfiles=",
      "notlabels=",
      "nop",
      "noprompt",
      "num_sectors=",
      "port=",
      "porttracename=",
      "port_type=",
      "power=",
      "search_path=",
      "sectorsizeinbytes=",
      "sendimage=",
      "sendxml=",
      "setactivepartition=",
      "showpercentagecomplete",
      "signeddigests=",
      "slot=",
      "start_sector=",
      "verbose",
      "verify_programming_getsha",
      "verify_programming",
      "generatesha256file",
      "verifysha256file",
      "zlpawarehost=",
      "validate_image_size",
      "maxreadpayloadsizeinbytes=",
      "excludeerasepartitioninfo="}
  )
, firehose_tag_data()
, CurrentHandlerFunction()
, m_interalErrorString()
, m_errorFromDevice()
, m_rxTimeoutInMs(std::chrono::milliseconds(0))
, ValidateImageSize(0)
{
   for(uint64_t i = 0; MAX_XML_FILES > i; ++i)
   {
      XMLFileTable[i] = new char[MAX_PATH_SIZE];
      search_path[i] = new char[MAX_PATH_SIZE];
      filter_files[i] = new char[MAX_PATH_SIZE];
      filter_not_files[i] = new char[MAX_PATH_SIZE];
      XMLStringTable[i] = new char[MAX_XML_SIZE];
      XMLStringTableTemp[i] = new char[MAX_XML_SIZE];
   }
   for(uint64_t i = 0; MAX_FILES_TO_TRACK > i; ++i)
   {
      MaxFilesToTrack[i] = new char[MAX_PATH_SIZE];
   }

   std::strcpy(PortTraceName, "port_trace.txt");
   std::strcpy(DigestsPerFileName, "DigestsPerFile.txt");
   std::strcpy(CommandTraceName, "command_trace.txt");
   std::strcpy(ConfigXMLName, "config_");
   std::strcpy(xml_header, "<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n");

   AllAttributes = new Attributes_Struct[NUM_ATTRIBUTES];
   Attributes_Struct* attributes = &AllAttributes[0];
   *attributes++ = Attributes_Struct{"Verbose", "", (SIZE_T*)&fh.attrs.Verbose, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{
      "MaxPayloadSizeToTargetInBytes",
      "",
      (SIZE_T*)&fh.attrs.MaxPayloadSizeToTargetInBytes,
      0,
      0,
      512,
      0,
      NULL,
      'i',
      0
   };
   *attributes++ = Attributes_Struct{
      "MaxPayloadSizeToTargetInBytesSupported",
      "",
      (SIZE_T*)&fh.attrs.MaxPayloadSizeToTargetInBytesSupported,
      0,
      0,
      512,
      0,
      NULL,
      'i',
      0
   };
   *attributes++ = Attributes_Struct{
      "MaxPayloadSizeFromTargetInBytes",
      "",
      (SIZE_T*)&fh.attrs.MaxPayloadSizeFromTargetInBytes,
      0,
      0,
      512,
      0,
      NULL,
      'i',
      0
   };
   *attributes++ = Attributes_Struct{
      "MaxDigestTableSizeInBytes",
      "",
      (SIZE_T*)&fh.attrs.MaxDigestTableSizeInBytes,
      0,
      0,
      512,
      0,
      NULL,
      'i',
      0
   };
   *attributes++ = Attributes_Struct{
      "AckRawDataEveryNumPackets",
      "",
      (SIZE_T*)&fh.attrs.AckRawDataEveryNumPackets,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   };
   *attributes++ = Attributes_Struct{"delayinseconds", "", (SIZE_T*)&fh.attrs.delayinseconds, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{"address64", "", (SIZE_T*)&fh.attrs.address64, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{"value64", "", (SIZE_T*)&fh.attrs.value64, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ =
      Attributes_Struct{"storagedrive", "", (SIZE_T*)&fh.attrs.physical_partition_number, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ =
      Attributes_Struct{"SECTOR_SIZE_IN_BYTES", "", (SIZE_T*)&fh.attrs.SECTOR_SIZE_IN_BYTES, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{"byte_offset", "", (SIZE_T*)&fh.attrs.byte_offset, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{
      "physical_partition_number",
      "",
      (SIZE_T*)&fh.attrs.physical_partition_number,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   };
   *attributes++ = Attributes_Struct{"size_in_bytes", "", (SIZE_T*)&fh.attrs.size_in_bytes, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{
      "num_partition_sectors",
      "",
      (SIZE_T*)&fh.attrs.num_partition_sectors,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   };
   *attributes++ =
      Attributes_Struct{"file_sector_offset", "", (SIZE_T*)&fh.attrs.file_sector_offset, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{"trials", "", (SIZE_T*)&fh.attrs.trials, 0, 0, 1, 0, NULL, 'i', 0}; // 16
   *attributes++ = Attributes_Struct{"ZlpAwareHost", "", (SIZE_T*)&fh.attrs.ZlpAwareHost, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{"SkipWrite", "", (SIZE_T*)&fh.attrs.SkipWrite, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{"BufferWrites", "", (SIZE_T*)&fh.attrs.BufferWrites, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{"AlwaysValidate", "", (SIZE_T*)&fh.attrs.AlwaysValidate, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{
      "commit",
      "",
      NULL,
      0,
      0,
      1,
      sizeof(fh.attrs.Commit),
      (char*)fh.attrs.Commit,
      'x',
      0
   }; // string convert to fh.attrs.commit
   *attributes++ =
      Attributes_Struct{"TestWritePerformance", "", (SIZE_T*)&fh.attrs.TestWritePerformance, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ =
      Attributes_Struct{"TestReadPerformance", "", (SIZE_T*)&fh.attrs.TestReadPerformance, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{
      "TestDigestPerformance",
      "",
      (SIZE_T*)&fh.attrs.TestDigestPerformance,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   }; // 24
   *attributes++ = Attributes_Struct{
      "SkipStorageInit",
      "",
      (SIZE_T*)&fh.attrs.SkipStorageInit,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   }; // <configure SkipStorageInit="1"
   *attributes++ = Attributes_Struct{
      "SkipSettingMinClocks",
      "",
      (SIZE_T*)&fh.attrs.SkipSettingMinClocks,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   }; // <configure SkipSettingMinClocks="1"
   *attributes++ = Attributes_Struct{
      "SkipSettingMaxClocks",
      "",
      (SIZE_T*)&fh.attrs.SkipSettingMaxClocks,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   }; // <configure SkipSettingMaxClocks="1"
   *attributes++ = Attributes_Struct{
      "actual_size_in_bytes",
      "",
      (SIZE_T*)&fh.attrs.actual_size_in_bytes,
      0,
      0,
      1,
      0,
      NULL,
      'i',
      0
   }; // <configure actual_size_in_bytes="1234"
   *attributes++ = Attributes_Struct{"slot", "", (SIZE_T*)&fh.attrs.slot, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ =
      Attributes_Struct{"bNumberLU", "", (uint8_t*)&storage_extras.ufs_extras.bNumberLU, 0, 0, 1, 0, NULL, 'b', 0};
   *attributes++ =
      Attributes_Struct{"bBootEnable", "", (uint8_t*)&storage_extras.ufs_extras.bBootEnable, 0, 0, 1, 0, NULL, 'b', 0};
   *attributes++ = Attributes_Struct{
      "bDescrAccessEn",
      "",
      (uint8_t*)&storage_extras.ufs_extras.bDescrAccessEn,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "bInitPowerMode",
      "",
      (uint8_t*)&storage_extras.ufs_extras.bInitPowerMode,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "bHighPriorityLUN",
      "",
      (uint8_t*)&storage_extras.ufs_extras.bHighPriorityLUN,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "bSecureRemovalType",
      "",
      (uint8_t*)&storage_extras.ufs_extras.bSecureRemovalType,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "bInitActiveICCLevel",
      "",
      (uint8_t*)&storage_extras.ufs_extras.bInitActiveICCLevel,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "wPeriodicRTCUpdate",
      "",
      (int16_t*)&storage_extras.ufs_extras.wPeriodicRTCUpdate,
      0,
      0,
      1,
      0,
      NULL,
      'n',
      0
   };
   *attributes++ = Attributes_Struct{
      "bConfigDescrLock",
      "",
      (uint8_t*)&storage_extras.ufs_extras.bConfigDescrLock,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   }; // 37
   *attributes++ = Attributes_Struct{"LUNum", "", (uint8_t*)&UFS_LUN_Var_Struct.LUNum, 0, 7, 1, 0, NULL, 'i', 0};
   *attributes++ =
      Attributes_Struct{"bLUEnable", "", (uint8_t*)&UFS_LUN_Var_Struct.bLUEnable, 0, 0, 1, 0, NULL, 'b', 0};
   *attributes++ =
      Attributes_Struct{"bBootLunID", "", (uint8_t*)&UFS_LUN_Var_Struct.bBootLunID, 0, 0, 1, 0, NULL, 'b', 0};
   *attributes++ =
      Attributes_Struct{"bLUWriteProtect", "", (uint8_t*)&UFS_LUN_Var_Struct.bLUWriteProtect, 0, 0, 1, 0, NULL, 'b', 0};
   *attributes++ =
      Attributes_Struct{"bMemoryType", "", (uint8_t*)&UFS_LUN_Var_Struct.bMemoryType, 0, 0, 1, 0, NULL, 'b', 0};
   *attributes++ =
      Attributes_Struct{"size_in_KB", "", (SIZE_T*)&UFS_LUN_Var_Struct.size_in_KB, 0, 0, 1, 0, NULL, 'i', 0};
   *attributes++ = Attributes_Struct{
      "bDataReliability",
      "",
      (uint8_t*)&UFS_LUN_Var_Struct.bDataReliability,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "bLogicalBlockSize",
      "",
      (uint8_t*)&UFS_LUN_Var_Struct.bLogicalBlockSize,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "bProvisioningType",
      "",
      (uint8_t*)&UFS_LUN_Var_Struct.bProvisioningType,
      0,
      0,
      1,
      0,
      NULL,
      'b',
      0
   };
   *attributes++ = Attributes_Struct{
      "wContextCapabilities",
      "",
      (int16_t*)&UFS_LUN_Var_Struct.wContextCapabilities,
      0,
      0,
      1,
      0,
      NULL,
      'n',
      0
   }; // 47
   *attributes++ = Attributes_Struct{
      "MemoryName",
      "",
      NULL,
      0,
      0,
      1,
      sizeof(fh.attrs.MemoryName),
      (char*)fh.attrs.MemoryName,
      's',
      0
   };
   *attributes++ = Attributes_Struct{
      "TargetName",
      "",
      NULL,
      0,
      0,
      1,
      sizeof(fh.attrs.TargetName),
      (char*)fh.attrs.TargetName,
      's',
      0
   };
   *attributes++ =
      Attributes_Struct{"filename", "", NULL, 0, 0, 1, sizeof(fh.attrs.filename), (char*)fh.attrs.filename, 's', 0};
   *attributes++ =
      Attributes_Struct{"value", "", NULL, 0, 0, 1, sizeof(fh.attrs.value), (char*)fh.attrs.value, 's', 0}; // 51
   *attributes++ = Attributes_Struct{
      "start_sector",
      "",
      NULL,
      0,
      0,
      1,
      sizeof(fh.attrs.start_sector),
      (char*)fh.attrs.start_sector,
      's',
      0
   };
   *attributes++ = Attributes_Struct{
      "sha256",
      "",
      NULL,
      0,
      0,
      1,
      sizeof(fh.attrs.sha256_verify),
      (char*)fh.attrs.sha256_verify,
      's',
      0
   }; // 53rd index of array

   m_storageInfo = {"", "", "", "", "", "", "", "", "", "", false};

   clearSettings();
}

// ----------------------------------------------------------------------------
// ~FirehoseLoader
//
// ----------------------------------------------------------------------------
FirehoseLoader::~FirehoseLoader()
{
   delete cwd;
   delete MainOutputDir;
   delete SignedDigestTableFilename;
   delete ChainedDigestTableFilename;
   delete InFileListFilename;
   delete flattenbuildto;
   delete flavor;
   delete flattenbuildvariant;
   delete MyArg;
   delete MyOpt;
   delete MyStringPairs;
   delete ConfigXML;
   delete port_name;
   delete PortTraceName;
   delete DigestsPerFileName;
   delete CommandTraceName;
   delete ConfigXMLName;
   delete full_filename_with_path;
   delete contents_full_filename_with_path;
   delete tx_buffer;
   delete tx_buffer_backup;
   delete temp_buffer;
   delete temp_buffer2;
   delete temp_buffer3;
   delete last_log_value;
   delete ContensXML;
   delete ContensXMLPath;
   delete ReadBuffer;
   delete AllAttributes;

   for(uint64_t i = 0; MAX_XML_FILES > i; ++i)
   {
      delete XMLFileTable[i];
      delete search_path[i];
      delete filter_files[i];
      delete filter_not_files[i];
      delete XMLStringTable[i];
      delete XMLStringTableTemp[i];
   }
   for(uint64_t i = 0; MAX_FILES_TO_TRACK > i; ++i)
   {
      delete MaxFilesToTrack[i];
   }
}

// ----------------------------------------------------------------------------
// memscpy
//
// ----------------------------------------------------------------------------
size_t FirehoseLoader::memscpy(void* dst, size_t dst_size, const void* src, size_t src_size)
{
   size_t copy_size = (dst_size <= src_size) ? dst_size : src_size;

   memcpy(dst, src, copy_size);

   return copy_size;
}

// ----------------------------------------------------------------------------
// clearSettings
//
// ----------------------------------------------------------------------------
void FirehoseLoader::clearSettings()
{
   m_errorCode = 0;
   COPY_BUF_SIZE = (4ULL * 1024 * 1024 * 1024) - 1;
   ShowXMLFileInLog = 0;
   LOAD_RAW_PROGRAM_FILES = 1;
   LOAD_PATCH_PROGRAM_FILES = 1;
   UsingValidation = 0;
   ShowDigestPerPacket = 0;
   createcommandtrace = 0;
   showpercentagecomplete = 0;
   ShowDigestPerFile = 0;
   ConvertProgram2Firmware = 0;
   ConvertProgram2Read = 0;
   forcecontentsxmlpaths = 0;
   verify_build = 0;
   ThereWereErrors = 0;
   RemoveCommentsFromXMLFiles = 1;
   ParseAttributes = 1;
   ComPortOpenTimeout = 3.0;
   memset(cwd, 0, MAX_PATH_SIZE);
   memset(MainOutputDir, 0, MAX_PATH_SIZE);
   std::strcpy(MainOutputDir, "./");
   memset(SignedDigestTableFilename, 0, MAX_STRING_SIZE);
   memset(ChainedDigestTableFilename, 0, MAX_STRING_SIZE);
   memset(InFileListFilename, 0, MAX_STRING_SIZE);
   memset(flattenbuildto, 0, MAX_STRING_SIZE);
   memset(flavor, 0, MAX_STRING_SIZE);
   memset(flattenbuildvariant, 0, MAX_STRING_SIZE);
   DeviceProgrammerIndex = -1;
   memset(MyArg, 0, MAX_STRING_SIZE);
   memset(MyOpt, 0, MAX_STRING_SIZE);
   memset(MyStringPairs, 0, MAX_STRING_PAIR_SIZE);
   ThisXMLLength = 0;
   TotalTransferSize = 0;
   BuildSizeTransferSoFar = 0;
   PercentageBuildLoaded = 0.0;
   memset(&fh_loader_start, 0, sizeof(fh_loader_start));
   memset(&fh_loader_end, 0, sizeof(fh_loader_end));
   NumTries = 1000;
   memset(ConfigXML, 0, MAX_PATH_SIZE);
   memset(port_name, 0, MAX_STRING_SIZE);
   SectorSizeInBytes = 512;
   GotACK = 0;
   stresstest = 0;
   AllowReset = 0;
   PromptUser = 1;
   SortTags = 1;
   ValidateImageSize = 0;

   for(uint64_t i = 0; MAX_XML_FILES > i; ++i)
   {
      memset(XMLFileTable[i], 0, MAX_PATH_SIZE);
      memset(search_path[i], 0, MAX_PATH_SIZE);
      memset(filter_files[i], 0, MAX_PATH_SIZE);
      memset(filter_not_files[i], 0, MAX_PATH_SIZE);
      memset(XMLStringTable[i], 0, MAX_XML_SIZE);
      memset(XMLStringTableTemp[i], 0, MAX_XML_SIZE);
   }
   for(uint64_t i = 0; MAX_FILES_TO_TRACK > i; ++i)
   {
      memset(MaxFilesToTrack[i], 0, MAX_PATH_SIZE);
   }


   NumXMLFilesInTable = 1;
   FileToTrackCount = 0;
   memset(PortTraceName, 0, MAX_PATH_SIZE);
   memset(DigestsPerFileName, 0, MAX_PATH_SIZE);
   memset(CommandTraceName, 0, MAX_PATH_SIZE);
   memset(ConfigXMLName, 0, MAX_PATH_SIZE);
   memset(full_filename_with_path, 0, MAX_PATH_SIZE);
   memset(contents_full_filename_with_path, 0, MAX_PATH_SIZE);
   num_search_paths = 0;
   num_xml_files_to_send = 0;
   num_filter_files = 0;
   num_filter_not_files = 0;
   fp = nullptr;
   fc = nullptr;
   ft = nullptr;
   fg = nullptr;
   fdp = nullptr;
   RawMode = 0;
   testvipimpact = 0;
   memset(tx_buffer, 0, FIREHOSE_TX_BUFFER_SIZE);
   memset(tx_buffer_backup, 0, FIREHOSE_TX_BUFFER_SIZE);
   memset(temp_buffer, 0, FIREHOSE_TX_BUFFER_SIZE);
   memset(temp_buffer2, 0, FIREHOSE_TX_BUFFER_SIZE);
   memset(temp_buffer3, 0, FIREHOSE_TX_BUFFER_SIZE);
   memset(last_log_value, 0, FIREHOSE_TX_BUFFER_SIZE);
   BytesWritten = 0;
   BytesRead = 0;
   PacketsSent = 0;
   CurrentDigestLocation = 0;
   digest_file_offset = 0;
   NumDigestsFound = 0;
   DigestSizeInBytes = 32;
   MaxNumDigestsPerTable = 0;
   sha256_file_offset = 0;
   verify_programming = 0;
   verify_programming_sha256 = 0;
   GenerateSha256File = 0;
   VerifySha256File = 0;
   memset(temp_hash_value, 0, sizeof(temp_hash_value));
   memset(verify_hash_value, 0, sizeof(verify_hash_value));
   memset(last_hash_value, 0, sizeof(last_hash_value));
   SizeOfDataFedToHashRoutine = 0;
   memset(ContensXML, 0, sizeof(ContentsXML_Struct) * MAX_CONTENTS_XML_ENTRIES);
   memset(ContensXMLPath, 0, sizeof(ContentsXML_Struct) * MAX_CONTENTS_XML_ENTRIES);
   NumContensXML = 0;
   NumContentsXMLPath = 0;
   memset(&storage_extras, 0, sizeof(storage_extras));
   memset(&UFS_LUN_Var_Struct, 0, sizeof(UFS_LUN_Var_Struct));
   memset(&fh, 0, sizeof(fh));
   port_fd = 0;
   ret = 0;
   MaxBytesToReadFromUSB = MAX_READ_BUFFER_SIZE;
   memset(ReadBuffer, 0, MAX_READ_BUFFER_SIZE);
   CharsInBuffer = 0;
   PacketLoc = 0;
   PacketStart = 0;
   WarningsBuffer.clear();
   NumWarnings = 0;
   memset(&VerboseLevel, 0, sizeof(VerboseLevel));
   PrettyPrintRawPacketsToo = 0;
   ReadBogusDataBeforeSendingConfigure = 0;
   Simulate = 0;
   SimulateBack = 0;
   Interactive = 0;
   skipharddriveread = 0;
   CreateDigests = 0;
   Verbose = 0;
   wipefirst = 0;
   erasefirst = 0;
   FlattenBuild = 0;
   SimulateForFileSize = 0;
   createconfigxml = 0;
   ForceOverwrite = 0;
   WipeFirstFileName = const_cast<char*>("Zeros16KB.bin");
   StressTestFileName = const_cast<char*>("Zeros1GB.bin");
   LastFindFileFileSize = 0;
   memset(NewTagName, 0, sizeof(NewTagName));
   firehose_tag_data.clear();
   memset(&CurrentHandlerFunction, 0, sizeof(CurrentHandlerFunction));
   m_interalErrorString.clear();
   m_errorFromDevice.clear();

   if(global_file_path_list != nullptr)
   {
      file_path_destroy(global_file_path_list);
      global_file_path_list = nullptr;
   }


   std::strcpy(PortTraceName, "port_trace.txt");
   std::strcpy(DigestsPerFileName, "DigestsPerFile.txt");
   std::strcpy(CommandTraceName, "command_trace.txt");
   std::strcpy(ConfigXMLName, "config_");
   std::strcpy(xml_header, "<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n");

   m_storageInfo = {"", "", "", "", "", "", "", "", "", "", false};
}

int32_t FirehoseLoader::HasAPathCharacter(const char* sz, SIZE_T Length)
{
   SIZE_T i;


   for(i = 0; i < Length; i++)
   {
      if(sz[i] == ':')
      {
         // could be c:\blah\blah
         return 1;
      }
   }

   if(sz[0] == '\\' && sz[1] == '\\')
   {
      return 1;
   }

#ifndef TOOLS_TARGET_WINDOWS
   // check if path is linux root
   if(sz[0] == '/')
   {
      return 1;
   }
#endif

   /*
     for(i=0;i<Length;i++)
     {
       if(sz[i]=='/')  { return 1; }
       if(sz[i]=='\\') { return 1; }
     }
   */

   return 0;
}

int32_t FirehoseLoader::IsARelativePath(char* sz, SIZE_T Length)
{
   SIZE_T i;

   for(i = 0; i < Length; i++)
   {
      if(sz[i] == ':')
      {
         // could be c:\blah\blah
         return 1;
      }
   }

   if(sz[0] == '\\' && sz[1] == '\\')
   {
      return 1;
   }

   return 0;
}


FirehoseLoader::SIZE_T FirehoseLoader::IsEmptySpace(char c)
{
   if(c == 0xA)
   {
      return 1;
   }

   if(c == 0xD)
   {
      return 1;
   }

   if(c == 0x9)
   {
      return 1;
   }

   if(c == 0x20)
   {
      return 1;
   }

   return 0;
}


void FirehoseLoader::print_usage()
{
   ShowCommandLineOptions();

   dbg(
      LOG_INFO,
      "\nDebugging\n"
      "\nBy default --loglevel=1. This is a decent amount of "
      "logging for debugging."
      "\n           --loglevel=2. This shows more information such "
      "as RAW packets being sent."
      "\n           --loglevel=3. This is very verbose, showing "
      "every byte in a HEX editor view that is sent/received."
      "\n           --loglevel=0. Turns off nearly all logging."
      "\n           --showdigestperpacket - This shows the SHA256 "
      "digest for every packet sent"
      "\n           --createdigests"
   );
   dbg(
      LOG_INFO,
      "\nUsage: \n"
      "fh_loader.exe --port=\\\\.\\COM19 --sendimage=AnyFile.bin "
      "--search_path=c:\\builds\\Perforce_main\\core\\storage\\tools\\ "
      "--noreset --noprompt\n"
      "fh_loader.exe --port=\\\\.\\COM19 --sendxml=rawprogram0.xml "
      "--search_path=c:\\builds\\Perforce_main\\core\\storage\\tools\\ \n"
      "fh_loader.exe --port=\\\\.\\COM19 --sendxml=rawprogram0.xml "
      "--search_path=c:\\builds\\Perforce_main\\core\\storage\\tools\\ "
      "--loglevel=2\n"
   );
   dbg(
      LOG_INFO,
      "\nValidation\n"
      "\n         --generatesha256file. Generate the sha256 file per "
      "sendimage by simulation, --digestsperfilename specify the file path."
      "\n         --verifysha256file.   After flashing image to target, "
      "compute sha256 on target side and compare with --digestsperfilename."
   );
   dbg(
      LOG_INFO,
      "\nImage Validation Usage: \n"
      "fh_loader.exe --port=\\\\.\\COM19 --memoryname=ufs "
      "--setactivepartition=1 --generatesha256file "
      "--digestsperfilename=C:\\workspace\\fh_loader\\Testsha256file  "
      "--json_in=Waipio.LA.1.0-00711-GKI.INT-1.json\n"
      "fh_loader.exe --port=\\\\.\\COM19 --memoryname=ufs "
      "--setactivepartition=1 --verify_programming_getsha --verifysha256file "
      "--digestsperfilename=C:\\workspace\\fh_loader\\Testsha256file  "
      "--json_in=Waipio.LA.1.0-00711-GKI.INT-1.json\n"
      "fh_loader.exe --port=\\\\.\\COM19 --memoryname=ufs "
      "--setactivepartition=1 --verify_programming --verifysha256file "
      "--digestsperfilename=C:\\workspace\\fh_loader\\Testsha256file  "
      "--json_in=Waipio.LA.1.0-00711-GKI.INT-1.json\n"
   );
   dbg(
      LOG_INFO,
      "\nARGUMENTS\n--json_in=  A text file or json formatted file "
      "with the following format\n"
      "==== FILE START ====\n{\n"
      "  \"partition_patch\": [\n"
      "    \"\\\\PAHT_TO_PATCH\\patch0.xml\", \n"
      "  ],\n"
      "  \"partition\": [\n"
      "    \"\\\\PAHT_TO_RAWPROGRAM\\rawprogram0.xml\",\n"
      "    \"\\\\PAHT_TO_RAWPROGRAM\\rawprogram1.xml\",\n"
      "  ],\n"
      "  \"partition_bin\": [\n"
      "    \"\\\\PATH_TO_BINARIESsnowcone\\file1.elf\", \n"
      "    \"\\\\PATH_TO_BINARIESsnowcone\\file2.elf\", \n"
      "  ]\n"
      "}\n==== FILE END ====\n"
      "\n"
      "  This file can be generated using meta_cli.py in the "
      "metabuild, for example:\n"
      "    <meta_path>\\common\\build\\app\\meta_cli.py "
      "get_partition_files flavor='asic' group=True storage='ufs'\n"
      "\n"
   );

   return;
}

void FirehoseLoader::InitAttributes(void)
{
   fh.attrs.MaxPayloadSizeToTargetInBytes = ONE_MEGABYTE; // Target will force this to real value after <configure>
                                                          // tag
   fh.attrs.MaxPayloadSizeToTargetInBytesSupported = ONE_MEGABYTE; // ONE_MEGABYTE;
   fh.attrs.MaxPayloadSizeFromTargetInBytes = 8192;                // ONE_MEGABYTE;

   MaxBytesToReadFromUSB = fh.attrs.MaxPayloadSizeFromTargetInBytes;

   fh.attrs.Verbose = 0;
   fh.attrs.MaxDigestTableSizeInBytes = 8192;
   fh.attrs.AckRawDataEveryNumPackets = 0;
   fh.attrs.delayinseconds = 1;
   fh.attrs.address64 = 0;
   fh.attrs.value64 = 0;
   fh.attrs.storagedrive = 0;
   fh.attrs.physical_partition_number = 0;
   fh.attrs.SECTOR_SIZE_IN_BYTES = 512;
   fh.attrs.byte_offset = 0;
   fh.attrs.physical_partition_number = 0;
   fh.attrs.size_in_bytes = 0;
   fh.attrs.num_partition_sectors = 0;
   fh.attrs.file_sector_offset = 0;

   fh.attrs.ZlpAwareHost = ZLPAWAREHOST;
   fh.attrs.SkipWrite = 0;
   fh.attrs.BufferWrites = 0;
   // fh.attrs.AckRawData               = 0;
   fh.attrs.AlwaysValidate = 0;

   // fh.attrs.display                    = 0;
   // fh.attrs.read_back_verify           = 0;
   fh.attrs.TestWritePerformance = 0;
   fh.attrs.TestReadPerformance = 0;
   fh.attrs.TestDigestPerformance = 0;
   fh.attrs.SkipStorageInit = 0;
   fh.attrs.SkipSettingMinClocks = 0;
   fh.attrs.SkipSettingMaxClocks = 0;
   fh.attrs.actual_size_in_bytes = 0;
   fh.attrs.slot = 0;

   // fh.attrs.start_sector               = 0;
   // strncpy(fh.attrs.start_sector,"0",sizeof("0"));
   if(CopyString(fh.attrs.start_sector, "0", 0, 0, sizeof("0"), sizeof(fh.attrs.start_sector), sizeof("0")) == 0)
   {
      dbg(
         LOG_ERROR,
         "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
         "0",
         strlen("0")
      );
      ExitAndShowLog(1);
   }

   // strncpy(fh.attrs.MemoryName,"eMMC",sizeof("eMMC"));
   if(CopyString(fh.attrs.MemoryName, "eMMC", 0, 0, sizeof("eMMC"), sizeof(fh.attrs.MemoryName), sizeof("eMMC")) == 0)
   {
      dbg(
         LOG_ERROR,
         "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.MemoryName",
         "eMMC",
         strlen("eMMC")
      );
      ExitAndShowLog(1);
   }
}

void FirehoseLoader::InitLogging(void)
{
   // fp = ReturnFileHandle (PortTraceName, MAX_PATH_SIZE, "w");    // will exit
   // if not successful fh_fclose(fp); setvbuf(stdout, NULL, _IOFBF,
   // PORT_TRACE_LOG_BUFFER_SIZE);

   // setvbuf(fp, NULL, _IOFBF, PORT_TRACE_LOG_BUFFER_SIZE);  // aaron put this
   // back on later
}


void FirehoseLoader::MyLog(int32_t log_level, const char* func_name, int32_t line_number, const char* format, ...)
{
   va_list args;
   va_start(args, format);

   // Get the required buffer size
   va_list args_copy;
   va_copy(args_copy, args);
   int size = vsnprintf(nullptr, 0, format, args_copy);
   va_end(args_copy);

   // Format the string
   std::string log(size + 1, '\0');
   vsnprintf(&log[0], log.size(), format, args);
   log.resize(size); // Remove null terminator

   va_end(args);

   switch(log_level)
   {
      case LOG_ERROR:
         ThereWereErrors = 1;
         // m_interalErrorString += "\n";
         m_interalErrorString += log.c_str();
         FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE(log + ", " + func_name + ":" + std::to_string(line_number));
         break;

      case LOG_INFO:
         FIREHOSE_LOADER_PORT_TRACE_LOG_MESSAGE(log + ", " + func_name + ":" + std::to_string(line_number));
         break;

      case LOG_ALWAYS:
         FIREHOSE_LOADER_PORT_TRACE_LOG_MESSAGE(log + ", " + func_name + ":" + std::to_string(line_number));
         break;

      case LOG_ONLY:
         FIREHOSE_LOADER_PORT_TRACE_LOG_MESSAGE(log + ", " + func_name + ":" + std::to_string(line_number));
         break;

      case LOG_DEBUG:
         FIREHOSE_LOADER_PORT_TRACE_DEBUG_MESSAGE(log + ", " + func_name + ":" + std::to_string(line_number));
         break;

      case LOG_WARN:

         NumWarnings++;
         WarningsBuffer += "\n";
         WarningsBuffer += log;
         FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE(log + ", " + func_name + ":" + std::to_string(line_number));
         break;


      default:
         break;
   }
}

int32_t
FirehoseLoader::getoptarg(int32_t i, char* argv[], char* Option, SIZE_T SizeOfOpt, char* Argument, SIZE_T SizeOfArg)
{
   char* pch = nullptr;
   uint32_t Length = 0;

   memset(Option, 0, SizeOfOpt);
   memset(Argument, 0, SizeOfArg);


   if(argv[i][0] != '-' || argv[i][1] != '-')
   {
      dbg(LOG_ERROR, "Option '%s' does not begin with --\n", argv[i]);
      ExitAndShowLog(1);
   }

   Length = static_cast<uint32_t>(strlen(argv[i]));

   if(Option[Length - 1] == '=')
   {
      dbg(LOG_ERROR, "Option '%s' did not supply an argument after the '=' sign\n", argv[i]);
      ExitAndShowLog(1);
   }

   if(CopyString(Option, &argv[i][2], 0, 0, strlen(&argv[i][2]), SizeOfOpt, strlen(&argv[i][2])) == 0)
   {
      dbg(LOG_ERROR, "Failed to copy string of length %" SIZE_T_FORMAT " bytes into Option", strlen(&argv[i][2]));
      ExitAndShowLog(1);
   }

   // strncpy(Option,&argv[i][2],strlen(&argv[i][2]));
   Option[strlen(&argv[i][2])] = '\0'; // ensure NULL is here
   // dbg (LOG_INFO, "\n\n\n\n\n\n\nOption='%s'\n\n",Option);

   // Now Option no longer has "--" at the beginning
   // So --superverbose becomes superverbose

   pch = strstr(Option, "=");

   if(pch != nullptr)
   {
      // dbg (LOG_INFO, "pch is NOT NULL\n\n");
      //  To be here pch is pointing at "=COM5", Option is pointing at
      //  "--port=COM5"

      if(CopyString(Argument, pch, 0, 1, strlen(&pch[1]), SizeOfArg, strlen(pch)) == 0)
      {
         dbg(LOG_ERROR, "Failed to copy string of length %" SIZE_T_FORMAT " bytes into Argument", strlen(&pch[1]));
         ExitAndShowLog(1);
      }

      // strncpy(Argument, &pch[1], strlen(pch));    // This stores "port=" into
      // Option
      Option[pch - Option + 1] = '\0'; // cut off just before = sign
   }

   // To be here Option is something like "port=" or "simulate" or
   // "superverbose" or "xml="

   // dbg (LOG_INFO, "Option   = '%s'\n",Option);
   // dbg (LOG_INFO, "Argument = '%s'\n\n",Argument);

   return 0;
}

void FirehoseLoader::ExitAndShowLog(uint32_t ExitCode)
{
   // SIZE_T Length;

   ClosePort();

   if(global_file_path_list != nullptr)
   {
      file_path_destroy(global_file_path_list);
      global_file_path_list = nullptr;
   }

   if(fdp != nullptr)
   {
      fh_fclose(fdp);
      fdp = nullptr;
   }

   if(fc != nullptr)
   {
      fh_fclose(fc);
      fc = nullptr;
   }

   if(ft != nullptr)
   {
      fh_fclose(ft);
      ft = nullptr;
   }

   if(fg != nullptr)
   {
      fh_fclose(fg);
      fg = nullptr;
   }

   m_errorCode = ExitCode;
   if(0 != ExitCode)
   {
      // This should be replaced with TOOLS_THROW. Currently TOOLS_THROW has
      // compilation issue if used here.
      throw ExitCode;
   }
}

void FirehoseLoader::PerformSHA256(uint8_t* inputPtr, uint32_t inputLen, uint8_t* outputPtr)
{
   // struct __sechsh_ctx_s   context;

   sechsharm_sha256_init(&context);
   sechsharm_sha256_update(&context, context.leftover, &(context.leftover_size), inputPtr, inputLen);

   sechsharm_sha256_final(&context, context.leftover, &(context.leftover_size), outputPtr);
}


void FirehoseLoader::LoadBuffer(char* MyBuffer, SIZE_T Length)
{
   SIZE_T i;

   for(i = 0; i < Length; i++)
      MyBuffer[i] = (i & 0xFF);
}

/* Get the month the binary was built.
   @param month [output] it should be at least 2 characters long.
*/
void FirehoseLoader::get_build_month(char* month)
{
   if(__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
   {
      *month = '0';
      *(month + 1) = '1';
   }
   else if(__DATE__[0] == 'F' && __DATE__[1] == 'e' && __DATE__[2] == 'b')
   {
      *month = '0';
      *(month + 1) = '2';
   }
   else if(__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
   {
      *month = '0';
      *(month + 1) = '3';
   }
   else if(__DATE__[0] == 'A' && __DATE__[1] == 'p' && __DATE__[2] == 'r')
   {
      *month = '0';
      *(month + 1) = '4';
   }
   else if(__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
   {
      *month = '0';
      *(month + 1) = '5';
   }
   else if(__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
   {
      *month = '0';
      *(month + 1) = '6';
   }
   else if(__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
   {
      *month = '0';
      *(month + 1) = '7';
   }
   else if(__DATE__[0] == 'A' && __DATE__[1] == 'u' && __DATE__[2] == 'g')
   {
      *month = '0';
      *(month + 1) = '8';
   }
   else if(__DATE__[0] == 'S' && __DATE__[1] == 'e' && __DATE__[2] == 'p')
   {
      *month = '0';
      *(month + 1) = '9';
   }
   else if(__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')
   {
      *month = '1';
      *(month + 1) = '0';
   }
   else if(__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')
   {
      *month = '1';
      *(month + 1) = '1';
   }
   else if(__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')
   {
      *month = '1';
      *(month + 1) = '2';
   }
   else
   {
      dbg(LOG_INFO, "Cant convert date string to number at compile time");
      *month = __DATE__[0];
      *(month + 1) = __DATE__[2];
   }
}


struct Device::Protocol::FirehoseLoader::file_list* FirehoseLoader::file_path_init(void)
{
   struct file_list* list;

   list = (struct file_list*)malloc(sizeof(struct file_list));
   if(list == NULL)
   {
      dbg(LOG_ERROR, "Failed to allocate structure to store list of files %d", 0);
      return NULL;
   }

   list->in_use = 1;
   list->count = 0;
   list->index = 0;
   list->list = nullptr;
   return list;
}

int32_t FirehoseLoader::file_path_grow(struct file_list* list)
{
   if(list->list == NULL)
   {
      size_t i;
      list->list = (struct file_path*)malloc(sizeof(struct file_path) * 16);
      if(list->list == NULL)
      {
         dbg(LOG_ERROR, "Failed to allocate first buffer for list of files %d", 0);
         return -1;
      }
      list->count = 16;
      list->index = 0;

      for(i = 0; i < list->count; i++)
      {
         list->list[i].path = nullptr;
         list->list[i].name = nullptr;
      }
   }
   else if(list->count == list->index) /* If the array is full, allocate more
                                          space. */
   {
      size_t new_size = list->count;
      struct file_path* new_list;
      size_t i;

      if(list->count < 128) /* If list count is small, grow it exponentially. */
         new_size *= 2;
      else
         new_size += 128; /* If not, don't grow it exponentially. */

      if(new_size > SIZE_MAX / sizeof(struct file_path))
      {
         dbg(LOG_ERROR, "Hit the limit of SIZE_T! %llu * %d", (uint64_t)new_size, sizeof(struct file_path));
         return -1;
      }

      new_list = (struct file_path*)realloc(list->list, new_size * sizeof(struct file_path));
      if(new_list == NULL)
      {
         dbg(LOG_ERROR, "Failed to grow buffer for list of files %llu", (uint64_t)new_size);
         return -1;
      }
      list->list = new_list;
      list->count = new_size;

      for(i = list->index; i < list->count; i++)
      {
         list->list[i].path = nullptr;
         list->list[i].name = nullptr;
      }
   }
   return 0;
}

/* File names are supposed to be unique, this function will search the list and
return the first instance with the same file name.

return NULL if file does not exist in the list, pointer to path if file name
is found
*/
char* FirehoseLoader::file_path_search(struct file_list* list, char* name)
{
   size_t i;
   char* path = nullptr;
   if(list == NULL) return NULL;
   for(i = 0; i < list->index; i++)
   {
      // Replace path separator for comparision gen4_gvm / vbmeta.img
#ifdef TOOLS_TARGET_WINDOWS
      string_replace(name, strlen(name), "/", "\\");
#else
      string_replace(name, strlen(name), "\\", "/");
#endif
      if(strcmp(list->list[i].name, name) == 0)
      {
         path = list->list[i].path;
         break;
      }

      // If file name is not matching, try to macth with file path
      if(std::string::npos != std::string(list->list[i].path).find(name) &&
         strcmp(
            std::filesystem::path(list->list[i].path).filename().string().c_str(),
            std::filesystem::path(name).filename().string().c_str()
         ) == 0)
      {
         dbg(LOG_INFO, "Found matched path with file: %s, %s, %s", list->list[i].name, name, list->list[i].path);
         path = list->list[i].path;
         break;
      }
   }
   return path;
}

int32_t FirehoseLoader::file_path_add_path(struct file_list* list, char* path)
{
   int32_t result = 0;
   char* existing_path;
   if(list == NULL) return -1;

   if(list->count == list->index) /* If the array is full, allocate more space. */
      result = file_path_grow(list);

   if(result == 0)
   {
      int64_t i, len;
      char* out_path;
      char* name = nullptr;
      len = strlen(path);
      if(len > MAX_PATH_SIZE)
      {
         dbg(LOG_ERROR, "Supplied path length is too long. Len: %llu path: %s", (uint64_t)len, path);
         result = -1;
         return result;
      }
      if(len == 0)
      {
         dbg(LOG_ERROR, "Invalid path length of zero (%llu)", (uint64_t)len);
         result = -1;
         return result;
      }

      out_path = (char*)malloc(len + 1);
      if(out_path == NULL)
      {
         dbg(LOG_ERROR, "Failed to allocate buffer for path %s, %llu", path, (uint64_t)len);
         result = -1;
         return result;
      }

      memscpy(out_path, len, path, len);
      out_path[len] = '\0';
      for(i = len; i >= 0; i--)
      {
         name = &out_path[i];
         if(i == 0) break;
         if(path[i - 1] == '/' || path[i - 1] == '\\') break;
      }

      existing_path = file_path_search(list, name);
      /* Check if we already have the file in the list */
      if(existing_path != nullptr)
      {
         if(strcmp(out_path, existing_path) != 0)
         {
            dbg(LOG_WARN, "Multiple paths found for the file: %s", name);
         }
         else
         {
            dbg(LOG_INFO, "Input points to two %s, but both have the same path", name);
         }
      }

      i = list->index;
      list->list[i].path = out_path;
      list->list[i].name = name;
      list->index++;
   }

   return result;
}

int32_t FirehoseLoader::file_path_count(struct file_list* list)
{
   if(list == NULL) return 0;

   if(list->in_use == 0) return -1;

   return static_cast<int32_t>(list->index);
}

int32_t FirehoseLoader::file_path_destroy(struct file_list* list)
{
   size_t i, count;

   if(list == NULL) return 0;

   if(list->in_use == 0) return -1;

   count = list->count;
   try
   {
      for(i = 0; i < count; i++)
      {
         char* curr_path = list->list[i].path;
         if(curr_path != nullptr)
         {
            free(curr_path);
            list->list[i].path = nullptr;
            list->list[i].name = nullptr;
            curr_path = nullptr;
         }
      }

      free(list->list);
      list->list = nullptr;
      free(list);
   }
   TOOLS_CATCH(e, FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE(std::string("Free file list failed Error") + e.what()));
   return 0;
}


int32_t FirehoseLoader::add_string_to_xml_file_table(char* buffer)
{
   size_t len = strlen(buffer);

   if(CopyString(XMLFileTable[num_xml_files_to_send], buffer, 0, 0, len, MAX_PATH_SIZE, len) == 0)
   {
      dbg(
         LOG_ERROR,
         "Failed to copy string '%s' of length %i into XMLFileTable[%i]",
         buffer,
         len,
         num_xml_files_to_send
      );
      ExitAndShowLog(1);
   }
   num_xml_files_to_send++;
   if(num_xml_files_to_send >= MAX_FILES_IN_OPTIONS)
   {
      dbg(LOG_ERROR, "1. Too many XML files in XMLFileTable, max is %d", XMLStringTable);
      ExitAndShowLog(1);
   }
   return static_cast<int32_t>(num_xml_files_to_send);
}


/* String replace
 *
 * Search string 'find' in 'in' and replace with 'new'. The size of 'new' must
 * be less than or equal to find.
 *
 * in     input string.
 * len    length of input string.
 * find   String to search for.
 * new    String that replaces 'find'
 *
 */
void FirehoseLoader::string_replace(char* in, size_t len, const char* find, const char* newStr)
{
   size_t ahead, behind, find_len, new_len;
   char* head;
   char* tail = nullptr;
   find_len = strlen(find);
   if(find_len > len) return;

   new_len = strlen(newStr);
   if(new_len > find_len)
   {
      dbg(LOG_ERROR, "Invalid New string lengths %d > find len %d", new_len, find_len);
      return;
   }

   ahead = 0;
   behind = 0;
   while(ahead <= len)
   {
      head = &in[ahead];
      tail = &in[behind];
      if(memcmp(head, find, find_len) == 0)
      {
         memscpy(tail, len - ahead, newStr, new_len);
         ahead += find_len;
         behind += new_len;
      }
      else
      {
         *tail = *head;
         ahead++;
         behind++;
      }
   }
   *tail = '\0';

   return;
}

/* Move the the given character skipping the white paces.*/
int32_t FirehoseLoader::json_move_to_char(char* json, char next_c)
{
   int32_t json_len;
   int32_t i;
   json_len = static_cast<int32_t>(strlen(json));
   i = 0;
   for(i = 0; i < json_len; i++)
   {
      char c = json[i];
      if(c == next_c) break;
      if(c != ' ' && c != '\n' && c != '\r' && c != '\t') return -1;
   }
   if(i == json_len) return -1;

   return i;
}

int32_t FirehoseLoader::json_add_xml_to_list(char* start, char* end)
{
   int32_t offset;
   char* curr;
   curr = start;
   while(curr < end)
   {
      offset = json_move_to_char(curr, '"');
      if(offset == -1) return -1;

      start = curr = curr + offset + 1;
      /* Find the closing quote */
      while(curr < end)
      {
         if(*curr == '"' && *(curr - 1) != '\\') break;
         curr++;
      }
      *curr = '\0';
      add_string_to_xml_file_table(start);
      while(curr < end && *curr != ',')
         curr++;
      curr++;
   }

   return 0;
}

int32_t FirehoseLoader::json_add_file_to_list(char* start, char* end)
{
   int32_t offset;
   char* curr;

   if(global_file_path_list == NULL) global_file_path_list = file_path_init();

   curr = start;
   while(curr < end)
   {
      offset = json_move_to_char(curr, '"');
      if(offset == -1) return -1;

      start = curr = curr + offset + 1;
      /* Find the closing quote */
      while(curr < end)
      {
         if(*curr == '"' && *(curr - 1) != '\\') break;
         curr++;
      }
      *curr = '\0';
      file_path_add_path(global_file_path_list, start);
      while(curr < end && *curr != ',')
         curr++;
      curr++;
   }

   return 0;
}

void FirehoseLoader::metacli_json_in(char* json_path)
{
   std::shared_ptr<std::fstream> json_fp;
   size_t json_len;
   char* json;
   size_t read_size, tot_read = 0;

   json_fp = fh_fopen(json_path, "rb");
   if(!json_fp || !json_fp->is_open())
   {
      dbg(LOG_ERROR, "Failed to open JSON formatted input file %s", json_path);
      ExitAndShowLog(1);
      return;
   }

   // Get file size using std::fstream
   json_fp->seekg(0, std::ios::end);
   json_len = json_fp->tellg();
   json_fp->seekg(0, std::ios::beg);

   json = (char*)malloc(json_len);
   if(json == NULL)
   {
      dbg(LOG_ERROR, "Failed to allocate JSON memory of length %ld", json_len);
      fh_fclose(json_fp);
      ExitAndShowLog(1);
      return;
   }

   do
   {
      json_fp->read(json + tot_read, json_len - tot_read);
      read_size = json_fp->gcount();
      tot_read += read_size;
   } while(read_size > 0 && tot_read < json_len);

   fh_fclose(json_fp);
   if(tot_read < json_len)
   {
      dbg(LOG_WARN, "File size %d was larger than total data read %llu from json file.", json_len, (uint64_t)tot_read);
      json_len = tot_read;
   }

   string_replace(json, json_len, "\\\\", "\\");
   {
      int32_t offset;
      char* end;
      char *open, *close;
      char* curr;
      int32_t done = 0;
      char* limit = &json[json_len];
      offset = json_move_to_char(json, '{');
      if(offset == -1)
      {
         dbg(LOG_ERROR, "Failed to find open bracket { for json file %d", offset);
         free(json);
         ExitAndShowLog(1);
         return;
      }

      curr = &json[offset + 1];

      while(curr <= limit && done == 0)
      {
         offset = json_move_to_char(curr, '"');
         if(offset == -1)
         {
            dbg(LOG_ERROR, "Failed to find opening \" on json input %d", offset);
            free(json);
            ExitAndShowLog(1);
            return;
         }

         curr = &curr[offset + 1];
         end = strstr(curr, "\"");
         if(end == NULL)
         {
            dbg(LOG_ERROR, "Failed to find closing \" starting at offset %d", offset + 1);
            free(json);
            ExitAndShowLog(1);
            return;
         }
         *end = '\0';

         open = strstr(end + 1, "[");
         close = strstr(end + 1, "]");
         if(open == NULL || close == NULL)
         {
            dbg(LOG_ERROR, "JSON file format not as expected, failed to path array %d", offset);
            free(json);
            ExitAndShowLog(1);
            return;
         }
         *close = '\0';
         open++; /* Move pass the open bracket. */

         if(strcmp("partition_patch", curr) == 0 || strcmp("partition", curr) == 0)
         {
            json_add_xml_to_list(open, close);
         }
         else if(strcmp("partition_bin", curr) == 0)
         {
            json_add_file_to_list(open, close);
         }

         curr = close + 1;
         while(curr <= limit)
         {
            char c = *curr;
            if(c == '}' || c == '\0')
            {
               done = 1;
               break;
            }
            if(c == ',')
            {
               curr++;
               break;
            }
            curr++;
         }
      }
   }

   dbg(LOG_INFO, "Successfully loaded %d file paths from json input", file_path_count(global_file_path_list));
   free(json);
   return;
}

/*--------------------------------------------------------
----------------SPARSE FILE FUNCTIONS---------------------
---------------------------------------------------------*/
#define CHUNK_FILL_PATTERN_LENGTH 4
struct FirehoseLoader::sparse_handle_t* FirehoseLoader::
   sparse_open(std::shared_ptr<std::fstream> fd, uint64_t current_start_sector, uint64_t sector_size_in_bytes)
{
   struct sparse_handle_t* return_sparse_handle;

   struct sparse_header_t sparse_header;
   struct chunk_header_t chunk_header;
   const size_t SPARSE_HEADER_LEN = sizeof(struct sparse_header_t);
   const size_t CHUNK_HEADER_LEN = sizeof(struct chunk_header_t);

   int32_t start_new_chunk = TRUE;
   int32_t i = 0;
   uint64_t temp;
   size_t result;
   uint64_t file_size_sectors = 0;

   uint64_t chunk = 0;
   uint64_t length = 0;

   int64_t file_offset;

   uint32_t total_blocks = 0;
   int32_t cur_group = -1;

   // Do a pass through the file to figure out how many bytes we need to read
   fd->read(reinterpret_cast<char*>(&sparse_header), SPARSE_HEADER_LEN);
   result = fd->gcount();

   if(result != SPARSE_HEADER_LEN)
   {
      dbg(LOG_ERROR, "Didn't properly read the sparse_header!\n");
      ExitAndShowLog(1);
   }

   // Check if headers are correct
   if(sparse_header.magic != SPARSE_HEADER_MAGIC)
   {
      dbg(LOG_ERROR, "Bad magic: %x probably not processing a sparse image\n", sparse_header.magic);
      ExitAndShowLog(1);
   }

   if(sparse_header.major_version != SPARSE_HEADER_MAJOR_VER)
   {
      dbg(LOG_ERROR, "Unknown major version number\n", sparse_header.major_version);
      ExitAndShowLog(1);
   }

   // Allocate space based on sparse header, this is worst case space allocation
   return_sparse_handle = (struct sparse_handle_t*)malloc(sizeof(struct sparse_handle_t));

   if(return_sparse_handle == NULL)
   {
      dbg(LOG_ERROR, "Malloc for return sparse handle failed");
      ExitAndShowLog(1);
   }

   return_sparse_handle->start_sector = (uint64_t*)malloc(sparse_header.total_chunks * sizeof(uint64_t));
   return_sparse_handle->bytes_remaining = (uint64_t*)malloc(sparse_header.total_chunks * sizeof(uint64_t));
   return_sparse_handle->group_file_offset = (int64_t*)malloc(sparse_header.total_chunks * sizeof(int64_t));

   if(return_sparse_handle->start_sector == NULL || return_sparse_handle->bytes_remaining == NULL ||
      return_sparse_handle->group_file_offset == NULL)
   {
      dbg(LOG_ERROR, "Malloc in sparse_open failed");
      ExitAndShowLog(1);
   }

   // return_sparse_handle->fd = fd;
   return_sparse_handle->total_groups = 0;
   return_sparse_handle->chunk_header_idx = 0;


   if(sparse_header.file_hdr_sz > SPARSE_HEADER_LEN)
   {
      // skip the remaining bytes in a header that is longer than we expected
      fd->seekg(sparse_header.file_hdr_sz - SPARSE_HEADER_LEN, std::ios::cur);
   }

   // Loop through the chunks
   for(i = 0; (size_t)i < sparse_header.total_chunks; i++)
   {
      file_offset = fd->tellg();
      fd->read(reinterpret_cast<char*>(&chunk_header), CHUNK_HEADER_LEN);
      result = fd->gcount();

      if(result != CHUNK_HEADER_LEN)
      {
         dbg(LOG_ERROR, "Didn't get proper amount of bytes for chunk_header\n");
         ExitAndShowLog(1);
      }

      if(sparse_header.chunk_hdr_sz > CHUNK_HEADER_LEN)
      {
         // Skip the remaining bytes in a header that is longer than we expected
         fd->seekg(sparse_header.chunk_hdr_sz - CHUNK_HEADER_LEN, std::ios::cur);
      }

      if(chunk_header.chunk_type == CHUNK_TYPE_RAW)
      {
         // Actually handle the splitting here
         // will have chunk_header.chunk_sz blocks of raw data
         if(chunk_header.total_sz != (sparse_header.chunk_hdr_sz + (chunk_header.chunk_sz * sparse_header.blk_sz)))
         {
            dbg(LOG_ERROR, "Bogus chunk size for chunk %d, type Raw\n", i);
            ExitAndShowLog(1);
         }

         temp = chunk_header.chunk_sz * sparse_header.blk_sz;

         if((temp % sector_size_in_bytes) == 0)
         {
            file_size_sectors = temp / sector_size_in_bytes;
         }
         else
         {
            dbg(LOG_ERROR, "File chunk size %d is not a sector-multiple", temp);
            ExitAndShowLog(1);
         }

         if(start_new_chunk)
         {
            // First RAW we encounter
            cur_group++;
            return_sparse_handle->total_groups = cur_group + 1;
            // Create a new file for writing data
            start_new_chunk = FALSE;
            return_sparse_handle->group_file_offset[cur_group] = file_offset;
            return_sparse_handle->start_sector[cur_group] = current_start_sector;
            return_sparse_handle->bytes_remaining[cur_group] = 0;
         }

         // Pull out all length bytes of data from file
         length = chunk_header.chunk_sz * sparse_header.blk_sz;

         while(length != 0)
         {
            chunk = (length > COPY_BUF_SIZE) ? COPY_BUF_SIZE : length;

            fd->seekg(chunk, std::ios::cur);
            if(fd->fail())
            {
               dbg(LOG_ERROR, "Error when trying to read RAW data from file\n");
               ExitAndShowLog(1);
            }

            length -= chunk;

            return_sparse_handle->bytes_remaining[cur_group] += chunk;
         } // end while(length != 0)

         current_start_sector += file_size_sectors;
         total_blocks = total_blocks + chunk_header.chunk_sz;
      } // end if(chunk_header.chunk_type == CHUNK_TYPE_RAW)
      else if(chunk_header.chunk_type == CHUNK_TYPE_DONT_CARE)
      {
         // Useless data that does not need to be sent
         // signifies the end of one group
         if(chunk_header.total_sz != sparse_header.chunk_hdr_sz)
         {
            dbg(LOG_ERROR, "Bogus chunk size for chunk %d, type Don't Care\n", i);
            ExitAndShowLog(1);
         }

         if(start_new_chunk == FALSE) // We encountered a CHUNK_RAW before this
         {
            start_new_chunk = TRUE;
         }

         temp = chunk_header.chunk_sz * sparse_header.blk_sz;

         if((temp % sector_size_in_bytes) == 0)
         {
            current_start_sector += (temp / sector_size_in_bytes);
         }
         else
         {
            dbg(LOG_ERROR, "Number of bytes skipped %d is not a sector-multiple\n", temp);
            ExitAndShowLog(1);
         }

         total_blocks = total_blocks + chunk_header.chunk_sz;
      } // end if(chunk_header.chunk_type == CHUNK_TYPE_DONT_CARE)
      else if(chunk_header.chunk_type == CHUNK_TYPE_FILL)
      {
         // Useless data that does not need to be sent
         // signifies the end of one group
         if(chunk_header.total_sz != (sizeof(struct chunk_header_t) + CHUNK_FILL_PATTERN_LENGTH))
         {
            dbg(LOG_ERROR, "Bogus chunk size for chunk %d, type Fill\n", i);
            ExitAndShowLog(1);
         }

         // A new FILL type chunk. Each FILL type chunk considered to be a new
         // unsparsed file.
         start_new_chunk = TRUE;

         temp = chunk_header.chunk_sz * sparse_header.blk_sz;

         if((temp % sector_size_in_bytes) == 0)
         {
            file_size_sectors = temp / sector_size_in_bytes;
         }
         else
         {
            dbg(LOG_ERROR, "File chunk size %d is not a sector-multiple", temp);
            ExitAndShowLog(1);
         }

         // This is a a new FILL type, fill the details in the next offset
         cur_group++;
         return_sparse_handle->total_groups = cur_group + 1;
         // Create a new file for writing data
         return_sparse_handle->group_file_offset[cur_group] = file_offset;
         return_sparse_handle->start_sector[cur_group] = current_start_sector;
         return_sparse_handle->bytes_remaining[cur_group] = 0;

         // Seek to skip the 4 bytes of fill data
         fd->seekg(4, std::ios::cur);
         if(fd->fail())
         {
            dbg(LOG_ERROR, "Error when trying to read RAW data from file\n");
            ExitAndShowLog(1);
         }

         // Pull out all length bytes of data from file
         length = chunk_header.chunk_sz * sparse_header.blk_sz;

         return_sparse_handle->bytes_remaining[cur_group] = length;
         current_start_sector += file_size_sectors;
         total_blocks = total_blocks + chunk_header.chunk_sz;
      } // end if(chunk_header.chunk_type == CHUNK_TYPE_FILL)
      else
      {
         dbg(LOG_ERROR, "Unknown chunk type %x\n", chunk_header.chunk_type);
         ExitAndShowLog(1);
      }
   } // End for loop through total_blocks

   // Sanity check to make sure we didnt miss any blocks
   if(sparse_header.total_blks != total_blocks)
   {
      dbg(LOG_ERROR, "Wrote %d blocks, expected to write %d blocks\n", total_blocks, sparse_header.total_blks);
      ExitAndShowLog(1);
   }

   fd->seekg(0, std::ios::beg);

   return return_sparse_handle;
}

// Assumes sparse open was called before this
size_t FirehoseLoader::sparse_read(
   char* data,
   size_t num_bytes,
   std::shared_ptr<std::fstream> fd,
   struct sparse_read_info_t* sparse_read_struct,
   struct sparse_header_t sparse_header
)
{
   const size_t SPARSE_HEADER_LEN = sizeof(struct sparse_header_t);
   const size_t CHUNK_HEADER_LEN = sizeof(struct chunk_header_t);

   uint32_t i = 0;

   uint64_t chunk = 0;
   uint64_t length = 0;
   uint64_t data_offset = 0;
   size_t bytes_read = 0;

   // How many bytes are we reading out
   length = (num_bytes < sparse_read_struct->bytes_remaining) ? num_bytes : sparse_read_struct->bytes_remaining;

   // Loop through the chunks, reading one by one
   while(length > 0)
   {
      // If current file position is at a header, read past all headers until a
      // raw header
      if(sparse_read_struct->chunk_remaining == 0)
      {
         read_chunk_header(fd, sparse_header.chunk_hdr_sz, &sparse_read_struct->chunk_header);
         sparse_read_struct->chunk_remaining = 1UL * sparse_read_struct->chunk_header.chunk_sz * sparse_header.blk_sz;
         sparse_read_struct->get_fill_chunk_value = FALSE;
      }

      chunk = (sparse_read_struct->chunk_remaining > length) ? length : sparse_read_struct->chunk_remaining;

      if(sparse_read_struct->chunk_header.chunk_type == CHUNK_TYPE_FILL)
      {
         if(sparse_read_struct->get_fill_chunk_value == FALSE)
         {
            fd->read(reinterpret_cast<char*>(&sparse_read_struct->fill_chunk_value), 4);
            if(fd->gcount() != 4)
            {
               dbg(
                  LOG_ERROR,
                  "Error when trying to read %llu bytes of data in "
                  "sparse_read\n",
                  chunk
               );
               ExitAndShowLog(1);
            }
            sparse_read_struct->get_fill_chunk_value = TRUE;
         }

         for(i = 0; i < chunk / 4; i++)
         {
            if(memscpy((data + data_offset + (i * 4)), 4, &sparse_read_struct->fill_chunk_value, 4) != 4)
            {
               dbg(
                  LOG_ERROR,
                  "Error when trying to read %llu bytes of data in "
                  "sparse_read\n",
                  chunk
               );
               ExitAndShowLog(1);
            }
         }
      }
      else
      {
         // ACTUALLY READING A BYTE OF DATA
         // OPTIMIZE TO READ AS MUCH AS POSSIBLE
         fd->read(data + data_offset, chunk);
         if(static_cast<uint64_t>(fd->gcount()) != chunk)
         {
            dbg(LOG_ERROR, "Error when trying to read %llu bytes of data in sparse_read\n", chunk);
            ExitAndShowLog(1);
         }
      }

      data_offset += chunk;
      bytes_read += chunk;
      sparse_read_struct->chunk_remaining -= chunk;
      sparse_read_struct->bytes_remaining -= chunk;
      length -= chunk;
   }

   return bytes_read;
}

void FirehoseLoader::sparse_close(struct sparse_handle_t* sparse_file_handle)
{
   // Deletes and frees all thing associated with the struct sparse_handle_t
   free(sparse_file_handle->start_sector);
   free(sparse_file_handle->bytes_remaining);
   sparse_file_handle->start_sector = nullptr;
   sparse_file_handle->bytes_remaining = nullptr;
   free(sparse_file_handle);
   sparse_file_handle = nullptr;
}

struct FirehoseLoader::sparse_header_t FirehoseLoader::read_sparse_header(std::shared_ptr<std::fstream> fd)
{
   struct sparse_header_t sparse_header;
   const size_t SPARSE_HEADER_LEN = sizeof(struct sparse_header_t);
   size_t result;
   // READ PAST THE FIRST HEADER, DONT NEED TO DO SANITY CHECKS. ALREADY DONE IN
   // OPEN
   fd->read(reinterpret_cast<char*>(&sparse_header), SPARSE_HEADER_LEN);
   result = fd->gcount();

   if(result != SPARSE_HEADER_LEN)
   {
      dbg(LOG_ERROR, "Didn't properly read the sparse_header!\n");
      ExitAndShowLog(1);
   }

   if(sparse_header.file_hdr_sz > SPARSE_HEADER_LEN)
   {
      // skip the remaining bytes in a header that is longer than we expected
      fd->seekg(sparse_header.file_hdr_sz - SPARSE_HEADER_LEN, std::ios::cur);
   }

   return sparse_header;
}

FirehoseLoader::boolean FirehoseLoader::TestIfSparse(std::shared_ptr<std::fstream> fd)
{
   struct sparse_header_t sparse_header;
   const size_t SPARSE_HEADER_LEN = sizeof(struct sparse_header_t);
   size_t result;
   // READ PAST THE FIRST HEADER, DONT NEED TO DO SANITY CHECKS. ALREADY DONE IN
   // OPEN
   fd->read(reinterpret_cast<char*>(&sparse_header), SPARSE_HEADER_LEN);
   result = fd->gcount();

   if(result != SPARSE_HEADER_LEN)
   {
      fd->seekg(0, std::ios::beg);
      return FALSE;
   }

   // Check if headers are correct
   if(sparse_header.magic != SPARSE_HEADER_MAGIC || sparse_header.major_version != SPARSE_HEADER_MAJOR_VER)
   {
      fd->seekg(0, std::ios::beg);
      return FALSE;
   }

   fd->seekg(0, std::ios::beg);
   return TRUE;
}

void FirehoseLoader::
   read_chunk_header(std::shared_ptr<std::fstream> fd, uint16_t chunk_hdr_sz, struct chunk_header_t* chunk_header)
{
   const size_t SPARSE_HEADER_LEN = sizeof(struct sparse_header_t);
   const size_t CHUNK_HEADER_LEN = sizeof(struct chunk_header_t);
   size_t result;

   fd->read(reinterpret_cast<char*>(chunk_header), CHUNK_HEADER_LEN);
   result = fd->gcount();

   if(result != CHUNK_HEADER_LEN)
   {
      dbg(LOG_ERROR, "Didn't get proper amount of bytes for chunk_header\n");
      ExitAndShowLog(1);
   }

   if(chunk_hdr_sz > CHUNK_HEADER_LEN)
   {
      // Skip the remaining bytes in a header that is longer than we expected
      fd->seekg(chunk_hdr_sz - CHUNK_HEADER_LEN, std::ios::cur);
   }

   // keep going to the next raw header
   while(chunk_header->chunk_type == CHUNK_TYPE_DONT_CARE)
   {
      fd->read(reinterpret_cast<char*>(chunk_header), CHUNK_HEADER_LEN);
      result = fd->gcount();

      if(result != CHUNK_HEADER_LEN)
      {
         dbg(LOG_ERROR, "Didn't get proper amount of bytes for chunk_header\n");
         ExitAndShowLog(1);
      }

      if(chunk_hdr_sz > CHUNK_HEADER_LEN)
      {
         // Skip the remaining bytes in a header that is longer than we expected
         fd->seekg(chunk_hdr_sz - CHUNK_HEADER_LEN, std::ios::cur);
      }
   }
}

struct FirehoseLoader::sparse_header_t FirehoseLoader::sparse_read_handle_init(
   std::shared_ptr<std::fstream> fd,
   SIZE_T file_sector_offset,
   SIZE_T FileSize,
   struct sparse_read_info_t* sparse_read_handle
)
{
   struct sparse_header_t sparse_header;
   sparse_header = read_sparse_header(fd); // read past the sparse header, can pull more
                                           // information out of this function if needed
   // seek to correct offset (from beginning)
   fd->seekg(file_sector_offset, std::ios::beg);
   // populate first chunk header stuff for sparse_read_handle
   sparse_read_handle->bytes_remaining = FileSize;
   read_chunk_header(fd, sparse_header.chunk_hdr_sz, &sparse_read_handle->chunk_header);
   sparse_read_handle->chunk_remaining = 1UL * sparse_read_handle->chunk_header.chunk_sz * sparse_header.blk_sz;
   sparse_read_handle->get_fill_chunk_value = FALSE;
   return sparse_header;
}

void FirehoseLoader::GenerateSparseXMLTags(
   std::shared_ptr<std::fstream> fd,
   char* filename,
   SIZE_T start_sector,
   SIZE_T SECTOR_SIZE_IN_BYTES,
   SIZE_T physical_partition
)
{
   struct sparse_handle_t* sparse_file_handle = nullptr;
   SIZE_T FileSizeNumSectors = 0;
   uint32_t i;
   dbg(
      LOG_INFO,
      "Reading through sparse file '%s' and pulling out relevant header "
      "information...",
      filename
   );
   sparse_file_handle = sparse_open(fd, start_sector, SECTOR_SIZE_IN_BYTES);

   dbg(
      LOG_INFO,
      "File %s is a sparse file, being split up into %d separate XML tags",
      filename,
      sparse_file_handle->total_groups
   );

   for(i = 0; i < sparse_file_handle->total_groups; i++)
   {
      FileSizeNumSectors = sparse_file_handle->bytes_remaining[i] / SECTOR_SIZE_IN_BYTES;

      if(sparse_file_handle->bytes_remaining[i] % SECTOR_SIZE_IN_BYTES)
      {
         FileSizeNumSectors++;
      }

      InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
      AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "%s ", "<program "); // the program tag
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "filename=\"%s\" ", filename);

      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SECTOR_SIZE_IN_BYTES);
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ",
              FileSizeNumSectors); // sparse_file_handle->
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "physical_partition_number=\"%" SIZE_T_FORMAT "\" ", physical_partition);
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "start_sector=\"%" SIZE_T_FORMAT "\" ", sparse_file_handle->start_sector[i]);
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "file_sector_offset=\"%" SIZE_T_FORMAT "\" ", sparse_file_handle->group_file_offset[i]);
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "sparse=\"%s\" ", "true");
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      sprintf(temp_buffer, "UNSPARSE_FILE_SIZE=\"%" SIZE_T_FORMAT "\" ", sparse_file_handle->bytes_remaining[i]);
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

      AppendToBuffer(tx_buffer, " />\n</data>", FIREHOSE_TX_BUFFER_SIZE);

      CopyString(
         XMLStringTable[NumXMLFilesInTable],
         tx_buffer,
         0,
         0,
         strlen(tx_buffer),
         MAX_XML_SIZE,
         FIREHOSE_TX_BUFFER_SIZE
      );
      NumXMLFilesInTable++;
   }

   sparse_close(sparse_file_handle);
}
/*----------------------------------------------------------------
----------------------- End Sparse File Functions-----------------
-----------------------------------------------------------------*/

void FirehoseLoader::addSlotAttribute(void)
{
   SIZE_T k = 0, TempLength = 0;
   char* pch;

   if(fh.attrs.slot == 0)
   {
      return;
   }

   while(XMLStringTable[k][0] != '\0')
   {
      if(strstr(XMLStringTable[k], "<configure ") != nullptr || strstr(XMLStringTable[k], "<nop ") != nullptr ||
         strstr(XMLStringTable[k], "<power ") != nullptr || strstr(XMLStringTable[k], "<peek ") != nullptr ||
         strstr(XMLStringTable[k], "<poke ") != nullptr)
      {
         k++;
         continue;
      }

      memset(temp_buffer, 0x0, FIREHOSE_TX_BUFFER_SIZE); // zero out to begin

      pch = strstr(XMLStringTable[k], "slot");

      if(pch == NULL)
      {
         pch = strstr(XMLStringTable[k], "</data>");

         TempLength = pch - XMLStringTable[k];

         CopyString(temp_buffer, XMLStringTable[k], 0, 0, TempLength, FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);

         pch = strstr(temp_buffer, "/>");

         if(pch == NULL)
         {
            dbg(LOG_ERROR, "Missing />");
            ExitAndShowLog(1);
         }

         snprintf_x(pch, sizeof(XMLStringTable[k]) - TempLength, "slot=\"%" SIZE_T_FORMAT "\" /></data>", fh.attrs.slot);
      }
      else
      {
         snprintf_x(pch, sizeof(XMLStringTable[k]) - TempLength, "slot=\"%" SIZE_T_FORMAT "\"", fh.attrs.slot);
      }

      memscpy(XMLStringTable[k], MAX_XML_SIZE, temp_buffer,
              strlen(temp_buffer)); // memcpy

      k++;

   } // end while
}

int32_t FirehoseLoader::processCommand(int32_t argc, char* argv[])
{
   char szTemp[2048] = "19-2";
   int32_t szIndex = 0;
   SIZE_T i, j, k;
   enum fh_reboot_options reboot_option = FH_REBOOT_NONE;
   uint32_t reboot_seconds = 0;
   std::shared_ptr<std::fstream> fd = nullptr;
   char build_month[2];
   SIZE_T MyLun = 0, MyStartSector = 0, MyNumSectors = 0, MyTrials = 10;
   int32_t use_json_input = 0;
   char fh_loader_version[18] = {
      // Build the version information.
      __DATE__[9],
      __DATE__[10],
      '.', // Year
      __DATE__[0],
      __DATE__[2],
      '.', // Month
      __DATE__[4],
      __DATE__[5],
      '.', // Day
      __TIME__[0],
      __TIME__[1],
      '.', // Hour
      __TIME__[3],
      __TIME__[4],
      '.', // minute
      __TIME__[6],
      __TIME__[7],
      0, // seconds
   };

   get_build_month(build_month);
   fh_loader_version[3] = build_month[0];
   fh_loader_version[4] = build_month[1];

   LoadBuffer(temp_buffer, ONE_MEGABYTE);
   // MyCopyFile("C:\\Users\\ahughes\\hello.txt","C:\\Users\\ahughes\\hello2.txt");
   // FindPartitionByLabel(0, "mom", "gpt_main0.bin");
   // exit(0);

   dbg(LOG_INFO, "\nBinary build date: %s @ %s\n", __DATE__, __TIME__);
   dbg(LOG_INFO, "Build version: %s\n", fh_loader_version);

   /*
     struct timeval time_start, time_end, AbsoluteTimeStart;
       double ElaspedTime;

     gettimeofday(&time_start, NULL);

       dbg (LOG_INFO, "\n");
       for(i=0;i<5;i++)
       {
           dbg (LOG_INFO, ".\n");
           sleep(1);
       }

     gettimeofday(&time_end, NULL);
       ElaspedTime = ReturnTimeInSeconds(&time_start,&time_end);
       dbg (LOG_INFO, "ElaspedTime = %f\n",ElaspedTime);
       exit(0);
   */
   // MyParseExpression(szTemp, 20, &i);
   // parseExpression(szTemp,&i);


   /* check argument count, print the usage help and quit */
   if(argc < 2)
   {
      print_usage();
      exit(0);
   }

   firehose_tag_data.insert(std::make_pair(std::string("program"), &FirehoseLoader::handleProgram));
   firehose_tag_data.insert(std::make_pair(std::string("firmwarewrite"), &FirehoseLoader::handleFirmwareWrite));
   firehose_tag_data.insert(std::make_pair(std::string("configure"), &FirehoseLoader::handleConfigure));
   firehose_tag_data.insert(std::make_pair(std::string("power"), &FirehoseLoader::handlePower));
   firehose_tag_data
      .insert(std::make_pair(std::string("setbootablestoragedrive"), &FirehoseLoader::handleSetBootableStorageDrive));
   firehose_tag_data.insert(std::make_pair(std::string("nop"), &FirehoseLoader::handleNop));
   firehose_tag_data.insert(std::make_pair(std::string("erase"), &FirehoseLoader::handleErase));
   firehose_tag_data.insert(std::make_pair(std::string("read"), &FirehoseLoader::handleRead));
   firehose_tag_data.insert(std::make_pair(std::string("log"), &FirehoseLoader::handleLog));
   firehose_tag_data.insert(std::make_pair(std::string("response"), &FirehoseLoader::handleResponse));
   firehose_tag_data.insert(std::make_pair(std::string("options"), &FirehoseLoader::handleOptions));
   firehose_tag_data.insert(std::make_pair(std::string("file_mappings"), &FirehoseLoader::handleFileMappings));
   firehose_tag_data.insert(std::make_pair(std::string("search_paths"), &FirehoseLoader::handleSearchPaths));
   firehose_tag_data.insert(std::make_pair(std::string("rawprogram"), &FirehoseLoader::handleRawProgram));
   firehose_tag_data.insert(std::make_pair(std::string("patch"), &FirehoseLoader::handlePatchProgram));
   firehose_tag_data.insert(std::make_pair(std::string("benchmark"), &FirehoseLoader::handleBenchmark));

   InitAttributes(); // all the fh.attrs

   std::string tempPath = Device::Manager::getInstance()->getTempDirectory().string().c_str();
   strncpy(cwd, reinterpret_cast<const char*>(tempPath.c_str()), tempPath.size() + 1);
   i = strlen(cwd);
   cwd[i] = SLASH;
   cwd[i + 1] = '\0';

   PromptUser = 0; // turn off here. So that any mistake at this point just exits


   // PASS 1 OF COMMAND LINE OPTIONS
   //    PURPOSE: Make sure options are valid

   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);

      if(PartOfCommandLineOptions(MyOpt) == 0)
      {
         ShowCommandLineOptions();

         dbg(LOG_ERROR, "Command line argument '%s' is not recognized as a valid option", MyOpt);
         ExitAndShowLog(1);
      }
   }

   // Quick sanity check
   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);

      if(MyOpt[strlen(MyOpt) - 1] == '=')
      {
         // did they provide an argument?
         if(strlen(MyArg) == 0)
         {
            dbg(LOG_ERROR, "Missing argument for --%s", MyOpt);
            ExitAndShowLog(1);
         }
      }
   }

   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);
      // dbg (LOG_INFO, "Looks like opt='%s' and arg='%s'\n\n",MyOpt,MyArg);

      if(strncmp(MyOpt, "porttracename=", MAX(strlen(MyOpt), strlen("porttracename"))) == 0)
      {
         // dbg (LOG_INFO, "User changed '%s' to '%s'\n", PortTraceName, MyArg);

         // if ( CopyString (PortTraceName, MyArg, 0, 0, strlen (MyArg),
         // MAX_PATH_SIZE, strlen (MyArg) )  == 0)
         //{
         //     dbg (LOG_ERROR, "Failed to copy '%s' of length %" SIZE_T_FORMAT
         //     " bytes into PortTraceName", MyArg, strlen (MyArg) );
         //     ExitAndShowLog (1);
         // }

         continue;
      }
      else if(strncmp(MyOpt, "mainoutputdir=", MAX(strlen(MyOpt), strlen("mainoutputdir="))) == 0)
      {
         setReadPath(MyArg);
      }
   }

   InitLogging(); // Init Logging

   std::string cmdString =
      "FH_LOADER WAS CALLED EXACTLY LIKE "
      "THIS\n************************************************\n";

   for(i = 0; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
      cmdString += " " + std::string(argv[i]);

   cmdString += ("\n************************************************\n");
   szIndex = 0;
   dbg(LOG_INFO, cmdString.c_str());


   dbg(LOG_DEBUG, "Binary build date: %s @ %s\n", __DATE__, __TIME__);
   dbg(LOG_DEBUG, "Build Version: %s\n", fh_loader_version);
   dbg(LOG_INFO, "Current working dir (cwd): %s", cwd);
   dbg(LOG_INFO, "Showing network mappings to allow debugging");

   PromptUser = 1; // this turns on here, user can turn off below with --noprompt

   // PASS 2 OF COMMAND LINE OPTIONS
   //    PURPOSE: Options that must be first, such as search_paths to allow
   //    finding of files possibly passed as other command line options
   //         erasefirst and wipefirst are flags that affect the <program> tag
   //         by adding erase/wipe prior to programming
   //               so if user used the --sendfile option, and --wipefirst I
   //               need to know

   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);
      // dbg (LOG_INFO, "Looks like opt='%s' and arg='%s'\n\n",MyOpt,MyArg);
      // get memory type first
      if(strncmp(MyOpt, "memoryname=", MAX(strlen(MyOpt), strlen("memoryname="))) == 0)
      {
         if(MyArg[0] == 'u' || MyArg[0] == 'U')
         {
            if(CopyString(fh.attrs.MemoryName, "ufs", 0, 0, strlen("ufs"), sizeof(fh.attrs.MemoryName), sizeof("ufs")) ==
               0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.MemoryName",
                  "ufs",
                  strlen("ufs")
               );
               ExitAndShowLog(1);
            }
         }
         else if(MyArg[0] == 'n' && MyArg[1] == 'a')
         {
            if(CopyString(
                  fh.attrs.MemoryName,
                  "nand",
                  0,
                  0,
                  strlen("nand"),
                  sizeof(fh.attrs.MemoryName),
                  sizeof("nand")
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.MemoryName",
                  "nand",
                  strlen("nand")
               );
               ExitAndShowLog(1);
            }
         }
         else if(MyArg[0] == 's')
         {
            if(CopyString(
                  fh.attrs.MemoryName,
                  "spinor",
                  0,
                  0,
                  strlen("spinor"),
                  sizeof(fh.attrs.MemoryName),
                  sizeof("spinor")
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.MemoryName",
                  "spinor",
                  strlen("spinor")
               );
               ExitAndShowLog(1);
            }
         }
         else if(MyArg[0] == 'n' && MyArg[1] == 'v')
         {
            if(CopyString(
                  fh.attrs.MemoryName,
                  "NVMe",
                  0,
                  0,
                  strlen("NVMe"),
                  sizeof(fh.attrs.MemoryName),
                  sizeof("NVMe")
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.MemoryName",
                  "NVMe",
                  strlen("NVMe")
               );
               ExitAndShowLog(1);
            }
         }
         else if(MyArg[0] == 'n' && MyArg[1] == 'u')
         {
            if(CopyString(
                  fh.attrs.MemoryName,
                  "null",
                  0,
                  0,
                  strlen("null"),
                  sizeof(fh.attrs.MemoryName),
                  sizeof("null")
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.MemoryName",
                  "null",
                  strlen("null")
               );
               ExitAndShowLog(1);
            }
         }
         else
         {
            if(CopyString(
                  fh.attrs.MemoryName,
                  "emmc",
                  0,
                  0,
                  strlen("emmc"),
                  sizeof(fh.attrs.MemoryName),
                  sizeof("emmc")
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.MemoryName",
                  "emmc",
                  strlen("emmc")
               );
               ExitAndShowLog(1);
            }
         }

         if(fh.attrs.MemoryName[0] == 'u' || fh.attrs.MemoryName[0] == 's')
         {
            fh.attrs.SECTOR_SIZE_IN_BYTES = 4096;
            SectorSizeInBytes = 4096;
         }
         else // if(fh.attrs.MemoryName[0]=='e')
         {
            fh.attrs.SECTOR_SIZE_IN_BYTES = 512;
            SectorSizeInBytes = 512;
         }

         continue;
      }
   }
   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);
      if(strncmp(MyOpt, "digestsperfilename=", MAX(strlen(MyOpt), strlen("digestsperfilename"))) == 0)
      {
         dbg(LOG_INFO, "User changed '%s' to '%s'\n", DigestsPerFileName, MyArg);

         if(CopyString(DigestsPerFileName, MyArg, 0, 0, strlen(MyArg), MAX_PATH_SIZE, strlen(MyArg)) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into DigestsPerFileName",
               MyArg,
               strlen(MyArg)
            );
            ExitAndShowLog(1);
         }

         continue;
      }
      else if(strncmp(MyOpt, "noprompt", MAX(strlen(MyOpt), strlen("noprompt"))) == 0)
      {
         PromptUser = 0; // will no longer ask user, will instead just do the default
         continue;
      }
      else if(strncmp(MyOpt, "convertprogram2read", MAX(strlen(MyOpt), strlen("convertprogram2read"))) == 0)
      {
         ConvertProgram2Read = 1;
      }
      else if(strncmp(MyOpt, "erase=", MAX(strlen(MyOpt), strlen("erase="))) == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<erase ", FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SectorSizeInBytes);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "physical_partition_number=\"%d\" ", atoi(MyArg));
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         if(fh.attrs.MemoryName[0] == 'n' || fh.attrs.MemoryName[1] == 'a')
         {
            AppendToBuffer(tx_buffer, "num_partition_sectors=\"4294967295\" ", FIREHOSE_TX_BUFFER_SIZE);
         }

         //------------------------------------------------------
         // Exclude partition from erasing based on start sector
         //  --excludeerasepartitioninfo=0:6,1:66,2:666,3:6666,4:66666,5:666666,6:6666666,7:66666666
         // exclude partition information will be provided with comma separated
         // string
         //------------------------------------------------------
         char* key = new char[MAX_STRING_SIZE];
         char* value = new char[MAX_STRING_SIZE];
         SIZE_T count = 0;
         for(count = 1; count < (SIZE_T)argc; count++)
         {
            getoptarg(static_cast<int32_t>(count), argv, key, MAX_STRING_SIZE, value, MAX_STRING_SIZE);
            if(strncmp(key, "excludeerasepartitioninfo=", MAX(strlen(key), strlen("excludeerasepartitioninfo="))) == 0)
            {
               std::vector<std::string> indices = Util::split(std::string(value), ',');

               for(uint64_t iteIndices = 0; indices.size() > iteIndices; ++iteIndices)
               {
                  std::vector<std::string> sectors = Util::split(std::string(indices[iteIndices]), ':');

                  if(sectors.size() >= 2 && sectors.at(0) == MyArg)
                  {
                     sprintf(temp_buffer, "skip_cdt=\"%d\" ", atoi(sectors.at(1).c_str()));
                     AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
                     break;
                  }
               }
            }
         }

         delete[] key;
         delete[] value;

         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      } // end of nop

      if(strncmp(MyOpt, "files=", MAX(strlen(MyOpt), strlen("files="))) == 0)
      {
         // MyArg is split on commas, results into
         // filter_files[MAX_XML_FILES][MAX_PATH_SIZE]
         num_filter_files = SplitStringOnCommas(
            MyArg,
            MAX_STRING_SIZE,
            filter_files,
            static_cast<int32_t>(num_filter_files),
            MAX_XML_FILES,
            MAX_PATH_SIZE
         );

         filter_files[num_filter_files][0] = '\0'; // j is how many strings were valid, make sure last one in
                                                   // table is null

         continue;
      }

      if(strncmp(MyOpt, "notfiles=", MAX(strlen(MyOpt), strlen("notfiles="))) == 0)
      {
         // MyArg is split on commas, results into
         // filter_not_files[MAX_XML_FILES][MAX_PATH_SIZE]
         num_filter_not_files = SplitStringOnCommas(
            MyArg,
            MAX_STRING_SIZE,
            filter_not_files,
            static_cast<int32_t>(num_filter_not_files),
            MAX_XML_FILES,
            MAX_PATH_SIZE
         );

         filter_not_files[num_filter_not_files][0] = '\0'; // j is how many strings were valid, make sure last one in
                                                           // table is null

         continue;
      }
   }

   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);
      // dbg (LOG_INFO, "Looks like opt='%s' and arg='%s'\n\n",MyOpt,MyArg);

      if(strncmp(MyOpt, "search_path=", MAX(strlen(MyOpt), strlen("search_path="))) == 0)
      {
         // MyArg is split on commas, results into
         // search_path[MAX_XML_FILES][MAX_PATH_SIZE]
         num_search_paths = SplitStringOnCommas(
            MyArg,
            MAX_STRING_SIZE,
            search_path,
            static_cast<int32_t>(num_search_paths),
            MAX_XML_FILES,
            MAX_PATH_SIZE
         );

         search_path[num_search_paths][0] = '\0'; // j is how many strings were valid, make sure last one in
                                                  // table is null

         CleanseSearchPaths();
         continue;
      }
      else if(strncmp(MyOpt, "firmwarewrite", MAX(strlen(MyOpt), strlen("firmwarewrite"))) == 0)
      {
         ConvertProgram2Firmware = 1;
         fh.attrs.SECTOR_SIZE_IN_BYTES = 1;
         SectorSizeInBytes = 1;
      }
      else if(strncmp(MyOpt, "sectorsizeinbytes=", MAX(strlen(MyOpt), strlen("sectorsizeinbytes="))) == 0)
      {
         fh.attrs.SECTOR_SIZE_IN_BYTES = atoi(MyArg);
         SectorSizeInBytes = atoi(MyArg);
         dbg(LOG_DEBUG, "User set SECTOR_SIZE_IN_BYTES to %ld\n", fh.attrs.SECTOR_SIZE_IN_BYTES);
         continue;
      }
      else if(strncmp(MyOpt, "contentsxml=", MAX(strlen(MyOpt), strlen("contentsxml="))) == 0)
      {
         CopyString(InFileListFilename, MyArg, 0, 0, strlen(MyArg), MAX_STRING_SIZE, MAX_STRING_SIZE);
         use_json_input = 0;
      }
      else if(strncmp(MyOpt, "json_in=", MAX(strlen(MyOpt), strlen("json_in="))) == 0)
      {
         CopyString(InFileListFilename, MyArg, 0, 0, strlen(MyArg), MAX_STRING_SIZE, MAX_STRING_SIZE);
         use_json_input = 1;
      }
      else if(strncmp(MyOpt, "flavor=", MAX(strlen(MyOpt), strlen("flavor="))) == 0)
      {
         CopyString(flavor, MyArg, 0, 0, strlen(MyArg), MAX_STRING_SIZE, MAX_STRING_SIZE);
      }
      else if(strncmp(MyOpt, "createconfigxml", MAX(strlen(MyOpt), strlen("createconfigxml"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--createconfigxml parameter is deprecated and "
            "scheduled to be removed"
         );
         createconfigxml = 1;
         Simulate = 1;
      }
      else if(strncmp(MyOpt, "flattenbuildto=", MAX(strlen(MyOpt), strlen("flattenbuildto="))) == 0)
      {
         if(CopyString(flattenbuildto, MyArg, 0, 0, strlen(MyArg), MAX_STRING_SIZE, MAX_STRING_SIZE) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildto",
               MyArg,
               strlen(MyArg)
            );
            ExitAndShowLog(1);
         }

         if(flattenbuildto[strlen(flattenbuildto)] == '\0')
         {
            flattenbuildto[strlen(flattenbuildto) + 1] = '\0';
            flattenbuildto[strlen(flattenbuildto)] = SLASH;
         }

         if(!HasAPathCharacter(flattenbuildto, strlen(flattenbuildto)))
         {
            // to be here means user entered a relative path, i.e. something
            // like path2\path3 instead of c:\path1\path2\path3
            CopyString(temp_buffer, cwd, 0, 0, strlen(cwd), MAX_PATH_SIZE, MAX_PATH_SIZE);
            CopyString(
               temp_buffer,
               flattenbuildto,
               strlen(temp_buffer),
               0,
               strlen(flattenbuildto),
               MAX_PATH_SIZE,
               MAX_PATH_SIZE
            );
            dbg(LOG_INFO, "'%s' changed to", flattenbuildto);
            CopyString(flattenbuildto, temp_buffer, 0, 0, strlen(temp_buffer), MAX_PATH_SIZE, MAX_PATH_SIZE);
            dbg(LOG_INFO, "this '%s'", flattenbuildto);
         }


         dbg(LOG_INFO, "Trying to create directory for --flattenbuildto='%s'", flattenbuildto);

         // user provided this path, therefore save files into it
         memset(flattenbuildvariant, 0x0, MAX_STRING_SIZE);

         if(CopyString(
               flattenbuildvariant,
               flattenbuildto,
               strlen(flattenbuildvariant),
               0,
               strlen(flattenbuildto),
               MAX_STRING_SIZE,
               MAX_STRING_SIZE
            ) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildvariant",
               flattenbuildto,
               strlen(flattenbuildto)
            );
            ExitAndShowLog(1);
         }

         if(CopyString(
               flattenbuildvariant,
               fh.attrs.MemoryName,
               strlen(flattenbuildvariant),
               0,
               strlen(fh.attrs.MemoryName),
               MAX_STRING_SIZE,
               strlen(fh.attrs.MemoryName)
            ) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy fh.attrs.MemoryName string '%s' of length "
               "%" SIZE_T_FORMAT " bytes into flattenbuildvariant",
               fh.attrs.MemoryName,
               strlen(fh.attrs.MemoryName)
            );
            ExitAndShowLog(1);
         }

         if(CopyString(
               flattenbuildvariant,
               "\\",
               strlen(flattenbuildvariant),
               0,
               strlen("\\"),
               MAX_STRING_SIZE,
               strlen("\\")
            ) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildvariant",
               "\\",
               strlen("\\")
            );
            ExitAndShowLog(1);
         }

         dbg(
            LOG_INFO,
            "Trying to create directory '%s' since fh.attrs.MemoryName is "
            "'%s'",
            flattenbuildvariant,
            fh.attrs.MemoryName
         );

         Util::createPath(flattenbuildvariant);


         dbg(
            LOG_DEBUG,
            "User wants FLATTEN BUILD - This will be a SIMULATE - "
            "Not reading/writing from a port\n"
         );
         Simulate = 1;
         FlattenBuild = 1;
         createconfigxml = 1;
         AllowReset = 0; // won't send <power DelayInSeconds="2" value="reset" /> anymore
      }
      else if(strncmp(MyOpt, "wipefirst", MAX(strlen(MyOpt), strlen("wipefirst"))) == 0)
      {
         std::shared_ptr<std::fstream> fTemp;
         char MyBufferOfZeros[16 * 1024] = {0};
         dbg(
            LOG_DEBUG,
            "User wants wipefirst - Will write zeros for the first "
            "16KB of every <program> tag\n"
         );
         wipefirst = 1;
         // Need to create the 16KB file of zeros
         fTemp = ReturnFileHandle(WipeFirstFileName, strlen(WipeFirstFileName),
                                  "wb"); // will exit if not successful
         fTemp->write(MyBufferOfZeros, sizeof(MyBufferOfZeros));
         fh_fclose(fTemp);
         fTemp = nullptr;
         continue;
      }
      else if(strncmp(MyOpt, "erasefirst", MAX(strlen(MyOpt), strlen("erasefirst"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--erasefirst parameter is deprecated and scheduled to be "
            "removed"
         );
         dbg(
            LOG_DEBUG,
            "User wants erasefirst - Will issue <erase> tag before "
            "every <program> tag\n"
         );
         erasefirst = 1;

         if(fh.attrs.MemoryName[0] == 'n' || fh.attrs.MemoryName[1] == 'a')
         {
            // looks like they chose nand || NAND
         }
         else
         {
            dbg(
               LOG_ERROR,
               "\n\nIt looks like you chose --erasefirst with "
               "MemoryName='%s'.\nThis is a --memoryname=NAND feature!!"
               "\n\n\tWhat you are asking to do here is issue an <erase> "
               "command for every <program> tag"
               "\n\tBut on '%s' this will have the effect of erasing the "
               "entire device *every time* for each <program> tag.\n\tAt a "
               "high level this means only your last file will be programmed!!"
               "\n\n\nYou don't want this feature and you are looking for "
               "--erase\n\n"
               "\n\n\t\tSomething like:    fh_loader.exe --erase=0 --port= "
               "etc etc\n\n",
               fh.attrs.MemoryName,
               fh.attrs.MemoryName
            );
            ExitAndShowLog(1);
         }

         continue;
      }
   }

   if(use_json_input == 0)
      ParseContentsXML(InFileListFilename); // "\\\\sundae\\builds664\\PROD\\M8994AAAAANLYD1024.4\\contents.xml"
   else
      metacli_json_in(InFileListFilename);

   // add all the search paths
   for(i = 0; i < NumContensXML; i++)
   {
      // StorageType; // 0=unknown, 'e'=emmc,'u'=ufs,'n'=nand
      // FileType;  // 0=unknown,'r'=<partition_file
      // (rawprogram),'p'=<partition_patch_file (patch)

      if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
      {
         if(!AlreadyHaveThisPath(ContensXML[i].Path))
         {
            if(CopyString(
                  search_path[num_search_paths],
                  ContensXML[i].Path,
                  0,
                  0,
                  strlen(ContensXML[i].Path),
                  MAX_PATH_SIZE,
                  MAX_PATH_SIZE
               ) == 0)
            {
               dbg(LOG_ERROR, "Failed to copy string '%s' into search_path[%i]", ContensXML[i].Path, num_search_paths);
               return 1;
            }

            num_search_paths++;
         }
      }
   }

   if(NumContensXML > 0)
   {
      // to be here means ParseContentsXML(InFileListFilename) returned
      // something, so fill in the rawprograms and search_paths etc
      for(i = 0; i < NumContensXML; i++)
      {
         if(ContensXML[i].FileType == 'r') // && ContensXML[i].StorageType==fh.attrs.MemoryName[0])
         {
            // this is a rawprogram file
            if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
            {
               // std::shared_ptr<std::fstream> fTemp =
               // ReturnFileHandle(ContensXML[i].Filename, MAX_PATH_SIZE, "r");
               // // will exit if not successful fh_fclose(fTemp);
               if(CopyString(
                     XMLFileTable[num_xml_files_to_send],
                     ContensXML[i].Path,
                     0,
                     0,
                     strlen(ContensXML[i].Path),
                     MAX_PATH_SIZE,
                     MAX_PATH_SIZE
                  ) == 0)
               {
                  dbg(
                     LOG_ERROR,
                     "Failed to copy string '%s' of length %i into "
                     "XMLFileTable[%i]",
                     ContensXML[i].Filename,
                     strlen(ContensXML[i].Filename),
                     num_xml_files_to_send
                  );
                  return 1;
               }

               if(CopyString(
                     XMLFileTable[num_xml_files_to_send],
                     ContensXML[i].Filename,
                     strlen(ContensXML[i].Path),
                     0,
                     strlen(ContensXML[i].Filename),
                     MAX_PATH_SIZE,
                     MAX_ATTR_NAME_SIZE
                  ) == 0)
               {
                  dbg(
                     LOG_ERROR,
                     "Failed to copy string '%s' of length %i into "
                     "XMLFileTable[%i]",
                     ContensXML[i].Filename,
                     strlen(ContensXML[i].Filename),
                     num_xml_files_to_send
                  );
                  return 1;
               }

               num_xml_files_to_send++;
            }
         } // end if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType ||
           // ContensXML[i].StorageType==0 )
      }

      OpenAndStoreAllXMLFiles();
      num_xml_files_to_send = 0;

      for(i = 0; i < NumContensXML; i++)
      {
         // now load the patch files
         if(ContensXML[i].FileType == 'p') // && ContensXML[i].StorageType==fh.attrs.MemoryName[0])
         {
            if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
            {
               // this is a patch file
               // std::shared_ptr<std::fstream> fTemp =
               // ReturnFileHandle(ContensXML[i].Filename, MAX_PATH_SIZE, "r");
               // // will exit if not successful fh_fclose(fTemp);
               if(CopyString(
                     XMLFileTable[num_xml_files_to_send],
                     ContensXML[i].Path,
                     0,
                     0,
                     strlen(ContensXML[i].Path),
                     MAX_PATH_SIZE,
                     MAX_PATH_SIZE
                  ) == 0)
               {
                  dbg(
                     LOG_ERROR,
                     "Failed to copy string '%s' of length %i into "
                     "XMLFileTable[%i]",
                     ContensXML[i].Filename,
                     strlen(ContensXML[i].Filename),
                     num_xml_files_to_send
                  );
                  return 1;
               }

               if(CopyString(
                     XMLFileTable[num_xml_files_to_send],
                     ContensXML[i].Filename,
                     strlen(ContensXML[i].Path),
                     0,
                     strlen(ContensXML[i].Filename),
                     MAX_PATH_SIZE,
                     MAX_ATTR_NAME_SIZE
                  ) == 0)
               {
                  dbg(
                     LOG_ERROR,
                     "Failed to copy string '%s' of length %i into "
                     "XMLFileTable[%i]",
                     ContensXML[i].Filename,
                     strlen(ContensXML[i].Filename),
                     num_xml_files_to_send
                  );
                  return 1;
               }

               num_xml_files_to_send++;
            }
         } // end if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType ||
           // ContensXML[i].StorageType==0 )
      }

      OpenAndStoreAllXMLFiles();
      num_xml_files_to_send = 0;

   } // end of if(NumContensXML>0)


   // PASS 3 OF COMMAND LINE OPTIONS
   //    PURPOSE: Most command line options parsed here

   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);
      // dbg (LOG_INFO, "Looks like opt='%s' and arg='%s'\n\n",MyOpt,MyArg);

      if(strncmp(MyOpt, "port=", MAX(strlen(MyOpt), strlen("port="))) == 0)
      {
         if(CopyString(port_name, MyArg, 0, 0, strlen(MyArg), MAX_STRING_SIZE, MAX_STRING_SIZE) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into port_name",
               MyArg,
               strlen(MyArg)
            );
            ExitAndShowLog(1);
         }

         continue;
      }
      else if(strncmp(MyOpt, "loglevel=", MAX(strlen(MyOpt), strlen("loglevel="))) == 0)
      {
         k = atoi(MyArg);

         if(k == 0) // user wants silent
         {
            dbg(LOG_ALWAYS, "User requested *minimal* logging with --loglevel=0");
            VerboseLevel = LOG_ALWAYS; // LOG_ALWAYS or lower shown (i.e. LOG_ERROR)
         }
         else if(k == 2) // user wants superverbose <-- log will show HEX dump
                         // of RAW packets too BIG LOG
         {
            dbg(LOG_ALWAYS, "User requested *verbose* logging with --loglevel=2");
            VerboseLevel = LOG_DEBUG; // Everything is shown
            createcommandtrace = 1;
         }
         else if(k == 3) // user wants superverbose <-- log will show HEX dump
                         // of RAW packets too BIG LOG
         {
            dbg(LOG_ALWAYS, "User requested *super verbose* logging with --loglevel=3");
            VerboseLevel = LOG_DEBUG; // Everything is shown
            PrettyPrintRawPacketsToo = 1;
            createcommandtrace = 1;
         }
         else // user wants normal
         {
            dbg(
               LOG_ALWAYS,
               "User requested *semi-verbose* logging with "
               "--loglevel=1 - This is default"
            );
            VerboseLevel = LOG_INFO; // Everything is shown
         }

         continue;
      }
      else if(strncmp(MyOpt, "interactive=", MAX(strlen(MyOpt), strlen("interactive="))) == 0)
      {
         dbg(
            LOG_WARN,
            "--interactive= parameter is deprecated and scheduled "
            "to be removed"
         );
         dbg(LOG_DEBUG, "User wants INTERACTIVE\n");
         Interactive = static_cast<uint8_t>(atoi(MyArg));

         if(Interactive > 2) Interactive = 2; // 1 = XML files only, 2 means RAWDATA also

         continue;
      }
      else if(strncmp(MyOpt, "lun=", MAX(strlen(MyOpt), strlen("lun="))) == 0)
      {
         dbg(LOG_DEBUG, "User is specifying lun=%s\n", MyArg);
         MyLun = (SIZE_T)atoi(MyArg);
         dbg(LOG_DEBUG, "MyLun=%" SIZE_T_FORMAT "\n", MyLun);

         if(MyLun > 7)
            dbg(
               LOG_WARN,
               "In most cases devices only have LUN 0,1,2,3,4,5,6,7. You have "
               "specified to use LUN %" SIZE_T_FORMAT " and this might fail\n\n",
               MyLun
            );

         continue;
      }
      else if(strncmp(MyOpt, "num_sectors=", MAX(strlen(MyOpt), strlen("num_sectors="))) == 0)
      {
         boolean num_conversion;

         dbg(LOG_DEBUG, "User is specifying num_sectors=%s\n", MyArg);

         MyNumSectors = stringToNumber((const char*)MyArg, &num_conversion);

         if(FALSE == num_conversion)
         {
            dbg(
               LOG_INFO,
               "Call to stringToNumber failed for MyNumSectors with value "
               "'%s'",
               MyArg
            );
            ExitAndShowLog(1);
         }

         dbg(LOG_DEBUG, "MyNumSectors=%" SIZE_T_FORMAT "\n", MyNumSectors);
         continue;
      }
      else if(strncmp(MyOpt, "trials=", MAX(strlen(MyOpt), strlen("trials="))) == 0)
      {
         boolean num_conversion;

         dbg(LOG_WARN, "--trials= parameter is deprecated and scheduled to be removed");
         dbg(LOG_DEBUG, "User is specifying trials=%s\n", MyArg);

         MyTrials = stringToNumber((const char*)MyArg, &num_conversion);

         if(FALSE == num_conversion)
         {
            dbg(LOG_INFO, "Call to stringToNumber failed for MyTrials with value '%s'", MyArg);
            ExitAndShowLog(1);
         }

         dbg(LOG_DEBUG, "MyTrials=%" SIZE_T_FORMAT "\n", MyTrials);
         continue;
      }
      else if(strncmp(MyOpt, "start_sector=", MAX(strlen(MyOpt), strlen("start_sector="))) == 0)
      {
         boolean num_conversion;

         dbg(LOG_DEBUG, "User is specifying start_sector=%s\n", MyArg);

         MyStartSector = stringToNumber((const char*)MyArg, &num_conversion);

         if(FALSE == num_conversion)
         {
            dbg(
               LOG_INFO,
               "Call to stringToNumber failed for MyStartSector with value "
               "'%s'",
               MyArg
            );
            ExitAndShowLog(1);
         }

         dbg(LOG_DEBUG, "MyStartSector=%" SIZE_T_FORMAT "\n", MyStartSector);
         continue;
      }
      else if(strncmp(MyOpt, "noreset", MAX(strlen(MyOpt), strlen("noreset"))) == 0)
      {
         dbg(LOG_WARN, "--noreset parameter is deprecated and scheduled to be removed");
         AllowReset = 0; // won't send <power DelayInSeconds="2" value="reset" /> anymore
         continue;
      }
      else if(strncmp(MyOpt, "reset", MAX(strlen(MyOpt), strlen("reset"))) == 0)
      {
         dbg(LOG_WARN, "--reset parameter is deprecated and scheduled to be removed");
         AllowReset = 1; // won't send <power DelayInSeconds="2" value="reset" /> anymore
         continue;
      }
      else if(strncmp(MyOpt, "power=", MAX(strlen(MyOpt), strlen("power="))) == 0)
      {
         char* power_opt;
         const char* power_time;
         char* split;
         char* end;
         split = strstr(MyArg, ",");
         if(split == NULL)
         {
            dbg(
               LOG_ERROR,
               "Invalid command line option %s expect "
               "--power=<Option>,time_in_seconds, for example --power=reset,1 "
               "to reset the device after a 1 second delay.",
               MyArg
            );
            ExitAndShowLog(1);
         }
         *split = '\0';
         power_opt = MyArg;
         power_time = split + 1;
         if(strcmp("edl", power_opt) == 0)
            reboot_option = FH_REBOOT_EDL;
         else if(strcmp("reset", power_opt) == 0)
            reboot_option = FH_REBOOT_RESET;
         else if(strcmp("off", power_opt) == 0)
            reboot_option = FH_REBOOT_POWER_OFF;
         else
         {
            dbg(LOG_ERROR, "Invalid power selection %s expect edl, reset or off", power_opt);
            ExitAndShowLog(1);
         }
         reboot_seconds = strtol(power_time, &end, 10);
         if(power_time == end || *end != '\0' || reboot_seconds > FH_MAX_DEVICE_DELAY_FOR_POWER_OP)
         {
            dbg(
               LOG_ERROR,
               "Invalid power selection for time in seconds %s expected "
               "decimal value greater or equal to 0 and less than %d",
               power_time,
               FH_MAX_DEVICE_DELAY_FOR_POWER_OP
            );
            ExitAndShowLog(1);
         }

         continue;
      }
      else if(strncmp(MyOpt, "signeddigests=", MAX(strlen(MyOpt), strlen("signeddigests="))) == 0)
      {
         CopyString(SignedDigestTableFilename, MyArg, 0, 0, strlen(MyArg), MAX_STRING_SIZE, MAX_STRING_SIZE);
      }
      else if(strncmp(MyOpt, "chaineddigests=", MAX(strlen(MyOpt), strlen("chaineddigests="))) == 0)
      {
         CopyString(ChainedDigestTableFilename, MyArg, 0, 0, strlen(MyArg), MAX_STRING_SIZE, MAX_STRING_SIZE);
      }
      else if(strncmp(MyOpt, "setactivepartition=", MAX(strlen(MyOpt), strlen("setactivepartition="))) == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<setbootablestoragedrive ", FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "value=\"%i\" ", atoi(MyArg));
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(
            tx_buffer,
            "/>\n</data>\n",
            FIREHOSE_TX_BUFFER_SIZE
         ); // HACK for Zeno, remove the
            // \n

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      }
      else if(strncmp(MyOpt, "slot=", MAX(strlen(MyOpt), strlen("slot="))) == 0)
      {
         dbg(LOG_INFO, "User wants to change slot");
         dbg(
            LOG_INFO,
            "This will only affect the following TAGS : <program>, "
            "<read>, <getstorageinfo>\n"
         );
         fh.attrs.slot = atoi(MyArg);
         continue;
      }
      else if(strncmp(MyOpt, "getstorageinfo=", MAX(strlen(MyOpt), strlen("getstorageinfo="))) == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<getstorageinfo ", FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "physical_partition_number=\"%i\" ", atoi(MyArg));
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(
            tx_buffer,
            "/>\n</data>\n",
            FIREHOSE_TX_BUFFER_SIZE
         ); // HACK for Zeno, remove the
            // \n

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      }
      else if(strncmp(MyOpt, "dontsorttags", MAX(strlen(MyOpt), strlen("dontsorttags"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--dontsorttags parameter is deprecated and scheduled "
            "to be removed"
         );
         SortTags = 0; // will no longer ensure that <erase> is first and
                       // <patch> is last
         continue;
      }
      else if(strncmp(MyOpt, "showdigestperpacket", MAX(strlen(MyOpt), strlen("showdigestperpacket"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--showdigestperpacket parameter is deprecated and "
            "scheduled to be removed"
         );
         ShowDigestPerPacket = 1; // will show a SHA256 on all packets sent to target
         createcommandtrace = 1;
         continue;
      }
      else if(strncmp(MyOpt, "showdigestperfile", MAX(strlen(MyOpt), strlen("showdigestperfile"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--showdigestperfile parameter is deprecated and "
            "scheduled to be removed"
         );
         ShowDigestPerFile = 1; // will show a SHA256 per file
         fdp = ReturnFileHandle(DigestsPerFileName, MAX_PATH_SIZE,
                                "w"); // will exit if not successful

         continue;
      }
      else if(strncmp(MyOpt, "showpercentagecomplete", MAX(strlen(MyOpt), strlen("showpercentagecomplete"))) == 0)
      {
         showpercentagecomplete = 1; // does a simulation first to get build size
         continue;
      }
      else if(strncmp(MyOpt, "createcommandtrace", MAX(strlen(MyOpt), strlen("createcommandtrace"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--createcommandtrace parameter is deprecated and "
            "scheduled to be removed"
         );
         createcommandtrace = 1; // will show a SHA256 on all packets sent to target
         continue;
      }
      else if(strncmp(MyOpt, "sendxml=", MAX(strlen(MyOpt), strlen("sendxml="))) == 0)
      {
         // split on commas, store in XMLFileTable[MAX_XML_FILES][MAX_PATH_SIZE]
         num_xml_files_to_send = SplitStringOnCommas(
            MyArg,
            MAX_STRING_SIZE,
            XMLFileTable,
            static_cast<int32_t>(num_xml_files_to_send),
            MAX_XML_FILES,
            MAX_PATH_SIZE
         );

         XMLFileTable[num_xml_files_to_send][0] = '\0'; // j is how many strings were valid, make sure last one in
                                                        // table is null

         OpenAndStoreAllXMLFiles();

         num_xml_files_to_send = 0;

         continue;
      }
      else if(strncmp(MyOpt, "verbose", MAX(strlen(MyOpt), strlen("verbose"))) == 0)
      {
         dbg(
            LOG_DEBUG,
            "User wants FIREHOSE VERBOSE - Target will log much of "
            "what it is doing\n"
         );
         Verbose = 1;
         continue;
      }
      else if(strncmp(MyOpt, "zlpawarehost=", MAX(strlen(MyOpt), strlen("zlpawarehost="))) == 0)
      {
         fh.attrs.ZlpAwareHost = atoi(MyArg);

         if(fh.attrs.ZlpAwareHost > 1) fh.attrs.ZlpAwareHost = 1;

         dbg(LOG_DEBUG, "User set ZLPAWAREHOST to %" SIZE_T_FORMAT "\n", fh.attrs.ZlpAwareHost);
         continue;
      }
      else if(strncmp(MyOpt, "verify_programming", MAX(strlen(MyOpt), strlen("verify_programming"))) == 0)
      {
         dbg(LOG_DEBUG, "User wants verify_programming\n");
         verify_programming = 1;
         verify_programming_sha256 = 0;
         continue;
      }
      else if(strncmp(MyOpt, "verify_programming_getsha", MAX(strlen(MyOpt), strlen("verify_programming_getsha"))) == 0)
      {
         dbg(
            LOG_DEBUG,
            "User wants to Verify flashing/programming using the "
            "getsha256digest command\n"
         );
         verify_programming = 1;
         verify_programming_sha256 = 1;
         continue;
      }
      else if(strncmp(MyOpt, "generatesha256file", MAX(strlen(MyOpt), strlen("generatesha256file"))) == 0)
      {
         dbg(LOG_DEBUG, "User wants to generate the sha256 file of sendimage\n");
         GenerateSha256File = 1;
         verify_programming = 1;
         Simulate = 1;
         continue;
      }
      else if(strncmp(MyOpt, "verifysha256file", MAX(strlen(MyOpt), strlen("verifysha256file"))) == 0)
      {
         dbg(LOG_DEBUG, "User wants to generate the sha256 file of sendimage\n");
         VerifySha256File = 1;
         continue;
      }
      else if(strncmp(MyOpt, "testvipimpact", MAX(strlen(MyOpt), strlen("testvipimpact"))) == 0)
      {
         dbg(LOG_DEBUG, "User wants testvipimpact\n");
         testvipimpact = 1;
         continue;
      }
      else if(strncmp(MyOpt, "simulate", MAX(strlen(MyOpt), strlen("simulate"))) == 0)
      {
         dbg(LOG_DEBUG, "User wants SIMULATION - Not reading/writing from a port\n");
         Simulate = 1;
         continue;
      }
      else if(strncmp(MyOpt, "createdigests", MAX(strlen(MyOpt), strlen("createdigests"))) == 0)
      {
         dbg(
            LOG_DEBUG,
            "User wants SIMULATION (CreateDigests) - Not "
            "reading/writing from a port\n"
         );
         dbg(
            LOG_WARN,
            "--createdigests parameter is deprecated and scheduled "
            "to be removed, use --createvipdigests= instead."
         );
         CreateDigests = 1;
         Simulate = 1;
         ShowDigestPerPacket = 1;
         continue;
      }
      else if(strncmp(MyOpt, "createvipdigests=", MAX(strlen(MyOpt), strlen("createvipdigests"))) == 0)
      {
         struct header_types_cmd_line
         {
            const char* name;
            enum fh_digest_header_type type;
         };
         int32_t idx;
         int32_t num_types;
         struct header_types_cmd_line header_types[] = {
            {"none", FH_DIGEST_HEADER_NONE},
            {"elf", FH_DIGEST_HEADER_ELF},
            {"mbn", FH_DIGEST_HEADER_MBN},
         };

         CreateDigests = 1;
         Simulate = 1;
         ShowDigestPerPacket = 1;

         num_types = sizeof(header_types) / sizeof(struct header_types_cmd_line);
         for(idx = 0; idx < num_types; idx++)
         {
            const char* com_type = header_types[idx].name;
            int32_t res = strncasecmp(MyArg, com_type, MAX(strlen(MyArg), strlen(com_type)));
            if(res == 0)
            {
               digest_header_type = header_types[idx].type;
               break;
            }
         }

         if(idx == num_types)
         {
            dbg(
               LOG_INFO,
               "Invalid VIP digest specified in createvipdigests, "
               "possible values are:"
            );
            for(idx = 0; idx < num_types; idx++)
               dbg(LOG_INFO, "%s", header_types[idx].name);
            ExitAndShowLog(1);
         }
         dbg(LOG_INFO, "Selected header: %d", digest_header_type);
      }
      else if(strncmp(MyOpt, "skipwrite", MAX(strlen(MyOpt), strlen("skipwrite"))) == 0)
      {
         dbg(
            LOG_DEBUG,
            "User wants skipwrite - Will not commit to flash. Use "
            "this to benchmark USB speed\n"
         );
         fh.attrs.SkipWrite = 1;
         continue;
      }
      else if(strncmp(MyOpt, "skippatch", MAX(strlen(MyOpt), strlen("skippatch"))) == 0)
      {
         dbg(LOG_WARN, "--skippatch parameter is deprecated and scheduled to be removed");
         dbg(
            LOG_DEBUG,
            "User wants skippatch (LOAD_PATCH_PROGRAM_FILES=0) - "
            "Will issue any <patch commands\n"
         );
         LOAD_PATCH_PROGRAM_FILES = 0;
         continue;
      }
      else if(strncmp(MyOpt, "skipstorageinit", MAX(strlen(MyOpt), strlen("skipstorageinit"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--skipstorageinit parameter is no longer needed for "
            "targets after MSM8998"
         );
         dbg(
            LOG_DEBUG,
            "User wants skipstorageinit - Will not initialize "
            "storage with <configure> i.e. handleConfigure()\n"
         );
         fh.attrs.SkipStorageInit = 1;
         continue;
      }
      else if(strncmp(MyOpt, "readbogusdata", MAX(strlen(MyOpt), strlen("readbogusdata"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--readbogusdata parameter is deprecated and scheduled "
            "to be removed"
         );
         dbg(
            LOG_DEBUG,
            "User wants ReadBogusDataBeforeSendingConfigure - Will attempt to "
            "read 4KB of data before sending <configure> command\n"
         );
         ReadBogusDataBeforeSendingConfigure = 1;
         continue;
      }

      else if(strncmp(MyOpt, "skipharddriveread", MAX(strlen(MyOpt), strlen("skipharddriveread"))) == 0)
      {
         dbg(
            LOG_DEBUG,
            "User wants skipharddriveread - Will get filesizes "
            "from files, but data will be garbage\n"
         );
         skipharddriveread = 1;
         continue;
      }
      else if(strncmp(MyOpt, "maxpayloadsizeinbytes=", MAX(strlen(MyOpt), strlen("maxpayloadsizeinbytes="))) == 0)
      {
         fh.attrs.MaxPayloadSizeToTargetInBytes = atoi(MyArg);
         dbg(LOG_DEBUG, "User set MaxPayloadSizeToTargetInBytes to %ld\n", fh.attrs.MaxPayloadSizeToTargetInBytes);
         continue;
      }
      else if(strncmp(MyOpt, "maxreadpayloadsizeinbytes=", MAX(strlen(MyOpt), strlen("maxreadpayloadsizeinbytes="))) ==
              0)
      {
         fh.attrs.MaxPayloadSizeFromTargetInBytes = atoi(MyArg);
         dbg(LOG_INFO, "User set MaxPayloadSizeFromTargetInBytes to %ld\n", fh.attrs.MaxPayloadSizeFromTargetInBytes);
         continue;
      }
      else if(strncmp(MyOpt, "comportopentimeout=", MAX(strlen(MyOpt), strlen("comportopentimeout="))) == 0)
      {
         ComPortOpenTimeout = static_cast<float>(atof(MyArg));
         dbg(
            LOG_WARN,
            "--comportopentimeout= parameter is deprecated and "
            "scheduled to be removed"
         );
         dbg(LOG_INFO, "User set ComPortOpenTimeout to %f\n", ComPortOpenTimeout);
         continue;
      }
      else if(strncmp(MyOpt, "maxdigesttablesizeinbytes=", MAX(strlen(MyOpt), strlen("maxdigesttablesizeinbytes="))) ==
              0)
      {
         fh.attrs.MaxDigestTableSizeInBytes = atoi(MyArg);
         dbg(
            LOG_WARN,
            "--maxdigesttablesizeinbytes= parameter is deprecated "
            "and scheduled to be removed"
         );
         dbg(LOG_DEBUG, "User set MaxDigestTableSizeInBytes to %ld\n", fh.attrs.MaxDigestTableSizeInBytes);
         MaxNumDigestsPerTable = fh.attrs.MaxDigestTableSizeInBytes / DigestSizeInBytes;
         continue;
      }
      else if(strncmp(MyOpt, "nop", MAX(strlen(MyOpt), strlen("nop"))) == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(
            tx_buffer,
            "<NOP value=\"ping\" ",
            FIREHOSE_TX_BUFFER_SIZE
         ); // HACK for Zeno, Change NOP
            // back to nop
         AppendToBuffer(
            tx_buffer,
            "/>\n</data>\n",
            FIREHOSE_TX_BUFFER_SIZE
         ); // HACK for Zeno, remove the
            // \n

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      } // end of nop
      else if(strncmp(MyOpt, "forceoverwrite", MAX(strlen(MyOpt), strlen("forceoverwrite"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--forceoverwrite parameter is deprecated and scheduled "
            "to be removed"
         );
         ForceOverwrite = 1;
      }
      else if(strncmp(MyOpt, "validate_image_size", MAX(strlen(MyOpt), strlen("validate_image_size"))) == 0)
      {
         dbg(
            LOG_WARN,
            "User wants to validate partition image size against "
            "partition xml size from build"
         );
         ValidateImageSize = 1;
      }
   }


   OpenAndStoreAllXMLFiles();
   num_xml_files_to_send = 0;


   // PASS 4 OF COMMAND LINE OPTIONS
   //    PURPOSE: Options that need to be parsed last, such as getgptmainbackup
   //    because this issues a <read>
   //               and I need to know MemoryName, sectorsizeinbytes etc etc
   //               before creating this tag

   for(i = 1; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
   {
      getoptarg(static_cast<int32_t>(i), argv, MyOpt, MAX_STRING_SIZE, MyArg, MAX_STRING_SIZE);

      if(strncmp(MyOpt, "getgptmainbackup=", MAX(strlen(MyOpt), strlen("getgptmainbackup="))) == 0)
      {
         dbg(
            LOG_WARN,
            "--getgptmainbackup= parameter is deprecated and "
            "scheduled to be removed"
         );
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<read ", FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", fh.attrs.SECTOR_SIZE_IN_BYTES);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         if(fh.attrs.SECTOR_SIZE_IN_BYTES == 4096)
            sprintf(temp_buffer, "num_partition_sectors=\"6\" ");
         else
            sprintf(temp_buffer,
                    "num_partition_sectors=\"34\" "); // 512 byte sector size

         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "filename=\"gpt_main%d.bin\" ", atoi(MyArg));
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "physical_partition_number=\"%i\" ", atoi(MyArg));
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "start_sector=\"0\" ");
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         AppendToBuffer(
            tx_buffer,
            "/>\n</data>\n",
            FIREHOSE_TX_BUFFER_SIZE
         ); // HACK for Zeno, remove the
            // \n

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }


         // Now the backup
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<read ", FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", fh.attrs.SECTOR_SIZE_IN_BYTES);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         if(fh.attrs.SECTOR_SIZE_IN_BYTES == 4096)
            sprintf(temp_buffer, "num_partition_sectors=\"5\" ");
         else
            sprintf(temp_buffer,
                    "num_partition_sectors=\"33\" "); // 512 byte sector size

         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "filename=\"gpt_backup%d.bin\" ", atoi(MyArg));
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "physical_partition_number=\"%i\" ", atoi(MyArg));
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         if(fh.attrs.SECTOR_SIZE_IN_BYTES == 4096)
            snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "start_sector=\"NUM_DISK_SECTORS-5.\" ");
         else
            snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "start_sector=\"NUM_DISK_SECTORS-33.\" ");
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         AppendToBuffer(
            tx_buffer,
            "/>\n</data>\n",
            FIREHOSE_TX_BUFFER_SIZE
         ); // HACK for Zeno, remove the
            // \n

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }


         continue;
      }
      else if(strncmp(MyOpt, "forcecontentsxmlpaths", MAX(strlen(MyOpt), strlen("forcecontentsxmlpaths"))) == 0)
      {
         dbg(LOG_INFO, "User requested forcecontentsxmlpaths");
         forcecontentsxmlpaths = 1;
      }
      else if(strncmp(MyOpt, "verify_build", MAX(strlen(MyOpt), strlen("verify_build"))) == 0)
      {
         dbg(
            LOG_WARN,
            "--verify_build parameter is deprecated and scheduled "
            "to be removed"
         );
         dbg(
            LOG_INFO,
            "User requested verify_build, NOTE: This is to be used "
            "with --contentsxml="
         );
         verify_build = 1;
         Simulate = 1;
         AllowReset = 0; // won't send <power DelayInSeconds="2" value="reset"
                         // /> anymore VerboseLevel  = LOG_DEBUG;
      }
      else if(strncmp(MyOpt, "fixgpt=", MAX(strlen(MyOpt), strlen("fixgpt="))) == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<fixgpt ", FIREHOSE_TX_BUFFER_SIZE);

         if(atoi(MyArg) == -1 || strncmp(MyArg, "all", MAX(strlen(MyOpt), strlen("all"))) == 0)
         {
            snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "lun=\"all\" ");
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
         }
         else
         {
            snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "lun=\"%d\" ", atoi(MyArg));
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
         }

         snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "grow_last_partition=\"1\" ");
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      }
      else if(strncmp(MyOpt, "benchmarkwrites", MAX(strlen(MyOpt), strlen("benchmarkwrites"))) == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(
            tx_buffer,
            "<benchmark test=\"TestWritePerformance\" "
            "trials=\"500\" TestWritePerformance=\"1\" ",
            FIREHOSE_TX_BUFFER_SIZE
         );
         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      } // end of nop

      else if(strncmp(MyOpt, "benchmarkreads", MAX(strlen(MyOpt), strlen("benchmarkreads"))) == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(
            tx_buffer,
            "<benchmark test=\"TestReadPerformance\" "
            "trials=\"500\" TestReadPerformance=\"1\" ",
            FIREHOSE_TX_BUFFER_SIZE
         );
         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      } // end of nop
      else if(strncmp(MyOpt, "benchmarkdigestperformance", MAX(strlen(MyOpt), strlen("benchmarkdigestperformance"))) ==
              0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(
            tx_buffer,
            "<benchmark test=\"TestDigestPerformance\" "
            "trials=\"500\" TestDigestPerformance=\"1\" ",
            FIREHOSE_TX_BUFFER_SIZE
         );
         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

         // Save into XMLFileTable
         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }

         continue;
      } // end of nop

      else if(strncmp(MyOpt, "benchmarkddr", MAX(strlen(MyOpt), strlen("benchmarkddr"))) == 0)
      {
         dbg(LOG_ERROR, "benchmarkddr option is no longer supported.");
         continue;
      }


      else if(strncmp(MyOpt, "sendimage=", MAX(strlen(MyOpt), strlen("sendimage="))) == 0)
      {
         // Find image
         SIZE_T FileSizeNumSectors = 0;

         char* SendImageFile = find_file(MyArg, 1);

         if(SendImageFile == NULL)
         {
            // Couldn't find the file
            if(ConvertProgram2Read == 0)
            {
               dbg(
                  LOG_ERROR,
                  "The file you specified with '--sendimage=%s' could not be "
                  "found",
                  MyArg
               );
               ExitAndShowLog(1);
            }
            else
            {
               // to be here it's a <read> so filesize is based on --num_sectors
               // the user provided
               FileSizeNumSectors = MyNumSectors;
            }
         }
         else
         {
            FileSizeNumSectors = LastFindFileFileSize / SectorSizeInBytes;

            if(LastFindFileFileSize % SectorSizeInBytes)
            {
               FileSizeNumSectors++;
            }
         }


         dbg(LOG_DEBUG, "File size is %" SIZE_T_FORMAT " bytes", LastFindFileFileSize);
         dbg(LOG_DEBUG, "NumSectors needed %" SIZE_T_FORMAT, FileSizeNumSectors);

         if(MyNumSectors > 0)
         {
            // user specified some value
            if(MyNumSectors < FileSizeNumSectors)
            {
               FileSizeNumSectors = MyNumSectors;
               dbg(
                  LOG_DEBUG,
                  "NumSectors changed to %" SIZE_T_FORMAT " by --num_sectors=%" SIZE_T_FORMAT,
                  FileSizeNumSectors,
                  MyNumSectors
               );
            }
            else
               dbg(
                  LOG_WARN,
                  "User specified --num_sectors=%" SIZE_T_FORMAT " but file only has %" SIZE_T_FORMAT
                  " sectors. **Ignoring --num_sectors\n\n",
                  MyNumSectors,
                  FileSizeNumSectors
               );

         } // end if(MyNumSectors>0)

         if(FileSizeNumSectors == 0)
         {
            dbg(
               LOG_ERROR,
               "Nothing to send!! File is empty!! FileSizeNumSectors==0 for "
               "--sendimage=%s   USE --num_sectors=200 "
               "--sectorsizeinbytes=512",
               MyArg
            );
            ExitAndShowLog(1);
         }

         if(wipefirst)
         {
            InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<program ", FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SectorSizeInBytes);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ", ((16 * 1024) / SectorSizeInBytes));
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "filename=\"%s\" ", WipeFirstFileName);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(
               temp_buffer,
               "physical_partition_number=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.physical_partition_number
            );
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "start_sector=\"%s\" ", fh.attrs.start_sector);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);


            // Save into XMLFileTable
            CopyString(
               XMLStringTable[NumXMLFilesInTable],
               tx_buffer,
               0,
               0,
               strlen(tx_buffer),
               MAX_XML_SIZE,
               FIREHOSE_TX_BUFFER_SIZE
            );
            NumXMLFilesInTable++;

            if(NumXMLFilesInTable >= MAX_XML_FILES)
            {
               dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
               ExitAndShowLog(1);
            }

         } // end of if(wipefirst)

         if(erasefirst)
         {
            InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<erase ", FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SectorSizeInBytes);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ", FileSizeNumSectors);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(
               temp_buffer,
               "physical_partition_number=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.physical_partition_number
            );
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "start_sector=\"%s\" ", fh.attrs.start_sector);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);


            // Save into XMLFileTable
            CopyString(
               XMLStringTable[NumXMLFilesInTable],
               tx_buffer,
               0,
               0,
               strlen(tx_buffer),
               MAX_XML_SIZE,
               FIREHOSE_TX_BUFFER_SIZE
            );
            NumXMLFilesInTable++;

            if(NumXMLFilesInTable >= MAX_XML_FILES)
            {
               dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
               ExitAndShowLog(1);
            }

         } // end of if(erasefirst)

         if(SendImageFile != nullptr)
         {
            fd = ReturnFileHandle(SendImageFile, MAX_PATH_SIZE,
                                  "rb"); // will exit if not successful
         }

         if(SendImageFile != nullptr && TestIfSparse(fd))
         {
            dbg(LOG_INFO, "%s is being treated as a sparse image", MyArg);
            GenerateSparseXMLTags(fd, MyArg, MyStartSector, SectorSizeInBytes, MyLun);
         }
         else
         {
            InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);

            if(ConvertProgram2Read)
            {
               AppendToBuffer(tx_buffer, "<read ", FIREHOSE_TX_BUFFER_SIZE);
            }
            else
            {
               AppendToBuffer(tx_buffer, "<program ", FIREHOSE_TX_BUFFER_SIZE);

               if(ConvertProgram2Firmware) SectorSizeInBytes = 1;
            }

            sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SectorSizeInBytes);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ", FileSizeNumSectors);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "filename=\"%s\" ", MyArg);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            // sprintf(temp_buffer,"physical_partition_number=\"%" SIZE_T_FORMAT
            // "\" ",fh.attrs.physical_partition_number);
            sprintf(temp_buffer, "physical_partition_number=\"%" SIZE_T_FORMAT "\" ", MyLun);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            // sprintf(temp_buffer,"start_sector=\"%s\"
            // ",fh.attrs.start_sector);
            if(MyStartSector >= 0)
               sprintf(temp_buffer, "start_sector=\"%" SIZE_T_FORMAT "\" Version=\"1\" ", MyStartSector);
            else
               sprintf(temp_buffer, "start_sector=\"NUM_DISK_SECTORS%" SIZE_T_FORMAT "\" ", MyStartSector);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);


            // Save into XMLFileTable
            CopyString(
               XMLStringTable[NumXMLFilesInTable],
               tx_buffer,
               0,
               0,
               strlen(tx_buffer),
               MAX_XML_SIZE,
               FIREHOSE_TX_BUFFER_SIZE
            );
            NumXMLFilesInTable++;
         }

         // Close file handle
         if(fd != nullptr)
         {
            fh_fclose(fd);
            fd = nullptr;
         }

         if(NumXMLFilesInTable >= MAX_XML_FILES)
         {
            dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
            ExitAndShowLog(1);
         }


         continue;
      } // end of --sendimage=
      else if(strncmp(MyOpt, "stresstest", MAX(strlen(MyOpt), strlen("stresstest"))) == 0)
      {
         std::shared_ptr<std::fstream> fTemp;
         char MyBufferOfZeros[16 * 1024] = {0};
         int64_t idx;
         SIZE_T FileSizeNumSectors;

         dbg(
            LOG_WARN,
            "--stresstest= parameter is deprecated and scheduled to "
            "be removed"
         );
         dbg(
            LOG_DEBUG,
            "User wants stresstest - Also enabling verify_programming. Ensure "
            "this passes before leaving bring up lab\n"
         );
         stresstest = 1;
         verify_programming = 1;

         dbg(LOG_INFO, "Creating 1GB all zero file '%s'\n", StressTestFileName);
         // Need to create the 16KB file of zeros
         fTemp = ReturnFileHandle(StressTestFileName, strlen(StressTestFileName),
                                  "wb"); // will exit if not successful

         for(idx = 0; idx < 65536; idx++)
         {
            fTemp->write(MyBufferOfZeros, sizeof(MyBufferOfZeros));
            // dbg (LOG_INFO, ".");
         }

         fh_fclose(fTemp);
         fTemp = nullptr;

         LastFindFileFileSize = 16 * 1024 * 65536;

         FileSizeNumSectors = LastFindFileFileSize / SectorSizeInBytes;

         if(LastFindFileFileSize % SectorSizeInBytes)
         {
            FileSizeNumSectors++;
         }


         dbg(LOG_DEBUG, "File size is %" SIZE_T_FORMAT " bytes", LastFindFileFileSize);
         dbg(LOG_DEBUG, "NumSectors needed %" SIZE_T_FORMAT, FileSizeNumSectors);


         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);

         AppendToBuffer(tx_buffer, "<program ", FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SectorSizeInBytes);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ", FileSizeNumSectors);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "filename=\"%s\" ", StressTestFileName);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         // sprintf(temp_buffer,"physical_partition_number=\"%" SIZE_T_FORMAT
         // "\" ",fh.attrs.physical_partition_number);
         sprintf(temp_buffer, "physical_partition_number=\"%" SIZE_T_FORMAT "\" ", MyLun);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         if(MyStartSector >= 0)
            sprintf(temp_buffer, "start_sector=\"%" SIZE_T_FORMAT "\" ", MyStartSector);
         else
            sprintf(temp_buffer, "start_sector=\"NUM_DISK_SECTORS%" SIZE_T_FORMAT "\" ", MyStartSector);

         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);


         // Save into XMLFileTable
         for(idx = 0; idx < MyTrials; idx++)
         {
            CopyString(
               XMLStringTable[NumXMLFilesInTable],
               tx_buffer,
               0,
               0,
               strlen(tx_buffer),
               MAX_XML_SIZE,
               FIREHOSE_TX_BUFFER_SIZE
            );
            NumXMLFilesInTable++;

            if(NumXMLFilesInTable >= MAX_XML_FILES)
            {
               dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
               ExitAndShowLog(1);
            }
         }

         continue;
      }
   }


   if(Simulate)
   {
      dbg(
         LOG_ALWAYS,
         "Remember: This *is* a simulation. Nothing will be sent "
         "to target, COM PORT NOT OPENED"
      );
      dbg(LOG_ALWAYS, "      _                 _       _       ");
      dbg(LOG_ALWAYS, "     (_)               | |     | |      ");
      dbg(LOG_ALWAYS, "  ___ _ _ __ ___  _   _| | __ _| |_ ___ ");
      dbg(LOG_ALWAYS, " / __| | '_ ` _ \\| | | | |/ _` | __/ _ \\");
      dbg(LOG_ALWAYS, " \\__ \\ | | | | | | |_| | | (_| | ||  __/");
      dbg(LOG_ALWAYS, " |___/_|_| |_| |_|\\__,_|_|\\__,_|\\__\\___|\n");
   }
   else
   {
      if(NULL == port_name || strlen(port_name) == 0)
      {
         dbg(LOG_ERROR, "Port device name not specified; Example: --port=\\\\.\\COM19 ");
         ExitAndShowLog(1);
      }

      dbg(LOG_INFO, "User wants to talk to port '%s'", port_name);

      if(OpenPort(port_name) != 0)
      {
         ExitAndShowLog(1);
      }

      if(ReadBogusDataBeforeSendingConfigure)
      {
         // hack to get past weird sahara message in buffer
         dbg(
            LOG_ALWAYS,
            "**Attempting to read 4KB to flush out any possible strange first "
            "log packet since user chose ReadBogusDataBeforeSendingConfigure"
         );
         BytesRead = ReadPort((uint8_t*)ReadBuffer, 4096,
                              MAX_READ_BUFFER_SIZE); // null doesn't matter in RAW mode

         if(BytesRead > 0)
         {
            PRETTYPRINT((uint8_t*)ReadBuffer, static_cast<uint32_t>(BytesRead), MAX_READ_BUFFER_SIZE);
         }
      }
   }

   // GetNextPacket();  // this will set all variables, including GotACK

   if(createconfigxml)
   {
      if(CopyString(
            ConfigXMLName,
            fh.attrs.MemoryName,
            strlen(ConfigXMLName),
            0,
            strlen(fh.attrs.MemoryName),
            sizeof(fh.attrs.MemoryName),
            MAX_PATH_SIZE
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy string '%s' of length %i into ConfigXMLName",
            fh.attrs.MemoryName,
            strlen(fh.attrs.MemoryName)
         );
         return 1;
      }

      if(CopyString(ConfigXMLName, ".xml", strlen(ConfigXMLName), 0, strlen(".xml"), MAX_PATH_SIZE, 4) == 0)
      {
         dbg(LOG_ERROR, "Failed to copy string '%s' of length %i into ConfigXMLName", ".xml", strlen(".xml"));
         return 1;
      }

      fg = ReturnFileHandle(ConfigXMLName, MAX_PATH_SIZE,
                            "w"); // will exit if not successful

      *fg << "<?xml version=\"1.0\" ?>\n";
      *fg << "<configuration>\n";
      *fg << "  <!--NOTE: This is an ** Autogenerated file **-->\n";

      szIndex = 0;

      for(i = 0; i < (SIZE_T)argc; i++) // skip 0 since argv[0] is fh_loader.exe
         szIndex += sprintf(&szTemp[szIndex], "%s ", argv[i]);

      szIndex = 0;

      *fg << "  <!--NOTE: " << szTemp << " -->\n";

      *fg << "  <options>\n";
      *fg << "    DOWNLOAD_PROTOCOL       = FIREHOSE\n";

      if(LOAD_RAW_PROGRAM_FILES)
      {
         *fg << "    LOAD_RAW_PROGRAM_FILES  = true\n";
      }
      else
      {
         *fg << "    LOAD_RAW_PROGRAM_FILES  = false\n";
      }

      if(LOAD_PATCH_PROGRAM_FILES)
      {
         *fg << "    LOAD_PATCH_PROGRAM_FILES= true\n";
      }
      else
      {
         *fg << "    LOAD_PATCH_PROGRAM_FILES= false\n";
      }

      *fg << "  </options>\n";
      *fg << "  <search_paths>\n";

      *fg << "    " << flattenbuildvariant << "\n";

      for(j = 0; j < num_search_paths; j++)
         *fg << "    " << search_path[j] << "\n";

      *fg << "  </search_paths>\n";
      *fg << "  <rawprogram>\n";

      for(i = 0; i < NumContensXML; i++)
      {
         if(ContensXML[i].FileType == 'r')
         {
            // if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType) // ||
            // ContensXML[i].StorageType==0 )
            if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
               *fg << "    " << ContensXML[i].Filename << "\n";
         }
      }

      *fg << "  </rawprogram>\n";
      *fg << "  <patch>\n";

      for(i = 0; i < NumContensXML; i++)
      {
         if(ContensXML[i].FileType == 'p')
         {
            // if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType) // ||
            // ContensXML[i].StorageType==0 )
            if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
               *fg << "    " << ContensXML[i].Filename << "\n";
         }
      }

      *fg << "  </patch>\n";

      *fg << "  <file_mappings>\n";
      *fg << "  NOTE: Do not trust these mappings. These are only POSSIBLE "
             "file/path mappings\n";
      *fg << "  NOTE: If any of the files are *also* found in the search paths "
             "listed above, that file will be used\n\n";

      for(i = 0; i < NumContensXML; i++)
      {
         if(ContensXML[i].FileType != 'r' && ContensXML[i].FileType != 'p')
         {
            if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
            {
               *fg << "    " << ContensXML[i].Path;
               *fg << ContensXML[i].Filename << "\n";
            }
         }
      }

      *fg << "  </file_mappings>\n";
      *fg << "  </configuration>\n";

      fh_fclose(fg);
      fg = nullptr;
   }

   if(createcommandtrace)
   {
      fc = ReturnFileHandle(CommandTraceName, MAX_PATH_SIZE,
                            "w"); // will exit if not successful
      fh_fclose(fc);
      fc = nullptr;
   }


   MaxNumDigestsPerTable = fh.attrs.MaxDigestTableSizeInBytes / DigestSizeInBytes;
   CurrentDigestLocation =
      MaxNumDigestsPerTable - 54; // default to this. SendSignedDigestTable() below will correct this

   if(CreateDigests)
   {
      ft = ReturnFileHandle("DIGEST_TABLE.bin", strlen("DIGEST_TABLE.bin"),
                            "wb"); // will exit if not successful
      fh_fclose(ft);
      ft = nullptr;
   }


   //  BytesWritten = WritePort(buffer, strlen((const char *)buffer) );
   //  PRETTYPRINT(buffer, BytesWritten);
   // handleNop();  // turns on VERBOSE, if you don't want that, send 2

   LoadConfigureIntoStringTable(); // always loads at location 0 (i.e.
                                   // NumXMLFilesInTable=0)


   if(reboot_option > FH_REBOOT_NONE && reboot_option < FH_REBOOT_MAX)
      LoadPowerOpIntoStringTable(reboot_option, reboot_seconds);

   if(AllowReset)
      LoadResetIntoStringTable(); // always loads at NumXMLFilesInTable (i.e.
                                  // the last location)

   // SendConfigure();
   // handleNop();  // turns on VERBOSE, if you don't want that, send 2

   ModifyTags(); // this optionally converts <program to <read if user
                 // specified --convertprogram2read

   addSlotAttribute();

   // SNAPSHOT TIME HERE
   gettimeofday(&fh_loader_start, NULL);

   if(SortTags)
   {
      // ensure that <erase> is first and <patch> tags are last
      dbg(
         LOG_INFO,
         "Sorting TAGS to ensure order is <configure>,<erase>, "
         "others, <patch>,<power>"
      );
      SortMyXmlFiles();
   }
   else
      dbg(LOG_INFO, "*Not* sorting tags since --dontsorttags was provided\n");

   if(showpercentagecomplete)
   {
      // For Corporate Tools they want to show "52% loaded" type of message. I
      // don't know ahead of time how big the build is This is made especially
      // tricky since --files or --notfiles, therefore the best way is to do a
      // simulation
      SimulateBack = Simulate; // backup
      Simulate = 1;
      SimulateForFileSize = 1; // this makes simulation quit right after getting file size
      BuildSizeTransferSoFar = 0;
      SendSignedDigestTable(SignedDigestTableFilename); // just returns if
                                                        // SignedDigestTableFilename
                                                        // is empty
      SendXmlFiles();                                   // this will modify BuildSizeTransferSoFar

      TotalTransferSize = BuildSizeTransferSoFar;

      // If we are verifying the build it will the same ammount of data again.
      // It isn't perfect (because of sector rounding)
      //  more accurate than not doing it.
      if(verify_programming && (verify_programming_sha256 == 0))
      {
         TotalTransferSize = static_cast<SIZE_T>(TotalTransferSize * 2.00);
      }

      if(TotalTransferSize > 0)
      {
         ReturnSizeString(TotalTransferSize, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
         dbg(LOG_INFO, "\n\nTotal to be tansferd with <program> or <read> is %s\n\n", temp_buffer);
      }

      Simulate = SimulateBack; // restore
      SimulateForFileSize = 0; // no longer just for file size
   }

   if(GenerateSha256File)
   {
      fd = ReturnFileHandle(DigestsPerFileName, strlen(DigestsPerFileName),
                            "wb"); // will exit if not successful
      fh_fclose(fd);
      fd = nullptr;
   }

   BuildSizeTransferSoFar = 0;
   SendSignedDigestTable(SignedDigestTableFilename); // just returns if SignedDigestTableFilename
                                                     // is empty
   SendXmlFiles();

   // SNAPSHOT TIME HERE
   gettimeofday(&fh_loader_end, NULL);


   if(!Simulate) ClosePort();


   if(CreateDigests)
   {
      try
      {
         dbg(LOG_INFO, "Calling PrepareDigestTable()");
         PrepareDigestTable();
         dbg(LOG_INFO, "PrepareDigestTable() completed successfully");
      }
      catch(const std::ios::failure& e)
      {
         dbg(LOG_ERROR, "std::ios::failure caught - Failed to create digest files: %s", e.what());
         dbg(LOG_ERROR, "This may be due to insufficient disk space or file access permissions");
         ExitAndShowLog(1);
      }
      catch(const std::exception& e)
      {
         dbg(LOG_ERROR, "std::exception caught - Failed to create digest files: %s", e.what());
         ExitAndShowLog(1);
      }
      catch(...)
      {
         dbg(LOG_ERROR, "Unknown exception caught while creating digest files");
         ExitAndShowLog(1);
      }
   }


   dbg(LOG_ALWAYS, "==============================================================");
   dbg(LOG_ALWAYS, "Files used and their paths");

   for(i = 0; i < FileToTrackCount; i++)
   {
      dbg(LOG_ALWAYS, "%3" SIZE_T_FORMAT " '%s'", i + 1, MaxFilesToTrack[i]);
   }

   dbg(LOG_INFO, "\n");

   if(NumWarnings > 0)
   {
      display_warning();
      dbg(LOG_ALWAYS, "==============================================================");
      dbg(
         LOG_ALWAYS,
         "NOTE: There were WARNINGS!! Repeated here, but please "
         "see log for more detail"
      );
      // dbg (LOG_ALWAYS,"%s",WarningsBuffer);  <-- this wants \\snowcone\blah
      // submitted as \\\\snowcone\\blah, debug this later
      dbg(
         LOG_ALWAYS,
         "%s\nNOTE: There were WARNINGS!! Repeated above, but please see log "
         "for more detail\n\n\n",
         WarningsBuffer.c_str()
      );
      dbg(LOG_ALWAYS, "==============================================================");
   }

   if(Simulate)
   {
      dbg(LOG_ALWAYS, "Remember: This was a simulation. Nothing was sent to a target");
      dbg(LOG_ALWAYS, "      _                 _       _       ");
      dbg(LOG_ALWAYS, "     (_)               | |     | |      ");
      dbg(LOG_ALWAYS, "  ___ _ _ __ ___  _   _| | __ _| |_ ___ ");
      dbg(LOG_ALWAYS, " / __| | '_ ` _ \\| | | | |/ _` | __/ _ \\");
      dbg(LOG_ALWAYS, " \\__ \\ | | | | | | |_| | | (_| | ||  __/");
      dbg(LOG_ALWAYS, " |___/_|_| |_| |_|\\__,_|_|\\__,_|\\__\\___|\n");
   }


   dbg(LOG_ALWAYS, "     _             (done)");
   dbg(LOG_ALWAYS, "    | |                 ");
   dbg(LOG_ALWAYS, "  __| | ___  _ __   ___ ");
   dbg(LOG_ALWAYS, " / _` |/ _ \\| '_ \\ / _ \\");
   dbg(LOG_ALWAYS, "| (_| | (_) | | | |  __/");
   dbg(LOG_ALWAYS, " \\__,_|\\___/|_| |_|\\___|");

   if(fh.attrs.slot != 0)
   {
      dbg(LOG_ALWAYS, "Slot changed to %d)   ", fh.attrs.slot);
   }

   if(ThereWereErrors)
      dbg(LOG_ALWAYS, "{Not Finished Successfully - There were ERRORS}\n");
   else
      dbg(LOG_ALWAYS, "{All Finished Successfully}\n");


   // dbg (LOG_ALWAYS,"Total time was %6.2f
   // seconds\n",ReturnTimeInSeconds(&fh_loader_start,&fh_loader_end));

   time_throughput_calculate(&fh_loader_start, &fh_loader_end, BuildSizeTransferSoFar, -1.0, LOG_INFO);


   if(createconfigxml)
      dbg(
         LOG_ALWAYS,
         "\n\n\nCreated '%s'.\n\n"
         "NOTE: You don't need to use --contentsxml= anymore. YOU CAN NOW USE "
         "--sendxml=%s\n\n",
         ConfigXMLName,
         ConfigXMLName
      );

   if(verify_build)
   {
      if(DeviceProgrammerIndex >= 0)
      {
         dbg(LOG_INFO, "\n\n");
         dbg(
            LOG_INFO,
            "QSaharaServer.exe -p \\\\.\\COM27 -s 13:%s -b %s\n",
            ContensXML[DeviceProgrammerIndex].Filename,
            ContensXML[DeviceProgrammerIndex].Path
         );
      }
   }

   PromptUser = 0; // turn this off here
   if(ThereWereErrors)
   {
      ExitAndShowLog(1); // this will prompt user to press any key if needed
   }
   else
   {
      ExitAndShowLog(0); // this will prompt user to press any key if needed
   }

   return m_errorCode;
} // end main()

void FirehoseLoader::PrepareDigestTable(void)
{
   SIZE_T NumDigestsInFile, Remainder, NumTablesAfter1st, DigestsPerTable;
   SIZE_T i, SeekLoc, HashOfNextTableLoc, TempValue;
   std::shared_ptr<std::fstream> FTemp;
   uint64_t DigestFileSize;
   uint32_t first_table_size;

   char* DigestFile = nullptr;

   std::string outDir(MainOutputDir);
   char digestTable[MAX_PATH_SIZE];
   snprintf_x(digestTable, sizeof(digestTable), "%s", (outDir + SLASH + "DIGEST_TABLE.bin").c_str());

   DigestFile = find_file(digestTable, 1);

   if(DigestFile == NULL)
   {
      dbg(LOG_ERROR, "'%s' not found", digestTable);
      ExitAndShowLog(1);
   }

   BytesRead = 0;

   NumDigestsInFile = LastFindFileFileSize / 32;
   Remainder = LastFindFileFileSize % 32;

   if(Remainder > 0)
   {
      dbg(LOG_ERROR, "Your DIGEST TABLE '%s' file is not a multiple of 32 bytes", "DIGEST_TABLE.bin");
      ExitAndShowLog(1);
   }

   dbg(LOG_ALWAYS, "NumDigestsInFile = %" SIZE_T_FORMAT, NumDigestsInFile);


   if(MaxNumDigestsPerTable == 0)
   {
      dbg(LOG_INFO, "3. In PrepareDigestTable AND MaxNumDigestsPerTable WAS ZERO");
      MaxNumDigestsPerTable = fh.attrs.MaxDigestTableSizeInBytes / DigestSizeInBytes;
   }

   DigestsPerTable = MaxNumDigestsPerTable - 1; // -1 to make room for hash of next table

   if(NumDigestsInFile >= 54)
   {
      uint64_t DigestsInLastTable;
      NumTablesAfter1st = ((NumDigestsInFile - 53) / DigestsPerTable) + 1;
      DigestsInLastTable = (NumDigestsInFile - 53) % DigestsPerTable;
      if(DigestsInLastTable == 0)
      {
         NumTablesAfter1st--;
      }
   }
   else
   {
      NumTablesAfter1st = 0;
   }

   HashOfNextTableLoc = NumTablesAfter1st - 1;

   ft = ReturnFileHandle(digestTable, strlen(digestTable),
                         "rb"); // will exit if not successful


   memset(tx_buffer, 0x0,
          FIREHOSE_TX_BUFFER_SIZE); // this is strictly not needed
   memset(tx_buffer_backup, 0x0, FIREHOSE_TX_BUFFER_SIZE);
   memset(temp_hash_value, 0x0, 32);

   // Pass 1: Create HASHES of the next tables and store them in
   // tx_buffer_backup
   for(i = 0; i < NumTablesAfter1st; i++)
   {
      // seek to location

      SeekLoc = (53 + HashOfNextTableLoc * DigestsPerTable) * DigestSizeInBytes;

      // Clear any error flags before seeking to ensure seek works correctly
      ft->clear();
      ft->seekg(SeekLoc, std::ios::beg);

      ft->read(tx_buffer, fh.attrs.MaxDigestTableSizeInBytes - 32);
      BytesRead = ft->gcount();

      // this copies last hash into current table in order to calculate the hash
      memscpy(
         &tx_buffer[fh.attrs.MaxDigestTableSizeInBytes - 32],
         FIREHOSE_TX_BUFFER_SIZE,
         temp_hash_value,
         sizeof(temp_hash_value)
      ); //

      if(i > 0) // On the very last table, if there is only 1 hash left, then above
                // copied an all zero hash. Don't consider this as additional data
         BytesRead += 32;

      DigestFileSize = ft->tellg();
      ft->seekg(0, std::ios::end);

      // USB will hang if we send any multiple of 512. Add 1 byte to the last
      // table to get around it.
      if(DigestFileSize == static_cast<uint64_t>(ft->tellg()))
      {
         tx_buffer[BytesRead] = 0;
         BytesRead += 1;
      }

      sechsharm_sha256_init(&context_per_packet);
      sechsharm_sha256_update(
         &context_per_packet,
         context_per_packet.leftover,
         &(context_per_packet.leftover_size),
         (uint8_t*)tx_buffer,
         static_cast<uint32_t>(BytesRead)
      );
      sechsharm_sha256_final(
         &context_per_packet,
         context_per_packet.leftover,
         &(context_per_packet.leftover_size),
         temp_hash_value
      );

      memscpy(
         &tx_buffer_backup[HashOfNextTableLoc * DigestSizeInBytes],
         FIREHOSE_TX_BUFFER_SIZE,
         temp_hash_value,
         sizeof(temp_hash_value)
      ); // memcpy
      HashOfNextTableLoc--;
   }

   if(NumDigestsInFile >= 54)
      first_table_size = 54 * 32;
   else
      first_table_size = static_cast<uint32_t>((NumDigestsInFile + 1) * 32); /* add 1 hash for
                                                                                all zero hash at
                                                                                the end */

   if(digest_header_type == FH_DIGEST_HEADER_MBN)
   {
      // Now create DigestsToSign.bin
      FTemp = ReturnFileHandle(
         "DigestsToSign.bin.mbn",
         strlen("DigestsToSign.bin.mbn"),
         "wb"
      ); // will exit if not successful

      TempValue = 0x1A;
      FTemp->write(reinterpret_cast<const char*>(&TempValue), 4); // SW_ID
      TempValue = 0x03;
      FTemp->write(reinterpret_cast<const char*>(&TempValue),
                   4); // header_vsn_num Header version number
      TempValue = 40;
      FTemp->write(reinterpret_cast<const char*>(&TempValue),
                   4); // image_src Location of image in flash
      TempValue = 0x00;
      FTemp->write(reinterpret_cast<const char*>(&TempValue),
                   4); // image_dest_ptr
      FTemp->write(reinterpret_cast<const char*>(&first_table_size),
                   4); // Size of complete image in bytes
      FTemp->write(reinterpret_cast<const char*>(&first_table_size),
                   4); // Size of code region of image in bytes

      TempValue = 0x00;
      FTemp->write(reinterpret_cast<const char*>(&TempValue),
                   4); // Pointer to images attestation signature
      TempValue = 0x00;
      FTemp->write(reinterpret_cast<const char*>(&TempValue),
                   4); // Size of the attestation signature in bytes
      TempValue = 0x00;
      FTemp->write(
         reinterpret_cast<const char*>(&TempValue),
         4
      ); // Pointer to the chain of attestation certificates
         // associated with the image
      TempValue = 0x00;
      FTemp->write(reinterpret_cast<const char*>(&TempValue),
                   4); // Size of the attestation chain in bytes
   }
   else if(digest_header_type == FH_DIGEST_HEADER_ELF)
   {
      uint8_t region_size[8];
      uint8_t elf_header_last_bytes[ELF_HEADER_SIZE - sizeof(elf_header_first_96bytes) - 2 * 8];
      dbg(LOG_INFO, "HEADER ELF %d", 0);
      /* Newer targets require an ELF header. Use the same inital table, add an
       * ELF header instead. */
      FTemp = ReturnFileHandle(
         "DigestsToSign.bin.elf",
         strlen("DigestsToSign.bin.elf"),
         "wb"
      ); // will exit if not successful
      FTemp
         ->write(reinterpret_cast<const char*>(elf_header_first_96bytes), sizeof(elf_header_first_96bytes)); /* Initial
                                                                                                                ELF
                                                                                                                header
                                                                                                              */
      /* Section to write the FileSz and MemSz of ELF header*/

      memset(region_size, 0, sizeof(region_size));

      region_size[0] = first_table_size & 0xFF;
      region_size[1] = first_table_size >> 8 & 0xFF;
      /* The maximum size of the first table is 1728 or 0x6C0. That can be
      represented by 2 bytes, no need to keep converting to little endian. */
      /* Write the FileSize and MemSize in ELF header. */
      FTemp->write(reinterpret_cast<const char*>(region_size), sizeof(region_size));
      FTemp->write(reinterpret_cast<const char*>(region_size), sizeof(region_size));

      /* ELF Header is 232 bytes in size, write the end which is all zeros for
       * this blob. */
      memset(elf_header_last_bytes, 0, sizeof(elf_header_last_bytes));
      FTemp->write(reinterpret_cast<const char*>(elf_header_last_bytes), sizeof(elf_header_last_bytes));
   }
   else if(digest_header_type == FH_DIGEST_HEADER_NONE)
   {
      FTemp = ReturnFileHandle("DigestsToSign.bin", strlen("DigestsToSign.bin"), "wb");
   }
   else
   {
      dbg(LOG_ERROR, "Invalid selection of VIP digest header: %d", (int32_t)digest_header_type);
      ExitAndShowLog(1);
   }

   /* Read the first hashes from the file and place it in digest to sign file.
    */
   dbg(LOG_DEBUG, "About to write hash table to DigestsToSign file, BytesRead will be %" SIZE_T_FORMAT, 53 * DigestSizeInBytes);
   dbg(LOG_DEBUG, "Stream state before seek - good: %d, eof: %d, fail: %d, bad: %d", ft->good(), ft->eof(), ft->fail(), ft->bad());
   ft->clear(); // Clear any error flags
   ft->seekg(0, std::ios::beg);
   dbg(LOG_DEBUG, "Stream state after seek - good: %d, eof: %d, fail: %d, bad: %d", ft->good(), ft->eof(), ft->fail(), ft->bad());
   ft->read(tx_buffer, 53 * DigestSizeInBytes);
   BytesRead = ft->gcount();
   dbg(LOG_DEBUG, "Stream state after read - good: %d, eof: %d, fail: %d, bad: %d", ft->good(), ft->eof(), ft->fail(), ft->bad());
   dbg(LOG_DEBUG, "Read %" SIZE_T_FORMAT " bytes from DIGEST_TABLE.bin, about to write to DigestsToSign", BytesRead);
   /* Write the hash table. */
   FTemp->write(tx_buffer, BytesRead);
   dbg(LOG_DEBUG, "Wrote %" SIZE_T_FORMAT " bytes to DigestsToSign file", BytesRead);

   /* Write the next hash or zero depending on the number of hashes.  */
   if(NumDigestsInFile < 54) memset(tx_buffer_backup, 0, sizeof(tx_buffer_backup));
   FTemp->write(reinterpret_cast<const char*>(&tx_buffer_backup[0]), 32);
   dbg(LOG_DEBUG, "Wrote 32 bytes (next hash) to DigestsToSign file, about to close");
   fh_fclose(FTemp);
   FTemp = nullptr;

   // Pass 2: Create ChainedTableOfDigests.bin
   FTemp = ReturnFileHandle(
      "ChainedTableOfDigests.bin",
      strlen("ChainedTableOfDigests.bin"),
      "wb"
   ); // will exit if not successful

   for(i = 0; i < NumTablesAfter1st; i++)
   {
      // seek to location
      SeekLoc = (53 + i * DigestsPerTable) * DigestSizeInBytes;

      // Clear any error flags before seeking to ensure seek works correctly
      ft->clear();
      ft->seekg(SeekLoc, std::ios::beg);

      memset(tx_buffer, 0x0,
             FIREHOSE_TX_BUFFER_SIZE); // this is strictly not needed

      ft->read(tx_buffer, fh.attrs.MaxDigestTableSizeInBytes - 32);
      BytesRead = ft->gcount();

      FTemp->write(tx_buffer, BytesRead);

      if(i < NumTablesAfter1st - 1)
      {
         // Now write hash of the next table
         FTemp->write(reinterpret_cast<const char*>(&tx_buffer_backup[(i + 1) * 32]), 32);
      }
      else
      {
         // On the last table add one byte to make sure it is a short packet
         //  this gets around the requirement that the last packet can't a
         //  multiple of 512.
         tx_buffer[0] = 0;
         FTemp->write(tx_buffer, 1);
      }
   }

   DigestFileSize = FTemp->tellp();

   if(DigestFileSize % 512 == 0 && DigestFileSize % fh.attrs.MaxDigestTableSizeInBytes != 0)
   {
      dbg(
         LOG_ERROR,
         "This should not be hit. The Digest output file is a "
         "multiple of 512. This will cause the target to hang. "
         "Please re-run and add --nop to fix this issue"
      );
      ExitAndShowLog(1);
   }

   fh_fclose(FTemp);
   FTemp = nullptr;

   fh_fclose(ft);
   ft = nullptr;
}

void FirehoseLoader::TestIfWeNeedToSendDigestTable(void)
{
   char *TempFile = NULL, BackRawMode;
   std::shared_ptr<std::fstream> fd;
   SIZE_T LocalBytesRead, FileSize;

   if(!UsingValidation)
   {
      return; // not even using VIP, so exit
   }

   // if(MaxNumDigestsPerTable>NumDigestsFound)
   if(NumDigestsFound <= 53)
   {
      // dbg (LOG_INFO,"First table only had %" SIZE_T_FORMAT " digests, so no
      // need to send chained digests",NumDigestsFound);
      return;
   }

   if((CurrentDigestLocation + 1) % MaxNumDigestsPerTable == 0)
   {
      // Ex. every 224 packets we send the digest table, as in 0, 223, 447, etc
   }
   else
   {
      // don't need to send a Digest Table yet
      return;
   }

   // Backup tx_buffer into tx_buffer_backup first
   memscpy(tx_buffer_backup, FIREHOSE_TX_BUFFER_SIZE, tx_buffer,
           FIREHOSE_TX_BUFFER_SIZE); // memcpy

   if(strlen(ChainedDigestTableFilename) == 0)
   {
      // return;
      dbg(
         LOG_ERROR,
         "ChainedDigestTableFilename was not set with "
         "--chaineddigests=SomeFilename.bin and you have at least "
         "%" SIZE_T_FORMAT " digests",
         NumDigestsFound
      );
      ExitAndShowLog(1);
   }


   dbg(LOG_DEBUG, "\t _               _       _        _     _      ");
   dbg(LOG_DEBUG, "\t| |             | |     | |      | |   | |     ");
   dbg(LOG_DEBUG, "\t| |__   __ _ ___| |__   | |_ __ _| |__ | | ___ ");
   dbg(LOG_DEBUG, "\t| '_ \\ / _` / __| '_ \\  | __/ _` | '_ \\| |/ _ \\");
   dbg(LOG_DEBUG, "\t| | | | (_| \\__ \\ | | | | || (_| | |_) | |  __/");
   dbg(LOG_DEBUG, "\t|_| |_|\\__,_|___/_| |_|  \\__\\__,_|_.__/|_|\\___|");

   dbg(
      LOG_DEBUG,
      "Since CurrentDigestLocation=%" SIZE_T_FORMAT
      ", Need to send digest table here digest_file_offset=%" SIZE_T_FORMAT,
      CurrentDigestLocation,
      digest_file_offset
   );

   // if digestTableSize is 992 bytes, this is 31 digests per table. Meaning we
   // send Singed DigestTable (1st command) then next 30 commands for a total of
   // 31 Ex   Command -1 is Digest Table, commands 0  to 29  <-- i.e. 30
   // commands + Digest Table is 31 commands then Command 30 is Digest Table,
   // commands 31 to 60  <-- i.e. 30 commands + Digest Table is 31 commands then
   // Command 61 is Digest Table, commands 62 to 91  <-- i.e. 30 commands +
   // Digest Table is 31 commands then Command 92 is Digest Table, commands 93
   // to 122 <-- i.e. 30 commands + Digest Table is 31 commands

   // find this file ChainedDigestTableFilename
   TempFile = find_file(ChainedDigestTableFilename, 0);

   if(TempFile == NULL)
   {
      dbg(LOG_ERROR, "'%s' not found", ChainedDigestTableFilename);
      ExitAndShowLog(1);
   }

   fd = ReturnFileHandle(TempFile, MAX_PATH_SIZE,
                         "rb"); // will exit if not successful

   FileSize = ReturnFileSize(fd);
   dbg(LOG_DEBUG, "File size is %" SIZE_T_FORMAT " bytes", FileSize);

   if(FileSize == 0)
   {
      dbg(
         LOG_INFO,
         "--chaineddigests file was 0 bytes, not sending any "
         "chained digest tables"
      );
      return;
   }


   if(digest_file_offset > 0) fd->seekg(digest_file_offset, std::ios::beg);

   dbg(
      LOG_DEBUG,
      "At location %" SIZE_T_FORMAT " in '%s' attempting to read %" SIZE_T_FORMAT " bytes",
      digest_file_offset,
      ChainedDigestTableFilename,
      fh.attrs.MaxDigestTableSizeInBytes
   );

   fd->read(tx_buffer, fh.attrs.MaxDigestTableSizeInBytes);
   LocalBytesRead = fd->gcount();
   fh_fclose(fd);
   fd = nullptr;

   dbg(LOG_DEBUG, "BytesRead is %" SIZE_T_FORMAT " bytes", LocalBytesRead);

   if(LocalBytesRead == 0)
   {
      dbg(LOG_ERROR, "Read 0 bytes from '%s'", ChainedDigestTableFilename);
      ExitAndShowLog(1);
   }

   // ------------------// ------------------// ------------------//
   // ------------------
   // ------------------// ------------------// ------------------//
   // ------------------
   UsingValidation = 0; // NOTE - TURNING OFF VIP HERE WHEN SENDING THIS BUFFER to avoid
                        // recursion into TestIfWeNeedToSendDigestTable()
   UsingValidation = 0; // NOTE - TURNING OFF VIP HERE WHEN SENDING THIS BUFFER to avoid
                        // recursion into TestIfWeNeedToSendDigestTable()

   if(sendTransmitBufferBytes(LocalBytesRead) == 0)
   {
      ExitAndShowLog(1);
   }

   UsingValidation = 1; // NOTE - TURNING VIP *BACK ON* HERE
   UsingValidation = 1; // NOTE - TURNING VIP *BACK ON* HERE
   // ------------------// ------------------// ------------------//
   // ------------------
   // ------------------// ------------------// ------------------//
   // ------------------


   // Put tx_buffer_backup into tx_buffer first
   memscpy(tx_buffer, FIREHOSE_TX_BUFFER_SIZE, tx_buffer_backup,
           FIREHOSE_TX_BUFFER_SIZE); // memcpy


   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   BackRawMode = RawMode; // Backup this flag
   RawMode = 0;           // turn off RawMode to ensure XML parsing happens on incoming packet
   GetNextPacket();       // Get the packet
   RawMode = BackRawMode; // Put back whatever state we were in

   // dbg (LOG_INFO, "\n\n\n\n\n\nAARON - Reading twice");
   // GetNextPacket();  // this will set all variables, including GotACK

   if(GotACK)
   {
      dbg(LOG_DEBUG, "Got the ACK");
      dbg(
         LOG_DEBUG,
         "DIGEST TABLE ACCEPTED - Just RESET CurrentDigestLocation "
         "to 0, since now using a new table"
      );
      CurrentDigestLocation = 0;
      digest_file_offset += MaxNumDigestsPerTable * DigestSizeInBytes;
   }
   else
   {
      dbg(LOG_ERROR, "Digest Table was NOT ACCEPTED. Can't continue");
      ExitAndShowLog(1);
   }
}

void FirehoseLoader::SendSignedDigestTable(char* signedDigestTableFilename)
{
   char* TempFile = nullptr;
   std::shared_ptr<std::fstream> fd;
   SIZE_T localBytesRead, FileSize;

   if(strlen(signedDigestTableFilename) == 0) return;

   UsingValidation = 1;
   ShowDigestPerPacket = 1;

   // find this file signedDigestTableFilename
   TempFile = find_file(signedDigestTableFilename, 1);

   if(TempFile == NULL)
   {
      dbg(LOG_ERROR, "'%s' not found", signedDigestTableFilename);
      ExitAndShowLog(1);
   }

   dbg(LOG_INFO, "Signed Digest Table (%" SIZE_T_FORMAT " bytes) '%s' ", LastFindFileFileSize, full_filename_with_path);

   fd = ReturnFileHandle(TempFile, MAX_PATH_SIZE,
                         "rb"); // will exit if not successful

   FileSize = ReturnFileSize(fd);

   dbg(LOG_DEBUG, "File size is %" SIZE_T_FORMAT " bytes", (FileSize));

   fd->read(tx_buffer, FileSize);
   localBytesRead = fd->gcount();
   fh_fclose(fd);
   fd = nullptr;

   if(localBytesRead == 0)
   {
      dbg(LOG_ERROR, "Read 0 bytes from '%s'", signedDigestTableFilename);
      ExitAndShowLog(1);
   }

   if(localBytesRead < FileSize)
   {
      dbg(
         LOG_ERROR,
         "Read %" SIZE_T_FORMAT " bytes from '%s' but expected %" SIZE_T_FORMAT,
         localBytesRead,
         signedDigestTableFilename,
         FileSize
      );
      ExitAndShowLog(1);
   }


   NumDigestsFound = 54;
   // if(NumDigestsFound < 0:
   // log_error("Something is wrong with your digest table. For testing it at
   // least needs a 40 byte MBN header") log_error("Ideally it should be fully
   // digitally signed") ExitAndShowLogName()

   dbg(LOG_INFO, "SIGNED Digest table had %" SIZE_T_FORMAT " digests", NumDigestsFound);

   dbg(LOG_INFO, "MaxNumDigestsPerTable is %" SIZE_T_FORMAT, MaxNumDigestsPerTable);

   CurrentDigestLocation = MaxNumDigestsPerTable - NumDigestsFound;
   CurrentDigestLocation = MaxNumDigestsPerTable - 54;
   dbg(
      LOG_INFO,
      "Setting CurrentDigestLocation to number %" SIZE_T_FORMAT ", meaning %" SIZE_T_FORMAT
      " hashes to go before a new table must be sent",
      CurrentDigestLocation,
      MaxNumDigestsPerTable - CurrentDigestLocation
   );


   // ------------------// ------------------// ------------------//
   // ------------------
   // ------------------// ------------------// ------------------//
   // ------------------
   UsingValidation = 0; // NOTE - TURNING OFF VIP HERE WHEN SENDING THIS BUFFER
   UsingValidation = 0; // NOTE - TURNING OFF VIP HERE WHEN SENDING THIS BUFFER

   dbg(LOG_INFO, "Calling sendTransmitBufferBytes(%" SIZE_T_FORMAT " bytes)", FileSize);

   if(sendTransmitBufferBytes(FileSize) == 0)
   {
      ExitAndShowLog(1);
   }

   UsingValidation = 1; // NOTE - TURNING VIP *BACK ON* HERE
   UsingValidation = 1; // NOTE - TURNING VIP *BACK ON* HERE
   // ------------------// ------------------// ------------------//
   // ------------------
   // ------------------// ------------------// ------------------//
   // ------------------


   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(GotACK)
      dbg(LOG_DEBUG, "Got the ACK");
   else
   {
      dbg(
         LOG_ERROR,
         "The Digitally Signed Digest Table was rejected by the target\n"
         "\n\t1. Are you sure it was digitally signed? Was it digitally "
         "signed correctly for this target?"
         "\n\t2. Are you sure you have a secure boot enabled target? Maybe "
         "your target was expecting an XML file?"
         "\n\nBecause you used --signeddigests=%s option, this tool sends "
         "\n'%s' as the first packet. It is sent as a RAW binary file for VIP"
         "\nYour target either wasn't expecting this file, OR your file was "
         "*not* digitially signed correctly"
         "\n\nCan't continue due to this error. Your target does *not* like "
         "what you sent\n\n",
         signedDigestTableFilename,
         signedDigestTableFilename
      );
      ExitAndShowLog(1);
   }

   dbg(LOG_INFO, "\t _____ _                   _                  ");
   dbg(LOG_INFO, "\t/  ___(_)                 | |                 ");
   dbg(LOG_INFO, "\t\\ `--. _  __ _ _ __   __ _| |_ _   _ _ __ ___ ");
   dbg(LOG_INFO, "\t `--. \\ |/ _` | '_ \\ / _` | __| | | | '__/ _ \\");
   dbg(LOG_INFO, "\t/\\__/ / | (_| | | | | (_| | |_| |_| | | |  __/");
   dbg(LOG_INFO, "\t\\____/|_|\\__, |_| |_|\\__,_|\\__|\\__,_|_|  \\___|");
   dbg(LOG_INFO, "\t          __/ |                               ");
   dbg(LOG_INFO, "\t         |___/   \n");
   dbg(LOG_INFO, "\t______                      _ ");
   dbg(LOG_INFO, "\t| ___ \\                    | |");
   dbg(LOG_INFO, "\t| |_/ /_ _ ___ ___  ___  __| |");
   dbg(LOG_INFO, "\t|  __/ _` / __/ __|/ _ \\/ _` |");
   dbg(LOG_INFO, "\t| | | (_| \\__ \\__ \\  __/ (_| |");
   dbg(LOG_INFO, "\t\\_|  \\__,_|___/___/\\___|\\__,_|\n");
   dbg(
      LOG_DEBUG,
      "FIREHOSE: Signed DIGEST sent. This is not counted towards command "
      "sent"
   );
   dbg(LOG_DEBUG, "FIREHOSE: PacketsSent reset to 0");

   PacketsSent = 0;
   CurrentDigestLocation -= 1; // Go back 1 since DigSig table doesn't count
}

void FirehoseLoader::StripNewLines(char* Buffer, SIZE_T BufferLength, SIZE_T Start, SIZE_T Length)
{
   SIZE_T i;

   if(Start + Length > BufferLength) return;

   for(i = 0; i < Length; i++)
   {
      if(Buffer[Start + i] == '\n' || Buffer[Start + i] == '\r') Buffer[Start + i] = 0x20; // make a space
   }
}

void FirehoseLoader::
   PrettyPrintHexValueIntoTempBuffer(uint8_t* temp_hash_val, int32_t length, int32_t offset, int32_t MaxLength)
{
   int32_t i;

   if(offset + (2 * length) > FIREHOSE_TX_BUFFER_SIZE) return; // this is too big

   if(length > MaxLength) return; // this is too big

   for(i = 0; i < length; i++)
   {
      sprintf(&temp_buffer[offset + i * 2], "%02X", temp_hash_val[i]);
   }

   temp_buffer[offset + i * 2] = '\0';
}

uint32_t FirehoseLoader::WritePort(uint8_t* pData, uint32_t length, uint32_t MaxLength, uint8_t RawData)
{
   uint32_t localBytesWritten = 0, i = 0, localNumTries = 100;
   static int32_t HashTablesSent = 1; // start at 1 since the signed digest table was #0
   enum LOG_TYPES LogType = LOG_DEBUG;
   char answer = 'y';

   if(Interactive) LogType = LOG_INFO;

   if(length > MaxLength)
   {
      dbg(
         LOG_ERROR,
         "In WritePort and length %d bytes is greater than buffer size of %d "
         "bytes",
         length,
         MaxLength
      );
      ExitAndShowLog(1);
   }

   if(ShowDigestPerPacket)
   {
      sechsharm_sha256_init(&context_per_packet);
      sechsharm_sha256_update(
         &context_per_packet,
         context_per_packet.leftover,
         &(context_per_packet.leftover_size),
         (uint8_t*)pData,
         length
      );
      sechsharm_sha256_final(
         &context_per_packet,
         context_per_packet.leftover,
         &(context_per_packet.leftover_size),
         temp_hash_value
      );

      // Show some of the rawdata packet COMMAND_TRACE_BYTES_TO_RECORD/2 since
      // each HEX value takes 2 bytes
      PrettyPrintHexValueIntoTempBuffer(temp_hash_value, 32, 0,
                                        32); // from, size, offset, maxlength
   }

   if(RawMode)
   {
      char Size1[2048];

      ReturnSizeString(length, Size1, sizeof(Size1));

      if(!ShowDigestPerPacket)
         dbg(
            LogType,
            "CHANNEL DATA (P%" SIZE_T_FORMAT4 ") (H%" SIZE_T_FORMAT5 ") (%s) - HOST TO TARGET -->",
            PacketsSent,
            CurrentDigestLocation,
            Size1
         );
      else
         dbg(
            LogType,
            "CHANNEL DATA (P%" SIZE_T_FORMAT4 ") (H%" SIZE_T_FORMAT5 ") (%s) - HOST TO TARGET --> SHA256 DIGEST: %s",
            PacketsSent,
            CurrentDigestLocation,
            Size1,
            temp_buffer
         );
   }
   else
   {
      if(!ShowDigestPerPacket)
         dbg(
            LogType,
            "CHANNEL DATA (P%" SIZE_T_FORMAT4 ") (H%" SIZE_T_FORMAT5 ") (%7d bytes) - HOST TO TARGET -->\n"
            "================================================================="
            "==========================================\n"
            "%s\n"
            "================================================================="
            "===========================================\n",
            PacketsSent,
            CurrentDigestLocation,
            length,
            pData
         );
      else
         dbg(
            LogType,
            "CHANNEL DATA (P%" SIZE_T_FORMAT4 ") (H%" SIZE_T_FORMAT5
            ") (%7d bytes) - HOST TO TARGET --> SHA256 DIGEST: %s\n"
            "================================================================="
            "==========================================\n"
            "%s\n"
            "================================================================="
            "===========================================\n",
            PacketsSent,
            CurrentDigestLocation,
            length,
            temp_buffer,
            pData
         );
   }

   // dbg (LOG_DEBUG, "In WritePort(uint8_t *pData, %d)\n",length);

   if(PrettyPrintRawPacketsToo == 1)
   {
      PRETTYPRINT(pData, length, MaxLength); // always show everything
   }
   else if(RawData == 0 && VerboseLevel >= LOG_DEBUG)
   {
      PRETTYPRINT(pData, length, MaxLength); // else, only if it is an XML file
                                             // and user didn't request silent
   }


   if((Interactive == 2 && RawData == 1) || (Interactive == 1 && RawData == 0))
   {
      dbg(
         LOG_INFO,
         "\n*INTERACTIVE* Press any key to **SEND** the above %d bytes (q to "
         "quit interactive): ",
         length
      );
      answer = static_cast<char>(getchar());
      fseek(stdin, 0, SEEK_END); // get rid of extra \n keys

      if(answer == 'q') Interactive = 0;

      dbg(LOG_INFO, "\n\n");
   }


   if(Simulate)
      localBytesWritten = length; // strlen((const char *)tx_buffer);
   else
   {
      for(i = 0; i < localNumTries; i++)
      {
         // Use firehose protocol for Tx
         Device::SharedByteBufferPtr pWriteDataBuffer = Buffer::createBuffer(length);
         pWriteDataBuffer->assign(reinterpret_cast<uint8_t*>(pData), length);
         m_pFirehose->sendSync(pWriteDataBuffer, std::nullopt, false);
         localBytesWritten = length;
         pData += localBytesWritten;

         if(localBytesWritten < length)
            dbg(
               LOG_DEBUG,
               "Only %d bytes written of %d requested, attempt %d of %d",
               localBytesWritten,
               length,
               i + 1,
               localNumTries
            );

         length -= localBytesWritten; // this is what breaks us out

         if(length == 0)
         {
            break;
         }
      } // end i

      if(i >= localNumTries)
      {
         dbg(
            LOG_ERROR,
            "Could not write to '%s', Your device is probably *not* on this "
            "port, attempted %d times",
            port_name,
            i
         );
         ExitAndShowLog(1);
      }
   }

   if(localBytesWritten == 0)
   {
      dbg(LOG_ERROR, "WritePort failed, reported 0 bytes written");
      ExitAndShowLog(1);
      return 0;
   }

   /*
     if(localBytesWritten!=length)
     {
       dbg (LOG_ERROR, "(P%" SIZE_T_FORMAT4 ") (H%" SIZE_T_FORMAT5 ") Tried to
     write %7d bytes, but only wrote %d bytes)
     ---------->\n\n\n",PacketsSent,CurrentDigestLocation,length,localBytesWritten);
       ExitAndShowLog(1);
       return 0;
     }
   */
   if(CreateDigests)
   {
      sechsharm_sha256_init(&context_per_packet);
      sechsharm_sha256_update(
         &context_per_packet,
         context_per_packet.leftover,
         &(context_per_packet.leftover_size),
         (uint8_t*)tx_buffer,
         localBytesWritten
      );
      sechsharm_sha256_final(
         &context_per_packet,
         context_per_packet.leftover,
         &(context_per_packet.leftover_size),
         temp_hash_value
      );

      if(RawData == 0)
      {
         // offset 48 would be NOP, as in ?><data><NOP value, i.e. 48 is the
         // beginning of the command
         CopyString(
            temp_buffer,
            tx_buffer,
            0,
            48,
            COMMAND_TRACE_BYTES_TO_RECORD,
            FIREHOSE_TX_BUFFER_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
      }
      else
      {
         // Show some of the rawdata packet COMMAND_TRACE_BYTES_TO_RECORD/2
         // since each HEX value takes 2 bytes
         PrettyPrintHexValueIntoTempBuffer(
            pData,
            COMMAND_TRACE_BYTES_TO_RECORD / 2,
            0,
            32
         ); // from, size, offset,
            // maxlength
      }

      for(i = static_cast<uint32_t>(strlen(temp_buffer)); i < COMMAND_TRACE_BYTES_TO_RECORD; i++)
         temp_buffer[i] = 0x20; // string wasn't long enough, so pad with space

      AppendToBuffer(temp_buffer, " ---- ",
                     FIREHOSE_TX_BUFFER_SIZE); // 0 to 37 is now full

      PrettyPrintHexValueIntoTempBuffer(
         temp_hash_value,
         32,
         static_cast<int32_t>(strlen(temp_buffer)),
         32
      ); // from, size, offset, maxlength
      StripNewLines(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, 0, static_cast<int32_t>(strlen(temp_buffer)));


      if(createcommandtrace)
      {
         fc = ReturnFileHandle(CommandTraceName, strlen(CommandTraceName),
                               "a"); // will exit if not successful

         if(CurrentDigestLocation == (MaxNumDigestsPerTable - 1))
         {
            // this condition is when we would send a HASH table, so simulate
            // this
            dbg(
               LOG_INFO,
               "(P%" SIZE_T_FORMAT4 ") (H%" SIZE_T_FORMAT5 ") (%7d bytes)                   NEW HASH TABLE SENT HERE "
               "(%3i)                 ---- "
               "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
               "XX\n",
               PacketsSent,
               CurrentDigestLocation,
               (uint32_t)fh.attrs.MaxDigestTableSizeInBytes,
               HashTablesSent
            );
            HashTablesSent++;
            PacketsSent++;
            CurrentDigestLocation = 0;
         }

         dbg(
            LOG_INFO,
            "(P%" SIZE_T_FORMAT4 ") (H%" SIZE_T_FORMAT5 ") (%7d bytes) %s\n",
            PacketsSent,
            CurrentDigestLocation,
            localBytesWritten,
            temp_buffer
         );
         fh_fclose(fc);
         fc = nullptr;
      }

      // BINARY FILE
      // if(strlen(SignedDigestTableFilename)>0)
      if(CreateDigests)
      {
         std::string outDir(MainOutputDir);
         char digestTable[MAX_PATH_SIZE];
         snprintf_x(digestTable, sizeof(digestTable), "%s", (outDir + SLASH + "DIGEST_TABLE.bin").c_str());

         ft = ReturnFileHandle(digestTable, strlen(digestTable),
                               "ab"); // will exit if not successful

         if(ft != nullptr)
         {
            ft->write(reinterpret_cast<const char*>(temp_hash_value), 32);
            fh_fclose(ft);
            ft = nullptr;
         }
      }
   }


   PacketsSent++;
   CurrentDigestLocation++;

   return localBytesWritten;
}

uint32_t FirehoseLoader::ReadPort(uint8_t* pData, uint32_t length, uint32_t MaxLength)
{
   uint32_t localBytesRead = 0;
   uint64_t byteToRead = std::min(length, MaxLength);
   Device::SharedByteBufferPtr pReadDataBuffer;
   dbg(LOG_DEBUG, "CharsInBuffer=%d Trying to read from USB %ld bytes", CharsInBuffer, length);
   // Issue blocking read here
   if(m_rxTimeoutInMs.has_value() && m_rxTimeoutInMs.value() > std::chrono::milliseconds(0))
   {
      m_pFirehose->waitForData(m_rxTimeoutInMs);
   }
   else
   {
      m_pFirehose->waitForData(std::chrono::seconds(100));
   }
   m_pFirehose->getBytes(pData, static_cast<uint32_t>(byteToRead), localBytesRead);
   if(0 == localBytesRead)
   {
      dbg(
         LOG_ERROR,
         "Could not read from '%s', Your device is probably *not* on this "
         "port\n\n",
         port_name
      );
      ExitAndShowLog(1);
   }

   dbg(LOG_DEBUG, "CHANNEL DATA (%ld bytes) <-- TARGET to HOST", localBytesRead);
   return localBytesRead;
}

uint32_t FirehoseLoader::OpenPort(char* pData)
{
   (void)pData; // Suppress unused parameter warning
#if 0
    struct timeval time_start, time_end;
    double Elapsed;
    gettimeofday (&time_start, NULL);

    if (pData == NULL || strlen (pData) == 0)
    {
        dbg (LOG_ERROR, "Port to open was not specified. Please use --port= option\n\n");
        ExitAndShowLog (1);
    }

#ifdef _MSC_VER // i.e. if compiling under Windows
    port_fd = //CreateFileA(  (LPCSTR)pData,
        CreateFileA (  (LPCSTR) pData,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_ALWAYS,//OPEN_EXISTING,
                       0,              // FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
                       NULL);

    if (port_fd == INVALID_HANDLE_VALUE)
    {
        dbg (LOG_ERROR, "Failed to open com port '%s'", pData);
        return 1;
    }

#else
    port_fd = open (pData, O_RDWR | O_SYNC);

    if (port_fd == -1)
    {
        dbg (LOG_ERROR, "Could not open '%s'\n", pData);
        return 1;
    }

#endif

    dbg (LOG_DEBUG, "port_fd=0x%X\n", port_fd);


    gettimeofday (&time_end, NULL);
    Elapsed = ReturnTimeInSeconds (&time_start, &time_end);

    if (Elapsed >= ComPortOpenTimeout)
    {
        dbg (LOG_ERROR, "It took %16.8f seconds to open port. Which is longer than %.3f. This indicates your target is not stable", Elapsed, ComPortOpenTimeout);
        ExitAndShowLog (1);
    }
    else
        dbg (LOG_INFO, "Took %16.8f seconds to open port", Elapsed);
#endif
   return 0;
}

void FirehoseLoader::ClosePort(void)
{
#if 0
    if (port_fd)
    {
#ifdef _MSC_VER // i.e. if compiling under Windows
        CloseHandle (port_fd);
        port_fd = NULL;
#else
        close (port_fd);
        port_fd = 0;
#endif
    }


#endif
}

void FirehoseLoader::
   printBuffer(const uint8_t* buffer, uint32_t length, uint32_t MaxLength, const char* func_name, int32_t line_number)
{
   char CurrentChar = 0;
   char szBuffer[1024];

   // uint8_t asciiRepresentation[rowLength + 1];
   // asciiRepresentation[rowLength] = '\0';
   uint32_t i, j = 0, k = 0, b = 0;

   if(VerboseLevel == LOG_ALWAYS) return; // user has requested loglevel=0


   if(length > MaxLength)
   {
      dbg(
         LOG_ERROR,
         "PRETTYPRINT %s:%d Buffer is %d bytes which is BIGGER than %d "
         "bytes\n",
         func_name,
         line_number,
         length,
         MaxLength
      );
      ExitAndShowLog(1);
   }

   // dbg (LOG_ONLY, "PRETTYPRINT %s:%d Buffer is %d bytes\n", func_name,
   // line_number, length);
   dbg(LOG_ONLY, "PRETTYPRINT Buffer is %d bytes\n", length);

   if(length == 0)
   {
      return;
   }

   i = 0;

   while(1)
   {
      // dbg (LOG_INFO, "\n");
      for(j = 0; j < 16; j++)
      {
         if(j + i >= length)
         {
            break;
         }

         if(b)
         {
            dbg(LOG_INFO, "\n1 i+j=%i", i + j);
         }

         CurrentChar = buffer[j + i];
         sprintf(&szBuffer[j * 3], "%02X ", (CurrentChar & 0xFF));
      } // end j

      k = j * 3;

      while(16 - j > 0)
      {
         j++;

         if(b)
         {
            dbg(LOG_INFO, "\n1 k=%i", k);
         }

         sprintf(&szBuffer[k], "   ");
         k += 3; // 1 for each space
      }

      for(j = 0; j < 16; j++)
      {
         if(j + i >= length)
         {
            break;
         }

         if(b)
         {
            dbg(LOG_INFO, "\n2 i+j=%i", i + j);
         }

         if(b)
         {
            dbg(LOG_INFO, "\nk+j=%i", k + j);
         }

         CurrentChar = buffer[j + i];

         if(CurrentChar >= 0x20 && CurrentChar <= 0x7E)
         {
            if(CurrentChar == '%')
            {
               sprintf(&szBuffer[k + j], "%%%%");
               k++;
            }
            else
               sprintf(&szBuffer[k + j], "%c",
                       CurrentChar); // this puts a null
         }
         else
            sprintf(&szBuffer[k + j], ".");
      } // end j

      dbg(LOG_ONLY, szBuffer);

      k += j;
      i += 16;

      if(i >= length)
      {
         break;
      }

   } // end while

   sprintf(szBuffer, "\n\n");
   dbg(LOG_ONLY, szBuffer);

   return;
}

void FirehoseLoader::InitBufferWithXMLHeader(char* MyBuffer, SIZE_T Length)
{
   memset(MyBuffer, 0x0, Length);
   memscpy(MyBuffer, (size_t)Length, xml_header, (size_t)XML_HEADER_LENGTH);
   // MyBuffer[XML_HEADER_LENGTH] = '\0';
} // end void InitBufferWithXMLHeader(void)

void FirehoseLoader::AppendToBuffer(char* MyBuffer, const char* buf, SIZE_T MaxBufferSize)
{
   size_t SpaceLeft, CharsToAdd = 0;
   size_t Length;

   Length = strlen((const char*)MyBuffer);
   CharsToAdd = strlen((const char*)buf); // size of string to append

   // SpaceLeft = FIREHOSE_TX_BUFFER_SIZE - Length - XML_TAIL_LENGTH;      //
   // tx_buffer is transmit_buffer of size FIREHOSE_TX_BUFFER_SIZE
   SpaceLeft = MaxBufferSize - Length - XML_TAIL_LENGTH; // tx_buffer is transmit_buffer of size
                                                         // FIREHOSE_TX_BUFFER_SIZE


   if(CharsToAdd > SpaceLeft)
   {
      return; // too big to add this
   }

   // NOTE: If you're *not* seeing your messages, increase the size of
   // transmit_buffer[FIREHOSE_TX_BUFFER_SIZE]
   //       or break up your logging

   // memcpy((MyBuffer+Length), buf, CharsToAdd);
   memscpy((MyBuffer + Length), (size_t)MaxBufferSize, buf,
           (size_t)CharsToAdd); // memcpy
   MyBuffer[Length + CharsToAdd] = '\0';

} // end void AppendToBuffer(car *MyBuffer, char *buf)


uint32_t FirehoseLoader::sendTransmitBufferBytes(SIZE_T Length)
{
   TestIfWeNeedToSendDigestTable();
   BytesWritten = WritePort(
      (uint8_t*)tx_buffer,
      static_cast<uint32_t>(Length),
      FIREHOSE_TX_BUFFER_SIZE,
      1
   ); // 1 means it is RawData
   return static_cast<uint32_t>(BytesWritten);
}
uint32_t FirehoseLoader::sendTransmitBuffer(void)
{
   SIZE_T BytesBeingSent;

   TestIfWeNeedToSendDigestTable();

   BytesBeingSent = strlen((const char*)tx_buffer);
   //  dbg (LOG_DEBUG,"CHANNEL DATA (%ld bytes) - HOST TO TARGET
   //  -->\n",BytesBeingSent);

   BytesWritten = WritePort(
      (uint8_t*)tx_buffer,
      static_cast<uint32_t>(BytesBeingSent),
      FIREHOSE_TX_BUFFER_SIZE,
      0
   ); // 0 means it is a string
   return static_cast<uint32_t>(BytesWritten);
}

FirehoseLoader::SIZE_T FirehoseLoader::CopyString(
   char* Dest,
   const char* Source,
   SIZE_T Dstart,
   SIZE_T Sstart,
   SIZE_T length,
   SIZE_T DestSize,
   SIZE_T SourceSize
)
{
   SIZE_T i = Dstart + length;
   char Temp;

   // WARNING, don't use dbg (LOG_INFO,etc) in this function or you get infinite
   // recurrsion!! WARNING, don't use dbg (LOG_INFO,etc) in this function or you
   // get infinite recurrsion!! WARNING, don't use dbg (LOG_INFO,etc) in this
   // function or you get infinite recurrsion!!

   // if(length>=6000)
   //   dbg (LOG_INFO, "\nlength=%" SIZE_T_FORMAT " CopyString String of length
   //   %" SIZE_T_FORMAT " and DESTINATION Array of length %" SIZE_T_FORMAT
   //   ,length,Dstart+length-1,DestSize);

   if(Dest == NULL)
   {
      dbg(LOG_INFO, "CopyString Dest is NULL"); // Dest is null
      ExitAndShowLog(1);
   }

   if(Source == NULL)
   {
      dbg(LOG_INFO, "CopyString Source is NULL"); // Source is null
      ExitAndShowLog(1);
   }

   if(Sstart + length > SourceSize)
   {
      dbg(
         LOG_INFO,
         "CopyString Range of %" SIZE_T_FORMAT " exceeds Array of length %" SIZE_T_FORMAT,
         Sstart + length,
         SourceSize
      );
      // ExitAndShowLog(1);
      return 0;
   } // range to copy is beyond source string

   if(i > 0 && (i - 1) > DestSize)
   {
      dbg(LOG_INFO, "\ni=%" SIZE_T_FORMAT ", (i-1)=%" SIZE_T_FORMAT " and DestSize=%" SIZE_T_FORMAT, i, i - 1, DestSize);
      dbg(
         LOG_INFO,
         "\nCopyString String of length %" SIZE_T_FORMAT " at offset %" SIZE_T_FORMAT
         " of *dest will exceed Array of length %" SIZE_T_FORMAT " by %" SIZE_T_FORMAT " bytes",
         length,
         Dstart,
         DestSize,
         (length + Dstart - 1) - DestSize
      );
      // ExitAndShowLog(1);
      return 0;
   } // string to copy over is too big for destination

   if(length == 0)
   {
      // This is a case like filename="", and so user is copying a null string
      Dest[Dstart] = '\0'; // NULL
      return 1;
   }

   // dbg (LOG_INFO, "CopyString(0x%X, 0x%X, %" SIZE_T_FORMAT ", %"
   // SIZE_T_FORMAT ", %" SIZE_T_FORMAT ", %" SIZE_T_FORMAT ", %" SIZE_T_FORMAT
   // ")",Dest,Source,Dstart,Sstart,length,DestSize,SourceSize);
   if(Source == Dest)
      return length; // user passed same buffer as source and destination,
                     // therefore nothing to copy

   for(i = 0; i < length; i++)
   {
      Temp = *(Source + Sstart + i);

      *(Dest + Dstart + i) = Temp;
   } // end i

   *(Dest + Dstart + i) = '\0'; // NULL

   return length;
}

char* FirehoseLoader::RemoveBannedChars(char* p)
{
   char* pOrig = p;

   while(*p != '\0') // NULL)
   {
      if(*p == '"' || *p == '<' || *p == '>') *p = '_';

      p++;
   }

   return pOrig;
}

/*
char * RemovePossibleDecimalPoint(char *p)
{
    char *pOrig = p;

    while(*p!='\0')
    {
        if( *p=='.')
        {
            *p = '\0'; // replace with NULL, number ends here
            break;
        }

         p++;
    }

    return pOrig;
}
*/


/* Converts an decimal ascii string to a uint64_t value.
 *
 * param  ascii   NULL terminated string containing the number.
 * param  value[out]  Converted value.
 *
 * returns  0 on success, -1 on error
 **/
int32_t FirehoseLoader::hex_atoi(const char* ascii, uint64_t* value)
{
   uint32_t multiplier = 16;
   int32_t i = 0;
   char v;
   uint64_t result = 0;
   uint64_t val;

   /* Check for empty string. */
   if(ascii[0] == '\0') return -1;

   while(ascii[i] != '\0')
   {
      result = result * multiplier;
      v = ascii[i];
      if(v >= '0' && v <= '9')
         val = (v - 48); /* 48 is the ascii value for 0. */
      else if(v >= 'A' && v <= 'F')
         val = (v - 'A' + 10); /* 64 ascii value for A, +10 because 0xA=10 */
      else if(v >= 'a' && v <= 'f')
         val = (v - 'a' + 10); /* 98 ascii value for a, +10 because 0xa=10 */
      else
         return -1;

      /* Check for overflow. */
      if(result > UINT64_MAX - val) return -1;

      result += val;
      i++;
   }
   *value = result;
   return 0;
}


FirehoseLoader::SIZE_T FirehoseLoader::stringToNumber(const char* buffer, boolean* retval)
{
   char c;
   SIZE_T number = 0, ScaleFactor = 10;
   int32_t i = 0, Length = static_cast<int32_t>(strlen(buffer));
   int32_t sign = 1;

   if(NULL == retval) return number;

   *retval = FALSE; // assume false

   if(*buffer == '\0' || Length > 19) return number;
   i = 0;
   /* Check if the number is negative. */
   if(buffer[0] == '-')
   {
      sign = -1;
      i++;
   }
   for(; i < Length; i++)
   {
      c = buffer[i];

      if(c >= '0' && c <= '9')
         number = (number * ScaleFactor) + (c - 0x30); // ScaleFactor = 10
      else if(c >= 'a' && c <= 'f')
         number = (number * ScaleFactor) + (c - 87); // i.e. 'a' = 97, so 97-87 = 10
      else if(c >= 'A' && c <= 'F')
         number = (number * ScaleFactor) + (c - 55); // i.e. 'A' = 65, so 65-55 = 10
      else if(c == ',')
         continue; // user did this 12,345
      else if(c == 'x' || c == 'X')
      {
         /* user did 0x or 0X or any other weird combination (1xA, 23x0, 456xB,
            not robust but simple and works for most cases) */
         ScaleFactor = 16; // this is a hex value
         continue;
      }
      else if(c == '.')
         break; // user did this 12.3, so return 12
      else
         return 0; // user provided something unexpected, return 0
   }

   *retval = TRUE; // made it this far, all is well

   return sign * number;
}

int32_t FirehoseLoader::ReturnNumAttributes(void)
{
   return NUM_ATTRIBUTES;
}

int32_t FirehoseLoader::ReturnAttributeLocation(char* NewAttrName)
{
   volatile SIZE_T i;
   SIZE_T StringLength1 = 0, StringLength2 = 0;

   StringLength1 = strlen((const char*)NewAttrName);

   for(i = 0; i < (SIZE_T)ReturnNumAttributes(); i++)
   {
      StringLength2 = strlen((const char*)AllAttributes[i].Name);

      if(StringLength1 < StringLength2) StringLength1 = StringLength2; // want the biggest number

      if(strncasecmp(AllAttributes[i].Name, NewAttrName, StringLength1) == 0)
      {
         return static_cast<int32_t>(i);
      }
   }

   return -1;
}

size_t FirehoseLoader::replace_xml_escape_chars(char* data, size_t size)
{
   char* out = data;
   char* in = data;
   char* end = data + size;
   while(in < end)
   {
      if(*in == '&')
      {
         if(strncmp("&quot;", in, 6) == 0)
         {
            *out = '"';
            out++;
            in += 6;
         }
         else if(strncmp("&apos;", in, 6) == 0)
         {
            *out = '\'';
            out++;
            in += 6;
         }
         else if(strncmp("&lt;", in, 4) == 0)
         {
            *out = '<';
            out++;
            in += 4;
         }
         else if(strncmp("&gt;", in, 4) == 0)
         {
            *out = '>';
            out++;
            in += 4;
         }
         else if(strncmp("&amp;", in, 5) == 0)
         {
            *out = '&';
            out++;
            in += 5;
         }
         else
         {
            *out = *in;
            out++;
            in++;
         }
         continue;
      }
      if(in != out) *out = *in;
      out++;
      in++;
   }

   /* If some data was replaced, make sure the new string is NULL terminated*/
   if(((size_t)(out - data)) < size) *out = '\0';

   return out - data;
}

int32_t FirehoseLoader::AssignAttribute(char* NewAttrName, char* NewAttrValue, SIZE_T NewAttrValueLen)
{
   volatile SIZE_T i, j = 0;
   SIZE_T TempValue;
   // dbg (LOG_INFO, "\n\nIn
   // AssignAttribute(%s,%s)\n",NewAttrName,NewAttrValue); logMessage("In
   // AssignAttribute(%s,%s) and
   // fh.attrs.Verbose=0x%X",NewAttrName,NewAttrValue,fh.attrs.Verbose);

   // Get attribute location
   i = ReturnAttributeLocation(NewAttrName);

   if(i == -1)
   {
      if(fh.attrs.Verbose == TRUE)
         dbg(LOG_INFO, "IGNORING UNRECOGNIZED Attribute '%s' with value '%s'", NewAttrName, NewAttrValue);

      return 0;
   }

   // To be this far we matched an attribute

   // if(fh.attrs.Verbose == TRUE)
   //     logMessage("Recognized Attribute Number %d ('%s') with value '%s' type
   //     is '%c'",i,NewAttrName,NewAttrValue,AllAttributes[i].Type);
   if(CopyString(
         AllAttributes[i].Raw,
         NewAttrValue,
         0,
         0,
         strlen(NewAttrValue),
         sizeof(AllAttributes[i].Raw),
         NewAttrValueLen
      ) == 0)
   {
      dbg(
         LOG_ERROR,
         "Failed to copy string '%s' of length %i into AllAttributes[%i].Raw "
         "of length %i for fh.attrs.%s",
         NewAttrValue,
         strlen(NewAttrValue),
         i,
         sizeof(AllAttributes[i].Raw),
         NewAttrName
      );
      return 1;
   }

   if(AllAttributes[i].Type == 'i' || AllAttributes[i].Type == 'w' || AllAttributes[i].Type == 'n' ||
      AllAttributes[i].Type == 'b')
   {
      // to be here means attribute is some sort of number (not a string or a
      // complex variable such as "NUM_DISK_SECTORS-33."
      boolean num_conversion;

      TempValue = stringToNumber((const char*)AllAttributes[i].Raw, &num_conversion);

      if(FALSE == num_conversion)
      {
         dbg(
            LOG_INFO,
            "Call to stringToNumber failed on attribute '%s' with value '%s'",
            NewAttrName,
            AllAttributes[i].Raw
         );
         return 1; // error
      }

      // Enforce bounds checking on the number we just converted
      // When it's handleFirmwareWrite we allow SECTOR_SIZE_IN_BYTES=1, so don't
      // enforce any rules if it's that
      if(CurrentHandlerFunction != &FirehoseLoader::handleFirmwareWrite)
      {
         if((TempValue % AllAttributes[i].MultipleOf != 0))
         {
            dbg(LOG_INFO, "Attribute '%s' is not a multiple of %i", NewAttrName, AllAttributes[i].MultipleOf);
            return 1; // error
         }
      }

      if(AllAttributes[i].Max != 0)
      {
         // Need to check against max value
         if(TempValue > AllAttributes[i].Max)
         {
            dbg(LOG_INFO, "Attribute '%s' is larger than %i", NewAttrName, AllAttributes[i].Max);
            return 1; // error
         }
      }

      if(AllAttributes[i].Min != 0)
      {
         // Need to check against min value
         if(TempValue < AllAttributes[i].Min)
         {
            dbg(LOG_INFO, "Attribute '%s' is smaller than %i", NewAttrName, AllAttributes[i].Min);
            return 1; // error
         }
      }

      // Now assign to the actual fh.attrs.variable

      if(AllAttributes[i].Type == 'w')
         *((uint32_t*)AllAttributes[i].pValue) = (uint32_t)TempValue;
      else if(AllAttributes[i].Type == 'n')
         *((int16_t*)AllAttributes[i].pValue) = (int16_t)TempValue;
      else if(AllAttributes[i].Type == 'b')
         *((char*)AllAttributes[i].pValue) = (char)TempValue;
      else
         *((SIZE_T*)AllAttributes[i].pValue) = (SIZE_T)TempValue;
   }
   else if(AllAttributes[i].Type == 's')
   {
      replace_xml_escape_chars(NewAttrValue, strlen(NewAttrValue));
      if(CopyString(
            AllAttributes[i].pStr,
            NewAttrValue,
            0,
            0,
            strlen(NewAttrValue),
            AllAttributes[i].SizeOfpStr,
            NewAttrValueLen
         ) == 0)
      {
         dbg(LOG_ERROR, "Failed to copy string '%s' into AllAttributes[%" SIZE_T_FORMAT "].pStr", NewAttrValue, i);
         return 1;
      }
   }

   // Here show user what just happened
   // if(fh.attrs.Verbose == TRUE)
   //  dbg (LOG_INFO, "fh_loader Recognized Attribute Number %d ('%s') with
   //  value '%s' type is
   //  '%c'",i,NewAttrName,NewAttrValue,AllAttributes[i].Type);

   // else we have the special 'x' case, handled later if Assigned has been set
   j = i; // for breakpoint
   AllAttributes[i].Assigned = 1;

   return 0;
}

char* FirehoseLoader::RemoveEverythingButTags(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T MaxPacketSize)
{
   SIZE_T PacketLength, localPacketLoc = 0, i, LengthOfXMLHeader = 0;
   char* pch;
   int64_t FoundLeftBrace = -1, FoundRightBrace = -1, Found = 0;
   char CurrentTagName[MAX_TAG_NAME_LENGTH + 1];

   if(CurrentPacketLoc >= MaxPacketSize) return Packet; // nothing to do

   ThisXMLLength = 0;

   if(CurrentPacketLoc > 0) memset(Packet, 0x20, CurrentPacketLoc);

   PacketLength = strlen((const char*)Packet);

   if(CurrentPacketLoc >= PacketLength) return Packet; // nothing to do, we have run out of packet

   if(CharsInBuffer > 0 && Packet == ReadBuffer)
   {
      PacketLength = CharsInBuffer;
      Packet[CharsInBuffer] = '\0';
   }

   if(PacketLength > MaxPacketSize) return Packet; // should be impossible


   // For debugging, if something goes wrong, I need to see the XML I'm looking
   // at
   if(CopyString(temp_buffer2, Packet, 0, 0, PacketLength, FIREHOSE_TX_BUFFER_SIZE, PacketLength) == 0)
   {
      dbg(LOG_ERROR, "Couldn't backup Packet into temp_buffer2 before parsing!");
      ExitAndShowLog(1);
   }

   //-------------------------------------------------------------------------------------------------
   // Step 1 is nuke any comments in the file
   if(RemoveCommentsFromXMLFiles)
   {
      for(i = CurrentPacketLoc; i < PacketLength; i++)
      {
         if(Found == 0)
         {
            if(i + 3 < PacketLength)
            {
               if(Packet[i] == '<' && Packet[i + 1] == '!' && Packet[i + 2] == '-' &&
                  Packet[i + 3] == '-') // protected by if(i+3<PacketLength) above
               {
                  Found = 1;
                  Packet[i] = ' ';
                  Packet[i + 1] = ' ';
                  Packet[i + 2] = ' ';
                  Packet[i + 3] = ' ';
                  i += 3;
               }
            }
         } // end of if(Found==0)
         else
         {
            // we are inside of a comment
            if(i + 2 < PacketLength)
            {
               if(Packet[i] == '-' && Packet[i + 1] == '-' &&
                  Packet[i + 2] == '>') // protected by if(i+2<PacketLength) above
               {
                  Found = 0;
                  Packet[i] = ' ';
                  Packet[i + 1] = ' ';
                  Packet[i + 2] = ' ';
                  i += 2;
                  continue;
               }
            } // end if(i+2<PacketLength)

            Packet[i] = ' '; // remove since this is a comment
         }

         i = i;
      } // end for(i=CurrentPacketLoc;i<PacketLength;i++)
   } // end of if(RemoveCommentsFromXMLFiles)

   // At this point, XML file has all <!-- comments --> replaced by spaces
   //-------------------------------------------------------------------------------------------------


   // Now nuke the first <?xml version="1.0" ?>
   do
   {
      pch = strstr(Packet, "xml version");

      if(pch == NULL)
      {
         break;
      }

      // to be this far we found "xml version"
      // Now look slightly beyond that

      pch = strstr(pch, ">"); // is now pointing to the beginning of the file
                              // "><data><log etc"

      if(pch != nullptr) //'\0')         // pch = "<configuration> <options>
                         // DOWNLOAD_PROTOCOL       = FIREHOSE etc etc
      {
         LengthOfXMLHeader = pch - &Packet[CurrentPacketLoc] + 1;

         if(ShowXMLFileInLog)
         {
            // Show user the packet we have
            if(CopyString(
                  temp_buffer,
                  Packet,
                  0,
                  CurrentPacketLoc,
                  LengthOfXMLHeader,
                  FIREHOSE_TX_BUFFER_SIZE,
                  MaxPacketSize
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy string of length %" SIZE_T_FORMAT " bytes into temp_buffer",
                  pch - &Packet[localPacketLoc] + 1
               );
               ExitAndShowLog(1);
            }
         }

         memset(&Packet[CurrentPacketLoc], 0x20,
                LengthOfXMLHeader); // this removes <?xml version="1.0" ?>
      }

      // We could have more than 1 XML file here
      pch = strstr(Packet, "xml version");

      if(pch != nullptr)
      {
         PacketLength = pch - Packet; // now length updated to start of next XML
      }


      FoundRightBrace = -1;
      FoundLeftBrace = -1;

      // find the tag name, i.e. <data>
      for(i = CurrentPacketLoc; i < PacketLength; i++)
      {
         if(Packet[i] == '<')
         {
            FoundLeftBrace = i;
         }

         if(FoundLeftBrace > 0)
            if(Packet[i] == '>')
            {
               FoundRightBrace = i;
               break;
            }

      } // end for

      if(FoundRightBrace == -1 || FoundLeftBrace == -1)
      {
         dbg(
            LOG_ERROR,
            "Didn't fine either Left Brace or Right Brace. XML "
            "file is not formed correctly"
         );
         ExitAndShowLog(1);
      }

      if((FoundRightBrace - FoundLeftBrace + 2) > MAX_STRING_SIZE - 1)
      {
         dbg(
            LOG_ERROR,
            "Either closing tag is bigger than %d bytes, or the XML file is "
            "not formed correctly",
            MAX_STRING_SIZE
         );
         ExitAndShowLog(1);
      }


      memset(CurrentTagName, 0, sizeof(CurrentTagName));

      if(CopyString(
            CurrentTagName,
            Packet,
            0,
            FoundLeftBrace,
            FoundRightBrace - FoundLeftBrace + 1,
            sizeof(CurrentTagName),
            MaxPacketSize
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy string of length %" SIZE_T_FORMAT " bytes into CurrentTagName",
            FoundRightBrace - FoundLeftBrace + 1
         );
         ExitAndShowLog(1);
      }

      // Here CurrentTagName most likely holds <data>

      CurrentTagName[0] = '/'; // CurrentTagName = "<data>" becomes "/data>" OR
                               // "<configuration>" becomes "/configuration>"

      // Now we are looking for the closing </data>
      pch = strstr(
         Packet,
         CurrentTagName
      ); // is now pointing to the beginning of the
         // file

      if(pch == NULL) //'\0')
      {
         dbg(LOG_ERROR, "Could not find closing tag in file '%s'", CurrentTagName);
         dbg(LOG_ERROR, "Below is what I think I'm looking at, %i bytes", strlen(Packet));
         // dbg (LOG_INFO, "length is %i",strlen(Packet));
         pch = strstr(Packet, "start_sector");
         pch = strstr(Packet, "filename");
         pch = strstr(Packet, "label");
         pch = strstr(Packet, "/data>");

         PRETTYPRINT((uint8_t*)temp_buffer2, FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);

         dbg(LOG_ALWAYS, "%s", temp_buffer2);

         ExitAndShowLog(1);
      }

      // To be this far I know I have at least 1 complete XML file

      ThisXMLLength = (pch - Packet) + strlen(CurrentTagName);

      // CLEAR anything after this to be safe
      if(ThisXMLLength < (SIZE_T)CharsInBuffer)
         memset(&Packet[CharsInBuffer], 0, MaxPacketSize - CharsInBuffer);
      else
         memset(&Packet[ThisXMLLength], 0, MaxPacketSize - ThisXMLLength);


      if(ShowXMLFileInLog)
      {
         // I can print the full packet here, copy it into temp_buffer
         if(CopyString(
               temp_buffer,
               Packet,
               LengthOfXMLHeader,
               CurrentPacketLoc + LengthOfXMLHeader,
               ThisXMLLength - LengthOfXMLHeader,
               FIREHOSE_TX_BUFFER_SIZE,
               MaxPacketSize
            ) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy string of length %" SIZE_T_FORMAT " bytes into temp_buffer",
               PacketLength - CurrentPacketLoc
            );
            ExitAndShowLog(1);
         }

         dbg(
            LOG_DEBUG,
            "XML FILE (%i bytes): CharsInBuffer=%i-%i=%i\n"
            "-----------------------------------------------------------------"
            "--------------------------\n"
            "%s\n"
            "-----------------------------------------------------------------"
            "--------------------------\n",
            strlen(temp_buffer),
            CharsInBuffer,
            strlen(temp_buffer),
            CharsInBuffer - strlen(temp_buffer),
            temp_buffer
         );
      }

      memset(
         &Packet[FoundLeftBrace - 1],
         0x20,
         strlen(CurrentTagName) + 1
      ); // this takes out <configuration> OR
         // <data> at the beginning of the file

      memset(
         &Packet[ThisXMLLength - strlen(CurrentTagName) - 1],
         0x20,
         strlen(CurrentTagName) + 1
      ); // this takes out </configuration> OR
         // </data> at the end of the file


      break; // breaking the do loop

   } while(1); // end of looking for "xml version"

   return Packet;
}

void FirehoseLoader::ResetAllAttributeAssignFlags(void)
{
   SIZE_T i;

   for(i = 0; i < (SIZE_T)ReturnNumAttributes(); i++)
      AllAttributes[i].Assigned = 0;
}

FirehoseLoader::SIZE_T FirehoseLoader::DetermineTag(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T MaxPacketSize)
{
   volatile SIZE_T i = 0;
   SIZE_T PacketLength;
   char* pch;
   int64_t FoundRightBrace = -1;

   if(CurrentPacketLoc >= MaxPacketSize)
   {
      dbg(LOG_ERROR, "CurrentPacketLoc>=MaxPacketSize");
      return 0; // out of packet
   }

   Packet = RemoveEverythingButTags(
      Packet,
      CurrentPacketLoc,
      MaxPacketSize
   ); // When this func is done, only
      // <response etc etc remains
   // Do strlen after RemoveEverythingButTags since the last "\n" can be
   // replaced with a '\0'
   PacketLength = ThisXMLLength;

   if(PacketLength == 0)
   {
      // this means it was a config.xml, i.e. multiple tags don't have the <?xml
      // header, so above calls it length 0
      PacketLength = strlen((const char*)Packet);

      if(PacketLength == 0)
      {
         dbg(
            LOG_ERROR,
            "No XML detected?? Something has gone wrong. Either an XML file "
            "you provided has syntax errors, or your target responded with "
            "garbage. Cannot continue. Log will give more information"
         );
         ExitAndShowLog(1);
      }
   }

   if(CurrentPacketLoc >= PacketLength)
   {
      // we have run out of packet
      return CurrentPacketLoc;
   }

   if(PacketLength > MaxPacketSize)
   {
      dbg(LOG_ERROR, "PacketLength>MaxPacketSize");
      return 0; // should be impossible
   }

   ResetAllAttributeAssignFlags();

   // This chomps up empty space till we hit a tag
   while(IsEmptySpace(Packet[CurrentPacketLoc]))
   {
      CurrentPacketLoc++;

      if(CurrentPacketLoc >= PacketLength)
      {
         dbg(
            LOG_ERROR,
            "XML packet not formed correctly. Ran out of room looking for "
            "TAG"
         );
         return 0;
      }
   }

   if(Packet[CurrentPacketLoc] != '<')
   {
      dbg(LOG_ERROR, "XML not formed correctly. Expected a &lt; character at loc %d", CurrentPacketLoc);
      return 0; // not formed correctly
   }

   CurrentPacketLoc++; // move past the above opening <

   // Could be one solid tag like <options>, test by looking for spaces *before*
   // seeing a '>'
   FoundRightBrace = -1;

   for(i = CurrentPacketLoc; i < PacketLength; i++)
   {
      if(Packet[i] == '>')
      {
         FoundRightBrace = i;
         break;
      }

      if(Packet[i] == ' ')
      {
         break;
      }
   }

   if(FoundRightBrace > 0)
   {
      // this is something like <options> some text </options>
      ParseAttributes = 0; // need to parse the strings

      CopyString(
         NewTagName,
         Packet,
         0,
         CurrentPacketLoc,
         FoundRightBrace - CurrentPacketLoc,
         sizeof(NewTagName),
         MaxPacketSize
      );                             // NewTagName has the TAG now
      RemoveBannedChars(NewTagName); // chars like " < > are replaced by _

      CurrentPacketLoc = FoundRightBrace + 1; // The +1 to get past the >
   }
   else
   {
      // or could be tag with attributes like <program filename
      ParseAttributes = 1;

      pch = strstr(&Packet[CurrentPacketLoc],
                   " "); // Find a space after the TAG name

      if(pch == NULL) //'\0')   // if null, XML is not formed correctly
      {
         dbg(
            LOG_ERROR,
            "1) XML not formed correctly. Expected one SPACE character at loc "
            "%d",
            CurrentPacketLoc
         );
         return 0; // not formed correctly
      }

      if(pch - &Packet[CurrentPacketLoc] > MAX_TAG_NAME_LENGTH)
      {
         dbg(
            LOG_ERROR,
            "The XML TAG is too long (%d chars) only %d chars allowed",
            (pch - &Packet[CurrentPacketLoc]),
            MAX_TAG_NAME_LENGTH
         );
         return 0; // not formed correctly
      }


      CopyString(
         NewTagName,
         Packet,
         0,
         CurrentPacketLoc,
         pch - &Packet[CurrentPacketLoc],
         sizeof(NewTagName),
         MaxPacketSize
      );                             // NewTagName has the TAG now
      RemoveBannedChars(NewTagName); // chars like " < > are replaced by _

      CurrentPacketLoc += strlen((const char*)NewTagName);
      CurrentPacketLoc++; // move past the above opening space we confirmed
                          // exists above
   }

   FirehoseTagDataT::iterator it = firehose_tag_data.find(std::string(NewTagName));
   if(it != firehose_tag_data.end())
   {
      CurrentHandlerFunction = it->second;
   }
   else
   {
      dbg(LOG_ALWAYS, "**INFO Unrecognized tag '%s', so I will simply look for ACK or NAK", NewTagName);
      CurrentHandlerFunction = &FirehoseLoader::handleUnrecognized;
      // return 0;
   }

   // The above got the <TAG>, accessible like this
   // firehose_tag_data[CurrentTag].handler and
   // firehose_tag_data[CurrentTag].tag_name

   return CurrentPacketLoc;

} // end DetermineTag(char *Packet, SIZE_T  PacketLoc)


FirehoseLoader::SIZE_T FirehoseLoader::GetStringFromXML(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T PacketLength)
{
   char* pch;
   // to be here means we have determined it's a bunch of string value pairs
   // DOWNLOAD_PROTOCOL       = FIREHOSE
   // LOAD_RAW_PROGRAM_FILES  = true
   // LOAD_PATCH_PROGRAM_FILES= true
   // </options>

   // MyStringPairs[MAX_STRING_PAIR_SIZE]
   pch = strstr(&Packet[CurrentPacketLoc], "<"); // Find an opening brace


   if(strncasecmp(NewTagName, "file_mappings", strlen("file_mappings")) == 0)
   {
      // This is a huge list of file mappings but not needed for programmer
      CurrentPacketLoc += (pch - &Packet[CurrentPacketLoc]) + 1; // +1 to get past the > symbol
      CurrentPacketLoc += strlen("</file_mappings");             // get past this

      // This chomps up empty space till we hit a tag
      while(IsEmptySpace(Packet[CurrentPacketLoc]))
      {
         CurrentPacketLoc++;

         if(CurrentPacketLoc >= PacketLength)
         {
            // probably ran out of file, this is expected
            break;
         }
      }

      return CurrentPacketLoc;
   }

   if(pch - &Packet[CurrentPacketLoc] > MAX_STRING_PAIR_SIZE)
   {
      dbg(
         LOG_ERROR,
         "Too many string pairs. Size of %d is bigger than %d",
         pch - &Packet[CurrentPacketLoc],
         MAX_STRING_PAIR_SIZE
      );
      ExitAndShowLog(1);
   }

   CopyString(
      MyStringPairs,
      Packet,
      0,
      CurrentPacketLoc,
      pch - &Packet[CurrentPacketLoc],
      MAX_STRING_PAIR_SIZE,
      MAX_STRING_PAIR_SIZE
   ); // NewAttrValue has the ATTR value now

   pch = strstr(&Packet[CurrentPacketLoc], ">"); // Find closing brace

   if(pch - &Packet[CurrentPacketLoc] > MAX_STRING_PAIR_SIZE)
   {
      dbg(
         LOG_ERROR,
         "Too many string pairs. Size of %d is bigger than %d",
         pch - &Packet[CurrentPacketLoc],
         MAX_STRING_PAIR_SIZE
      );
      ExitAndShowLog(1);
   }

   return CurrentPacketLoc + (pch - &Packet[CurrentPacketLoc]) + 1; // +1 to get past the > symbol
}

FirehoseLoader::SIZE_T FirehoseLoader::DetermineAttributes(char* Packet, SIZE_T CurrentPacketLoc, SIZE_T MaxPacketSize)
{
   SIZE_T AttributesFound = 0;
   SIZE_T PacketLength;

   char NewAttrName[MAX_TAG_ATTR_LENGTH];
   char NewAttrValue[MAX_TAG_ATTR_LENGTH];
   char* pch;

   if(CurrentPacketLoc >= MaxPacketSize) return 0; // ran out of packet

   // step 4. Get attributes
   PacketLength = strlen((const char*)Packet);
   // Packet[CurrentPacketLoc] should be at the first letter of the attribute
   // now They all look like this attribute1="value1" attribute2="value2"
   // Meaning only spaces seperate them

   if(PacketLength > MaxPacketSize) return 0; // should be impossible

   AttributesFound = 0;

   while(CurrentPacketLoc < PacketLength)
   {
      pch = strstr(&Packet[CurrentPacketLoc], "="); // Find an equals sign

      if(pch == NULL) //'\0')   // if null, we didn't find it
      {
         dbg(LOG_ERROR, "XML not formed correctly. Could not find '=' character");
         return 0; // not formed correctly
      }

      if(pch - &Packet[CurrentPacketLoc] > MAX_TAG_ATTR_LENGTH)
      {
         dbg(
            LOG_ERROR,
            "The value for XML attribute is too long (%d chars) only %d chars "
            "allowed",
            (pch - &Packet[CurrentPacketLoc]),
            MAX_TAG_ATTR_LENGTH
         );
         return 0; // not formed correctly
      }

      CopyString(
         NewAttrName,
         Packet,
         0,
         CurrentPacketLoc,
         pch - &Packet[CurrentPacketLoc],
         sizeof(NewAttrName),
         MaxPacketSize
      ); // NewAttrName has the ATTR now

      RemoveBannedChars(NewAttrName); // chars like " < > are replaced by _

      TrimTrailingWhiteSpaceFromStringVariable(NewAttrName);

      CurrentPacketLoc += strlen((const char*)NewAttrName);

      CurrentPacketLoc++; // move past the = sign

      if(Packet[CurrentPacketLoc] != '"')
      {
         dbg(
            LOG_ERROR,
            "XML not formed correctly!!\n\n"
            "\nYou probably have *extra* spaces near %s in your XML file\n\n"
            "** You need to manually edit this file and correct this **\n\n\n"
            "NOTE: The XML parsing is limited.\n\nattribute=\"value\"   <--- "
            "This program expects this\nattribute = \"value\" <--- Not this "
            "(Notice the *extra* spaces here)\n\n",
            NewAttrName
         );

         ExitAndShowLog(1); // not formed correctly
      }

      CurrentPacketLoc++; // Before "value" and now we move past the " sign,
                          // all that is left now is the actual value"

      pch = strstr(&Packet[CurrentPacketLoc], "\""); // Find closing "

      if(pch == NULL) //'\0')   // if null, we didn't find it
      {
         dbg(
            LOG_ERROR,
            "XML not formed correctly. Expected one &quot; character at loc "
            "%d",
            CurrentPacketLoc
         );
         return 0; // not formed correctly
      }

      if(pch - &Packet[CurrentPacketLoc] > MAX_TAG_ATTR_LENGTH)
      {
         dbg(
            LOG_ERROR,
            "The value for XML attribute '%s' is too long (%d chars) only %d "
            "chars allowed",
            NewAttrName,
            (pch - &Packet[CurrentPacketLoc]),
            MAX_TAG_ATTR_LENGTH
         );
         return 0; // not formed correctly
      }

      CopyString(
         NewAttrValue,
         Packet,
         0,
         CurrentPacketLoc,
         pch - &Packet[CurrentPacketLoc],
         sizeof(NewAttrValue),
         MaxPacketSize
      );                               // NewAttrValue has the ATTR value now
      RemoveBannedChars(NewAttrValue); // chars like " < > are replaced by _

      AttributesFound++;

      CurrentPacketLoc += strlen((const char*)NewAttrValue);
      CurrentPacketLoc++; // move past the closing quote " sign

      // FIGURE OUT WHICH ATTRIBUTE AND ASSIGN VALUE

      if(AssignAttribute(NewAttrName, NewAttrValue, sizeof(NewAttrValue)) != 0)
      {
         if(strstr(NewAttrName, "num_partition_sectors") != nullptr &&
            strstr(NewAttrValue, "NUM_DISK_SECTORS") != nullptr)
         {
            dbg(
               LOG_INFO,
               "Found NUM_DISK_SECTORS on num_partition_sectors, "
               "only allowed for <erase"
            );
         }
         else
         {
            dbg(LOG_ERROR, "AssignAttribute failed. Could not assign '%s' the value '%s'", NewAttrName, NewAttrValue);
            ExitAndShowLog(1); // not formed correctly
         }
      }

      while(IsEmptySpace(Packet[CurrentPacketLoc]))
      {
         CurrentPacketLoc++;

         if(CurrentPacketLoc >= PacketLength)
         {
            dbg(
               LOG_ERROR,
               "XML packet not formed correctly. Ran out of room "
               "looking for ATTRIBUTES"
            );

            return 0;
         }
      }


      if(Packet[CurrentPacketLoc] == '/')
      {
         // This is the END of the packet
         return CurrentPacketLoc + 2; // +2 gets us past the />
      }
   } // end of while(CurrentPacketLoc < PacketLength)

   return 0; // to be here means we ran out of packet

} // end DetermineAttributes(char *Packet, SIZE_T CurrentPacketLoc)

FirehoseLoader::firehose_error_t FirehoseLoader::handlePower()
{
   dbg(LOG_INFO, "Sending <power>");
   // tx_buffer already holds the XML file
   notifyMessage(FirehoseEvent::EventId::FIREHOSE_RESETTING, tx_buffer);
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(GotACK)
      dbg(LOG_DEBUG, "Got the ACK");
   else
      return FIREHOSE_ERROR;

   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleSetBootableStorageDrive()
{
   dbg(LOG_INFO, "Sending <setbootablestoragedrive>");
   // tx_buffer already holds the XML file
   notifyMessage(FirehoseEvent::EventId::FIREHOSE_SELECT_PARTITION, tx_buffer);
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(GotACK)
      dbg(LOG_DEBUG, "Got the ACK");
   else
      return FIREHOSE_ERROR;

   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleConfigure()
{
   char* pch;
   SIZE_T OrigMaxPayloadSizeToTargetInBytes = fh.attrs.MaxPayloadSizeToTargetInBytes;
   // NOTE: This packet can be sent more than once until negotion is done

   memset(last_log_value, 0x0, strlen(last_log_value));

   do
   {
      dbg(LOG_INFO, "Sending <configure>");

      // tx_buffer already holds the XML file
      sendTransmitBuffer();

      if(Simulate)
      {
         InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);

         // ReadBuffer[strlen((const char *)ReadBuffer)-1]='\0';  // HACK to
         // remove \n above for testing

         AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);

         // AppendToBuffer(ReadBuffer,"<data>",MAX_READ_BUFFER_SIZE); // HACK TO
         // TEST
         if(fh.attrs.MaxPayloadSizeToTargetInBytes > fh.attrs.MaxPayloadSizeToTargetInBytesSupported)
         {
            AppendToBuffer(ReadBuffer, "<response value=\"NAK\" ", MAX_READ_BUFFER_SIZE);
            sprintf(
               temp_buffer,
               "MaxPayloadSizeToTargetInBytes=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.MaxPayloadSizeToTargetInBytesSupported
            );
            AppendToBuffer(ReadBuffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
            sprintf(
               temp_buffer,
               "MaxPayloadSizeToTargetInBytesSupported=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.MaxPayloadSizeToTargetInBytesSupported
            );
            AppendToBuffer(ReadBuffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
         }
         else
         {
            AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
            sprintf(
               temp_buffer,
               "MaxPayloadSizeToTargetInBytes=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.MaxPayloadSizeToTargetInBytes
            );
            AppendToBuffer(ReadBuffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
            sprintf(
               temp_buffer,
               "MaxPayloadSizeToTargetInBytesSupported=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.MaxPayloadSizeToTargetInBytesSupported
            );
            AppendToBuffer(ReadBuffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
         }

         AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
         // AppendToBuffer(ReadBuffer,"/></data>",MAX_READ_BUFFER_SIZE);  //
         // HACK to test targets that don't put \n at the end
         CharsInBuffer = strlen(ReadBuffer);
         // sprintf(temp_buffer,"value=\"%s\" ",fh.attrs.value);
         // AppendToBuffer(ReadBuffer,temp_buffer);
      }

      GetNextPacket(); // this will set all variables, including GotACK

      // Target can ACK or NAK my configure packet. As it is setup now, all that
      // changes is that Target will have *set* variables on PC side That is
      // fh_loader.exe will have it's MaxPayloadSizeToTargetInBytes *set* to
      // whatever target has

      dbg(LOG_INFO, "fh.attrs.MaxPayloadSizeToTargetInBytes = %" SIZE_T_FORMAT, fh.attrs.MaxPayloadSizeToTargetInBytes);
      dbg(
         LOG_INFO,
         "fh.attrs.MaxPayloadSizeToTargetInBytesSupported = %" SIZE_T_FORMAT,
         fh.attrs.MaxPayloadSizeToTargetInBytesSupported
      );

      MaxBytesToReadFromUSB = fh.attrs.MaxPayloadSizeFromTargetInBytes;


      // last_log_value
      if(!GotACK)
      {
         // to be here means a NAK came. Target is rejecting something
         if(UsingValidation)
         {
            dbg(
               LOG_ERROR,
               "Got a NAK. Target *did not* like your <configure>. This is "
               "most likely due to one of your parameters (attributes) not "
               "matching the target!"
               "\n\nFor example, this can happen if you have specified in "
               "<configure> MaxPayloadSizeToTargetInBytes=\"16384\"\nbut "
               "target has specified in <response> "
               "MaxPayloadSizeToTargetInBytes=\"8192\" "
               "\n\nNOTE: For VIP your <configure> parameters must match the "
               "targets <response>. Check the LOG and see the targets "
               "<response>. You manually need to check the LOG"
               "\n\nThus for example, you need to change your <configure> "
               "MaxPayloadSizeToTargetInBytes=\"8192\", by doing "
               "--maxpayloadsizeinbytes=8192"
               "\n\nOR, you must regenerate your digests tables, i.e. redo "
               "the 'dry run' with --createdigests"
               "\n\nReset your target to continue"
               "\nReset your target to continue"
               "\nReset your target to continue"
               "\n\nNOTE: This is *not* a VIP error nor is it a hash "
               "mismatch. This is your parameters don't match the targets, so "
               "we can't continue\n\n"
            );

            ExitAndShowLog(1);
         }

         // to be this far it's not VIP

         if(AllAttributes[1].Assigned == 0)
         {
            // if target didn't assign any variables then this is an indication
            // that it failed on some storage call and thus didn't ReadBuffer is
            // like <response value="NAK" />   i.e. it didn't make it far enough
            // to fill in attrs
            dbg(
               LOG_INFO,
               "Something failed. The target rejected your <configure>. "
               "Please inspect log for more information"
            );
            ExitAndShowLog(1); // these match so this must be a true failure,
                               // so quit
         }

         pch = strstr(last_log_value, "ERROR");

         // to be this far we have a NAK and not using VIP, so maybe we can
         // recover
         if(pch != nullptr)
         {
            dbg(
               LOG_INFO,
               "Something failed with <configure>. This is usually related to "
               "STORAGE '%s' *not* coming up",
               fh.attrs.MemoryName
            );
            ExitAndShowLog(1); // these match so this must be a true failure,
                               // so quit
         }
         else
         {
            if(BytesRead == 0)
            {
               dbg(LOG_ERROR, "Your target is NOT responding!!");
               ExitAndShowLog(1);
            }

            dbg(
               LOG_INFO,
               "Target returned NAK for your <configure> but it does not seem "
               "to be an error. This is ok, fh_loader.exe attributes updated"
            );
         }
      }


      if(GotACK) dbg(LOG_DEBUG, "Got the ACK for the <configure>");

      if(OrigMaxPayloadSizeToTargetInBytes != fh.attrs.MaxPayloadSizeToTargetInBytes)
      {
         if(UsingValidation)
         {
            dbg(
               LOG_ERROR,
               "You are using VIP and target wants "
               "MaxPayloadSizeToTargetInBytes=%" SIZE_T_FORMAT " and you have --maxpayloadsizeinbytes=%" SIZE_T_FORMAT,
               fh.attrs.MaxPayloadSizeToTargetInBytes,
               OrigMaxPayloadSizeToTargetInBytes
            );
            ExitAndShowLog(1); // these match so this must be a true failure,
                               // so quit
         }
      }

      if(CreateDigests) break; // do not negotiate during the dry run

      if(UsingValidation)
         break; // do not negotiate when using VIP, since the "dry run" would
                // have been made with this value

      // NOTE: above I checked that host and target agree


      // To be this far it is not a "dry run" and we are not using VIP
      // Therefore it is possible the target can handle a bigger payload size

      if(fh.attrs.MaxPayloadSizeToTargetInBytes == fh.attrs.MaxPayloadSizeToTargetInBytesSupported)
         break; // we are done, target does not offer a bigger payload size


      // to be this far means we don't agree on payload size yet
      fh.attrs.MaxPayloadSizeToTargetInBytes = fh.attrs.MaxPayloadSizeToTargetInBytesSupported;
      LoadConfigureIntoTXBuffer();

   } while(1); // end do

   return FIREHOSE_SUCCESS;
}

void FirehoseLoader::trimSubStr(std::string& srcStr, const std::string& eraseStr)
{
   std::size_t fndIdx = srcStr.find(eraseStr);

   while(std::string::npos != fndIdx)
   {
      srcStr.erase(fndIdx, eraseStr.length());
      fndIdx = srcStr.find(eraseStr);
   }
}

// Edma address infomations string from device looks like this:
//<?xml version="1.0" encoding="UTF-8" ?>
//<data>
//<log value="Using address 0x200100"/>
//</data>
// Create a binary file parsing the address as file name
void FirehoseLoader::parseEdmaAddressInfo(const std::string& inpEdmaAddressInfoStr)
{
   std::string edmaAddressInfoStr = inpEdmaAddressInfoStr.substr(strlen(FIREHOSE_PREFIX_EDMA_ADDRESS_LOG_STR));

   // create a new file
   m_edmaFile.clear();
   m_edmaFile.append(MainOutputDir);
   m_edmaFile.append(edmaAddressInfoStr.c_str());

   // Append extension name .bin to PBL dump file
   // PBL dump parser only support.bin file. So the users need to add .bin
   // manually such as:0x14800000.bin
   m_edmaFile.append("_QIL.bin");
   std::shared_ptr<std::fstream> fe = ReturnFileHandle(
      (char*)m_edmaFile.c_str(),
      MAX_PATH_SIZE,
      "wb"
   ); // will exit if not successful
   if(fe != nullptr)
   {
      fh_fclose(fe);
      fe = nullptr;
   }
}

// Edma data infomations string from device is pure hex buffer looks like this:
//<?xml version="1.0" encoding="UTF-8" ?>
//<data>
//<log value = "0x01 0x07 0x00 0x04 0x00 0x00 0x60 0x00 0x01 0x07 0x20 0x04 0x00
// 0x00 0x60 0x00 " / >
//</data>
// Convert the string into binary data and write into the file
void FirehoseLoader::parseEdmaDataInfo(const std::string& inpEdmaDataInfoStr)
{
   std::shared_ptr<std::fstream> fe;
   if(!m_edmaFile.empty())
   {
      fe = ReturnFileHandle((char*)m_edmaFile.c_str(), MAX_PATH_SIZE,
                            "ab"); // will exit if not successful
   }
   if(fe != nullptr)
   {
      std::string edmaDataInfoStr = inpEdmaDataInfoStr;
      std::string subStr;
      for(size_t i = 0; i < edmaDataInfoStr.length(); i++)
      {
         if(' ' == edmaDataInfoStr[i])
         {
            if(!subStr.empty())
            {
               if(4 == subStr.length()) // hex buff
               {
                  fe->write(subStr.c_str(), 1);
               }
               subStr.clear();
            }
         }
         else
         {
            subStr.push_back(edmaDataInfoStr[i]);
         }
      }
      fh_fclose(fe);
      fe = nullptr;
   }
}

// Storage Info string from device looks like this:
// INFO: {"storage_info": {"total_blocks":29595648, "block_size":4096,
// "page_size":4096,
//  "num_physical":6, "manufacturer_id":462, "serial_num":1297306958,
//  "fw_version":"400", "mem_type":"UFS","prod_name":"KLUDG4UHDB-B2D1"}}

void FirehoseLoader::parseStorageInfo(const std::string& inpStorageInfoStr)
{
   int32_t posStart = static_cast<int32_t>(inpStorageInfoStr.find("total_blocks\""));
   std::string strStorageInfo;
   if(std::string::npos != static_cast<size_t>(posStart))
   {
      strStorageInfo = inpStorageInfoStr.substr(posStart);
      trimSubStr(strStorageInfo, ".");
      trimSubStr(strStorageInfo, "\"");
      trimSubStr(strStorageInfo, "}");
      m_storageInfo.isSet = true;
   }
   else
   {
      dbg(LOG_INFO, "storage_info String format changed. Need to revisit this");
      m_storageInfo.isSet = false;
      return;
   }

   char *savePtr1, *savePtr2;
   char *prmsComma, *prmsColon, *val;
   std::map<const char*, std::string&> storageVarMap;
   std::map<const char*, std::string&>::iterator it;
   char inpCStr[MAX_STRING_SIZE];


   snprintf_x(inpCStr, sizeof(inpCStr), "%s", strStorageInfo.c_str());

   storageVarMap.insert(std::pair<const char*, std::string&>("total_blocks", m_storageInfo.totalBlocks));
   storageVarMap.insert(std::pair<const char*, std::string&>("block_size", m_storageInfo.blockSize));
   storageVarMap.insert(std::pair<const char*, std::string&>("page_size", m_storageInfo.pageSize));
   storageVarMap.insert(std::pair<const char*, std::string&>("num_physical", m_storageInfo.numPhysicalPartitions));
   storageVarMap.insert(std::pair<const char*, std::string&>("manufacturer_id", m_storageInfo.manufacturerId));
   // Legacy devices send mfr_id as string instead of manufacturer_id
   storageVarMap.insert(std::pair<const char*, std::string&>("mfr_id", m_storageInfo.manufacturerId));
   storageVarMap.insert(std::pair<const char*, std::string&>("serial_num", m_storageInfo.serialNum));
   storageVarMap.insert(std::pair<const char*, std::string&>("fw_version", m_storageInfo.fwVersion));
   storageVarMap.insert(std::pair<const char*, std::string&>("mem_type", m_storageInfo.memType));
   storageVarMap.insert(std::pair<const char*, std::string&>("prod_name", m_storageInfo.prodName));
   storageVarMap.insert(std::pair<const char*, std::string&>("spec_version", m_storageInfo.specVersion));

   // Tokenize
   prmsComma = strtok_x(inpCStr, ",", &savePtr1);

   while(prmsComma != nullptr)
   {
      it = storageVarMap.begin();
      while(it != storageVarMap.end())
      {
         if(strstr(prmsComma, (it->first)) != nullptr)
         {
            prmsColon = strtok_x(prmsComma, ":", &savePtr2);
            val = strtok_x(NULL, ":", &savePtr2);

            if(val != nullptr)
            {
               it->second = val;
            }

            storageVarMap.erase(it);
            break;
         }
         ++it;
      }
      prmsComma = strtok_x(NULL, ",", &savePtr1);
   }
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleLog()
{
   // dbg (LOG_INFO, "\n");
   dbg(LOG_INFO, "TARGET SAID: '%s'", fh.attrs.value);
   notifyMessage(FirehoseEvent::EventId::FIREHOSE_TARGET_SAID, fh.attrs.value);
   CopyString(
      last_log_value,
      fh.attrs.value,
      0,
      0,
      strlen(fh.attrs.value),
      FIREHOSE_TX_BUFFER_SIZE,
      FIREHOSE_TX_BUFFER_SIZE
   );

   // Additional parsing logic for storage info not present in core
   // firehoseloader code.
   char* storageStr = strstr(fh.attrs.value, FIREHOSE_PREFIX_STORAGE_LOG_STR);
   if(storageStr != nullptr)
   {
      std::string storageInfoStr(fh.attrs.value);
      parseStorageInfo(storageInfoStr);
   }

   // Additional parsing logic for edma info not present in core firehoseloader
   // code.
   char* edmaStr = strstr(fh.attrs.value, FIREHOSE_PREFIX_EDMA_ADDRESS_LOG_STR);
   if(edmaStr == fh.attrs.value)
   {
      std::string edmaAddressStr(fh.attrs.value);
      parseEdmaAddressInfo(edmaAddressStr);
   }
   edmaStr = strstr(fh.attrs.value, FIREHOSE_PREFIX_EDMA_DATA_LOG_STR);
   if(edmaStr == fh.attrs.value)
   {
      std::string edmaDataStr(fh.attrs.value);
      parseEdmaDataInfo(edmaDataStr);
   }

   // Extract Error from Device to be passed to the client
   if(m_errorFromDevice.empty())
   {
      char* pch = strstr(fh.attrs.value, FIREHOSE_DEVICE_ERROR_STR);
      if(pch != nullptr)
      {
         m_errorFromDevice = fh.attrs.value;
      }
   }

   if(m_interalErrorString.empty())
   {
      if(!GotACK)
      {
         m_interalErrorString = fh.attrs.value;
      }
   }
   if(UsingValidation)
   {
      char* pch;

      pch = strstr(last_log_value, "PACKET_HASH_MISMATCH");

      if(pch != nullptr)
      {
         dbg(
            LOG_ERROR,
            "The SHA256 digest of the last packet sent by HOST PC *does not* "
            "match what target expected"
            "\n\nThis means either"
            "\n\t1. You sent the wrong signed digest table, i.e. "
            "--signeddigests=DigestsToSign.bin.mbn"
            "\n\t2. You have changed for example, a parameter such as "
            "--maxpayloadsizeinbytes=16384 whereas when you did the \"dry "
            "run\" it was --maxpayloadsizeinbytes=8192 "
            "\n\t3. Possibly one of the files have changed since when you ran "
            "the \"dry run\". If you saved the original "
            "\"command_trace.txt\", this can help in debugging"
            "\n\nThere is no recovery from this. You need to reset your "
            "target OR send a different signed digest table "
            "--signeddigests=DigestsToSign2.bin.mbn\n"
         );
         ExitAndShowLog(1);
      }
   }

   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleSearchPaths()
{
   SIZE_T localPacketLoc = 0, PacketLength, i = 0;
   SIZE_T j, k;
   char c = 0;

   // Strings in MyStringPairs will be like
   //  \\snowcone\builds672\TEST\M8994AAAAANLGD1000178.1\common\build\emmc
   //  \\snowcone\builds672\TEST\M8994AAAAANLGD1000178.1\common\build\emmc\bin\asic\

   PacketLength = strlen(MyStringPairs);

   while(IsEmptySpace(MyStringPairs[localPacketLoc]))
   {
      localPacketLoc++;

      if(localPacketLoc >= PacketLength) break; // out of packet, so get out of here
   }

   // I can use this function below, as long as the newlines are replaced by
   // commas num_search_paths =
   // SplitStringOnCommas(MyArg,search_path,num_search_paths);

   while(localPacketLoc < PacketLength)
   {
      if(MyStringPairs[localPacketLoc] == 0xA) MyStringPairs[localPacketLoc] = ',';

      localPacketLoc++;
   }

   // MyStringPairs into search_path[MAX_XML_FILES][MAX_PATH_SIZE]
   num_search_paths = SplitStringOnCommas(
      MyStringPairs,
      MAX_STRING_PAIR_SIZE,
      search_path,
      static_cast<int32_t>(num_search_paths),
      MAX_XML_FILES,
      MAX_PATH_SIZE
   );

   search_path[num_search_paths][0] = '\0'; // j is how many strings were valid, make sure last one in table is
                                            // null

   for(j = 0; j < num_search_paths; j++)
   {
      for(k = 0; (uint32_t)k < strlen(search_path[j]); k++)
      {
         // find a slash
         c = search_path[j][k];

         if(c == FORWARDSLASH || c == BACKSLASH) break;
      }

      if(c != FORWARDSLASH && c != BACKSLASH) c = SLASH;


      k = strlen(search_path[j]);

      // Now does it end in a slash
      if(search_path[j][k - 1] != c)
      {
         search_path[j][k] = c;
         search_path[j][k + 1] = '\0';
      }
   }

   for(i = 0; i < num_search_paths; i++)
   {
      j = strlen(search_path[i]);
   }

   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleRawProgram()
{
   SIZE_T localPacketLoc = 0, PacketLength;

   // Strings in MyStringPairs will
   //    rawprogram_unsparse.xml
   //    rawprogram2.xml

   PacketLength = strlen(MyStringPairs);

   while(IsEmptySpace(MyStringPairs[localPacketLoc]))
   {
      localPacketLoc++;

      if(localPacketLoc >= PacketLength) break; // out of packet, so get out of here
   }

   // I can use this function below, as long as the newlines are replaced by
   // commas num_search_paths =
   // SplitStringOnCommas(MyArg,search_path,num_search_paths);

   while(localPacketLoc < PacketLength)
   {
      if(MyStringPairs[localPacketLoc] == 0xA) MyStringPairs[localPacketLoc] = ',';

      localPacketLoc++;
   }

   if(LOAD_RAW_PROGRAM_FILES)
   {
      // MyStringPairs into XMLFileTable[MAX_XML_FILES][MAX_PATH_SIZE]
      num_xml_files_to_send = SplitStringOnCommas(
         MyStringPairs,
         MAX_STRING_PAIR_SIZE,
         XMLFileTable,
         static_cast<int32_t>(num_xml_files_to_send),
         MAX_XML_FILES,
         MAX_PATH_SIZE
      );

      XMLFileTable[num_xml_files_to_send][0] = '\0'; // j is how many strings were valid, make sure last one in table
                                                     // is null
   }

   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handlePatchProgram() // used for <patch>
{
   SIZE_T localPacketLoc = 0, PacketLength, i = 0;

   if(ParseAttributes == 0)
   {
      // this is like <patch>patch0.xml</patch>, i.e. when user has
      // --xml=firehose_config.xml

      // Strings in MyStringPairs will
      //    rawprogram_unsparse.xml
      //    rawprogram2.xml

      PacketLength = strlen(MyStringPairs);

      while(IsEmptySpace(MyStringPairs[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= PacketLength) break; // out of packet, so get out of here
      }

      // I can use this function below, as long as the newlines are replaced by
      // commas num_search_paths =
      // SplitStringOnCommas(MyArg,search_path,num_search_paths);

      while(localPacketLoc < PacketLength)
      {
         if(MyStringPairs[localPacketLoc] == 0xA) MyStringPairs[localPacketLoc] = ',';

         localPacketLoc++;
      }

      if(LOAD_PATCH_PROGRAM_FILES)
      {
         // MyStringPairs into XMLFileTable[MAX_XML_FILES][MAX_PATH_SIZE]
         num_xml_files_to_send = SplitStringOnCommas(
            MyStringPairs,
            MAX_STRING_PAIR_SIZE,
            XMLFileTable,
            static_cast<int32_t>(num_xml_files_to_send),
            MAX_XML_FILES,
            MAX_PATH_SIZE
         );

         XMLFileTable[num_xml_files_to_send][0] = '\0'; // j is how many strings were valid, make sure last one in
                                                        // table is null
      }

      return FIREHOSE_SUCCESS;
   }
   else
   {
      for(i = 0; i < static_cast<SIZE_T>(strlen(fh.attrs.filename)); i++)
      {
         if(IsEmptySpace(fh.attrs.filename[i])) fh.attrs.filename[i] = '\0';
      }

      if(strlen(fh.attrs.filename) == 0) return FIREHOSE_SUCCESS;

      if(strncmp(fh.attrs.filename, "DISK", MAX(strlen(fh.attrs.filename), strlen("DISK"))) != 0)
         return FIREHOSE_SUCCESS; // **unless it is filename="DISK", skip it

      if(!LOAD_PATCH_PROGRAM_FILES) return FIREHOSE_SUCCESS; // **user doesn't want to patch

      // this is like <patch SECTOR_SIZE_IN_BYTES="512" etc
      if(fh.attrs.SECTOR_SIZE_IN_BYTES != SectorSizeInBytes)
      {
         SectorSizeInBytes = fh.attrs.SECTOR_SIZE_IN_BYTES;
         dbg(LOG_INFO, "SectorSizeInBytes changed to %d", SectorSizeInBytes);
      }

      /*
          InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
          //AppendToBuffer(tx_buffer,"<data>\n");
          AppendToBuffer(tx_buffer,"<patches>\n",FIREHOSE_TX_BUFFER_SIZE);
          AppendToBuffer(tx_buffer,"<patch ",FIREHOSE_TX_BUFFER_SIZE);

          sprintf(temp_buffer,"SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\"
         ",fh.attrs.SECTOR_SIZE_IN_BYTES);
          AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

          sprintf(temp_buffer,"byte_offset=\"%" SIZE_T_FORMAT "\"
         ",fh.attrs.byte_offset);
          AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

          sprintf(temp_buffer,"filename=\"DISK\" ");
          AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

          sprintf(temp_buffer,"physical_partition_number=\"%" SIZE_T_FORMAT "\"
         ",fh.attrs.physical_partition_number);
          AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

          sprintf(temp_buffer,"size_in_bytes=\"%" SIZE_T_FORMAT "\"
         ",fh.attrs.size_in_bytes);
          AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

          sprintf(temp_buffer,"start_sector=\"%s\" ",fh.attrs.start_sector);
          AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

          sprintf(temp_buffer,"value=\"%s\" ",fh.attrs.value);
          AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

          AppendToBuffer(tx_buffer,"/>\n</patches>",FIREHOSE_TX_BUFFER_SIZE);
      */

      // tx_buffer already holds the XML file
      notifyMessage(FirehoseEvent::EventId::FIREHOSE_TRANSFER_PATCH_PROGRAMMER, tx_buffer);
      sendTransmitBuffer();


      if(Simulate)
      {
         InitBufferWithXMLHeader(&ReadBuffer[localPacketLoc], MAX_READ_BUFFER_SIZE - localPacketLoc);
         AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
         AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
         AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
         CharsInBuffer = strlen(ReadBuffer);
      }


      GetNextPacket(); // this will set all variables, including GotACK

      if(!GotACK)
      {
         dbg(LOG_ERROR, "Something failed during patching. Please see port trace");
         ExitAndShowLog(1);
         // return FIREHOSE_ERROR;
      }


      return FIREHOSE_SUCCESS;
   }
}


FirehoseLoader::firehose_error_t FirehoseLoader::handleFileMappings()
{
   // does nothing, just ignore this
   return FIREHOSE_SUCCESS;
}

void FirehoseLoader::TrimTrailingWhiteSpaceFromStringVariable(char* sz)
{
   SIZE_T i = 0;

   for(i = strlen(sz); i > 0; i--)
   {
      if(sz[i] == 0x9 || sz[i] == 0x20) // tab or space
         sz[i] = '\0';                  // put a null there
   }
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleOptions()
{
   char Key[MAX_STRING_SIZE] = {0}, Value[MAX_STRING_SIZE] = {0};
   SIZE_T localPacketLoc = 0, PacketLength, i = 0, OnKey = 1, PairFound = 0;

   PacketLength = strlen(MyStringPairs);

   while(localPacketLoc < PacketLength)
   {
      // Nuke any whitespace
      while(IsEmptySpace(MyStringPairs[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= PacketLength) continue; // out of packet, so get out of here
      }

      if(OnKey)
      {
         if(i >= MAX_STRING_SIZE)
         {
            dbg(LOG_ERROR, "The key value pair in your <options> section is too big");
            ExitAndShowLog(1);
         }

         Key[i++] = MyStringPairs[localPacketLoc];

         if(MyStringPairs[localPacketLoc] == '=')
         {
            OnKey = 0;
            Key[i - 1] = '\0';
            i = 0;
         }
      }
      else
      {
         if(i >= MAX_STRING_SIZE)
         {
            dbg(
               LOG_ERROR,
               "The key value pair in your <options> section is "
               "too big for Value"
            );
            ExitAndShowLog(1);
         }

         Value[i++] = MyStringPairs[localPacketLoc];

         if(MyStringPairs[localPacketLoc + 1] == 0xA || MyStringPairs[localPacketLoc + 1] == 0xD)
         {
            OnKey = 1;
            Value[i] = '\0';
            i = 0;
            PairFound = 1;
         }
      } // end OnKey

      localPacketLoc++;

      if(PairFound)
      {
         // to be here I have a Key and Value
         /*
               if(
            strncmp(Key,"DOWNLOAD_PROTOCOL",MAX(strlen(Key),strlen("DOWNLOAD_PROTOCOL")))==0
            )
               {
                 // Protocol must be FIREHOSE
                 if( strncmp(Value,"FIREHOSE",strlen(Value))!=0 )
                 {
                    dbg (LOG_ERROR, "You have specified a protocol *other* than
            FIREHOSE. This tool only speaks FIREHOSE"); ExitAndShowLog(1);
                 }
               } // end of DOWNLOAD_PROTOCOL
         */
         if(strncmp(Key, "LOAD_RAW_PROGRAM_FILES", MAX(strlen(Key), strlen("LOAD_RAW_PROGRAM_FILES"))) == 0)
         {
            // Protocol must be FIREHOSE
            if(strncmp(Value, "false", strlen(Value)) == 0)
            {
               dbg(LOG_INFO, "You have specified to NOT load rawprogram files");
               LOAD_RAW_PROGRAM_FILES = 0;
            }
         } // end of LOAD_RAW_PROGRAM_FILES

         if(strncmp(Key, "LOAD_PATCH_PROGRAM_FILES", MAX(strlen(Key), strlen("LOAD_PATCH_PROGRAM_FILES"))) == 0)
         {
            // Protocol must be FIREHOSE
            if(strncmp(Value, "false", strlen(Value)) == 0)
            {
               dbg(LOG_INFO, "You have specified to NOT load patch files");
               LOAD_PATCH_PROGRAM_FILES = 0;
            }
         } // end of LOAD_PATCH_PROGRAM_FILES

         PairFound = 0;
      } // end of if(PairFound)

   } // end of while(localPacketLoc<PacketLength)


   dbg(LOG_INFO, "Parsed XML Options successfully");
   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleNop()
{
   // tx_buffer already holds the XML file
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(!GotACK) return FIREHOSE_ERROR;

   dbg(LOG_INFO, "Got the PING response");

   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleErase()
{
   dbg(LOG_INFO, "                        Issuing Erase                       ");
   dbg(LOG_INFO, " _____              _               _____                   ");
   dbg(LOG_INFO, "|_   _|            (_)             |  ___|                  ");
   dbg(LOG_INFO, "  | | ___ ___ _   _ _ _ __   __ _  | |__ _ __ __ _ ___  ___ ");
   dbg(LOG_INFO, "  | |/ __/ __| | | | | '_ \\ / _` | |  __| '__/ _` / __|/ _ \\");
   dbg(LOG_INFO, " _| |\\__ \\__ \\ |_| | | | | | (_| | | |__| | | (_| \\__ \\  __/");
   dbg(LOG_INFO, " \\___/___/___/\\__,_|_|_| |_|\\__, | \\____/_|  \\__,_|___/\\___|");
   dbg(LOG_INFO, "                             __/ |                          ");
   dbg(LOG_INFO, "                            |___/ \n\n");

   notifyMessage(FirehoseEvent::EventId::FIREHOSE_ERASE, tx_buffer);
   // tx_buffer already holds the XML file
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(!GotACK)
   {
      // return FIREHOSE_ERROR;
      dbg(
         LOG_ERROR,
         "Something failed with the <erase> command. The port_trace.txt can "
         "shed more information on this"
         "\nNote that for eMMC <erase> is only valid for physical_partition "
         "0. For UFS <erase> is valid for all LUNs\n\n"
      );
      ExitAndShowLog(1);
   }

   dbg(LOG_INFO, "\n\n");
   dbg(LOG_INFO, "ERASE SUCCESSFUL");
   dbg(LOG_INFO, "ERASE SUCCESSFUL");
   dbg(LOG_INFO, "ERASE SUCCESSFUL\n\n");

   return FIREHOSE_SUCCESS;
}


FirehoseLoader::firehose_error_t FirehoseLoader::handleUnrecognized()
{
   /*
     SIZE_T i;

     InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
       AppendToBuffer(tx_buffer,"<data>\n",FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"<%s ",NewTagName);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

     for(i=0;i<(SIZE_T)ReturnNumAttributes();i++)
     {
           if(AllAttributes[i].Assigned==0)
               continue; // variable wasn't even assigned, so no point checking

       sprintf(temp_buffer,"%s=\"%s\"
     ",AllAttributes[i].Name,AllAttributes[i].Raw);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);
     }

       AppendToBuffer(tx_buffer,"/>\n</data>",FIREHOSE_TX_BUFFER_SIZE);
   */

   // tx_buffer already holds the XML file
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(GotACK)
   {
      dbg(LOG_INFO, "Response was success in running to unrecognized TAG (%s)", NewTagName);
      return FIREHOSE_SUCCESS;
   }
   else
   {
      dbg(LOG_ERROR, "Error in handling to unrecognized TAG (%s)", NewTagName);
      return FIREHOSE_ERROR;
   }
}


// This function goes until a packet is found that is not a log
// so it chomps through until a <tag> is found that is not <log>, such as
// <response>

FirehoseLoader::firehose_error_t FirehoseLoader::GetNextPacket(void)
{
   int32_t i, TimeToBreak = 0, NeedToReadFromChannel = 0;
   int32_t localNumTries = 0, MaxTries = 10000;
   SIZE_T PacketLength;

   // PacketLoc = 0;

   // dbg (LOG_DEBUG,"In GetNextPacket with CharsInBuffer = %ld",CharsInBuffer);

   BytesRead = 0; // reset here

   while(TimeToBreak == 0)
   {
      // Aaron, need to always have a /data> in the buffer, or else
      //        I need to read more to get a complete packet

      if(RawMode && !Simulate && (SIZE_T)CharsInBuffer < MaxBytesToReadFromUSB) NeedToReadFromChannel = 1;

      // if(PacketLoc==0)
      //   NeedToReadFromChannel = 1;
      if(CharsInBuffer <= (XML_HEADER_LENGTH + 2 * XML_TAIL_LENGTH)) NeedToReadFromChannel = 1;


      if(NeedToReadFromChannel == 0 && RawMode == 0)
      {
         // make sure there is at least an end /data> in the buffer
         char* pch = nullptr;

         pch = strstr((char*)&ReadBuffer[PacketLoc], "</data>");

         if(pch == NULL)
            NeedToReadFromChannel = 1;
         else if((char*)pch - (char*)&ReadBuffer[PacketLoc] < 10 &&
                 strlen(pch) <= (XML_HEADER_LENGTH + 2 * XML_TAIL_LENGTH))
            NeedToReadFromChannel = 1;
         else
         {
            // Show what target sent
            CopyString(
               temp_buffer,
               ReadBuffer,
               0,
               PacketLoc,
               pch - &ReadBuffer[PacketLoc] + strlen("</data>"),
               FIREHOSE_TX_BUFFER_SIZE,
               MAX_READ_BUFFER_SIZE
            );
            // dbg (LOG_DEBUG,"\n%s", temp_buffer);
         }
      }

      // dbg (LOG_INFO, "\nNeedToReadFromChannel==%i",NeedToReadFromChannel);

      if(CharsInBuffer > 0 && PacketLoc > 0)
      {
         // Move the data back to the start of buffer
         for(i = 0; i < CharsInBuffer; i++)
            ReadBuffer[i] = ReadBuffer[i + PacketLoc]; // AARON - NO BOUNDS CHECK ON
                                                       // PacketLoc here!!!

         PacketLoc = 0; // all data moved back to the start
         memset(&ReadBuffer[CharsInBuffer], 0x0, MAX_READ_BUFFER_SIZE - CharsInBuffer);
      }


      if(NeedToReadFromChannel)
      {
         // dbg (LOG_INFO, "\nNeedToReadFromChannel==1");

         NeedToReadFromChannel = 0;


         if(localNumTries >= MaxTries)
         {
            dbg(LOG_ERROR, "Nothing read from target for %i tries\n", MaxTries);
            ExitAndShowLog(1);
         }

         if(PacketLoc == 0) memset(&ReadBuffer[CharsInBuffer], 0x0, MAX_READ_BUFFER_SIZE - CharsInBuffer);

         // dbg (LOG_INFO, "\nAbout to call ReadPort");

         BytesRead = ReadPort(
            (uint8_t*)&ReadBuffer[CharsInBuffer],
            static_cast<uint32_t>(MaxBytesToReadFromUSB - CharsInBuffer),
            MAX_READ_BUFFER_SIZE
         ); // null doesn't matter in RAW mode

         localNumTries++;

         // dbg (LOG_INFO,"Read %i bytes from
         // port,NumTries=%d",BytesRead,localNumTries);

         CharsInBuffer += BytesRead;

         dbg(LOG_DEBUG, "CharsInBuffer = %ld", CharsInBuffer);

         // BytesReadPlusLeftOver += BytesRead;
         if(CharsInBuffer > 0)
         {
            // We have data, we can break
            // if(!RawMode || VerboseLevel==LOG_DEBUG) // User wants to see it
            if(!RawMode) // Don't show this in RAW Mode since packets can be
                         // huge! Will slow down
            {
               PRETTYPRINT((uint8_t*)ReadBuffer, static_cast<uint32_t>(CharsInBuffer), MAX_READ_BUFFER_SIZE);
            }
            else if(PrettyPrintRawPacketsToo == 1)
            {
               PRETTYPRINT((uint8_t*)ReadBuffer, static_cast<uint32_t>(CharsInBuffer), MAX_READ_BUFFER_SIZE);
            }

            // else
            //   dbg (LOG_DEBUG,"%d new bytes read from USB,
            //   CharsInBuffer=%d",BytesRead,CharsInBuffer);
         }
         else
         {
            // Didn't get anything, pause and try again
            dbg(
               LOG_INFO,
               "Didn't get any data from USB, sleeping for 1ms and trying "
               "again NumTries=%d of 10",
               localNumTries
            );

            // increase timeout to 5 minute to accommodate slow targets
            if(localNumTries >= 10)
            // if(NumTries>=9999999999)  // use this to debug issues with JTAG
            {
               // break;  //Don't exit here, allow caller to handle this
               dbg(LOG_ERROR, "Can't write to your target?");
               ExitAndShowLog(1);
            }

            // sleep(10);    // use this to debug issues with JTAG
            sleep(1);
            dbg(LOG_INFO, "Back from sleep");
            continue; // we have no data yet
         }
      }

      NumTries = 0;
      // dbg (LOG_INFO,"Resetting NumTries=%d",NumTries);

      if(RawMode) break;

      // strcpy(ReadBuffer,"<?xml version=\"1.0\" ?>\n<patches>\n<patch
      // SECTOR_SIZE_IN_BYTES=\"512\" byte_offset=\"88\" filename=\"DISK\"
      // physical_partition_number=\"0\" size_in_bytes=\"4\"
      // start_sector=\"NUM_DISK_SECTORS-1.\"
      // value=\"CRC32(NUM_DISK_SECTORS-33.,4608)\" what=\"Update Backup Header
      // with CRC of Partition Array.\"/>\n</patches>"); PacketLoc =
      // DetermineTag((char *)&ReadBuffer[PacketLoc], PacketLoc,
      // MAX_READ_BUFFER_SIZE);    // This sets CurrentHandlerFunction()

      PacketStart = PacketLoc;
      ShowXMLFileInLog = 1;
      PacketLoc = DetermineTag(
         (char*)ReadBuffer,
         PacketLoc,
         MAX_READ_BUFFER_SIZE
      ); // This sets
         // CurrentHandlerFunction()
      PacketLength = ThisXMLLength;
      // PacketLoc should be past the xml and data tags, pointing at the 1st
      // attribute So if PacketLoc is still it 0, something went wrong

      if(PacketLoc == 0)
      {
         dbg(LOG_ERROR, "3. TAG not found or recognized");

         if((ReadBuffer[0] == 1 && ReadBuffer[4] == 48) || (ReadBuffer[0] == 4 && ReadBuffer[4] == 16))
         {
            dbg(
               LOG_ERROR,
               "\n\n\tThere is a chance your target is in SAHARA mode!!"
               "\n\tThere is a chance your target is in SAHARA mode!!"
               "\n\tThere is a chance your target is in SAHARA mode!!"
               "\n\nThis can mean"
               "\n\t1. You forgot to send DeviceProgrammer first (i.e. "
               "QSaharaServer.exe -s 13:prog_emmc_firehose_8994_lite.mbn)"
               "\n\t2. OR, you did send DeviceProgrammer, but it has crashed "
               "and/or is not correct for this target"
               "\n\nRegardless this program speaks FIREHOSE protocol and your "
               "target is speaking SAHARA protcol, so this will not work\n\n"
            );
            ExitAndShowLog(1);
         }

         return FIREHOSE_ERROR;
      }

      if(ParseAttributes)
      {
         // To be this far fh.xml_buffer[PacketLoc] is pointing to the first
         // char of the first attribute
         PacketLoc = DetermineAttributes((char*)ReadBuffer, PacketLoc, MAX_READ_BUFFER_SIZE);

         if(PacketLoc == 0)
         {
            dbg(LOG_ERROR, "ATTRIBUTES not found or recognized");
            return FIREHOSE_ERROR;
         }
      }
      else
         PacketLoc = GetStringFromXML((char*)&ReadBuffer[PacketLoc], PacketLoc, FIREHOSE_TX_BUFFER_SIZE);


      // At this point some attributes will need additional processing before
      // we call the Tag Handler function
      if(ParseComplicatedAttributes() == FIREHOSE_ERROR) // i.e. start_sector="NUM_DISKSECTORS-33."
      {
         return FIREHOSE_ERROR;
      }

      // Below is the function pointer, i.e. tag_handler_t
      // CurrentHandlerFunction;
      (this->*(CurrentHandlerFunction))();

      while(IsEmptySpace(ReadBuffer[PacketLoc]))
      {
         PacketLoc++;

         if((SIZE_T)PacketLoc >= PacketLength) break;
      }

      // CharsInBuffer         = BytesReadPlusLeftOver - PacketLoc;
      // //MAX_READ_BUFFER_SIZE - PacketLoc; //BytesRead - PacketLoc;
      CharsInBuffer -= (PacketLoc - PacketStart); // (PacketLoc-PacketStart) is
                                                  // how many characters we used
      // BytesReadPlusLeftOver = CharsInBuffer;

      if(CharsInBuffer <= 0)
      {
         CharsInBuffer = 0;
         PacketLoc = 0;
         // BytesReadPlusLeftOver = 0;
      }

      // else
      //   dbg (LOG_INFO,"BUFFER: (%i bytes)\n%s",CharsInBuffer,(char
      //   *)&ReadBuffer[PacketLoc]);

      TimeToBreak = 1; // assume in here once

      if(strncasecmp(NewTagName, "log", strlen((const char*)NewTagName)) == 0)
      {
         TimeToBreak = 0; // to be here this packet was a log, so continue
      }
   }

   // BytesRead = ReadPort(buffer, 1024);
   // PRETTYPRINT(buffer, BytesRead);
   return FIREHOSE_SUCCESS;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleResponse()
{
   GotACK = 0; // reset

   dbg(LOG_DEBUG, "Response was '%s'", fh.attrs.value);

   if(strncasecmp((const char*)fh.attrs.value, "ack", strlen((const char*)fh.attrs.value)) == 0)
   {
      GotACK = 1;
   }

   return FIREHOSE_SUCCESS;
}

void FirehoseLoader::LoadPowerOpIntoStringTable(enum fh_reboot_options option, uint32_t seconds)
{
   std::string reboot_str;
   switch(option)
   {
      case FH_REBOOT_POWER_OFF:
         reboot_str = "off";
         break;
      case FH_REBOOT_EDL:
         reboot_str = "reset_to_edl";
         break;
      case FH_REBOOT_RESET:
      default:
         reboot_str = "reset";
         break;
   }
   InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
   AppendToBuffer(tx_buffer, "<data>\n<power ", FIREHOSE_TX_BUFFER_SIZE);
   sprintf(temp_buffer, "DelayInSeconds=\"%d\" ", seconds);
   AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
   sprintf(temp_buffer, "value=\"%s\" />\n</data>", reboot_str.c_str());
   AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

   CopyString(
      XMLStringTable[NumXMLFilesInTable],
      tx_buffer,
      0,
      0,
      strlen(tx_buffer),
      MAX_XML_SIZE,
      FIREHOSE_TX_BUFFER_SIZE
   );
   NumXMLFilesInTable++;

   if(NumXMLFilesInTable >= MAX_XML_FILES)
   {
      dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
      ExitAndShowLog(1);
   }
}

void FirehoseLoader::LoadResetIntoStringTable(void)
{
   InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
   AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
   AppendToBuffer(
      tx_buffer,
      "<power DelayInSeconds=\"10\" value=\"reset\" ",
      FIREHOSE_TX_BUFFER_SIZE
   ); // HACK for Zeno, set 10 back to 2

   AppendToBuffer(tx_buffer, "/>\n</data>\n",
                  FIREHOSE_TX_BUFFER_SIZE); // HACK for Zeno, remove the \n

   // Save into XMLFileTable
   CopyString(
      XMLStringTable[NumXMLFilesInTable],
      tx_buffer,
      0,
      0,
      strlen(tx_buffer),
      MAX_XML_SIZE,
      FIREHOSE_TX_BUFFER_SIZE
   );
   NumXMLFilesInTable++;

   if(NumXMLFilesInTable >= MAX_XML_FILES)
   {
      dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
      ExitAndShowLog(1);
   }
}

void FirehoseLoader::LoadConfigureIntoTXBuffer(void)
{
   InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
   AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
   AppendToBuffer(tx_buffer, "<configure ", FIREHOSE_TX_BUFFER_SIZE);

   sprintf(temp_buffer, "MemoryName=\"%s\" ", fh.attrs.MemoryName);
   AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);


   if(Verbose)
      AppendToBuffer(tx_buffer, "Verbose=\"1\" ", FIREHOSE_TX_BUFFER_SIZE);
   else
      AppendToBuffer(tx_buffer, "Verbose=\"0\" ", FIREHOSE_TX_BUFFER_SIZE);

   if(testvipimpact)
      AppendToBuffer(tx_buffer, "AlwaysValidate=\"1\" ", FIREHOSE_TX_BUFFER_SIZE);
   else
      AppendToBuffer(tx_buffer, "AlwaysValidate=\"0\" ", FIREHOSE_TX_BUFFER_SIZE);


   sprintf(temp_buffer, "MaxDigestTableSizeInBytes=\"%" SIZE_T_FORMAT "\" ", fh.attrs.MaxDigestTableSizeInBytes);
   AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

   sprintf(temp_buffer, "MaxPayloadSizeToTargetInBytes=\"%" SIZE_T_FORMAT "\" ", fh.attrs.MaxPayloadSizeToTargetInBytes);
   AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

   if(fh.attrs.ZlpAwareHost)
      AppendToBuffer(tx_buffer, "ZlpAwareHost=\"1\" ", FIREHOSE_TX_BUFFER_SIZE);
   else
      AppendToBuffer(tx_buffer, "ZlpAwareHost=\"0\" ", FIREHOSE_TX_BUFFER_SIZE);

   if(fh.attrs.SkipWrite)
   {
      sprintf(temp_buffer, "SkipWrite=\"%" SIZE_T_FORMAT "\" ", fh.attrs.SkipWrite);
      AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
   } // HACK, I removed else to match zeno

   //  if(fh.attrs.SkipStorageInit)
   //  {
   sprintf(temp_buffer, "SkipStorageInit=\"%" SIZE_T_FORMAT "\" ", fh.attrs.SkipStorageInit);
   AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
   //  }

   // AppendToBuffer (tx_buffer, "TargetName=\"8960\" ",
   // FIREHOSE_TX_BUFFER_SIZE); // HACK, to match Zeno

   AppendToBuffer(
      tx_buffer,
      "/>\n</data>\n",
      FIREHOSE_TX_BUFFER_SIZE
   ); // HACK, the \n here is to match
      // Zeno
}

void FirehoseLoader::LoadConfigureIntoStringTable(void)
{
   // always goes at 0
   LoadConfigureIntoTXBuffer();
   // Save into XMLFileTable at location 0, not at NumXMLFilesInTable since
   // <configure> is always our first command
   CopyString(XMLStringTable[0], tx_buffer, 0, 0, strlen(tx_buffer), MAX_XML_SIZE, FIREHOSE_TX_BUFFER_SIZE);
}
/*
void SendConfigure(void)
{
  dbg (LOG_INFO,"Sending <configure>");

  // tx_buffer already holds the XML file
    sendTransmitBuffer();

  if(Simulate)
  {
    InitBufferWithXMLHeader(&ReadBuffer[PacketLoc],
MAX_READ_BUFFER_SIZE-PacketLoc);
    AppendToBuffer(ReadBuffer,"<data>\n",MAX_READ_BUFFER_SIZE);
    if(fh.attrs.MaxPayloadSizeToTargetInBytes >
fh.attrs.MaxPayloadSizeToTargetInBytesSupported)
    {
      AppendToBuffer(ReadBuffer,"<response value=\"NAK\"
",MAX_READ_BUFFER_SIZE); sprintf(temp_buffer,"MaxPayloadSizeToTargetInBytes=\"%"
SIZE_T_FORMAT "\" ",fh.attrs.MaxPayloadSizeToTargetInBytesSupported);
      AppendToBuffer(ReadBuffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);
      sprintf(temp_buffer,"MaxPayloadSizeToTargetInBytesSupported=\"%"
SIZE_T_FORMAT "\" ",fh.attrs.MaxPayloadSizeToTargetInBytesSupported);
      AppendToBuffer(ReadBuffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);
    }
    else
    {
      AppendToBuffer(ReadBuffer,"<response value=\"ACK\"
",MAX_READ_BUFFER_SIZE); sprintf(temp_buffer,"MaxPayloadSizeToTargetInBytes=\"%"
SIZE_T_FORMAT "\" ",fh.attrs.MaxPayloadSizeToTargetInBytes);
      AppendToBuffer(ReadBuffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);
      sprintf(temp_buffer,"MaxPayloadSizeToTargetInBytesSupported=\"%"
SIZE_T_FORMAT "\" ",fh.attrs.MaxPayloadSizeToTargetInBytesSupported);
      AppendToBuffer(ReadBuffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);
    }
    AppendToBuffer(ReadBuffer,"/>\n</data>",MAX_READ_BUFFER_SIZE);
    CharsInBuffer = strlen(ReadBuffer);
    //sprintf(temp_buffer,"value=\"%s\" ",fh.attrs.value);
    //AppendToBuffer(ReadBuffer,temp_buffer);

  }

  GetNextPacket();  // this will set all variables, including GotACK

  // Target can ACK or NAK my configure packet. As it is setup now, all that
changes is that Target will have *set* variables on PC side
  // That is fh_loader.exe will have it's MaxPayloadSizeToTargetInBytes *set* to
whatever target has

  //MaxPayloadSizeToTargetInBytesSupported


  if(GotACK)
    dbg (LOG_INFO,"Got the ACK");
  else
    dbg (LOG_INFO,"Got a NAK, but this is OK. fh_loader switched to use Targets
parameters!");

}
*/

void FirehoseLoader::SendReset(void)
{
   dbg(LOG_INFO, "Sending RESET to target");

   // tx_buffer already holds the XML file
   notifyMessage(FirehoseEvent::EventId::FIREHOSE_RESETTING, tx_buffer);
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(GotACK)
   {
      dbg(LOG_DEBUG, "Got the ACK");
   }
}

FirehoseLoader::SIZE_T FirehoseLoader::SplitStringOnCommas(
   char* sz,
   SIZE_T SizeOfString,
   char* szArray[],
   int32_t offset,
   SIZE_T ArraySize,
   SIZE_T StringSize
)
{
   (void)StringSize; // Suppress unused parameter warning
   char c;
   SIZE_T i, j, k, comma = 0;
   j = strlen(sz);

   k = offset;

   if(k >= ArraySize)
   {
      dbg(LOG_ERROR, "In SplitStringOnCommas the array cannot hold anymore strings");
      ExitAndShowLog(1);
   }

   for(i = 0; i < j; i++)
   {
      // look for comma
      c = sz[i];

      if(c == ',')
      {
         c = c;

         // NOTE: there could be white space after the comma
         while(IsEmptySpace(sz[comma]))
         {
            comma++;

            if(comma >= j)
            {
               dbg(
                  LOG_ERROR,
                  "XML packet not formed correctly. Ran out of "
                  "room looking for TAG"
               );
               return 0;
            }
         }

         if(i - comma == 0)
         {
            // user has extra enters or extra commas, i.e.
            // path1
            // path2
            //         <-- just ignore this, it will be length 0, i.e.
            //         (i-comma==0)
            // path3
         }
         else
         {
            if(CopyString(szArray[k], sz, 0, comma, i - comma, MAX_PATH_SIZE, SizeOfString) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy string of length %" SIZE_T_FORMAT " bytes into szArray[%" SIZE_T_FORMAT "]",
                  i - comma,
                  k
               );
               ExitAndShowLog(1);
            }

            k++;

            if(k >= ArraySize)
            {
               dbg(
                  LOG_ERROR,
                  "In SplitStringOnCommas the array cannot hold "
                  "anymore strings"
               );
               ExitAndShowLog(1);
            }
         }

         comma = i + 1; // +1 to skip this comma
      }
   }

   if(comma < j)
   {
      if(CopyString(szArray[k], sz, 0, comma, i - comma, MAX_PATH_SIZE, SizeOfString) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy string of length %" SIZE_T_FORMAT " bytes into szArray[%" SIZE_T_FORMAT "]",
            i - comma,
            k
         );
         ExitAndShowLog(1);
      }

      // quick sanity check, since this is the last of the split, we could have
      // had filename,,, where the multiple commas are hard to deal with. So,
      // lets look at szArray[k] and see if it is all spaces

      j = 0;

      while(IsEmptySpace(szArray[k][j]))
      {
         j++;

         if(j >= static_cast<SIZE_T>(strlen(szArray[k]))) break;
      }

      if(j < static_cast<SIZE_T>(strlen(szArray[k])))
         k++; // this means the string had something more than spaces, so let
              // it stay
   }

   return k;
}

void FirehoseLoader::CheckContentsXMLCompleteFileAndPath(char* filename)
{
   SIZE_T i = 0;

   memset(contents_full_filename_with_path, 0x0, MAX_PATH_SIZE);

   if(NumContensXML == 0) return;

   dbg(
      LOG_DEBUG,
      "============================================================"
      "======================"
   );
   dbg(LOG_DEBUG, "Seeing if file '%s' is in contents.xml", filename);


   for(i = 0; i < NumContensXML; i++)
   {
      // loop through files found in contents.xml
      if(strncmp(filename, ContensXML[i].Filename, MAX(strlen(filename), strlen(ContensXML[i].Filename))) == 0)
      {
         // it matches
         if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
         {
            // to be here means either ContensXML[i].StorageType=0 which is
            // unknown, so use it OR, ContensXML[i].StorageType=='eMMC' which
            // matches fh.attrs.MemoryName[0]

            i = i; // Aaron, send back concat path and filename

            if(CopyString(
                  contents_full_filename_with_path,
                  ContensXML[i].Path,
                  0,
                  0,
                  strlen(ContensXML[i].Path),
                  MAX_PATH_SIZE,
                  MAX_PATH_SIZE
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy string '%s' of length %i into "
                  "contents_full_filename_with_path",
                  ContensXML[i].Filename,
                  strlen(ContensXML[i].Filename)
               );
               ExitAndShowLog(1);
            }

            if(CopyString(
                  contents_full_filename_with_path,
                  ContensXML[i].Filename,
                  strlen(ContensXML[i].Path),
                  0,
                  strlen(ContensXML[i].Filename),
                  MAX_PATH_SIZE,
                  MAX_ATTR_NAME_SIZE
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy string '%s' of length %i into "
                  "contents_full_filename_with_path",
                  ContensXML[i].Filename,
                  strlen(ContensXML[i].Filename)
               );
               ExitAndShowLog(1);
            }

            return;
         } // end of matching memory name
      }
   } // end i

   return;
}

void FirehoseLoader::PossiblyShowContentsXMLDifferentFileFoundWarning(char* CurrentPathAndFilenameFound)
{
   if(strlen(contents_full_filename_with_path) > 0)
   {
      // means we have this file also in contents.xml that the user provided, so
      // see if the path is the same otherwise they have picked up the file in a
      // different location then what contents.xml specified. this is a FEATURE
      // to allow users to over ride what contents.xml has. BUT, it might also
      // be unexpected
      if(strncasecmp(
            CurrentPathAndFilenameFound,
            contents_full_filename_with_path,
            MAX(strlen(CurrentPathAndFilenameFound), strlen(contents_full_filename_with_path))
         ) != 0)
      {
         // to be here means we have found the file in a different spot then
         // what contents.xml specified
         dbg(
            LOG_WARN,
            "You provided a contents.xml which specified '%s'\n"
            "However searching through --search_paths found '%s'\n"
            "This is a FEATURE to allow you to override contents.xml "
            "mappings\n"
            "If you DID NOT WANT THIS behavior, use --forcecontentsxmlpaths\n",
            contents_full_filename_with_path,
            CurrentPathAndFilenameFound
         );
      }
   } // end if strlen
}

char* FirehoseLoader::find_file(char* filename, char ShowToScreen)
{
   SIZE_T i = 0;
   std::shared_ptr<std::fstream> fj;

   // for breakpoint only
   if(strncasecmp(filename, "cache_1.img", MAX(strlen(filename), strlen("cache_1.img"))) == 0)
   {
      i = i; // for breakpoint
   }

   dbg(
      LOG_DEBUG,
      "============================================================"
      "======================"
   );
   dbg(
      LOG_DEBUG,
      "============================================================"
      "======================"
   );

   if(ShowToScreen)
      dbg(LOG_INFO, "Looking for file '%s'", filename);
   else
      dbg(LOG_DEBUG, "Looking for file '%s'", filename);

   CheckContentsXMLCompleteFileAndPath(filename); // this will fill contents_full_filename_with_path

   if(forcecontentsxmlpaths)
   {
      // if I find this file below in another location, warn user
      // If file found above then contents_full_filename_with_path will have the
      // path and filename
      if(strlen(contents_full_filename_with_path) > 0)
      {
         filename = contents_full_filename_with_path;
      }
   }

   // It is possible filename already has a path in it
   if(HasAPathCharacter(filename, strlen(filename)))
   {
      // if(IsARelativePath(filename, strlen(filename)))

      /*
          if(stat(filename, &status_buf) == 0)
          {

          }
      */

      fj = ReturnFileHandle(filename, MAX_PATH_SIZE,
                            "rb"); // will EXIT if not successful

      LastFindFileFileSize = ReturnFileSize(fj);
      fh_fclose(fj);
      fj = nullptr;

      dbg(LOG_DEBUG, "Found '%s' (%" SIZE_T_FORMAT " bytes)", filename, LastFindFileFileSize);

      if(AlreadyHaveThisFile(filename) == 0)
      {
         if(CopyString(
               MaxFilesToTrack[FileToTrackCount++],
               filename,
               0,
               0,
               strlen(filename),
               MAX_PATH_SIZE,
               MAX_PATH_SIZE
            ) == 0)
         {
            dbg(
               LOG_ERROR,
               "Failed to copy '%s' of length %" SIZE_T_FORMAT
               " bytes into MaxFilesToTrack at location %" SIZE_T_FORMAT,
               filename,
               strlen(filename),
               FileToTrackCount
            );
            ExitAndShowLog(1);
         }
      }

      if(CopyString(full_filename_with_path, filename, 0, 0, strlen(filename), MAX_PATH_SIZE, MAX_PATH_SIZE) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into full_filename_with_path",
            filename,
            strlen(filename)
         );
         ExitAndShowLog(1);
      }

      PossiblyShowContentsXMLDifferentFileFoundWarning(filename);

      return filename;
   }

   //  if(stat(filename, &status_buf) == 0)
   //  {
   //  }

   for(i = 0; i < num_search_paths; i++)
   {
      // dbg (LOG_DEBUG, "Looking in --> %s", search_path[i]);


      if(CopyString(
            full_filename_with_path,
            search_path[i],
            0,
            0,
            strlen(search_path[i]),
            MAX_PATH_SIZE,
            MAX_PATH_SIZE
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into full_filename_with_path",
            search_path[i],
            strlen(search_path[i])
         );
         ExitAndShowLog(1);
      }

      // if(strncpy(full_filename_with_path, search_path[i], MAX_PATH_SIZE) ==
      // 0)

      if(CopyString(
            full_filename_with_path,
            filename,
            strlen(full_filename_with_path),
            0,
            strlen(filename),
            MAX_PATH_SIZE,
            strlen(filename)
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into full_filename_with_path",
            search_path[i],
            strlen(search_path[i])
         );
         ExitAndShowLog(1);
      }

      // if (strncat(full_filename_with_path, filename, MAX_PATH_SIZE) == 0)

      dbg(LOG_DEBUG, "1. Calling stat(%s')", full_filename_with_path);

      // if (_stat64 (full_filename_with_path, &status_buf) == 0)
      if(std::filesystem::exists(full_filename_with_path)
#ifdef TOOLS_TARGET_WINDOWS
         || (GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND)
#endif
      )
      {
         fj = ReturnFileHandle(full_filename_with_path, MAX_PATH_SIZE,
                               "rb"); // will exit if not successful

         LastFindFileFileSize = ReturnFileSize(fj);
         fh_fclose(fj);
         fj = nullptr;

         dbg(LOG_DEBUG, "Found '%s' (%" SIZE_T_FORMAT " bytes)", full_filename_with_path, LastFindFileFileSize);

         if(AlreadyHaveThisFile(full_filename_with_path) == 0)
            CopyString(
               MaxFilesToTrack[FileToTrackCount++],
               full_filename_with_path,
               0,
               0,
               strlen(full_filename_with_path),
               MAX_PATH_SIZE,
               MAX_PATH_SIZE
            );

         PossiblyShowContentsXMLDifferentFileFoundWarning(full_filename_with_path);

         return full_filename_with_path;
      }
   }

   {
      char* full_path;
      full_path = file_path_search(global_file_path_list, filename);
      if(full_path != nullptr)
      {
         dbg(LOG_DEBUG, "2. Calling stat(%s)", full_path);
         // if (_stat64 (full_path, &status_buf) == 0)
         if(std::filesystem::exists(full_path)
#ifdef TOOLS_TARGET_WINDOWS
            || (GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND)
#endif
         )
         {
            CopyString(full_filename_with_path, filename, 0, 0, strlen(filename), MAX_PATH_SIZE, MAX_PATH_SIZE);


            // fj = ReturnFileHandle(filename, MAX_PATH_SIZE, "rb"); // will
            // exit if not successful
            fj = ReturnFileHandle(full_path, MAX_PATH_SIZE,
                                  "rb"); // will exit if not successful
            LastFindFileFileSize = ReturnFileSize(fj);
            fh_fclose(fj);
            fj = nullptr;

            dbg(LOG_DEBUG, "Found '%s' (%" SIZE_T_FORMAT " bytes)", full_path, LastFindFileFileSize);
            return full_path;
         }
      }
   }

   // to be here means we didn't find the file yet, so possibly we can use the
   // contents.xml mapping if it was provided
   if(strlen(contents_full_filename_with_path) > 0)
   {
      dbg(LOG_INFO, "Using contents.xml mapping '%s'", contents_full_filename_with_path);
      return contents_full_filename_with_path;
   }


   dbg(LOG_DEBUG, "2. Calling stat(%s')", filename);

   // if (_stat64 (filename, &status_buf) == 0)
   if(std::filesystem::exists(filename)
#ifdef TOOLS_TARGET_WINDOWS
      || (GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND)
#endif
   )
   {
      CopyString(full_filename_with_path, cwd, 0, 0, strlen(cwd), MAX_PATH_SIZE, MAX_PATH_SIZE);
      CopyString(full_filename_with_path, filename, strlen(cwd), 0, strlen(filename), MAX_PATH_SIZE, strlen(filename));

      // fj = ReturnFileHandle(filename, MAX_PATH_SIZE, "rb"); // will exit if
      // not successful
      fj = ReturnFileHandle(full_filename_with_path, MAX_PATH_SIZE,
                            "rb"); // will exit if not successful
      LastFindFileFileSize = ReturnFileSize(fj);
      fh_fclose(fj);
      fj = nullptr;

      dbg(
         LOG_DEBUG,
         "Found '%s' (%" SIZE_T_FORMAT " bytes) in local directory '%s",
         filename,
         LastFindFileFileSize,
         cwd
      );

      // CopyString(MaxFilesToTrack[FileToTrackCount-1], filename, strlen(cwd),
      // 0, strlen(filename), MAX_PATH_SIZE, strlen(filename));
      PossiblyShowContentsXMLDifferentFileFoundWarning(full_filename_with_path);

      return full_filename_with_path;
   }

   // to be here means we didn't find the file yet, so possibly we can use the
   // contents.xml mapping if it was provided
   if(strlen(contents_full_filename_with_path) > 0)
   {
      dbg(LOG_INFO, "Using contents.xml mapping '%s'", contents_full_filename_with_path);
      return contents_full_filename_with_path;
   }

   dbg(LOG_WARN, "Couldn't find the file '%s', returning NULL", filename);
   return NULL;
}

int32_t FirehoseLoader::ThisFileIsInFilterFiles(char* filename_only)
{
   SIZE_T i;

   for(i = 0; i < num_filter_files; i++)
   {
      if(strncasecmp(filter_files[i], filename_only, MAX(strlen(filter_files[i]), strlen(filename_only))) == 0)
      {
         return 1; // TRUE, load this file
      }
   }

   // This file is an exception Zeros16KB.bin
   if(strncasecmp("Zeros16KB.bin", filename_only, MAX(strlen("Zeros16KB.bin"), strlen(filename_only))) == 0)
   {
      return 1; // TRUE, load this file
   }

   return 0; // don't load this file
}

int32_t FirehoseLoader::ThisFileIsInNotFilterFiles(char* filename_only)
{
   SIZE_T i;

   for(i = 0; i < num_filter_not_files; i++)
   {
      if(strncasecmp(filter_not_files[i], filename_only, MAX(strlen(filter_not_files[i]), strlen(filename_only))) == 0)
      {
         return 1; // TRUE, do not load this file
      }
   }

   return 0; // FALSE, user is ok to load this file
}

int32_t FirehoseLoader::AlreadyHaveThisFile(char* filePath)
{
   SIZE_T i;

   for(i = 0; i < FileToTrackCount; i++)
   {
      if(strncasecmp(MaxFilesToTrack[i], filePath, MAX(strlen(MaxFilesToTrack[i]), strlen(filePath))) == 0)
      {
         return 1; // TRUE, we already have this file
      }
   }

   return 0; // FALSE, we do not have this file yet
}

int32_t FirehoseLoader::AlreadyHaveThisPath(char* path)
{
   SIZE_T i;

   for(i = 0; i < num_search_paths; i++)
   {
      if(strncmp(search_path[i], path, MAX(strlen(search_path[i]), strlen(path))) == 0)
      {
         return 1; // TRUE, we already have this file
      }
   }

   return 0; // FALSE, we do not have this file yet
}


void FirehoseLoader::OpenAndStoreAllXMLFiles(void)
{
   SIZE_T k = 0;
   char* XmlFile = nullptr;

   while(XMLFileTable[k][0] != '\0')
   {
      dbg(LOG_INFO, "\n\n\n");
      dbg(LOG_INFO, "Trying to store '%s' in string table", XMLFileTable[k]);
      XmlFile = find_file(XMLFileTable[k], 1);

      if(XmlFile == NULL)
      {
         dbg(LOG_ERROR, "'%s' not found", XMLFileTable[k]);
         ExitAndShowLog(1);
      }

      // To be here we have a file to parse
      StoreXMLFile(XmlFile);
      XMLFileTable[k][0] = '\0';
      k++;
   }
}

void FirehoseLoader::SendXmlFiles(void)
{
   SIZE_T k = 0;

   while(XMLStringTable[k][0] != '\0')
   {
      if(SimulateForFileSize)
      {
         // only care about sending <program when SimulateForFileSize
         char *pch, *pch2;
         pch = strstr(XMLStringTable[k], "<program");
         pch2 = strstr(XMLStringTable[k], "<read");

         if(pch == NULL && pch2 == NULL)
         {
            k++; // move to next file
            continue;
         }
      }

      if(GenerateSha256File)
      {
         // only care about sending <program when GenerateSha256File
         char* pch;
         pch = strstr(XMLStringTable[k], "<program");
         if(pch == NULL)
         {
            k++; // move to next file
            continue;
         }
      }

      // dbg (LOG_ALWAYS,"FOR DEBUG XMLStringTable[%i]
      // is\n%s",k,XMLStringTable[k]);
      SendXMLString(XMLStringTable[k], MAX_XML_SIZE);
      /*
          if(stresstest)
          {
            // this file will grow crazy large doing this
            fh_fclose(fp);
            fp = ReturnFileHandle(PortTraceName, MAX_PATH_SIZE, "w");     //
         will exit if not successful
          }
      */
      k++;
   }

   NumXMLFilesInTable = k;
}

void FirehoseLoader::StoreXMLFile(char* FileWithPath)
{
   SIZE_T localPacketLoc = 0, PacketLocBack = 0;
   SIZE_T XMLFileSize = 0;
   char* pch;
   char single_xml_buffer[MAX_STRING_SIZE];
   char* sparse_pch = nullptr;
   int32_t i;
   char* FileToSendWithPath = nullptr;
   size_t xml_buffer_size;
   char* xml_buffer;

   std::shared_ptr<std::fstream> fd = ReturnFileHandle(FileWithPath, MAX_PATH_SIZE,
                                                       "r"); // will exit if not successful

   XMLFileSize = ReturnFileSize(fd);

   xml_buffer_size = XMLFileSize + 1; /* +1 for NULL character*/
   xml_buffer = (char*)calloc(xml_buffer_size, 1);
   if(xml_buffer == NULL)
   {
      dbg(
         LOG_ERROR,
         "Falled to allocate a buffer of %zu bytes to store read the file %s, "
         "reduce the file size or free some memory and try again.",
         xml_buffer_size,
         FileWithPath
      );
      ExitAndShowLog(1);
   }

   memset(xml_buffer, 0, xml_buffer_size);
   fd->read(xml_buffer, XMLFileSize);
   fh_fclose(fd);
   fd = nullptr;

   /*
     // I can handle only a single </ in the file, which should come at the end,
     i.e. </configuration> or </data>
     // so if I find more than one, probably somebody has a compound XML file,
     so I'll just ignore it pch = xml_buffer; pch = strstr(pch, "</");
     if(pch!=NULL)
     {
       pch++;  // move 1 further
       pch = strstr(pch, "</");
       if(pch!=NULL)
       {
         dbg (LOG_INFO,"The XML file has compound xml tags that cannot be
     handled. Ignoring '%s'",FileWithPath); free(xml_buffer); return;
       }
     }
   */

   // Could be a config.xml file created by parseContentsXML.py
   pch = strstr(xml_buffer, "<options>");

   if(pch != nullptr)
   {
      pch = strstr(xml_buffer, "<search_paths>");

      if(pch != nullptr)
      {
         parseConfigXML(XMLFileSize, xml_buffer,
                        xml_buffer_size); // works on EntireXMLFileBuffer
         CleanseSearchPaths();
         free(xml_buffer);
         return;
      }
   }

   // Could be a partition_nand.xml, which we don't support
   pch = strstr(xml_buffer, "<magic_numbers>");

   if(pch != nullptr)
   {
      dbg(
         LOG_WARN,
         "Found 'magic_numbers' tag, appears to be a "
         "partition_nand.xml, so ignoring this"
      );
      free(xml_buffer);
      return;
   }

   while(1)
   {
      // xml_buffer holds the entire XML file, which might have multiple <tags>
      ShowXMLFileInLog = 0;
      localPacketLoc = DetermineTag(
         (char*)xml_buffer,
         localPacketLoc,
         xml_buffer_size
      ); // This sets CurrentHandlerFunction()


      // Note, if I find <parser_instructions>, then this is an older
      // contents.xml where they include partition.xml
      //       so just bail, I only care about rawprogram.xml and patch.xml

      if(strncmp(NewTagName, "parser_instructions", MAX(strlen(NewTagName), strlen("parser_instructions"))) == 0)
      {
         dbg(
            LOG_DEBUG,
            "File '%s' is being ignored since it contained "
            "<parser_instructions> and is probably a partition.xml file",
            FileWithPath
         );
         free(xml_buffer);
         return;
      }

      // This chomps up empty space till we hit a tag
      while(IsEmptySpace(xml_buffer[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= XMLFileSize)
         {
            break; // we have run out of packet all is well
         }
      }

      // XMLFileSize = strlen((char *)xml_buffer); // since DetermineTag() can
      // replace final '\n' with '\0'

      if(localPacketLoc == 0)
      {
         dbg(LOG_ERROR, "1. TAG not found or recognized");
         free(xml_buffer);
         return;
      }

      pch = strstr(xml_buffer, "/>");

      if(pch == NULL)
      {
         dbg(LOG_ERROR, "XML is not formatted correctly. Could not find closing />\n\n");
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      // NOTE: Below I parse attributes *only* to support the wipefirst feature.
      // This is because I need to know the
      //       start sector at which to write 16KB of zeros. However,
      //       ParseAttributes will *increase* localPacketLoc which I don't
      //       want, so back it up, and replace when done.

      PacketLocBack = localPacketLoc;

      if(ParseAttributes)
      {
         // To be this far fh.xml_buffer[localPacketLoc] is pointing to the
         // first char of the first attribute
         localPacketLoc = DetermineAttributes((char*)xml_buffer, localPacketLoc, xml_buffer_size);

         if(localPacketLoc == 0)
         {
            dbg(LOG_ERROR, "ATTRIBUTES not found or recognized");
            free(xml_buffer);
            return;
         }
      }

      localPacketLoc = PacketLocBack;

      if(strncmp(NewTagName, "program", MAX(strlen(NewTagName), strlen("program"))) == 0)
      {
         // To be here means we're looking at a <program command. This is the
         // only tag I want to possibly add an <erase or <program
         // filename="Zeros16KB.bin"

         if(wipefirst)
         {
            InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<program ", FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SectorSizeInBytes);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ", ((16 * 1024) / SectorSizeInBytes));
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "filename=\"%s\" ", WipeFirstFileName);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(
               temp_buffer,
               "physical_partition_number=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.physical_partition_number
            );
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "start_sector=\"%s\" ", fh.attrs.start_sector);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);


            // Save into XMLFileTable
            CopyString(
               XMLStringTable[NumXMLFilesInTable],
               tx_buffer,
               0,
               0,
               strlen(tx_buffer),
               MAX_XML_SIZE,
               FIREHOSE_TX_BUFFER_SIZE
            );
            NumXMLFilesInTable++;

            if(NumXMLFilesInTable >= MAX_XML_FILES)
            {
               dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
               free(xml_buffer);
               ExitAndShowLog(1);
            }
         }

         if(erasefirst)
         {
            InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
            AppendToBuffer(tx_buffer, "<erase ", FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", SectorSizeInBytes);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ", fh.attrs.num_partition_sectors);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(
               temp_buffer,
               "physical_partition_number=\"%" SIZE_T_FORMAT "\" ",
               fh.attrs.physical_partition_number
            );
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            sprintf(temp_buffer, "start_sector=\"%s\" ", fh.attrs.start_sector);
            AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

            AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

            CopyString(
               XMLStringTable[NumXMLFilesInTable],
               tx_buffer,
               0,
               0,
               strlen(tx_buffer),
               MAX_XML_SIZE,
               FIREHOSE_TX_BUFFER_SIZE
            );
            NumXMLFilesInTable++;

            if(NumXMLFilesInTable >= MAX_XML_FILES)
            {
               dbg(LOG_ERROR, "1. Too many XML files in XMLStringTable, max is %d", XMLStringTable);
               free(xml_buffer);
               ExitAndShowLog(1);
            }
         } // end of erasefirst

      } // end of this is a <program command
      snprintf_x(single_xml_buffer, pch - &xml_buffer[localPacketLoc] + 3, "%s", xml_buffer + localPacketLoc);
      sparse_pch = strstr(single_xml_buffer, "sparse=\"true\"");

      if(sparse_pch != nullptr && (strcmp(NewTagName, "program") == 0) && fh.attrs.filename[0] != '\0')
      {
         // ITS A SPARSE IMAGE!!!
         // fh.attrs.filename is correct at this point...
         for(i = 0; i < static_cast<int32_t>(strlen(fh.attrs.filename)); i++)
         {
            if(IsEmptySpace(fh.attrs.filename[i])) fh.attrs.filename[i] = '\0';
         }

         FileToSendWithPath = find_file(fh.attrs.filename, 1);

         if(FileToSendWithPath != nullptr)
         {
            fd = ReturnFileHandle(FileToSendWithPath, MAX_PATH_SIZE,
                                  "rb"); // will exit if not successful
            GenerateSparseXMLTags(
               fd,
               fh.attrs.filename,
               atoi(fh.attrs.start_sector),
               fh.attrs.SECTOR_SIZE_IN_BYTES,
               fh.attrs.physical_partition_number
            );
         }
      }
      else
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n<", FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "%s ", NewTagName);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         CopyString(
            tx_buffer,
            xml_buffer,
            strlen(tx_buffer),
            localPacketLoc,
            pch - &xml_buffer[localPacketLoc],
            FIREHOSE_TX_BUFFER_SIZE,
            xml_buffer_size
         );
         AppendToBuffer(tx_buffer, " />\n</data>", FIREHOSE_TX_BUFFER_SIZE);


         CopyString(
            XMLStringTable[NumXMLFilesInTable],
            tx_buffer,
            0,
            0,
            strlen(tx_buffer),
            MAX_XML_SIZE,
            FIREHOSE_TX_BUFFER_SIZE
         );
         NumXMLFilesInTable++;
      }

      localPacketLoc += pch - &xml_buffer[localPacketLoc] + 2; // +2 gets past the />

      if(NumXMLFilesInTable >= MAX_XML_FILES)
      {
         dbg(LOG_ERROR, "2. Too many XML files in XMLStringTable, max is %d", MAX_XML_FILES);
         free(xml_buffer);
         ExitAndShowLog(1);
      }


      // This chomps up empty space till we hit a tag
      while(IsEmptySpace(xml_buffer[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= XMLFileSize)
         {
            break; // we have run out of packet all is well
         }
      }

      if(xml_buffer[localPacketLoc] == 0) break; // we're out of packet, all is well

      // if(strlen( &xml_buffer[localPacketLoc] ) == 0)
      //   break;
   }
   free(xml_buffer);
}

void FirehoseLoader::SendXMLString(char* sz, SIZE_T MaxLength)
{
   SIZE_T localPacketLoc = 0;
   SIZE_T XMLFileSize = strlen(sz);

   if(XMLFileSize > MaxLength)
   {
      dbg(
         LOG_ERROR,
         "The XML string of size %" SIZE_T_FORMAT " is bigger than %" SIZE_T_FORMAT
         " buffer size. Please reduce the file size",
         XMLFileSize,
         MaxLength
      );
      ExitAndShowLog(1);
   }

   // Load tx_buffer with XML to send
   CopyString(tx_buffer, sz, 0, 0, XMLFileSize, FIREHOSE_TX_BUFFER_SIZE, MaxLength);

   while(1)
   {
      // localPacketLoc = DetermineTag((char
      // *)&EntireXMLFileBuffer[localPacketLoc], localPacketLoc,
      // MAX_XML_FILE_SIZE);    // This sets CurrentHandlerFunction()
      ShowXMLFileInLog = 0;
      localPacketLoc = DetermineTag((char*)sz, localPacketLoc,
                                    MaxLength); // This sets CurrentHandlerFunction()

      // This chomps up empty space till we hit a tag
      while(IsEmptySpace(sz[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= XMLFileSize)
         {
            break; // we have run out of packet all is well
         }
      }

      // XMLFileSize = strlen((char *)sz); // since DetermineTag() can replace
      // final '\n' with '\0'

      if(localPacketLoc == 0)
      {
         dbg(LOG_ERROR, "1. TAG not found or recognized");
         return; // FIREHOSE_ERROR;
      }

      if(ParseAttributes)
      {
         // To be this far fh.xml_buffer[localPacketLoc] is pointing to the
         // first char of the first attribute
         localPacketLoc = DetermineAttributes((char*)sz, localPacketLoc, MaxLength);

         if(localPacketLoc == 0)
         {
            dbg(LOG_ERROR, "ATTRIBUTES not found or recognized");
            return; // FIREHOSE_ERROR;
         }
      }
      else
      {
         localPacketLoc = GetStringFromXML((char*)&sz[localPacketLoc], localPacketLoc, MaxLength);
      }


      // At this point some attributes will need additional processing before
      // we call the Tag Handler function
      if(ParseComplicatedAttributes() == FIREHOSE_ERROR) // i.e. start_sector="NUM_DISKSECTORS-33."
      {
         return;
      }

      // Below is the function pointer, i.e. tag_handler_t
      // CurrentHandlerFunction;
      (this->*(CurrentHandlerFunction))();

      // This chomps up empty space till we hit a tag
      while(IsEmptySpace(sz[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= XMLFileSize)
         {
            break; // we have run out of packet all is well
         }
      }

      if(sz[localPacketLoc] == 0) break; // we're out of packet, all is well

      // if(localPacketLoc>=XMLFileSize)
      //   break;
   }

   // Put tx_buffer back into sz, which preserves it for additional runs
   CopyString(sz, tx_buffer, 0, 0, XMLFileSize, FIREHOSE_TX_BUFFER_SIZE, MaxLength);
}


void FirehoseLoader::display_error()
{
   FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE("\n\n\n");
   FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE("\t _____                    ");
   FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE("\t|  ___|                   ");
   FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE("\t| |__ _ __ _ __ ___  _ __ ");
   FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE("\t|  __| '__| '__/ _ \\| '__|");
   FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE("\t| |__| |  | | | (_) | |   ");
   FIREHOSE_LOADER_PORT_TRACE_ERROR_MESSAGE("\t\\____/_|  |_|  \\___/|_|  "
                                            "\n\n");
}

void FirehoseLoader::display_warning()
{
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\n\n\n");
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\t                         (_)  "
                                              "          ");
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\t__      ____ _ _ __ _ __  _ _ "
                                              "__   __ _ ");
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\t\\ \\ /\\ / / _` | '__| '_ "
                                              "\\| | '_ \\ / _` |");
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\t \\ V  V / (_| | |  | | | | | "
                                              "| | | (_| |");
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\t  \\_/\\_/ \\__,_|_|  |_| "
                                              "|_|_|_| |_|\\__, |");
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\t                              "
                                              "     __/ |");
   FIREHOSE_LOADER_PORT_TRACE_WARNING_MESSAGE("\t                              "
                                              "    |___/ \n\n");
}

#define IS_WRITE(mode) (strchr(mode, 'w') != nullptr || strchr(mode, 'a') != nullptr)
#define IS_APPEND(mode) (strchr(mode, 'a') != nullptr)
std::shared_ptr<std::fstream> FirehoseLoader::fh_fopen(const char* filename, const char* mode)
{
   auto pFile = std::make_shared<std::fstream>();
   std::filesystem::path localPath =
      Device::Manager::getInstance()
         ->getAccessiblePath(filename, std::filesystem::path(), false, Protocol::Base::NO_CLIENT_ID, true);

   // Try to open the file and return nullptr on failure
   try
   {
      // Convert mode string to std::ios flags
      std::ios::openmode iosMode = std::ios::binary;
      if(IS_WRITE(mode))
      {
         iosMode |= std::ios::out;
         if(IS_APPEND(mode)) iosMode |= std::ios::app;
         pFile->exceptions(std::ios::failbit | std::ios::badbit);
      }
      else
      {
         iosMode |= std::ios::in;
         pFile->exceptions(std::ios::badbit);
      }
      pFile->open(localPath.string().c_str(), iosMode);
   }
   catch(const std::ios::failure&)
   {
      std::string description = m_pFirehose.isExpired() ? "offline" : m_pFirehose->getDevice()->getDescription();
      dbg(
         LOG_ERROR,
         (std::string("fh_fopen failed for ") + std::string(filename) + " : " + mode + " : " + description).c_str()
      );
      return nullptr;
   }

   // Double check if file is actually open
   if(!pFile->is_open())
   {
      std::string description = m_pFirehose.isExpired() ? "offline" : m_pFirehose->getDevice()->getDescription();
      dbg(
         LOG_ERROR,
         (std::string("fh_fopen failed to open ") + std::string(filename) + " : " + mode + " : " + description).c_str()
      );
      return nullptr;
   }

   std::string description = m_pFirehose.isExpired() ? "offline" : m_pFirehose->getDevice()->getDescription();
   dbg(LOG_DEBUG, (std::string("fh_fopen ") + std::string(filename) + " : " + mode + " : " + description).c_str());

   return pFile;
}

void FirehoseLoader::fh_fclose(std::shared_ptr<std::fstream> pFile)
{
   if(pFile && pFile->is_open())
   {
      std::string description = m_pFirehose.isExpired() ? "offline" : m_pFirehose->getDevice()->getDescription();
      dbg(LOG_DEBUG, (std::string("fh_fclose") + " : " + description).c_str());
      pFile->close();
   }
}

std::shared_ptr<std::fstream>
FirehoseLoader::ReturnFileHandle(const char* filename, int64_t MaxLength, const char* AccessMode)
{
   char FilePlusOutputDir[MAX_PATH_SIZE] = {0};
   std::shared_ptr<std::fstream> FTemp;
   const char* pch;
   const char* w_pch = nullptr;
   const char* dir_pch = nullptr;
   const char* dir2_pch = nullptr;
   // int32_t ret;
   w_pch = strchr(AccessMode, 'w');  // check if "w" is in AccessMode
   dir_pch = strchr(filename, '\\'); // assume that with '\', user has a particular
                                     // directory for the file in mind
   dir2_pch = strchr(filename, '/'); // assume that with '/', user has a particular
                                     // directory for the file in mind

   if((strlen(MainOutputDir) > 0) && (w_pch != nullptr) && (dir_pch == NULL) &&
      (dir2_pch == NULL)) // user specified this at command line
   {
      std::string outDir(MainOutputDir);
      std::string outFile(filename);
      snprintf_x(FilePlusOutputDir, sizeof(FilePlusOutputDir), "%s", (outDir + SLASH + outFile).c_str());

      dbg(LOG_INFO, "1. Calling fopen('%s') with AccessMode='%s'", FilePlusOutputDir, AccessMode);
      FTemp = fh_fopen(FilePlusOutputDir, AccessMode);

      if(FTemp != nullptr)
      {
         // dbg (LOG_DEBUG,"Successfully called fopen with access mode '%s' on
         // '%s'",AccessMode,FilePlusOutputDir);

         if(AlreadyHaveThisFile(FilePlusOutputDir) == 0)
            CopyString(
               MaxFilesToTrack[FileToTrackCount++],
               FilePlusOutputDir,
               0,
               0,
               strlen(FilePlusOutputDir),
               MAX_PATH_SIZE,
               MAX_PATH_SIZE
            );

         return FTemp;
      }
   }

   // They may have provided a path already
   dbg(LOG_DEBUG, "2. Calling fopen('%s') with AccessMode='%s'", filename, AccessMode);
   FTemp = fh_fopen(filename, AccessMode);

   if(FTemp != nullptr)
   {
      // if there is no slashes in the filename, they did not provide a patch
      pch = strstr(filename, "/");

      if(pch == NULL) pch = strstr(filename, "\\");

      if(pch == NULL)
      {
         // they didn't provide a path
         CopyString(FilePlusOutputDir, cwd, 0, 0, strlen(cwd), MAX_PATH_SIZE, MAX_PATH_SIZE);
         CopyString(
            FilePlusOutputDir,
            filename,
            strlen(FilePlusOutputDir),
            0,
            strlen(filename),
            MAX_PATH_SIZE,
            MaxLength
         );
      }
      else
         CopyString(FilePlusOutputDir, filename, 0, 0, strlen(filename), MAX_PATH_SIZE, MaxLength);

      if(AlreadyHaveThisFile(FilePlusOutputDir) == 0)
         CopyString(
            MaxFilesToTrack[FileToTrackCount++],
            FilePlusOutputDir,
            0,
            0,
            strlen(FilePlusOutputDir),
            MAX_PATH_SIZE,
            MAX_PATH_SIZE
         );


      // dbg (LOG_DEBUG,"Successfully called fopen with access mode '%s' on
      // '%s'",AccessMode,FilePlusOutputDir);

      return FTemp;
   }

   // It is possible filename already has a path in it
   if(HasAPathCharacter(filename, strlen(filename)))
   {
      dbg(LOG_ERROR, "Could not access '%s' with acess mode '%s'", filename, AccessMode);
      ExitAndShowLog(1);
   }

   // To be this far the above MainOutputDir wasn't provided and/or it didn't
   // work so let's try the CWD

   CopyString(FilePlusOutputDir, cwd, 0, 0, strlen(cwd), MAX_PATH_SIZE, MAX_PATH_SIZE);
   CopyString(FilePlusOutputDir, filename, strlen(FilePlusOutputDir), 0, strlen(filename), MAX_PATH_SIZE, MaxLength);

   dbg(LOG_INFO, "3. Calling fopen('%s') with AccessMode='%s'", FilePlusOutputDir, AccessMode);
   FTemp = fh_fopen(FilePlusOutputDir, AccessMode);

   if(FTemp != nullptr)
   {
      // dbg (LOG_DEBUG,"Successfully called fopen with access mode '%s' on
      // '%s'",AccessMode,FilePlusOutputDir);

      if(AlreadyHaveThisFile(FilePlusOutputDir) == 0)
         CopyString(
            MaxFilesToTrack[FileToTrackCount++],
            FilePlusOutputDir,
            0,
            0,
            strlen(FilePlusOutputDir),
            MAX_PATH_SIZE,
            MAX_PATH_SIZE
         );

      return FTemp;
   }

   dbg(LOG_ERROR, "Calling fopen with with access mode '%s' on '%s was NOT successful' ", AccessMode, FilePlusOutputDir);
   ExitAndShowLog(1);

   return NULL;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleFirmwareWrite()
{
   firehose_error_t ReturnValue;
   ConvertProgram2Firmware = 1;
   dbg(LOG_INFO, "In handleFirmwareWrite, passing control to handleProgram()");
   ReturnValue = handleProgram();

   if(ReturnValue == FIREHOSE_SUCCESS)
   {
      dbg(LOG_INFO, "\n");
      dbg(LOG_INFO, "                      (firmware updated)");
      dbg(
         LOG_INFO,
         "______ ______________  ____    _  ___  ______ _____   _   "
         "_____________  ___ _____ ___________ "
      );
      dbg(
         LOG_INFO,
         "|  ___|_   _| ___ \\  \\/  | |  | |/ _ \\ | ___ \\  ___| "
         "| | | | ___ \\  _  \\/ _ \\_   _|  ___|  _  \\"
      );
      dbg(
         LOG_INFO,
         "| |_    | | | |_/ / .  . | |  | / /_\\ \\| |_/ / |__   | "
         "| | | |_/ / | | / /_\\ \\| | | |__ | | | |"
      );
      dbg(
         LOG_INFO,
         "|  _|   | | |    /| |\\/| | |/\\| |  _  ||    /|  __|  | "
         "| | |  __/| | | |  _  || | |  __|| | | |"
      );
      dbg(
         LOG_INFO,
         "| |    _| |_| |\\ \\| |  | \\  /\\  / | | || |\\ \\| |___ "
         " | |_| | |   | |/ /| | | || | | |___| |/ / "
      );
      dbg(
         LOG_INFO,
         "\\_|    \\___/\\_| \\_\\_|  |_/\\/  \\/\\_| |_/\\_| \\_\\____/   "
         "\\___/\\_|   |___/ \\_| |_/\\_/ \\____/|___/\n\n"
      );
   }

   return ReturnValue;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleProgram()
{
   SIZE_T FileSize = 0, FileSizeNumSectors = 0, localBytesRead = 0, NumSectors = 0, i, TotalSectorsFlashed = 0;
   char* FileToSendWithPath = nullptr;
   // char FileToReadBackWithPath[MAX_PATH_SIZE];
   char MyTempString[1024] = {0};
   std::shared_ptr<std::fstream> fd;
   struct timeval time_start, time_end, AbsoluteTimeStart;
   struct timeval network_start;
   double NetworkElapsed = 0.0;
   char* pch;
   SIZE_T TempBufferSize = 0, TempLength = 0;
   const char* OperationString[] = {"program", "firmwarewrite", "read"};
   char Index = 0; // defaults to program

   boolean isSparseFile = FALSE;
   char temp_num[20];
   struct sparse_read_info_t sparse_read_handle;
   struct sparse_header_t sparse_header = {};
   int32_t result;
   char local_temp_buffer[2048] = {0};
   int32_t pre_verify_programming = 0;

   if(ConvertProgram2Firmware)
   {
      Index = 1;
   }

   for(i = 0; i < static_cast<SIZE_T>(strlen(fh.attrs.filename)); i++)
   {
      if(IsEmptySpace(fh.attrs.filename[i])) fh.attrs.filename[i] = '\0';
   }

   if(strlen(fh.attrs.filename) == 0) return FIREHOSE_SUCCESS;


   if(num_filter_files > 0)
   {
      if(!ThisFileIsInFilterFiles(fh.attrs.filename))
      {
         dbg(
            LOG_DEBUG,
            "'%s' is being SKIPPED since it was not in --files provided by "
            "the user at the command line",
            fh.attrs.filename
         );
         return FIREHOSE_SUCCESS;
      }
   }

   if(num_filter_not_files > 0)
   {
      if(ThisFileIsInNotFilterFiles(fh.attrs.filename))
      {
         dbg(
            LOG_INFO,
            "handleProgram() '%s' is being SKIPPED since it was in --notfiles "
            "provided by the user at the command line",
            fh.attrs.filename
         );
         return FIREHOSE_SUCCESS;
      }
   }

   if(createconfigxml || SimulateForFileSize || verify_build)
   {
      // don't need to print anything, it's just messy
   }
   else
      dbg(LOG_INFO, "In handleProgram('%s')",
          fh.attrs.filename); // so user knows what were doing

   pch = strstr(tx_buffer, "sparse=\"true\"");

   if(pch != nullptr)
   {
      isSparseFile = TRUE;
      dbg(LOG_INFO, "%s is a sparse image", fh.attrs.filename);
   }

   // to be this far we are in RAW MODE. Need to transfer the file
   FileToSendWithPath = find_file(fh.attrs.filename, 1);

   if(FileToSendWithPath == NULL)
   {
      // File doesn't exist, BUT, if we are flattening a build, just WARN the
      // user, don't bail
      if(FlattenBuild || verify_build)
      {
         dbg(
            LOG_WARN,
            "'%s' not found. You will need --notfiles=%s when loading this "
            "build\n\n",
            fh.attrs.filename,
            fh.attrs.filename
         );
         return FIREHOSE_SUCCESS;
      }
      else
      {
         if(!PromptUser)
         {
            dbg(
               LOG_ERROR,
               "Software image: '%s' could not be found in the build path. "
               "You could possibly try --notfiles=%s,OtherFileToSkip.bin "
               "(note, exiting since you specified --noprompt)",
               fh.attrs.filename,
               fh.attrs.filename
            );
            ExitAndShowLog(1);
         }
         else
         {
            dbg(
               LOG_WARN,
               "'%s' not found. You will need --notfiles=%s when loading this "
               "build\n"
               "OR, you need to provide a --search_path to this file\n\n",
               fh.attrs.filename,
               fh.attrs.filename
            );
            return FIREHOSE_SUCCESS;
         }
      }
   }

   if(fh.attrs.SECTOR_SIZE_IN_BYTES != SectorSizeInBytes)
   {
      SectorSizeInBytes = fh.attrs.SECTOR_SIZE_IN_BYTES;
      dbg(LOG_INFO, "SectorSizeInBytes changed to %d", SectorSizeInBytes);
   }

   fd = ReturnFileHandle(FileToSendWithPath, MAX_PATH_SIZE,
                         "rb"); // will exit if not successful

   if(isSparseFile)
   {
      boolean num_conversion;
      pch = strstr(tx_buffer,
                   "UNSPARSE_FILE_SIZE=\"");           // go to unsparse file size
      pch = strstr(&tx_buffer[pch - tx_buffer], "\""); // find terminating area
      pch += 1;                                        // gets rid of the first "
      // get the actual numbers from file size
      i = 0;

      while(pch[i] != '"')
      {
         temp_num[i] = pch[i];
         i++;
      }

      temp_num[i] = '\0'; // terminate the string
      // FileSize = strtoull (temp_num, NULL, 10); //convert to uint64_t
      FileSize = stringToNumber((const char*)temp_num, &num_conversion);
      if(FALSE == num_conversion)
      {
         dbg(LOG_INFO, "Call to stringToNumber failed for FileSize with value '%s'", temp_num);
         ExitAndShowLog(1);
      }
   }
   else
   {
      FileSize = ReturnFileSize(fd);
   }

   if(FileSize == 0)
   {
      dbg(LOG_ERROR, "Filesize is 0 bytes. This is usually a mistake!! Please check '%s'", FileToSendWithPath);
      ExitAndShowLog(1);
   }

   BuildSizeTransferSoFar += FileSize; // BuildSizeTransferSoFar is manually
                                       // reset before starting all of this

   if(SimulateForFileSize)
   {
      fh_fclose(fd);
      return FIREHOSE_SUCCESS; // above we know the file exists and we got the
                               // build size, now return
   }

   if(FlattenBuild)
   {
      // to be here we are just copying the file to a local directory, and not
      // actually doing the <program>

      // user provided this path, therefore save files into it
      memset(flattenbuildto, 0x0, MAX_STRING_SIZE);

      if(CopyString(
            flattenbuildto,
            flattenbuildvariant,
            0,
            0,
            strlen(flattenbuildvariant),
            MAX_STRING_SIZE,
            MAX_STRING_SIZE
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildto",
            flattenbuildvariant,
            strlen(flattenbuildvariant)
         );
         ExitAndShowLog(1);
      }

      if(CopyString(
            flattenbuildto,
            fh.attrs.filename,
            strlen(flattenbuildto),
            0,
            strlen(fh.attrs.filename),
            MAX_STRING_SIZE,
            strlen(fh.attrs.filename)
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildto",
            fh.attrs.filename,
            strlen(fh.attrs.filename)
         );
         ExitAndShowLog(1);
      }
      result = MyCopyFile(FileToSendWithPath, flattenbuildto);
      if(result == -1) // this flatten build is for files possibly exluded by
                       // --notfiles --files etc
      {
         dbg(LOG_ERROR, "Failed to copy '%s'\n\t\t\tto '%s'\n\n", FileToSendWithPath, flattenbuildto);
         ExitAndShowLog(1);
      }
      else if(result == 0)
         dbg(LOG_INFO, "Copied '%s'\n\t\t\tto '%s'\n\n", FileToSendWithPath, flattenbuildto);

      if(showpercentagecomplete && TotalTransferSize > 0)
      {
         PercentageBuildLoaded = (float)((100.0 * (double)BuildSizeTransferSoFar) / (double)TotalTransferSize);
         if(PercentageBuildLoaded - this->PrePercentageBuildLoaded >= 1.0)
         {
            dbg(LOG_INFO, "{percent files transferred %6.2f%%}", PercentageBuildLoaded);
            // notify when progression alteration is greater than 1%
            notifyProgression(PercentageBuildLoaded);
            this->PrePercentageBuildLoaded = PercentageBuildLoaded;
         }
      }

      fh_fclose(fd);
      return FIREHOSE_SUCCESS;
   }

   if(createconfigxml)
   {
      fh_fclose(fd);
      return FIREHOSE_SUCCESS; // above we know the file exists and we got the
                               // build size, now return
   }

   if(verify_build)
   {
      fh_fclose(fd);
      return FIREHOSE_SUCCESS; // above we know the file exists and we got the
                               // build size, now return
   }

   FileSizeNumSectors = FileSize / SectorSizeInBytes;

   if(FileSize % SectorSizeInBytes)
   {
      FileSizeNumSectors++;
   }


   dbg(LOG_DEBUG, "File size is %" SIZE_T_FORMAT " bytes", FileSize);
   dbg(LOG_DEBUG, "NumSectors needed %" SIZE_T_FORMAT, FileSizeNumSectors);

   if(FileSizeNumSectors == 0)
   {
      dbg(LOG_ERROR, "FileSizeNumSectors is 0. Nothing to program!!");
      ExitAndShowLog(1);
   }


   if(fh.attrs.num_partition_sectors == 0)
   {
      // this is usually a mistake. Something like last partition is set to grow
      // and thus size was set to 0
      fh.attrs.num_partition_sectors = FileSizeNumSectors;
   }

   if(FileSizeNumSectors > fh.attrs.num_partition_sectors)
   {
      char FileSize1[2048], FileSize2[2048];

      ReturnSizeString(fh.attrs.num_partition_sectors * SectorSizeInBytes, FileSize1, sizeof(FileSize1));
      ReturnSizeString(FileSize, FileSize2, sizeof(FileSize2));

      dbg(
         LOG_WARN,
         "User requested to ONLY program num_partition_sectors %" SIZE_T_FORMAT " of '%s' (%s).\n"
         "To load this file completely would require %" SIZE_T_FORMAT
         " sectors (%s) and SectorSizeInBytes=%" SIZE_T_FORMAT "."
         "\nThis is usually a MISTAKE but there is a use case where you want "
         "to write only part of the image!\n",
         fh.attrs.num_partition_sectors,
         fh.attrs.filename,
         FileSize1,
         FileSizeNumSectors,
         FileSize2,
         SectorSizeInBytes
      );
      FileSizeNumSectors = fh.attrs.num_partition_sectors;
      // FileToSendWithPath
   }


   // NOTE: Can't send <program> tag as is since num_partition_sectors is most
   // likely BIGGER than the filesize tx_buffer already holds the XML file but
   // we need to modify it and we'll use temp_buffer for this
   // 1. Need to put my own TAG
   // 2. Need to put my own num_partition_sectors

   memset(temp_buffer, 0x0, FIREHOSE_TX_BUFFER_SIZE); // zero out to begin

   pch = strstr(tx_buffer, "<program");

   if(pch == NULL)
   {
      dbg(LOG_ERROR, "missing TAG '<program ??? Should be impossible'\n");
      ExitAndShowLog(1);
   }

   TempLength = pch - tx_buffer;

   CopyString(temp_buffer, tx_buffer, 0, 0, TempLength, FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);

   if(ConvertProgram2Firmware)
      AppendToBuffer(temp_buffer, "<firmwarewrite ", FIREHOSE_TX_BUFFER_SIZE);
   else
      AppendToBuffer(temp_buffer, "<program ", FIREHOSE_TX_BUFFER_SIZE);

   TempLength += strlen("<program "); // need to know where this ends

   pch = strstr(tx_buffer, "num_partition_sectors");

   if(pch == NULL)
   {
      dbg(LOG_ERROR, "<%s> tag is missing attribute 'num_partition_sectors'\n", OperationString[Index]);
      ExitAndShowLog(1);
   }

   if((SIZE_T)(pch - tx_buffer) > TempLength)
   {
      // to be here means there were more attributes between "<program "  and
      // "num_partition_sectors"
      CopyString(
         temp_buffer,
         tx_buffer,
         strlen(temp_buffer),
         TempLength,
         (pch - tx_buffer) - TempLength,
         FIREHOSE_TX_BUFFER_SIZE,
         FIREHOSE_TX_BUFFER_SIZE
      );
   }

   TempBufferSize = strlen(temp_buffer);

   // CopyString(temp_buffer, tx_buffer, strlen(temp_buffer), pch-tx_buffer,
   // TempBufferSize, FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);
   sprintf(
      &temp_buffer[TempBufferSize],
      "num_partition_sectors=\"%" SIZE_T_FORMAT "\"",
      FileSizeNumSectors
   ); // no space at end here!!

   pch = strstr(&tx_buffer[pch - tx_buffer],
                "\""); // looking for " in num_partition_sectors="1234"

   if(pch == NULL)
   {
      dbg(LOG_ERROR, "<%s> tag is missing first \" after 'num_partition_sectors'\n", OperationString[Index]);
      ExitAndShowLog(1);
   }

   TempBufferSize = pch - tx_buffer + 1; // +1 gets us past the first "

   pch = strstr(&tx_buffer[TempBufferSize],
                "\""); // looking for " in num_partition_sectors="1234"

   if(pch == NULL)
   {
      dbg(LOG_ERROR, "<%s> tag is missing second \" after 'num_partition_sectors'\n", OperationString[Index]);
      ExitAndShowLog(1);
   }

   TempBufferSize = pch - tx_buffer + 1; // +1 gets us past the second "

   AppendToBuffer(temp_buffer, &tx_buffer[TempBufferSize], FIREHOSE_TX_BUFFER_SIZE);

   memscpy(tx_buffer, FIREHOSE_TX_BUFFER_SIZE, temp_buffer,
           FIREHOSE_TX_BUFFER_SIZE); // memcpy

   /*
     InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);

     // HACK, to match Zeno
     CopyString(tx_buffer, "<?xml version=\"1.0\" ?>\n", 0, 0, strlen("<?xml
     version=\"1.0\" ?>\n"), FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);

       AppendToBuffer(tx_buffer,"<data>\n",FIREHOSE_TX_BUFFER_SIZE);
       AppendToBuffer(tx_buffer,"  <program ",FIREHOSE_TX_BUFFER_SIZE);  //
     HACK, 2 spaces are to match Zeno

       sprintf(temp_buffer,"SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\"
     ",fh.attrs.SECTOR_SIZE_IN_BYTES);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"filename=\"dummy64.bin\" label=\"single_file\" ");
     // HACK, DeviceProg doesn't require this info
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"num_partition_sectors=\"%" SIZE_T_FORMAT "\"
     ",FileSizeNumSectors);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"physical_partition_number=\"%" SIZE_T_FORMAT "\"
     ",fh.attrs.physical_partition_number);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       //sprintf(temp_buffer,"start_sector=\"%s\" ",fh.attrs.start_sector);
     sprintf(temp_buffer,"start_sector=\"%s\"",fh.attrs.start_sector); // HACK,
     in Zeno I don't have a space at the end
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       AppendToBuffer(tx_buffer,"/>\n</data>\n",FIREHOSE_TX_BUFFER_SIZE);  //
     HACK, \n is for Zeno
   */

   if(UsingValidation && skipharddriveread)
      sprintf(
         MyTempString,
         " **Using VIP**\n\t\t** NOTE: skipharddriveread=1 "
         "(will use accurate filesize but all 0xFF data)"
      );
   else if(skipharddriveread)
      sprintf(
         MyTempString,
         "\n\t\t** NOTE: skipharddriveread=1 (will use "
         "accurate filesize but all 0xFF data)"
      );
   else if(UsingValidation)
      sprintf(MyTempString, " **Using VIP**");

   dbg(LOG_ALWAYS, "=======================================================");
   dbg(LOG_ALWAYS, "{<%s> FILE: '%s'%s}", OperationString[Index], FileToSendWithPath, MyTempString);

   ReturnSizeString(FileSize, MyTempString, sizeof(MyTempString));

   dbg(
      LOG_ALWAYS,
      "{<%s> (%s) Partition image size: %" SIZE_T_FORMAT ", Partition max size: %" SIZE_T_FORMAT ", %" SIZE_T_FORMAT
      " sectors needed at location %i on LUN %" SIZE_T_FORMAT "}",
      OperationString[Index],
      MyTempString,
      FileSize,
      fh.attrs.num_partition_sectors * fh.attrs.SECTOR_SIZE_IN_BYTES,
      FileSizeNumSectors,
      atoi(fh.attrs.start_sector),
      fh.attrs.physical_partition_number
   );
   dbg(LOG_ALWAYS, "=======================================================\n");

   if(ValidateImageSize)
   {
      if(FileSize > fh.attrs.num_partition_sectors * fh.attrs.SECTOR_SIZE_IN_BYTES)
      {
         dbg(
            LOG_ERROR,
            ". Partition image size: %" SIZE_T_FORMAT " exceeds partition max size %" SIZE_T_FORMAT
            " from partition xml. Verify partition image size from build",
            FileSize,
            fh.attrs.num_partition_sectors * fh.attrs.SECTOR_SIZE_IN_BYTES
         );
         ExitAndShowLog(1);
      }
   }

   // tx_buffer already holds the XML file
   notifyMessage(FirehoseEvent::EventId::FIREHOSE_TRANSFER_RAW_PROGRAMMER, tx_buffer);
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" rawmode=\"true\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(!GotACK)
   {
      dbg(LOG_ERROR, "%s FAILED - Please see log", OperationString[Index]);
      ExitAndShowLog(1);
   }


   // fread(EntireXMLFileBuffer, XMLFileSize, 1, fd->get());
   dbg(LOG_INFO, "\n");

   gettimeofday(&time_start, NULL);
   AbsoluteTimeStart = time_start;

   // Initialize SHA256
   if(verify_programming && !VerifySha256File)
   {
      pre_verify_programming = 1;
   }

   if(pre_verify_programming || ShowDigestPerFile)
   {
      SizeOfDataFedToHashRoutine = 0; // reset this
      sechsharm_sha256_init(&context);
   }

   RawMode = 1;

   // Request from corporate tools is to show percentage complete per packet
   BuildSizeTransferSoFar -= FileSize; // this undoes above, will += this below again

   if(isSparseFile)
   {
      sparse_header = sparse_read_handle_init(fd, fh.attrs.file_sector_offset, FileSize, &sparse_read_handle);
   }
   else
   {
      fd->seekg(
         fh.attrs.file_sector_offset * fh.attrs.SECTOR_SIZE_IN_BYTES,
         std::ios::beg
      ); // seek to the correct position in the file
   }
   while(FileSizeNumSectors > 0)
   {
      gettimeofday(&network_start, NULL);

      if(!skipharddriveread)
      {
         if(isSparseFile)
         {
            localBytesRead =
               sparse_read(tx_buffer, fh.attrs.MaxPayloadSizeToTargetInBytes, fd, &sparse_read_handle, sparse_header);
         }
         else
         {
            fd->read(
               tx_buffer,
               fh.attrs.MaxPayloadSizeToTargetInBytes
            ); // read from hard
               // drive
            localBytesRead = fd->gcount();
         }
      }
      else
      {
         localBytesRead = fh.attrs.MaxPayloadSizeToTargetInBytes; // fake it
         memset(tx_buffer, 0xFF, FIREHOSE_TX_BUFFER_SIZE);
      }

      if(localBytesRead == 0)
      {
         dbg(LOG_ERROR, "Read 0 bytes from '%s'", FileToSendWithPath);
         dbg(LOG_ERROR, "There are still %" SIZE_T_FORMAT " sectors to go", FileSizeNumSectors);
         ExitAndShowLog(1);
      }

      BuildSizeTransferSoFar += localBytesRead;

      gettimeofday(&time_end, NULL);

      NumSectors = localBytesRead / fh.attrs.SECTOR_SIZE_IN_BYTES;

      if(localBytesRead % fh.attrs.SECTOR_SIZE_IN_BYTES)
      {
         // Need to zero pad to fill last sector
         NumSectors++;

         // fh.attrs.SECTOR_SIZE_IN_BYTES             = 512
         //(localBytesRead%fh.attrs.SECTOR_SIZE_IN_BYTES) = 168
         //  Therefore need 512-168=344 zeros

         for(i = 0; i < fh.attrs.SECTOR_SIZE_IN_BYTES - (localBytesRead % fh.attrs.SECTOR_SIZE_IN_BYTES); i++)
            tx_buffer[i + localBytesRead] = 0; // zero padding to 1 full sector
      }

      // Now Send tx_buffer
      if(NumSectors > FileSizeNumSectors)
         NumSectors = FileSizeNumSectors; // we read more file than user wants
                                          // to send, so truncate to this


      TotalSectorsFlashed += NumSectors;
      NetworkElapsed += ReturnTimeInSeconds(&network_start, &time_end);
      ReturnSizeString(
         (SIZE_T)(TotalSectorsFlashed * fh.attrs.SECTOR_SIZE_IN_BYTES),
         temp_buffer,
         FIREHOSE_TX_BUFFER_SIZE
      );
      // dbg (LOG_INFO, "\n%s read in
      // NetworkElapsed=%f",temp_buffer,NetworkElapsed);

      if(ReturnTimeInSeconds(&time_start, &time_end) >= 2.0)
      {
         time_throughput_calculate(
            &AbsoluteTimeStart,
            &time_end,
            TotalSectorsFlashed * fh.attrs.SECTOR_SIZE_IN_BYTES,
            NetworkElapsed,
            LOG_INFO
         );
         gettimeofday(&time_start, NULL);

         if(VerboseLevel == LOG_ALWAYS) // this means user wants minimal logging
         {
            // dbg (LOG_INFO, ".");
         }
      }

      if(sendTransmitBufferBytes(NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES) == 0)
      {
         fh_fclose(fd);
         return FIREHOSE_ERROR;
      }

      FileSizeNumSectors -= NumSectors;

      if(pre_verify_programming || ShowDigestPerFile)
      {
         SizeOfDataFedToHashRoutine += (NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES);
         sechsharm_sha256_update(
            &context,
            context.leftover,
            &(context.leftover_size),
            (uint8_t*)tx_buffer,
            static_cast<uint32_t>(NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES)
         );
      }

   } // end of while(FileSizeNumSectors>0)

   fh_fclose(fd);
   fd = nullptr;

   RawMode = 0; // we are out, treat all incoming data now as XML again

   gettimeofday(&time_end, NULL);

   FileSizeNumSectors = FileSize / SectorSizeInBytes;

   if(FileSize % SectorSizeInBytes)
   {
      FileSizeNumSectors++;
   }

   if(FileSizeNumSectors > fh.attrs.num_partition_sectors) FileSizeNumSectors = fh.attrs.num_partition_sectors;

   time_throughput_calculate(
      &AbsoluteTimeStart,
      &time_end,
      FileSizeNumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES,
      -1.0,
      LOG_ALWAYS
   );

   // Finalize SHA256

   if(pre_verify_programming || ShowDigestPerFile)
   {
      sechsharm_sha256_final(&context, context.leftover, &(context.leftover_size), verify_hash_value);
      PrettyPrintHexValueIntoTempBuffer(verify_hash_value, 32, 0,
                                        32); // from, size, offset, maxlength
      dbg(LOG_INFO, "\n");
      dbg(
         LOG_INFO,
         "verify_programming '%s'\nSHA256 (%7lld bytes) %s\n",
         FileToSendWithPath,
         SizeOfDataFedToHashRoutine,
         temp_buffer
      );

      if(ShowDigestPerFile)
      {
         dbg(
            LOG_INFO,
            "'%s' --> LUN %" SIZE_T_FORMAT " start_sector %s SHA256 (%" SIZE_T_FORMAT " bytes) %s\n",
            FileToSendWithPath,
            fh.attrs.physical_partition_number,
            fh.attrs.start_sector,
            SizeOfDataFedToHashRoutine,
            temp_buffer
         );
      }
      if(GenerateSha256File)
      {
         fd = ReturnFileHandle(DigestsPerFileName, strlen(DigestsPerFileName),
                               "ab"); // will exit if not successful
         if(fd != nullptr)
         {
            fd->write(reinterpret_cast<const char*>(verify_hash_value), 32);
            fh_fclose(fd);
            fd = nullptr;
            return FIREHOSE_SUCCESS;
         }
         dbg(LOG_ERROR, "opening the file you specified with '%s' could not be found", DigestsPerFileName);
         ExitAndShowLog(1);
      }
   }

   // dbg (LOG_INFO, "\n");

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" rawmode=\"false\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   // fflush (fp); // flush to port_trace.txt

   GetNextPacket(); // this will set all variables, including GotACK

   if(!GotACK)
   {
      dbg(LOG_ERROR, "Please see log");
      ExitAndShowLog(1);
   }


   // =========================================== VERIFY PROGRAMMING HAPPENS
   // HERE IF REQUESTED ====================================
   // =========================================== VERIFY PROGRAMMING HAPPENS
   // HERE IF REQUESTED ====================================
   // =========================================== VERIFY PROGRAMMING HAPPENS
   // HERE IF REQUESTED ====================================

   if(wipefirst &&
      strncmp(fh.attrs.filename, WipeFirstFileName, MAX(strlen(fh.attrs.filename), strlen(WipeFirstFileName))) == 0)
   {
      // to be here means this is the Zeros16KB.bin file I write with
      // --wipefirst so do nothing with regards to verify_programming, i.e. I
      // don't want to read back the file of all zeros
   }
   else if(verify_programming)
   {
      if(verify_programming_sha256 == 0)
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<read ", FIREHOSE_TX_BUFFER_SIZE);

         // There can be no path on this name
         sprintf(temp_buffer, "filename=\"%s\" ", fh.attrs.filename);

         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ", fh.attrs.SECTOR_SIZE_IN_BYTES);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ", FileSizeNumSectors);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "physical_partition_number=\"%" SIZE_T_FORMAT "\" ", fh.attrs.physical_partition_number);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         sprintf(temp_buffer, "start_sector=\"%s\" ", fh.attrs.start_sector);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "slot=\"%" SIZE_T_FORMAT "\" ", fh.attrs.slot);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

         memscpy(temp_buffer3, FIREHOSE_TX_BUFFER_SIZE, tx_buffer,
                 strlen(tx_buffer)); // memcpy

         SendXMLString(temp_buffer3, strlen(temp_buffer3));

         if(Simulate == 0) /* We are generating VIP tables, no need to verify the hash. */
         {
            /* fh.attrs.filename assumes images are in cwd so pick the correct
             * image path to calculate sha */
            std::string temp;
            temp.append(MainOutputDir);
            temp.append(fh.attrs.filename);
            dbg(LOG_INFO, "********** for '%s'********", temp.c_str());
            fd = ReturnFileHandle((char*)temp.c_str(), MAX_PATH_SIZE,
                                  "rb"); // will exit if not successful
            FileSize = ReturnFileSize(fd);

            if(FileSize == 0)
            {
               // dbg (LOG_ERROR, "Filesize is 0 bytes for '%s'. Previous <read>
               // failed??",FileToReadBackWithPath);
               dbg(LOG_ERROR, "Filesize is 0 bytes for '%s'. Previous <read> failed??", fh.attrs.filename);
               ExitAndShowLog(1);
            }


            // Initialize SHA256
            SizeOfDataFedToHashRoutine = 0; // reset this
            sechsharm_sha256_init(&context);

            memset(tx_buffer, 0x0, FIREHOSE_TX_BUFFER_SIZE);

            dbg(LOG_INFO, "Calculating SHA256");

            while(FileSize > FIREHOSE_TX_BUFFER_SIZE)
            {
               fd->read(tx_buffer,
                        FIREHOSE_TX_BUFFER_SIZE); // read from hard drive
               localBytesRead = fd->gcount();
               SizeOfDataFedToHashRoutine += (SIZE_T)FIREHOSE_TX_BUFFER_SIZE;
               sechsharm_sha256_update(
                  &context,
                  context.leftover,
                  &(context.leftover_size),
                  (uint8_t*)tx_buffer,
                  FIREHOSE_TX_BUFFER_SIZE
               );
               FileSize -= FIREHOSE_TX_BUFFER_SIZE;
               // dbg (LOG_INFO, ".");
            }

            // get what is left
            fd->read(tx_buffer, FileSize); // read from hard drive
            localBytesRead = fd->gcount();
            SizeOfDataFedToHashRoutine += FileSize;
            sechsharm_sha256_update(
               &context,
               context.leftover,
               &(context.leftover_size),
               (uint8_t*)tx_buffer,
               static_cast<uint32_t>(FileSize)
            );
            if(verify_programming && (FileSize % fh.attrs.SECTOR_SIZE_IN_BYTES))
            {
               dbg(LOG_ERROR, "File size is not a multiple of sector size");
               ExitAndShowLog(1);
            }

            fh_fclose(fd);
            fd = nullptr;


            sechsharm_sha256_final(&context, context.leftover, &(context.leftover_size), last_hash_value);
            PrettyPrintHexValueIntoTempBuffer(
               last_hash_value,
               32,
               0,
               32
            ); // from, size, offset,
               // maxlength
            dbg(
               LOG_INFO,
               "verify_programming Read FROM TARGET '%s'\nSHA256 "
               "(%7" SIZE_T_FORMAT " bytes) %s\n",
               fh.attrs.filename,
               SizeOfDataFedToHashRoutine,
               temp_buffer
            );
         }
      }
      else /* Use getsha256digest command to get the hash of the data in disk.
            */
      {
         InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);
         AppendToBuffer(tx_buffer, "<getsha256digest ", FIREHOSE_TX_BUFFER_SIZE);

         snprintf_x(
            temp_buffer,
            FIREHOSE_TX_BUFFER_SIZE,
            "SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\" ",
            fh.attrs.SECTOR_SIZE_IN_BYTES
         );
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         snprintf_x(
            temp_buffer,
            FIREHOSE_TX_BUFFER_SIZE,
            "num_partition_sectors=\"%" SIZE_T_FORMAT "\" ",
            FileSizeNumSectors
         );
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         snprintf_x(
            temp_buffer,
            FIREHOSE_TX_BUFFER_SIZE,
            "physical_partition_number=\"%" SIZE_T_FORMAT "\" ",
            fh.attrs.physical_partition_number
         );
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "start_sector=\"%s\" ", fh.attrs.start_sector);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         snprintf_x(temp_buffer, FIREHOSE_TX_BUFFER_SIZE, "slot=\"%" SIZE_T_FORMAT "\" ", fh.attrs.slot);
         AppendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);

         AppendToBuffer(tx_buffer, "/>\n</data>", FIREHOSE_TX_BUFFER_SIZE);

         memscpy(temp_buffer3, FIREHOSE_TX_BUFFER_SIZE, tx_buffer,
                 strlen(tx_buffer)); // memcpy
         SendXMLString(temp_buffer3, strlen(temp_buffer3));

         if(Simulate == 0) /* We are generating VIP tables, no need to verify the hash. */
         {
            dbg(LOG_INFO, "verify_programming computed FROM TARGET\n");
            for(i = 0; i < 32; i++)
            {
               char byte[3];
               uint64_t val;
               byte[0] = fh.attrs.sha256_verify[i * 2];
               byte[1] = fh.attrs.sha256_verify[i * 2 + 1];
               byte[2] = 0;
               if(hex_atoi(byte, &val) == 0)
               {
                  last_hash_value[i] = val & 0xFF;
               }
               else
               {
                  memset(last_hash_value, 0xEE, sizeof(last_hash_value));
                  dbg(LOG_ERROR, "Invalid sha string received from target %s\n", fh.attrs.sha256_verify);
               }
            }
            PrettyPrintHexValueIntoTempBuffer(
               last_hash_value,
               32,
               0,
               32
            ); // from, size, offset,
               // maxlength
            dbg(
               LOG_INFO,
               "verify_programming_sha256 Read FROM TARGET '%s'\nSHA256 %s\n",
               fh.attrs.filename,
               temp_buffer
            );
         }
      }
      if(VerifySha256File)
      {
         ReadSha256File();
         PrettyPrintHexValueIntoTempBuffer(
            verify_hash_value,
            32,
            0,
            32
         ); // from, size, offset,
            // maxlength
         dbg(
            LOG_INFO,
            "verify_programming read SHA256 from '%s'\n'%s'SHA256 : %s\n",
            DigestsPerFileName,
            fh.attrs.filename,
            temp_buffer
         );
      }
      if(Simulate)
      {
         memscpy(last_hash_value, sizeof(last_hash_value), verify_hash_value, sizeof(verify_hash_value));
      }

      for(i = 0; i < 32; i++)
      {
         if(verify_hash_value[i] != last_hash_value[i]) break;
      }

      if(i == 32)
      {
         // dbg (LOG_INFO, "MATCHED - '%s'\n",AllAttributes[i].Name);
         dbg(LOG_ALWAYS, "  __ _           _     _            ");
         dbg(LOG_ALWAYS, " / _| |         | |   (_)            ");
         dbg(LOG_ALWAYS, "| |_| | __ _ ___| |__  _ _ __   __ _ ");
         dbg(LOG_ALWAYS, "|  _| |/ _` / __| '_ \\| | '_ \\ / _` |");
         dbg(LOG_ALWAYS, "| | | | (_| \\__ \\ | | | | | | | (_| |");
         dbg(LOG_ALWAYS, "|_| |_|\\__,_|___/_| |_|_|_| |_|\\__, |");
         dbg(LOG_ALWAYS, "                                __/ |");
         dbg(LOG_ALWAYS, "                               |___/ ");
         dbg(LOG_ALWAYS, "                    _                ");
         dbg(LOG_ALWAYS, "                   | |               ");
         dbg(LOG_ALWAYS, "__      _____  _ __| | _____     ");
         dbg(LOG_ALWAYS, "\\ \\ /\\ / / _ \\| '__| |/ / __|        ");
         dbg(LOG_ALWAYS, " \\ V  V / (_) | |  |   <\\__ \\        ");
         dbg(LOG_ALWAYS, "  \\_/\\_/ \\___/|_|  |_|\\_\\___/        \n");
         dbg(
            LOG_ALWAYS,
            "FLASHING WORKS. This tool *is* able to successfully "
            "flash data onto this device\n\n"
         );

         if(1 == verify_programming && 0 == CreateDigests)
         {
            /* no point in keeping readback images if flashing is success so to
             * save space delete it */
            snprintf_x(local_temp_buffer, MAX_PATH_SIZE, "%s%s", MainOutputDir, fh.attrs.filename);
            /* check if readback file exists or not before deleting it */
            try
            {
               std::error_code errorCode;
               std::filesystem::remove(local_temp_buffer, errorCode);
               if(errorCode)
               {
                  dbg(
                     LOG_ALWAYS,
                     "Failed to deleted the read back file '%s': %s",
                     local_temp_buffer,
                     errorCode.message().c_str()
                  );
                  ExitAndShowLog(1);
               }
               dbg(LOG_ALWAYS, "Just deleted the read back file '%s'", local_temp_buffer);
            }
            TOOLS_CATCH(e, dbg(LOG_WARN, "unable to delete the read back file '%s': %s", local_temp_buffer, e.what()))
         }
      }
      else
      {
         dbg(LOG_ALWAYS, "  __ _           _     _            ");
         dbg(LOG_ALWAYS, " / _| |         | |   (_)            ");
         dbg(LOG_ALWAYS, "| |_| | __ _ ___| |__  _ _ __   __ _ ");
         dbg(LOG_ALWAYS, "|  _| |/ _` / __| '_ \\| | '_ \\ / _` |");
         dbg(LOG_ALWAYS, "| | | | (_| \\__ \\ | | | | | | | (_| |");
         dbg(LOG_ALWAYS, "|_| |_|\\__,_|___/_| |_|_|_| |_|\\__, |");
         dbg(LOG_ALWAYS, "                                __/ |");
         dbg(LOG_ALWAYS, "                               |___/ ");
         dbg(LOG_ALWAYS, "  __      _ _          _            ");
         dbg(LOG_ALWAYS, " / _|    (_) |        | |            ");
         dbg(LOG_ALWAYS, "| |_ __ _ _| | ___  __| |          ");
         dbg(LOG_ALWAYS, "|  _/ _` | | |/ _ \\/ _` |            ");
         dbg(LOG_ALWAYS, "| || (_| | | |  __/ (_| |            ");
         dbg(
            LOG_ALWAYS,
            "|_| \\__,_|_|_|\\___|\\__,_|                         "
            "          \n"
         );
         dbg(
            LOG_ALWAYS,
            "FLASHING FAILED. This tool was *NOT* able to flash "
            "data onto this device\n\n"
         );
         ExitAndShowLog(1);
      }
   } // end if(verify_programming)

   // =========================================== DONE VERIFY PROGRAMMING
   // ====================================
   // =========================================== DONE VERIFY PROGRAMMING
   // ====================================
   // =========================================== DONE VERIFY PROGRAMMING
   // ====================================

   /*
     dbg (LOG_INFO, "\n\nTESTING TIMING\n");
     gettimeofday(&time_start, NULL);
     sleep(5000);
     gettimeofday(&time_end, NULL);
     timeval_subtract(&time_result, &time_end, &time_start);
     TimeInSeconds = (double)time_result.tv_sec +
     (double)(time_result.tv_usec/1000000.0); dbg (LOG_INFO, "\n\nDONE TESTING
     TIMING TimeInSeconds=%f\n",TimeInSeconds);
   */


   if(!GotACK)
   {
      dbg(LOG_ERROR, "Please see log");
      ExitAndShowLog(1);
   }

   dbg(LOG_INFO, "");
   dbg(LOG_INFO, "=======================================================");
   dbg(LOG_INFO, "==================== {SUCCESS} ========================");
   dbg(LOG_INFO, "=======================================================\n\n");

   if(SimulateForFileSize == 0 && TotalTransferSize != 0)
   {
      if(TotalTransferSize > 0)
      {
         PercentageBuildLoaded = (float)((100.0 * (double)BuildSizeTransferSoFar) / (double)TotalTransferSize);
         if(PercentageBuildLoaded - this->PrePercentageBuildLoaded >= 1.0)
         {
            dbg(LOG_INFO, "{percent files transferred %6.2f%%}", PercentageBuildLoaded);
            // notify when progression alteration is greater than 1%
            notifyProgression(PercentageBuildLoaded);
            this->PrePercentageBuildLoaded = PercentageBuildLoaded;
         }
      }
   }


   // fflush (fp); // flush to port_trace.txt

   return FIREHOSE_SUCCESS;
}


FirehoseLoader::firehose_error_t FirehoseLoader::handleRead()
{
   SIZE_T FileSizeNumSectors = 0, FileSizeNumSectorsLeft = 0, NumSectors = 0, i, TotalSectorsFlashed = 0,
          WriteCount = 0;
   char* FileToSendWithPath = nullptr;
   std::shared_ptr<std::fstream> fr, fd;
   struct timeval time_start, time_end, AbsoluteTimeStart;
   SIZE_T FileSize = 0, TempBufferSize = 0;


   for(i = 0; i < static_cast<SIZE_T>(strlen(fh.attrs.filename)); i++)
   {
      if(IsEmptySpace(fh.attrs.filename[i])) fh.attrs.filename[i] = '\0';
   }

   if(strlen(fh.attrs.filename) == 0) return FIREHOSE_SUCCESS;


   if(num_filter_files > 0)
   {
      if(!ThisFileIsInFilterFiles(fh.attrs.filename))
      {
         dbg(
            LOG_DEBUG,
            "'%s' is being SKIPPED since it was not in --files provided by "
            "the user at the command line",
            fh.attrs.filename
         );
         return FIREHOSE_SUCCESS;
      }
   }

   if(num_filter_not_files > 0)
   {
      if(ThisFileIsInNotFilterFiles(fh.attrs.filename))
      {
         dbg(
            LOG_INFO,
            "handleRead() '%s' is being SKIPPED since it was in --notfiles "
            "provided by the user at the command line",
            fh.attrs.filename
         );
         return FIREHOSE_SUCCESS;
      }
   }

   if(createconfigxml || SimulateForFileSize || verify_build)
   {
      // don't need to print anything, it's just messy
   }
   else
   {
      dbg(LOG_INFO, "In handleRead('%s')", fh.attrs.filename);
   }

   // -----------------------------------------------------------------------------------------------------------------------------------------
   // Step 1 is to see if the original file exists, in that case, user probably
   // wants to compare to the original

   // There is no FileSize in this case since we can only read the number of
   // sectors the partition has
   FileSizeNumSectors = fh.attrs.num_partition_sectors;

   FileToSendWithPath = find_file(fh.attrs.filename, 1);

   if(FileToSendWithPath != nullptr)
   {
      if(fh.attrs.SECTOR_SIZE_IN_BYTES != SectorSizeInBytes)
      {
         SectorSizeInBytes = fh.attrs.SECTOR_SIZE_IN_BYTES;
         dbg(LOG_INFO, "SectorSizeInBytes changed to %d", SectorSizeInBytes);
      }

      fd = ReturnFileHandle(FileToSendWithPath, MAX_PATH_SIZE,
                            "rb"); // will exit if not successful

      FileSize = ReturnFileSize(fd);

      if(FileSize == 0)
      {
         FileSize = fh.attrs.num_partition_sectors * SectorSizeInBytes;
         dbg(
            LOG_WARN,
            "Previous Filesize is 0 bytes. Therefore reading size of "
            "partition!! Please check '%s'",
            FileToSendWithPath
         );
      }

      FileSizeNumSectors = FileSize / SectorSizeInBytes;

      if(FileSize % SectorSizeInBytes)
      {
         FileSizeNumSectors++;
      }

      if(TestIfSparse(fd)) FileSizeNumSectors = fh.attrs.num_partition_sectors;

      fh_fclose(fd);
      fd = nullptr;
      dbg(LOG_DEBUG, "File size is %" SIZE_T_FORMAT " bytes", FileSize);
      dbg(LOG_DEBUG, "NumSectors needed %" SIZE_T_FORMAT, FileSizeNumSectors);
   }

   // this might have been updated above
   FileSizeNumSectorsLeft = FileSizeNumSectors;

   // -----------------------------------------------------------------------------------------------------------------------------------------

   // to be this far we are in RAW MODE. Need to read the file
   std::string temp;
   temp.append(MainOutputDir);
   temp.append(fh.attrs.filename);

   // Create base path if not exists for reading images
   std::string basePath(temp);
   if(!std::filesystem::exists(std::filesystem::path(basePath.c_str()).parent_path()))
   {
      dbg(LOG_INFO, "Partition read path not found, creating it now: %s", basePath.c_str());
      Util::createPath(std::filesystem::path(basePath.c_str()).parent_path());
   }

   if(CreateDigests)
   {
      fr = ReturnFileHandle((char*)temp.c_str(), MAX_PATH_SIZE,
                            "rb"); // will exit if not successful
   }
   else
   {
      fr = ReturnFileHandle((char*)temp.c_str(), MAX_PATH_SIZE,
                            "wb"); // will exit if not successful
   }

   if(fh.attrs.SECTOR_SIZE_IN_BYTES != SectorSizeInBytes)
   {
      SectorSizeInBytes = fh.attrs.SECTOR_SIZE_IN_BYTES;
      dbg(LOG_INFO, "SectorSizeInBytes changed to %d", SectorSizeInBytes);
   }

   if(SimulateForFileSize)
   {
      BuildSizeTransferSoFar += FileSize; // BuildSizeTransferSoFar is manually
                                          // reset before starting all of this

      // Close file handle
      if(fr != nullptr)
      {
         fh_fclose(fr);
         fr = nullptr;
      }
      return FIREHOSE_SUCCESS; // above we know the file exists and we got the
                               // build size, now return
   }

   if(FileSize > 0)
   {
      // original file exists
      char* pch;

      memset(temp_buffer, 0x0, FIREHOSE_TX_BUFFER_SIZE); // zero out to begin

      pch = strstr(tx_buffer, "num_partition_sectors");

      if(pch == NULL)
      {
         dbg(LOG_ERROR, "<read> tag is missing attribute 'num_partition_sectors'\n");
         ExitAndShowLog(1);
      }

      CopyString(temp_buffer, tx_buffer, 0, 0, (pch - tx_buffer), FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);

      TempBufferSize = strlen(temp_buffer);

      // CopyString(temp_buffer, tx_buffer, strlen(temp_buffer), pch-tx_buffer,
      // TempBufferSize, FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);
      sprintf(
         &temp_buffer[TempBufferSize],
         "num_partition_sectors=\"%" SIZE_T_FORMAT "\"",
         FileSizeNumSectors
      ); // no space at end here!!

      pch = strstr(&tx_buffer[pch - tx_buffer],
                   "\""); // looking for " in num_partition_sectors="1234"

      if(pch == NULL)
      {
         dbg(LOG_ERROR, "<read> tag is missing first \" after 'num_partition_sectors'\n");
         ExitAndShowLog(1);
      }

      TempBufferSize = pch - tx_buffer + 1; // +1 gets us past the first "

      pch = strstr(&tx_buffer[TempBufferSize],
                   "\""); // looking for " in num_partition_sectors="1234"

      if(pch == NULL)
      {
         dbg(LOG_ERROR, "<read> tag is missing second \" after 'num_partition_sectors'\n");
         ExitAndShowLog(1);
      }

      TempBufferSize = pch - tx_buffer + 1; // +1 gets us past the second "

      AppendToBuffer(temp_buffer, &tx_buffer[TempBufferSize], FIREHOSE_TX_BUFFER_SIZE);

      memscpy(tx_buffer, FIREHOSE_TX_BUFFER_SIZE, temp_buffer,
              FIREHOSE_TX_BUFFER_SIZE); // memcpy

   } // original file exists


   // tx_buffer already holds the XML file
   notifyMessage(FirehoseEvent::EventId::FIREHOSE_READ_IMAGE_FILES, tx_buffer);
   sendTransmitBuffer();


   /*
     InitBufferWithXMLHeader(tx_buffer, FIREHOSE_TX_BUFFER_SIZE);
       AppendToBuffer(tx_buffer,"<data>\n",FIREHOSE_TX_BUFFER_SIZE);
       AppendToBuffer(tx_buffer,"<read ",FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"SECTOR_SIZE_IN_BYTES=\"%" SIZE_T_FORMAT "\"
     ",fh.attrs.SECTOR_SIZE_IN_BYTES);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"num_partition_sectors=\"%" SIZE_T_FORMAT "\"
     ",FileSizeNumSectors);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"physical_partition_number=\"%" SIZE_T_FORMAT "\"
     ",fh.attrs.physical_partition_number);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       sprintf(temp_buffer,"start_sector=\"%s\" ",fh.attrs.start_sector);
       AppendToBuffer(tx_buffer,temp_buffer,FIREHOSE_TX_BUFFER_SIZE);

       AppendToBuffer(tx_buffer,"/>\n</data>",FIREHOSE_TX_BUFFER_SIZE);
   */
   dbg(LOG_ALWAYS, "=======================================================");
   dbg(
      LOG_ALWAYS,
      "<read> (%.1fKB) %" SIZE_T_FORMAT " sectors from location %s FILE: '%s'",
      (double)(FileSizeNumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES) / 1024.0,
      FileSizeNumSectors,
      fh.attrs.start_sector,
      fh.attrs.filename
   );
   dbg(LOG_ALWAYS, "=======================================================");
   // sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" rawmode=\"true\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   RemoveCommentsFromXMLFiles = 0; // Unlikely but possible we are reading an XML file from the eMMC
   // thus getnextpacket below will cleans the ReadBuffer of comments

   GetNextPacket(); // this will set all variables, including GotACK

   if(!GotACK)
   {
      dbg(LOG_ERROR, "Please see log");
      ExitAndShowLog(1);
   }

   dbg(LOG_INFO, "\n");

   // Initialize SHA256
   if(ShowDigestPerFile)
   {
      SizeOfDataFedToHashRoutine = 0; // reset this
      sechsharm_sha256_init(&context);
   }


   RawMode = 1;

   gettimeofday(&time_start, NULL);
   AbsoluteTimeStart = time_start;

   while(RawMode > 0)
   {
      if(MaxBytesToReadFromUSB > FileSizeNumSectorsLeft * fh.attrs.SECTOR_SIZE_IN_BYTES)
         MaxBytesToReadFromUSB = FileSizeNumSectorsLeft * fh.attrs.SECTOR_SIZE_IN_BYTES;

      if(MaxBytesToReadFromUSB > MAX_READ_BUFFER_SIZE) MaxBytesToReadFromUSB = MAX_READ_BUFFER_SIZE;

      if(MaxBytesToReadFromUSB < fh.attrs.SECTOR_SIZE_IN_BYTES) MaxBytesToReadFromUSB = fh.attrs.SECTOR_SIZE_IN_BYTES;

      if(Simulate) CharsInBuffer = MaxBytesToReadFromUSB;

      dbg(LOG_DEBUG, "FileSizeNumSectorsLeft = %" SIZE_T_FORMAT, FileSizeNumSectorsLeft);

      GetNextPacket(); // In RawMode this will return without setting any
                       // attributes

      while(CharsInBuffer < fh.attrs.SECTOR_SIZE_IN_BYTES)
      {
         // add logic to handle scenario where recived data is less that sector
         // size.
         GetNextPacket();
      }

      NumSectors = CharsInBuffer / fh.attrs.SECTOR_SIZE_IN_BYTES;
      BuildSizeTransferSoFar += CharsInBuffer;

      if(NumSectors == 0)
      {
         dbg(
            LOG_ERROR,
            "In HandleRead ** NOTHING READ FROM TARGET ** Can't continue if "
            "not getting data. CharsInBuffer=%i and "
            "fh.attrs.SECTOR_SIZE_IN_BYTES=%" SIZE_T_FORMAT,
            CharsInBuffer,
            fh.attrs.SECTOR_SIZE_IN_BYTES
         );
         PRETTYPRINT(
            (uint8_t*)ReadBuffer,
            static_cast<uint32_t>(PacketLoc + CharsInBuffer),
            MAX_READ_BUFFER_SIZE
         ); // always show everything
         ExitAndShowLog(1);
      }

      // NOTE: On the last RAW packet there is a chance we have XML data in
      // there too
      if(NumSectors > FileSizeNumSectorsLeft) NumSectors = FileSizeNumSectorsLeft; // this is what was left of RawData

      WriteCount++;
      if(!CreateDigests)
      {
         fr->write(ReadBuffer, NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES);
      }
      //    dbg (LOG_DEBUG,"Just called fwrite with %" SIZE_T_FORMAT "
      //    CharsInBuffer=%ld\n",WriteCount,CharsInBuffer);

      // dbg (LOG_INFO,"FileSizeNumSectorsLeft = %" SIZE_T_FORMAT
      // ,FileSizeNumSectorsLeft);

      gettimeofday(&time_end, NULL);

      TotalSectorsFlashed += NumSectors;

      if(NumSectors < FileSizeNumSectorsLeft) // avoiding a roll over
         FileSizeNumSectorsLeft -= NumSectors;
      else
         FileSizeNumSectorsLeft = 0;

      if(FileSizeNumSectorsLeft <= 0) RawMode = 0;

      // dbg (LOG_INFO, "\n\nNEW FileSizeNumSectorsLeft=%" SIZE_T_FORMAT
      // "\n",FileSizeNumSectorsLeft);

      PacketLoc += NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES;
      CharsInBuffer -= NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES;

      if(CharsInBuffer <= 0)
      {
         CharsInBuffer = 0;
         PacketLoc = 0;
      }


      if(ReturnTimeInSeconds(&time_start, &time_end) >= 2.0)
      {
         // To be here means more than 2 seconds has passed

         if(TotalSectorsFlashed > 0)
         {
            time_throughput_calculate(
               &AbsoluteTimeStart,
               &time_end,
               TotalSectorsFlashed * fh.attrs.SECTOR_SIZE_IN_BYTES,
               -1.0,
               LOG_INFO
            );
         }

         // gettimeofday(&time_start, NULL);
         time_start = time_end;

         if(VerboseLevel == LOG_ALWAYS) // this means user wants minimal logging
         {
            // dbg (LOG_INFO, ".");
         }
      }


      if(ShowDigestPerFile)
      {
         SizeOfDataFedToHashRoutine += (NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES);
         sechsharm_sha256_update(
            &context,
            context.leftover,
            &(context.leftover_size),
            (uint8_t*)ReadBuffer,
            static_cast<uint32_t>(NumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES)
         );
      }

   } // end of while(RawMode>0)

   gettimeofday(&time_end, NULL);

   RawMode = 0;


   time_throughput_calculate(
      &AbsoluteTimeStart,
      &time_end,
      FileSizeNumSectors * fh.attrs.SECTOR_SIZE_IN_BYTES,
      -1.0,
      LOG_ALWAYS
   );

   // Finalize SHA256
   if(ShowDigestPerFile)
   {
      sechsharm_sha256_final(&context, context.leftover, &(context.leftover_size), last_hash_value);
      PrettyPrintHexValueIntoTempBuffer(last_hash_value, 32, 0,
                                        32); // from, size, offset, maxlength
      dbg(LOG_INFO, "\n");
      dbg(
         LOG_INFO,
         "verify_programming '%s'\nSHA256 (%7lld bytes) %s\n",
         fh.attrs.filename,
         SizeOfDataFedToHashRoutine,
         temp_buffer
      );
      memscpy(verify_hash_value, sizeof(verify_hash_value), last_hash_value,
              sizeof(last_hash_value)); // memcpy

      if(ShowDigestPerFile)
      {
         dbg(
            LOG_INFO,
            "'%s' --> LUN %" SIZE_T_FORMAT " start_sector %s SHA256 (%" SIZE_T_FORMAT " bytes) %s\n",
            FileToSendWithPath,
            fh.attrs.physical_partition_number,
            fh.attrs.start_sector,
            SizeOfDataFedToHashRoutine,
            temp_buffer
         );
      }
   }

   fh_fclose(fr);
   fr = nullptr;

   // Restored this to normal
   MaxBytesToReadFromUSB = fh.attrs.MaxPayloadSizeFromTargetInBytes;

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" rawmode=\"false\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }


   GetNextPacket(); // this will set all variables, including GotACK

   RemoveCommentsFromXMLFiles = 1; // turn this back on


   if(!GotACK)
   {
      dbg(LOG_ERROR, "Please see log");
      ExitAndShowLog(1);
   }


   dbg(LOG_INFO, "=======================================================");
   dbg(LOG_INFO, "===================== SUCCESS =========================");
   dbg(LOG_INFO, "=======================================================\n\n");

   return FIREHOSE_SUCCESS;
}


FirehoseLoader::SIZE_T FirehoseLoader::ReturnFileSize(std::shared_ptr<std::fstream> fd)
{
   SIZE_T FileSize = 0;
   dbg(LOG_DEBUG, "Trying get filesize using tellg()");

   // Save current position
   std::streampos currentPos = fd->tellg();

   // Seek to end to get size
   fd->seekg(0, std::ios::end);
   FileSize = fd->tellg();

   // Restore original position
   fd->seekg(currentPos);

   return (SIZE_T)FileSize;
}

int32_t FirehoseLoader::gettimeofday(struct timeval* t, void* tzp)
{
   (void)tzp;   // Suppress unused parameter warning
#ifdef _MSC_VER // i.e. if compiling under Windows
   uint64_t system_uptime = GetTickCount();
   t->tv_sec = system_uptime / 1000;
   t->tv_usec = (system_uptime % 1000) * 1000;
#else
   struct timespec ts;

   if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
   {
      dbg(LOG_ERROR, "clock_gettime() failed");
      ExitAndShowLog(1);
   }

   t->tv_sec = ts.tv_sec;
   t->tv_usec = ts.tv_nsec / 1000; // nano to microseconds

   // dbg (LOG_INFO, "\n\nt->tv_sec  = %" SIZE_T_FORMAT , t->tv_sec);
   // dbg (LOG_INFO, "\n\nt->tv_usec = %" SIZE_T_FORMAT , t->tv_usec);
#endif
   return 0;
}

double FirehoseLoader::ReturnTimeInSeconds(struct timeval* start_time, struct timeval* end_time)
{
   struct timeval result;
   double TimeInSeconds = 0.0;

   timeval_subtract(&result, end_time, start_time);

   TimeInSeconds = (double)result.tv_usec;
   TimeInSeconds = TimeInSeconds / 1000000.0;
   TimeInSeconds += (double)result.tv_sec;

   return TimeInSeconds;
}

void FirehoseLoader::time_throughput_calculate(
   struct timeval* start_time,
   struct timeval* end_time,
   SIZE_T size_bytes,
   double NetworkElapsed,
   enum LOG_TYPES ThisLogType
)
{
   struct timeval result;
   double TimeInSeconds = 0.0, NetworkSlow = 0.0;
   char SizeString1[2048];
   char SizeString2[2048];
   char SizeString3[5000] = ""; // Larger buffer to avoid truncation warnings
                                // when combining SizeString1 and SizeString2

   timeval_subtract(&result, end_time, start_time);

   if(result.tv_sec == 0 && result.tv_usec == 0)
   {
      dbg(
         ThisLogType,
         "Time measurement too small --- Overall to target 0.0 seconds "
         "(0Bps)"
      );
      return;
   }

   TimeInSeconds = (double)result.tv_sec + (((double)result.tv_usec) / 1000000.0);

   // TimeInSeconds = ReturnTimeInSeconds(start_time,end_time);

   ReturnSizeString((SIZE_T)(size_bytes), SizeString2, sizeof(SizeString2));

   // dbg (LOG_INFO, "%d bytes transferred in %" SIZE_T_FORMAT ".%06ld seconds",
   // size_bytes, result.tv_sec, result.tv_usec); dbg (LOG_INFO, "%d bytes
   // transferred in %f seconds", size_bytes, TimeInSeconds);
   if(TimeInSeconds == 0.0)
   {
      dbg(
         ThisLogType,
         "Time measurement too small --- Overall to target 0.0 seconds "
         "(0Bps)"
      );
      return;
   }

   if(NetworkElapsed > 0.0)
   {
      NetworkSlow = (double)size_bytes / NetworkElapsed;

      if(NetworkSlow < 100000000.0)
      {
         ReturnSizeString((SIZE_T)(size_bytes / NetworkElapsed), SizeString1, sizeof(SizeString1));
         snprintf_x(
            SizeString3,
            sizeof(SizeString3),
            "FILE ACCESS SLOW!! %10s in %6.3f seconds (%10sps) --- ",
            SizeString2,
            NetworkElapsed,
            SizeString1
         );
      }
   }

   ReturnSizeString((SIZE_T)(size_bytes / TimeInSeconds), SizeString1, sizeof(SizeString1));

   dbg(ThisLogType, "%sOverall to target %6.3f seconds (%sps)", SizeString3, TimeInSeconds, SizeString1);

   if(showpercentagecomplete && TotalTransferSize > 0)
   {
      PercentageBuildLoaded = (float)((100.0 * (double)BuildSizeTransferSoFar) / (double)TotalTransferSize);
      if(PercentageBuildLoaded - this->PrePercentageBuildLoaded >= 1.0)
      {
         dbg(ThisLogType, "{percent files transferred %6.2f%%}", PercentageBuildLoaded);
         // notify when progression alteration is greater than 1%
         notifyProgression(PercentageBuildLoaded);
         this->PrePercentageBuildLoaded = PercentageBuildLoaded;
      }
      if(std::fabs(PercentageBuildLoaded - 100.0) < FLOAT32_EPSINON)
      {
         snprintf_x(
            SizeString3,
            sizeof(SizeString3),
            "Overall to target %6.3f seconds (%sps)",
            TimeInSeconds,
            SizeString1
         );
         // notify when progression alteration is reach 100%
         notifyMessage(FirehoseEvent::EventId::FIREHOSE_THROUGHPUT, SizeString3);
      }
   }
}

// Subtract the `struct timeval' values X and Y,
//   storing the result in RESULT.
//   Return 1 if the difference is negative, otherwise 0.
int32_t FirehoseLoader::timeval_subtract(struct timeval* result, struct timeval* x, struct timeval* y)
{
   int32_t nsec;

   // Perform the carry for the later subtraction by updating y.
   if(x->tv_usec < y->tv_usec)
   {
      nsec = static_cast<int32_t>((y->tv_usec - x->tv_usec) / 1000000 + 1);
      y->tv_usec -= 1000000 * static_cast<int64_t>(nsec);
      y->tv_sec += nsec;
   }

   if(x->tv_usec - y->tv_usec > 1000000)
   {
      nsec = static_cast<int32_t>((x->tv_usec - y->tv_usec) / 1000000);
      y->tv_usec += 1000000 * static_cast<int64_t>(nsec);
      y->tv_sec -= nsec;
   }

   // Compute the time remaining to wait. tv_usec is certainly positive. */
   result->tv_sec = x->tv_sec - y->tv_sec;
   result->tv_usec = x->tv_usec - y->tv_usec;

   // Return 1 if result is negative.
   return x->tv_sec < y->tv_sec;
}

void FirehoseLoader::parseConfigXML(SIZE_T XMLFileSize, char* xml_buffer, size_t xml_buffer_size)
{
   (void)XMLFileSize; // Suppress unused parameter warning
   SIZE_T localPacketLoc = 0, PacketSize = 0;

   dbg(LOG_INFO, "In parseConfigXML");

   PacketSize = strlen((char*)xml_buffer);

   while(1)
   {
      ShowXMLFileInLog = 0; // hide this from it is just config stuff
      localPacketLoc = DetermineTag(
         (char*)xml_buffer,
         localPacketLoc,
         xml_buffer_size
      ); // This sets CurrentHandlerFunction()

      // This chomps up empty space till we hit a tag
      while(IsEmptySpace(xml_buffer[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= PacketSize)
         {
            break; // we have run out of packet all is well
         }
      }

      if(localPacketLoc >= PacketSize)
      {
         // we have run out of packet
         return; // all is well
      }

      if(localPacketLoc == 0)
      {
         dbg(LOG_ERROR, "2. TAG not found or recognized");
         return; // FIREHOSE_ERROR;
      }

      if(ParseAttributes)
      {
         // To be this far fh.xml_buffer[localPacketLoc] is pointing to the
         // first char of the first attribute
         localPacketLoc = DetermineAttributes((char*)xml_buffer, localPacketLoc, xml_buffer_size);

         if(localPacketLoc == 0)
         {
            dbg(LOG_ERROR, "ATTRIBUTES not found or recognized");
            return; // FIREHOSE_ERROR;
         }
      }
      else
         localPacketLoc = GetStringFromXML((char*)xml_buffer, localPacketLoc, xml_buffer_size);


      // At this point some attributes will need additional processing before
      // we call the Tag Handler function
      if(ParseComplicatedAttributes() == FIREHOSE_ERROR) // i.e. start_sector="NUM_DISKSECTORS-33."
      {
         return;
      }

      // Below is the function pointer, i.e. tag_handler_t
      // CurrentHandlerFunction;
      dbg(LOG_INFO, "Run Function %d", CurrentHandlerFunction);
      (this->*(CurrentHandlerFunction))();

      // This chomps up empty space till we hit a tag
      while(IsEmptySpace(xml_buffer[localPacketLoc]))
      {
         localPacketLoc++;

         if(localPacketLoc >= PacketSize)
         {
            break; // we have run out of packet all is well
         }
      }

      if(xml_buffer[localPacketLoc] == 0) break; // we're out of packet, all is well

      // if(XMLFileSize-localPacketLoc<50)
      //   break;
   }


   // EntireXMLFileBuffer holds the entire XML file
}

int32_t FirehoseLoader::operatorHigherPrecedenceThan(char left, char right)
{
   char operator_string[] = "-+*/";
   char* left_pos = strchr(operator_string, left);
   char* right_pos = strchr(operator_string, right);

   if(left_pos != nullptr && right_pos != nullptr && left_pos > right_pos)
   {
      return 1;
   }

   return 0;
}

FirehoseLoader::boolean FirehoseLoader::MyParseExpression(char* buffer, SIZE_T BufferLength, SIZE_T* result)
{
   SIZE_T i = 0, TempValue = 0;
   char Operand = '+', ch;

   *result = 0;

   for(i = 0; i < BufferLength; i++)
   {
      ch = buffer[i];

      if(ch == '\0')
         break;
      else if(ch >= '0' && ch <= '9')
      {
         TempValue = (TempValue * 10) + (ch - 0x30);
      }
      else
      {
         // we have a char that is NOT A NUMBER, so evaluate
         if(Operand == '+')
         {
            *result += TempValue;
         }
         else if(Operand == '-')
         {
            *result -= TempValue;
         }

         // all others ignored

         TempValue = 0;
         Operand = '+'; // defaults to adding 0 if we get a weird operand

         if(ch == '-') Operand = '-';
      }
   }

   // in the event user does this "1+2+" i.e. forgets final number, it would be
   // 0 anyway, so -0 or +0 makes no change
   if(Operand == '+')
   {
      *result += TempValue;
   }
   else if(Operand == '-')
   {
      *result -= TempValue;
   }


   return TRUE;
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleStorageExtras()
{
   return FIREHOSE_SUCCESS;
}


FirehoseLoader::SIZE_T FirehoseLoader::ParseComplicatedAttributes(void)
{
   volatile SIZE_T i, j;

   for(i = 0; i < (SIZE_T)ReturnNumAttributes(); i++)
   {
      j = i; // for breakpoint

      if(AllAttributes[i].Assigned == 0) continue; // variable wasn't even assigned, so no point checking


      if(CurrentHandlerFunction == &FirehoseLoader::handleStorageExtras)
      {
         // This is a special case, where the <ufs> or <emmc> tags are used and
         // variables are sent over multiple packets. In this case the struct
         // UFS_LUN_Var is used as for temporary holding and these values need
         // to be moved to storage_extras.ufs_extras.unit[ 0 ].bLUEnable
         j = i; // for breakpoint
      }
      else if(AllAttributes[i].Type == 'i' && AllAttributes[i].Type == 'b' && AllAttributes[i].Type == 'w')
      {
         continue; // integer attributes are not complicated, Only want 'x' or
                   // 's'
      }


      j = i + 1; // for breakpoint

      if(strncasecmp(AllAttributes[i].Name, "start_sector", strlen("start_sector")) == 0)
      {
         // dbg (LOG_INFO, "MATCHED - '%s'\n",AllAttributes[i].Name);

         j = i; // for breakpoint
         /*
                         if (parseSectorValue(AllAttributes[i].Raw,
            sizeof(AllAttributes[i].Raw), &fh.attrs.start_sector,
            fh.attrs.physical_partition_number, "start_sector") == FALSE)
                         {
                             logMessage("parseSectorValue could not handle
            start_sector value"); return FIREHOSE_ERROR;
                         }
         */
         j = i; // for breakpoint

      } // end of looking for start_sector

      // =================================================================

      if(strncasecmp(AllAttributes[i].Name, "MemoryName", strlen("MemoryName")) == 0)
      {
         // dbg (LOG_INFO, "MATCHED - '%s'\n",AllAttributes[i].Name);

         j = i; // for breakpoint

         /*
                        // convert memory name to enum value
                        if (strncasecmp( AllAttributes[i].Raw, "emmc",
            strlen("eMMC") ) == 0)
                        {
                            fh.attrs.storage_type = HOTPLUG_TYPE_MMC;
                            fh.store_dev.sector_size = 512; // for eMMC

                            i = j;
                        }
                        else if (strncasecmp( AllAttributes[i].Raw, "ufs",
            strlen("UFS") ) == 0)
                        {
                            fh.attrs.storage_type = HOTPLUG_TYPE_UFS;
                            fh.store_dev.sector_size = 4096; // for UFS
                            i = j;
                        }
                        else
                        {
                            logMessage("Invalid storage type: %s", (char
            *)AllAttributes[i].Raw); return FIREHOSE_ERROR;
                        }
         */
         j = i; // for breakpoint
      } // end of looking for MemoryName

      // ==================================================================
      if(strncasecmp(AllAttributes[i].Name, "LUNum", strlen("LUNum")) == 0)
      {
         if(CurrentHandlerFunction != &FirehoseLoader::handleStorageExtras)
         {
            continue;
         }

         j = i; // for breakpoint
         /*
                         Temp1 = UFS_LUN_Var_Struct.LUNum;

                         // All attributes for this LUN would have also been
            assigned Temp2 = UFS_LUN_Var_Struct.bLUEnable;
            storage_extras.ufs_extras.unit[ Temp1 ].bLUEnable           = Temp2;
                         Temp2 = UFS_LUN_Var_Struct.bBootLunID;
            storage_extras.ufs_extras.unit[ Temp1 ].bBootLunID          = Temp2;
                         Temp2 = UFS_LUN_Var_Struct.bLUWriteProtect;
            storage_extras.ufs_extras.unit[ Temp1 ].bLUWriteProtect     = Temp2;
                         Temp2 = UFS_LUN_Var_Struct.bMemoryType;
            storage_extras.ufs_extras.unit[ Temp1 ].bMemoryType         = Temp2;
                         Temp2 = UFS_LUN_Var_Struct.bDataReliability;
            storage_extras.ufs_extras.unit[ Temp1 ].bDataReliability    = Temp2;
                         Temp2 = UFS_LUN_Var_Struct.bLogicalBlockSize;
            storage_extras.ufs_extras.unit[ Temp1 ].bLogicalBlockSize   = Temp2;
                         Temp2 = UFS_LUN_Var_Struct.bProvisioningType;
            storage_extras.ufs_extras.unit[ Temp1 ].bProvisioningType   = Temp2;
                         Temp2 = UFS_LUN_Var_Struct.wContextCapabilities;
            storage_extras.ufs_extras.unit[ Temp1 ].wContextCapabilities= Temp2;

                         // We allow user to enter size_in_KB but the real
            attribute is dNumAllocUnits Temp2 = UFS_LUN_Var_Struct.size_in_KB;

                         storage_extras.ufs_extras.unit[ Temp1 ].dNumAllocUnits
            = Temp2; // for now, dNumAllocUnits *is* size_in_KB until we call
            ufs_commit_extras()
         */
         /*
                         if(fh.store_dev.blocks_per_alloc_unit==0)
                             OpenUFSforFWvalues(&fh.store_dev);

                         if(fh.store_dev.blocks_per_alloc_unit==0)
                         {
                             logMessage("ERROR: handleStorageExtras and
            ParseComplicatedAttributes() fh.store_dev.blocks_per_alloc_unit is
            0. Was MemoryName='ufs' specified in the configure tag?"); return
            FIREHOSE_ERROR;
                         }

                         if (Temp2 % fh.store_dev.blocks_per_alloc_unit != 0)
                         {
                             logMessage("ERROR: For UFS, %d is not a multiple of
            blocks_per_alloc_unit %d",Temp2,fh.store_dev.blocks_per_alloc_unit
            ); return FIREHOSE_ERROR;
                         }

                         storage_extras.ufs_extras.unit[ Temp1 ].dNumAllocUnits
            = Temp2/fh.store_dev.blocks_per_alloc_unit;

                         logMessage("dNumAllocUnits=%d since size_in_KB=%d and
            blocks_per_alloc_unit=%d",storage_extras.ufs_extras.unit[ Temp1
            ].dNumAllocUnits,Temp2,fh.store_dev.blocks_per_alloc_unit);

                         memset(&UFS_LUN_Var_Struct,0,sizeof(UFS_LUN_Var_Struct));
            // clear this out for next run
         */
         j = i; // for breakpoint

      } // end of looking for start_sector

      // =================================================================
      // ==================================================================

      if(strncasecmp(AllAttributes[i].Name, "commit", strlen("commit")) == 0)
      {
         // dbg (LOG_INFO, "MATCHED - '%s'\n",AllAttributes[i].Name);

         // Need to update this value

         j = i; // for breakpoint

         if(strncasecmp(AllAttributes[i].Raw, "true", strlen("true")) == 0)
            fh.attrs.commit = 1;
         else if(strncasecmp(AllAttributes[i].Raw, "1", strlen("1")) == 0)
            fh.attrs.commit = 1;
         else
            fh.attrs.commit = 0;

         j = i; // for breakpoint

      } // end of looking for start_sector

      // =================================================================

   } // end for i

   return FIREHOSE_SUCCESS;

} // end SIZE_T  ParseComplicatedAttributes(void)

void FirehoseLoader::ShowCommandLineOptions(void)
{
   int32_t PERCOLUMN = 21;

   for(uint64_t i = 0; i < PERCOLUMN; i++)
   {
      if(i < CommandLineOptions.size())
      {
         dbg(LOG_INFO, "\n%-30s", CommandLineOptions.at(i).c_str());
      }
      else if(1 * PERCOLUMN + i < CommandLineOptions.size())
      {
         dbg(LOG_INFO, "\t%-30s", CommandLineOptions.at(1 * PERCOLUMN + i).c_str());
      }
      else if(2 * PERCOLUMN + i < CommandLineOptions.size())
      {
         dbg(LOG_INFO, "\t%-30s", CommandLineOptions.at(2 * PERCOLUMN + i).c_str());
      }
   }

   dbg(LOG_INFO, "\n\n");
}

bool FirehoseLoader::isOptionInList(const char* option, const std::vector<std::string>& list)
{
   if(std::find(list.begin(), list.end(), std::string(option)) != list.end())
   {
      return true;
   }
   else
   {
      if(std::find(list.begin(), list.end(), std::string(option) + "=") != list.end())
      {
         ShowCommandLineOptions();

         dbg(
            LOG_ERROR,
            "\n\nYou have possibly missed an '=' sign on your command line "
            "argument. "
            "Look at the list above of possible options."
            "\n\n\tInstead of --%s did you mean --%s=  (i.e. notice it ends "
            "in an equals sign)\n\n\n\n",
            option,
            option
         );
         ExitAndShowLog(1);
      }
   }
   return false;
}

int32_t FirehoseLoader::PartOfCommandLineOptions(char* option)
{
   std::vector<std::string> DeprecatedOptions({
      "comportopentimeout=",
      "createcommandtrace",
      "createdigests",
      "createconfigxml",
      "dontsorttags",
      "erasefirst",
      "forceoverwrite",
      "getgptmainbackup=",
      "interactive=",
      "maxdigesttablesizeinbytes=",
      "noreset",
      "readbogusdata",
      "reset",
      "showdigestperfile",
      "showdigestperpacket",
      "skippatch",
      "stresstest",
      "trials=",
      "verify_build",
      "wipefirst",
   });
   std::vector<std::string> HiddenOptions({
      "simulate",
      "skipharddriveread",
      "skipstorageinit", /* Can be deprecated, no longer needed on newer
                            targets. */
      "skipwrite",
      "testvipimpact",
   });


   if(isOptionInList(option, CommandLineOptions) || isOptionInList(option, DeprecatedOptions) ||
      isOptionInList(option, HiddenOptions))
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

void FirehoseLoader::FindPartitionByLabel(SIZE_T LUN, char* LabelToMatch, char* Filename)
{
   (void)LUN; // Suppress unused parameter warning
   std::shared_ptr<std::fstream> fTemp;
   SIZE_T FileSize, NumberOfPartitionsFound = 0, offset, k, PartitionNumber, i, j, NumSectors = 0;
   SIZE_T nFirstLBA = 0, nLastLBA = 0;
   char CurrentLabel[128], CurrentUTF16Char, GUID[128], SmallString[4], LastLBA[128], FirstLBA[128];

   char szLogMessages[MAX_STRING_SIZE];
   char *sz = find_file(Filename, 1), Matched = 0;

   if(sz == NULL) ExitAndShowLog(1);

   fTemp = ReturnFileHandle(sz, MAX_PATH_SIZE, "rb"); // will exit if not successful
   FileSize = ReturnFileSize(fTemp);

   fTemp->read(tx_buffer, FileSize);
   BytesRead = fTemp->gcount();

   if(strlen(LabelToMatch) > 0) // user might just want to list partitions, thus NO LabelToMatch
      dbg(LOG_INFO, "\n\nLooking at Partition Names, looking for '%s'", LabelToMatch);


   offset = SectorSizeInBytes;
   memset(CurrentLabel, 0x0, sizeof(CurrentLabel));

   for(k = 0; k < 8; k++)
   {
      CurrentUTF16Char = tx_buffer[offset + k]; // don't forget, moving by 2's since UTF16,
                                                // converting to UTF8
      CurrentLabel[k] = CurrentUTF16Char;
   }

   if(strncmp(CurrentLabel, "EFI PART", MAX(strlen(CurrentLabel), strlen("EFI PART"))) != 0)
   {
      dbg(LOG_ERROR, "File '%s' does not have a GPT partition table present", Filename);
      ExitAndShowLog(1);
   }

   PartitionNumber = 0;

   for(i = 0; i < 128; i++)
   {
      offset = PARTITION_ARRAY_SECTOR * SectorSizeInBytes;
      offset += i * PARTITION_SIZE_IN_BYTES;
      offset += PARTITION_NAME_OFFSET_IN_BYTES;

      memset(CurrentLabel, 0x0, sizeof(CurrentLabel));

      for(k = 0; k < 72; k += 2)
      {
         CurrentUTF16Char = tx_buffer[offset + k]; // don't forget, moving by 2's since UTF16,
                                                   // converting to UTF8

         if(CurrentUTF16Char > '\x7E' || CurrentUTF16Char < '\x20') break;

         CurrentLabel[k / 2] += CurrentUTF16Char;
      }


      if(strlen(CurrentLabel) == 0)
      {
         break;
      }

      offset = PARTITION_ARRAY_SECTOR * SectorSizeInBytes;
      offset += i * PARTITION_SIZE_IN_BYTES;
      offset += PARTITION_GUID_OFFSET_IN_BYTES;
      memset(GUID, 0x0, sizeof(GUID));
      k = 15;

      for(j = 0; j < 16; j++)
      {
         CurrentUTF16Char = tx_buffer[offset + k]; // don't forget, moving by 2's since UTF16,
                                                   // converting to UTF8
         sprintf(SmallString, "%.2X", CurrentUTF16Char & 0xFF);

         GUID[2 * j] = SmallString[0];
         GUID[2 * j + 1] = SmallString[1];

         k--;
      }


      offset = PARTITION_ARRAY_SECTOR * SectorSizeInBytes;
      offset += i * PARTITION_SIZE_IN_BYTES;
      offset += PARTITION_LLBA_OFFSET_IN_BYTES;
      memset(LastLBA, 0x0, sizeof(LastLBA));
      k = 7;

      for(j = 0; j < 8; j++)
      {
         CurrentUTF16Char = tx_buffer[offset + k]; // don't forget, moving by 2's since UTF16,
                                                   // converting to UTF8
         sprintf(SmallString, "%.2X", CurrentUTF16Char & 0xFF);

         LastLBA[2 * j] = SmallString[0];
         LastLBA[2 * j + 1] = SmallString[1];

         k--;
      }


      offset = PARTITION_ARRAY_SECTOR * SectorSizeInBytes;
      offset += i * PARTITION_SIZE_IN_BYTES;
      offset += PARTITION_FLBA_OFFSET_IN_BYTES;
      memset(FirstLBA, 0x0, sizeof(FirstLBA));
      k = 7;

      for(j = 0; j < 8; j++)
      {
         CurrentUTF16Char = tx_buffer[offset + k]; // don't forget, moving by 2's since UTF16,
                                                   // converting to UTF8
         sprintf(SmallString, "%.2X", CurrentUTF16Char & 0xFF);

         FirstLBA[2 * j] = SmallString[0];
         FirstLBA[2 * j + 1] = SmallString[1];

         k--;
      }

      i = i;

      for(j = 0; j < static_cast<SIZE_T>(strlen(FirstLBA)); j++)
      {
         nFirstLBA = nFirstLBA * 16;

         if(FirstLBA[j] >= 0x30 && FirstLBA[j] <= 0x39)
            nFirstLBA += (FirstLBA[j] - 0x30);
         else
            nFirstLBA += (FirstLBA[j] - 55); // 'A' = 65, and 0xA is 10, therefore subtrace (65-10)=55
      }

      for(j = 0; j < static_cast<SIZE_T>(strlen(LastLBA)); j++)
      {
         nLastLBA = nLastLBA * 16;

         if(LastLBA[j] >= 0x30 && LastLBA[j] <= 0x39)
            nLastLBA += (LastLBA[j] - 0x30);
         else
            nLastLBA += (LastLBA[j] - 55); // 'A' = 65, and 0xA is 10, therefore subtrace (65-10)=55
      }

      NumSectors = nLastLBA - nFirstLBA + 1;

      // to be here we have the label
      Matched = 0;

      if(strlen(LabelToMatch) > 0)
      {
         if(strncasecmp(CurrentLabel, LabelToMatch, MAX(strlen(CurrentLabel), strlen(LabelToMatch))) == 0)
         {
            Matched = 1;
         }
      }

      if(strlen(CurrentLabel) > 0)
      {
         char FileSize1[2048];
         ReturnSizeString(NumSectors * SectorSizeInBytes, FileSize1, sizeof(FileSize1));

         NumberOfPartitionsFound += 1;

         if(IsASCIIString(CurrentLabel))
         {
            sprintf(
               temp_buffer,
               "\n(%.2d) '%36s' - %10d sectors (%12s) - at sector %10d",
               (int32_t)PartitionNumber,
               CurrentLabel,
               (uint32_t)NumSectors,
               FileSize1,
               (uint32_t)nFirstLBA
            );
            AppendToBuffer(szLogMessages, temp_buffer, MAX_STRING_SIZE);
         }
         else
         {
            sprintf(
               temp_buffer,
               "\n(%.2d) Found Partition Name: only garbage characters "
               "found",
               (uint32_t)PartitionNumber
            );
            AppendToBuffer(szLogMessages, temp_buffer, MAX_STRING_SIZE);
         }
      }

      dbg(LOG_INFO, "%s", temp_buffer);

      PartitionNumber += 1;

   } // end for i


   fh_fclose(fTemp);
   fTemp = nullptr;
}

int32_t FirehoseLoader::IsASCIIString(char* sz)
{
   int32_t i, j;

   j = static_cast<int32_t>(strlen(sz));

   for(i = 0; i < j; i++)
   {
      if((uint32_t)sz[i] > 128 || sz[i] < 20) return 0; // not all ASCII characters
   }

   return 1; // to be here means they are all ASCII characters
}


void FirehoseLoader::ReturnSizeString(uint64_t size_bytes, char* sz, uint64_t Length)
{
   (void)Length; // Suppress unused parameter warning
   // static char MyReturnSizeString[2048];

   if(size_bytes >= (1024 * 1024 * 1024))
      sprintf(sz, "%.2f GB", (size_bytes * 1.0) / (1024.0 * 1024.0 * 1024.0));
   else if(size_bytes >= (1024 * 1024))
      sprintf(sz, "%.2f MB", (size_bytes * 1.0) / (1024.0 * 1024.0));
   else if(size_bytes >= (1024))
      sprintf(sz, "%.2f KB", (size_bytes * 1.0) / (1024.0));
   else
      sprintf(sz, "%.2f B", (size_bytes * 1.0));

   // return MyReturnSizeString;
}

void FirehoseLoader::ParseContentsXML(char* FileAndPath)
{
   // FileAndPath =
   // "\\\\sundae\\builds664\\PROD\\M8994AAAAANLYD1024.4\\contents.xml"

   std::shared_ptr<std::fstream> fx;
   char *pch, *pchMAX, *pchOld;
   SIZE_T CurrentPacketLoc = 0, i, j, k, StringLength;
   SIZE_T FileSize;
   char SaveThis = 0;
   int32_t result;
   size_t xml_buffer_size;
   char* xml_buffer;

   if(FileAndPath == NULL || strlen(FileAndPath) == 0) return;

   dbg(LOG_INFO, "Attempting to access '%s'", FileAndPath);
   fx = ReturnFileHandle(FileAndPath, MAX_PATH_SIZE,
                         "r"); // will exit if not successful
   FileSize = ReturnFileSize(fx);

   dbg(LOG_DEBUG, "CONTENTS.xml file size is %" SIZE_T_FORMAT " bytes", FileSize);

   if(FileSize == 0)
   {
      dbg(LOG_INFO, "--contentsxml file size was 0 bytes!!");
      ExitAndShowLog(1);
   }

   xml_buffer_size = FileSize + 1; /* +1 for NULL character*/
   xml_buffer = (char*)calloc(xml_buffer_size, 1);
   if(xml_buffer == NULL)
   {
      dbg(
         LOG_ERROR,
         "Falled to allocate a buffer of %zu bytes to store contents.xml you "
         "provided (%s), free some memory and try again.",
         xml_buffer_size,
         FileAndPath
      );
      ExitAndShowLog(1);
   }

   memset(xml_buffer, 0, xml_buffer_size);
   fx->read(xml_buffer, FileSize);
   BytesRead = fx->gcount();
   fh_fclose(fx);
   fx = nullptr;

   FileSize = BytesRead;


   CurrentPacketLoc = 0; // back to the start

   while(1)
   {
      // #TODO Need the Linux path if not windows
      pch = strstr(&xml_buffer[CurrentPacketLoc],
                   "<windows_root_path"); // Find a space after the TAG name

      if(pch == NULL) //'\0')   // if null, XML is not formed correctly
      {
         // We're done, no more files
         break;
      }

      pch += strlen("<windows_root_path"); // here pch = flavor="asic"> OR pch = >

      CurrentPacketLoc = pch - xml_buffer;

      // Now hunt for > character
      for(i = CurrentPacketLoc; i < FileSize; i++)
      {
         if(xml_buffer[i] == '>') break;
      }

      CurrentPacketLoc = i + 1;

      pch = strstr(&xml_buffer[CurrentPacketLoc], "</windows_root_path>");

      if(pch == NULL) //'\0')   // if null, XML is not formed correctly
      {
         // We're done, no more files
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      if(CopyString(
            ContensXMLPath[NumContentsXMLPath].Path,
            xml_buffer,
            0,
            CurrentPacketLoc,
            pch - &xml_buffer[CurrentPacketLoc],
            sizeof(ContensXMLPath[NumContentsXMLPath].Path),
            xml_buffer_size
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
            "0",
            strlen("0")
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      ContensXMLPath[NumContentsXMLPath].Address = CurrentPacketLoc;


      // SPECIAL CASE, if the path is ./ then I want to extract the path from
      // FileAndPath
      if(ContensXMLPath[NumContentsXMLPath].Path[0] == '.')
      {
         for(i = strlen(FileAndPath); i > 1; i--)
         {
            if(FileAndPath[i] == '/' || FileAndPath[i] == '\\') break;
         }

         if(i > 1) // possible that FileAndPath="contents.xml", and thus
                   // doesn't have a directory path in front of it
         {
            if(ContensXMLPath[NumContentsXMLPath].Path[2] == '\0' &&
               (ContensXMLPath[NumContentsXMLPath].Path[1] == '/' || ContensXMLPath[NumContentsXMLPath].Path[1] == '\\'
               ))
            {
               if(CopyString(
                     ContensXMLPath[NumContentsXMLPath].Path,
                     FileAndPath,
                     0,
                     0,
                     i + 1,
                     sizeof(ContensXMLPath[NumContentsXMLPath].Path),
                     xml_buffer_size
                  ) == 0)
               {
                  dbg(
                     LOG_ERROR,
                     "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
                     "0",
                     strlen("0")
                  );
                  free(xml_buffer);
                  ExitAndShowLog(1);
               }
            }
            else
            {
               size_t len = strlen(ContensXMLPath[NumContentsXMLPath].Path);
               memmove(temp_buffer, ContensXMLPath[NumContentsXMLPath].Path, len);
               temp_buffer[len] = '\0';
               if(CopyString(
                     ContensXMLPath[NumContentsXMLPath].Path,
                     FileAndPath,
                     0,
                     0,
                     i + 1,
                     sizeof(ContensXMLPath[NumContentsXMLPath].Path),
                     xml_buffer_size
                  ) == 0)
               {
                  dbg(
                     LOG_ERROR,
                     "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
                     "0",
                     strlen("0")
                  );
                  free(xml_buffer);
                  ExitAndShowLog(1);
               }
               if(CopyString(
                     ContensXMLPath[NumContentsXMLPath].Path,
                     temp_buffer,
                     i + 1,
                     0,
                     len,
                     sizeof(ContensXMLPath[NumContentsXMLPath].Path),
                     len
                  ) == 0)
               {
                  dbg(
                     LOG_ERROR,
                     "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
                     "0",
                     strlen("0")
                  );
                  free(xml_buffer);
                  ExitAndShowLog(1);
               }
            }
         }
      }

      NumContentsXMLPath++;

   } // end while(1)


   CurrentPacketLoc = 0; // back to the start

   while(1)
   {
      char* pch_download_end;
      char* pch_ref;
      SaveThis = 1; // assume we will keep this. We won't keep those with * in filename

      ContensXML[NumContensXML].StorageType = 0; // 0 is both or unknown storage type, 'e'=emmc,'u'=ufs,'n'=nand
      ContensXML[NumContensXML].FileType = 0;    // assume normal file such as sbl1.mbn
      ContensXML[NumContensXML].Flavor[0] = 0;   // assume no flavor

      // Get the filename
      // ---------------------------------------------------------------------------------------------------------------------------------------
      // Get the filename
      // ---------------------------------------------------------------------------------------------------------------------------------------
      // Get the filename
      // ---------------------------------------------------------------------------------------------------------------------------------------

      //<file_name>mba.mbn</file_name>
      //<file_name flavor="asic_8916">sbl1.mbn</file_name>
      pch = strstr(&xml_buffer[CurrentPacketLoc], "<download_file");
      if(pch != nullptr)
      {
         pch_download_end = strstr(pch, "</download_file>");
         if(pch_download_end == NULL)
         {
            dbg(LOG_WARN, "Failed to find closing </download_file>, last offset %d", CurrentPacketLoc);
            break;
         }
      }
      else
      {
         pch_download_end = &xml_buffer[CurrentPacketLoc];
      }

      pch = strstr(
         &xml_buffer[CurrentPacketLoc],
         "<file_name"
      ); // this gets us to either <filename> OR
         // <file_name flavor="asic_8916">
      if(pch == NULL)
      {
         // We're done, no more files
         break;
      }

      // Check if <file_ref> includes "ignore="true""
      pch_ref = strstr(&xml_buffer[CurrentPacketLoc],
                       "<file_ref"); // this gets us to either <file_ref
      // Make sure the <file_ref tag comes before the <file_name tag
      if(pch_ref != nullptr && pch_ref < pch)
      {
         char* pch_end;
         pch_end = strstr(pch_ref, ">");
         pch_ref = strstr(pch_ref, "ignore=\"");
         // Check if ignore tag is inside the tag.
         if(pch_ref != nullptr && pch_ref < pch_end)
         {
            pch_ref += strlen("ignore=\"");
            if(strncmp(pch_ref, "true", 4) == 0)
            {
               // Ignore is TRUE, skip this <file_ref>
               pch_ref = strstr(pch_end, "</file_ref>");
               CurrentPacketLoc = pch_ref - xml_buffer;
               continue;
            }
         }
      }

      CurrentPacketLoc = pch - xml_buffer;

      // Need to decide

      if(xml_buffer[CurrentPacketLoc + strlen("<file_name")] == '>')
      {
         // we have this <file_name>mba.mbn</file_name>
         pch += strlen("<file_name>");
      }
      else
      {
         // we have this <file_name flavor="asic_8916">sbl1.mbn</file_name>
         // ContensXML[NumContensXML].Flavor

         pch = strstr(
            &xml_buffer[CurrentPacketLoc],
            "flavor=\""
         ); // this gets us to either <filename> OR
            // <file_name flavor="asic_8916">

         if(pch == NULL)
         {
            dbg(
               LOG_ERROR,
               "Looks like fh_loader cannot parse this contents.xml file. "
               "Confused by what looked like <file_name flavor=, shown "
               "below\n\n%s",
               &xml_buffer[CurrentPacketLoc]
            );
            free(xml_buffer);
            ExitAndShowLog(1);
         }

         pch += strlen("flavor=\"");
         CurrentPacketLoc = pch - xml_buffer; // &xml_buffer[CurrentPacketLoc] =
                                              // rawprogram_unsparse.xml</file_name>

         // where is the closing "
         for(i = CurrentPacketLoc; i < FileSize; i++)
         {
            if(xml_buffer[i] == '\"')
            {
               break;
            }
         }

         if(CopyString(
               ContensXML[NumContensXML].Flavor,
               xml_buffer,
               0,
               CurrentPacketLoc,
               i - CurrentPacketLoc,
               sizeof(ContensXML[NumContensXML].Flavor),
               xml_buffer_size
            ) == 0)
         {
            dbg(LOG_ERROR, "Failed to copy string into ContensXML[NumContensXML].Flavor");
            free(xml_buffer);
            ExitAndShowLog(1);
         }

         if(strncmp(
               ContensXML[NumContensXML].Flavor,
               flavor,
               MAX(strlen(ContensXML[NumContensXML].Flavor), strlen(flavor))
            ) != 0)
         {
            // don't match, user is looking for a different flavor, ignore this
            continue;
         }

         i = i;

         pch += strlen(ContensXML[NumContensXML].Flavor) + 2; // +2 is for ">
         CurrentPacketLoc = pch - xml_buffer;                 // &xml_buffer[CurrentPacketLoc] =
                                                              // rawprogram_unsparse.xml</file_name>
      }


      // At this point &EntireXMLFileBuffer[CurrentPacketLoc] is pointing at
      // <file_name>rawprogram_unsparse.xml</file_name> but there is a chance
      // the XML line prior to this is <partition_file storage_type="emmc"> and
      // I want to know the storage_type if it exists. So look backwards for a
      // <, then forwards for storage type

      for(i = CurrentPacketLoc - 1; i > 1; i--)
      {
         if(xml_buffer[i] == '<')
         {
            break;
         }
      }


      // here pch = NON-HLOS.bin"

      CurrentPacketLoc = pch - xml_buffer; // &xml_buffer[CurrentPacketLoc] =
                                           // rawprogram_unsparse.xml</file_name>

      // To be here means I should be at something like       <partition_file
      // storage_type="emmc">
      //                                                         <file_name>rawprogram_unsparse.xml</file_name>

      pch = strstr(&xml_buffer[i], "<partition_file");

      if(pch != nullptr) //'\0')
      {
         // we found it, but did we go to far?
         if(pch < &xml_buffer[CurrentPacketLoc])
         {
            // we found it *before* our current position, so this is for this
            // <file_name>
            ContensXML[NumContensXML].FileType = 'r'; // this is like a rawprogram0.xml file
         }
      }


      pch = strstr(&xml_buffer[i], "<partition_patch_file");

      if(pch != nullptr) //'\0')
      {
         // we found it, but did we go to far?
         if(pch < &xml_buffer[CurrentPacketLoc])
         {
            // we found it *before* our current position, so this is for this
            // <file_name>
            ContensXML[NumContensXML].FileType = 'p'; // this is like a patch0.xml file
         }
      }

      // Now look forward for storage_type

      pch = strstr(&xml_buffer[i],
                   "storage_type="); // Find a space after the TAG name

      if(pch != nullptr) //'\0')
      {
         // we found it, but did we go to far?
         if(pch < &xml_buffer[CurrentPacketLoc])
         {
            // we found it *before* our current position, so this is for this
            // <file_name>
            pch += strlen("storage_type=") + 1; // here pch = emmc"

            if(pch[0] == 'e' && pch[1] == 'm' && pch[2] == 'm' && pch[3] == 'c')
               ContensXML[NumContensXML].StorageType = 'e'; // 'e'=emmc,'u'=ufs,'n'=nand
            else if(pch[0] == 'u' && pch[1] == 'f' && pch[2] == 's')
               ContensXML[NumContensXML].StorageType = 'u'; // 'e'=emmc,'u'=ufs,'n'=nand
            else if(pch[0] == 'n' && pch[1] == 'a' && pch[2] == 'n' && pch[3] == 'd')
               ContensXML[NumContensXML].StorageType = 'n'; // 'e'=emmc,'u'=ufs,'n'=nand

         } // found the attr earlier than the <file_name> tag
      } // end if(pch!=NULL) - i.e. we found a storage_type attr
      else
      {
         // dbg (LOG_DEBUG,"Contents.xml is missing storage_type= and therefore
         // assuming both eMMC and UFS"); ContensXML[NumContensXML].StorageType
         // = 'e';  // 'e'=emmc,'u'=ufs,'n'=nand
      }


      pch = strstr(&xml_buffer[CurrentPacketLoc],
                   "</file_name>"); // Find a space after the TAG name

      if(pch == NULL) //'\0')   // if null, XML is not formed correctly
      {
         dbg(
            LOG_ERROR,
            "2) XML not formed correctly. Expected one SPACE character at loc "
            "%d",
            CurrentPacketLoc
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      if(CopyString(
            ContensXML[NumContensXML].Filename,
            xml_buffer,
            0,
            CurrentPacketLoc,
            pch - &xml_buffer[CurrentPacketLoc],
            sizeof(ContensXML[NumContensXML].Filename),
            xml_buffer_size
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
            "0",
            strlen("0")
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      if(strncmp(
            ContensXML[NumContensXML].Filename,
            "qdsp6sw.mbn",
            MAX(strlen(ContensXML[NumContensXML].Filename), strlen("qdsp6sw.mbn"))
         ) == 0)
      {
         CurrentPacketLoc = CurrentPacketLoc; // solely for breakpoint, i.e.
                                              // change filename above as needed
      }


      ContensXML[NumContensXML].Address = CurrentPacketLoc;

      // if the filename has a * in it, we won't save those
      for(i = 0; i < static_cast<SIZE_T>(strlen(ContensXML[NumContensXML].Filename)); i++)
      {
         if(ContensXML[NumContensXML].Filename[i] == '*' || ContensXML[NumContensXML].Filename[i] == '$') SaveThis = 0;
      }


      // Get the path
      // ---------------------------------------------------------------------------------------------------------------------------------------
      // Get the path
      // ---------------------------------------------------------------------------------------------------------------------------------------
      // Get the path
      // ---------------------------------------------------------------------------------------------------------------------------------------

      //<file_path flavor="asic">
      //<file_path flavor="modemlite">
      //<file_path>

      pch = strstr(&xml_buffer[CurrentPacketLoc], "<file_path");

      if(pch == NULL) //'\0')   // if null, XML is not formed correctly
      {
         dbg(
            LOG_ERROR,
            "3) XML not formed correctly. Expected one SPACE character at loc "
            "%d",
            CurrentPacketLoc
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      pch += strlen("<file_path"); // here pch = flavor="asic"> OR pch = >

      CurrentPacketLoc = pch - xml_buffer;

      if(pch[0] == ' ')
      {
         // to be here means there are other tags in <file_path >.
         // We will be hunting for flavor possibly, so we need to know how far
         // we can go
         pchMAX = strstr(&xml_buffer[CurrentPacketLoc],
                         ">"); // End of <file_path.....>

         pchOld = pch; // backup since the next search can blow way past the
                       // actual file
         while(1)
         {
            pch = strstr(
               &xml_buffer[CurrentPacketLoc],
               "flavor="
            ); // <file_path flavor="asic"> -->
               // flavor="asic">

            if(pch == NULL || pch > pchMAX) // if null, XML is not formed correctly
            {
               dbg(
                  LOG_INFO,
                  "Could not find the --flavor='%s' for filename '%s",
                  flavor,
                  ContensXML[NumContensXML].Filename
               );
               pch = pchOld;
               break;
            }

            pch += strlen("flavor=") + 1; // here pch = asic"> timmy
            CurrentPacketLoc = pch - xml_buffer;

            // not guaranteed it says asic, so now find next >

            // where is the closing "
            for(i = CurrentPacketLoc; i < FileSize; i++)
            {
               if(xml_buffer[i] == '\"')
               {
                  break;
               }
            }

            if(CopyString(
                  ContensXML[NumContensXML].Flavor,
                  xml_buffer,
                  0,
                  CurrentPacketLoc,
                  i - CurrentPacketLoc,
                  sizeof(ContensXML[NumContensXML].Flavor),
                  xml_buffer_size
               ) == 0)
            {
               dbg(
                  LOG_ERROR,
                  "Failed to copy string into "
                  "ContensXML[NumContensXML].Flavor"
               );
               free(xml_buffer);
               ExitAndShowLog(1);
            }

            if(strlen(flavor) == 0)
               break; // user didn't specify a flavor, so we're good with the
                      // first one

            // to be this far user specified the flavor
            if(strncmp(
                  ContensXML[NumContensXML].Flavor,
                  flavor,
                  MAX(strlen(ContensXML[NumContensXML].Flavor), strlen(flavor))
               ) != 0)
            {
               // don't match, user is looking for a different flavor, ignore
               // this
               pch = strstr(pch, "<file_path ");
               if(pch > pch_download_end || pch == NULL)
               {
                  dbg(LOG_INFO, "Failed to match a matching flavor %s, offset %d", flavor, CurrentPacketLoc);
                  pch = pchOld;
                  break;
               }
               pch += strlen("<file_path ");
               pchMAX = strstr(pch, ">");
               continue;
            }

            break; // we are done

         } // end while(1)

         pch = strstr(&xml_buffer[CurrentPacketLoc],
                      ">"); // <file_path flavor="asic"> -->   >

         if(pch == NULL) //'\0')   // if null, XML is not formed correctly
         {
            dbg(
               LOG_ERROR,
               "5) XML not formed correctly. Expected one SPACE character at "
               "loc %d",
               CurrentPacketLoc
            );
            free(xml_buffer);
            ExitAndShowLog(1);
         }

         if(pch > pchMAX)
         {
            dbg(
               LOG_ERROR,
               "Could not find the --flavor='%s' for filename '%s'",
               flavor,
               ContensXML[NumContensXML].Filename
            );
            free(xml_buffer);
            ExitAndShowLog(1);
         }

         pch += strlen(">");
      }
      else
      {
         // pch[0]='>'
         pch++;
      }

      CurrentPacketLoc = pch - xml_buffer;

      pch = strstr(&xml_buffer[CurrentPacketLoc], "</file_path>");

      if(pch == NULL) //'\0')   // if null, XML is not formed correctly
      {
         dbg(
            LOG_ERROR,
            "6) XML not formed correctly. Expected one SPACE character at loc "
            "%d",
            CurrentPacketLoc
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }


      // temp buffer
      if(CopyString(
            temp_buffer,
            xml_buffer,
            0,
            CurrentPacketLoc,
            pch - &xml_buffer[CurrentPacketLoc],
            FIREHOSE_TX_BUFFER_SIZE,
            xml_buffer_size
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
            "0",
            strlen("0")
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      for(j = 0; j < NumContentsXMLPath; j++)
      {
         if(ContensXMLPath[j].Address > ContensXML[NumContensXML].Address)
         {
            break;
         } // end of comparing address

      } // end j

      if(CopyString(
            ContensXML[NumContensXML].Path,
            ContensXMLPath[j - 1].Path,
            0,
            0,
            strlen(ContensXMLPath[j - 1].Path),
            MAX_PATH_SIZE,
            MAX_PATH_SIZE
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
            "0",
            strlen("0")
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }
      {
         size_t len = strlen(ContensXML[NumContensXML].Path);
         if(ContensXML[NumContensXML].Path[len - 1] != '/' && ContensXML[NumContensXML].Path[len - 1] != '\\' &&
            temp_buffer[0] != '/' && temp_buffer[0] != '\\')
         {
            ContensXML[NumContensXML].Path[len] = '/';
            ContensXML[NumContensXML].Path[len + 1] = '\0';
         }
      }
      if(CopyString(
            ContensXML[NumContensXML].Path,
            temp_buffer,
            strlen(ContensXML[NumContensXML].Path),
            0,
            strlen(temp_buffer),
            MAX_PATH_SIZE,
            MAX_PATH_SIZE
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy '%s' of length %" SIZE_T_FORMAT " bytes into fh.attrs.start_sector",
            "0",
            strlen("0")
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }


      // if the path has a * in it, we won't save those
      for(i = 0; i < static_cast<SIZE_T>(strlen(ContensXML[NumContensXML].Path)); i++)
      {
         if(ContensXML[NumContensXML].Path[i] == '*') SaveThis = 0;
      }

      // There is a chance the path is in this format ${modem_bid:FAAAANAZ}
      // or worse
      // <file_path>debug_image/build/ms/bin/${sdi_bid:AAAAANAZ}/msm8994/</file_path>
      //<file_path>debug_image/build/ms/bin/          AAAAANAZ
      /// msm8994/</file_path>

      if(SaveThis)
      {
         int32_t found_special = 0;
         j = 0;
         k = 0;

         for(i = 0; i < static_cast<SIZE_T>(strlen(ContensXML[NumContensXML].Path)); i++)
         {
            if(ContensXML[NumContensXML].Path[i] == '$')
            {
               j = i;
               found_special = 1;
               break;
            }
         }
         if(found_special == 1)
         {
            for(; i < static_cast<SIZE_T>(strlen(ContensXML[NumContensXML].Path)); i++)
            {
               if(ContensXML[NumContensXML].Path[i] == ':')
               {
                  k = i + 1;
                  break;
               }
            }
         }

         // Don't correct this though \\corebsp-tst-112\c$\preflight\

         if(ContensXML[NumContensXML].Path[j + 1] == '\\')
         {
            j = 0; // we have \\corebsp-tst-112\c$\preflight\, so don't correct
                   // it
         }

         if(j > 0)
         {
            StringLength = strlen(ContensXML[NumContensXML].Path);

            for(i = j; i < StringLength; i++)
            {
               if(ContensXML[NumContensXML].Path[k] == '}')
               {
                  k++;
                  ContensXML[NumContensXML].Path[i] = SLASH;

                  if(ContensXML[NumContensXML].Path[k] == 0)
                     ContensXML[NumContensXML].Path[i + 1] = '\0';
                  else
                     k++; // get past the slash // we have this case
                          // <file_path>debug_image/build/ms/bin/${sdi_bid:AAAAANAZ}/msm8994/</file_path>

                  // break;
               }
               else
               {
                  ContensXML[NumContensXML].Path[i] = ContensXML[NumContensXML].Path[k];
                  k++;
               }
            } // end for i
         } // end if(j>0), we found the $ format

         // Before saving this file, let's make sure the user doesn't really
         // want it
         NumContensXML++; // those with a * in the filename are not saved

         if(num_filter_not_files > 0)
         {
            if(ThisFileIsInNotFilterFiles(ContensXML[NumContensXML - 1].Filename))
            {
               dbg(
                  LOG_INFO,
                  "'%s' is being SKIPPED since it was in --notfiles provided "
                  "by the user at the command line",
                  ContensXML[NumContensXML - 1].Filename
               );
               NumContensXML--; // back out the above change
            }
         } // end of if(num_filter_not_files>0)
      }

      if(NumContensXML >= MAX_CONTENTS_XML_ENTRIES)
      {
         dbg(LOG_ERROR, "More than %i entries in contents.xml, can't continue", MAX_CONTENTS_XML_ENTRIES);
         free(xml_buffer);
         ExitAndShowLog(1);
      }

   } // end while(1)


   // now clean up the slashes

   for(i = 0; i < NumContensXML; i++)
   {
      for(j = 0; j < static_cast<SIZE_T>(strlen(ContensXML[i].Path)); j++)
      {
         if(ContensXML[i].Path[j] == WRONGSLASH) ContensXML[i].Path[j] = SLASH;
      }

      // does it end in a slash?
      if(ContensXML[i].Path[j - 1] != SLASH)
      {
         ContensXML[i].Path[j] = SLASH;
         ContensXML[i].Path[j + 1] = '\0';
      }

      i = i;
   }


   dbg(LOG_INFO, "\n\nEMMC\n");

   j = 0;

   for(i = 0; i < NumContensXML; i++)
   {
      if(ContensXML[i].StorageType == 'e')
      {
         j++;
         // dbg (LOG_INFO,"(%2i) %i %30s
         // %s",j,ContensXML[i].FileType,ContensXML[i].Filename,ContensXML[i].Path);
         dbg(LOG_INFO, "(%2i) %30s    %s", j, ContensXML[i].Filename, ContensXML[i].Path);
      } // end if( ContensXML[i].StorageType == 'e' )

   } // end for(i=0;i<NumContensXML;i++)

   dbg(LOG_INFO, "\n\nUFS\n");
   j = 0;

   for(i = 0; i < NumContensXML; i++)
   {
      if(ContensXML[i].StorageType == 'u')
      {
         j++;
         // dbg (LOG_INFO,"(%2i) %i %30s
         // %s",j,ContensXML[i].FileType,ContensXML[i].Filename,ContensXML[i].Path);
         dbg(LOG_INFO, "(%2i) %30s    %s", j, ContensXML[i].Filename, ContensXML[i].Path);
      }
   }

   dbg(LOG_INFO, "\n\nSPINOR\n");
   j = 0;

   for(i = 0; i < NumContensXML; i++)
   {
      if(ContensXML[i].StorageType == 's')
      {
         j++;
         // dbg(LOG_INFO,"(%2i) %i %30s
         // %s",j,ContensXML[i].FileType,ContensXML[i].Filename,ContensXML[i].Path);
         dbg(LOG_INFO, "(%2i) %30s    %s", j, ContensXML[i].Filename, ContensXML[i].Path);
      }
   }

   dbg(LOG_INFO, "\n\nBOTH\n");
   j = 0;
   {
      char* localPch;

      for(i = 0; i < NumContensXML; i++)
      {
         localPch = strstr(ContensXML[i].Filename, "prog_emmc");

         if(localPch != nullptr) DeviceProgrammerIndex = i;

         if(ContensXML[i].StorageType == 0)
         {
            j++;
            // dbg (LOG_INFO,"(%2i) %i %36s
            // %s",j,ContensXML[i].FileType,ContensXML[i].Filename,ContensXML[i].Path);
            dbg(LOG_INFO, "(%2i) %36s  %s", j, ContensXML[i].Filename, ContensXML[i].Path);
         }
      }
   }


   // if flattening the build, need to copy the XML files also

   if(FlattenBuild)
   {
      // to be here we are just copying the file to a local directory, and not
      // actually doing the <program>

      // user provided this path, therefore save files into it
      memset(flattenbuildto, 0x0, MAX_STRING_SIZE);

      if(CopyString(
            flattenbuildto,
            flattenbuildvariant,
            0,
            0,
            strlen(flattenbuildvariant),
            MAX_STRING_SIZE,
            MAX_STRING_SIZE
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildto",
            flattenbuildvariant,
            strlen(flattenbuildvariant)
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }

      if(CopyString(
            flattenbuildto,
            fh.attrs.filename,
            strlen(flattenbuildto),
            0,
            strlen(fh.attrs.filename),
            MAX_STRING_SIZE,
            strlen(fh.attrs.filename)
         ) == 0)
      {
         dbg(
            LOG_ERROR,
            "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildto",
            fh.attrs.filename,
            strlen(fh.attrs.filename)
         );
         free(xml_buffer);
         ExitAndShowLog(1);
      }


      dbg(LOG_INFO, "\n\nCopy XML files\n");
      j = 0;
      {
         char *localPch, *localPch2;

         for(i = 0; i < NumContensXML; i++)
         {
            localPch = strstr(ContensXML[i].Filename, ".xml");
            localPch2 = strstr(ContensXML[i].Filename, "firehose");

            if(localPch != nullptr || localPch2 != nullptr)
            {
               // copy this file it's an XML file

               if(fh.attrs.MemoryName[0] == ContensXML[i].StorageType || ContensXML[i].StorageType == 0)
               {
                  dbg(LOG_INFO, "(%2i) %36s  %s", j, ContensXML[i].Filename, ContensXML[i].Path);

                  if(CopyString(
                        tx_buffer,
                        ContensXML[i].Path,
                        0,
                        0,
                        strlen(ContensXML[i].Path),
                        FIREHOSE_TX_BUFFER_SIZE,
                        sizeof(ContensXML[i].Path)
                     ) == 0)
                  {
                     dbg(
                        LOG_ERROR,
                        "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into tx_buffer",
                        ContensXML[i].Path,
                        strlen(ContensXML[i].Path)
                     );
                     free(xml_buffer);
                     ExitAndShowLog(1);
                  }

                  if(CopyString(
                        tx_buffer,
                        ContensXML[i].Filename,
                        strlen(tx_buffer),
                        0,
                        strlen(ContensXML[i].Filename),
                        FIREHOSE_TX_BUFFER_SIZE,
                        strlen(ContensXML[i].Filename)
                     ) == 0)
                  {
                     dbg(
                        LOG_ERROR,
                        "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into tx_buffer",
                        ContensXML[i].Filename,
                        strlen(ContensXML[i].Filename)
                     );
                     free(xml_buffer);
                     ExitAndShowLog(1);
                  }


                  // user provided this path, therefore save files into it
                  memset(flattenbuildto, 0x0, MAX_STRING_SIZE);

                  if(CopyString(
                        flattenbuildto,
                        flattenbuildvariant,
                        0,
                        0,
                        strlen(flattenbuildvariant),
                        MAX_STRING_SIZE,
                        MAX_STRING_SIZE
                     ) == 0)
                  {
                     dbg(
                        LOG_ERROR,
                        "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildto",
                        flattenbuildvariant,
                        strlen(flattenbuildvariant)
                     );
                     free(xml_buffer);
                     ExitAndShowLog(1);
                  }

                  if(CopyString(
                        flattenbuildto,
                        ContensXML[i].Filename,
                        strlen(flattenbuildto),
                        0,
                        strlen(ContensXML[i].Filename),
                        MAX_STRING_SIZE,
                        strlen(ContensXML[i].Filename)
                     ) == 0)
                  {
                     dbg(
                        LOG_ERROR,
                        "Failed to copy string '%s' of length %" SIZE_T_FORMAT " bytes into flattenbuildto",
                        ContensXML[i].Filename,
                        strlen(ContensXML[i].Filename)
                     );
                     free(xml_buffer);
                     ExitAndShowLog(1);
                  }

                  result = MyCopyFile(tx_buffer, flattenbuildto);
                  if(result == -1) // this is only copying XML files like
                                   // rawprogram0.xml and patch0.xml
                  {
                     dbg(LOG_ERROR, "Failed to copy '%s'\n\t\t\tto '%s'\n\n", tx_buffer, flattenbuildto);
                     free(xml_buffer);
                     ExitAndShowLog(1);
                  }
                  else if(result == 0)
                     dbg(LOG_INFO, "Copied '%s'\n\t\t\tto '%s'\n\n", tx_buffer, flattenbuildto);

               } // emd of checking storage type
            } // end if(localPch!=NULL)
         } // end for i
      }
   }

   NumContensXML = NumContensXML;
   free(xml_buffer);
}

int32_t FirehoseLoader::MyCopyFile(char* FileNameSource, char* FileNameDest)
{
   std::shared_ptr<std::fstream> f1;
   std::shared_ptr<std::fstream> f2;
   // char FileBuffer[1024*1024]; // char temp_buffer[FIREHOSE_TX_BUFFER_SIZE];
   SIZE_T localBytesRead = 0, localBytesWritten = 0;

   if(!ForceOverwrite)
   {
      // if (_stat64(FileNameDest, &buf) == 0) //check if file already exists
      if(std::filesystem::exists(FileNameDest)
#ifdef TOOLS_TARGET_WINDOWS
         || GetLastError() != ERROR_FILE_NOT_FOUND
#endif
      )
      {
         dbg(
            LOG_INFO,
            "%s already exists and --forceoverwrite not set. Leaving file as "
            "is.\n",
            FileNameDest
         );
         return 1;
      }
   }

   f1 = ReturnFileHandle(FileNameSource, MAX_PATH_SIZE,
                         "rb"); // will exit if not successful
   f2 = ReturnFileHandle(FileNameDest, MAX_PATH_SIZE,
                         "wb"); // will exit if not successful

   dbg(LOG_INFO, "\n**BEGIN COPY from\n\t%s\nto\n\t%s**\n", FileNameSource, FileNameDest);

   while(1)
   {
      localBytesWritten = 0; // reset

      f1->read(temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
      localBytesRead = f1->gcount();

      // dbg (LOG_INFO, ".");

      if(localBytesRead == 0) break;

      f2->write(temp_buffer, localBytesRead);
      localBytesWritten = localBytesRead;

      if(localBytesWritten != localBytesRead) break;

   } // end while

   fh_fclose(f1);
   f1 = nullptr;
   fh_fclose(f2);
   f2 = nullptr;

   if(localBytesWritten != localBytesRead)
      return -1;
   else
      return 0;
}


void FirehoseLoader::SortMyXmlFiles(void)
{
   // configure first
   // erase
   // EVERYTHING ELSE
   // patch
   // reset

   SIZE_T k = 0, j = 0;
   char *pch, *pch2;


   // Make a backup
   while(XMLStringTable[k][0] != '\0')
   {
      CopyString(XMLStringTableTemp[k], XMLStringTable[k], 0, 0, strlen(XMLStringTable[k]), MAX_XML_SIZE, MAX_XML_SIZE);
      k++;
   } // end while

   k = 0;

   while(XMLStringTableTemp[k][0] != '\0')
   {
      pch = strstr(XMLStringTableTemp[k], "<configure ");

      if(pch != nullptr)
      {
         CopyString(
            XMLStringTable[j],
            XMLStringTableTemp[k],
            0,
            0,
            strlen(XMLStringTableTemp[k]),
            MAX_XML_SIZE,
            MAX_XML_SIZE
         );
         XMLStringTableTemp[k][0] = '*'; // mark it as used up
         j++;
      }

      k++;
   } // end while

   k = 0;

   while(XMLStringTableTemp[k][0] != '\0')
   {
      pch = strstr(XMLStringTableTemp[k], "<erase ");

      if(pch != nullptr)
      {
         CopyString(
            XMLStringTable[j],
            XMLStringTableTemp[k],
            0,
            0,
            strlen(XMLStringTableTemp[k]),
            MAX_XML_SIZE,
            MAX_XML_SIZE
         );
         XMLStringTableTemp[k][0] = '*'; // mark it as used up
         j++;
      }

      k++;
   } // end while


   k = 0;

   while(XMLStringTableTemp[k][0] != '\0')
   {
      pch = strstr(XMLStringTableTemp[k], "<patch ");
      pch2 = strstr(XMLStringTableTemp[k], "<power ");

      if(pch == NULL && pch2 == NULL)
      {
         // to be here it's not <patch or <power
         if(XMLStringTableTemp[k][0] != '*')
         {
            CopyString(
               XMLStringTable[j],
               XMLStringTableTemp[k],
               0,
               0,
               strlen(XMLStringTableTemp[k]),
               MAX_XML_SIZE,
               MAX_XML_SIZE
            );
            XMLStringTableTemp[k][0] = '*'; // mark it as used up
            j++;
         }
      }

      k++;
   } // end while


   k = 0;

   while(XMLStringTableTemp[k][0] != '\0')
   {
      pch = strstr(XMLStringTableTemp[k], "<patch ");

      if(pch != nullptr)
      {
         CopyString(
            XMLStringTable[j],
            XMLStringTableTemp[k],
            0,
            0,
            strlen(XMLStringTableTemp[k]),
            MAX_XML_SIZE,
            MAX_XML_SIZE
         );
         XMLStringTableTemp[k][0] = '*'; // mark it as used up
         j++;
      }

      k++;
   } // end while

   k = 0;

   while(XMLStringTableTemp[k][0] != '\0')
   {
      pch = strstr(XMLStringTableTemp[k], "<power ");

      if(pch != nullptr)
      {
         CopyString(
            XMLStringTable[j],
            XMLStringTableTemp[k],
            0,
            0,
            strlen(XMLStringTableTemp[k]),
            MAX_XML_SIZE,
            MAX_XML_SIZE
         );
         XMLStringTableTemp[k][0] = '*'; // mark it as used up
         j++;
      }

      k++;
   } // end while

   k = 0; // for breakpoint

} // end of SortMyXmlFiles

void FirehoseLoader::ModifyTags(void)
{
   SIZE_T k = 0, TempLength = 0;
   char* pch;

   if(!ConvertProgram2Read) return; // nothing to do

   // to be this far, user wants all <program tags to be converted to <read tags

   // loop through
   while(XMLStringTable[k][0] != '\0')
   {
      // backup into tx_buffer
      CopyString(tx_buffer, XMLStringTable[k], 0, 0, strlen(XMLStringTable[k]), FIREHOSE_TX_BUFFER_SIZE, MAX_XML_SIZE);

      // is there even a program tag to begin with
      pch = strstr(tx_buffer, "<program");

      if(pch == NULL)
      {
         k++;
         continue; // not a program tag, so get out of here
      }

      memset(temp_buffer, 0x0, FIREHOSE_TX_BUFFER_SIZE); // zero out to begin

      TempLength = pch - tx_buffer;

      CopyString(temp_buffer, tx_buffer, 0, 0, TempLength, FIREHOSE_TX_BUFFER_SIZE, FIREHOSE_TX_BUFFER_SIZE);

      AppendToBuffer(temp_buffer, "<read ", FIREHOSE_TX_BUFFER_SIZE);

      TempLength += strlen("<program "); // need to know where this ends

      AppendToBuffer(temp_buffer, &tx_buffer[TempLength], FIREHOSE_TX_BUFFER_SIZE);

      memscpy(XMLStringTable[k], MAX_XML_SIZE, temp_buffer,
              strlen(temp_buffer)); // memcpy

      k++;

   } // end while
}

FirehoseLoader::firehose_error_t FirehoseLoader::handleBenchmark()
{
   dbg(LOG_INFO, "In handleBenchmark");

   NumTries = 1001; // this alone will make ReadPort add a sleep(1) between calls

   // tx_buffer already holds the XML file
   sendTransmitBuffer();

   if(Simulate)
   {
      InitBufferWithXMLHeader(&ReadBuffer[PacketLoc], MAX_READ_BUFFER_SIZE - PacketLoc);
      AppendToBuffer(ReadBuffer, "<data>\n", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "<response value=\"ACK\" ", MAX_READ_BUFFER_SIZE);
      AppendToBuffer(ReadBuffer, "/>\n</data>", MAX_READ_BUFFER_SIZE);
      CharsInBuffer = strlen(ReadBuffer);
   }

   GetNextPacket(); // this will set all variables, including GotACK

   if(!GotACK)
   {
      dbg(LOG_ERROR, "Please see log");
      ExitAndShowLog(1);
   }

   dbg(LOG_INFO, "=======================================================");
   dbg(LOG_INFO, "===================== SUCCESS =========================");
   dbg(LOG_INFO, "=======================================================\n\n");

   return FIREHOSE_SUCCESS;
}

void FirehoseLoader::CleanseSearchPaths(void)
{
   int64_t j, k;
   char c = 0;

   for(j = 0; j < num_search_paths; j++)
   {
      for(k = 0; (uint32_t)k < strlen(search_path[j]); k++)
      {
         // find a slash
         c = search_path[j][k];

         if(c == FORWARDSLASH || c == BACKSLASH) break;
      }

      if(c != FORWARDSLASH && c != BACKSLASH) c = SLASH;

      k = strlen(search_path[j]);

      // Now does it end in a slash
      if(search_path[j][k - 1] != c)
      {
         search_path[j][k] = c;
         search_path[j][k + 1] = '\0';
      }

      if(!HasAPathCharacter(search_path[j], strlen(search_path[j])))
      {
         // to be here means user entered a relative path, i.e. something like
         // path2\path3 instead of c:\path1\path2\path3
         CopyString(temp_buffer, cwd, 0, 0, strlen(cwd), MAX_PATH_SIZE, MAX_PATH_SIZE);
         CopyString(
            temp_buffer,
            search_path[j],
            strlen(temp_buffer),
            0,
            strlen(search_path[j]),
            MAX_PATH_SIZE,
            MAX_PATH_SIZE
         );
         dbg(LOG_INFO, "'%s' changed to", search_path[j]);
         CopyString(search_path[j], temp_buffer, 0, 0, strlen(temp_buffer), MAX_PATH_SIZE, MAX_PATH_SIZE);
         dbg(LOG_INFO, "this '%s'", search_path[j]);
      }
   }
} // end of void CleanseSearchPaths(void);

void FirehoseLoader::ReadSha256File(void)
{
   std::shared_ptr<std::fstream> fd;
   char* Sha256File = find_file(DigestsPerFileName, 1);

   if(Sha256File == NULL)
   {
      // Couldn't find the file
      dbg(LOG_ERROR, "The file you specified with '%s' could not be found", DigestsPerFileName);
      ExitAndShowLog(1);
   }
   fd = ReturnFileHandle(Sha256File, strlen(Sha256File),
                         "rb"); // will exit if not successful
   if(fd != nullptr)
   {
      fd->seekg(sha256_file_offset * 32,
                std::ios::beg); // seek to the correct position in the file
      fd->read(reinterpret_cast<char*>(verify_hash_value), 32);
      sha256_file_offset++;
      fh_fclose(fd);
      fd = nullptr;
   }
   return;
}

int32_t FirehoseLoader::executeCommand(Firehose::FirehoseCommandType& pParameters)
{
   int32_t argc = 0;
   int32_t status = 0;
   char* argv[32];
   Device::SharedByteBufferPtr pArgv[32];
   this->PrePercentageBuildLoaded = 0.0;
   for(uint64_t i = 0; i < pParameters.size(); ++i)
   {
      pArgv[i] = std::make_shared<Device::SharedByteBuffer>();
      pArgv[i]->resize(pParameters.at(i).size() + 1);
      std::memcpy(
         pArgv[i]->begin(),
         pParameters.at(i).c_str(),
         pParameters.at(i).size() + 1 // + 1 for NULL terminator
      );

      argv[argc] = reinterpret_cast<char*>(pArgv[i]->begin());
      ++argc;
   }

   dbg(LOG_INFO, "Enter fh_loader\n");
   status = processCommand(argc, argv);
   dbg(LOG_INFO, "Exit fh_loader: %d\n", status);

   return status;
}

void FirehoseLoader::setRxTimeout(const std::optional<std::chrono::milliseconds>& timeout)
{
   if(timeout.has_value())
   {
      dbg(LOG_INFO, "Download rx timeout is set to %lld ms", static_cast<long long>(timeout.value().count()));
   }
   m_rxTimeoutInMs = timeout;
}

void FirehoseLoader::setReadPath(const std::filesystem::path& readImagePath)
{
   std::string pathString;
   if(readImagePath.string().find_last_of(std::filesystem::path::preferred_separator) !=
      readImagePath.string().size() - 1)
   {
      pathString = (readImagePath.string() + static_cast<char>(std::filesystem::path::preferred_separator)).c_str();
   }
   else
   {
      pathString = readImagePath.string().c_str();
   }
   if(MAX_PATH_SIZE <= pathString.length())
   {
      dbg(LOG_ERROR, "Main output path is too long");
      ExitAndShowLog(1);
   }
   std::strcpy(MainOutputDir, pathString.c_str());
   Util::createPath(MainOutputDir);
}

void FirehoseLoader::notifyMessage(const FirehoseEvent::EventId eventId,
                                   const char* eventString) // callback
{
   std::string message = "";
   char* start = strstr((char*)eventString, "<data>\n<");
   if(start != nullptr)
   {
      start += strlen("<data>\n<");
      char* end = strstr(start, " />\n");
      size_t length = end - start;
      if(end != nullptr)
      {
         message = std::string(start, length);
      }
   }
   if(message == "")
   {
      message = eventString;
   }

   notify(std::make_shared<FirehoseEvent>(eventId, message));
}

void FirehoseLoader::notifyProgression(const float progression)
{
   std::ostringstream oss;
   oss << std::fixed << std::setprecision(2) << std::setw(6) << progression << "%";
   std::string message = oss.str();
   notifyMessage(FirehoseEvent::EventId::FIREHOSE_PROGRESSION, message.c_str());
}

} // namespace Protocol
} // namespace Device

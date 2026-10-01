// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <string>

namespace Device {

static const std::string DEVICE_STORAGE_OPEN_FAILURE_PATTERN = ".*?Failed to open.*?";
static const std::string FIREHOSE_IMAGE_NOT_FOUND_PATTERN = ".*?Software image: *?";
static const std::string FIREHOSE_SIGNATURE_VERIFICATION_PATTERN = ".*?Verifying signature failed.*?";

// Errors
/*Sahara Protocol*/
#define ERR_SAHARA_IMAGE_NOT_FOUND ("Image not found")
#define ERR_FIREHOSE_PROGRAMMER_NOT_FOUND ("Programmer not found")
#define ERR_MEMDUMP_DOWNLOAD_FAILURE                                                                                    \
   ("Failed to download partition from the device.")
#define ERR_DEVICE_STORAGE_OPEN_FAILURE ("Failed to open device storage or flash memory for download")
#define ERR_SAHARA_PROTOCOL_RESET                                                                                       \
   ("Device programmer file is invalid. Sahara reset signal was received from the target device during programming.")
#define ERR_INVALID_EDL_STATE                                                                                           \
   ("Device has entered an invalid EDL state due to the download of incorrect images")

/*QMI Protocol*/
#define ERR_QMI_SERVICE_INIT_FAILURE ("QMI Service could not be initialized")
#define ERR_QMI_MESSAGE_ID_MISMATCH_ICD                                                                                  \
   ("Invalid QMI Message. The QMI message ID does not match with the ICD document format or QMI Service XMLs")
#define ERR_INVALID_PARAMETERS(category) ("Invalid parameter for " + category)

/*Firehose Protocol*/
#define ERR_IMAGE_NOT_FOUND ("Software image could not be found in the build path")
#define ERR_SIGNATURE_VERIFICATION_FAILED ("Signature verification failed for the build")
#define ERR_SERVICE_ALREADY_INITIALIZED ("Service already initialized")
#define ERR_PROTOCOL_ALREADY_OPENED ("Protocol already opened")
#define ERR_SEND_ZERO_BYTES_FAILED ("Send zero bytes failed")
#define ERR_LOADER_FREED ("Loader freed prematurely")
#define ERR_FIREHOSE_PROCESS_FAILED ("Firehose process failed")
#define ERR_LOADER_NOT_ACTIVE ("Loader not active")

/*ImageManagementServiceHandler*/
#define ERR_SERVICE_LOCKED ("Service locked")
#define ERR_PROTOCOL_INVALID ("Protocol invalid")
#define ERR_INVALID_DEVICE_IMAGE_MODE ("Invalid device image mode")
#define ERR_EDL_SWITCH_NOT_AVAILABLE ("EDL mode switch unavailable")
#define ERR_PARTITION_NOT_FOUND ("Partition not found")
#define ERR_PRESERVATION_NOT_SUPPORTED ("Preserve partition not supported")
#define ERR_FAILED_TO_RETRIEVE_MEMORY_TYPE ("Failed to retrieve memory type")

/*Manager*/
#define ERR_DEVICE_HANDLE_NOT_FOUND ("Device handle not found")
#define ERR_PROTOCOL_HANDLE_NOT_FOUND ("Protocol handle not found")
#define ERR_READ_ACCESS_LOCKED ("Read access locked for protocol")
#define ERR_WRITE_ACCESS_LOCKED ("Write access locked for protocol")
#define ERR_RELATIVE_FILE_PATH ("Relative file path")
#define ERR_FILE_SAVE_FAILED ("File save failed")
#define ERR_DIRECTORY_ITERATION_FAILED ("Directory iteration failed")

/*Connection*/
#define ERR_NULL_PACKET_SEND ("NULL packet send attempted")
#define ERR_NO_WRITE_ACCESS_SEND ("No write access for send operation")
#define ERR_TRANSACTION_ID_MISMATCH ("Transaction ID mismatch")
#define ERR_NO_ASYNC_RESPONSE ("No async response received")
#define ERR_NO_WRITE_ACCESS_CANCEL ("No write access for cancel operation")

/*Buffer*/
#define ERR_NULL_BUFFER ("NULL buffer")
#define ERR_INSUFFICIENT_BYTES ("Insufficient bytes for cast")
#define ERR_NULL_SHARED_BUFFER ("NULL shared buffer")

/*FunctionTracker*/
#define ERR_SERVICE_NOT_INITIALIZED ("Service not initialized")

/*ImageTransfer*/
#define ERR_NO_RESPONSE_RECEIVED ("No response received from device")
#define ERR_COMMAND_EXECUTION_FAILED ("Command execution failed")
#define ERR_INVALID_DEVICE_STATE ("Invalid device state")
#define ERR_FILE_OPERATION_FAILED ("File operation failed")
#define ERR_OPERATION_NOT_SUPPORTED ("Operation not supported")

/*Base Protocol*/
#define ERR_PROTOCOL_LOCK_NOT_SUPPORTED ("Protocol lock feature is not supported")
#define ERR_PROTOCOL_UNLOCK_NOT_SUPPORTED ("Protocol unlock feature is not supported")
#define ERR_SEND_SYNC_WITH_KEY_NOT_SUPPORTED ("Send sync with key feature is not supported")
#define ERR_SEND_ASYNC_WITH_KEY_NOT_SUPPORTED ("Send async with key feature is not supported")

/*Sahara Protocol - Connection*/
#define ERR_SAHARA_PROTOCOL_UNAVAILABLE ("Sahara protocol unavailable")

/*FlowControl*/
#define ERR_FLOW_CONTROL_WATERMARKS_NOT_SET ("Flow control watermarks not set")

/*Usb*/
#define ERR_USB_CONNECTION_OPEN_FAILURE ("Could not open USB connection")

/*SystemHelper*/
#define ERR_CREATE_DIRECTORY_FAILED ("Failed to create directory")
#define ERR_CREATE_SECURITY_DESCRIPTOR_FAILED ("Failed to create discretionary access control list")
#define ERR_CREATE_DIRECTORY_WINDOWS_FAILED ("Failed to create directory with access control")
#define ERR_CREATE_DIRECTORY_LINUX_FAILED ("Failed to create directory")
#define ERR_TEMP_FILE_CREATION_FAILED ("Temporary-file creation failed")

// Descriptions
// (In-depth detail for the RCA JSON "description" field — specific payloads/values, distinct from
// the general "issue" text above.)
/*Sahara Protocol*/
#define DESC_SAHARA_IMAGE_NOT_FOUND(imageId)                                                                           \
   ("Sahara image Id: " + imageId + " could not be found in the build path")
#define DESC_FIREHOSE_PROGRAMMER_NOT_FOUND                                                                             \
   ("Firehose programmer could not be found in the build path")
#define DESC_DEVICE_STORAGE_OPEN_FAILURE                                                                               \
   ("Failed to open device storage or flash memory for download")
#define DESC_SAHARA_PROTOCOL_RESET(imageFile, statusCode)                                                              \
   ("Specified device programmer file: '" + imageFile +                                                                \
    "' is invalid. A Sahara reset signal was received from the target "                                                \
    "device, indicating a failure during the programming process. Error "                                              \
    "details: '" +                                                                                                     \
    statusCode + "'.")

/*Firehose Protocol*/
#define DESC_IMAGE_NOT_FOUND(imageName)                                                                                \
   ("Software image: " + imageName + " could not be found in the build path")
#define DESC_SIGNATURE_VERIFICATION_FAILED                                                                             \
   ("Signature verification failed for the build")
#define DESC_PROTOCOL_ALREADY_OPENED(protocol, description)                                                            \
   (protocol + " protocol already opened: " + description)
#define DESC_SEND_ZERO_BYTES_FAILED(description)                                                                       \
   ("Send 0 byte fail: " + description)
#define DESC_LOADER_FREED                                                                                              \
   ("Firehose loader is freed")
#define DESC_FIREHOSE_PROCESS_FAILED(errorString)                                                                      \
   ("Firehose process failed: " + errorString)
#define DESC_LOADER_NOT_ACTIVE(description)                                                                            \
   ("Firehose Loader not active: " + description)

/*UtilityServiceHandler*/
#define DESC_UNSUPPORTED_MEMORY_TYPE(memoryType)                                                                       \
   ("Unsupported memory type : " + std::to_string(static_cast<int32_t>(memoryType)))
#define DESC_PRESERVE_PARTITION_NOT_SUPPORTED                                                                          \
   ("Preserve partition is not supported for VIP process")

/*Service*/
#define DESC_SERVICE_ALREADY_INITIALIZED                                                                               \
   ("Service already initialized. If initialization is needed on a different protocol, create a new service for it.")

/*ImageManagementServiceHandler*/
#define DESC_IMG_SERVICE_LOCKED ("Image management service locked")
#define DESC_IMG_DOWNLOAD_MODE_NOT_AVAILABLE ("Download mode not available")
#define DESC_IMG_UNSUPPORTED_MEMORY_TYPE(memoryType)                                                                   \
   ("Unsupported memory type: " + std::to_string(static_cast<int32_t>(memoryType)))
#define DESC_IMG_INVALID_PAGE_SIZE_NAND(pageSize)                                                                      \
   ("Invalid page size for NAND: " + pageSize)
#define DESC_IMG_PAGE_SIZE_CONVERSION_ERROR(pageSize)                                                                  \
   ("Conversion error for page size: " + pageSize)
#define DESC_IMG_INVALID_BLOCK_SIZE_NAND(blockSize)                                                                    \
   ("Invalid block size for NAND: " + blockSize)
#define DESC_IMG_BLOCK_SIZE_CONVERSION_ERROR(blockSize)                                                                \
   ("Conversion error for block size: " + blockSize)
#define DESC_IMG_ERASE_PARTITION_NUMBER_NOT_SET ("Erase Partition Number is not set")
#define DESC_IMG_INVALID_PARTITION_NUMBER(partNum)                                                                     \
   ("Invalid Partition Number: " + partNum)
#define DESC_IMG_PARTITION_NOT_FOUND(partName)                                                                         \
   ("Can not find partition: " + partName)
#define DESC_IMG_PARTITION_NOT_FOUND_IN_BUILD(partName)                                                                \
   ("Can not find partition in build: " + partName)
#define DESC_IMG_UNSUPPORTED_DEVICE_IMAGE_MODE(mode)                                                                   \
   ("Unsupported device image mode: " + std::to_string(static_cast<int32_t>(mode)))
#define DESC_EDL_SWITCH_NOT_AVAILABLE(mode)                                                                            \
   ("Cannot switch device to EDL mode from " + mode + " mode")
#define DESC_IMG_INVALID_LUN(imagePath)                                                                                \
   ("Provide valid LUN for the image: " + imagePath)
#define DESC_IMG_INVALID_START_SECTOR_FOR_IMAGE(imagePath)                                                             \
   ("Provide valid start sector for the image: " + imagePath)
#define DESC_IMG_INVALID_START_SECTOR(startSector)                                                                     \
   ("Invalid start sector: " + startSector)
#define DESC_IMG_INVALID_START_SECTOR_WITH_IMAGE(imagePath, startSector)                                               \
   ("Invalid start sector for image: " + imagePath + ", startSector: " + startSector)
#define DESC_IMG_INVALID_SECTOR_NUMBER(sectorCount)                                                                    \
   ("Invalid sector number: " + sectorCount)
#define DESC_IMG_MISSING_START_SECTOR ("Missing start sector")
#define DESC_IMG_MISSING_SECTOR_NUMBER ("Missing sector number")
#define DESC_IMG_MISSING_READBACK_FILE_PATH ("Missing readback file path!")
#define DESC_IMG_FILE_NOT_FOUND(imagePath)                                                                             \
   ("Image file not found: " + imagePath)
#define DESC_IMG_DEVICE_NOT_IN_FIREHOSE_MODE ("Device has not entered firehose mode!")
#define DESC_IMG_CONNECTION_NOT_AVAILABLE(protocol)                                                                    \
   (protocol + " connection not available")
#define DESC_IMG_FAILED_TO_RETRIEVE_MEMORY_TYPE ("Failed to retrive memory type from the device")
#define DESC_IMG_PRESERVATION_NOT_SUPPORTED_VIP ("Preserve partition is not supported for VIP process")
#define DESC_IMG_PRESERVATION_NOT_SUPPORTED_SINGLE_IMAGE ("Preserve partition is not supported for single image")
#define DESC_IMG_PROTOCOL_NOT_AVAILABLE(protocol)                                                                      \
   (protocol + " protocol not available")

/*Manager*/
#define DESC_DEVICE_HANDLE_NOT_FOUND(handle, lookupContext)                                                             \
   ("Could not find device handle: " + std::to_string(handle) + " in the " + lookupContext + " device list")
#define DESC_PROTOCOL_HANDLE_NOT_FOUND(handle)                                                                          \
   ("Could not find protocol handle: " + std::to_string(handle))
#define DESC_READ_ACCESS_LOCKED(requestingClientId, holdingClientId, description)                                       \
   ("Read access locked for protocol by another client: " + std::to_string(holdingClientId) + ", protocol: " +        \
    description + ", requested by client: " + std::to_string(requestingClientId))
#define DESC_WRITE_ACCESS_LOCKED(requestingClientId, holdingClientId, description)                                      \
   ("Write access locked for protocol by another client: " + std::to_string(holdingClientId) + ", protocol: " +       \
    description + ", requested by client: " + std::to_string(requestingClientId))
#define DESC_MHI_EDL_NOT_SUPPORTED ("MHI EDL switch feature not supported")
#define DESC_RELATIVE_FILE_PATH(filePath)                                                                               \
   ("Relative file path; all paths must be absolute: " + filePath)
#define DESC_FILE_SAVE_FAILED(filePath, innerError)                                                                     \
   ("Unable to save file: " + filePath + ". Inner error: " + innerError)
#define DESC_DIRECTORY_ITERATION_FAILED(directory, detail)                                                              \
   ("Unable to iterate directory " + directory + ": " + detail)

/*Connection*/
#define DESC_NULL_PACKET_SEND(description)                                                                              \
   ("Attempting to send NULL packet: " + description)
#define DESC_NO_WRITE_ACCESS_SEND(operation, description)                                                               \
   ("Cannot " + operation + "; no write access on protocol " + description)
#define DESC_TRANSACTION_ID_MISMATCH(transactionId, description)                                                        \
   ("getAsyncResponse Error: Transaction id = " + std::to_string(transactionId) + " doesn't match any corresponding async request txid. " + description)
#define DESC_NO_ASYNC_RESPONSE(transactionId, description)                                                              \
   ("getAsyncResponse Error: No async response received for Transaction id = " + std::to_string(transactionId) + ", " + description)
#define DESC_NO_WRITE_ACCESS_CANCEL(description)                                                                        \
   ("Cannot cancelTx; no write access on protocol " + description)

/*Buffer*/
#define DESC_NULL_BUFFER ("pBuffer is NULL")
#define DESC_INSUFFICIENT_BYTES ("Insufficient bytes for cast to type")
#define DESC_NULL_SHARED_BUFFER ("SharedByteBuffer is NULL")

/*FunctionTracker*/
#define DESC_SERVICE_NOT_INITIALIZED(serviceName)                                                                        \
   (serviceName + " was used without being initialized. Call initializeService()")

/*ImageTransfer*/
#define DESC_NO_PACKET_RECEIVED(context, description)                                                                    \
   ("No packet received from device " + context + ": " + description)
#define DESC_NO_RESPONSE_RECEIVED(what, description)                                                                     \
   ("No " + what + " received from Sahara protocol: " + description)
#define DESC_MAX_RESET_CYCLES_EXCEEDED ("Beyond max resetable cycle")
#define DESC_COMMAND_MODE_NOT_AVAILABLE(description)                                                                     \
   ("Command mode not available in Sahara protocol: " + description)
#define DESC_INVALID_RESPONSE_LENGTH(description)                                                                        \
   ("Invalid response length from device get info command execute (command state): " + description)
#define DESC_INVALID_PROGRAMMER ("Invalid device programmer, provide either a firehoseProgPath or a saharaImageList option")
#define DESC_CONFLICTING_PROGRAMMER_OPTIONS ("Conflict in choosing device programmer, provide either a firehoseProgPath or saharaImageList option")
#define DESC_IMAGE_TRANSFER_MODE_NOT_AVAILABLE(description)                                                                \
   ("Image transfer mode not available in Sahara protocol: " + description)
#define DESC_PROTOCOL_MODE_UNKNOWN(description)                                                                          \
   ("Sahara protocol mode unknown: " + description)
#define DESC_IMAGE_REJECTED_BY_DEVICE(imagePath, description)                                                            \
   ("Image " + imagePath + " rejected by device: " + description)
#define DESC_INVALID_TRANSFER_STATUS(description)                                                                        \
   ("Invalid image transfer status from Sahara protocol: " + description)
#define DESC_FILE_READ_FAILED(imagePath, offset, length, description)                                                    \
   ("Image transfer fail to read file: " + imagePath + " offset " + offset + " length " + length + ": " + description)
#define DESC_INVALID_MEMORY_TYPE(memoryType)                                                                             \
   ("Invalid device memory type: " + memoryType)
#define DESC_INVALID_DIGEST_HEADER_TYPE ("Invalid Digest Header Type")
#define DESC_MISSING_SIGNED_DIGESTS ("Missing signed digests option")
#define DESC_NO_PROGRAM_FILE_AVAILABLE ("No Firehose program file available")
#define DESC_INVALID_PARTITION_NUMBER(partNum)                                                                           \
   ("Invalid Partition Number: " + std::to_string(partNum))
#define DESC_NO_VALIDATION_FILE_AVAILABLE ("No build validation file available")
#define DESC_NO_RESPONSE_DEVICE_INVALID_STATE(description)                                                               \
   ("No response received from the device, found in invalid state: " + description)
#define DESC_DEVICE_NOT_READY(description)                                                                               \
   ("Device found in invalid state, not ready to receive commands: " + description)
#define DESC_ERROR_DUMPING_FILE(description)                                                                             \
   ("Error dumping file from: " + description)
#define DESC_SOC_VERSION_NOT_SUPPORTED ("SOC_HW_VERSION not supported by the device, failed to retrive storage or memory type from the device")
#define DESC_BOOT_CONFIG_NOT_SUPPORTED ("Boot config not supported by the device, failed to retrive storage or memory type from the device")
#define DESC_FAILED_RETRIEVE_MEMORY_TYPE_DB(hwVersion, error)                                                            \
   ("Failed to retrive storage or memory type from the DCL DB, SOC_HW_VERSION: " + hwVersion + ", Error: " + error)
#define DESC_INVALID_OFFLINE_PROCESS ("Invalid offline process")
#define DESC_MIBIB_MAGIC_NOT_FOUND ("Can not find mibib magic number")
#define DESC_NO_COMMAND_DATA_RECEIVED(command)                                                                          \
   ("No data received from device sahara command: " + command)
#define DESC_END_OF_TRANSFER_FOR_COMMAND(command, status)                                                               \
   ("Received end of image transfer from the device for sahara command: " + command + ", status: " + status)
#define DESC_COMMAND_EXECUTE_RESPONSE_NOT_RECEIVED(description)                                                         \
   ("Command execute response not received from Sahara protocol: " + description)
#define DESC_PARTITION_VALIDATION_FAILED(label, value) (label + ": " + value)
#define DESC_NAND_INFO_INVALID ("NAND information is invalid, please run get flash information in advance")
#define DESC_SECTOR_SIZE_MISMATCH ("Sector size in bytes mismatch between partition and patch file")

/*Base Protocol*/
#define DESC_PROTOCOL_LOCK_NOT_SUPPORTED(description) (description + " does not support the lock operation")
#define DESC_PROTOCOL_UNLOCK_NOT_SUPPORTED(description) (description + " does not support the unlock operation")
#define DESC_SEND_SYNC_WITH_KEY_NOT_SUPPORTED(description) (description + " does not support sendSyncWithKey")
#define DESC_SEND_ASYNC_WITH_KEY_NOT_SUPPORTED(description) (description + " does not support sendAsyncWithKey")

/*Sahara Protocol - Connection*/
#define DESC_SAHARA_PROTOCOL_UNAVAILABLE(description) ("Sahara protocol is disconnected: " + description)

/*FlowControl*/
#define DESC_FLOW_CONTROL_WATERMARKS_NOT_SET                                                                            \
   ("updateFlowControlCount was called before flow control watermarks were configured")

/*Usb*/
#define DESC_USB_CONNECTION_OPEN_FAILURE(identifier) ("Could not open connection: " + identifier)

/*SystemHelper*/
#define DESC_CREATE_DIRECTORY_FAILED(directory, errorMessage)                                                           \
   ("Create directory " + directory + " failed: " + errorMessage)
#define DESC_CREATE_SECURITY_DESCRIPTOR_FAILED                                                                          \
   ("Failed to create discretionary access control list for directory creation")
#define DESC_CREATE_DIRECTORY_WINDOWS_FAILED(errorCode)                                                                \
   ("Failed to create directory with discretionary access control list, Error Code: " + errorCode)
#define DESC_CREATE_DIRECTORY_LINUX_FAILED(directory) ("Could not create directory " + directory)
#define DESC_TEMP_FILE_CREATION_FAILED(directory, detail)                                                               \
   ("Unable to create a temporary file in " + directory + ": " + detail)

// Suggestions
/*Sahara Protocol*/
#define SUGG_FILE_NOT_FOUND                                                                                            \
   ("Please verify the file path and try again. If the issue persists, "                                               \
    "contact below meta build or target for assistance")
#define SUGG_SAHARA_IMAGE_NOT_FOUND                                                                                    \
   ("Please verify the sahara image from build path. If the issue persists, "                                          \
    "contact below meta build or target for assistance")
#define SUGG_DEVICE_STORAGE_OPEN_FAILURE                                                                               \
   ("For UFS storage devices, please attempt UFS provisioning before "                                                 \
    "proceeding with the download. For other storage devices, ensure that "                                            \
    "the DIP switches are correctly set")
#define SUGG_SAHARA_PROTOCOL_RESET                                                                                     \
   ("Description:\n\n"                                                                                                 \
    "When programming a target device, it is essential to use the correct "                                            \
    "device programmer file that matches the specifications and architecture "                                         \
    "of the target hardware. The programmer file contains configuration data "                                         \
    "and instructions that enable communication between the programming tool "                                         \
    "and the target device.\n\n"                                                                                       \
    "Why This Matters:\n\n"                                                                                            \
    "Compatibility: Using an incorrect programmer file may result in failed "                                          \
    "programming attempts, corrupted firmware, or even permanent damage to "                                           \
    "the device.\n"                                                                                                    \
    "Communication Errors: Mismatched files can prevent the programmer from "                                          \
    "establishing a proper connection with the target.\n"                                                              \
    "Unexpected Behavior: The device may not function as intended if "                                                 \
    "programmed with an incompatible file.\n\n"                                                                        \
    "Recommended Actions:\n\n"                                                                                         \
    "Identify the Target Device:\n"                                                                                    \
    "Confirm the exact model and configuration of the target hardware.\n\n"                                            \
    "Select the Correct Programmer File:\n"                                                                            \
    "Ensure the programmer file corresponds to the target device "                                                     \
    "specifications, including version, memory layout, and supported "                                                 \
    "features.\n\n"                                                                                                    \
    "Check File Integrity:\n"                                                                                          \
    "Verify that the programmer file is not corrupted or outdated. Use "                                               \
    "version-controlled or officially provided files whenever possible.\n\n"                                           \
    "Consult Documentation:\n"                                                                                         \
    "Refer to the device datasheet or programming guide to confirm the "                                               \
    "correct file format and naming conventions.\n\n"                                                                  \
    "Contact Support if Needed:\n"                                                                                     \
    "If you're unsure about the correct file or continue to experience "                                               \
    "issues, reach out to the hardware vendor or development team for "                                                \
    "assistance.")
#define SUGG_INVALID_EDL_STATE                                                                                         \
   ("Please attempt to place the device back into EDL (Emergency Download) "                                           \
    "mode by rebooting it. If this approach fails, kindly escalate the issue "                                         \
    "to the engineering team for further assistance.")


/*QMI Protocol*/
#define SUGG_QMI_SERVICE_INIT_FAILURE                                                                                  \
   ("Please verify if the QMI is up or in a responsive state. If the issue "                                           \
    "persists, contact the QMI( go/qmi ) or Qualcomm CE team for assistance")
#define SUGG_QMI_MESSAGE_ID_MISMATCH_ICD                                                                               \
   ("Please verify whether the QMI message is valid. If the issue persists, "                                          \
    "refer to go/qmi for ICD details on the different services")
#define SUGG_MEMDUMP_DOWNLOAD_FAILURE                                                                                  \
   ("Issue due to potential problems such as device software, insufficient "                                           \
    "disk space on the PC, USB toggling, etc.")

/*Firehose Protocol*/
#define SUGG_IMAGE_NOT_FOUND                                                                                           \
   ("Description:\n The system was unable to locate the specified software "                                           \
    "image in the designated build path. This may be due to a missing or "                                             \
    "incorrectly referenced image file. \n Recommended Actions: \n \n 1. "                                             \
    "Verify File Presence: \n Ensure that the image file '%s' exists in the "                                          \
    "expected build directory and that the path is correctly configured. \n "                                          \
    "\n 2. Check File Naming and Permissions: \n Confirm that the file name "                                          \
    "is correct (including case sensitivity) and that the file has the "                                               \
    "appropriate read permissions. \n \n 3. Contact Support Teams: \n If the "                                         \
    "issue persists after verification: \n Reach out to the Target team to "                                           \
    "confirm the image was generated and placed correctly. \n Contact the "                                            \
    "Client team if the image is expected to be provided externally or if "                                            \
    "integration issues are suspected.")
#define SUGG_SIGNATURE_VERIFICATION_FAILED                                                                             \
   ("The issue may be occurring due to one of the following reasons:\n\n "                                             \
    "Unsigned or Missing Digest File: Ensure that the digest file exists and "                                         \
    "is properly signed before initiating the process. \n\n Mismatch in "                                              \
    "Download Configuration: Please ensure there are no inconsistencies in "                                           \
    "the selected download parameters such as: Reset, Memory type and Slot "                                           \
    "number")
#define SUGG_PROTOCOL_ALREADY_OPENED                                                                                   \
   ("Only one client may hold the connection; serialize access.")
#define SUGG_SEND_ZERO_BYTES_FAILED                                                                                    \
   ("Check underlying CommonIO/USB link is open and healthy before sendAsync.")
#define SUGG_LOADER_FREED                                                                                              \
   ("Do not invoke handleFirehoseLoaderError() after m_pFirehoseLoader is released.")
#define SUGG_FIREHOSE_PROCESS_FAILED                                                                                   \
   ("Inspect loader logs; extend regex handling if recurring.")
#define SUGG_LOADER_NOT_ACTIVE                                                                                         \
   ("Call processCommand() (which instantiates m_pFirehoseLoader) before getStorageInfo().")
#define SUGG_UNSUPPORTED_MEMORY_TYPE                                                                                   \
   ("Ensure options.memoryType is within valid QC::MemoryType range "                                                  \
    "(< MEMORY_TYPE_UNKNOWN) before invoking offlineFirehoseProcess.")
#define SUGG_PRESERVE_PARTITION_NOT_SUPPORTED                                                                          \
   ("For VIP flows, do not set preservationOption with non-NONE mode + preserved partitions; use PRESERVATION_NONE.")
#define SUGG_SERVICE_ALREADY_INITIALIZED                                                                               \
   ("Include the service name in the message to aid debugging when multiple services exist.")

/*ImageManagementServiceHandler*/
#define SUGG_IMG_SERVICE_LOCKED(operationName)                                                                         \
   ("Release any lock held by another client before calling " + operationName + ".")
#define SUGG_IMG_DOWNLOAD_MODE_NOT_AVAILABLE(operationName)                                                            \
   ("Boot device into EDL mode before calling " + operationName + ".")
#define SUGG_IMG_INVALID_PAGE_SIZE_NAND                                                                                \
   ("Confirm programmer reports numeric nonzero pageSize for NAND storage.")
#define SUGG_IMG_INVALID_BLOCK_SIZE_NAND                                                                               \
   ("Confirm programmer reports numeric nonzero blockSize for NAND.")
#define SUGG_IMG_BLOCK_SIZE_CONVERSION_ERROR(blockSize, pageSize)                                                      \
   ("Ensure blockSize is greater than or equal to pageSize (blockSize: " + std::to_string(blockSize) + ", pageSize: "  \
   + std::to_string(pageSize) + ").")
#define SUGG_IMG_UNSUPPORTED_MEMORY_TYPE                                                                               \
   ("Pass a supported memoryType (UFS/EMMC/NAND/NVME/SPINOR).")
#define SUGG_IMG_INVALID_PARTITION_NUMBER                                                                              \
   ("Provide partition indices >= MAX_PARTITION_NUM(-1).")
#define SUGG_IMG_ERASE_PARTITION_NUMBER_NOT_SET                                                                        \
   ("Populate options.partitionIndexList before calling erasePartition.")
#define SUGG_IMG_PARTITION_NOT_FOUND                                                                                   \
   ("Ensure preservedPartition name matches an existing partition on device GPT.")
#define SUGG_IMG_PARTITION_NOT_FOUND_IN_BUILD                                                                          \
   ("Confirm preserved partition exists in target build and sector count matches.")
#define SUGG_IMG_INVALID_LUN                                                                                           \
   ("Set options.lun > Function::ImageTransfer::INVALID_LUN when using singleImagePath.")
#define SUGG_IMG_INVALID_START_SECTOR_FOR_IMAGE                                                                        \
   ("Set options.startSector > INVALID_START_SECTOR when using singleImagePath.")
#define SUGG_IMG_UNSUPPORTED_DEVICE_IMAGE_MODE(actualMode, expectedMode)                                               \
   ("Device is in " + actualMode + " mode, but " + expectedMode + " mode is expected.")
#define SUGG_IMG_PRESERVATION_NOT_SUPPORTED_VIP                                                                        \
   ("Disable preservation options when doing a VIP download.")
#define SUGG_IMG_PRESERVATION_NOT_SUPPORTED_SINGLE_IMAGE                                                               \
   ("Do not combine partition preservation with singleImagePath; do full build download.")
#define SUGG_IMG_INVALID_START_SECTOR                                                                                  \
   ("Use numeric start sectors or NUM_DISK_SECTORS[-N] token format.")
#define SUGG_IMG_MISSING_START_SECTOR(operationName)                                                                   \
   ("Set DataChunkOptions.startSector before " + operationName + ".")
#define SUGG_IMG_INVALID_SECTOR_NUMBER                                                                                 \
   ("Ensure DataChunkOptions.sectorCount is set to a numeric value or NUM_DISK_SECTORS[-N] token.")
#define SUGG_IMG_MISSING_READBACK_FILE_PATH                                                                            \
   ("Set DataChunkOptions.imagePath so read data can be saved.")
#define SUGG_IMG_DEVICE_NOT_IN_FIREHOSE_MODE                                                                           \
   ("Ensure device is in Firehose mode via downloadBuild or similar operation.")
#define SUGG_IMG_PROTOCOL_NOT_AVAILABLE                                                                                \
   ("Protocol object is not registered/attached; verify initialization.")
#define SUGG_IMG_CONNECTION_NOT_AVAILABLE                                                                              \
   ("Check connection creation; connection object may have failed to initialize.")
#define SUGG_IMG_FAILED_TO_RETRIEVE_MEMORY_TYPE                                                                        \
   ("Set options.memoryType explicitly if programmer cannot auto-detect.")
#define SUGG_IMG_INVALID_DEVICE_IMAGE_MODE                                                                             \
   ("Only invoke doEdlSwitch when device is in SAHARA_DOWNLOAD.")
#define SUGG_EDL_SWITCH_NOT_AVAILABLE                                                                                   \
   ("Ensure EDL switching is supported for this device/platform.")
#define SUGG_IMG_FILE_NOT_FOUND(operationName)                                                                         \
   ("Verify the image file path exists and is readable before " + operationName + ".")

/*Manager*/
#define SUGG_DEVICE_HANDLE_NOT_FOUND                                                                                    \
   ("Verify device is connected and handle is valid.")
#define SUGG_PROTOCOL_HANDLE_NOT_FOUND                                                                                  \
   ("Verify protocol is registered and handle is valid.")
#define SUGG_READ_ACCESS_LOCKED                                                                                         \
   ("Release or acquire read access; only one read-exclusive client per protocol.")
#define SUGG_WRITE_ACCESS_LOCKED                                                                                        \
   ("Release or acquire write access; only one write-exclusive client per protocol.")
#define SUGG_MHI_EDL_NOT_SUPPORTED                                                                                      \
   ("MHI EDL switch is not supported on this device/protocol; use the standard EDL entry method instead.")
#define SUGG_RELATIVE_FILE_PATH                                                                                         \
   ("Provide absolute file paths; relative paths are not supported.")
#define SUGG_FILE_SAVE_FAILED                                                                                           \
   ("Verify file path is writable and has sufficient disk space.")
#define SUGG_DIRECTORY_ITERATION_FAILED                                                                                \
   ("Verify the directory exists, is readable, and contains accessible entries.")

/*Connection*/
#define SUGG_NULL_PACKET_SEND                                                                                           \
   ("Validate buffer before calling send; ensure buffer is not null.")
#define SUGG_NO_WRITE_ACCESS_SEND                                                                                        \
   ("Open the connection with WRITE access before sending.")
#define SUGG_TRANSACTION_ID_MISMATCH                                                                                     \
   ("Only pass transaction IDs returned by sendAsync on same Connection, before consumption/cancel.")
#define SUGG_NO_ASYNC_RESPONSE                                                                                           \
   ("Use non-zero timeout when polling, or check isAsyncRequestFinished first.")
#define SUGG_NO_WRITE_ACCESS_CANCEL                                                                                      \
   ("Open the connection with WRITE access before cancelling.")

/*Buffer*/
#define SUGG_NULL_BUFFER                                                                                                 \
   ("Include buffer length and calling context; validate buffer before use.")
#define SUGG_INSUFFICIENT_BYTES                                                                                          \
   ("Include actual length and required sizeof(*out); ensure buffer has sufficient bytes.")
#define SUGG_NULL_SHARED_BUFFER                                                                                          \
   ("Distinguish null smart-pointer vs null underlying buffer; validate before cast.")

/*FunctionTracker*/
#define SUGG_SERVICE_NOT_INITIALIZED                                                                                     \
   ("Ensure initializeService() is called before any RPC entry using DEVICE_RPC_TRY.")

/*ImageTransfer*/
#define SUGG_NO_PACKET_RESET_STATE                                                                                        \
   ("Verify USB cable/EDL mode is stable; ensure reset-state-machine cmd reached device.")
#define SUGG_MAX_RESET_CYCLES_EXCEEDED                                                                                    \
   ("Perform hard/power reset; warm-reset cap exhausted.")
#define SUGG_NO_PACKET_COMMAND_STATE                                                                                      \
   ("Check device is still in command mode; increase getFrame timeout.")
#define SUGG_COMMAND_RESPONSE_NOT_RECEIVED                                                                                \
   ("Ensure reset response arrives; power-cycle if desynced.")
#define SUGG_COMMAND_MODE_NOT_AVAILABLE                                                                                   \
   ("Confirm target is in EDL/image_tx_pending; only EDL supports command mode.")
#define SUGG_COMMAND_READY_NOT_RECEIVED                                                                                   \
   ("Ensure Hello-response mode switch succeeded; boot a compatible programmer.")
#define SUGG_INVALID_RESPONSE_LENGTH                                                                                      \
   ("Device must support read commands with >= 4 bytes.")
#define SUGG_INVALID_PROGRAMMER                                                                                           \
   ("Pass either firehoseProgPath or a saharaImageList with IMAGE_ID_EDL_PROGRAMMER before calling transferFirehoseProgrammer.")
#define SUGG_CONFLICTING_PROGRAMMER_OPTIONS                                                                               \
   ("Don't set both firehoseProgPath and a non-empty IMAGE_ID_EDL_PROGRAMMER entry.")
#define SUGG_PROTOCOL_MODE_UNKNOWN                                                                                        \
   ("Ensure firmware/host versions are compatible.")
#define SUGG_READ_DATA_NOT_RECEIVED                                                                                       \
   ("Confirm target sent valid read data after Hello-response.")
#define SUGG_IMAGE_REJECTED_BY_DEVICE                                                                                     \
   ("Verify image is signed for/compatible with this SoC.")
#define SUGG_INVALID_TRANSFER_STATUS                                                                                      \
   ("Check firmware compatibility; response had unexpected status.")
#define SUGG_FILE_READ_FAILED                                                                                             \
   ("Verify file exists, is accessible, and offset+length within its size.")
#define SUGG_INVALID_MEMORY_TYPE                                                                                          \
   ("Call firehoseSetMemoryType() with EMMC/UFS/NAND/NVME/SPINOR before firehose commands.")
#define SUGG_INVALID_DIGEST_HEADER_TYPE                                                                                   \
   ("Pass one of DIGEST_HEADER_TYPE_NONE/MBN/ELF.")
#define SUGG_MISSING_SIGNED_DIGESTS                                                                                       \
   ("When providing chainedDigestsFile, also supply signedDigestsFile.")
#define SUGG_NO_PROGRAM_FILE_AVAILABLE                                                                                    \
   ("Provide at least one of singleImage, jsonFile, or rawprogram/patch XML lists.")
#define SUGG_INVALID_PARTITION_NUMBER                                                                                     \
   ("Partition indices must be >= 0.")
#define SUGG_NO_VALIDATION_FILE_AVAILABLE                                                                                 \
   ("For VALIDATION_MODE_*_WITH_DIGESTS_FILE, set m_validationDigestsFile via firehoseSetBuildValidationDigests().")
#define SUGG_NO_RESPONSE_DEVICE_INVALID_STATE                                                                             \
   ("Reconnect/reboot to EDL; timeout waiting for ready.")
#define SUGG_DEVICE_NOT_READY                                                                                             \
   ("Device kept returning non-ready past max attempts; hard reset.")
#define SUGG_ERROR_DUMPING_FILE(command)                                                                                  \
   ("Verify " + command + " command support on this firmware.")
#define SUGG_SOC_VERSION_NOT_SUPPORTED                                                                                    \
   ("Target firmware must expose sufficient size for version info; use a newer programmer.")
#define SUGG_BOOT_CONFIG_NOT_SUPPORTED                                                                                    \
   ("Ensure firmware returns sufficient size for boot config.")
#define SUGG_FAILED_RETRIEVE_MEMORY_TYPE_DB                                                                               \
   ("DCL lookup disabled; set memory type explicitly via firehoseSetMemoryType.")
#define SUGG_LOADER_NOT_ACTIVE_IMG                                                                                        \
   ("Ensure loader instantiated successfully; check memory/library init.")
#define SUGG_INVALID_OFFLINE_PROCESS                                                                                      \
   ("Pass PROCESS_VIP_DIGEST or PROCESS_BUILD_VALIDATION_DIGEST.")
#define SUGG_MIBIB_MAGIC_NOT_FOUND                                                                                        \
   ("Confirm file is a valid NAND MIBIB; magic must appear in first 32 blocks.")
#define SUGG_NO_COMMAND_DATA_RECEIVED(command)                                                                            \
   ("Confirm target supports the " + command + " command and returns response data.")
#define SUGG_END_OF_TRANSFER_FOR_COMMAND(command)                                                                         \
   ("Device aborted the " + command + " command; inspect the EndOfImageTransfer status code.")
#define SUGG_COMMAND_EXECUTE_RESPONSE_NOT_RECEIVED                                                                        \
   ("Ensure device replies with SAHARA_COMMAND_EXECUTE_RESP or SAHARA_END_OF_IMAGE_TRANS.")
#define SUGG_PARTITION_HEADER_FILE_TOO_SMALL(requiredSize)                                                                \
   ("Verify the partition-header file is at least " + requiredSize + " bytes.")
#define SUGG_PARTITION_HEADER_SIGNATURE_MISMATCH(expectedSignature)                                                       \
   ("Confirm the file is a valid partition header with signature " + expectedSignature + ".")
#define SUGG_PARTITION_ENTRY_SIZE_INVALID(requiredSize)                                                                   \
   ("Pass an entrySize >= " + requiredSize + " bytes.")
#define SUGG_PARTITION_ENTRY_FILE_TOO_SMALL(requiredSize)                                                                  \
   ("Verify the partition-entry file is at least " + requiredSize + " bytes.")
#define SUGG_PARTITION_HEAD_NUMBER_INVALID(expectedCount)                                                                  \
   ("Confirm numberOfPartitionHeaderEntries equals " + expectedCount + ".")
#define SUGG_MIBIB_PARTITION_SIZE_MISMATCH(expectedSize)                                                                  \
   ("Confirm partitionHeader.sizeOfPartitionEntry equals " + expectedSize + ".")
#define SUGG_NAND_INFO_INVALID(pageSize, pagePerBlock)                                                                    \
   ("Run getFlashInfo in advance; current pageSize=" + pageSize + ", pagePerBlock=" + pagePerBlock + ".")
#define SUGG_SECTOR_SIZE_MISMATCH(patchSectorSize, partitionSectorSize)                                                   \
   ("Ensure patch (" + patchSectorSize + ") and partition (" + partitionSectorSize +                                     \
    ") sector sizes match, or are zero/default.")
#define SUGG_PARTITION_FILE_TOO_SMALL_FOR_PATCH(requiredSize)                                                             \
   ("Verify the target partition is at least " + requiredSize + " bytes.")
#define SUGG_PARTITION_FILE_TOO_SMALL_FOR_HEADER(requiredSize)                                                            \
   ("Verify the partition file is at least " + requiredSize + " bytes.")

/*Base Protocol*/
#define SUGG_PROTOCOL_LOCK_NOT_SUPPORTED                                                                                 \
   ("Use a protocol implementation that supports locking (e.g. Firehose), or avoid calling lock() on this protocol.")
#define SUGG_PROTOCOL_UNLOCK_NOT_SUPPORTED                                                                              \
   ("Use a protocol implementation that supports unlocking (e.g. Firehose), or avoid calling unlock() on this protocol.")
#define SUGG_SEND_SYNC_WITH_KEY_NOT_SUPPORTED                                                                           \
   ("Use a protocol implementation that supports keyed sends (e.g. Firehose), or use sendSync() without a key.")
#define SUGG_SEND_ASYNC_WITH_KEY_NOT_SUPPORTED                                                                          \
   ("Use a protocol implementation that supports keyed sends (e.g. Firehose), or use sendAsync() without a key.")

/*Sahara Protocol - Connection*/
#define SUGG_SAHARA_PROTOCOL_UNAVAILABLE                                                                                \
   ("Ensure the Sahara protocol is connected before attempting to use it; reconnect the device if necessary.")

/*FlowControl*/
#define SUGG_FLOW_CONTROL_WATERMARKS_NOT_SET                                                                            \
   ("Call FlowControl::setFlowControlWatermarks(low, high, dne) before invoking updateFlowControlCount.")

/*Usb*/
#define SUGG_USB_CONNECTION_OPEN_FAILURE                                                                                \
   ("Verify the USB device is connected, drivers are installed, and the identifier is correct.")

/*SystemHelper*/
#define SUGG_CREATE_DIRECTORY_FAILED                                                                                    \
   ("Verify the parent directory exists and the current user has permission to create directories there.")
#define SUGG_CREATE_SECURITY_DESCRIPTOR_FAILED                                                                          \
   ("Verify the security descriptor string is valid for this Windows version; run with elevated privileges if needed.")
#define SUGG_CREATE_DIRECTORY_WINDOWS_FAILED                                                                            \
   ("Check the Windows error code and verify the current user has permission to create directories in this location.")
#define SUGG_CREATE_DIRECTORY_LINUX_FAILED                                                                             \
   ("Check errno/permissions and verify the current user has permission to create directories in this location.")
#define SUGG_TEMP_FILE_CREATION_FAILED                                                                                  \
   ("Verify the temporary directory exists and is writable, then check available disk space and permissions.")
#define ERR_PROTOCOL_UNAVAILABLE ("Protocol unavailable")
#define DESC_PROTOCOL_UNAVAILABLE(protocol) (protocol + " Protocol Unavailable")
#define SUGG_PROTOCOL_UNAVAILABLE(protocol)                                                                              \
   ("Ensure " + protocol + " protocol is in a connectable state (not STATE_DISCONNECTED).")
#define ERR_ACCESS_VIOLATION ("Access violation")
#define ERR_DIVIDE_BY_ZERO ("Divide by zero")
#define ERR_STRUCTURED_EXCEPTION ("Structured exception")
#define SUGG_ACCESS_VIOLATION                                                                                             \
   ("Guard pointer dereferences and validate memory before access.")
#define SUGG_DIVIDE_BY_ZERO                                                                                              \
   ("Check the divisor for zero before division.")
#define SUGG_STRUCTURED_EXCEPTION                                                                                         \
   ("Handle the specific SEH code and investigate the underlying cause.")

// POC's
#define TARGET ("Target Team")
#define CE ("Qualcomm CE Team")
#define METABUILD ("Meta build Team")
#define BOOT_STORAGE ("Boot Storage Team")
#define BOOT ("Boot Team")
#define CLIENT ("Clients, ex: Axiom, PCAT")

#ifdef INTERNAL_BUILD
#define POC(team) team
#else
#define POC(team) CE
#endif

} // namespace Device

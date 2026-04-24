// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

namespace Device {

static const std::string DEVICE_STORAGE_OPEN_FAILURE_PATTERN = ".*?Failed to open.*?";
static const std::string FIREHOSE_IMAGE_NOT_FOUND_PATTERN = ".*?Software image: *?";
static const std::string FIREHOSE_SIGNATURE_VERIFICATION_PATTERN = ".*?Verifying signature failed.*?";

// Errors
/*Sahara Protocol*/
#define ERR_SAHARA_IMAGE_NOT_FOUND(imageId) ("Sahara image Id: " + imageId + " could not be found in the build path")
#define ERR_FIREHOSE_PROGRAMMER_NOT_FOUND ("Firehose programmer could not be found in the build path")
#define ERR_MEMDUMP_DOWNLOAD_FAILURE(partitionName)                                                                    \
   ("Failed to download dump partition: " + partitionName + " from the device.")
#define ERR_DEVICE_STORAGE_OPEN_FAILURE ("Failed to open device storage or flash memory for download")
#define ERR_SAHARA_PROTOCOL_RESET(imageFile, errorCodeString)                                                          \
   ("Specified device programmer file: '" + imageFile +                                                                \
    "' is invalid. A Sahara reset signal was received from the target "                                                \
    "device, indicating a failure during the programming process. Error "                                              \
    "details: '" +                                                                                                     \
    errorCodeString + "'.")
#define ERR_INVALID_EDL_STATE                                                                                          \
   ("Device has entered an invalid EDL state due to the download of "                                                  \
    "incorrect images")

/*QMI Protocol*/
#define ERR_QMI_SERVICE_INIT_FAILURE(serviceId) ("QMI Service with service Id" + serviceId + "could not be initialized")
#define ERR_QMI_MESSAGE_ID_MISMATCH_ICD(messageIdOrName)                                                               \
   ("Invalid QMI Message:" + messageIdOrName +                                                                         \
    " The QMI message ID does not match with the ICD document format or QMI "                                          \
    "Service XMLs")

/*Firehose Protocol*/
#define ERR_IMAGE_NOT_FOUND(imageName) ("Software image: " + imageName + " could not be found in the build path")
#define ERR_SIGNATURE_VERIFICATION_FAILED ("Signature verification failed for the build")

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

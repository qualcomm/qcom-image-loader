// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "CliOptions.h"

namespace QC {
namespace CLI {

/**
 * @brief Event IDs for DLL service events (matching Firehose protocol events)
 */
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
   FIREHOSE_INFORMATION = 255
};

/**
 * @brief Command execution logic
 *
 * Executes commands based on parsed CLI options
 */
class CliCommands
{
public:
   /**
    * @brief Execute command based on options
    *
    * @param options Parsed CLI options
    * @return Exit code (0 = success, non-zero = error)
    */
   static int execute(const CliOptions& options);

private:
   /**
    * @brief List all available devices
    *
    * @return Exit code
    */
   static int listDevices();

   /**
    * @brief Download build to device
    *
    * @param options CLI options
    * @return Exit code
    */
   static int downloadBuild(const CliOptions& options);

   /**
    * @brief Create build validtion digest
    *
    * @param options CLI options
    * @return Exit code
    */
   static int createDigest(const CliOptions& options);

   /**
    * @brief Erase flash
    *
    * @param options CLI options
    * @return Exit code
    */
   static int eraseFlash(const CliOptions& options);

   /**
    * @brief Get flash information
    *
    * @param options CLI options
    * @return Exit code
    */
   static int getFlashInfo(const CliOptions& options);

   /**
    * @brief Reset device in firehose mode
    *
    * @param options CLI options
    * @return Exit code
    */
   static int resetDevice(const CliOptions& options);

   /**
    * @brief Find target device by identifier
    *
    * @param deviceIdentifier Device identifier to search for.
    */
   static QC::DeviceInfo findTargetDevice(const std::string& deviceIdentifier);

   /**
    * @brief Print download build options
    *
    * @param downloadOptions downloadBuildOptions to print
    */
   static void printDownloadOptions(const QC::DownloadBuildOptions downloadOptions);

   /**
    * @brief Get QFS Software Download Application version
    *
    * @return Exit code
    */
   static int displayVersion();

   /**
    * @brief Print digest file location after creation
    *
    * @param options CLI options
    */
   static void printDigestFileLocation(const CliOptions& options);

   /**
    * @brief Get the protocol name from enum value
    *
    * @param type Protocol enum
    */
   static std::string getProtocolName(QC::ProtocolType type);
};

} // namespace CLI
} // namespace QC

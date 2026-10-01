// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "MetaCli.h"
#include "../exports/ImageManagementDefinitions.h"

namespace QC {
namespace Function {

/**
 * @brief Core flatten operation for metabuild
 *
 * Parses metabuild XML, processes metadata, and generates
 * flattened output structure.
 */
class FlattenMetaBuild
{
public:
   FlattenMetaBuild();
   ~FlattenMetaBuild();

   /**
    * @brief Execute flatten operation
    *
    * @param meta MetaCli instance for querying build metadata
    * @param outputPath Output directory path
    * @param memoryType Memory type (e.g., "UFS", "EMMC")
    * @param productFlavor Product flavor
    * @param skuConfig SKU configuration (optional)
    * @param skuConfigsExist Whether SKU configs exist for this build
    * @return true if successful, false otherwise
    */
   /**
    * @brief Execute flatten operation with validation
    *
    * @param buildPath Path to the build
    * @param options Build options (memoryType, productFlavor, skuConfig, outputPath)
    * @return true if successful, false otherwise
    */
   bool flatten(
       const std::string& buildPath,
       const FlattenMetaBuildOptions& options
   );

private:
   std::vector<std::string> imagesUnderFolder;
   std::string memoryTypeSubdirectoryPath;  // Cached path to avoid recalculation

   bool flattenMetabuild(
       MetaCli& meta,
       const std::string& outputPath,
       const std::string& memoryType,
       const std::string& productFlavor,
       const std::string& skuConfig,
       bool skuConfigsExist = false
   );

   bool validateOutputPath(const std::string& outputPath);
   bool createOutputStructure(const std::string& outputPath, const std::string& memoryType);
   bool processMetadata(MetaCli& meta,
                       const std::string& outputPath,
                       const std::string& memoryType,
                       const std::string& productFlavor,
                       const std::string& skuConfig,
                       bool skuConfigsExist);
   bool copyFiles(std::vector<std::filesystem::path>& files,
                  const std::string& localFlatBuildPath,
                  long long& totalBytesTransferred,
                  long long totalBytesToTransfer);
   bool copyFileWithProgress(const std::string& source,
                            const std::string& destination,
                            long long& totalBytesTransferred,
                            long long totalBytesToTransfer);
   std::vector<std::string> parseRawXmlForFilenames(const std::filesystem::path& xmlPath);
};

} // namespace Function
} // namespace QC

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <string>
#include <vector>

namespace QC {
namespace CLI {

/**
 * @brief CDT (Configuration Data Table) utility functions
 *
 * This class provides utilities for handling CDT binary files and
 * modifying rawprogram XML files to reference custom CDT binaries.
 */
class Cdt
{
public:
   /**
    * @brief Result structure for CDT operations
    */
   struct CdtResult
   {
      bool success = false;
      std::string error;
      std::string modifiedXmlPath;
      std::string cdtBinaryPath;
      std::vector<std::string> updatedRawXmlList;
   };

   /**
    * @brief Modify rawprogram XML files to use a custom CDT binary
    *
    * @param cdtBinaryPath Path to the CDT binary file
    * @param buildPath Path to the build directory
    * @param rawXmlList List of rawprogram XML files to process
    * @return CdtResult Result of the CDT modification operation
    */
   static CdtResult modifyCdtRawprogramXml(
      const std::string& cdtBinaryPath,
      const std::string& buildPath,
      const std::vector<std::string>& rawXmlList
   );

   /**
    * @brief Check if a rawprogram XML file contains CDT entries
    *
    * @param xmlPath Path to the rawprogram XML file
    * @return true if the XML contains CDT entries, false otherwise
    */
   static bool containsCdtEntry(const std::string& xmlPath);

   /**
    * @brief Extract CDT information from rawprogram XML files
    *
    * @param rawXmlList List of rawprogram XML files
    * @return Information about CDT partitions found
    */
   static std::string getCdtInfo(const std::vector<std::string>& rawXmlList);

private:
   /**
    * @brief Generate a unique temporary XML filename
    *
    * @param originalXmlPath Original XML file path
    * @return Unique temporary filename
    */
   static std::string generateTempXmlPath(const std::string& originalXmlPath);

   /**
    * @brief Copy CDT binary to build directory
    *
    * @param sourcePath Source CDT binary path
    * @param buildPath Target build directory
    * @return true if copy successful, false otherwise
    */
   static bool copyCdtToBuild(const std::string& sourcePath, const std::string& buildPath);

   /**
    * @brief Get current timestamp string for unique filenames
    *
    * @return Timestamp string in format YYYYMMDD_HHMMSS
    */
   static std::string getCurrentTimeString();
};

} // namespace CLI
} // namespace QC

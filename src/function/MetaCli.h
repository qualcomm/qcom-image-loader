// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace QC {
namespace Function {

struct MetaCliResponse
{
   bool status;
   std::string output;
   std::string errorOutput;
   int exitCode;
};

struct PartitionImages
{
   std::vector<std::filesystem::path> partition;
   std::vector<std::filesystem::path> partition_patch;
   std::vector<std::filesystem::path> partition_bin;
};

struct ProgrammerConfig
{
   std::vector<std::filesystem::path> programmer_bin;
   std::vector<std::filesystem::path> programmer_config;
};

class MetaCli
{
private:
   std::string contentsXml;
   std::string buildPath;
   std::string metaCliPath;
   bool isMetaCliPy;

   std::string getMetaCliPath(const std::string& type);
   MetaCliResponse executeCommand(const std::string& args, int timeoutMs = 300000);
   std::string getPythonExecutable();
   std::vector<std::string> getFiles(
       const std::vector<std::string>& fileTypes,
       const std::string& memoryType = "",
       const std::string& productFlavor = "",
       const std::string& build = "",
       const std::string& attribute = "",
       const std::string& fileName = "");

public:
   MetaCli(const std::string& contentsXml);
   ~MetaCli();

   bool exists() const;
   std::string getBuildPath() const { return buildPath; }

   std::vector<std::string> getMemoryTypes();
   std::vector<std::string> getProductFlavours();
   std::vector<std::string> getSkuConfigs(const std::string& memoryType = "",
                                          const std::string& productFlavor = "");
   PartitionImages getPartitionImages(const std::string& memoryType,
                                      const std::string& productFlavor,
                                      const std::string& skuConfig = "");
   std::string getDeviceProgrammer(const std::string& memoryType,
                                   const std::string& productFlavor);
   ProgrammerConfig getProgrammerConfig(const std::string& memoryType,
                                        const std::string& productFlavor);
};

} // namespace Function
} // namespace QC


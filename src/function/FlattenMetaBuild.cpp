// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "FlattenMetaBuild.h"

#include "../util/FileSystem.h"
#include "MetaCli.h"
#include "callback/ClientCallbackHandler.h"
#include "util/StringHelper.h"

#include "cli/Spinner.h"

#include <KL/kLogger.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <errno.h>
#endif

namespace fs = std::filesystem;

namespace QC {
namespace Function {

static QC::CLI::Spinner g_spinner;
static size_t g_maxFileNameLength = 0;

static size_t addSize(std::vector<std::string>& files, long long& totalBytesToTransfer)
{
   size_t maxLen = 0;
   for(const auto& f : files)
   {
      totalBytesToTransfer += Util::FileSystem::getFileSize(f);
      size_t nameLength = Util::FileSystem::getFileName(f).string().length();
      if(nameLength > maxLen)
         maxLen = nameLength;
   }
   return maxLen;
}

static size_t addSize(std::vector<std::filesystem::path>& files, long long& totalBytesToTransfer)
{
   size_t maxLen = 0;
   for(const auto& f : files)
   {
      totalBytesToTransfer += Util::FileSystem::getFileSize(f.string());
      size_t nameLength = Util::FileSystem::getFileName(f).string().length();
      if(nameLength > maxLen)
         maxLen = nameLength;
   }
   return maxLen;
}

FlattenMetaBuild::FlattenMetaBuild()
{
}

FlattenMetaBuild::~FlattenMetaBuild()
{
}

bool FlattenMetaBuild::flatten(
    const std::string& buildPath,
    const FlattenMetaBuildOptions& options)
{
   MetaCli meta(buildPath);

   if(!meta.exists())
   {
      std::string msg = "meta_cli not found";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      throw std::runtime_error(msg);
   }

   // Get valid memory types and flavors
   std::vector<std::string> validMemoryTypes = meta.getMemoryTypes();
   if(validMemoryTypes.empty())
   {
      throw std::runtime_error("No memory types available");
   }

   std::vector<std::string> validFlavors = meta.getProductFlavours();
   if(validFlavors.empty())
   {
      throw std::runtime_error("No product flavors available");
   }

   // Validate options
   std::string memoryTypeLower = Util::toLowerCopy(options.memoryType);
   std::string productFlavorLower = Util::toLowerCopy(options.productFlavor);
   std::string skuConfigLower = Util::toLowerCopy(options.skuConfig);

   bool validMemoryType = false;
   for(const auto& mt : validMemoryTypes)
   {
      if(Util::toLowerCopy(mt) == memoryTypeLower)
      {
         validMemoryType = true;
         break;
      }
   }

   if(!validMemoryType)
   {
      std::string msg = "Invalid memory type: " + options.memoryType + ". Available: ";
      for(size_t i = 0; i < validMemoryTypes.size(); ++i)
      {
         msg += validMemoryTypes[i];
         if(i + 1 < validMemoryTypes.size())
            msg += ", ";
      }
      throw std::runtime_error(msg);
   }

   bool validFlavor = false;
   for(const auto& f : validFlavors)
   {
      if(Util::toLowerCopy(f) == productFlavorLower)
      {
         validFlavor = true;
         break;
      }
   }

   if(!validFlavor)
   {
      std::string msg = "Invalid product flavor: " + options.productFlavor + ". Available: ";
      for(size_t i = 0; i < validFlavors.size(); ++i)
      {
         msg += validFlavors[i];
         if(i + 1 < validFlavors.size())
            msg += ", ";
      }
      throw std::runtime_error(msg);
   }

   // Get SKU configs if needed
   std::vector<std::string> validSkuConfigs = meta.getSkuConfigs(memoryTypeLower, productFlavorLower);
   bool skuConfigsExist = !validSkuConfigs.empty();

   if(options.__isset.skuConfig && !options.skuConfig.empty() && skuConfigsExist)
   {
      bool validSku = false;
      for(const auto& sku : validSkuConfigs)
      {
         if(Util::toLowerCopy(sku) == skuConfigLower)
         {
            validSku = true;
            break;
         }
      }

      if(!validSku)
      {
         std::string msg = "Invalid SKU config: " + options.skuConfig + ". Available: ";
         for(size_t i = 0; i < validSkuConfigs.size(); ++i)
         {
            msg += validSkuConfigs[i];
            if(i + 1 < validSkuConfigs.size())
               msg += ", ";
         }
         throw std::runtime_error(msg);
      }
   }

   // Execute flatten operation
   std::string outputPath = options.__isset.outputPath && !options.outputPath.empty()
                                ? options.outputPath
                                : ".";

   // Append build directory name to output path
   fs::path buildDirPath = Util::FileSystem::getDirectoryName(buildPath);
   std::string buildDirName = buildDirPath.filename().string();
   if(!buildDirName.empty())
   {
      outputPath = Util::FileSystem::combinePath({outputPath, buildDirName}).string();
   }

   return flattenMetabuild(meta, outputPath, memoryTypeLower, productFlavorLower, skuConfigLower, skuConfigsExist);
}

bool FlattenMetaBuild::flattenMetabuild(
    MetaCli& meta,
    const std::string& outputPath,
    const std::string& memoryType,
    const std::string& productFlavor,
    const std::string& skuConfig,
    bool skuConfigsExist)
{
   if(!validateOutputPath(outputPath))
   {
      FLOG_ERROR("Invalid output path: " + outputPath);
      throw std::runtime_error("Invalid output path: " + outputPath);
   }

   if(!createOutputStructure(outputPath, memoryType))
   {
      std::string errorMsg = "Failed to create output directory structure";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         errorMsg += " - " + lastError;
      FLOG_ERROR(errorMsg);
      throw std::runtime_error(errorMsg);
   }

   if(!processMetadata(meta, outputPath, memoryType, productFlavor, skuConfig, skuConfigsExist))
   {
      std::string errorMsg = "Failed to process metadata";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         errorMsg += " - " + lastError;
      FLOG_ERROR(errorMsg);
      throw std::runtime_error(errorMsg);
   }

   g_spinner.finish("Flatten operation completed successfully");
   return true;
}

bool FlattenMetaBuild::validateOutputPath(const std::string& outputPath)
{
   if(outputPath.empty())
   {
      throw std::runtime_error("Output path is empty");
   }

   return true;
}

bool FlattenMetaBuild::createOutputStructure(const std::string& outputPath, const std::string& memoryType)
{
   fs::path outPath(outputPath);
   if(!Util::FileSystem::createDirectory(outPath))
   {
      std::string msg = "Failed to create output directory: " + outputPath;
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      throw std::runtime_error(msg);
   }

   FLOG_INFO("Created output directory: " + outputPath);

   // Create FIREHOSE subdirectory
   std::string fireHosePath = Util::FileSystem::combinePath({outputPath, "FIREHOSE"}).string();
   if(!Util::FileSystem::createDirectory(fireHosePath))
   {
      std::string msg = "Failed to create FIREHOSE directory: " + fireHosePath;
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      throw std::runtime_error(msg);
   }

   // Create memory-type subdirectory using the member variable
   memoryTypeSubdirectoryPath = Util::FileSystem::combinePath({outputPath, "FIREHOSE", ::Util::toUpperCopy(memoryType)}).string();
   if(!Util::FileSystem::createDirectory(memoryTypeSubdirectoryPath))
   {
      std::string msg = "Failed to create memory-type directory: " + memoryTypeSubdirectoryPath;
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      throw std::runtime_error(msg);
   }

   FLOG_INFO("Created directory structure: " + memoryTypeSubdirectoryPath);
   return true;
}

bool FlattenMetaBuild::processMetadata(
    MetaCli& meta,
    const std::string& outputPath,
    const std::string& memoryType,
    const std::string& productFlavor,
    const std::string& skuConfig,
    bool skuConfigsExist)
{
   PartitionImages partitionImages = meta.getPartitionImages(memoryType, productFlavor, skuConfig);

   // Check for license file when SKU configs are in use
   if(skuConfigsExist && !partitionImages.partition_bin.empty())
   {
      bool found = false;
      for(const auto& filePath : partitionImages.partition_bin)
      {
         std::string fileName = Util::FileSystem::getFileName(filePath).string();
         if(fileName == "license.bin")
         {
            found = true;
            QC::ClientCallbackHandler::getInstance()->onMessage(
               QC::MessageLevel::INFO, "FlattenMeta", "SKU License", "SKU license path: " + filePath.string());
            FLOG_INFO("SKU license path: " + filePath.string());
            break;
         }
      }
      if(!found && !skuConfig.empty())
      {
         QC::ClientCallbackHandler::getInstance()->onMessage(
            QC::MessageLevel::WARNING, "FlattenMeta", "SKU License", "No license.bin file found for SKU: " + skuConfig);
         FLOG_WARNING("No license.bin file found for SKU: " + skuConfig);
      }
   }

   if(partitionImages.partition.empty())
   {
      std::string msg = "Partition raw images not found in the meta build for the combination of memory type: " + memoryType +
                        ", flavor: " + productFlavor;
      if(!skuConfig.empty())
         msg += ", SKU: " + skuConfig;
      throw std::runtime_error(msg);
   }

   if(partitionImages.partition_patch.empty())
   {
      std::string msg = "Partition patch images not found in the meta build for the combination of memory type: " + memoryType +
                        ", flavor: " + productFlavor;
      if(!skuConfig.empty())
         msg += ", SKU: " + skuConfig;
      throw std::runtime_error(msg);
   }

   if(partitionImages.partition_bin.empty())
   {
      std::string msg = "Partition bin images not found in the meta build for the combination of memory type: " + memoryType +
                        ", flavor: " + productFlavor;
      if(!skuConfig.empty())
         msg += ", SKU: " + skuConfig;
      throw std::runtime_error(msg);
   }

   std::string ufsPath = memoryTypeSubdirectoryPath;

   ProgrammerConfig programmerConfig = meta.getProgrammerConfig(memoryType, productFlavor);

   std::string firehoseProgrammer;
   if(!programmerConfig.programmer_config.empty())
   {
      firehoseProgrammer = programmerConfig.programmer_config[0].string();
   }
   else
   {
      firehoseProgrammer = meta.getDeviceProgrammer(memoryType, productFlavor);
   }

   if(firehoseProgrammer.empty())
   {
      throw std::runtime_error("Device programmer not found in the meta build");
   }

   long long totalBytesToTransfer = 0;
   long long totalBytesTransferred = 0;

   size_t maxNameLen = 0;
   maxNameLen = std::max(maxNameLen, addSize(partitionImages.partition, totalBytesToTransfer));
   maxNameLen = std::max(maxNameLen, addSize(partitionImages.partition_bin, totalBytesToTransfer));
   maxNameLen = std::max(maxNameLen, addSize(partitionImages.partition_patch, totalBytesToTransfer));
   maxNameLen = std::max(maxNameLen, addSize(programmerConfig.programmer_bin, totalBytesToTransfer));
   totalBytesToTransfer += Util::FileSystem::getFileSize(firehoseProgrammer);
   maxNameLen = std::max(maxNameLen, Util::FileSystem::getFileName(firehoseProgrammer).string().length());

   g_spinner.update(0, "Copying files");

   imagesUnderFolder.clear();
   for(const auto& xmlFile : partitionImages.partition)
   {
      auto filenames = parseRawXmlForFilenames(xmlFile);
      imagesUnderFolder.insert(imagesUnderFolder.end(), filenames.begin(), filenames.end());
   }

   if(!copyFiles(partitionImages.partition, ufsPath, totalBytesTransferred, totalBytesToTransfer))
   {
      g_spinner.stop();
      std::cerr << "Failed to copy partition files" << std::endl;
      FLOG_ERROR("Failed to copy partition files");
      return false;
   }

   if(!copyFiles(partitionImages.partition_patch, ufsPath, totalBytesTransferred, totalBytesToTransfer))
   {
      g_spinner.stop();
      std::cerr << "Failed to copy partition patch files" << std::endl;
      FLOG_ERROR("Failed to copy partition patch files");
      return false;
   }

   if(!copyFiles(partitionImages.partition_bin, ufsPath, totalBytesTransferred, totalBytesToTransfer))
   {
      g_spinner.stop();
      std::cerr << "Failed to copy partition bin files" << std::endl;
      FLOG_ERROR("Failed to copy partition bin files");
      return false;
   }

   if(!copyFiles(programmerConfig.programmer_bin, ufsPath, totalBytesTransferred, totalBytesToTransfer))
   {
      g_spinner.stop();
      std::cerr << "Failed to copy programmer bin files" << std::endl;
      FLOG_ERROR("Failed to copy programmer bin files");
      return false;
   }

   std::string progDestination = Util::FileSystem::combinePath({ufsPath, Util::FileSystem::getFileName(firehoseProgrammer).string()}).string();
   if(!copyFileWithProgress(firehoseProgrammer, progDestination, totalBytesTransferred, totalBytesToTransfer))
   {
      g_spinner.stop();
      std::cerr << "File copy failed" << std::endl;
      FLOG_ERROR("File copy failed");
      return false;
   }

   g_spinner.stop();
   fs::path absolutePath = Util::FileSystem::resolvePath(ufsPath);
   std::cout << "All files copied successfully to '" + absolutePath.string() + "'" << std::endl;
   FLOG_INFO("All files copied successfully to '" + absolutePath.string() + "'");
   return true;
}

bool FlattenMetaBuild::copyFileWithProgress(const std::string& source,
                                            const std::string& destination,
                                            long long& totalBytesTransferred,
                                            long long totalBytesToTransfer)
{
#ifdef _WIN32
   struct ProgressData
   {
      long long* totalTransferred;
      long long totalToTransfer;
      long long startingBytes;
      std::string fileName;
   };

   ProgressData progressData;
   progressData.totalTransferred = &totalBytesTransferred;
   progressData.totalToTransfer = totalBytesToTransfer;
   progressData.startingBytes = totalBytesTransferred;
   progressData.fileName = Util::FileSystem::getFileName(source).string();

   auto progressRoutine = [](
       LARGE_INTEGER TotalFileSize,
       LARGE_INTEGER TotalBytesTransferred,
       LARGE_INTEGER StreamSize,
       LARGE_INTEGER StreamBytesTransferred,
       DWORD dwStreamNumber,
       DWORD dwCallbackReason,
       HANDLE hSourceFile,
       HANDLE hDestinationFile,
       LPVOID lpData) -> DWORD {

      ProgressData* data = static_cast<ProgressData*>(lpData);
      *(data->totalTransferred) = data->startingBytes + TotalBytesTransferred.QuadPart;

      if(data->totalToTransfer > 0)
      {
         int percentage = static_cast<int>((static_cast<double>(*(data->totalTransferred)) / data->totalToTransfer) * 100.0);
         std::string message = "Copying: " + data->fileName + " (" + std::to_string(percentage) + "%)";
         QC::ClientCallbackHandler::getInstance()->onServiceEvent("flattenMeta", 253, message);
      }

      return PROGRESS_CONTINUE;
   };

   BOOL result = CopyFileExW(
       std::filesystem::path(source).wstring().c_str(),
       std::filesystem::path(destination).wstring().c_str(),
       progressRoutine,
       &progressData,
       NULL,
       0
   );

   if(!result)
   {
      Util::FileSystem::setWindowsError(GetLastError());
      std::string msg = "Failed to copy file '" + source + "' to '" + destination + "'";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      throw std::runtime_error(msg);
   }

   return true;

#else
   int srcFd = open(source.c_str(), O_RDONLY);
   if(srcFd < 0)
   {
      Util::FileSystem::setLinuxError(errno);
      std::string msg = "Failed to open source file '" + source + "'";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      throw std::runtime_error(msg);
   }

   int dstFd = open(destination.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
   if(dstFd < 0)
   {
      Util::FileSystem::setLinuxError(errno);
      std::string msg = "Failed to open destination file '" + destination + "'";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      close(srcFd);
      throw std::runtime_error(msg);
   }

   struct stat st;
   if(fstat(srcFd, &st) < 0)
   {
      Util::FileSystem::setLinuxError(errno);
      std::string msg = "Failed to get file size for '" + source + "'";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      close(srcFd);
      close(dstFd);
      throw std::runtime_error(msg);
   }

   off_t fileSize = st.st_size;
   off_t offset = 0;

   while(offset < fileSize)
   {
      ssize_t bytesSent = sendfile(dstFd, srcFd, &offset, fileSize - offset);

      if(bytesSent < 0)
      {
         Util::FileSystem::setLinuxError(errno);
         std::string msg = "Failed to copy file '" + source + "' to '" + destination + "'";
         std::string lastError = Util::FileSystem::getLastError();
         if(!lastError.empty())
            msg += " - " + lastError;
         close(srcFd);
         close(dstFd);
         throw std::runtime_error(msg);
      }

      totalBytesTransferred += bytesSent;

      if(totalBytesToTransfer > 0)
      {
         int percentage = static_cast<int>((static_cast<double>(totalBytesTransferred) / totalBytesToTransfer) * 100.0);
         std::string fileName = Util::FileSystem::getFileName(source).string();
         std::string message = "Copying: " + fileName + " (" + std::to_string(percentage) + "%)";
         QC::ClientCallbackHandler::getInstance()->onServiceEvent("flattenMeta", 253, message);
      }
   }

   close(srcFd);
   close(dstFd);
   return true;
#endif
}

bool FlattenMetaBuild::copyFiles(std::vector<std::filesystem::path>& files,
                                 const std::string& localFlatBuildPath,
                                 long long& totalBytesTransferred,
                                 long long totalBytesToTransfer)
{
   for(size_t i = 0; i < files.size(); ++i)
   {
      const std::filesystem::path& image = files[i];
      std::string destinationPath = Util::FileSystem::combinePath({localFlatBuildPath, Util::FileSystem::getFileName(image).string()}).string();

      FLOG_INFO("Copying file: " + image.string());

      if(!copyFileWithProgress(image.string(), destinationPath, totalBytesTransferred, totalBytesToTransfer))
      {
         g_spinner.stop();
         return false;
      }
   }
   return true;
}

std::vector<std::string> FlattenMetaBuild::parseRawXmlForFilenames(const std::filesystem::path& xmlPath)
{
   std::vector<std::string> filenames;

   // xmlReadFile expects a UTF-8 or native-encoded path string
   // Use path.string() like QFC does
   std::string pathStr = xmlPath.string();

   xmlDocPtr doc = xmlReadFile(pathStr.c_str(), "UTF-8", XML_PARSE_NONET | XML_PARSE_NOBLANKS | XML_PARSE_RECOVER);

   if(!doc)
   {
      std::string msg = "Failed to open XML file '" + pathStr + "'";
      std::string lastError = Util::FileSystem::getLastError();
      if(!lastError.empty())
         msg += " - " + lastError;
      throw std::runtime_error(msg);
   }

   // Parse the XML document
   xmlNodePtr root = xmlDocGetRootElement(doc);
   if(root)
   {
      for(xmlNodePtr node = root->children; node; node = node->next)
      {
         if(node->type == XML_ELEMENT_NODE && xmlStrEqual(node->name, BAD_CAST "program"))
         {
            xmlChar* filename = xmlGetProp(node, BAD_CAST "filename");
            if(filename)
            {
               std::string filenameStr(reinterpret_cast<const char*>(filename));
               if(!filenameStr.empty() && filenameStr != ".")
               {
                  size_t slashPos = filenameStr.find_last_of("/\\");
                  if(slashPos != std::string::npos)
                  {
                     filenames.push_back(filenameStr);
                  }
               }
               xmlFree(filename);
            }
         }
      }
   }

   xmlFreeDoc(doc);
   return filenames;
}

} // namespace Function
} // namespace QC




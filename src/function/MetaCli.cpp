// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "MetaCli.h"
#include "../util/ProcessUtils.h"
#include "../util/FileSystem.h"
#include "util/JsonHelper.h"
#include "util/StringHelper.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

#include <KL/kLogger.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace QC {
namespace Function {

std::string MetaCli::getPythonExecutable()
{
   static const std::string candidates[] = {
       "python3",
       "python",
       "py"
   };
   for(const auto& py : candidates)
   {
      std::string testCmd = py + " --version";
      Util::ProcessResult r = Util::runExecutable(testCmd, 5);
      if(r.exitCode == 0)
         return py;
   }
   return "";
}

MetaCli::MetaCli(const std::string& contentsXml)
    : contentsXml(contentsXml), isMetaCliPy(false)
{
   buildPath = Util::FileSystem::getDirectoryName(contentsXml).string();
   metaCliPath = getMetaCliPath("EXE");

   if(metaCliPath.empty())
   {
      metaCliPath = getMetaCliPath("PY");
      if(!metaCliPath.empty())
         isMetaCliPy = true;
   }
}

MetaCli::~MetaCli()
{
}

bool MetaCli::exists() const
{
   return !metaCliPath.empty();
}

std::string MetaCli::getMetaCliPath(const std::string& type)
{
   std::string result;
   try
   {
      if(!Util::FileSystem::fileExists(contentsXml))
         return result;

      std::string binary = "meta_cli";
#ifdef _WIN32
      binary += ".";
      if(type == "EXE")
         binary += "exe";
      else if(type == "PY")
         binary += "py";
#else
      if(type == "PY")
         binary += ".py";
#endif

      std::string searchPath = Util::FileSystem::combinePath({buildPath, "common", "build", "app"}).string();

      auto files = Util::FileSystem::getFilesInDirectory(searchPath, true);
      for(const auto& f : files)
      {
         if(Util::FileSystem::getFileName(f).string() == binary)
         {
            result = f.string();
            break;
         }
      }
   }
   catch(const std::exception& ex)
   {
      throw std::runtime_error("Failed to get meta_cli path: " + std::string(ex.what()));
   }
   return result;
}

MetaCliResponse MetaCli::executeCommand(const std::string& args, int timeoutMs)
{
   MetaCliResponse response;
   response.status = false;

   try
   {
      if(metaCliPath.empty())
      {
         throw std::runtime_error("meta_cli not found in the meta build");
      }

      std::string command = metaCliPath;
      std::string fullArgs = args;

      if(isMetaCliPy)
      {
         std::string pythonExe = getPythonExecutable();
         if(pythonExe.empty())
         {
            throw std::runtime_error("Python not installed. Download from https://www.python.org/downloads/");
         }
         command = pythonExe;
         fullArgs = "\"" + metaCliPath + "\" --contentsxml=\"" + contentsXml + "\" " + args;
      }

      timeoutMs = 300000;
      std::string fullCommand = command + " " + fullArgs;

      Util::ProcessResult r = Util::runExecutable(fullCommand, timeoutMs / 1000);

      if(r.timedOut)
      {
         std::string msg = "meta_cli timed out after " + std::to_string(timeoutMs / 1000) + " seconds";
         if(!r.output.empty())
            msg += " - " + r.output;
         throw std::runtime_error(msg);
      }
      else if(r.exitCode != 0)
      {
         std::string msg = "meta_cli failed with exit code " + std::to_string(r.exitCode);
         if(!r.output.empty())
            msg += " - " + r.output;
         throw std::runtime_error(msg);
      }

      response.status = (r.exitCode == 0) || (r.timedOut && !r.output.empty());
      response.output = r.output;
      response.errorOutput = r.errorOutput;

   }
   catch(const std::exception& ex)
   {
      throw std::runtime_error("meta_cli exception: " + std::string(ex.what()));
   }
   return response;
}

std::vector<std::string> MetaCli::getMemoryTypes()
{
   MetaCliResponse r = executeCommand("get_storage_types");
   if(r.status)
   {
      try
      {
         return Util::JsonHelper::parseArray(r.output);
      }
      catch(const Util::JsonException& ex)
      {
         throw std::runtime_error("getMemoryTypes JSON parse error: " + std::string(ex.what()) + ". meta_cli output: " + r.output);
      }
   }
   else
   {
      throw std::runtime_error("meta_cli get_storage_types failed: " + r.output);
   }
   return {};
}

std::vector<std::string> MetaCli::getProductFlavours()
{
   MetaCliResponse r = executeCommand("get_product_flavors");
   if(r.status)
   {
      try
      {
         return Util::JsonHelper::parseArray(r.output);
      }
      catch(const Util::JsonException& ex)
      {
         throw std::runtime_error("getProductFlavours JSON parse error: " + std::string(ex.what()) + ". meta_cli output: " + r.output);
      }
   }
   else
   {
      throw std::runtime_error("meta_cli get_product_flavors failed: " + r.output);
   }
   return {};
}

PartitionImages MetaCli::getPartitionImages(
    const std::string& storageType,
    const std::string& productFlavor,
    const std::string& skuconfig)
{
   std::string args = "get_partition_files group=True";

   if(!productFlavor.empty())
      args += " flavor='" + productFlavor + "'";
   if(!storageType.empty())
      args += " storage='" + storageType + "'";
   if(!skuconfig.empty())
      args += " sku_config='" + skuconfig + "'";
   args += " critical=False";

   MetaCliResponse r = executeCommand(args);
   PartitionImages result;

   if(!r.status)
   {
      throw std::runtime_error("meta_cli get_partition_files failed: " + r.output);
   }

   try
   {
      auto obj = Util::JsonHelper::parseObject(r.output);
      for(const auto& [key, values] : obj)
      {
         std::vector<std::filesystem::path> paths;
         for(const auto& val : values)
         {
            paths.push_back(std::filesystem::path(val));
         }

         if(key == "partition_patch")
            result.partition_patch = paths;
         else if(key == "partition_bin")
            result.partition_bin = paths;
         else if(key == "partition")
            result.partition = paths;
      }
   }
   catch(const Util::JsonException& ex)
   {
      throw std::runtime_error("getPartitionImages JSON parse error: " + std::string(ex.what()) + ". meta_cli output: " + r.output);
   }

   return result;
}

std::vector<std::string> MetaCli::getFiles(
    const std::vector<std::string>& fileTypes,
    const std::string& memoryType,
    const std::string& productFlavor,
    const std::string& build,
    const std::string& attribute,
    const std::string& fileName)
{
   std::string args = "get_files";

   if(!fileTypes.empty())
   {
      args += " file_types=\"['";
      for(size_t i = 0; i < fileTypes.size(); ++i)
      {
         args += fileTypes[i];
         if(i + 1 < fileTypes.size())
            args += "','";
      }
      args += "']\"";
   }

   if(!attribute.empty())
      args += " attr='" + attribute + "'";
   if(!productFlavor.empty())
      args += " flavor='" + productFlavor + "'";
   if(!memoryType.empty())
      args += " storage='" + memoryType + "'";
   if(!fileName.empty())
      args += " file_name='" + fileName + "'";
   if(!build.empty())
      args += " build='" + build + "'";

   MetaCliResponse r = executeCommand(args);
   std::vector<std::string> result;

   if(!r.status)
      return result;

   try
   {
      return Util::JsonHelper::parseArray(r.output);
   }
   catch(const Util::JsonException& ex)
   {
      std::string msg = "getFiles JSON parse error: " + std::string(ex.what()) + ". meta_cli output: " + r.output;
      throw std::runtime_error(msg);
   }
   return result;
}

std::string MetaCli::getDeviceProgrammer(
    const std::string& memoryType,
    const std::string& productFlavor)
{
   auto programmers = getFiles({"device_programmer"}, memoryType, productFlavor);

   if(programmers.size() == 1)
      return programmers[0];

   if(programmers.size() > 1)
   {
      for(const auto& p : programmers)
      {
         std::string lower = ::Util::toLowerCopy(p);
         if(lower.find("lite") == std::string::npos)
            return p;
      }
      return programmers[0];
   }
   return "";
}

std::vector<std::string> MetaCli::getSkuConfigs(
    const std::string& memoryType,
    const std::string& productFlavor)
{
   MetaCliResponse r = executeCommand("get_sku_config_list");
   std::vector<std::string> result;

   if(!r.status)
      return result;

   try
   {
      return Util::JsonHelper::parseArray(r.output);
   }
   catch(const Util::JsonException& ex)
   {
      // SKU config list is optional; treat unparseable output as no SKUs
      FLOG_WARNING("getSkuConfigs JSON parse error (ignored): " + std::string(ex.what()) + ". meta_cli output: " + r.output);
   }
   return result;
}

ProgrammerConfig MetaCli::getProgrammerConfig(
    const std::string& memoryType,
    const std::string& productFlavor)
{
   std::string args = "get_device_programmer_config storage=";
   args += memoryType.empty() ? "" : ::Util::toLowerCopy(memoryType);
   args += " flavor=";
   args += productFlavor.empty() ? "" : ::Util::toLowerCopy(productFlavor);

   MetaCliResponse r = executeCommand(args);
   ProgrammerConfig result;

   if(!r.status)
      return result;

   try
   {
      auto obj = Util::JsonHelper::parseObject(r.output);
      for(const auto& [key, values] : obj)
      {
         std::vector<std::filesystem::path> paths;
         for(const auto& val : values)
         {
            paths.push_back(std::filesystem::path(val));
         }

         if(key == "programmer_bin")
            result.programmer_bin = paths;
         else if(key == "programmer_config")
            result.programmer_config = paths;
      }
   }
   catch(const Util::JsonException& ex)
   {
      std::string msg = "getProgrammerConfig JSON parse error: " + std::string(ex.what()) + ". meta_cli output: " + r.output;
      throw std::runtime_error(msg);
   }
   return result;
}

} // namespace Function
} // namespace QC






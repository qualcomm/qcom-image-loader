// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "Cdt.h"

#include "ExceptionHandler.h"
#include "FileSystem.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <KL/kLogger.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sstream>

namespace QC {
namespace CLI {

Cdt::CdtResult Cdt::modifyCdtRawprogramXml(
   const std::string& cdtBinaryPath,
   const std::string& buildPath,
   const std::vector<std::string>& rawXmlList
)
{
   CdtResult result;

   // Validate inputs
   QC_REQUIRE_PARAM(!cdtBinaryPath.empty(), "cdtBinaryPath");

   if(!FileSystem::fileExists(cdtBinaryPath))
   {
      QC_THROW_FILE_ERROR(QC::Common::Exception::FILE_NOT_FOUND, cdtBinaryPath, "CDT binary validation");
   }

   CFLOG_INFO("CDT Image: " + cdtBinaryPath, false);

   QC_REQUIRE_PARAM(!buildPath.empty(), "buildPath");

   if(!FileSystem::isDirectory(buildPath))
   {
      QC_THROW_FILE_ERROR(QC::Common::Exception::DIRECTORY_NOT_FOUND, buildPath, "build directory validation");
   }

   QC_REQUIRE_PARAM(!rawXmlList.empty(), "rawXmlList");

   // Copy the original list to modify
   result.updatedRawXmlList = rawXmlList;


   // Process each XML file to find CDT entries
   for(size_t i = 0; i < result.updatedRawXmlList.size(); ++i)
   {
      const std::string& xmlPath = result.updatedRawXmlList[i];

      if(!FileSystem::fileExists(xmlPath))
      {
         QC_THROW_FILE_ERROR(QC::Common::Exception::FILE_NOT_FOUND, xmlPath, "raw program XML validation");
      }

      // Check if this XML contains CDT entries
      if(!containsCdtEntry(xmlPath))
      {
         continue;
      }

      // Extract just the filename from the CDT binary path
      std::string cdtFilename = FileSystem::getFilename(cdtBinaryPath);

      // Load and modify XML with libxml2
      xmlDocPtr doc = xmlReadFile(xmlPath.c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
      if(!doc)
      {
         QC_THROW_FILE_ERROR(QC::Common::Exception::FILE_READ_ERROR, xmlPath, "opening XML file");
      }

      bool modified = false;
      xmlNodePtr root = xmlDocGetRootElement(doc);
      for(xmlNodePtr node = root ? root->children : nullptr; node; node = node->next)
      {
         if(node->type != XML_ELEMENT_NODE) continue;
         xmlChar* label = xmlGetProp(node, BAD_CAST "label");
         if(!label) continue;
         std::string labelLower = reinterpret_cast<char*>(label);
         xmlFree(label);
         std::transform(labelLower.begin(), labelLower.end(), labelLower.begin(), ::tolower);
         if(labelLower.find("cdt") == std::string::npos) continue;
         xmlSetProp(node, BAD_CAST "filename", BAD_CAST cdtFilename.c_str());
         modified = true;
      }

      if(!modified)
      {
         xmlFreeDoc(doc);
         continue;
      }

      // Generate a unique temporary XML filename
      std::string tempXmlPath = generateTempXmlPath(xmlPath);

      // Save modified XML to temporary file
      if(xmlSaveFormatFileEnc(tempXmlPath.c_str(), doc, "UTF-8", 1) < 0)
      {
         xmlFreeDoc(doc);
         QC_THROW_FILE_ERROR(QC::Common::Exception::FILE_WRITE_ERROR, tempXmlPath, "creating temporary XML file");
      }
      xmlFreeDoc(doc);

      // Update the XML list to use the modified file
      result.updatedRawXmlList[i] = tempXmlPath;
      result.modifiedXmlPath = tempXmlPath;

      // Copy CDT binary to the same temporary directory as the XML
      std::string tempDir = tempXmlPath.substr(0, tempXmlPath.find_last_of("/\\"));
      if(!copyCdtToBuild(cdtBinaryPath, tempDir))
      {
         QC_THROW(
            QC::Common::Exception::UNKNOWN_ERROR,
            "Failed to copy CDT binary to temporary directory",
            "Source: " + cdtBinaryPath + ", Target: " + tempDir
         );
      }

      result.cdtBinaryPath = cdtBinaryPath;
      result.success = true;
      break; // We found and processed the CDT XML, no need to continue
   }

   if(!result.success)
   {
      QC_THROW(
         QC::Common::Exception::VALIDATION_ERROR,
         "No CDT entries found in any of the provided XML files",
         "Checked " + std::to_string(rawXmlList.size()) + " XML file(s)"
      );
   }

   return result;
}

bool Cdt::containsCdtEntry(const std::string& xmlPath)
{
   xmlDocPtr doc = xmlReadFile(xmlPath.c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
   if(!doc) return false;
   bool found = false;
   xmlNodePtr root = xmlDocGetRootElement(doc);
   for(xmlNodePtr node = root ? root->children : nullptr; node && !found; node = node->next)
   {
      if(node->type != XML_ELEMENT_NODE) continue;
      xmlChar* label = xmlGetProp(node, BAD_CAST "label");
      if(label)
      {
         std::string labelStr = reinterpret_cast<char*>(label);
         std::transform(labelStr.begin(), labelStr.end(), labelStr.begin(), ::tolower);
         if(labelStr.find("cdt") != std::string::npos) found = true;
         xmlFree(label);
      }
   }
   xmlFreeDoc(doc);
   return found;
}

std::string Cdt::generateTempXmlPath(const std::string& originalXmlPath)
{
   // Use cross-platform temporary directory creation
   std::string timestamp = getCurrentTimeString();
   std::string tempDirName = "qfs_cdt_" + timestamp;

   std::string tempDir = FileSystem::createTempDirectory(tempDirName);

   if(tempDir.empty())
   {
      // If all attempts failed, use current directory with timestamp
      tempDir = tempDirName;
      if(!FileSystem::createDirectory(tempDir))
      {
         // Failed to create directory, use current directory
         CFLOG_ERROR(
            "Warning: Failed to create temporary directory, using "
            "current directory",
            false
         );
         tempDir = ".";
      }
      else
      {
         CFLOG_INFO("Using current directory for temporary files: " + tempDir, false);
      }
   }

   // Extract just the filename from the original path
   std::string fullFilename = FileSystem::getFilename(originalXmlPath);

   return FileSystem::joinPath(tempDir, fullFilename);
}

bool Cdt::copyCdtToBuild(const std::string& sourcePath, const std::string& buildPath)
{
   try
   {
      std::string filename = FileSystem::getFilename(sourcePath);
      std::string targetPath = FileSystem::joinPath(buildPath, filename);

      // Use simple file copy without std::filesystem for C++14 compatibility
      std::ifstream src(sourcePath, std::ios::binary);
      std::ofstream dst(targetPath, std::ios::binary);

      if(!src.is_open() || !dst.is_open())
      {
         return false;
      }

      dst << src.rdbuf();

      return src.good() && dst.good();
   }
   catch(const std::exception& e)
   {
      // Log error but don't throw (return false instead)
      std::ostringstream oss;
      oss << "Error copying CDT file: " << e.what();
      CFLOG_ERROR(oss.str(), /*printMsgOnly=*/false);

      return false;
   }
}

std::string Cdt::getCurrentTimeString()
{
   auto now = std::chrono::system_clock::now();
   auto timeT = std::chrono::system_clock::to_time_t(now);
   auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

   std::ostringstream oss;
   oss << std::put_time(std::localtime(&timeT), "%Y%m%d_%H%M%S");
   oss << "_" << std::setfill('0') << std::setw(3) << ms.count();

   return oss.str();
}

} // namespace CLI
} // namespace QC

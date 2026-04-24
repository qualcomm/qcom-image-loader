// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "CliParserUtil.h"

#include "CliArgumentDefinitions.h"
#include "ExceptionHandler.h"
#include "FileSystem.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
namespace QC {
namespace CLI {
bool CliParserUtil::parseBool(const std::string& value)
{
   std::string upper = toUpper(value);
   if(upper == "TRUE" || upper == "1" || upper == "YES")
   {
      return true;
   }
   else if(upper == "FALSE" || upper == "0" || upper == "NO")
   {
      return false;
   }
   QC_THROW_CMDLINE_ERROR(
      QC::Common::Exception::INVALID_PARAMETERS,
      "boolean value",
      "Invalid boolean value: " + value + " (expected TRUE/FALSE, 1/0, YES/NO)"
   );
}

QC::MemoryType::type CliParserUtil::parseMemoryType(const std::string& value)
{
   std::string upper = toUpper(value);
   if(upper == "UFS")
   {
      return QC::MemoryType::MEMORY_TYPE_UFS;
   }
   else if(upper == "EMMC")
   {
      return QC::MemoryType::MEMORY_TYPE_EMMC;
   }
   else if(upper == "NAND")
   {
      return QC::MemoryType::MEMORY_TYPE_NAND;
   }
   else if(upper == "SPINOR")
   {
      return QC::MemoryType::MEMORY_TYPE_SPINOR;
   }
   QC_THROW_CMDLINE_ERROR(
      QC::Common::Exception::INVALID_PARAMETERS,
      "memory-type",
      "Invalid MEMORYTYPE: " + value + " (expected UFS, EMMC, NAND, or SPINOR)"
   );
}

int CliParserUtil::parseInteger(const std::string& value)
{
   try
   {
      return std::stoi(value);
   }
   catch(const std::exception&)
   {
      QC_THROW_CMDLINE_ERROR(QC::Common::Exception::INVALID_PARAMETERS, "integer", "Invalid integer: " + value);
   }
}

std::vector<int> CliParserUtil::parseNaturalNumberList(const std::string& value, char delimiter)
{
   std::vector<int> result;
   std::stringstream ss(value);
   std::string item;

   while(std::getline(ss, item, delimiter))
   {
      // Trim whitespace
      item.erase(0, item.find_first_not_of(" \t"));
      item.erase(item.find_last_not_of(" \t") + 1);

      if(!item.empty())
      {
         result.push_back(parseNaturalNumber(item));
      }
   }

   return result;
}

int CliParserUtil::parseNaturalNumber(const std::string& value)
{
   try
   {
      int n = parseInteger(value);
      if(n < 0)
      {
         QC_THROW_CMDLINE_ERROR(
            QC::Common::Exception::INVALID_PARAMETERS,
            "natural number",
            "Invalid natural number: " + value
         );
      }
      return n;
   }
   catch(const QC::Common::CommandLineException&)
   {
      throw; // Re-throw RCA exceptions
   }
   catch(const std::exception& e)
   {
      QC_THROW_CMDLINE_ERROR(QC::Common::Exception::INVALID_PARAMETERS, "natural number", e.what());
   }
}

std::string CliParserUtil::parseFilePath(const std::string& root, const std::string& value)
{
   std::string expectedPath = FileSystem::joinPath(root, value);
   if(FileSystem::isFile(expectedPath))
   {
      return expectedPath;
   }
   QC_THROW_FILE_ERROR(QC::Common::Exception::FILE_NOT_FOUND, expectedPath, "file path validation");
}

std::vector<std::string>
CliParserUtil::parseFilePathList(const std::string& root, const std::string& value, char delimiter)
{
   std::vector<std::string> result;
   std::stringstream ss(value);
   std::string item;

   while(std::getline(ss, item, delimiter))
   {
      // Trim whitespace
      item.erase(0, item.find_first_not_of(" \t"));
      item.erase(item.find_last_not_of(" \t") + 1);

      if(!item.empty())
      {
         result.push_back(parseFilePath(root, item));
      }
   }

   return result;
}

std::string CliParserUtil::toUpper(const std::string& str)
{
   std::string result = str;
   std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
   return result;
}

QC::ValidationMode::type CliParserUtil::parseValidationMode(const std::string& str)
{
   QC::ValidationMode::type validationMode;
   switch(parseInteger(str))
   {
      case 0:
         validationMode = QC::ValidationMode::VALIDATION_MODE_NONE;
         break;
      case 1:
         validationMode = QC::ValidationMode::VALIDATION_MODE_BINARY_READBACK;
         break;
      case 2:
         validationMode = QC::ValidationMode::VALIDATION_MODE_SHA256_READBACK;
         break;
      case 3:
         validationMode = QC::ValidationMode::VALIDATION_MODE_BINARY_READBACK_WITH_DIGESTS_FILE;
         break;
      case 4:
         validationMode = QC::ValidationMode::VALIDATION_MODE_SHA256_READBACK_WITH_DIGESTS_FILE;
         break;
      default:
         QC_THROW_CMDLINE_ERROR(
            QC::Common::Exception::INVALID_PARAMETERS,
            "validation-mode",
            str + CliArgumentDefinitions::getValidOptions("validation-mode")
         );
   }
   return validationMode;
}

QC::DigestHeaderType::type CliParserUtil::parseDigestHeaderType(const std::string& str)
{
   QC::DigestHeaderType::type headerType;
   if(toUpper(str) == "MBN")
   {
      headerType = QC::DigestHeaderType::DIGEST_HEADER_TYPE_MBN;
   }
   else if(toUpper(str) == "ELF")
   {
      headerType = QC::DigestHeaderType::DIGEST_HEADER_TYPE_ELF;
   }
   else
   {
      QC_THROW_CMDLINE_ERROR(
         QC::Common::Exception::INVALID_PARAMETERS,
         "digest-header-type",
         str + CliArgumentDefinitions::getValidOptions("digest-header-type")
      );
   }
   return headerType;
}

} // namespace CLI
} // namespace QC

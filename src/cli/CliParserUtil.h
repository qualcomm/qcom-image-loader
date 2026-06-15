// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "ImageManagementDefinitions.h"

namespace QC {
namespace CLI {
class CliParserUtil
{
public:
   /**
    * @brief Parse boolean value
    *
    * Accepts: TRUE/FALSE, true/false, True/False, 1/0
    *
    * @param value String value
    * @return Boolean value
    * @throws std::invalid_argument if value is not a valid boolean
    */
   static bool parseBool(const std::string& value);

   /**
    * @brief Parse memory type
    *
    * @param value String value (UFS, EMMC, NAND, SPINOR)
    * @return Memory type enum
    * @throws std::invalid_argument if value is not a valid memory type
    */
   static QC::MemoryType::type parseMemoryType(const std::string& value);

   /**
    * @brief Parse integer from string
    *
    * @param value String (e.g., "-1")
    * @return parsed integer
    * @throws std::invalid_argument if parsing fails
    */
   static int parseInteger(const std::string& value);

   /**
    * @brief Parse comma-separated natural number list
    *
    * @param value String value (e.g., "1,2,3,4")
    * @param delimiter Delimiter character
    * @return Vector of integers
    * @throws std::invalid_argument if parsing fails
    */
   static std::vector<int> parseNaturalNumberList(const std::string& value, char delimiter);

   /**
    * @brief Parse natural number from string
    *
    * @param value String (e.g., "1")
    * @param delimiter Delimiter character
    * @return parsed natural number
    * @throws std::invalid_argument if parsing fails
    */
   static int parseNaturalNumber(const std::string& value);

   /**
    * @brief Parse path with with existense check
    *
    * @param value String value (e.g., "file1.xml")
    * @param delimiter Delimiter character
    * @return Vector of strings
    */
   static std::string parseFilePath(const std::string& root, const std::string& value);

   /**
    * @brief Parse delimited string list
    *
    * @param value String value (e.g., "file1.xml;file2.xml")
    * @param delimiter Delimiter character
    * @return Vector of strings
    */
   static std::vector<std::string> parseFilePathList(const std::string& root, const std::string& value, char delimiter);

   /**
    * @brief Convert string to uppercase
    *
    * @param str Input string
    * @return Uppercase string
    */
   static std::string toUpper(const std::string& str);

   static QC::ValidationMode::type parseValidationMode(const std::string& str);
   static QC::DigestHeaderType::type parseDigestHeaderType(const std::string& str);

   /**
    * @brief Parse preserve-partitions argument
    *
    * Format: "<index>:<name1>,<name2>,...;<index>:<name1>,<name2>,..."
    * where index is a LUN number or 'x' for auto-find.
    *
    * Example: "0:modem,fsc,fsg;1:persist" produces 4 PreservedPartitionInfo entries.
    *
    * @param value String value
    * @return Vector of PreservedPartitionInfo
    * @throws CommandLineException if format is invalid
    */
   static std::vector<QC::PreservedPartitionInfo> parsePreservePartitionList(const std::string& value);
};

} // namespace CLI
} // namespace QC
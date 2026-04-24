// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "CliOptions.h"

#include <set>
#include <string>
#include <vector>

namespace QC {
namespace CLI {

/**
 * @brief Command-line argument parser
 *
 * Parses command-line arguments with support for:
 * - Single dash (-) and double dash (--) prefixes
 * - Case-insensitive option names
 * - Multi-pass parsing for dependency resolution
 */
#define CLI_ARGUMENT std::pair<std::string, std::string>

class CliParser
{
public:
   /**
    * @brief Parse command-line arguments
    *
    * @param argc Argument count
    * @param argv Argument vector
    * @return Parsed CLI options
    * @throws std::invalid_argument on parsing errors
    */
   static CliOptions parse(int argc, char* argv[]);

private:
   /**
    * @brief Extract option name from argument
    *
    * Removes leading dashes and converts to uppercase for case-insensitive
    * matching
    *
    * @param arg Command-line argument
    * @return Normalized option name (uppercase, no dashes)
    */
   static CLI_ARGUMENT extractOption(const std::string& arg);

   /**
    * @brief Check if option is valid
    *
    * @param option Normalized option name
    * @return true if valid, false otherwise
    */
   static bool isValidOption(const std::string& option);

   /**
    * @brief initialize valid argument names
    */
   static void initializeValidOptions();

   static std::set<std::string> VALID_OPTIONS; // valid command line options
};

} // namespace CLI
} // namespace QC

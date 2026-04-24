// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "CliArgumentDefinitions.h"

#include <string>

namespace QC {
namespace CLI {

/**
 * @brief CLI Help system with dynamic help generation
 *
 * This class provides help functionality that automatically generates
 * help text from argument definitions, making it maintainable and
 * consistent with actual CLI options.
 */
class CliHelp
{
public:
   /**
    * @brief Show application version information
    */
   static void showVersion();

   /**
    * @brief Show complete usage information
    *
    * Displays all available commands, arguments organized by category,
    * examples, and usage notes. Help is dynamically generated from
    * argument definitions.
    */
   static void showUsage();

   static const std::string edlHelpModeString; // Contains info on how to switch device to edl mode

private:
   /**
    * @brief Show common commands section
    */
   static void showCommands();

   /**
    * @brief Show arguments for a specific category
    *
    * @param category Argument category to display
    * @param categoryName Display name for the category
    */
   static void showArgumentCategory(ArgumentCategory category, const std::string& categoryName);

   /**
    * @brief Show detailed information for a single argument
    *
    * @param arg Argument definition to display
    */
   static void showArgumentDetails(const ArgumentDefinition& arg);

   /**
    * @brief Format argument usage string
    *
    * @param arg Argument definition
    * @return Formatted usage string (e.g., "DEVICE <string>")
    */
   static std::string formatArgumentUsage(const ArgumentDefinition& arg);

   /**
    * @brief Show usage examples section
    */
   static void showExamples();

   /**
    * @brief Show general usage notes
    */
   static void showNotes();

};

} // namespace CLI
} // namespace QC

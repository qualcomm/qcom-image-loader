// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "CliOptions.h"

#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace QC {
namespace CLI {

/**
 * @brief Argument type enumeration
 */
enum class ArgumentType
{
   FLAG,        // Boolean flag (no value)
   STRING,      // String value
   INTEGER,     // Integer value
   BOOLEAN,     // Boolean value (TRUE/FALSE)
   STRING_LIST, // Comma-separated string list
   PATH         // File/directory path
};

/**
 * @brief Argument category for help organization
 */
enum class ArgumentCategory
{
   COMMANDS,
   DEVICE_OPTIONS,
   DOWNLOAD_OPTIONS,
   ADVANCED_OPTIONS,
   UFS_PROVISIONING,
   DIGEST_CREATION,
   FLASH_OPERATIONS,
   LOGGING
};

/**
 * @brief Single argument definition
 */
struct ArgumentDefinition
{
   std::string name;                                                     // Argument name (e.g., "DEVICE")
   std::string description;                                              // Human-readable description
   ArgumentType type;                                                    // Type of argument
   ArgumentCategory category;                                            // Category for help organization
   std::string defaultValue;                                             // Default value (empty if required)
   std::vector<std::string> validValues;                                 // Valid values (for enums)
   bool required;                                                        // Is this argument required?
   std::string example;                                                  // Usage example
   std::string notes;                                                    // Additional notes
   std::function<void(QC::CLI::CliOptions&, const std::string&)> setter; // Setter function to update the options
                                                                         // object with the parsed value
};

/**
 * @brief Command definition for help organization
 */
struct CommandDefinition
{
   std::string name;                                 // Command name
   std::string description;                          // Command description
   std::vector<ArgumentDefinition> requiredArgs;     // Required arguments
   std::vector<ArgumentDefinition> optionalArgs;     // Optional arguments
   std::string example;                              // Usage example
   CliOptions::CommandType commandType;              // Command type for execution routing
   std::function<void(QC::CLI::CliOptions&)> setter; // Setter function to update the options object, should not have
                                                     // value for command options
};

/**
 * @brief Centralized argument definitions
 *
 * This class provides a single source of truth for all CLI arguments,
 * their descriptions, types, and validation rules. The help system
 * automatically generates help text from these definitions.
 */
class CliArgumentDefinitions
{
public:
   /**
    * @brief Get all argument definitions
    */
   static const std::map<std::string, ArgumentDefinition>& getArgumentDefinitions();

   /**
    * @brief Get all command definitions
    */
   static const std::vector<CommandDefinition>& getCommandDefinitions();

   /**
    * @brief Get arguments by category
    */
   static std::vector<ArgumentDefinition> getArgumentsByCategory(ArgumentCategory category);

   /**
    * @brief Get argument definition by name
    */
   static const ArgumentDefinition* getArgumentDefinition(const std::string& name);

   /**
    * @brief Get valid options of an argument
    */
   static const std::string getValidOptions(const std::string& argument);

private:
   static std::map<std::string, ArgumentDefinition> argumentDefinitions;
   static std::vector<CommandDefinition> commandDefinitions;
   static bool initialized;


   /**
    * @brief Initialize all definitions
    */
   static void initialize();
};

} // namespace CLI
} // namespace QC

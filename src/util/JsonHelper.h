// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cctype>
#include <stdexcept>

namespace Util {

/**
 * @brief Exception thrown on JSON parsing errors
 */
class JsonException : public std::runtime_error
{
public:
   explicit JsonException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Generic JSON helper for parsing simple JSON structures
 *
 * Provides lightweight JSON parsing without external dependencies.
 * Supports: objects, arrays of strings, and basic key-value extraction.
 * Throws JsonException with detailed error messages on parse failures.
 *
 * Limitations:
 * - Does NOT support nested objects (objects as values are skipped)
 * - Only handles string and array values
 * - For complex nested JSON structures, use a full JSON library (e.g., nlohmann/json)
 *
 * Note: Can use third-party library (e.g., nlohmann/json) for JSON parsing in future if needed.
 */
class JsonHelper
{
public:
   /**
    * @brief Extract string value for a given key
    * @param json JSON string
    * @param key Key to search for
    * @return Value if found
    * @throws JsonException if JSON is invalid or key not found
    */
   static std::string extractString(const std::string& json, const std::string& key);

   /**
    * @brief Extract array of strings for a given key
    * @param json JSON string
    * @param key Key to search for
    * @return Vector of strings
    * @throws JsonException if JSON is invalid or key not found or not an array
    */
   static std::vector<std::string> extractArray(const std::string& json, const std::string& key);

   /**
    * @brief Extract array of strings from raw JSON array
    * @param json JSON string containing a raw array
    * @return Vector of strings
    * @throws JsonException if JSON is not a valid array
    *
    * @note Only handles arrays of strings. Nested arrays or objects within arrays are not supported.
    *       Example: ["string1", "string2"] works, but [["nested"], "string"] does not.
    */
   static std::vector<std::string> parseArray(const std::string& json);

   /**
    * @brief Extract object as map of key to array of strings
    * @param json JSON string containing an object
    * @return Map where each key maps to a vector of strings (or single string wrapped in vector)
    * @throws JsonException if JSON is not a valid object
    *
    * @note Does NOT support nested objects. Only handles:
    *       - String values: "key": "value"
    *       - Array values: "key": ["value1", "value2"]
    *       Nested objects are skipped. For complex nested JSON, use a full JSON library.
    */
   static std::map<std::string, std::vector<std::string>> parseObject(const std::string& json);

   /**
    * @brief Check if JSON is valid (has opening and closing braces)
    * @param json JSON string
    * @return true if valid structure, false otherwise
    * @throws JsonException with detailed reason if invalid
    */
   static bool isValid(const std::string& json);

private:
   /**
    * @brief Skip whitespace and find next character
    * @param str String to search
    * @param pos Current position
    * @param target Character to find
    * @return Position of target character, or std::string::npos if not found
    */
   static size_t findChar(const std::string& str, size_t pos, char target);

   /**
    * @brief Extract quoted string value
    * @param str String containing quoted value
    * @param pos Position after opening quote
    * @return Extracted string without quotes
    * @throws JsonException if closing quote not found
    */
   static std::string extractQuotedString(const std::string& str, size_t& pos);

   /**
    * @brief Find matching closing bracket
    * @param str String to search
    * @param pos Position of opening bracket
    * @param openChar Opening bracket character
    * @param closeChar Closing bracket character
    * @return Position of closing bracket, or std::string::npos if not found
    */
   static size_t findMatchingBracket(const std::string& str, size_t pos, char openChar, char closeChar);
};

} // namespace Util

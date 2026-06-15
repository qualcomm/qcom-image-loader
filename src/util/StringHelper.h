// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
// C++17 compatible version
// For C++20 version with ranges, see StringHelpers_Cpp20.h

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstring>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

// Prevent ambiguous 'byte' symbol conflict between Windows headers and
// std::byte
#ifndef _HAS_STD_BYTE
#define _HAS_STD_BYTE 0
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

namespace Util {

// Helper function to check if a string contains only digits
inline bool isNumber(const std::string& str)
{
   if(str.empty()) return false;
   return std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isdigit(c); });
}

// Helper function to convert string to uppercase (in-place)
inline std::string& toUpper(std::string& str)
{
   std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
      return static_cast<char>(std::toupper(c));
   });
   return str;
}

// Helper function to convert string to lowercase (copy)
inline std::string toLowerCopy(const std::string& str)
{
   std::string result = str;
   std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
      return static_cast<char>(std::tolower(c));
   });
   return result;
}

// Helper function to trim whitespace from left (in-place)
inline std::string& trimLeft(std::string& str)
{
   str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));
   return str;
}

// Helper function to trim whitespace from right (in-place)
inline std::string& trimRight(std::string& str)
{
   str.erase(
      std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
      str.end()
   );
   return str;
}

// Helper function to trim whitespace from both sides (in-place)
inline std::string& trim(std::string& str)
{
   return trimLeft(trimRight(str));
}

// Helper function to trim whitespace from both sides (copy)
inline std::string trimCopy(const std::string& str)
{
   std::string result = str;
   return trim(result);
}

// Helper function to split string by delimiter
inline std::vector<std::string> split(const std::string& str, char delimiter)
{
   std::vector<std::string> result;
   if(str.empty()) return result;

   std::stringstream ss(str);
   std::string item;

   while(std::getline(ss, item, delimiter))
   {
      result.push_back(item);
   }

   return result;
}

// Helper function to get C-string length (for compatibility with
// Lang::getStringLength)
inline size_t getStringLength(const char* str)
{
   return str ? std::strlen(str) : 0;
}

// Helper function to wrap string in quotes
inline std::string quote(const std::string& str)
{
   return "\"" + str + "\"";
}

// Helper function to convert std::string to std::wstring
inline std::wstring toWString(const std::string& str)
{
   if(str.empty()) return std::wstring();

#ifdef _WIN32
   // Check for string size overflow
   if(str.size() > static_cast<size_t>(INT_MAX))
   {
      return std::wstring();
   }

   // Use Windows API for conversion (UTF-8 to UTF-16)
   int strLen = static_cast<int>(str.size());
   int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), strLen, NULL, 0);
   if(sizeNeeded <= 0)
   {
      return std::wstring();
   }

   std::wstring result(static_cast<size_t>(sizeNeeded), 0);
   int charsWritten = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), strLen, &result[0], sizeNeeded);
   if(charsWritten <= 0)
   {
      return std::wstring();
   }

   return result;
#else
   // For non-Windows platforms, use a simple conversion (assuming ASCII/UTF-8)
   return std::wstring(str.begin(), str.end());
#endif
}

// Helper function to convert std::wstring to std::string
inline std::string fromWString(const std::wstring& wstr)
{
   if(wstr.empty()) return std::string();

#ifdef _WIN32
   // Check for string size overflow
   if(wstr.size() > static_cast<size_t>(INT_MAX))
   {
      return std::string();
   }

   // Use Windows API for conversion (UTF-16 to UTF-8)
   int wstrLen = static_cast<int>(wstr.size());
   int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstrLen, NULL, 0, NULL, NULL);
   if(sizeNeeded <= 0)
   {
      return std::string();
   }

   std::string result(static_cast<size_t>(sizeNeeded), 0);
   int bytesWritten = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstrLen, &result[0], sizeNeeded, NULL, NULL);
   if(bytesWritten <= 0)
   {
      return std::string();
   }

   return result;
#else
   // For non-Windows platforms, use a simple conversion (assuming ASCII/UTF-8)
   return std::string(wstr.begin(), wstr.end());
#endif
}

} // namespace Util

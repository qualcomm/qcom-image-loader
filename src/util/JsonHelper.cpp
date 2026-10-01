// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "JsonHelper.h"
#include <sstream>

namespace Util {

size_t JsonHelper::findChar(const std::string& str, size_t pos, char target)
{
   for(size_t i = pos; i < str.length(); ++i)
   {
      char c = str[i];
      if(c == target) return i;
      if(c != ' ' && c != '\n' && c != '\r' && c != '\t') return std::string::npos;
   }
   return std::string::npos;
}

std::string JsonHelper::extractQuotedString(const std::string& str, size_t& pos)
{
   std::string result;

   while(pos < str.length())
   {
      char c = str[pos];

      if(c == '\\' && pos + 1 < str.length())
      {
         char next = str[pos + 1];
         if(next == '\\')
         {
            result += '\\';
            pos += 2;
         }
         else if(next == '"')
         {
            result += '"';
            pos += 2;
         }
         else if(next == '/')
         {
            result += '/';
            pos += 2;
         }
         else if(next == 'b')
         {
            result += '\b';
            pos += 2;
         }
         else if(next == 'f')
         {
            result += '\f';
            pos += 2;
         }
         else if(next == 'n')
         {
            result += '\n';
            pos += 2;
         }
         else if(next == 'r')
         {
            result += '\r';
            pos += 2;
         }
         else if(next == 't')
         {
            result += '\t';
            pos += 2;
         }
         else
         {
            result += c;
            pos++;
         }
      }
      else if(c == '"')
      {
         pos++;
         return result;
      }
      else
      {
         result += c;
         pos++;
      }
   }
   throw JsonException("Unclosed quoted string - missing closing quote");
}

size_t JsonHelper::findMatchingBracket(const std::string& str, size_t pos, char openChar, char closeChar)
{
   int depth = 1;
   bool inString = false;
   bool escaped = false;

   for(size_t i = pos + 1; i < str.length(); ++i)
   {
      char c = str[i];

      if(escaped)
      {
         escaped = false;
         continue;
      }

      if(c == '\\')
      {
         escaped = true;
         continue;
      }

      if(c == '"')
      {
         inString = !inString;
         continue;
      }

      if(inString) continue;

      if(c == openChar) depth++;
      else if(c == closeChar)
      {
         depth--;
         if(depth == 0) return i;
      }
   }

   std::ostringstream oss;
   oss << "Unmatched '" << openChar << "' - missing closing '" << closeChar << "'";
   throw JsonException(oss.str());
}

bool JsonHelper::isValid(const std::string& json)
{
   if(json.empty())
      throw JsonException("JSON string is empty");

   size_t openPos = findChar(json, 0, '{');
   if(openPos == std::string::npos)
      throw JsonException("JSON must start with '{' - no opening brace found");

   size_t closePos = findMatchingBracket(json, openPos, '{', '}');
   if(closePos == std::string::npos)
      throw JsonException("Unmatched '{' - missing closing '}'");

   return true;
}

std::string JsonHelper::extractString(const std::string& json, const std::string& key)
{
   isValid(json);

   std::string searchKey = "\"" + key + "\"";
   size_t keyPos = json.find(searchKey);
   if(keyPos == std::string::npos)
   {
      std::ostringstream oss;
      oss << "Key '" << key << "' not found in JSON";
      throw JsonException(oss.str());
   }

   size_t colonPos = findChar(json, keyPos + searchKey.length(), ':');
   if(colonPos == std::string::npos)
   {
      std::ostringstream oss;
      oss << "Colon ':' not found after key '" << key << "'";
      throw JsonException(oss.str());
   }

   size_t quotePos = findChar(json, colonPos + 1, '"');
   if(quotePos == std::string::npos)
   {
      std::ostringstream oss;
      oss << "Opening quote not found for value of key '" << key << "'";
      throw JsonException(oss.str());
   }

   size_t valueStart = quotePos + 1;
   std::string value = extractQuotedString(json, valueStart);
   return value;
}

std::vector<std::string> JsonHelper::extractArray(const std::string& json, const std::string& key)
{
   std::vector<std::string> result;

   isValid(json);

   std::string searchKey = "\"" + key + "\"";
   size_t keyPos = json.find(searchKey);
   if(keyPos == std::string::npos)
   {
      std::ostringstream oss;
      oss << "Key '" << key << "' not found in JSON";
      throw JsonException(oss.str());
   }

   size_t colonPos = findChar(json, keyPos + searchKey.length(), ':');
   if(colonPos == std::string::npos)
   {
      std::ostringstream oss;
      oss << "Colon ':' not found after key '" << key << "'";
      throw JsonException(oss.str());
   }

   size_t openBracketPos = findChar(json, colonPos + 1, '[');
   if(openBracketPos == std::string::npos)
   {
      std::ostringstream oss;
      oss << "Value for key '" << key << "' is not an array - opening '[' not found";
      throw JsonException(oss.str());
   }

   size_t closeBracketPos = findMatchingBracket(json, openBracketPos, '[', ']');
   if(closeBracketPos == std::string::npos)
   {
      std::ostringstream oss;
      oss << "Unmatched '[' in array for key '" << key << "' - missing closing ']'";
      throw JsonException(oss.str());
   }

   size_t pos = openBracketPos + 1;
   while(pos < closeBracketPos)
   {
      size_t quotePos = findChar(json, pos, '"');
      if(quotePos == std::string::npos || quotePos >= closeBracketPos) break;

      size_t valueStart = quotePos + 1;
      std::string value = extractQuotedString(json, valueStart);
      if(!value.empty())
      {
         result.push_back(value);
      }

      pos = valueStart;
      // Skip comma and whitespace to next element
      while(pos < closeBracketPos && (json[pos] == ',' || json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r' || json[pos] == '\t'))
         pos++;
   }

   return result;
}

std::vector<std::string> JsonHelper::parseArray(const std::string& json)
{
   std::vector<std::string> result;

   if(json.empty())
      throw JsonException("JSON string is empty");

   size_t openBracketPos = findChar(json, 0, '[');
   if(openBracketPos == std::string::npos)
      throw JsonException("JSON must be an array - no opening '[' found");

   size_t closeBracketPos = findMatchingBracket(json, openBracketPos, '[', ']');
   if(closeBracketPos == std::string::npos)
      throw JsonException("Unmatched '[' - missing closing ']'");

   size_t pos = openBracketPos + 1;
   while(pos < closeBracketPos)
   {
      size_t quotePos = findChar(json, pos, '"');
      if(quotePos == std::string::npos || quotePos >= closeBracketPos) break;

      size_t valueStart = quotePos + 1;
      std::string value = extractQuotedString(json, valueStart);
      if(!value.empty())
      {
         result.push_back(value);
      }

      pos = valueStart;
      // Skip comma and whitespace to next element
      while(pos < closeBracketPos && (json[pos] == ',' || json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r' || json[pos] == '\t'))
         pos++;
   }

   return result;
}

std::map<std::string, std::vector<std::string>> JsonHelper::parseObject(const std::string& json)
{
   std::map<std::string, std::vector<std::string>> result;

   if(json.empty())
      throw JsonException("JSON string is empty");

   size_t openBracePos = findChar(json, 0, '{');
   if(openBracePos == std::string::npos)
      throw JsonException("JSON must be an object - no opening '{' found");

   size_t closeBracePos = findMatchingBracket(json, openBracePos, '{', '}');
   if(closeBracePos == std::string::npos)
      throw JsonException("Unmatched '{' - missing closing '}'");

   size_t pos = openBracePos + 1;
   while(pos < closeBracePos)
   {
      size_t keyQuotePos = findChar(json, pos, '"');
      if(keyQuotePos == std::string::npos || keyQuotePos >= closeBracePos) break;

      size_t keyStart = keyQuotePos + 1;
      std::string key = extractQuotedString(json, keyStart);

      size_t colonPos = findChar(json, keyStart, ':');
      if(colonPos == std::string::npos || colonPos >= closeBracePos) break;

      size_t valueStart = colonPos + 1;
      while(valueStart < closeBracePos && (json[valueStart] == ' ' || json[valueStart] == '\n' || json[valueStart] == '\r' || json[valueStart] == '\t'))
         valueStart++;

      std::vector<std::string> values;

      if(valueStart < closeBracePos && json[valueStart] == '[')
      {
         size_t arrayClosePos = findMatchingBracket(json, valueStart, '[', ']');
         if(arrayClosePos != std::string::npos)
         {
            size_t arrayPos = valueStart + 1;
            while(arrayPos < arrayClosePos)
            {
               size_t elemQuotePos = findChar(json, arrayPos, '"');
               if(elemQuotePos == std::string::npos || elemQuotePos >= arrayClosePos) break;

               size_t elemStart = elemQuotePos + 1;
               std::string elem = extractQuotedString(json, elemStart);
               if(!elem.empty())
                  values.push_back(elem);

               arrayPos = elemStart;
               while(arrayPos < arrayClosePos && (json[arrayPos] == ',' || json[arrayPos] == ' ' || json[arrayPos] == '\n' || json[arrayPos] == '\r' || json[arrayPos] == '\t'))
                  arrayPos++;
            }
            pos = arrayClosePos + 1;
         }
      }
      else if(valueStart < closeBracePos && json[valueStart] == '"')
      {
         size_t strStart = valueStart + 1;
         std::string value = extractQuotedString(json, strStart);
         if(!value.empty())
            values.push_back(value);
         pos = strStart;
      }
      else
      {
         pos = valueStart;
      }

      if(!key.empty() && !values.empty())
         result[key] = values;

      while(pos < closeBracePos && (json[pos] == ',' || json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r' || json[pos] == '\t'))
         pos++;
   }

   return result;
}

} // namespace Util


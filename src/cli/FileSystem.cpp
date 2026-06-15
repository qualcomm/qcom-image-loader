// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "FileSystem.h"
// #include "ExceptionHandler.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <KL/kLogger.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <regex>
#include <sstream>
#ifdef TOOLS_TARGET_WINDOWS
#include <chrono>
#include <thread>
#elif defined(TOOLS_TARGET_LINUX)
#include <chrono>
#include <errno.h>
#include <thread>
#endif

namespace QC {
namespace CLI {
#ifdef TOOLS_TARGET_LINUX
const mode_t ALL_USER_PERMISSIONS = 0777;
#endif

// ========== CLI-Specific File Operations ==========

std::map<int, std::string> FileSystem::findDeviceProgrammer(const std::string& buildPath)
{
   // Priority order for device programmer search:
   // 1. qsahara_device_programmer.xml - Returns full sahara image list
   // 2. xbl_s_devprg_ns.melf or xbl_s_devprg_ns.elf - Returns as ID 13
   // 3. prog_firehose_ddr.mbn or prog_firehose_ddr.elf - Returns as ID 13
   // 4. prog_firehose_lite.mbn or prog_firehose_lite.elf - Returns as ID 13
   // 5. Fallback to other prog_firehose*.elf files
   // 6. Fallback to other prog_firehose*.mbn files

   std::map<int, std::string> saharaImages;

   // 1. Try qsahara_device_programmer.xml first
   std::string qsaharaXmlPath = joinPath(buildPath, "qsahara_device_programmer.xml");
   if(fileExists(qsaharaXmlPath))
   {
      saharaImages = readSaharaImageList(buildPath);
      if(!saharaImages.empty())
      {
         return saharaImages;
      }
   }

   // 2. Try xbl_s_devprg_ns.melf or xbl_s_devprg_ns.elf
   auto melfFiles = findFiles(buildPath, "xbl_s_devprg_ns\\.(melf|elf)$", false);
   if(!melfFiles.empty())
   {
      saharaImages[13] = melfFiles[0];
      return saharaImages;
   }

   // 3. Try prog_firehose_ddr.mbn or prog_firehose_ddr.elf
   auto ddrFiles = findFiles(buildPath, "prog_firehose_ddr\\.(mbn|elf)$", false);
   if(!ddrFiles.empty())
   {
      saharaImages[13] = ddrFiles[0];
      return saharaImages;
   }

   // 4. Try prog_firehose_lite.mbn or prog_firehose_lite.elf
   auto liteFiles = findFiles(buildPath, "prog_firehose_lite\\.(mbn|elf)$", false);
   if(!liteFiles.empty())
   {
      saharaImages[13] = liteFiles[0];
      return saharaImages;
   }

   // 5. Fallback: try any prog_firehose*.elf files
   auto elfFiles = findFiles(buildPath, "prog_firehose.*\\.elf$", false);
   if(!elfFiles.empty())
   {
      // Prefer prog_firehose_ddr.elf if it exists
      for(const auto& file: elfFiles)
      {
         if(file.find("prog_firehose_ddr.elf") != std::string::npos)
         {
            saharaImages[13] = file;
            return saharaImages;
         }
      }
      saharaImages[13] = elfFiles[0];
      return saharaImages;
   }

   // 6. Fallback: try any other prog_firehose*.mbn files
   auto mbnFiles = findFiles(buildPath, "prog_firehose.*\\.mbn$", false);
   if(!mbnFiles.empty())
   {
      saharaImages[13] = mbnFiles[0];
      return saharaImages;
   }

   return saharaImages; // Return empty map if nothing found
}

std::string FileSystem::findBuildValidationFile(const std::string& buildPath)
{
   auto validationFiles = findFiles(buildPath, "BuildValidation.*\\.digest$", false);
   if(!validationFiles.empty())
   {
      // Trim any whitespace/newlines from the filename
      std::string result = validationFiles[0];
      // Remove trailing whitespace including newlines
      result.erase(
         std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
         result.end()
      );
      return result;
   }
   throw std::runtime_error("Build validation file not found in build path. Please create a build "
                            "validation file and place it in the build directory.");
}

std::vector<std::string> FileSystem::findRawProgramXmls(const std::string& buildPath)
{
   auto files = findFiles(buildPath, "rawprogram.*\\.xml$", false); // Non-recursive
   sortPartitionFiles(files);
   return files;
}

std::vector<std::string> FileSystem::findPatchProgramXmls(const std::string& buildPath)
{
   auto files = findFiles(buildPath, "patch.*\\.xml$", false); // Non-recursive
   sortPartitionFiles(files);
   return files;
}

void FileSystem::sortPartitionFiles(std::vector<std::string>& files)
{
   std::sort(files.begin(), files.end(), [](const std::string& a, const std::string& b) {
      // Extract just the filename from the full path
      size_t lastSlashA = a.find_last_of(PATH_SEPARATOR_CHAR);
      size_t lastSlashB = b.find_last_of(PATH_SEPARATOR_CHAR);

      std::string filenameA = (lastSlashA != std::string::npos) ? a.substr(lastSlashA + 1) : a;
      std::string filenameB = (lastSlashB != std::string::npos) ? b.substr(lastSlashB + 1) : b;

      int numA = extractNumber(filenameA);
      int numB = extractNumber(filenameB);

      // If both have numbers, sort by number
      if(numA != -1 && numB != -1)
      {
         return numA < numB;
      }

      // If only one has a number, it comes first
      if(numA != -1) return true;
      if(numB != -1) return false;

      // Otherwise, sort alphabetically
      return filenameA < filenameB;
   });
}

int FileSystem::extractNumber(const std::string& filename)
{
   // Look for digits in the filename
   std::regex numberRegex("(\\d+)");
   std::smatch match;

   if(std::regex_search(filename, match, numberRegex))
   {
      try
      {
         return std::stoi(match[1].str());
      }
      catch(...)
      {
         return -1;
      }
   }

   return -1;
}

std::map<int, std::string> FileSystem::readSaharaImageList(const std::string& buildPath)
{
   std::string qsaharaFilePath = joinPath(buildPath, "qsahara_device_programmer.xml");
   return readSaharaImageListFromFile(qsaharaFilePath, buildPath);
}

std::map<int, std::string>
FileSystem::readSaharaImageListFromFile(const std::string& xmlFilePath, const std::string& buildPath)
{
   std::map<int, std::string> saharaImageList;

   if(!fileExists(xmlFilePath))
   {
      return saharaImageList;
   }

   xmlDocPtr doc = xmlReadFile(xmlFilePath.c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
   if(!doc) return saharaImageList;

   xmlNodePtr root = xmlDocGetRootElement(doc);
   for(xmlNodePtr imagesNode = root ? root->children : nullptr; imagesNode; imagesNode = imagesNode->next)
   {
      if(imagesNode->type != XML_ELEMENT_NODE || xmlStrcmp(imagesNode->name, BAD_CAST "images") != 0) continue;
      for(xmlNodePtr node = imagesNode->children; node; node = node->next)
      {
         if(node->type != XML_ELEMENT_NODE || xmlStrcmp(node->name, BAD_CAST "image") != 0) continue;
         int imageId = -1;
         std::string filename;

         // Get id attribute
         xmlChar* idVal = xmlGetProp(node, BAD_CAST "image_id");
         if(idVal)
         {
            try
            {
               imageId = std::stoi(reinterpret_cast<char*>(idVal));
            }
            catch(...)
            {
            }
            xmlFree(idVal);
         }

         // Get file attribute, fallback to path
         xmlChar* pathVal = xmlGetProp(node, BAD_CAST "image_path");
         if(pathVal)
         {
            try
            {
               filename = std::string(reinterpret_cast<char*>(pathVal));
            }
            catch(...)
            {
            }
            xmlFree(pathVal);
         }

         if(imageId != -1 && !filename.empty())
         {
            saharaImageList[imageId] = joinPath(buildPath, filename);
         }
      }
   }
   xmlFreeDoc(doc);
   return saharaImageList;
}

// ========== Cross-Platform File System Operations ==========

std::vector<std::string> FileSystem::findFiles(const std::string& directory, const std::string& pattern, bool recursive)
{
   std::vector<std::string> results;

   if(!isDirectory(directory))
   {
      return results;
   }

   std::regex regexPattern(pattern, std::regex::icase);

   // Get all files first
   std::vector<std::string> allFiles;
#ifdef TOOLS_TARGET_WINDOWS
   allFiles = listDirectoryWindows(directory, recursive);
#elif defined(TOOLS_TARGET_LINUX)
   allFiles = listDirectoryLinux(directory, recursive);
#endif

   // Filter by regex pattern
   for(const auto& file: allFiles)
   {
      // Extract just the filename for pattern matching
      size_t lastSlash = file.find_last_of(PATH_SEPARATOR_CHAR);
      std::string filename = (lastSlash != std::string::npos) ? file.substr(lastSlash + 1) : file;

      if(std::regex_search(filename, regexPattern))
      {
         results.push_back(file);
      }
   }

   return results;
}

bool FileSystem::fileExists(const std::string& path)
{
#ifdef TOOLS_TARGET_WINDOWS
   DWORD dwAttrib = GetFileAttributesA(path.c_str());
   return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#elif defined(TOOLS_TARGET_LINUX)
   struct stat buffer;
   return (stat(path.c_str(), &buffer) == 0);
#endif
}

bool FileSystem::isDirectory(const std::string& path)
{
#ifdef TOOLS_TARGET_WINDOWS
   DWORD dwAttrib = GetFileAttributesA(path.c_str());
   return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(TOOLS_TARGET_LINUX)
   struct stat buffer;
   if(stat(path.c_str(), &buffer) != 0)
   {
      return false;
   }
   return S_ISDIR(buffer.st_mode);
#endif
}

bool FileSystem::isFile(const std::string& path)
{
#ifdef TOOLS_TARGET_WINDOWS
   DWORD dwAttrib = GetFileAttributesA(path.c_str());
   return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(TOOLS_TARGET_LINUX)
   struct stat buffer;
   if(stat(path.c_str(), &buffer) != 0)
   {
      return false;
   }
   return S_ISREG(buffer.st_mode);
#endif
}

std::string FileSystem::normalizePath(const std::string& path)
{
   std::string result = path;

#ifdef TOOLS_TARGET_WINDOWS
   // Convert forward slashes to backslashes on Windows
   std::replace(result.begin(), result.end(), '/', '\\');
#elif defined(TOOLS_TARGET_LINUX)
   // Convert backslashes to forward slashes on Linux
   std::replace(result.begin(), result.end(), '\\', '/');
#endif

   return result;
}


std::string FileSystem::getAbsolutePath(const std::string& path)
{
   // If already absolute, return as-is
   if(isAbsolutePath(path))
   {
      return normalizePath(path);
   }

   // Convert relative path to absolute
#ifdef TOOLS_TARGET_WINDOWS
   char absolutePath[MAX_PATH];
   if(_fullpath(absolutePath, path.c_str(), MAX_PATH) != nullptr)
   {
      return normalizePath(std::string(absolutePath));
   }
#elif defined(TOOLS_TARGET_LINUX)
   char absolutePath[PATH_MAX];
   if(realpath(path.c_str(), absolutePath) != nullptr)
   {
      return normalizePath(std::string(absolutePath));
   }
#endif

// If realpath fails (file doesn't exist yet), manually construct absolute path
#ifdef TOOLS_TARGET_WINDOWS
   char cwd[MAX_PATH];
   if(_getcwd(cwd, sizeof(cwd)) != nullptr)
   {
      return normalizePath(joinPath(std::string(cwd), path));
   }
#elif defined(TOOLS_TARGET_LINUX)
   char cwd[PATH_MAX];
   if(getcwd(cwd, sizeof(cwd)) != nullptr)
   {
      return normalizePath(joinPath(std::string(cwd), path));
   }
#endif

   // Fallback: return normalized input path
   return normalizePath(path);
}


std::string FileSystem::joinPath(const std::string& base, const std::string& component)
{
   if(base.empty())
   {
      return component;
   }
   if(component.empty())
   {
      return base;
   }

   // Handler absolute path and networkpath in component
   if(isAbsolutePath(component))
   {
      return component;
   }
   std::string result = base;

   // Ensure base doesn't end with separator
   if(result.back() == PATH_SEPARATOR_CHAR)
   {
      result.pop_back();
   }

   // Ensure component doesn't start with separator
   std::string comp = component;
   if(comp.front() == PATH_SEPARATOR_CHAR)
   {
      comp = comp.substr(1);
   }

   return result + PATH_SEPARATOR + comp;
}

std::string FileSystem::getFilename(const std::string& path)
{
   if(path.empty())
   {
      return "";
   }

   // Find the last path separator
   size_t lastSeparator = path.find_last_of("/\\");
   if(lastSeparator == std::string::npos)
   {
      // No separator found, entire path is filename
      return path;
   }

   // Return everything after the last separator
   return path.substr(lastSeparator + 1);
}


bool FileSystem::isAbsolutePath(const std::string& path)
{
   if(path.empty())
   {
      return false;
   }

#ifdef TOOLS_TARGET_WINDOWS
   // Windows: Check for drive letter (C:) or UNC path (\\)
   if(path.length() >= 2)
   {
      if(path[1] == ':' && std::isalpha(path[0]))
      {
         return true; // Drive letter like C:
      }
      if(path[0] == '\\' && path[1] == '\\')
      {
         return true; // UNC path like \\server\share
      }
   }
   return false;
#elif defined(TOOLS_TARGET_LINUX)
   // Linux: Check if starts with /
   return path[0] == '/';
#endif
}

bool FileSystem::hasExtension(const std::string& path, const std::string& extension)
{
   if(path.empty() || extension.empty())
   {
      return false;
   }

   // Normalize extension (ensure it starts with a dot)
   std::string normalizedExt = extension;
   if(normalizedExt[0] != '.')
   {
      normalizedExt = "." + normalizedExt;
   }

   // Convert both to lowercase for case-insensitive comparison
   std::string lowerPath = path;
   std::string lowerExt = normalizedExt;
   std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);
   std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);

   // Check if path ends with the extension
   if(lowerPath.length() >= lowerExt.length())
   {
      return lowerPath.substr(lowerPath.length() - lowerExt.length()) == lowerExt;
   }

   return false;
}

bool FileSystem::createDirectoryWithPermissions(const std::string& path)
{
   if(!isDirectory(path))
   {
      if(!createDirectory(path))
      {
         std::cerr <<"Could not create directory: " << path 
                   <<"\nPlease try to delete /var/tmp/QFS and try again" << std::endl;
         return false;
      }
   }
#ifndef TOOLS_TARGET_WINDOWS
   if(!checkDirectoryPermissions(path, ALL_USER_PERMISSIONS))
   {
      changeFolderPermission(path, ALL_USER_PERMISSIONS);
   }
#endif
   return true;
}

// ========== Cross-Platform Temporary Directory Operations ==========
bool FileSystem::createDirectoryReclusively(const std::string& path)
{
   if(path.empty())
   {
      return false;
   }

   size_t pos = 0;
   std::string current;

   // Checking if directory already exists and has correct permissions.
   // If not, continue to create directory and set correct permissions
   if(isDirectory(path))
   {
#ifndef TOOLS_TARGET_WINDOWS
      if(checkDirectoryPermissions(path, ALL_USER_PERMISSIONS))
      {
         return true;
      }
      else
      {
         std::cerr <<
            "Directory: " + path +
            " exists, but does not have 0777 permissions\n"
            "Attempting to change permissions..." << std::endl;
      }
#else
      return true;
#endif
   }
   else
   {
      std::cerr <<
         "Directory: " + path +
         " does not exist.\n"
         "Attempting to create directory: " +
         path + " with 0777 permissions" << std::endl;
   }

   // Process intermediate directories
   while((pos = path.find_first_of("/\\", pos)) != std::string::npos)
   {
      current = path.substr(0, pos++);

      if(!current.empty())
      {
         createDirectoryWithPermissions(current);
      }
   }

   // Create final directory if missing
   createDirectoryWithPermissions(path);

// Check if final directory has correct permissions
#ifndef TOOLS_TARGET_WINDOWS
   if(!checkDirectoryPermissions(path, ALL_USER_PERMISSIONS))
   {
      throw std::runtime_error("Could not create directory: " + path + " with 0777 permissions");
   }
#endif

   return true;
}

bool FileSystem::createDirectory(const std::string& path)
{
   if(path.empty())
   {
      return false;
   }

   // Check if directory already exists
   if(isDirectory(path))
   {
      return true;
   }

#ifdef TOOLS_TARGET_WINDOWS
   return CreateDirectoryA(path.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
#elif defined(TOOLS_TARGET_LINUX)

   int result = mkdir(path.c_str(), ALL_USER_PERMISSIONS);
   if(result == 0)
   {
      return true; // Created successfully
   }
   if(errno == EEXIST)
   {
      return true; // Already exists
   }
   return false; // Other error

#endif
}

bool FileSystem::isDirectoryWritable(const std::string& path)
{
   if(!isDirectory(path))
   {
      return false;
   }

   // Test write permissions by creating and removing a test file
   std::string testFile = joinPath(path, "test_write_permissions");

   std::ofstream test(testFile);
   if(!test.is_open())
   {
      return false;
   }

   test << "test";
   test.close();

   // Check if file was created successfully
   bool success = fileExists(testFile);

   // Clean up test file
   if(success)
   {
      removeFile(testFile);
   }

   return success;
}

std::vector<std::string> FileSystem::getTempDirectoryPaths()
{
   std::vector<std::string> tempPaths;

#ifdef TOOLS_TARGET_WINDOWS
   // Windows temporary directory paths in order of preference
   char tempPath[MAX_PATH];

   // Try GetTempPath first (usually C:\Users\<user>\AppData\Local\Temp)
   if(GetTempPathA(MAX_PATH, tempPath) > 0)
   {
      std::string path = tempPath;
      // Remove trailing backslash if present
      if(!path.empty() && path.back() == '\\')
      {
         path.pop_back();
      }
      tempPaths.push_back(path);
   }

   // Fallback options for Windows
   const char* envVars[] = {"TEMP", "TMP", "USERPROFILE"};
   for(const char* envVar: envVars)
   {
      const char* envPath = getenv(envVar);
      if(envPath && strlen(envPath) > 0)
      {
         std::string path = envPath;
         if(envVar == std::string("USERPROFILE"))
         {
            path = joinPath(path, "AppData\\Local\\Temp");
         }
         tempPaths.push_back(path);
      }
   }

   // Last resort for Windows
   tempPaths.push_back("C:\\Temp");
   tempPaths.push_back(".");

#elif defined(TOOLS_TARGET_LINUX)
   // Linux temporary directory paths in order of preference
   tempPaths.push_back("/tmp");
   tempPaths.push_back("/var/tmp");

   // Try environment variables
   const char* envVars[] = {"TMPDIR", "TMP", "TEMP"};
   for(const char* envVar: envVars)
   {
      const char* envPath = getenv(envVar);
      if(envPath && strlen(envPath) > 0)
      {
         tempPaths.push_back(envPath);
      }
   }

   // User home directory as fallback
   const char* home = getenv("HOME");
   if(home && strlen(home) > 0)
   {
      tempPaths.push_back(joinPath(home, "tmp"));
   }

   // Last resort
   tempPaths.push_back(".");
#endif

   return tempPaths;
}

bool FileSystem::removeFile(const std::string& path)
{
   if(path.empty() || !fileExists(path))
   {
      return false;
   }

#ifdef TOOLS_TARGET_WINDOWS
   return DeleteFileA(path.c_str()) != 0;
#elif defined(TOOLS_TARGET_LINUX)
   return unlink(path.c_str()) == 0;
#endif
}

std::string FileSystem::createTempDirectory(const std::string& baseName)
{
   std::vector<std::string> tempBasePaths = getTempDirectoryPaths();

   for(const std::string& basePath: tempBasePaths)
   {
      // Skip if base path doesn't exist
      if(!isDirectory(basePath))
      {
         continue;
      }

      std::string tempDir = joinPath(basePath, baseName);

      // Try to create the directory
      if(createDirectory(tempDir))
      {
         // Verify the directory was created and is writable
         if(isDirectory(tempDir) && isDirectoryWritable(tempDir))
         {
            return tempDir;
         }
      }
   }

   // If all attempts failed, return empty string
   return "";
}

std::string FileSystem::getDirectoryPath(const std::string& filePath)
{
   // Handle empty input
   if(filePath.empty())
   {
      return "";
   }

   // Find last directory separator
   size_t pos = filePath.find_last_of("/\\");

   // No separator found = file in current directory
   if(pos == std::string::npos)
   {
      return "./";
   }

   // Special case: root path (e.g., "/file" → "/")
   if(pos == 0 && filePath[0] == '/')
   {
      return "/";
   }

   // Windows drive root (e.g., "C:\\file" → "C:\\")
   if(pos == 2 && filePath.length() > 2 && filePath[1] == ':' && (filePath[2] == '\\' || filePath[2] == '/'))
   {
      return filePath.substr(0, 3);
   }

   // Return path up to and including last separator
   return filePath.substr(0, pos + 1);
}

// ========== Platform-Specific Implementations ==========

#ifdef TOOLS_TARGET_WINDOWS

std::vector<std::string> FileSystem::listDirectoryWindows(const std::string& directory, bool recursive)
{
   std::vector<std::string> results;

   std::string searchPath = joinPath(directory, "*");
   WIN32_FIND_DATAA findData;
   HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

   if(hFind == INVALID_HANDLE_VALUE)
   {
      return results;
   }

   do
   {
      std::string name = findData.cFileName;

      // Skip . and ..
      if(name == "." || name == "..")
      {
         continue;
      }

      std::string fullPath = joinPath(directory, name);

      if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         if(recursive)
         {
            auto subResults = listDirectoryWindows(fullPath, recursive);
            results.insert(results.end(), subResults.begin(), subResults.end());
         }
      }
      else
      {
         results.push_back(fullPath);
      }
   } while(FindNextFileA(hFind, &findData) != 0);

   FindClose(hFind);
   return results;
}

#elif defined(TOOLS_TARGET_LINUX)

std::vector<std::string> FileSystem::listDirectoryLinux(const std::string& directory, bool recursive)
{
   std::vector<std::string> results;

   DIR* dir = opendir(directory.c_str());
   if(!dir)
   {
      return results;
   }

   struct dirent* entry;

   while((entry = readdir(dir)) != nullptr)
   {
      std::string name = entry->d_name;

      // Skip . and ..
      if(name == "." || name == "..")
      {
         continue;
      }

      std::string fullPath = joinPath(directory, name);
      struct stat statbuf;

      if(stat(fullPath.c_str(), &statbuf) != 0)
      {
         continue;
      }

      if(S_ISDIR(statbuf.st_mode))
      {
         if(recursive)
         {
            auto subResults = listDirectoryLinux(fullPath, recursive);
            results.insert(results.end(), subResults.begin(), subResults.end());
         }
      }
      else if(S_ISREG(statbuf.st_mode))
      {
         results.push_back(fullPath);
      }
   }

   closedir(dir);
   return results;
}


void FileSystem::changeFolderPermission(const std::string& directory, const mode_t& permissions)
{
   struct stat info;
   if(stat(directory.c_str(), &info) == 0)
   {
      if(info.st_uid == getuid())
      {
         std::ostringstream oss;
         oss << "Changing directory permissions to " << std::oct << permissions << " for " << directory << std::endl;
         std::cerr << oss.str();
         chmod(directory.c_str(), permissions);
      }
      else
      {
         std::ostringstream oss;
         oss << "Skipping permission change for " << directory << " (owned by UID " << info.st_uid << ", current UID "
             << getuid() << ")" << std::endl;
         std::cerr << oss.str();
      }
   }
   else
   {
      std::cerr <<"stat failed for " + directory << std:: endl;
   }
}


bool FileSystem::checkDirectoryPermissions(const std::string& directory, const mode_t& permissions)
{
   struct stat info;
   if(stat(directory.c_str(), &info) != 0)
   {
      std::cerr << "Unable to access directory " + directory << std::endl;
      return false;
   }

   if(S_ISDIR(info.st_mode))
   {
      if(((info.st_mode & permissions) == permissions))
      {
         return true;
      }
   }

   return false;
}

#endif

} // namespace CLI
} // namespace QC

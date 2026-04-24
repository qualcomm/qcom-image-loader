// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <map>
#include <string>
#include <vector>

// Platform detection macros (already defined by CMake)
// TOOLS_TARGET_WINDOWS - defined on Windows
// TOOLS_TARGET_LINUX - defined on Linux

#ifdef TOOLS_TARGET_WINDOWS
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#define PATH_SEPARATOR "\\"
#define PATH_SEPARATOR_CHAR '\\'
#elif defined(TOOLS_TARGET_LINUX)
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#define PATH_SEPARATOR "/"
#define PATH_SEPARATOR_CHAR '/'
extern const mode_t ALL_USER_PERMISSIONS;
#else
#error "Unsupported platform"
#endif


namespace QC {
namespace CLI {

/**
 * @brief Cross-platform file system utilities for CLI operations
 *
 * This class provides a unified interface for file system operations
 * that works on both Windows and Linux platforms using the appropriate
 * platform-specific system calls wrapped behind a common API.
 */
class FileSystem
{
public:
   // ========== CLI-Specific File Operations ==========

   /**
    * @brief Find device programmer with priority order and return sahara image
    * list
    *
    * Searches for device programmer in priority order:
    * 1. qsahara_device_programmer.xml (returns full sahara image list)
    * 2. xbl_s_devprg_ns.melf or xbl_s_devprg_ns.elf (returns as ID 13)
    * 3. prog_firehose_ddr.mbn or prog_firehose_ddr.elf (returns as ID 13)
    * 4. prog_firehose_lite.mbn or prog_firehose_lite.elf (returns as ID 13)
    * 5. Fallback to other prog_firehose*.elf files
    * 6. Fallback to other prog_firehose*.mbn files
    *
    * @param buildPath Build directory path
    * @return Map of sahara image ID to file path (empty if not found)
    */
   static std::map<int, std::string> findDeviceProgrammer(const std::string& buildPath);

   /**
    * @brief Find build validation file in build directory
    *
    * Searches for BuildValidation*.digest files
    *
    * @param buildPath Build directory path
    * @return Path to build validation file, or empty string if not found
    */
   static std::string findBuildValidationFile(const std::string& buildPath);

   /**
    * @brief Find raw program XML files in build directory
    *
    * @param buildPath Build directory path
    * @return Sorted vector of raw program XML file paths
    */
   static std::vector<std::string> findRawProgramXmls(const std::string& buildPath);

   /**
    * @brief Find patch program XML files in build directory
    *
    * @param buildPath Build directory path
    * @return Sorted vector of patch program XML file paths
    */
   static std::vector<std::string> findPatchProgramXmls(const std::string& buildPath);

   /**
    * @brief Read sahara image list from qsahara_device_programmer.xml
    *
    * @param buildPath Build directory path
    * @return Map of image ID to file path
    */
   static std::map<int, std::string> readSaharaImageList(const std::string& buildPath);

   /**
    * @brief Read sahara image list from specific XML file
    *
    * @param xmlFilePath Full path to sahara XML file
    * @param buildPath Build directory path for resolving relative paths
    * @return Map of image ID to file path
    */
   static std::map<int, std::string>
   readSaharaImageListFromFile(const std::string& xmlFilePath, const std::string& buildPath);

   /**
    * @brief Sort partition files in natural order
    *
    * Sorts files like rawprogram0.xml, rawprogram1.xml, rawprogram10.xml
    * in correct numerical order
    *
    * @param files Vector of file paths to sort (modified in place)
    */
   static void sortPartitionFiles(std::vector<std::string>& files);

   // ========== Cross-Platform File System Operations ==========

   /**
    * @brief Find files matching a pattern in a directory
    *
    * @param directory Directory to search in
    * @param pattern Regex pattern to match (e.g., "rawprogram.*\\.xml$")
    * @param recursive Whether to search recursively
    * @return Vector of matching file paths
    */
   static std::vector<std::string>
   findFiles(const std::string& directory, const std::string& pattern, bool recursive = false);

   /**
    * @brief Check if a file exists
    *
    * @param path File path to check
    * @return true if file exists, false otherwise
    */
   static bool fileExists(const std::string& path);

   /**
    * @brief Check if a path is a directory
    *
    * @param path Path to check
    * @return true if path is a directory, false otherwise
    */
   static bool isDirectory(const std::string& path);

   /**
    * @brief Check if path is a regular file
    * @param path Path to check
    * @return true if regular file, false otherwise
    */
   static bool isFile(const std::string& path);

   /**
    * @brief Normalize path separators for current platform
    * @param path Path to normalize
    * @return Normalized path
    */
   static std::string normalizePath(const std::string& path);

   /**
    * @brief Convert relative path to absolute path
    * @param path Path to convert (can be relative or absolute)
    * @return Absolute path
    */
   static std::string getAbsolutePath(const std::string& path);

   /**
    * @brief Join path components
    * @param base Base path
    * @param component Component to join
    * @return Joined path
    */
   static std::string joinPath(const std::string& base, const std::string& component);

   /**
    * @brief Extract filename from path (cross-platform)
    * @param path Full path
    * @return Filename portion only
    */
   static std::string getFilename(const std::string& path);

   /**
    * @brief Extract directory path from full path (cross-platform)
    * @param path Full path
    * @return Directory portion only
    */
   static std::string getDirectoryPath(const std::string& path);

   /**
    * @brief Check if path is absolute (cross-platform)
    * @param path Path to check
    * @return true if absolute path, false if relative
    */
   static bool isAbsolutePath(const std::string& path);

   /**
    * @brief Check if file has specific extension (case-insensitive)
    * @param path File path to check
    * @param extension Extension to check (with or without dot)
    * @return true if file has the extension, false otherwise
    */
   static bool hasExtension(const std::string& path, const std::string& extension);

   // ========== Cross-Platform Temporary Directory Operations ==========
   /**
    * @brief Create a directory reclusively
    * @param path Directory path to create
    * @return true if successful or already exists, false otherwise
    */
   static bool createDirectoryReclusively(const std::string& path);

   /**
    * @brief Create a directory with cross-platform support
    * @param path Directory path to create
    * @return true if successful or already exists, false otherwise
    */
   static bool createDirectory(const std::string& path);

   /**
    * @brief Test if a directory is writable by creating and removing a test
    * file
    * @param path Directory path to test
    * @return true if writable, false otherwise
    */
   static bool isDirectoryWritable(const std::string& path);

   /**
    * @brief Get platform-specific temporary directory paths in order of
    * preference
    * @return vector of temporary directory paths to try
    */
   static std::vector<std::string> getTempDirectoryPaths();

   /**
    * @brief Remove a file with cross-platform support
    * @param path File path to remove
    * @return true if successful, false otherwise
    */
   static bool removeFile(const std::string& path);

   /**
    * @brief Create a temporary directory with proper permissions and fallback
    * handling
    * @param baseName Base name for the temporary directory
    * @return path to created temporary directory, empty string if all attempts
    * failed
    */
   static std::string createTempDirectory(const std::string& baseName);

   /**
    * @brief Check if a file has a specific file extension
    * @param path File path of file to check
    * @param extension Extension to check against
    * @return True if path has extension, false otherwise
    */
   static bool hasFileExtension(const std::string& path, const std::string& extension);

   // ========== Cross-Platform Threading Utilities ==========

   /**
    * @brief Sleep for specified milliseconds
    * @param milliseconds Time to sleep
    */
   static void sleepMs(int milliseconds);

private:
   /**
    * @brief Extract numeric suffix from filename for sorting
    *
    * @param filename Filename to extract number from
    * @return Numeric value, or -1 if no number found
    */
   static int extractNumber(const std::string& filename);

   /**
    * @brief Creates a directory with specified permissions. If Directory
    * already exists, checks and changes to correct permissions
    * @param path Directory to Create
    * @return True if directory created, else false
    */
   static bool createDirectoryWithPermissions(const std::string& path);


#ifdef TOOLS_TARGET_WINDOWS
   /**
    * @brief Windows-specific directory listing implementation
    */
   static std::vector<std::string> listDirectoryWindows(const std::string& directory, bool recursive);
#elif defined(TOOLS_TARGET_LINUX)
   /**
    * @brief Linux-specific directory listing implementation
    */
   static std::vector<std::string> listDirectoryLinux(const std::string& directory, bool recursive);

   /**
    * @brief Change a directory's permissions
    *
    * @param directory Directory to change permissions
    * @param permissions Permissions to apply to directory
    */
   static void changeFolderPermission(const std::string& directory, const mode_t& permissions);

   /**
    * @brief Check a directory's permissions
    *
    * @param directory Directory to check permissions
    * @param permissions Permissions to check for
    * @return True if directory's permissions match, else false
    */
   static bool checkDirectoryPermissions(const std::string& directory, const mode_t& permissions);

#endif
};

} // namespace CLI
} // namespace QC

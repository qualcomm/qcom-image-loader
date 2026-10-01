// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace Util {

/**
 * @brief Cross-platform file system utilities using std::filesystem
 *
 * Provides modern C++17 std::filesystem-based operations for path handling,
 * file queries, directory operations, and file copying with progress tracking.
 */
class FileSystem
{
public:
   /**
    * @brief Get last error message from file system operations
    *
    * @return Error message string, empty if no error
    */
   static std::string getLastError();

   /**
    * @brief Set error from Windows GetLastError()
    *
    * @param errorCode Windows error code from GetLastError()
    */
   static void setWindowsError(unsigned long errorCode);

   /**
    * @brief Set error from Linux errno
    *
    * @param errorCode Linux errno value
    */
   static void setLinuxError(int errorCode);

   /**
    * @brief Convert filesystem path to UTF-8 string
    *
    * @param path Path to convert
    * @return UTF-8 encoded string representation
    */
   static std::string toUtf8(const std::filesystem::path& path);

   /**
    * @brief Resolve path to absolute, normalized form
    *
    * Combines absolute path conversion with lexical normalization.
    * Handles both existing and non-existing paths.
    *
    * @param path Path to resolve
    * @return Absolute, normalized path
    */
   static std::filesystem::path resolvePath(const std::filesystem::path& path);

   /**
    * @brief Check if path is a regular file
    *
    * @param path Path to check
    * @return true if regular file, false otherwise
    */
   static bool isRegularFile(const std::filesystem::path& path);

   /**
    * @brief Check if file exists
    *
    * @param path Path to check
    * @return true if file exists, false otherwise
    */
   static bool fileExists(const std::filesystem::path& path);

   /**
    * @brief Check if directory exists
    *
    * @param path Path to check
    * @return true if directory exists, false otherwise
    */
   static bool directoryExists(const std::filesystem::path& path);

   /**
    * @brief Get file size in bytes
    *
    * @param filePath Path to file
    * @return File size in bytes, 0 if error
    */
   static long long getFileSize(const std::filesystem::path& filePath);

   /**
    * @brief Get directory name (parent path) from full path
    *
    * @param path Full path
    * @return Parent directory path
    */
   static std::filesystem::path getDirectoryName(const std::filesystem::path& path);

   /**
    * @brief Get filename from full path
    *
    * @param path Full path
    * @return Filename only
    */
   static std::filesystem::path getFileName(const std::filesystem::path& path);

   /**
    * @brief Get filename without extension
    *
    * @param path Full path
    * @return Filename stem (without extension)
    */
   static std::filesystem::path getFileNameWithoutExtension(const std::filesystem::path& path);

   /**
    * @brief Combine multiple path components
    *
    * Handles absolute path components correctly and normalizes result.
    *
    * @param paths Vector of path components to combine
    * @return Combined, normalized path
    */
   static std::filesystem::path combinePath(const std::vector<std::filesystem::path>& paths);

   /**
    * @brief List files in directory
    *
    * @param path Directory path
    * @param recursive Whether to search recursively
    * @return Vector of file paths found
    */
   static std::vector<std::filesystem::path> getFilesInDirectory(
      const std::filesystem::path& path,
      bool recursive = false);

   /**
    * @brief Create directory recursively
    *
    * @param path Directory path to create
    * @return true if successful or already exists, false otherwise
    */
   static bool createDirectory(const std::filesystem::path& path);

   /**
    * @brief Copy file with optional progress callback
    *
    * On Windows, uses CopyFileExW for progress tracking.
    * On Linux, uses std::filesystem::copy_file.
    *
    * @param source Source file path
    * @param destination Destination file path
    * @param progressFn Optional callback for progress (bytes transferred)
    * @return true if successful, false otherwise
    */
   static bool copyFile(
      const std::filesystem::path& source,
      const std::filesystem::path& destination,
      const std::function<void(long long)>& progressFn = nullptr);

   /**
    * @brief Print path to logger
    *
    * @param path Path to print
    */
   static void printPath(const std::filesystem::path& path);

   /**
    * @brief Check if full path ends with relative path (component-by-component)
    *
    * Case-insensitive on Windows, case-sensitive on Unix/Linux.
    *
    * @param fullPath Full path to check
    * @param relativePath Relative path suffix to match
    * @return true if fullPath ends with relativePath, false otherwise
    */
   static bool pathEndsWith(
      const std::filesystem::path& fullPath,
      const std::filesystem::path& relativePath);
};

} // namespace Util



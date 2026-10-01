// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "FileSystem.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace Util {

static thread_local std::error_code s_lastError;

std::string FileSystem::getLastError()
{
   return s_lastError ? s_lastError.message() : std::string{};
}

void FileSystem::setWindowsError(unsigned long errorCode)
{
#ifdef _WIN32
   s_lastError = std::error_code(static_cast<int>(errorCode), std::system_category());
#endif
}

void FileSystem::setLinuxError(int errorCode)
{
#ifndef _WIN32
   s_lastError = std::error_code(errorCode, std::system_category());
#endif
}

std::string FileSystem::toUtf8(const std::filesystem::path& path)
{
   return path.u8string();
}

std::filesystem::path FileSystem::resolvePath(const std::filesystem::path& path)
{
   if(path.empty())
      return path;

   std::error_code ec;
   auto result = fs::weakly_canonical(fs::absolute(path), ec);
   if(ec)
   {
      s_lastError = ec;
      return fs::absolute(path).lexically_normal();
   }
   s_lastError.clear();
   return result;
}

bool FileSystem::isRegularFile(const std::filesystem::path& path)
{
   std::error_code ec;
   return fs::is_regular_file(path, ec);
}

bool FileSystem::fileExists(const std::filesystem::path& path)
{
   std::error_code ec;
   bool exists = fs::is_regular_file(path, ec);
   if(ec)
   {
      s_lastError = ec;
   }
   else
   {
      s_lastError.clear();
   }
   return exists;
}

bool FileSystem::directoryExists(const std::filesystem::path& path)
{
   std::error_code ec;
   return fs::is_directory(path, ec);
}

long long FileSystem::getFileSize(const std::filesystem::path& filePath)
{
   std::error_code ec;
   auto sz = fs::file_size(filePath, ec);
   if(ec)
   {
      s_lastError = ec;
      return 0;
   }
   s_lastError.clear();
   return static_cast<long long>(sz);
}

std::filesystem::path FileSystem::getDirectoryName(const std::filesystem::path& path)
{
   if(path.empty())
      return {};
   return path.parent_path();
}

std::filesystem::path FileSystem::getFileName(const std::filesystem::path& path)
{
   if(path.empty())
      return {};
   return path.filename();
}

std::filesystem::path FileSystem::getFileNameWithoutExtension(const std::filesystem::path& path)
{
   if(path.empty())
      return {};
   return path.stem();
}

std::filesystem::path FileSystem::combinePath(const std::vector<std::filesystem::path>& paths)
{
   if(paths.empty())
      return {};

   std::filesystem::path result = paths[0];
   for(size_t i = 1; i < paths.size(); ++i)
   {
      const std::filesystem::path& part = paths[i];
      if(part.empty())
         continue;

      if(part.is_absolute())
      {
         result /= part.relative_path();
      }
      else
      {
         result /= part;
      }
   }

   return result.lexically_normal();
}

std::vector<std::filesystem::path> FileSystem::getFilesInDirectory(
   const std::filesystem::path& path,
   bool recursive)
{
   std::vector<std::filesystem::path> files;
   std::error_code ec;

   if(!fs::exists(path, ec) || !fs::is_directory(path, ec))
   {
      s_lastError.clear();
      return files;
   }

   if(recursive)
   {
      for(const auto& entry : fs::recursive_directory_iterator(
             path, fs::directory_options::skip_permission_denied, ec))
      {
         std::error_code eEntry;
         if(entry.is_regular_file(eEntry))
         {
            files.push_back(entry.path());
         }
      }
   }
   else
   {
      for(const auto& entry : fs::directory_iterator(
             path, fs::directory_options::skip_permission_denied, ec))
      {
         std::error_code eEntry;
         if(entry.is_regular_file(eEntry))
         {
            files.push_back(entry.path());
         }
      }
   }
   if(ec)
      s_lastError = ec;
   else
      s_lastError.clear();
   return files;
}

bool FileSystem::createDirectory(const std::filesystem::path& path)
{
   if(path.empty())
   {
      s_lastError = std::make_error_code(std::errc::invalid_argument);
      return false;
   }
   std::error_code ec;
   fs::create_directories(path, ec);
   if(ec && !fs::is_directory(path))
   {
      s_lastError = ec;
      return false;
   }
   s_lastError.clear();
   return true;
}

#ifdef _WIN32
struct CopyProgressData
{
   const std::function<void(long long)>* progressFn;
};

static DWORD WINAPI copyProgressRoutine(
   LARGE_INTEGER /*TotalFileSize*/,
   LARGE_INTEGER TotalBytesTransferred,
   LARGE_INTEGER /*StreamSize*/,
   LARGE_INTEGER /*StreamBytesTransferred*/,
   DWORD /*dwStreamNumber*/,
   DWORD /*dwCallbackReason*/,
   HANDLE /*hSourceFile*/,
   HANDLE /*hDestinationFile*/,
   LPVOID lpData)
{
   auto* d = static_cast<CopyProgressData*>(lpData);
   if(d->progressFn && *(d->progressFn))
      (*d->progressFn)(TotalBytesTransferred.QuadPart);
   return PROGRESS_CONTINUE;
}
#endif

bool FileSystem::copyFile(
   const std::filesystem::path& source,
   const std::filesystem::path& destination,
   const std::function<void(long long)>& progressFn)
{
#ifdef _WIN32
   // Normalize paths using lexically_normal to handle double backslashes
   auto normalizedSource = source.lexically_normal();
   auto normalizedDest = destination.lexically_normal();

   CopyProgressData data{&progressFn};
   if(!CopyFileExW(normalizedSource.c_str(), normalizedDest.c_str(),
                   copyProgressRoutine, &data, nullptr, 0))
   {
      s_lastError = std::error_code(static_cast<int>(GetLastError()), std::system_category());
      return false;
   }
   s_lastError.clear();
   return true;

#else
   std::error_code ec;
   fs::copy_file(source, destination, fs::copy_options::overwrite_existing, ec);
   if(ec)
   {
      s_lastError = ec;
      return false;
   }
   s_lastError.clear();

   if(progressFn)
   {
      std::error_code szEc;
      auto sz = fs::file_size(destination, szEc);
      if(!szEc)
         progressFn(static_cast<long long>(sz));
   }
   return true;
#endif
}

void FileSystem::printPath(const std::filesystem::path& path)
{
#ifdef _WIN32
   std::wcout << path;
#else
   std::cout << path;
#endif
}

bool FileSystem::pathEndsWith(
   const std::filesystem::path& fullPath,
   const std::filesystem::path& relativePath)
{
   const std::filesystem::path full = fullPath.lexically_normal();
   const std::filesystem::path rel = relativePath.lexically_normal();

   std::vector<std::filesystem::path> fullParts(full.begin(), full.end());
   std::vector<std::filesystem::path> relParts(rel.begin(), rel.end());

   if(relParts.empty())
      return true;
   if(relParts.size() > fullParts.size())
      return false;

   auto sameComponent = [](const std::filesystem::path& a, const std::filesystem::path& b) -> bool {
#ifdef _WIN32
      auto wa = a.wstring();
      auto wb = b.wstring();
      std::transform(wa.begin(), wa.end(), wa.begin(), ::towlower);
      std::transform(wb.begin(), wb.end(), wb.begin(), ::towlower);
      return wa == wb;
#else
      return a == b;
#endif
   };

   auto it = std::search(
      fullParts.rbegin(), fullParts.rend(),
      relParts.rbegin(), relParts.rend(),
      sameComponent);

   return it == fullParts.rbegin();
}


} // namespace Util




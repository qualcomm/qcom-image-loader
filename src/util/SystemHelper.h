// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
// #include "platform/Assertions.h"
#include "util/StringHelper.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#ifdef TOOLS_TARGET_WINDOWS
#include <process.h>
#include <psapi.h>
#include <sddl.h>
#include <tlhelp32.h>
#include <windows.h>
#elif defined TOOLS_TARGET_LINUX
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined TOOLS_TARGET_OSX
#include <mach/mach.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
namespace Util {

// ----------------------------------------------------------------------------
// createTempFileName
//
///
/// @returns the temp file path
// ----------------------------------------------------------------------------
inline std::filesystem::path createTempFileName(const std::filesystem::path& directory)
{
#ifdef TOOLS_TARGET_WINDOWS
   wchar_t buf[MAX_PATH] = {};
   if(0 == ::GetTempFileNameW(directory.wstring().c_str(), L"tmp", 0, buf))
      throw std::runtime_error("GetTempFileNameW failed");
   return std::filesystem::path(buf);
#else
   std::string tmpl = (directory / "tmpXXXXXX").string();
   int fd = ::mkstemp(tmpl.data());
   if(fd == -1) throw std::runtime_error("mkstemp failed");
   ::close(fd);
   return std::filesystem::path(tmpl);
#endif
}

enum AccessPrivileges
{
   CURRENT_USER = 0,
   ALLOW_ALL_USERS
};

// ----------------------------------------------------------------------------
// createDirectory
//
///
/// @returns the directory and set all permission
// ----------------------------------------------------------------------------
inline void createPath(const std::filesystem::path& directory, AccessPrivileges privileges = ALLOW_ALL_USERS)
{
   if(std::filesystem::is_directory(directory))
   {
      return;
   }
   if(privileges == CURRENT_USER)
   {
      std::error_code errorCode;
      if(std::filesystem::is_directory(directory))
      {
         return;
      } // Already a directory
      std::filesystem::create_directories(directory, errorCode);
      if(errorCode)
      {
         TOOLS_THROW(ToolException(
            std::string("Create directory ") + directory.string() + std::string(" failed:") + errorCode.message()
         ));
      }
   }

   if(privileges == ALLOW_ALL_USERS)
   {
#if defined TOOLS_TARGET_WINDOWS
      SECURITY_ATTRIBUTES securityAttributes;

      securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
      securityAttributes.bInheritHandle = FALSE;

      static WCHAR* szSD =
         L"D:"                   // Discretionary ACL
                                 // L"(A;OICI;GA;;;BG)"     // Deny access to built-in guests
                                 // L"(A;OICI;GA;;;AN)"     // Deny access to anonymous logon
         L"(A;OICI;GRGWGX;;;AU)" // Allow read/write/execute to authenticated
                                 // users
         L"(A;OICI;GA;;;WD)"     // Allow full control to everyone
         L"(A;OICI;GA;;;BA)";    // Allow full control to administrators

      TOOLS_ASSERT_OR_THROW(
         ConvertStringSecurityDescriptorToSecurityDescriptorW(
            szSD,
            SDDL_REVISION_1,
            &(securityAttributes.lpSecurityDescriptor),
            NULL
         ),
         ToolException("Failed to Create Discretionary Access Control List")
      );

      if(!::CreateDirectoryW(Util::toWString(directory.string()).c_str(), &securityAttributes))
      {
         TOOLS_THROW(ToolException(
            std::string("Failed to Create Discretionary "
                        "Access "
                        "Control List, Error Code: ") +
            std::to_string(GetLastError())
         ));
      }
#elif defined TOOLS_TARGET_LINUX || defined TOOLS_TARGET_OSX
      // Clear user file-creation mode mask (umask) is used to determine the
      // file permission for newly created files file creation permission =
      // umask & mode
      TOOLS_IGNORE_EXCEPTIONS(umask(0));
      TOOLS_ASSERT_OR_THROW(
         0 == mkdir(directory.string().c_str(), 0777),
         ToolException(
            std::string("Could not create "
                        "directory ") +
            Util::quote(directory.string())
         )
      );
#else
#error Unknown target
#endif
   }
}

// ----------------------------------------------------------------------------
// generateUuid
//
///
/// @returns a random UUID
// ----------------------------------------------------------------------------
inline std::string generateUuid()
{
   std::random_device rd;
   std::mt19937_64 gen(rd());
   std::uniform_int_distribution<uint64_t> dis;
   std::ostringstream oss;
   oss << std::hex << std::setfill('0') << std::setw(16) << dis(gen) << std::setw(16) << dis(gen);
   return oss.str(); // 32-char hex string
}

// ----------------------------------------------------------------------------
// getPid
//
///
/// @returns the Process ID
// ----------------------------------------------------------------------------
inline int32_t getPid()
{
#ifdef TOOLS_TARGET_WINDOWS
   return _getpid();
#elif defined TOOLS_TARGET_LINUX || defined TOOLS_TARGET_OSX
   return getpid();
#else
#error Unknown target
#endif
}

// ----------------------------------------------------------------------------
// getComputerName
//
///
/// @returns the computer name
// ----------------------------------------------------------------------------
inline std::string getComputerName()
{
   const uint32_t MAX_COMPUTER_NAME = 256;
#if defined TOOLS_TARGET_WINDOWS
   DWORD ulBufferCharacterCount = MAX_COMPUTER_NAME;
   WCHAR computerName[MAX_COMPUTER_NAME] = {0};
   BOOL ret = GetComputerNameW(computerName, &ulBufferCharacterCount);
   TOOLS_ASSERT_OR_RETURN(!!ret, std::string("No computer name found."));
   return Util::fromWString(computerName);
#elif defined TOOLS_TARGET_LINUX || defined TOOLS_TARGET_OSX
   char hostname[MAX_COMPUTER_NAME] = {0};
   int32_t ret = gethostname(hostname, sizeof(hostname));
   TOOLS_ASSERT_OR_RETURN(ret == 0, std::string("No computer name found."));
   hostname[sizeof(hostname) - 1] = '\0';
   return std::string(hostname);
#else
   TOOLS_ASSERT_OR_RETURN(!"Implemented", std::string());
#endif
}

// ----------------------------------------------------------------------------
// getUserName
//
///
/// @returns the User name
// ----------------------------------------------------------------------------
inline std::string getUserName()
{
#if defined TOOLS_TARGET_WINDOWS
   DWORD ulBufferCharacterCount = MAX_PATH;
   WCHAR userName[MAX_PATH] = {0};
   BOOL ret = GetUserNameW(userName, &ulBufferCharacterCount);
   TOOLS_ASSERT_OR_RETURN(!!ret, std::string("No username found."));
   return Util::fromWString(userName);
#elif defined TOOLS_TARGET_LINUX || defined TOOLS_TARGET_OSX
   uid_t uid = geteuid();
   struct passwd* pInfo = getpwuid(uid);
   TOOLS_ASSERT_OR_RETURN(nullptr != pInfo, std::string("No username found."));
   return std::string(pInfo->pw_name);
#else
   TOOLS_ASSERT_OR_RETURN(!"Implemented", std::string());
#endif
}

// ----------------------------------------------------------------------------
// getAvailableDiskBytes
//
///
/// @returns available disk bytes
// ----------------------------------------------------------------------------
inline uint64_t getAvailableDiskBytes()
{
#if defined TOOLS_TARGET_WINDOWS
   auto s = std::filesystem::space("C:\\");
#elif defined TOOLS_TARGET_LINUX || defined TOOLS_TARGET_OSX
   auto s = std::filesystem::space("/");
#else
   TOOLS_ASSERT_OR_RETURN(!"Implemented", 0);
#endif
   return s.available; // bytes available to non-privileged process
}

// ----------------------------------------------------------------------------
// getNumThreads
//
///
/// @returns #threads
// ----------------------------------------------------------------------------
inline uint32_t getNumThreads(int32_t pid)
{
#if defined TOOLS_TARGET_WINDOWS

   HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
   if(snap == INVALID_HANDLE_VALUE) return 0;

   THREADENTRY32 te{};
   te.dwSize = sizeof(te);

   std::uint32_t count = 0;
   if(Thread32First(snap, &te))
   {
      do
      {
         if(te.th32OwnerProcessID == static_cast<DWORD>(pid)) ++count;
      } while(Thread32Next(snap, &te));
   }

   CloseHandle(snap);
   return count;

#elif defined TOOLS_TARGET_LINUX

   namespace fs = std::filesystem;
   std::uint32_t count = 0;
   std::string taskPath = "/proc/" + std::to_string(pid) + "/task";
   for(auto& e: fs::directory_iterator(taskPath))
      if(e.is_directory()) ++count;
   return count;

#elif defined TOOLS_TARGET_OSX

   thread_act_array_t threads;
   mach_msg_type_number_t count = 0;

   if(task_threads(mach_task_self(), &threads, &count) != KERN_SUCCESS) return 0;

   // Must deallocate returned array
   vm_deallocate(mach_task_self(), (vm_address_t)threads, count * sizeof(thread_t));
   return static_cast<std::uint32_t>(count);

#else
   TOOLS_ASSERT_OR_RETURN(!"Implemented", 0);
#endif
}


// ----------------------------------------------------------------------------
// getProcessMemoryUsageBytes
//
///
/// @returns memory usage in bytes
// ----------------------------------------------------------------------------
inline std::uint64_t getProcessMemoryUsageBytes()
{
#if defined TOOLS_TARGET_WINDOWS
   PROCESS_MEMORY_COUNTERS_EX pmc{};
   if(!GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
      return 0;

   // WorkingSetSize approximately equals RAM; PrivateUsage approximately equals
   // commit/private bytes (pagefile-backed)
   return static_cast<std::uint64_t>(pmc.WorkingSetSize) + static_cast<std::uint64_t>(pmc.PrivateUsage);

#elif defined TOOLS_TARGET_LINUX

   std::ifstream in("/proc/self/statm");
   long pagesTotal = 0, pagesRss = 0;
   if(!(in >> pagesTotal >> pagesRss)) return 0;
   return static_cast<std::uint64_t>(pagesRss) * static_cast<std::uint64_t>(sysconf(_SC_PAGESIZE));


#elif defined TOOLS_TARGET_OSX

   task_basic_info_data_t info{};
   mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;

   if(task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &count) != KERN_SUCCESS)
      return 0;

   return static_cast<std::uint64_t>(info.resident_size);

#else
   TOOLS_ASSERT_OR_RETURN(!"Implemented", 0);
#endif
}


// ----------------------------------------------------------------------------
// CpuLoad
//
///
/// @returns CPU load percentage
// ----------------------------------------------------------------------------
class CpuLoad
{
public:
   // System-wide CPU usage in [0.0, 100.0].
   // Call periodically (e.g., every 250-1000ms). First call returns 0.0.
   static double Get()
   {
      Sample cur{};
      if(!ReadSample(cur)) return 0.0;

      if(!hasPrev_)
      {
         prev_ = cur;
         hasPrev_ = true;
         return 0.0;
      }

      const std::uint64_t dIdle = cur.idle - prev_.idle;
      const std::uint64_t dTotal = cur.total - prev_.total;

      prev_ = cur;

      if(dTotal == 0) return 0.0;

      double percent = 100.0 * (1.0 - (static_cast<double>(dIdle) / static_cast<double>(dTotal)));

      // Clamp for rare counter anomalies
      if(percent < 0.0) percent = 0.0;
      if(percent > 100.0) percent = 100.0;
      return percent;
   }

private:
   struct Sample
   {
      std::uint64_t idle;
      std::uint64_t total;
   };

   static inline Sample prev_{0, 0};
   static inline bool hasPrev_ = false;

#ifdef TOOLS_TARGET_WINDOWS
   static std::uint64_t FileTimeToU64(const FILETIME& ft)
   {
      ULARGE_INTEGER li;
      li.LowPart = ft.dwLowDateTime;
      li.HighPart = ft.dwHighDateTime;
      return static_cast<std::uint64_t>(li.QuadPart);
   }
#endif

   static bool ReadSample(Sample& out)
   {
#ifdef TOOLS_TARGET_WINDOWS
      FILETIME idleFt{}, kernelFt{}, userFt{};
      if(!GetSystemTimes(&idleFt, &kernelFt, &userFt)) return false;

      const std::uint64_t idle = FileTimeToU64(idleFt);
      const std::uint64_t kernel = FileTimeToU64(kernelFt);
      const std::uint64_t user = FileTimeToU64(userFt);

      // Windows: kernel includes idle
      out.idle = idle;
      out.total = kernel + user;
      return true;

#elif defined TOOLS_TARGET_LINUX
      std::ifstream in("/proc/stat");
      if(!in.is_open()) return false;

      std::string line;
      std::getline(in, line);

      std::istringstream iss(line);
      std::string cpuLabel;

      std::uint64_t user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0,
                    guestNice = 0;
      iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guestNice;

      if(cpuLabel.rfind("cpu", 0) != 0) return false;

      const std::uint64_t idleAll = idle + iowait;
      const std::uint64_t nonIdle = user + nice + system + irq + softirq + steal;

      out.idle = idleAll;
      out.total = idleAll + nonIdle;
      return true;

#elif defined TOOLS_TARGET_OSX
      host_cpu_load_info_data_t cpuinfo{};
      mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;

      if(host_statistics64(mach_host_self(), HOST_CPU_LOAD_INFO, reinterpret_cast<host_info64_t>(&cpuinfo), &count) !=
         KERN_SUCCESS)
         return false;

      const std::uint64_t user = cpuinfo.cpu_ticks[CPU_STATE_USER];
      const std::uint64_t system = cpuinfo.cpu_ticks[CPU_STATE_SYSTEM];
      const std::uint64_t idle = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
      const std::uint64_t nice = cpuinfo.cpu_ticks[CPU_STATE_NICE];

      out.idle = idle;
      out.total = user + system + idle + nice;
      return true;
#endif
   }
};


} // namespace Util

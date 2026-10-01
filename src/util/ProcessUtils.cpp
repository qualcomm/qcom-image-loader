// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "ProcessUtils.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#endif

namespace Util {

static volatile bool timeout_occurred = false;

#ifdef _WIN32
static VOID CALLBACK TimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
   timeout_occurred = true;
}
#else
static void alarm_handler(int sig)
{
   timeout_occurred = true;
}
#endif

ProcessResult runExecutable(const std::string& command, int timeoutSeconds)
{
   try
   {

      ProcessResult result = {-1, "", "", false};


#ifdef _WIN32


      // Convert to wide string for UNC path support

      int wlen = MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, NULL, 0);


      if(wlen <= 0)
      {

         result.output = "Failed to convert command to wide string";
         return result;
      }

      std::wstring wcommand(wlen - 1, 0);

      int result_conv = MultiByteToWideChar(CP_UTF8, 0, command.c_str(), -1, &wcommand[0], wlen);


      if(result_conv <= 0)
      {

         result.output = "Failed to convert command to wide string";
         return result;
      }

      // Wrap with cmd /c for proper shell execution (like QFC does)

      std::wstring wCmdLine = L"cmd /c " + wcommand;


   SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
   HANDLE hRead = NULL, hWrite = NULL;
   if(!CreatePipe(&hRead, &hWrite, &sa, 0))
   {
      DWORD err = GetLastError();
      result.output = "Failed to create pipe";
      if(err != 0)
      {
         LPSTR errMsg = nullptr;
         FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, (LPSTR)&errMsg, 0, nullptr);
         if(errMsg)
         {
            result.output += " - " + std::string(errMsg);
            LocalFree(errMsg);
         }
      }
      return result;
   }


   SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

   STARTUPINFOW si = {sizeof(si)};
   si.dwFlags = STARTF_USESTDHANDLES;
   si.hStdOutput = hWrite;
   si.hStdError = hWrite;

   PROCESS_INFORMATION pi;


   BOOL success = CreateProcessW(
       NULL,
       (LPWSTR)wCmdLine.c_str(),
       NULL, NULL, TRUE, CREATE_NO_WINDOW,
       NULL, NULL, &si, &pi
   );



   CloseHandle(hWrite);

   if(!success)
   {
      DWORD err = GetLastError();
      result.output = "Failed to start process";
      if(err != 0)
      {
         LPSTR errMsg = nullptr;
         FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, (LPSTR)&errMsg, 0, nullptr);
         if(errMsg)
         {
            result.output += " - " + std::string(errMsg);
            LocalFree(errMsg);
         }
      }
      CloseHandle(hRead);
      return result;
   }

   const DWORD BUFFER_SIZE = 4096;
   char buffer[BUFFER_SIZE];
   DWORD bytesRead;
   std::string output;

   timeout_occurred = false;
   HANDLE hTimer = NULL;
   HANDLE hTimerQueue = CreateTimerQueue();
   if(hTimerQueue == NULL)
   {
      DWORD err = GetLastError();
      result.output = "Failed to create timer queue";
      if(err != 0)
      {
         LPSTR errMsg = nullptr;
         FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, (LPSTR)&errMsg, 0, nullptr);
         if(errMsg)
         {
            result.output += " - " + std::string(errMsg);
            LocalFree(errMsg);
         }
      }
      CloseHandle(hRead);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return result;
   }

   if(!CreateTimerQueueTimer(&hTimer, hTimerQueue, TimerCallback, NULL,
                             timeoutSeconds * 1000, 0, WT_EXECUTEONLYONCE))
   {
      DWORD err = GetLastError();
      std::string errMsg = "Failed to create timer";
      if(err != 0)
      {
         LPSTR errMsgPtr = nullptr;
         FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, (LPSTR)&errMsgPtr, 0, nullptr);
         if(errMsgPtr)
         {
            errMsg += " - " + std::string(errMsgPtr);
            LocalFree(errMsgPtr);
         }
      }
      DeleteTimerQueue(hTimerQueue);
      result.output = errMsg;
      CloseHandle(hRead);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      return result;
   }

   DWORD waitResult = WAIT_TIMEOUT;

   while(waitResult == WAIT_TIMEOUT && !timeout_occurred)
   {
      waitResult = WaitForSingleObject(pi.hProcess, 0);

      bool dataRead = false;
      do
      {
         dataRead = false;
         DWORD availableBytes = 0;

         if(PeekNamedPipe(hRead, NULL, 0, NULL, &availableBytes, NULL) && availableBytes > 0)
         {
            DWORD bytesToRead = (BUFFER_SIZE - 1 < availableBytes) ? BUFFER_SIZE - 1 : availableBytes;
            if(ReadFile(hRead, buffer, bytesToRead, &bytesRead, NULL) && bytesRead > 0)
            {
               buffer[bytesRead] = '\0';
               output += buffer;
               dataRead = true;
            }
         }
      } while(dataRead && waitResult == WAIT_TIMEOUT && !timeout_occurred);

      if(waitResult == WAIT_TIMEOUT && !timeout_occurred)
      {
         Sleep(10);
      }
   }

   DeleteTimerQueueTimer(hTimerQueue, hTimer, NULL);
   DeleteTimerQueue(hTimerQueue);

   while(ReadFile(hRead, buffer, BUFFER_SIZE - 1, &bytesRead, NULL) && bytesRead > 0)
   {
      buffer[bytesRead] = '\0';
      output += buffer;
   }

   if(waitResult == WAIT_TIMEOUT && timeout_occurred)
   {
      TerminateProcess(pi.hProcess, STILL_ACTIVE);
      WaitForSingleObject(pi.hProcess, INFINITE);
      result.exitCode = -1;
      result.output = output + "\nProcess terminated due to timeout";
      result.timedOut = true;
   }
   else
   {
      DWORD exitCode = 0;
      GetExitCodeProcess(pi.hProcess, &exitCode);
      result.exitCode = static_cast<int>(exitCode);
      result.output = output;
      result.timedOut = false;
   }

   CloseHandle(hRead);
   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);

   return result;

#else
   int pipefd[2];
   if(pipe(pipefd) < 0)
   {
      int err = errno;
      result.output = "Failed to create pipe";
      if(err != 0)
      {
         result.output += " - " + std::string(strerror(err));
      }
      return result;
   }

   pid_t pid = fork();

   if(pid == 0)
   {
      close(pipefd[0]);
      dup2(pipefd[1], STDOUT_FILENO);
      dup2(pipefd[1], STDERR_FILENO);
      close(pipefd[1]);

      execl("/bin/sh", "sh", "-c", command.c_str(), (char*)NULL);
      int err = errno;
      std::string errMsg = "Failed to execute shell";
      if(err != 0)
      {
         errMsg += " - " + std::string(strerror(err));
      }
      std::cerr << errMsg << std::endl;
      exit(1);
   }
   else if(pid < 0)
   {
      int err = errno;
      result.output = "Failed to fork process";
      if(err != 0)
      {
         result.output += " - " + std::string(strerror(err));
      }
      close(pipefd[0]);
      close(pipefd[1]);
      return result;
   }
   {
      close(pipefd[1]);

      int flags = fcntl(pipefd[0], F_GETFL, 0);
      fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

      timeout_occurred = false;
      signal(SIGALRM, alarm_handler);
      alarm(timeoutSeconds);

      const size_t BUFFER_SIZE = 4096;
      char buffer[BUFFER_SIZE];
      std::string output;
      int status = 0;

      while(!timeout_occurred)
      {
         ssize_t count = read(pipefd[0], buffer, BUFFER_SIZE - 1);

         if(count > 0)
         {
            buffer[count] = '\0';
            output += buffer;
         }
         else if(count == 0)
         {
            break;
         }
         else if(errno == EAGAIN || errno == EWOULDBLOCK)
         {
            int child_status;
            pid_t wait_result = waitpid(pid, &child_status, WNOHANG);
            if(wait_result == pid)
            {
               status = child_status;
               break;
            }
            else if(wait_result == -1)
            {
               break;
            }

            usleep(10000);
         }
         else
         {
            break;
         }
      }

      alarm(0);

      if(timeout_occurred)
      {
         kill(pid, SIGKILL);
         waitpid(pid, &status, 0);
         result.timedOut = true;
         result.exitCode = -1;
         result.output = output + "\nProcess terminated due to timeout";
      }
      else
      {
         waitpid(pid, &status, 0);
         result.timedOut = false;
         result.exitCode = status;
         result.output = output;
      }

      close(pipefd[0]);
   }
#endif

   return result;
   }
   catch(const std::exception& ex)
   {

      ProcessResult result = {-1, "", std::string("Exception: ") + ex.what(), false};
      return result;
   }
   catch(...)
   {

      ProcessResult result = {-1, "", "Unknown exception", false};
      return result;
   }
}

} // namespace Util




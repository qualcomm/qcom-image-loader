// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "Spinner.h"

#include <iomanip>
#include <iostream>
#include <KL/kLogger.h>
#include <sstream>

#ifdef TOOLS_TARGET_WINDOWS
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace QC {
namespace CLI {

const char Spinner::spinnerChars[] = {'|', '/', '-', '\\'};

Spinner::Spinner()
: m_spinnerIndex(0)
, m_bIsActive(false)
, m_bIsTTYMode(false)
, m_lineWidth(DEFAULT_LINE_WIDTH)
, m_currentMessage("")
, m_lastPercentage(-1)
, m_lineOffset(0)
{
   m_lastUpdateTime = std::chrono::steady_clock::now();
   m_bIsTTYMode = isTTY();
   if(m_bIsTTYMode)
   {
      m_lineWidth = getTerminalWidth();
   }
}

Spinner::~Spinner()
{
   if(m_bIsActive)
   {
      stop();
   }
}

void Spinner::start(const std::string& message)
{
   if(!m_bIsActive)
   {
      // Register this spinner with kLogger for automatic line offset management
      KL::Logger::get_instance().addSpinnerLine();
      m_bIsActive = true;
   }

   m_currentMessage = message;
   m_spinnerIndex = 0;
   m_lastPercentage = 0;
   render(0, message);
}

void Spinner::update(int percentage, const std::string& message)
{
   if(!m_bIsActive)
   {
      start(message);
   }

   // Clamp percentage to 0-100
   if(percentage < 0) percentage = 0;
   if(percentage > 100) percentage = 100;

   m_currentMessage = message;
   m_lastPercentage = percentage;

   // Update spinner animation
   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdateTime).count();

   // Update spinner char every 100ms
   if(elapsed >= 100)
   {
      m_spinnerIndex = (m_spinnerIndex + 1) % 4;
      m_lastUpdateTime = now;
   }

   render(percentage, message);
}

void Spinner::updateSpinnerOnly(const std::string& message)
{
   if(!m_bIsActive)
   {
      start(message);
   }

   m_currentMessage = message;

   // Update spinner animation
   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdateTime).count();

   // Update spinner char every 100ms
   if(elapsed >= 100)
   {
      m_spinnerIndex = (m_spinnerIndex + 1) % 4;
      m_lastUpdateTime = now;
   }

   // Render spinner without progress bar
   renderSpinnerOnly(message);
}

void Spinner::stop()
{
   if(m_bIsActive)
   {
      clearLine();
      m_bIsActive = false;

      // Unregister this spinner from kLogger
      KL::Logger::get_instance().removeSpinnerLine();
   }
}

void Spinner::finish(const std::string& message)
{
   if(m_bIsActive)
   {
      clearLine();
      KL::Logger::get_instance().consoleOverwriteLine(message + " (100%)\n");
      m_bIsActive = false;

      // Unregister this spinner from kLogger
      KL::Logger::get_instance().removeSpinnerLine();
   }
}

void Spinner::setLineOffset(int offset)
{
   m_lineOffset = offset;
}


void Spinner::clearLine()
{
   if(m_bIsTTYMode)
   {
      // Build complete output string
      std::ostringstream output;

      // Add cursor positioning prefix
      if(m_bIsTTYMode && m_lineOffset > 0)
      {
         output << "\033[s";                       // Save cursor position
         output << "\033[" << m_lineOffset << "A"; // Move up N lines
         output << "\r";                           // Move to beginning of line
      }
      else
      {
         output << "\r"; // Just carriage return for line 0
      }

      // Use ANSI escape to clear line (only clears this line, not content above/below)
      output << "\033[K"; // Clear from cursor to end of line

      // Add cursor positioning suffix
      if(m_lineOffset > 0)
      {
         output << "\033[u"; // Restore cursor position
      }
      else
      {
         output << "\r";
      }

      // Write to console via KLogger (atomic operation with flush)
      KL::Logger::get_instance().consoleOverwriteLine(output.str());
   }
}

void Spinner::render(int percentage, const std::string& message)
{
   if(m_bIsTTYMode)
   {
      // TTY mode: Show animated spinner with progress bar
      // Build the entire line in a string first, then output once
      std::ostringstream oss;

      // Create progress bar
      int barWidth = 20;
      int filledWidth = (barWidth * percentage) / 100;

      oss << "[" << getSpinnerChar() << "] ";

      // Extract base message and filename if present
      std::string baseMessage = message;
      std::string filename = "";

      // Check if message contains ": " separator (e.g., "Firmware Download:
      // boot.img")
      size_t separatorPos = message.find(": ");
      if(separatorPos != std::string::npos)
      {
         baseMessage = message.substr(0, separatorPos);
         filename = message.substr(separatorPos + 2);
      }

      // Add base message
      oss << baseMessage;

      oss << " [";

      for(int i = 0; i < barWidth; i++)
      {
         if(i < filledWidth)
         {
            oss << "=";
         }
         else
         {
            oss << " ";
         }
      }

      oss << "] " << std::setw(3) << percentage << "%";

      // Add filename at the end if present
      if(!filename.empty())
      {
         oss << " " << filename;
      }

      // Pad with spaces to ensure we overwrite any previous longer content
      std::string line = oss.str();
      size_t lineLen = static_cast<size_t>(m_lineWidth);
      if(line.length() < lineLen)
      {
         line.append(lineLen - line.length(), ' ');
      }

      // Build complete output string with cursor movement
      std::ostringstream output;

      // Add cursor positioning prefix
      if(m_bIsTTYMode && m_lineOffset > 0)
      {
         output << "\033[s";                       // Save cursor position
         output << "\033[" << m_lineOffset << "A"; // Move up N lines
         output << "\r";                           // Move to beginning of line
      }
      else
      {
         output << "\r"; // Just carriage return for line 0
      }

      // Add the line content
      output << line;

      // Add cursor positioning suffix
      if(m_lineOffset > 0)
      {
         output << "\033[u"; // Restore cursor position
      }
      else
      {
         output << "\r"; // For offset 0, return to beginning of line
      }

      // Write to console via KLogger (atomic operation with flush)
      KL::Logger::get_instance().consoleOverwriteLine(output.str());
   }
   else
   {
      // Non-TTY mode: Print progress updates at intervals (every 10%) using
      // Logger
      static int lastPrintedPercentage = -1;
      if(percentage % 10 == 0 && percentage != lastPrintedPercentage)
      {
         CFLOG_INFO(message + " - " + std::to_string(percentage) + "%", false);
         lastPrintedPercentage = percentage;
      }
   }
}

void Spinner::renderSpinnerOnly(const std::string& message)
{
   if(m_bIsTTYMode)
   {
      // TTY mode: Show animated spinner WITHOUT progress bar
      std::ostringstream oss;

      oss << "[" << getSpinnerChar() << "] " << message;

      // Pad with spaces to ensure we overwrite any previous longer content
      std::string line = oss.str();
      size_t lineLen = static_cast<size_t>(m_lineWidth);
      if(line.length() < lineLen)
      {
         line.append(lineLen - line.length(), ' ');
      }

      // Build complete output string with cursor movement
      std::ostringstream output;

      // Add cursor positioning prefix
      if(m_bIsTTYMode && m_lineOffset > 0)
      {
         output << "\033[s";                       // Save cursor position
         output << "\033[" << m_lineOffset << "A"; // Move up N lines
         output << "\r";                           // Move to beginning of line
      }
      else
      {
         output << "\r"; // Just carriage return for line 0
      }

      // Add the line content
      output << line;

      // Add cursor positioning suffix
      if(m_lineOffset > 0)
      {
         output << "\033[u"; // Restore cursor position
      }
      else
      {
         output << "\r"; // For offset 0, return to beginning of line
      }

      // Write to console via KLogger (atomic operation with flush)
      KL::Logger::get_instance().consoleOverwriteLine(output.str());
   }
   else
   {
      // Non-TTY mode: Print message periodically (every few seconds)
      static auto lastPrintTime = std::chrono::steady_clock::now();
      auto now = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastPrintTime).count();

      if(elapsed >= 2)
      { // Print every 2 seconds in non-TTY mode
         CFLOG_INFO(message, false);
         lastPrintTime = now;
      }
   }
}

char Spinner::getSpinnerChar()
{
   return spinnerChars[m_spinnerIndex];
}

bool Spinner::isTTY()
{
#ifdef TOOLS_TARGET_WINDOWS
   return _isatty(_fileno(stdout)) != 0;
#else
   return isatty(fileno(stdout)) != 0;
#endif
}

int Spinner::getTerminalWidth()
{
#ifdef TOOLS_TARGET_WINDOWS
   // Windows: Use GetConsoleScreenBufferInfo
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

   if(GetConsoleScreenBufferInfo(hConsole, &csbi))
   {
      int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
      // Ensure reasonable bounds
      if(width > 40 && width < 500)
      {
         return width;
      }
   }
#else
   // Linux/Unix: Use ioctl with TIOCGWINSZ
   struct winsize w;
   if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
   {
      if(w.ws_col > 40 && w.ws_col < 500)
      {
         return w.ws_col;
      }
   }
#endif

   // Fallback to default if detection fails
   return DEFAULT_LINE_WIDTH;
}

} // namespace CLI
} // namespace QC

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <chrono>
#include <iostream>
#include <string>

namespace QC {
namespace CLI {

class Spinner
{
public:
   Spinner();
   ~Spinner();

   // Update with percentage and custom message
   void update(int percentage, const std::string& message);

   // Start the spinner
   void start(const std::string& message = "Processing");

   // Stop and clear the spinner
   void stop();

   // Finish with a success message
   void finish(const std::string& message = "Done");

   // Update spinner only (no progress bar)
   void updateSpinnerOnly(const std::string& message);

   // Set line offset for multi-line spinners (0 = current line, 1 = next line,
   // etc.)
   void setLineOffset(int offset);

private:
   void clearLine();
   void render(int percentage, const std::string& message);
   void renderSpinnerOnly(const std::string& message);
   char getSpinnerChar();
   bool isTTY();
   int getTerminalWidth();

   static const char spinnerChars[];
   static const int DEFAULT_LINE_WIDTH = 120; // Fallback if terminal width cannot be detected

   int m_spinnerIndex;
   bool m_bIsActive;
   bool m_bIsTTYMode;
   int m_lineWidth;
   std::string m_currentMessage;
   int m_lastPercentage;
   int m_lineOffset; // 0 = current line, 1 = next line, etc.
   std::chrono::steady_clock::time_point m_lastUpdateTime;
};

} // namespace CLI
} // namespace QC

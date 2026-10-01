// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <string>

namespace Util {

/**
 * @brief Result of process execution
 */
struct ProcessResult
{
   int exitCode;
   std::string output;      // stdout
   std::string errorOutput; // stderr
   bool timedOut;
};

/**
 * @brief Execute external process with timeout support
 *
 * Provides cross-platform process execution with output capture
 * and timeout handling.
 *
 * @param command Command to execute
 * @param timeoutSeconds Timeout in seconds (default: 5)
 * @return ProcessResult with exit code, output, and timeout flag
 */
ProcessResult runExecutable(const std::string& command, int timeoutSeconds = 5);

} // namespace Util



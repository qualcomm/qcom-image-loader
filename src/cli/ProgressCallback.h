// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include <functional>
#include <string>

namespace QC {
namespace CLI {

// Progress callback function signature
// Parameters: percentage (0-100), operation name, current details
typedef std::function<void(int percentage, const std::string& operation, const std::string& details)> ProgressCallback;

// Progress callback manager for integrating with QFS operations
class ProgressCallbackManager
{
public:
   static ProgressCallbackManager& getInstance();

   // Set the active progress callback
   void setCallback(const ProgressCallback& callback);

   // Clear the active callback
   void clearCallback();

   // Report progress (called by QFS operations)
   void reportProgress(int percentage, const std::string& operation, const std::string& details = "");

   // Check if callback is active
   bool hasCallback() const;

private:
   ProgressCallbackManager() = default;
   ProgressCallback m_callback;
   bool m_hasCallback = false;
};

// Convenience macros for progress reporting
#define REPORT_PROGRESS(percentage, operation, details)                                                                \
   ProgressCallbackManager::getInstance().reportProgress(percentage, operation, details)

#define REPORT_PROGRESS_SIMPLE(percentage, operation)                                                                  \
   ProgressCallbackManager::getInstance().reportProgress(percentage, operation, "")

} // namespace CLI
} // namespace QC

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "ProgressCallback.h"

namespace QC {
namespace CLI {

ProgressCallbackManager& ProgressCallbackManager::getInstance()
{
   static ProgressCallbackManager instance;
   return instance;
}

void ProgressCallbackManager::setCallback(const ProgressCallback& callback)
{
   m_callback = callback;
   m_hasCallback = true;
}

void ProgressCallbackManager::clearCallback()
{
   m_callback = nullptr;
   m_hasCallback = false;
}

void ProgressCallbackManager::reportProgress(int percentage, const std::string& operation, const std::string& details)
{
   if(m_hasCallback && m_callback)
   {
      m_callback(percentage, operation, details);
   }
}

bool ProgressCallbackManager::hasCallback() const
{
   return m_hasCallback;
}

} // namespace CLI
} // namespace QC

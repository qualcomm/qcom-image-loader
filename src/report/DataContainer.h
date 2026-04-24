// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "report/Fwd.h"

#include <atomic>
#include <string>

namespace Device {
namespace Report {

// ----------------------------------------------------------------------------
// DataContainer
//
/// Contains interface for reporting the container size
// ----------------------------------------------------------------------------
class DataContainer
{
public:
   DataContainer();
   DataContainer(const std::string& containerName, int32_t reportThreshold);
   virtual ~DataContainer();

   bool checkStatus() const;

   std::string getName() const;

   inline void incrementSize()
   {
      m_containerSize++;
   }
   inline void decrementSize()
   {
      m_containerSize--;
   }
   inline void setSize(int32_t continerSize)
   {
      m_containerSize = continerSize;
   }
   inline int32_t getSize() const
   {
      return m_containerSize;
   }

   inline int32_t getThreshold() const
   {
      return m_reportThreshold;
   }


private:
   std::string m_containerName;                   ///< Name to use when reporting
   volatile std::atomic<int32_t> m_containerSize; ///< Current size of the container
   int32_t m_reportThreshold;                     ///< When to issue warning about size
};

} // namespace Report
} // namespace Device

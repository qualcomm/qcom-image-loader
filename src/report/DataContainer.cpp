// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "report/DataContainer.h"

#include "device/Manager.h"
#include "report/Fwd.h"
#include "report/StatusManager.h"

namespace Device {
namespace Report {

// ----------------------------------------------------------------------------
// DataContainer
//
// ----------------------------------------------------------------------------
DataContainer::DataContainer()
: m_containerName()
, m_containerSize(0)
, m_reportThreshold(0)
{
}

// ----------------------------------------------------------------------------
// DataContainer
//
// ----------------------------------------------------------------------------
DataContainer::DataContainer(const std::string& containerName, int32_t reportThreshold)
: m_containerName(containerName)
, m_containerSize(0)
, m_reportThreshold(reportThreshold)
{
}

// ----------------------------------------------------------------------------
// ~DataContainer
//
// ----------------------------------------------------------------------------
DataContainer::~DataContainer()
{
}

// ----------------------------------------------------------------------------
// checkStatus
//
/// Logs when the size of the data container exceeds the expected threshold
// ----------------------------------------------------------------------------
bool DataContainer::checkStatus() const
{
   if(m_containerSize >= m_reportThreshold)
   {
      FLOG_WARNING(("WARNING!! Data Container exceeding threshold: " + m_containerName + " - Size = " +
                    std::to_string(int32_t(m_containerSize)) + " Threshold = " + std::to_string(m_reportThreshold))
                      .c_str());
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------
// getName
//
/// @returns The name by which to track this data container
// ----------------------------------------------------------------------------
std::string DataContainer::getName() const
{
   return m_containerName;
}

} // namespace Report
} // namespace Device

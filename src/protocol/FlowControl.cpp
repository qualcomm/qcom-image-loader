// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "protocol/FlowControl.h"

#include "device/Exception.h"


namespace Device {
namespace Protocol {
// ----------------------------------------------------------------------------
// FlowControl
//
// ----------------------------------------------------------------------------
FlowControl::FlowControl()
: m_bWatermarkSet(false)
, m_rxLowWatermark(0)
, m_rxHighWatermark(0)
, m_rxDneWatermark(0)
, m_count(0)
, m_flowControlDelegate(NULL)
, m_flowControlStatus(FlowControlStatus::FLOW_CONTROL_OFF)
, m_flowControlLevel(FlowControlLevel::FLOW_CONTROL_LOW)
{
}

// ----------------------------------------------------------------------------
// ~FlowControl
//
// ----------------------------------------------------------------------------
FlowControl::~FlowControl()
{
}

// ----------------------------------------------------------------------------
// setFlowControlWatermarks
//
/// Sets the flow control watermark level
// ----------------------------------------------------------------------------
void FlowControl::setFlowControlWatermarks(uint32_t lowWatermark, uint32_t highWatermark, uint32_t dneWatermark)
{
   m_rxLowWatermark = lowWatermark;
   m_rxHighWatermark = highWatermark;
   m_rxDneWatermark = dneWatermark;
   m_bWatermarkSet = true;
}

// ----------------------------------------------------------------------------
// setFlowControlCallback
//
/// Sets the callback function to handle flow control status change
// ----------------------------------------------------------------------------
void FlowControl::setFlowControlCallback(const FlowControlDelegate& flowControlDelegate)
{
   m_flowControlDelegate = flowControlDelegate;
}

// ----------------------------------------------------------------------------
// isFlowControlled
//
/// Returns true if flow control is enabled
// ----------------------------------------------------------------------------
bool FlowControl::isFlowControlled() const
{
   return (FLOW_CONTROL_ON == m_flowControlStatus);
}

// ----------------------------------------------------------------------------
// updateFlowControlCount
//
/// Updates the current item count
// ----------------------------------------------------------------------------
void FlowControl::updateFlowControlCount(uint32_t count)
{
   FlowControlLevel newLevel;

   TOOLS_ASSERT_OR_THROW(m_bWatermarkSet, ToolException("Flow control watermarks not set: "));

   if(!m_bWatermarkSet)
   {
      return;
   }

   if(m_rxDneWatermark <= count)
   {
      newLevel = FlowControlLevel::FLOW_CONTROL_DNE;
   }
   else if(m_rxHighWatermark <= count)
   {
      newLevel = FlowControlLevel::FLOW_CONTROL_HIGH;
   }
   else if(m_rxLowWatermark <= count)
   {
      newLevel = FlowControlLevel::FLOW_CONTROL_MID;
   }
   else
   {
      newLevel = FlowControlLevel::FLOW_CONTROL_LOW;
   }

   if(m_flowControlLevel == newLevel)
   {
      return;
   }
   m_flowControlLevel = newLevel;

   FlowControlStatus flowControlStatus = m_flowControlStatus;
   if(FLOW_CONTROL_OFF == m_flowControlStatus && FLOW_CONTROL_HIGH <= m_flowControlLevel)
   {
      flowControlStatus = FLOW_CONTROL_ON;
   }
   else if(FLOW_CONTROL_ON == m_flowControlStatus && FLOW_CONTROL_LOW == m_flowControlLevel)
   {
      flowControlStatus = FLOW_CONTROL_OFF;
   }

   if(m_flowControlStatus != flowControlStatus)
   {
      m_flowControlStatus = flowControlStatus;
      if(m_flowControlDelegate != NULL)
      {
         m_flowControlDelegate(m_flowControlStatus);
      }
   }
}

} // namespace Protocol
} // namespace Device

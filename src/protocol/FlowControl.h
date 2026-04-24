// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Fwd.h"

#include <functional>

namespace Device {
namespace Protocol {
// ----------------------------------------------------------------------------
// FlowControl
//
/// Monitor watermark level in a packet queue
// ----------------------------------------------------------------------------
class FlowControl
{
   TOOLS_FORBID_COPY(FlowControl);

public:
   enum FlowControlLevel
   {
      FLOW_CONTROL_LOW,
      FLOW_CONTROL_MID,
      FLOW_CONTROL_HIGH,
      FLOW_CONTROL_DNE
   };

   enum FlowControlStatus
   {
      FLOW_CONTROL_OFF,
      FLOW_CONTROL_ON
   };

   typedef std::function<void(FlowControlStatus status)> FlowControlDelegate;

   FlowControl();
   ~FlowControl();

   void setFlowControlWatermarks(uint32_t lowWatermark, uint32_t highWatermark, uint32_t dneWatermark);
   void setFlowControlCallback(const FlowControlDelegate& flowControlDelegate);
   bool isFlowControlled() const;
   void updateFlowControlCount(uint32_t count);

private:
   bool m_bWatermarkSet;       ///< Indicating if watermarks have been set
   uint32_t m_rxLowWatermark;  ///< Low watermark
   uint32_t m_rxHighWatermark; ///< High watermark
   uint32_t m_rxDneWatermark;  ///< Do not exceed watermark
   uint32_t m_count;           ///< Current watermark

   FlowControlLevel m_flowControlLevel;   ///< Flow control level
   FlowControlStatus m_flowControlStatus; ///< Flow control status

   FlowControlDelegate m_flowControlDelegate; ///< Flow control delegate
};

} // namespace Protocol
} // namespace Device

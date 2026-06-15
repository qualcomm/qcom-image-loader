// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#ifndef QC_TIME_HELPERS_H
#define QC_TIME_HELPERS_H

#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>

namespace Util {

// ============================================================================
// Time Types (formerly TimeTypes.h)
// ============================================================================

// 100ns tick duration (matches System::Time precision)
// This ensures compatibility with legacy System::Time::DateTime tick-based
// arithmetic
using tick_duration = std::chrono::duration<int64_t, std::ratio<1, 10000000>>;

// Primary time types using 100ns tick precision
using duration = tick_duration;

// ============================================================================
// Formatting Functions
// ============================================================================

/**
 * Format a duration as "HH:MM:SS.mmm"
 * Compatible with System::Time::Span::toString() format
 */
inline std::string format_duration(const duration& d)
{
   // Convert to total milliseconds
   auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();

   // Extract components
   auto hours = total_ms / 3600000;
   total_ms %= 3600000;
   auto minutes = total_ms / 60000;
   total_ms %= 60000;
   auto seconds = total_ms / 1000;
   auto millis = total_ms % 1000;

   // Build string in format: HH:MM:SS.mmm
   std::ostringstream oss;
   oss << std::setfill('0') << std::setw(2) << hours << ':' << std::setw(2) << minutes << ':' << std::setw(2) << seconds
       << '.' << std::setw(3) << millis;

   return oss.str();
}

/**
 * Template overload for any duration type
 */
template <typename Rep, typename Period>
inline std::string format_duration(const std::chrono::duration<Rep, Period>& d)
{
   // Convert to our internal duration type
   auto converted = std::chrono::duration_cast<duration>(d);
   return format_duration(converted);
}

} // namespace Util

#endif // QC_TIME_HELPERS_H

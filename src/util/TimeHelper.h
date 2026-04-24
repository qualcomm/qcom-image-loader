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
using time_point = std::chrono::time_point<std::chrono::system_clock, tick_duration>;
using duration = tick_duration;

// Optional variants for nullable timestamps
using optional_time_point = std::optional<time_point>;
using optional_duration = std::optional<duration>;

// ============================================================================
// Formatting Functions
// ============================================================================

/**
 * Format a time_point as "YYYY-MM-DD HH:MM:SS.mmm" (UTC)
 * Compatible with System::Time::DateTime::toString() format
 */
inline std::string format_time_point(const time_point& tp)
{
   // Convert to time_t for standard formatting
   auto time_since_epoch = tp.time_since_epoch();
   auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
   std::time_t tt = seconds.count();

   // Get milliseconds component
   auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch - seconds);

   // Format using gmtime (UTC)
   std::tm tm_buf;
   std::memset(&tm_buf, 0, sizeof(tm_buf));
#ifdef TOOLS_TARGET_WINDOWS
   gmtime_s(&tm_buf, &tt);
#else
   gmtime_r(&tt, &tm_buf);
#endif

   // Build string in format: YYYY-MM-DD HH:MM:SS.mmm
   std::ostringstream oss;
   oss << std::setfill('0') << std::setw(4) << (tm_buf.tm_year + 1900) << '-' << std::setw(2) << (tm_buf.tm_mon + 1)
       << '-' << std::setw(2) << tm_buf.tm_mday << ' ' << std::setw(2) << tm_buf.tm_hour << ':' << std::setw(2)
       << tm_buf.tm_min << ':' << std::setw(2) << tm_buf.tm_sec << '.' << std::setw(3) << millis.count();

   return oss.str();
}

/**
 * Template overload for any time_point type
 */
template <typename Clock, typename Duration>
inline std::string format_time_point(const std::chrono::time_point<Clock, Duration>& tp)
{
   // Convert to our internal time_point type
   auto converted = std::chrono::time_point_cast<tick_duration>(tp);
   return format_time_point(converted);
}

/**
 * Format a time_point as "YYYY-MM-DD HH:MM:SS.mmm" (local timezone)
 * Compatible with System::Time::DateTime::toLocal().toString() format
 */
inline std::string format_time_point_local(const time_point& tp)
{
   // Convert to time_t for standard formatting
   auto time_since_epoch = tp.time_since_epoch();
   auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
   std::time_t tt = seconds.count();

   // Get milliseconds component
   auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch - seconds);

   // Format using localtime (local timezone)
   std::tm tm_buf;
   std::memset(&tm_buf, 0, sizeof(tm_buf));
#ifdef TOOLS_TARGET_WINDOWS
   localtime_s(&tm_buf, &tt);
#else
   localtime_r(&tt, &tm_buf);
#endif

   // Build string in format: YYYY-MM-DD HH:MM:SS.mmm
   std::ostringstream oss;
   oss << std::setfill('0') << std::setw(4) << (tm_buf.tm_year + 1900) << '-' << std::setw(2) << (tm_buf.tm_mon + 1)
       << '-' << std::setw(2) << tm_buf.tm_mday << ' ' << std::setw(2) << tm_buf.tm_hour << ':' << std::setw(2)
       << tm_buf.tm_min << ':' << std::setw(2) << tm_buf.tm_sec << '.' << std::setw(3) << millis.count();

   return oss.str();
}

/**
 * Template overload for any time_point type
 */
template <typename Clock, typename Duration>
inline std::string format_time_point_local(const std::chrono::time_point<Clock, Duration>& tp)
{
   // Convert to our internal time_point type
   auto converted = std::chrono::time_point_cast<tick_duration>(tp);
   return format_time_point_local(converted);
}

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

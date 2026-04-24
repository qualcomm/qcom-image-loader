// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef> // for size_t
#include <vector>


namespace Util {

#ifdef TOOLS_TARGET_WINDOWS
using std::abs;
#else
template <typename _Type>
inline _Type abs(_Type value)
{
   return std::abs(value);
}
template <>
inline double abs(double value)
{
   return std::fabs(value);
}
template <>
inline float abs(float value)
{
   return std::fabs(value);
}
template <>
inline long double abs(long double value)
{
   return std::fabs(value);
}
#endif
using std::log10;
using std::max;
using std::min;
using std::sin;

// --------------------------------------------------------------------------
// percent
//
/// @return percentage of numenator in denumenator.
// --------------------------------------------------------------------------
template <typename _Type1, typename _Type2>
inline double percent(const _Type1& num, const _Type2& denum)
{
   TOOLS_ASSERT_OR_THROW(
      !(num != 0 && 0 == denum),
      ToolException("Invalid combination of num/denum in "
                    "percent.")
   );

   return (denum > 0 ? num * 100.0 / denum : 0.0);
}

} // namespace Util

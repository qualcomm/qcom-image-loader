// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include <string>

template <typename T>
inline std::string toString(const T& data)
{
   return std::to_string(data);
}

template <>
inline std::string toString(const std::string& stringData)
{
   return stringData;
}

namespace Device {
} // namespace Device
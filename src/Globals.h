// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "Definitions.h"
#include "platform/Exception.h"

#define LIB_CATCH TOOLS_CATCH(e, result.errorCode = QC::ErrorCode::DEVICE_UNKNOWN_ERROR; result.errorString = e.what();)

#define QFS_PRINT_FUNCTION PrintMessage(std::string("Entering ") + TOOLS_FUNCTION_NAME);
#define QFS_CHECK_INITIALIZE                                                                                           \
   QFS_PRINT_FUNCTION                                                                                                  \
   if(!g_handlers[m_deviceHandle]->isInitialized())                                                                    \
   {                                                                                                                   \
      QC::ErrorType errorType;                                                                                         \
      errorType.errorCode = QC::ErrorCode::DEVICE_SERVICE_NOT_INITIALIZED;                                             \
      errorType.errorString = "Service not initialized";                                                               \
      return errorType;                                                                                                \
   }

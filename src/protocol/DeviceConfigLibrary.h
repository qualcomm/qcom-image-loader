// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "protocol/DeviceConfigLibraryConstants.h"

#include <memory>
#include <mutex>
#include <string>
#include <vector>


using namespace std;

class DbFieldAccess;
class DbInterface;

class DeviceConfigLibrary
{
public:
   virtual ~DeviceConfigLibrary();

   static DeviceConfigLibrary& getInstance();

   // Configure path to sqlite3 db
   // (Not necessary if db is in the default location)
   // Param:
   //   dbPath - absolute path to sqlite3 .db file
   virtual void setDbPath(string dbPath);


   // Open the DB
   // Returns enum indicating success or failure reason
   // Param:
   //   errorString - in case of failure, populated with text describing what
   //   went wrong
   virtual DCL_RESULT_CODE startSession(string& errorString);


   // Close the DB
   // Returns enum indicating success or failure reason
   // Param:
   //   errorString - in case of failure, populated with text describing what
   //   went wrong
   virtual DCL_RESULT_CODE endSession(string& errorString);


   // Fetch Boot Config Description
   virtual DCL_RESULT_CODE
   getBootConfigDescription(string socHWVersion, int64_t configValue, string& description, string& errorString);


private:
   DeviceConfigLibrary();


   string m_dbPath;
   shared_ptr<DbInterface> m_dbInterface;
   mutex m_pmlMutex;
};

extern "C" {
DCL_EXPORT class DeviceConfigLibrary& getDeviceConfigLibraryInstance();
}


#ifdef _WIN_SOURCE
#include "windows.h"

typedef DeviceConfigLibrary&(WINAPIV* DCL_GET_INSTANCE_FN)();
static const LPCSTR DCL_GET_INSTANCE_STR = "getDeviceConfigLibraryInstance";

#endif


#ifdef _LINUX_SOURCE
#include <dlfcn.h>

typedef DeviceConfigLibrary& (*DCL_GET_INSTANCE_FN)();
static const char* DCL_GET_INSTANCE_STR = "getDeviceConfigLibraryInstance";

#endif

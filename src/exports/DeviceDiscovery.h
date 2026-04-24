// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include <cstdint>
#include <regex>

#pragma once
#include "Definitions.h"

namespace QC {
#ifdef TOOLS_TARGET_WINDOWS
typedef void(_stdcall* DeviceConnectedCallback)(const DeviceInfo&);
typedef void(_stdcall* ProtocolConnectedCallback)(const DeviceInfo&, const ProtocolInfo&);
typedef void(_stdcall* ProtocolStateCallback)(int64_t, ProtocolState);
typedef void(_stdcall* MessageCallback)(
   MessageLevel::type level,
   const std::string& location,
   const std::string& title,
   const std::string& description
);
typedef void(_stdcall* DeviceModeCallback)(int64_t deviceHandle, const DeviceMode newMode);

typedef void(_stdcall* ServiceStateChangeCallback)(const std::string& serviceName, int64_t deviceHandle);
typedef void(_stdcall* ServiceEventCallback)(
   const std::string& serviceName,
   int64_t eventId,
   const std::string& eventDescription
);

#elif defined TOOLS_TARGET_LINUX
typedef void (*DeviceConnectedCallback)(const DeviceInfo&);
typedef void (*ProtocolConnectedCallback)(const DeviceInfo&, const ProtocolInfo&);
typedef void (*ProtocolStateCallback)(int64_t, ProtocolState);
typedef void (*MessageCallback)(
   MessageLevel::type level,
   const std::string& location,
   const std::string& title,
   const std::string& description
);
typedef void (*DeviceModeCallback)(int64_t deviceHandle, const DeviceMode newMode);

typedef void (*ServiceStateChangeCallback)(const std::string& serviceName, int64_t deviceHandle);
typedef void (*ServiceEventCallback)(
   const std::string& serviceName,
   int64_t eventId,
   const std::string& eventDescription
);
#endif
class DeviceDiscovery
{
   DeviceDiscovery();
   virtual ~DeviceDiscovery();

public:
   /// <summary>
   /// Get the dll version as per the version file
   ///
   /// </summary>
   ///
   /// <returns>DLL version</returns>
   ///
   static std::string getDLLVersion();
   /// <summary>
   /// Start Monitoring the device discovery.
   /// This is mandatory call required to initialize the Device discovery
   /// resources and drivers.
   /// </summary>
   ///
   /// <returns>No return value.</returns>
   static void startMonitoring();

   /// <summary>
   /// Stop monitoring the device discovery.
   /// This is mandatory call during exit to clean up the device discovery
   /// resources.
   /// </summary>
   ///
   /// <returns>No return value.</returns>
   static void stopMonitoring();

   // API Calls

   /// <summary>
   /// Get the list of devices connected.
   ///
   /// </summary>
   ///
   /// <returns>list of devices connected.</returns>
   static std::list<DeviceInfo> getDeviceList();

   /// <summary>
   /// Get the list of protocols connected for a particular device.
   /// </summary>
   /// <param name="deviceHandle">The device handle to query the list of
   /// protocols available for this device.</param> <returns>The list of
   /// protocols available for this device.</returns>
   static std::list<ProtocolInfo> getProtocolList(const int64_t deviceHandle);

   /// <summary>
   /// Get a bit mask of DeviceDetail for particular device.
   /// </summary>
   /// <param name="deviceHandle">The device handle to query.</param>
   // <returns>number bit mask of known device details.</returns>
   static uint64_t getDeviceDetails(const int64_t deviceHandle);

   // Callbacks

   /// <summary>
   /// Register the message callback for receiving the library messages example
   /// errors.
   /// </summary>
   /// <param name="const MessageCallback& callback">Callback
   /// DeviceConnectedCallback message callback </param> <returns>No return
   /// value.</returns>
   ///
   static void setMessageCallback(const MessageCallback& callback);

   /// <summary>
   /// Register function to get the device connected  callback event.
   /// </summary>
   /// <param name="const DeviceConnectedCallback& callback">Callback
   /// DeviceConnectedCallback function ptr</param> <returns>No return
   /// value.</returns>
   ///
   static void setDeviceConnectedCallback(const DeviceConnectedCallback& callback);

   /// <summary>
   /// Register function to get the device disconnected callback event.
   /// </summary>
   /// <param name="const DeviceConnectedCallback& callback">Callback
   /// DeviceConnectedCallback callback function ptr</param> <returns>No return
   /// value.</returns>
   ///
   static void setDeviceDisconnectedCallback(const DeviceConnectedCallback& callback);

   /// <summary>
   /// Register function to get the device mode change callback event.
   /// </summary>
   /// <param name="const DeviceModeCallback& callback">Callback
   /// DeviceModeCallback  function ptr</param> <returns>No return
   /// value.</returns>
   ///
   static void setDeviceModeChangeCallback(const DeviceModeCallback& callback);

   /// <summary>
   /// Register function to get the protocol connected callback event.
   /// </summary>
   /// <param name="const ProtocolConnectedCallback& callback">Callback
   /// ProtocolConnectedCallback function ptr</param> <returns>No return
   /// value.</returns>
   ///
   static void setProtocolAddedCallback(const ProtocolConnectedCallback& callback);

   /// <summary>
   /// Register function to get the protocol removed callback event.
   /// </summary>
   /// <param name="const ProtocolConnectedCallback& callback">Callback
   /// ProtocolConnectedCallback function ptr</param> <returns>No return
   /// value.</returns>
   ///
   static void setProtocolRemovedCallback(const ProtocolConnectedCallback& callback);

   /// <summary>
   /// Register function to get the protocol state change callback event.
   /// </summary>
   /// <param name="const ProtocolStateCallback& callback">Callback
   /// ProtocolStateCallback function ptr</param> <returns>No return
   /// value.</returns>
   ///
   static void setProtocolStateChangeCallback(const ProtocolStateCallback& callback);

   /// <summary>
   /// Register function to get the service event.
   /// </summary>
   /// <param name="const ProtocolStateCallback& callback">Callback
   /// ProtocolStateCallback function ptr</param> <returns>No return
   /// value.</returns>
   ///
   static void setServiceEventCallback(const ServiceEventCallback& callback);
};
} // namespace QC

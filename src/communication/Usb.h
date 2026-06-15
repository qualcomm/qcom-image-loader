// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "communication/CommonIO.h"
#include "util/ThreadHelper.h"

#include <chrono>
#include <mutex>
#include <optional>
namespace Device {
namespace Communication {

class RxWorker;


// ----------------------------------------------------------------------------
// Usb
//
/// Contains the implementation for communicating over USB
// ----------------------------------------------------------------------------
class Usb
: public CommonIo
, public std::enable_shared_from_this<Usb>
{
   TOOLS_FORBID_COPY(Usb);

public:
   // Factory method to create Usb instances with std::shared_ptr
   static std::shared_ptr<Usb> create(
      const std::string& description,
      const std::string& identifier,
      const std::string& address,
      const std::string& serialNumber
   );

   Usb(
      const std::string& description,
      const std::string& identifier,
      const std::string& address,
      const std::string& serialNumber
   );
   virtual ~Usb();

   std::string getSerialNumber() const;


   virtual std::string getDescription() const;
   virtual std::string getIdentifier() const;
   virtual void setIdentifier(const std::string& identifier);
   virtual bool isOpen() const;
   virtual void open();
   virtual void close();
   virtual void reset();
   virtual uint64_t sendSync(const Device::SharedByteBufferPtr& pBuffer);
   virtual SendAsyncStatus sendAsync(const Device::SharedByteBufferPtr& pBuffer);
   virtual void registerReceiveAsync(const ReceiveDelegate& callback);

   // virtual LoggerPtr getLogger() const;

   virtual void enableLegacyTimeoutConfig();

private:
   typedef int64_t Handle;
   static const Handle INVALID_HANDLE;

   friend class RxWorker;

   std::string m_description;  ///< for UI display
   std::string m_identifier;   ///< for opening device
   std::string m_serialNumber; ///< Serial Number

   std::shared_ptr<RxWorker> m_pRxWork;
   std::shared_ptr<Util::StdThreadWrapper> m_pRxThread;

   volatile Handle m_handle; ///< USB interface
   std::recursive_mutex m_closeHandleMutex;

   ReceiveDelegate m_asyncCallback; ///< Callback for async data

   std::recursive_mutex m_mutex;
   std::string m_stream;
   volatile bool m_bClosing;

   int64_t m_baudRate;
   bool m_bLegacyTimeoutConfig;
   std::optional<std::chrono::system_clock::time_point> m_initialSendFailTime;

   void closeHandle();
};

} // namespace Communication
} // namespace Device

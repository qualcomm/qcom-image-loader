// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "communication/Usb.h"

#include "device/Buffer.h"
#include "device/Exception.h"
#include "device/Manager.h"
#include "qdpublic.h"
#include "report/Thread.h"
#include "util/FunctionHelper.h"
#include "util/MemoryHelper.h"
#include "util/StringHelper.h"
#include "util/ThisThread.h"

#ifdef TOOLS_TARGET_WINDOWS
#define handle_cast reinterpret_cast
#else
#define handle_cast static_cast
#endif

namespace Device {
namespace Communication {

static const size_t BUFFER_READ_SIZE = 128 * 1024;
static const size_t BUFFER_PROCESS_SIZE = 16384;
const Usb::Handle Usb::INVALID_HANDLE = -1;
static const size_t RECONNECT_RETRY_COUNT = 8;
static const uint64_t DEFAULT_BAUD_RATE = 38400;
static const uint16_t MAX_SEND_RETRY = 5;

// ----------------------------------------------------------------------------
// RxWorker
//
/// Handles incoming data from the communication layer
// ----------------------------------------------------------------------------
class RxWorker : public Report::Thread::Work
{
   TOOLS_FORBID_COPY(RxWorker);

public:
   RxWorker(const UsbPtr& pUsb)
   : Report::Thread::Work(pUsb->getDescription() + " USB Rx Thread")
   , m_pUsb(pUsb)
   , m_receiveCallback()
   , m_callbackMutex()
   , m_bLogBadBufferRead(false)
   {
   }

   void setAsyncCallback(const CommonIo::ReceiveDelegate& callback)
   {
      std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
      if(m_receiveCallback && callback && !Util::isSameFunction(m_receiveCallback, callback))
      {
         FLOG_WARNING("Callback function switch: " + m_pUsb->getDescription());
      }
      m_receiveCallback = callback;
   }

   virtual void onRun()
   {
      BufferPtr pUsbBuffer = Buffer::createBuffer(BUFFER_READ_SIZE);
      DWORD bytesRead = 0;
      bool bSucceed = false;

      while(!isStopSignaled() && Usb::INVALID_HANDLE != m_pUsb->m_handle)
      {
         try
         {
            bytesRead = 0;
            bSucceed = false;

            try
            {
               bSucceed = !!QcDevice::ReadFromDevice(
                  handle_cast<HANDLE>(m_pUsb->m_handle),
                  pUsbBuffer->begin(),
                  BUFFER_READ_SIZE,
                  &bytesRead
               );
            }
            TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e))

            if(!bSucceed)
            {
               m_pUsb->closeHandle();
               break;
            }
            else if(0 != bytesRead)
            {
               if(BUFFER_READ_SIZE >= bytesRead)
               {
                  pUsbBuffer->resize(bytesRead);

                  // Divide the buffer into max 16k chunks to keep our buffer
                  // pool consistent Use mutex to prevent callback function
                  // switching between sub-packets
                  size_t bufferOffset = 0;
                  bool bNewPacket = true;
                  std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
                  for(; bufferOffset < bytesRead; bufferOffset += BUFFER_PROCESS_SIZE)
                  {
                     callbackData(pUsbBuffer, bufferOffset, bytesRead, bNewPacket);
                     bNewPacket = false;
                  }
               }
               else
               {
                  if(!m_bLogBadBufferRead)
                  {
                     FLOG_ERROR("ReadFromDevice() invalid read size: " + std::to_string(bytesRead));
                     m_bLogBadBufferRead = true;
                  }
               }
            }
         }
         TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e))
      }
   }


private:
   void
   callbackData(const Device::SharedByteBufferPtr& pUsbBuffer, size_t bufferOffset, size_t bytesRead, bool bNewPacket)
   {
      Device::SharedByteBufferPtr pBuffer = Buffer::
         createCopy(pUsbBuffer->begin() + bufferOffset, std::min(BUFFER_PROCESS_SIZE, bytesRead - bufferOffset));
      if(bNewPacket)
      {
         PTRACE_LOG("# Receive from USB handle " + std::to_string(m_pUsb->m_handle) +
            " : " + m_pUsb->getDescription()
               + " Size: " + std::to_string(bytesRead) + " data: " + Util::bufferToHex(pBuffer));
      }
      else
      {
          PTRACE_LOG(
            "  Receive from USB handle " + std::to_string(m_pUsb->m_handle) +
            " : " + m_pUsb->getDescription()
               + " Size: " + std::to_string(bytesRead) + " data: " + Util::bufferToHex(pBuffer));
      }

      if(m_pUsb.isAlive() && m_receiveCallback)
      {
         m_receiveCallback(pBuffer);
      }
   }

   Util::CheckedPointer<Usb> m_pUsb;
   std::recursive_mutex m_callbackMutex;
   CommonIo::ReceiveDelegate m_receiveCallback;
   bool m_bLogBadBufferRead;
};


// ----------------------------------------------------------------------------
// Usb
//
// ----------------------------------------------------------------------------

// Factory method to create Usb with std::shared_ptr
std::shared_ptr<Usb> Usb::create(
   const std::string& description,
   const std::string& identifier,
   const std::string& address,
   const std::string& serialNumber
)
{
   return std::make_shared<Usb>(description, identifier, address, serialNumber);
}

Usb::Usb(
   const std::string& description,
   const std::string& identifier,
   const std::string& address,
   const std::string& serialNumber
)
: CommonIo()
, m_description(description)
, m_identifier(identifier)
, m_serialNumber(serialNumber)
, m_pRxWork()
, m_pRxThread()
, m_handle(INVALID_HANDLE)
, m_closeHandleMutex()
, m_asyncCallback()
, m_mutex()
, m_stream()
, m_bClosing(false)
, m_baudRate(DEFAULT_BAUD_RATE)
, m_bLegacyTimeoutConfig(false)
, m_initialSendFailTime(std::nullopt)
{
   (void)address;
}

// ----------------------------------------------------------------------------
// ~Device
//
// ----------------------------------------------------------------------------
Usb::~Usb()
{
   TOOLS_IGNORE_EXCEPTIONS(close());
}

// ----------------------------------------------------------------------------
// getIdentifier
//
/// @returns The identifier of the interface
// ----------------------------------------------------------------------------
std::string Usb::getIdentifier() const
{
   return m_identifier;
}

// ----------------------------------------------------------------------------
// setIdentifier
//
// ----------------------------------------------------------------------------
void Usb::setIdentifier(const std::string& identifier)
{
   m_identifier = identifier;
}

// ----------------------------------------------------------------------------
// getSerialNumber
//
/// @returns The serial number of the interface
// ----------------------------------------------------------------------------
std::string Usb::getSerialNumber() const
{
   return m_serialNumber;
}

// ----------------------------------------------------------------------------
// getDescription
//
/// @returns The description of the interface
// ----------------------------------------------------------------------------
std::string Usb::getDescription() const
{
   return m_description;
}

// ----------------------------------------------------------------------------
// isOpen
//
/// @returns True when the connection has been opened
// ----------------------------------------------------------------------------
bool Usb::isOpen() const
{
   return INVALID_HANDLE != m_handle;
}

// ----------------------------------------------------------------------------
// open
//
/// Opens the connection from QcDevice
// ----------------------------------------------------------------------------
void Usb::open()
{
#ifndef TOOLS_TARGET_OSX
#ifndef DRIVER_NOT_AVAILABLE
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(isOpen())
      {
         return;
      }

      TOOLS_ASSERT_OR_RETURN(INVALID_HANDLE == m_handle, TOOLS_VOID);

      m_bClosing = false;

      FLOG_INFO("Usb::open(): " + getDescription() + ", baudRate = " + std::to_string(m_baudRate));

#ifdef TOOLS_TARGET_WINDOWS
      m_handle = handle_cast<Handle>(QcDevice::OpenDevice(
         (void*)Util::toWString(m_description + m_stream).c_str(),
         static_cast<DWORD>(m_baudRate),
         m_bLegacyTimeoutConfig
      ));
#else
      m_handle = handle_cast<Handle>(QcDevice::OpenDevice((void*)(m_description + m_stream).c_str()));
#endif

      for(size_t i = 0; !m_bClosing && INVALID_HANDLE == m_handle && RECONNECT_RETRY_COUNT > i; ++i)
      {
         // FLOG_WARNING(
         //    ("Failed to open, retrying " + getDescription() + m_stream)
         //    );
         Util::ThisThread::sleep(std::chrono::milliseconds(250));
#ifdef TOOLS_TARGET_WINDOWS
         m_handle = handle_cast<Handle>(QcDevice::OpenDevice(
            (void*)Util::toWString(m_description + m_stream).c_str(),
            static_cast<DWORD>(m_baudRate),
            m_bLegacyTimeoutConfig
         ));
#else
         m_handle = handle_cast<Handle>(QcDevice::OpenDevice((void*)(m_description + m_stream).c_str()));
#endif
      }

      if(INVALID_HANDLE != m_handle)
      {
         m_pRxWork = std::make_shared<RxWorker>(shared_from_this());
         m_pRxThread = std::make_shared<Util::StdThreadWrapper>(m_pRxWork);
         m_pRxWork->setAsyncCallback(m_asyncCallback);
         m_pRxThread->start();

         FLOG_INFO("Opened USB handle " + std::to_string(m_handle) + " : " + getDescription() + m_stream);
         return;
      }
   }

   TOOLS_ASSERT(INVALID_HANDLE == m_handle);
   ::Device::Manager::getInstance()->reportCriticalEvent(::Device::EVENT_COMMUNICATION_USB_OPEN_FAILURE, m_identifier);
   TOOLS_THROW(ToolException("Could not open connection: " + m_identifier));
#endif
#endif
}

// ----------------------------------------------------------------------------
// close
//
/// Closes the connection
// ----------------------------------------------------------------------------
void Usb::close()
{
#ifndef TOOLS_TARGET_OSX
#ifndef DRIVER_NOT_AVAILABLE
   m_bClosing = true;

   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   if(!isOpen())
   {
      if(m_pRxThread != nullptr)
      {
         if(Util::StdThreadWrapper::State::RUNNING == m_pRxThread->getState())
         {
            m_pRxThread->stop();
            m_pRxThread->waitForStop();
         }

         m_pRxThread = NULL;
         m_pRxWork = NULL;
         FLOG_INFO("Usb::close(), m_pRxWork = NULL");
      }
      return;
   }

   m_pRxThread->stop();
   closeHandle();
   m_pRxThread->waitForStop();
#endif
#endif
   m_pRxThread = NULL;
   m_pRxWork = NULL;

   m_initialSendFailTime = std::nullopt;
}

// ----------------------------------------------------------------------------
// closeHandle
//
/// Closes port handle
// ----------------------------------------------------------------------------
void Usb::closeHandle()
{
   std::lock_guard<std::recursive_mutex> lock(m_closeHandleMutex);
   if(m_handle == INVALID_HANDLE)
   {
      return;
   }

   FLOG_INFO("Close USB handle " + std::to_string(m_handle) + " : " + getDescription() + m_stream);

   HANDLE ioHandle = handle_cast<HANDLE>(m_handle);
   m_handle = INVALID_HANDLE; // Set to INVALID befor eclosing CloseDevice() so
                              // the RxThread will break
#ifdef TOOLS_TARGET_WINDOWS
   ::CancelIoEx(ioHandle,
                NULL); // Should not be needed, remove once drivers are fixed
   FLOG_INFO(
      "Cancel IO USB handle " + std::to_string(reinterpret_cast<uintptr_t>(ioHandle)) + " : " + getDescription() +
      m_stream
   );
#endif
#ifndef DRIVER_NOT_AVAILABLE
   QcDevice::CloseDevice(ioHandle);
#endif
#ifdef TOOLS_TARGET_WINDOWS
   FLOG_INFO(
      "Finished close USB handle " + std::to_string(reinterpret_cast<uintptr_t>(ioHandle)) + " : " + getDescription() +
      m_stream
   );
#elif defined TOOLS_TARGET_LINUX
   FLOG_INFO(
      "Finished close USB handle " + std::to_string(static_cast<uintptr_t>(ioHandle)) + " : " + getDescription() +
      m_stream
   );
#endif
}

// ----------------------------------------------------------------------------
// reset
//
/// Closes and re-opens the connection
// ----------------------------------------------------------------------------
void Usb::reset()
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   TOOLS_ASSERT_OR_RETURN(INVALID_HANDLE != m_handle, TOOLS_VOID);


   FLOG_INFO("Reset USB handle " + std::to_string(m_handle) + " : " + getDescription() + m_stream);

   close();
   open();
}

// ----------------------------------------------------------------------------
// sendSync
//
/// Sends data and waits to ensure it's been sent out
/// @returns The number of bytes sent
// ----------------------------------------------------------------------------
uint64_t Usb::sendSync(const Device::SharedByteBufferPtr& pBuffer)
{
   uint64_t totalBytesSent = TOOLS_UI64(0);
   bool bSendTimeout = false;
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);

      if(INVALID_HANDLE == m_handle)
      {
         return TOOLS_UI64(0);
      }
#ifndef TOOLS_TARGET_OSX
#ifndef DRIVER_NOT_AVAILABLE
      DWORD bytesSent = 0;
      bool bSendStatus = true;

      PTRACE_LOG(
         "Send to USB handle " + std::to_string(m_handle) + " : "
            + getDescription() + " size: " + std::to_string(pBuffer->size()) + " data: " + Util::bufferToHex(pBuffer)
      );
      // FLOG_DATA(
      //    "Send to USB handle " + std::to_string(m_handle) + " : "
      //       + getDescription() + " size: " + std::to_string(pBuffer->size()),
      //    pBuffer,
      //    DataType::Tx
      // );

      uint16_t numAttempts = TOOLS_UI64(0);
      while(bSendStatus && totalBytesSent < pBuffer->size())
      {
         bSendStatus = !!QcDevice::SendToDevice(
            handle_cast<HANDLE>(m_handle),
            pBuffer->begin() + totalBytesSent,
            static_cast<DWORD>(pBuffer->size() - totalBytesSent),
            &bytesSent
         );

         if(bSendStatus && 0 != bytesSent)
         {
            totalBytesSent += bytesSent;
            m_initialSendFailTime = std::nullopt;
         }
         else
         {
            // Report error if USB send failure does not recover for more than
            // 10 seconds Note: It is normal to have USB send failures when port
            // is being closed
            if(m_initialSendFailTime.has_value())
            {
               auto elapsed = std::chrono::system_clock::now() - m_initialSendFailTime.value();
               if(elapsed > std::chrono::seconds(10))
               {
                  bSendTimeout = true;
                  m_initialSendFailTime = std::nullopt;
               }
            }
            else
            {
               m_initialSendFailTime = std::chrono::system_clock::now();
            }

            numAttempts++;
            FLOG_WARNING(
               "Send failed! USB handle " + std::to_string(m_handle) + " : " + "Bytes " + std::to_string(bytesSent) +
               " : " + "Status " + std::to_string(bSendStatus) + " : " + "Attempts " + std::to_string(numAttempts) +
               " : " + getDescription()
            );

            // Do not retry unless packet has been partially sent
            if((0 == bytesSent && totalBytesSent == pBuffer->size()) || numAttempts > MAX_SEND_RETRY)
            {
               break;
            }
         }
      }
#endif
#endif
   }
   if(bSendTimeout)
   {
      ::Device::Manager::getInstance()
         ->reportCriticalEvent(::Device::EVENT_COMMUNICATION_USB_SEND_FAILURE, m_identifier);
   }
   return totalBytesSent;
}

// ----------------------------------------------------------------------------
// sendAsync
//
/// Sends data via sendSync
/// @returns PENDING if it send successfully
// ----------------------------------------------------------------------------
Usb::SendAsyncStatus Usb::sendAsync(const Device::SharedByteBufferPtr& pBuffer)
{
   if(pBuffer->size() == sendSync(pBuffer))
   {
      return PENDING;
   }
   else
   {
      return FAIL;
   }
}

// ----------------------------------------------------------------------------
// registerReceiveAsync
//
/// Sets the function to call for received data
// ----------------------------------------------------------------------------
void Usb::registerReceiveAsync(const ReceiveDelegate& callback)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_asyncCallback = callback;
   if(m_pRxWork != nullptr)
   {
      m_pRxWork->setAsyncCallback(m_asyncCallback);
   }
}

// ----------------------------------------------------------------------------
// getLogger
//
/// @returns The logger for information relative to this connection
// ----------------------------------------------------------------------------
// LoggerPtr Usb::getLogger() const
// {
//    return m_pLogger;
// }

// ----------------------------------------------------------------------------
// enableLegacyTimeoutConfig
//
/// Use legacy COM port timeout configuration to maintain compitibility
// ----------------------------------------------------------------------------
void Usb::enableLegacyTimeoutConfig()
{
   m_bLegacyTimeoutConfig = true;
}

} // namespace Communication
} // namespace Device

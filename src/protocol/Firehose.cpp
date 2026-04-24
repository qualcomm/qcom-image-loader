// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "protocol/Firehose.h"

#include "communication/CommonIO.h"
#include "device/Buffer.h"
#include "device/ErrorMessage.h"
#include "device/Exception.h"
#include "device/Manager.h"
#include "protocol/firehose-loader/FirehoseLoader.h"
#include "util/ThisThread.h"

#include <chrono>
#include <regex>

namespace Device {
namespace Protocol {

static constexpr auto FIREHOSE_RX_WAIT_INTERVAL = std::chrono::milliseconds(100);
static constexpr auto FIREHOSE_RX_TIMEOUT = std::chrono::milliseconds(10000);

static const int32_t FIREHOSE_USB_RX_WATERMARK_LOW = 500;
static const int32_t FIREHOSE_USB_RX_WATERMARK_HIGH = 1000;
static const int32_t FIREHOSE_USB_RX_WATERMARK_DNE = 5000;

// ----------------------------------------------------------------------------
// FirehoseEvent
//
// ----------------------------------------------------------------------------
FirehoseEvent::FirehoseEvent(EventId eventId, const std::string& description)
: Util::Event()
, m_eventId(eventId)
, m_description(description)
{
}

// ----------------------------------------------------------------------------
// ~FirehoseEvent
//
// ----------------------------------------------------------------------------
FirehoseEvent::~FirehoseEvent()
{
}

// ----------------------------------------------------------------------------
// getEventId
//
// ----------------------------------------------------------------------------
FirehoseEvent::EventId FirehoseEvent::getEventId() const
{
   return m_eventId;
}

// ----------------------------------------------------------------------------
// getDescription
//
// ----------------------------------------------------------------------------
std::string FirehoseEvent::getDescription() const
{
   return m_description;
}

// ----------------------------------------------------------------------------
// Firehose
//
// ----------------------------------------------------------------------------
Firehose::Firehose(const Communication::CommonIoPtr& pIo, Device::DeviceHandle handle)
: Base(pIo)
, Report::DataContainer(pIo->getDescription() + " Firehose Response Queue", FIREHOSE_USB_RX_WATERMARK_LOW)
, m_responses()
, m_responseOffset(0)
, m_mutex()
, m_rxDataEvent(true)
, m_connectMutex()
, m_bConnected(false)
, m_dataLeftSend(TOOLS_UI64(0))
, m_dataLeftReceive(TOOLS_UI64(0))
, m_flowControlEvent(true, true)
{
   (void)handle; // Suppress unused parameter warning
}

// ----------------------------------------------------------------------------
// ~Firehose
//
// ----------------------------------------------------------------------------
Firehose::~Firehose()
{
   if(m_pFirehoseLoader != nullptr)
   {
      m_pFirehoseLoader->unsubscribe(this, &Firehose::onFirehoseEvent);
   }
   TOOLS_IGNORE_EXCEPTIONS(forceDisconnect());
}

// ----------------------------------------------------------------------------
// initialize
//
// ----------------------------------------------------------------------------
void Firehose::initialize()
{
   setFlowControlWatermarks(
      FIREHOSE_USB_RX_WATERMARK_LOW,
      FIREHOSE_USB_RX_WATERMARK_HIGH,
      FIREHOSE_USB_RX_WATERMARK_DNE
   );
   setFlowControlCallback(std::bind(&Base::flowControlCallback, getSharedPtr(), std::placeholders::_1));
}

// ----------------------------------------------------------------------------
// setIo
//
/// Firehose always use IO shared from Sahara
// ----------------------------------------------------------------------------
void Firehose::setIo(const Communication::CommonIoPtr& pIo)
{
   m_pIo = pIo;

   m_responses.clear();
   setSize(0);
   m_rxDataEvent.reset();
   m_responseOffset = 0;
   m_bConnected = false;
   m_dataLeftSend = TOOLS_UI64(0);
   m_dataLeftReceive = TOOLS_UI64(0);
}

// ----------------------------------------------------------------------------
// connect
//
/// Connects the protocol
// ----------------------------------------------------------------------------
void Firehose::connect(const int32_t clientId)
{
   (void)clientId; // Suppress unused parameter warning
   TOOLS_ASSERT_OR_THROW(STATE_DISCONNECTED != getState(), ToolException("Firehose Protocol Unavailable"));

   std::lock_guard<std::recursive_mutex> lock(m_connectMutex);

   // Firehose is very state dependent; only allow one client connection at a
   // time
   TOOLS_ASSERT_OR_THROW(
      !m_bConnected,
      Device::
         Exception(Device::Exception::DEVICE_CONNECTION_LOCKED, "Firehose protocol already opened: " + getDescription())
   );

   if(!m_bConnected)
   {
      m_pIo->registerReceiveAsync(std::bind(&Firehose::receiveData, getFirehoseSharedPtr(), std::placeholders::_1));

      m_pIo->enableLegacyTimeoutConfig();
      m_pIo->open();

      m_bConnected = true;

      FLOG_INFO("Connected: " + getDescription());
   }
}

// ----------------------------------------------------------------------------
// disconnect
//
/// Disconnects the protocol
// ----------------------------------------------------------------------------
void Firehose::disconnect(const int32_t clientId)
{
   (void)clientId; // Suppress unused parameter warning
   std::lock_guard<std::recursive_mutex> lock(m_connectMutex);
   if(m_bConnected)
   {
      m_bConnected = false;

      // closeLog(clientId);

      m_pIo->close();

      FLOG_INFO("Disconnected: " + getDescription());
   }
}

// ----------------------------------------------------------------------------
// reset
//
/// Resets the communication layer
// ----------------------------------------------------------------------------
void Firehose::reset(Direction dir)
{
   TOOLS_UNUSED_PARAMETER(dir);

   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_dataLeftSend = TOOLS_UI64(0);
   m_dataLeftReceive = TOOLS_UI64(0);
   m_responses.clear();
   setSize(0);
   m_rxDataEvent.reset();
   m_pIo->reset();
}

// ----------------------------------------------------------------------------
// sendSync
//
/// Calls SendAsync; Firehose does not expect a response
// ----------------------------------------------------------------------------
Device::DataPacketPtr Firehose::sendSync(
   const Device::SharedByteBufferPtr& pBuffer,
   const std::optional<std::chrono::milliseconds>& timeout,
   bool bPriority
)
{
   TOOLS_UNUSED_PARAMETER(timeout);

   TOOLS_UNUSED_RETURN(sendAsync(pBuffer, bPriority));

   return Device::DataPacketPtr();
}

// ----------------------------------------------------------------------------
// sendAsync
//
/// Sends the data
/// @returns NULL_TRANSACTION_ID - Firehose does not expect a response
// ----------------------------------------------------------------------------
Base::TransactionId Firehose::sendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority)
{
   TOOLS_UNUSED_PARAMETER(bPriority);

   if(0 != m_dataLeftSend)
   {
      if(pBuffer->size() > m_dataLeftSend)
      {
         TOOLS_ASSERT_MESSAGE("Firehose sending more data than expected");
         m_dataLeftSend = 0;
      }
      else
      {
         m_dataLeftSend -= pBuffer->size();
      }
   }
   else
   {
      // No Firehose traffic logging until native stack is implemented
#if 0
      m_pProtocolLogMgr->appendLogItem(
         std::chrono::system_clock::now(),
         Device::DataPacket::Direction::DIR_TX,
         NULL,
         0,
         pBuffer->begin(),
         pBuffer->size()
      );
#endif
   }

   // FLOG_DATA(
   //    "Tx size: " + std::string(std::to_string(pBuffer->size()).c_str()),
   //    pBuffer,
   //    DataType::Tx
   // );
   uint64_t bytesSent = m_pIo->sendSync(pBuffer);

   if(pBuffer->size() != bytesSent)
   {
      send(
         Util::Message::Level::WARNING,
         "Firehose send data",
         "Send Data Failure",
         "Firehose request not properly sent from Firehose protocol: " + getCommonIoDescription()
      );
   }

   TOOLS_ASSERT_OR_THROW(
      0 != bytesSent,
      Device::Exception(Device::Exception::DEVICE_INVALID_PACKET, "Send 0 byte fail: " + getDescription())
   );
   return NULL_TRANSACTION_ID;
}

// ----------------------------------------------------------------------------
// cancelTx
//
/// @returns False.  Firehose transactions cannot be canceled
// ----------------------------------------------------------------------------
bool Firehose::cancelTx(TransactionId transactionId)
{
   TOOLS_UNUSED_PARAMETER(transactionId);

   return false;
}

// ----------------------------------------------------------------------------
// forceDisconnect
//
/// Forces to disconnect.  Used when shutting the port down.
// ----------------------------------------------------------------------------
void Firehose::forceDisconnect()
{
   std::lock_guard<std::recursive_mutex> lock(m_connectMutex);

   if(!m_bConnected)
   {
      // Already disconnected
      return;
   }

   FLOG_INFO("Forcing disconnect: " + getDescription());

   disconnect();
}

// ----------------------------------------------------------------------------
// getBytes
//
/// Gets specific number of bytes from the cached responses
// ----------------------------------------------------------------------------
void Firehose::getBytes(uint8_t* pDestination, uint32_t bytesToRead, uint32_t& bytesRead)
{
   size_t offset = 0;
   bytesRead = 0;
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   while(getSize() != 0 && bytesToRead > 0)
   {
      Device::SharedByteBufferPtr pResponse = *m_responses.begin();

      TOOLS_ASSERT(pResponse->size() >= m_responseOffset);
      size_t bufferToCopy = std::min<size_t>(bytesToRead, pResponse->size() - m_responseOffset);

      std::memcpy(reinterpret_cast<void*>(pDestination + offset), pResponse->begin() + m_responseOffset, bufferToCopy);
      m_responseOffset += static_cast<uint32_t>(bufferToCopy);
      if(m_responseOffset == pResponse->size())
      {
         m_responses.pop_front();
         decrementSize();
         m_responseOffset = 0;
      }

      offset += bufferToCopy;
      bytesToRead -= static_cast<uint32_t>(bufferToCopy);
      bytesRead += static_cast<uint32_t>(bufferToCopy);
   }
   updateFlowControlCount(getSize());
   if(0 == getSize())
   {
      // LOG_TRACE(getLogger(), "All bytes pushed");
      m_rxDataEvent.reset();
   }
}

// ----------------------------------------------------------------------------
// handleFirehoseLoaderError
//
/// Handle the error returned from firehose loader
// ----------------------------------------------------------------------------
void Firehose::handleFirehoseLoaderError(void)
{
   if(m_pFirehoseLoader == nullptr)
   {
      TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Firehose loader is freed"));
   }
   std::string errorString(m_pFirehoseLoader->getDeviceErrorString());
   if(errorString.empty())
   {
      errorString = m_pFirehoseLoader->getInternalErrorString();
   }

   std::smatch results;
   if(std::regex_search(errorString, results, std::regex(DEVICE_STORAGE_OPEN_FAILURE_PATTERN.c_str())))
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_RESPONSE_ERROR,
         Device::Exception::getErrorJson(
            ERR_DEVICE_STORAGE_OPEN_FAILURE,
            SUGG_DEVICE_STORAGE_OPEN_FAILURE,
            std::string(POC(TARGET)) + " or " + std::string(POC(BOOT_STORAGE))
         )
      ));
   }
   else if(std::regex_search(errorString, results, std::regex(FIREHOSE_IMAGE_NOT_FOUND_PATTERN.c_str())))
   {
      size_t start = errorString.find("\'");
      size_t end = errorString.find("\'", start + 1);
      std::string imageName = "";
      if(start != std::string::npos && end != std::string::npos && end > start)
      {
         imageName = errorString.substr(start + 1, end - start - 1);
      }

      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_RESPONSE_ERROR,
         Device::Exception::getErrorJson(
            ERR_IMAGE_NOT_FOUND(imageName),
            SUGG_IMAGE_NOT_FOUND,
            std::string(POC(TARGET)) + " or " + std::string(POC(CLIENT))
         )
      ));
   }
   else if(std::regex_search(errorString, results, std::regex(FIREHOSE_SIGNATURE_VERIFICATION_PATTERN.c_str())))
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_RESPONSE_ERROR,
         Device::Exception::getErrorJson(
            ERR_SIGNATURE_VERIFICATION_FAILED,
            SUGG_SIGNATURE_VERIFICATION_FAILED,
            std::string(POC(TARGET)) + " or " + std::string(POC(CLIENT))
         )
      ));
   }
   else
   {
      TOOLS_THROW(Device::Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Firehose process failed: " + errorString)
      );
   }
}

// ----------------------------------------------------------------------------
// onFirehoseEvent
//
// ----------------------------------------------------------------------------
void Firehose::onFirehoseEvent(Device::Protocol::FirehoseEvent* pEvent)
{
   notify(std::shared_ptr<Device::Protocol::FirehoseEvent>(pEvent, [](Device::Protocol::FirehoseEvent*) {}));
}

// ----------------------------------------------------------------------------
// processCommand
//
/// Executes firehose loader with specified options
// ----------------------------------------------------------------------------
void Firehose::processCommand(
   FirehoseCommandType& pParameters,
   const std::filesystem::path& readImagePath,
   const std::optional<std::chrono::milliseconds>& rxTimeout
)
{
   if(m_pFirehoseLoader == nullptr)
   {
      FLOG_INFO("Create new Firehose Loader instance: " + getDescription());
      m_pFirehoseLoader = std::make_shared<Device::Protocol::FirehoseLoader>(getFirehoseSharedPtr());
      m_pFirehoseLoader->subscribe(this, &Firehose::onFirehoseEvent);
   }
   else
   {
      FLOG_INFO("Use existing Firehose Loader instance and clean setting: " + getDescription());
      m_pFirehoseLoader->clearSettings();
   }

   try
   {
      int32_t errorCode = 0;

      if(readImagePath.empty())
      {
         pParameters.push_back(
            std::string("--mainoutputdir=") + Device::Manager::getInstance()->getTempDirectory().string().c_str()
         );
      }
      else
      {
         pParameters.push_back(std::string("--mainoutputdir=") + readImagePath.string().c_str());
      }

      m_pFirehoseLoader->setRxTimeout(rxTimeout);

      FLOG_INFO("Firehose Loader execute command: " + getDescription());
      errorCode = m_pFirehoseLoader->executeCommand(pParameters);
      FLOG_INFO("Firehose Loader execute command done: " + getDescription());

      if(errorCode != 0)
      {
         handleFirehoseLoaderError();
      }
   }
   catch(const Device::Exception& e)
   {
      TOOLS_THROW(e);
   }
   catch(const ToolException& e)
   {
      TOOLS_THROW(e);
   }
   catch(...)
   {
      handleFirehoseLoaderError();
   }
}

// ----------------------------------------------------------------------------
// getStorageInfo
//
/// Get the parsed Storage/Flash information
// ----------------------------------------------------------------------------
void Firehose::getStorageInfo(StorageInfoType& info)
{
   TOOLS_ASSERT_OR_THROW(
      m_pFirehoseLoader != nullptr,
      Device::Exception(Device::Exception::DEVICE_UNKNOWN_ERROR, "Firehose Loader not active " + getDescription())
   );
   m_pFirehoseLoader->getStorageInfo(info);
}

// ----------------------------------------------------------------------------
// receiveData
//
/// Collects the data and notifies of new data available
// ----------------------------------------------------------------------------
void Firehose::receiveData(const Device::SharedByteBufferPtr& pBuffer)
{
   if(pBuffer != nullptr && pBuffer->size() > 0)
   {
      // FLOG_DATA(
      //    "Rx size: " + std::string(std::to_string(pBuffer->size()).c_str()),
      //    pBuffer,
      //    DataType::Rx
      // );

      // When Firehose protocol is flow controlled, still allow packets to go
      // through but with a delay. This is optimal for memory consumption but
      // can avoid stall. Need to investigate why Firehose response queue
      // consumer was not able to drain all packets.
      if(isFlowControlled())
      {
         Util::ThisThread::sleep(FIREHOSE_RX_WAIT_INTERVAL);
      }

      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      m_responses.push_back(pBuffer);
      incrementSize();
      updateFlowControlCount(getSize());
      m_rxDataEvent.signal();
   }
}

// ----------------------------------------------------------------------------
// waitForData
//
/// Waits until firehose response data is ready or timeout
// ----------------------------------------------------------------------------
bool Firehose::waitForData(const std::optional<std::chrono::milliseconds>& timeout)
{
   return Util::ThisThread::waitForEvent(&m_rxDataEvent, timeout);
}

} // namespace Protocol
} // namespace Device

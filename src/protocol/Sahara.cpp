// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "protocol/Sahara.h"

#include "communication/CommonIO.h"
#include "device/Buffer.h"
#include "device/Exception.h"
#include "device/Fwd.h"
#include "device/Impl.h"
#include "device/Manager.h"
#include "report/DataContainer.h"
#include "report/Thread.h"
#include "util/MemoryHelper.h"
#include "util/StringHelper.h"
#include "util/ThisThread.h"

#include <chrono>
#include <memory>

namespace Device {
namespace Protocol {



static const int32_t SAHARA_V30_MAX_WARM_RESET_CYCLES = 3;
// ----------------------------------------------------------------------------
// SaharaRxWorker
//
/// Handles incoming data from the communication layer
// ----------------------------------------------------------------------------
class SaharaRxWorker
: public Report::Thread::Work
, public Report::DataContainer
, public std::enable_shared_from_this<SaharaRxWorker>
{
   TOOLS_FORBID_COPY(SaharaRxWorker);

public:
   SaharaRxWorker(const SaharaPtr& pParent)
   : Report::Thread::Work(std::string(pParent->getDescription().c_str()) + " Sahara Rx Thread")
   , Report::DataContainer(std::string(pParent->getDescription().c_str()) + " Sahara Rx Queue", 2)
   , m_pParent(pParent)
   , m_incomingPacketQueueMutex()
   , m_rxWorkerEvent()
   , m_incomingPackets()
   , m_pPartialPacket(Device::Buffer::createBuffer())
   , m_dataLeftReceive(TOOLS_UI64(0))
   {
   }

   virtual ~SaharaRxWorker()
   {
   }

   virtual void onRun()
   {
      Device::SharedByteBufferPtr pBuffer;
      while(!isStopSignaled())
      {
         try
         {
            do
            {
               uint64_t offset = 0;
               pBuffer = NULL;
               {
                  // Reduce lock time and avoid nested lock
                  std::lock_guard<std::recursive_mutex> lock(m_incomingPacketQueueMutex);
                  if(!m_incomingPackets.empty())
                  {
                     pBuffer = m_incomingPackets.front();
                     m_incomingPackets.pop_front();
                     decrementSize();
                  }
               }
               if(pBuffer != nullptr)
               {
                  while(offset < pBuffer->size())
                  {
                     if(0 != m_dataLeftReceive)
                     {
                        // First, run through any data transfer payload
                        if(!m_pPartialPacket->empty())
                        {
                           TOOLS_ASSERT_MESSAGE("Sahara data transfer started "
                                                "with partial frame");
                           m_pPartialPacket = Device::Buffer::createBuffer();
                           m_pPartialPacket->reserve(TOOLS_SIZEOF(Sahara::FrameHeader));
                        }

                        uint64_t nBytes = std::min<uint64_t>(m_dataLeftReceive, pBuffer->size() - offset);
                        m_dataLeftReceive -= nBytes;
                        offset += nBytes;

                        {
                           std::lock_guard<std::recursive_mutex> lock(m_pParent->m_mutex);
                           if(nBytes == pBuffer->size())
                           {
                              m_pParent->m_responses.push_back(pBuffer);
                              m_pParent->incrementSize();
                           }
                           else
                           {
                              if(0 < nBytes)
                              {
                                 Device::SharedByteBufferPtr pCopyBuff = Device::Buffer::
                                    createCopy(pBuffer->begin() + offset, static_cast<uint64_t>(nBytes));
                                 if(pCopyBuff == nullptr)
                                 {
                                    // shall never comes here
                                    FLOG_WARNING("Sahara payload memory copy a "
                                                 "NULL buffer created");
                                 }
                                 m_pParent->m_responses.push_back(pCopyBuff);
                                 m_pParent->incrementSize();
                              }
                              else
                              {
                                 FLOG_WARNING("Sahara payload memory copy 0 "
                                              "bytes requested");
                              }
                           }
                           m_pParent->m_rxDataEvent.signal();
                        }
                     }
                     else
                     {
                        if(TOOLS_SIZEOF(Sahara::FrameHeader) > m_pPartialPacket->size())
                        {
                           // Fill the frame header
                           size_t headerToCopy = std::min<
                              size_t>(TOOLS_SIZEOF(Sahara::FrameHeader) - m_pPartialPacket->size(), pBuffer->size());
                           m_pPartialPacket->append(pBuffer->begin() + offset, headerToCopy);
                           offset += headerToCopy;

                           if(TOOLS_SIZEOF(Sahara::FrameHeader) == m_pPartialPacket->size())
                           {
                              const Sahara::FrameHeader* pFrame = Util::buffer_cast<
                                 const Sahara::FrameHeader*>(m_pPartialPacket->begin(), m_pPartialPacket->size());

                              // Check if the protocol should expect to receive
                              // a header only packet
                              switch(pFrame->m_command)
                              {
                                 case Sahara::SAHARA_RESET_RESP:
                                 case Sahara::SAHARA_COMMAND_READY: {
                                    std::lock_guard<std::recursive_mutex> lock(m_pParent->m_mutex);
                                    m_pParent->m_responses.push_back(m_pPartialPacket);
                                    m_pParent->incrementSize();
                                    m_pParent->m_rxDataEvent.signal();
                                 }
                                    m_pPartialPacket = Device::Buffer::createBuffer();
                                    m_pPartialPacket->reserve(TOOLS_SIZEOF(Sahara::FrameHeader));
                                    break;
                                 default:
                                    m_pPartialPacket->reserve(pFrame->m_length);
                                    break;
                              }
                           }
                        }
                        else
                        {
                           // Fill the frame payload
                           const Sahara::FrameHeader* pFrame = Util::buffer_cast<
                              const Sahara::FrameHeader*>(m_pPartialPacket->begin(), m_pPartialPacket->size());
                           size_t nBytes =
                              std::min<size_t>(pFrame->m_length - m_pPartialPacket->size(), pBuffer->size());
                           m_pPartialPacket->append(pBuffer->begin() + offset, nBytes);
                           offset += nBytes;

                           if(pFrame->m_length == m_pPartialPacket->size())
                           {
                              // Check if the protocol should expect to receive
                              // raw data
                              switch(pFrame->m_command)
                              {
                                 case Sahara::SAHARA_HELLO: {
                                    const Sahara::Hello* pHello = Util::buffer_cast<
                                       const Sahara::Hello*>(m_pPartialPacket->begin(), m_pPartialPacket->size());
                                    if(TOOLS_SIZEOF(Sahara::Hello) == m_pPartialPacket->size() &&
                                       m_pParent->getMode() != static_cast<Sahara::Mode>(pHello->m_mode))
                                    {
                                       FLOG_INFO("Sahara mode change: " + m_pParent->getDescription());
                                       m_pParent->setMode(static_cast<Sahara::Mode>(pHello->m_mode));
                                    }
                                 }
                                 break;

                                 case Sahara::SAHARA_READ_DATA: {
                                    const Sahara::ReadData* pReadData = Util::buffer_cast<
                                       const Sahara::ReadData*>(m_pPartialPacket->begin(), m_pPartialPacket->size());

                                    m_pParent->m_dataLeftSend = pReadData->m_dataLength;
                                 }
                                 break;
                                 case Sahara::SAHARA_COMMAND_EXECUTE_RESP: {
                                    const Sahara::CommandExecuteResp* pExecuteData = Util::buffer_cast<
                                       const Sahara::
                                          CommandExecuteResp*>(m_pPartialPacket->begin(), m_pPartialPacket->size());

                                    m_dataLeftReceive = pExecuteData->m_responseLength;
                                 }
                                 break;
                                 case Sahara::SAHARA_64_BIT_READ_DATA: {
                                    const Sahara::ReadData64Bit* pReadData = Util::buffer_cast<
                                       const Sahara::
                                          ReadData64Bit*>(m_pPartialPacket->begin(), m_pPartialPacket->size());

                                    m_pParent->m_dataLeftSend = pReadData->m_dataLength;
                                 }
                                 break;
                                 default:
                                    break;
                              }

                              {
                                 std::lock_guard<std::recursive_mutex> lock(m_pParent->m_mutex);
                                 m_pParent->m_responses.push_back(m_pPartialPacket);
                                 m_pParent->incrementSize();
                                 m_pParent->m_rxDataEvent.signal();
                              }

                              m_pPartialPacket = Device::Buffer::createBuffer();
                              m_pPartialPacket->reserve(TOOLS_SIZEOF(Sahara::FrameHeader));
                           }
                        }
                     }
                  }
               }
            } while(pBuffer != nullptr && !isStopSignaled());

            Util::ThisThread::waitForEvent(&m_rxWorkerEvent, SAHARA_RX_WAIT_INTERVAL);
         }
         TOOLS_CATCH(e, APP_PUBLISH_EXCEPTION(m_pParent, e));
      }
      FLOG_INFO("SaharaRx thread exited: " + m_pParent->getDescription());
   }

   void setDataLeftReceive(uint64_t dataLeftReceive)
   {
      std::lock_guard<std::recursive_mutex> lock(m_incomingPacketQueueMutex);
      m_dataLeftReceive = dataLeftReceive;
   }

   void reset()
   {
      std::lock_guard<std::recursive_mutex> lock(m_incomingPacketQueueMutex);
      m_dataLeftReceive = TOOLS_UI64(0);
      m_pPartialPacket->clear();
      m_incomingPackets.clear();
      setSize(0);
   }

   void setRxWorkerEventSignal()
   {
      m_rxWorkerEvent.signal();
   }

   void setReceicveCallback()
   {
      m_pParent->getCommonIo()
         ->registerReceiveAsync(std::bind(&SaharaRxWorker::receiveData, shared_from_this(), std::placeholders::_1));
   }

private:
   // ----------------------------------------------------------------------------
   // receiveData
   //
   /// Collects the data and notifies of new data available
   // ----------------------------------------------------------------------------
   void receiveData(const Device::SharedByteBufferPtr& pBuffer)
   {
      if(pBuffer != nullptr)
      {
         std::lock_guard<std::recursive_mutex> lock(m_incomingPacketQueueMutex);
         // FLOG_DATA(
         //    "Rx size: " +
         //    std::string(std::to_string(pBuffer->size()).c_str()), pBuffer,
         //    DataType::Rx
         // );

         m_incomingPackets.push_back(pBuffer);
         incrementSize();
         // FLOG_TRACE("Sahara bytes incomming. Size:" +
         // Lang::atom_cast<Lang::String>(pBuffer->size()));
         m_rxWorkerEvent.signal();
      }
   }

   typedef std::list<Device::SharedByteBufferPtr> PacketQueue;

   Util::CheckedPointer<Sahara> m_pParent;
   PacketQueue m_incomingPackets; ///< Packets read from CommonIo
   std::recursive_mutex m_incomingPacketQueueMutex;
   Util::Event m_rxWorkerEvent;

   Device::SharedByteBufferPtr m_pPartialPacket;
   uint64_t m_dataLeftReceive; ///< How much left to receive
};

// ----------------------------------------------------------------------------
// Sahara
//
/// Constructor for live connection
// ----------------------------------------------------------------------------
Sahara::Sahara(const Communication::CommonIoPtr& pIo, Mode mode)
: Base(pIo)
, Report::DataContainer(pIo->getDescription() + " Sahara Response Queue", 2)
, m_initialMode(mode)
, m_mode(mode)
, m_responses()
, m_mutex()
, m_rxDataEvent()
, m_connectMutex()
, m_bConnected(false)
, m_dataLeftSend(TOOLS_UI64(0))
, m_bFirehoseMode(false)
, m_bCommandMode(false)
, m_saharaResetTimes(0)
, m_currentVersion(static_cast<uint32_t>(Sahara::SUPPORTED_DEFAULT_VERSION))
{
}

// ----------------------------------------------------------------------------
// ~Sahara
//
// ----------------------------------------------------------------------------
Sahara::~Sahara()
{
   TOOLS_IGNORE_EXCEPTIONS(forceDisconnect());
}

// ----------------------------------------------------------------------------
// initialize
//
/// Creates Rx worker thread
// ----------------------------------------------------------------------------
void Sahara::initialize()
{
   if(m_pRxWork == nullptr)
   {
      m_pRxWork = std::make_shared<SaharaRxWorker>(getSaharaSharedPtr());
      m_pRxWork->setReceicveCallback();
      m_pRxThread = std::make_shared<Util::StdThreadWrapper>(m_pRxWork);
      m_pRxThread->start();
   }
}

// ----------------------------------------------------------------------------
// finalize
//
// Stops Rx worker thread
// ----------------------------------------------------------------------------
void Sahara::finalize()
{
   if(m_pRxThread != nullptr && Util::StdThreadWrapper::State::CREATED != m_pRxThread->getState())
   {
      TOOLS_IGNORE_EXCEPTIONS(m_pRxThread->stop(); m_pRxWork->setRxWorkerEventSignal(); m_pRxThread->waitForStop(););
   }

   m_pRxThread = NULL;
   m_pRxWork = NULL;
}

// ----------------------------------------------------------------------------
// getMode
//
/// @returns Last known Sahara mode
// ----------------------------------------------------------------------------
Sahara::Mode Sahara::getMode() const
{
   return m_mode;
}

// ----------------------------------------------------------------------------
// setMode
//
/// @returns Set new Sahara mode
// ----------------------------------------------------------------------------
void Sahara::setMode(Mode mode, bool bNotify)
{
   FLOG_INFO(
      "Sahara old mode " + std::string(std::to_string(m_mode)) + " new mode " + std::string(std::to_string(mode)) +
      " : " + getDescription()
   );

   /// EFS sync mode cannot be overridden
   if(m_mode != mode && MODE_EFS_SYNC != m_mode)
   {
      if(toDeviceMode(m_mode) != toDeviceMode(mode))
      {
         m_mode = mode;
         if(bNotify)
         {
            Device::Manager::getInstance()->notifyAsync(std::make_shared<DeviceModeChangeEvent>(this->getDevice()));
         }
      }
      else
      {
         m_mode = mode;
      }
   }
}

// ----------------------------------------------------------------------------
// toDeviceMode
//
/// @returns Device mode converted from Sahara mode
// ----------------------------------------------------------------------------
Device::DeviceMode Sahara::toDeviceMode(Mode mode)
{
   DeviceMode deviceMode = DEVICE_MODE_NONE;

   if(Device::Protocol::Sahara::Mode::MODE_MEMORY_DEBUG == mode)
   {
      deviceMode = DEVICE_MODE_SAHARA_CRASH;
   }
   else if(Device::Protocol::Sahara::Mode::MODE_EFS_SYNC == mode)
   {
      deviceMode = DEVICE_MODE_SAHARA_EFS_SYNC;
   }
   else if(Device::Protocol::Sahara::Mode::MODE_UNKNOWN != mode)
   {
      deviceMode = DEVICE_MODE_SAHARA_DOWNLOAD;
   }

   return deviceMode;
}

// ----------------------------------------------------------------------------
// connect
//
/// Connects the protocol
// ----------------------------------------------------------------------------
void Sahara::connect(const int32_t clientId)
{
   (void)clientId; // Suppress unused parameter warning
   TOOLS_ASSERT_OR_THROW(STATE_DISCONNECTED != getState(), ToolException("Sahara Protocol Unavailable"));

   std::lock_guard<std::recursive_mutex> lock(m_connectMutex);

   // Sahara is very state dependent; only allow one client connection at a time
   TOOLS_ASSERT_OR_THROW(
      !m_bConnected,
      Device::
         Exception(Device::Exception::DEVICE_CONNECTION_LOCKED, "Sahara protocol already opened: " + getDescription())
   );

   if(!m_bConnected)
   {
      if(m_pRxWork != nullptr)
      {
         m_pRxWork->setReceicveCallback();
      }
      m_pIo->open();
      m_bConnected = true;

      FLOG_INFO("Connected: " + getDescription());
   }
}

// ----------------------------------------------------------------------------
// disconnect
//
/// Disconnects the io
// ----------------------------------------------------------------------------
void Sahara::doDisconnect()
{
   m_pIo->close();
}

// ----------------------------------------------------------------------------
// disconnect
//
/// Disconnects the protocol
// ----------------------------------------------------------------------------
void Sahara::disconnect(const int32_t clientId)
{
   (void)clientId; // Suppress unused parameter warning
   std::lock_guard<std::recursive_mutex> lock(m_connectMutex);
   if(m_bConnected)
   {
      m_bConnected = false;

      m_pIo->close();

      FLOG_INFO("Disconnected: " + getDescription());
   }
}

// ----------------------------------------------------------------------------
// reset
//
/// Resets the communication layer
// ----------------------------------------------------------------------------
void Sahara::reset(Direction dir)
{
   TOOLS_UNUSED_PARAMETER(dir);

   m_dataLeftSend = TOOLS_UI64(0);
   m_responses.clear();
   setSize(0);

   if(m_pRxWork != nullptr)
   {
      m_pRxWork->reset();
   }
   m_pIo->reset();
}

// ----------------------------------------------------------------------------
// sendSync
//
/// Calls SendAsync; Sahara does not expect a response
// ----------------------------------------------------------------------------
Device::DataPacketPtr Sahara::sendSync(
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
/// @returns NULL_TRANSACTION_ID - Sahara does not expect a response
// ----------------------------------------------------------------------------
Base::TransactionId Sahara::sendAsync(const Device::SharedByteBufferPtr& pBuffer, bool bPriority)
{
   TOOLS_UNUSED_PARAMETER(bPriority);

   if(0 != m_dataLeftSend)
   {
      if(pBuffer->size() > m_dataLeftSend)
      {
         TOOLS_ASSERT_MESSAGE("Sahara sending more data than expected");
         m_dataLeftSend = 0;
      }
      else
      {
         m_dataLeftSend -= pBuffer->size();
      }
   }
   else
   {
      const FrameHeader* pFrame = Util::buffer_cast<const FrameHeader*>(pBuffer->begin(), pBuffer->size());

      // Check if the protocol should expect to receive raw data
      switch(pFrame->m_command)
      {
         case SAHARA_MEMORY_READ: {
            const MemoryRead* pReadData = Util::buffer_cast<const MemoryRead*>(pBuffer->begin(), pBuffer->size());

            m_pRxWork->setDataLeftReceive(pReadData->m_memoryLength);
         }
         break;
         case SAHARA_64_BIT_MEMORY_READ: {
            const MemoryRead64Bit* pExecuteData =
               Util::buffer_cast<const MemoryRead64Bit*>(pBuffer->begin(), pBuffer->size());

            m_pRxWork->setDataLeftReceive(pExecuteData->m_memoryLength);
         }
         break;
         default:
            break;
      }
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
         "Sahara send data",
         "Send Data Failure",
         "Sahara request not properly sent from Sahara protocol: " + getCommonIoDescription()
      );
   }

   return NULL_TRANSACTION_ID;
}

// ----------------------------------------------------------------------------
// cancelTx
//
/// @returns False.  Sahara transactions cannot be canceled
// ----------------------------------------------------------------------------
bool Sahara::cancelTx(TransactionId transactionId)
{
   TOOLS_UNUSED_PARAMETER(transactionId);

   return false;
}

// ----------------------------------------------------------------------------
// forceDisconnect
//
/// Forces to disconnect.  Used when shutting the port down.
// ----------------------------------------------------------------------------
void Sahara::forceDisconnect()
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
// onAdd
//
// ----------------------------------------------------------------------------
void Sahara::onAdd()
{
   setState(STATE_AVAILABLE);

   setMode(m_initialMode, false);
   Device::Manager::getInstance()->notifyAsync(std::make_shared<DeviceModeChangeEvent>(this->getDevice()));
}

// ----------------------------------------------------------------------------
// onDrop
//
// ----------------------------------------------------------------------------
void Sahara::onDrop()
{
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      m_responses.clear();
      setSize(0);
   }

   if(m_pRxWork != nullptr)
   {
      m_pRxWork->reset();
   }
   setFirehoseModeEnabled(false);
   setCommandModeEnabled(false);
   setMode(m_initialMode, false);
   setSaharaVersion(static_cast<uint32_t>(Sahara::SUPPORTED_DEFAULT_VERSION));
   m_saharaResetTimes = 0;
   Device::Manager::getInstance()->notifyAsync(std::make_shared<DeviceModeChangeEvent>(this->getDevice()));
}

// ----------------------------------------------------------------------------
// getFrame
//
/// Gets data from the cached responses
// ----------------------------------------------------------------------------
Device::SharedByteBufferPtr Sahara::getFrame(const std::optional<std::chrono::milliseconds>& timeout)
{
   {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      if(!m_responses.empty())
      {
         Device::SharedByteBufferPtr pResponse = *m_responses.begin();
         m_responses.pop_front();
         decrementSize();
         return pResponse;
      }
   }

   auto rxTimeOut = std::chrono::system_clock::now() + timeout.value_or(std::chrono::milliseconds(0));
   while(std::chrono::system_clock::now() < rxTimeOut)
   {
      {
         std::lock_guard<std::recursive_mutex> lock(m_mutex);
         if(!m_responses.empty())
         {
            Device::SharedByteBufferPtr pResponse = *m_responses.begin();
            m_responses.pop_front();
            decrementSize();
            return pResponse;
         }
      }

      Util::ThisThread::waitForEvent(&m_rxDataEvent, SAHARA_RX_WAIT_INTERVAL);
   }

   return Device::SharedByteBufferPtr();
}

// ----------------------------------------------------------------------------
// setFirehoseMode
//
/// specify device is entered into firehose mode
// ----------------------------------------------------------------------------
void Sahara::setFirehoseModeEnabled(bool enabled)
{
   m_bFirehoseMode = enabled;
}

// ----------------------------------------------------------------------------
// isFirehoseModeEnabled
//
/// @returns to see if the device entered firehose mode
// ----------------------------------------------------------------------------
bool Sahara::isFirehoseModeEnabled() const
{
   return m_bFirehoseMode;
}

// ----------------------------------------------------------------------------
// setCommandModeEnabled
//
/// specify device is entered into sahara command mode
// ----------------------------------------------------------------------------
void Sahara::setCommandModeEnabled(bool enabled)
{
   m_bCommandMode = enabled;
}

// ----------------------------------------------------------------------------
// isCommandModeEnabled
//
/// @returns to see if the device entered sahara command mode
// ----------------------------------------------------------------------------
bool Sahara::isCommandModeEnabled() const
{
   return m_bCommandMode;
}

// ----------------------------------------------------------------------------
// isSaharaV3WarmResetEnabled
//
/// @returns true if warm reset is still allowed
// ----------------------------------------------------------------------------
bool Sahara::isSaharaV3WarmResetEnabled() const
{
   return m_saharaResetTimes < SAHARA_V30_MAX_WARM_RESET_CYCLES;
}

// ----------------------------------------------------------------------------
// setSaharaVersion
//
/// specify current sahara main version info
// ----------------------------------------------------------------------------
void Sahara::setSaharaVersion(const uint32_t version)
{
   m_currentVersion = version;
}

// ----------------------------------------------------------------------------
// getSaharaVersion
//
/// @returns current sahara main version info
// ----------------------------------------------------------------------------
uint32_t Sahara::getSaharaVersion() const
{
   return m_currentVersion;
}

} // namespace Protocol
} // namespace Device

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "device/Manager.h"

#include "communication/CommonIO.h"
#include "communication/Usb.h"
#include "device/Exception.h"
#include "protocol/Base.h"
#include "protocol/Firehose.h"
#include "protocol/Sahara.h"
#include "qdpublic.h"
#include "report/StatusManager.h"
#include "report/Thread.h"
#include "util/Event.h"
#include "util/MemoryHelper.h"
#include "util/StringHelper.h"
#include "util/SystemHelper.h"
#include "util/ThisThread.h"
#include "util/TimeHelper.h"

#include <chrono>
#include <mutex>
#include <optional>
#include <regex>
#include <util/SystemHelper.h>
#ifdef TOOLS_TARGET_WINDOWS
#include "../Common/utils.h"
#endif

#if defined TOOLS_TARGET_OSX || defined TOOLS_TARGET_LINUX
#include <dirent.h>
#include <regex.h>
#include <sys/stat.h>
#endif


namespace Device {

static const DeviceHandle DEVICE_HANDLE_INCREMENT = TOOLS_I64(0x0001000000000000);
static constexpr auto INITIAL_DISCOVERY_WAIT_PERIOD = std::chrono::milliseconds(10000);
static constexpr auto INITIAL_DISCOVERY_WAIT_PERIOD_NO_EVENT = std::chrono::milliseconds(2000);
static constexpr auto DISCOVERY_WAIT_PERIOD = std::chrono::milliseconds(50);
static constexpr auto SIBLING_PROTOCOL_WAIT = std::chrono::milliseconds(5000);
static constexpr auto REVERSED_DEPARTURE_ARRIVAL_WAIT = std::chrono::milliseconds(5000);
static constexpr auto SAHARA_OTHER_PROTOCOL_WAIT = std::chrono::milliseconds(30000);

static constexpr auto STATUS_CHECK_PERIOD = std::chrono::seconds(10);
static const std::string DUMMY_ADB_SERIAL_NUMBER = ("1234567");


static const std::string STRING_PATTERN_SAHARA = ".*?SAHARA";
static const std::string STRING_PATTERN_FIREHOSE = ".*?FIREHOSE";

static bool g_bFirstCallback = true;
typedef std::set<std::shared_ptr<Device::Communication::CommonIo>> ComSet;
ComSet g_forcedDisconnects;

static const std::string USB_MATCH = "USB\\";
static const std::string USB_VID = "VID";
static const std::string USB_PID = "PID";
static const std::string USB_CID = "CID_";

static const std::string PCI_MATCH = "PCI\\";
static const std::string PCI_VID = "VEN";
static const std::string PCI_PID = "DEV";

static const CriticalEventMap g_criticalEventMap = {
   {EVENT_COMMUNICATION_USB_OPEN_FAILURE, "USB port open unsuccessful"},
   {EVENT_COMMUNICATION_USB_SEND_FAILURE, "USB send data unsuccessful"},

   {EVENT_PROTOCOL_QDSS_ATB_DECODING_ERROR, "QDSS ATB decoding error"},
   {EVENT_PROTOCOL_QDSS_HIGH_MEMORY_DATA_DROP_ERROR, "QDSS channel data drop due to memory"},
   {EVENT_PROTOCOL_QDSS_ATB_FLOW_CONTROL_DATA_DROP_ERROR, "QDSS ATB data drop due to flow control"}
};

enum MatchType
{
   MATCH_FAIL = 0,
   MATCH_SUCCESS,
   MATCH_UNKNOWN
};


MatchType isSerialNumberMatched(
   const Device::ImplPtr& pDevice,
   const std::string& serialNumberMsm,
   const std::string& serialNumberAdb
)
{
   if(!serialNumberMsm.empty() && !pDevice->getSerialNumberMsm().empty() &&
      Util::toLowerCopy(serialNumberMsm) == Util::toLowerCopy(pDevice->getSerialNumberMsm()))
   {
      return MATCH_SUCCESS;
   }

   if(!serialNumberAdb.empty() && !pDevice->getSerialNumberAdb().empty() &&
      Util::toLowerCopy(serialNumberAdb) == Util::toLowerCopy(pDevice->getSerialNumberAdb()))
   {
      return MATCH_SUCCESS;
   }

   if(!serialNumberMsm.empty() && !pDevice->getSerialNumberAdb().empty() &&
      Util::toLowerCopy(serialNumberMsm) == Util::toLowerCopy(pDevice->getSerialNumberAdb()))
   {
      return MATCH_SUCCESS;
   }

   if(!serialNumberAdb.empty() && !pDevice->getSerialNumberMsm().empty() &&
      Util::toLowerCopy(serialNumberAdb) == Util::toLowerCopy(pDevice->getSerialNumberMsm()))
   {
      return MATCH_SUCCESS;
   }

   if((!serialNumberAdb.empty() && !pDevice->getSerialNumberAdb().empty()) ||
      (!serialNumberMsm.empty() && !pDevice->getSerialNumberMsm().empty()))
   {
      return MATCH_FAIL;
   }

   return MATCH_UNKNOWN;
}

bool isUniqueIdentifierMatched(const Device::ImplPtr& pDevice, const std::string& uniqueIdentifier)
{
   // Use unique identifier to address corner cases when serial numbers do not
   // match
   if(!uniqueIdentifier.empty() && !pDevice->getUniqueIdentifier().empty() &&
      uniqueIdentifier == pDevice->getUniqueIdentifier())
   {
      FLOG_INFO(("Matched uniqueIdentifier: " + uniqueIdentifier + " in device: " + std::to_string(pDevice->getHandle())
      )
                   .c_str());
      return true;
   }

   return false;
}

bool isPhysicalLocationMatched(const Device::ImplPtr& pDevice, const std::string& physicalLocation)
{
   // Use parent name to address corner cases when unique identifiers do not
   // match
   if(!physicalLocation.empty() && !pDevice->getDevicePhysicalLocation().empty() &&
      physicalLocation == pDevice->getDevicePhysicalLocation())
   {
      FLOG_INFO(("Matched physicalLocation: " + physicalLocation + " in device: " + std::to_string(pDevice->getHandle())
      )
                   .c_str());
      return true;
   }

   return false;
}

bool isParentNameMatched(const Device::ImplPtr& pDevice, const std::string& parentName)
{
   // Use parent name to address corner cases when unique identifiers do not
   // match
   if(!parentName.empty() && !pDevice->getDescription().empty() && parentName == pDevice->getDescription())
   {
      FLOG_INFO(("Matched parentName: " + parentName + " in device: " + std::to_string(pDevice->getHandle())).c_str());
      return true;
   }

   return false;
}

bool isDevicePathMatched(const Device::ImplPtr& pDevice, const std::string& devicePath)
{
   // Use device path to address corner cases when all other parameters do not
   // match
   if(!devicePath.empty() && !pDevice->getDevicePath().empty() &&
      (std::string::npos != devicePath.find(pDevice->getDevicePath()) ||
       std::string::npos != pDevice->getDevicePath().find(devicePath)))
   {
      FLOG_INFO(("Matched devicePath: " + devicePath + " in device: " + std::to_string(pDevice->getHandle()) + " " +
                 pDevice->getDevicePath())
                   .c_str());
      return true;
   }

   return false;
}

bool isDescriptionMatched(const Device::ImplPtr& pDevice, const std::string& description)
{
   bool matched;

   matched = pDevice->searchUnavailableProtocolByDescription(description);
   if(matched)
   {
      FLOG_INFO(("Matched description: " + description +
                 " to unavailable protocol in device:" + std::to_string(pDevice->getHandle()))
                   .c_str());
   }
   return matched;
}

#ifndef TOOLS_TARGET_WINDOWS
std::string getStringWithoutIndex(const std::string& buffer)
{
   std::string result = buffer;
   size_t offset;

   // Linux drivers may append index at the end of DevDesc, DevName and DevPath
   // Example:
   //    Index (":21") at the end of DevDesc:
   //       /dev/Qualcomm_HS-USB_Diagnostics_90DB:21
   offset = buffer.rfind(":");
   if(std::string::npos != offset)
   {
      std::string indexString = buffer.substr(offset + 1);

      std::string::iterator it = indexString.begin();
      std::string::iterator end = indexString.end();
      while(it != end && *it >= '0' && *it <= '9')
      {
         ++it;
      }

      if(it == end)
      {
         result = buffer.substr(0, offset);
      }
   }

   return result;
}
#endif

bool isIdentifierMatched(const std::string& identifier, const std::string& identifierFromEvent)
{
   bool matched;

#ifdef TOOLS_TARGET_WINDOWS
   matched = (identifier == identifierFromEvent) ? true : false;
#else
   matched = (getStringWithoutIndex(identifier) == getStringWithoutIndex(identifierFromEvent)) ? true : false;
#endif

   if(matched)
   {
      FLOG_INFO(("Matched event identifier: " + identifierFromEvent + " to protocol in device:" + identifier).c_str());
   }
   return matched;
}

bool isRearrival(const Device::ImplPtr& pDevice, const std::string& description, const std::string& identifier)
{
   return pDevice->searchAvailableProtocolByDescriptionAndIdentifier(description, identifier);
}

bool isArrivalProhibited(const Device::ImplPtr& pDevice)
{
   // Arrival should be blocked when Sahara (none Remote EFS Sync) is present
   Device::Impl::ProtocolList protocolList = pDevice->getProtocolList();

   Impl::ProtocolList::iterator prot = protocolList.begin();
   Impl::ProtocolList::iterator protEnd = protocolList.end();
   for(; prot != protEnd; ++prot)
   {
      Protocol::SaharaPtr pSahara = (*prot).dynamicCast<Protocol::Sahara>();
      if(pSahara != nullptr && pSahara->getMode() != Protocol::Sahara::MODE_EFS_SYNC)
      {
         return true;
      }
   }

   return false;
}

// ----------------------------------------------------------------------------
// isBlacklisted
//
/// Check if an interface cannot be supported
// ----------------------------------------------------------------------------
bool isBlacklisted(const std::string& description)
{
   if(std::string::npos != description.find("arm64") ||
      std::string::npos != description.find("Composite Device")) // Composite parent device
                                                                 // cannot be used as protocol
   {
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------
// open
//
/// Open a file with access in system mode.
// ----------------------------------------------------------------------------
void AccessibleFile::open(
   const std::filesystem::path& path, ///< Name of the file to open
   std::ios::openmode mode            ///< Open mode (std::ios flags)
)
{
   m_path = path;

   // Do not open network file directly. Create a local temp file and
   // copy it to the network location later when temp file is closed.
   if(m_path.is_relative() || !(m_path.root_name().string().substr(0, 2) == "\\\\"))
   {
      // Try to open the file; if success just return this path.  That means
      // the system has access to it; no need to copy it.
      try
      {
         std::fstream testFile;
         testFile.exceptions(std::ios::failbit | std::ios::badbit);
         testFile.open(m_path.string().c_str(), std::ios::out | std::ios::binary);
         testFile.close();

         FLOG_INFO("Able to access file: " + m_path.string());
         m_tempPath = m_path;
      }
      TOOLS_CATCH(e, {
         std::filesystem::path directory = Manager::getInstance()->getTempDirectory();
         m_tempPath = Util::createTempFileName(directory);
         m_tempPath = m_tempPath.parent_path() / (m_path.filename().string() + "_" + Util::generateUuid());
         m_tempPath.replace_extension("." + m_path.extension().string().substr(1));

         FLOG_WARNING(
            "Unable to access file: " + std::string(m_path.string().c_str()) +
            ", use temp file: " + std::string(m_tempPath.string().c_str()) + std::string(e.what()) + " " + e.where()
         );
      });
   }
   else
   {
      m_tempPath = m_path;
   }

   // Enable exceptions for better error handling
   m_stream.exceptions(std::ios::failbit | std::ios::badbit);

   // Open the file
   m_stream.open(m_tempPath.string().c_str(), mode);
}

// ----------------------------------------------------------------------------
// close
//
/// Close a file with access in system mode.
// ----------------------------------------------------------------------------
void AccessibleFile::close() throw()
{
   m_stream.close();

   // Copy file to its original network path
   if(m_tempPath != m_path)
   {
      Device::Manager::getInstance()->saveFile(m_tempPath, m_path, true);
   }
}

// ----------------------------------------------------------------------------
// DeviceManagerHelper
//
/// Handles all discovery events from the queue.  Must be on a separate thread
/// since it may do things with the port that cannot be done on the callback
/// thread from QcDevice
// ----------------------------------------------------------------------------
class DeviceManagerHelper : public Report::Thread::Work
{
   TOOLS_FORBID_COPY(DeviceManagerHelper);

public:
   struct DeviceEvent
   {
      std::chrono::system_clock::time_point m_eventTime;                            ///< Time the callback occurred
      std::optional<std::chrono::system_clock::time_point> m_initialProcessingTime; ///< Time the event being processed
                                                                                    ///< the first time
      bool m_bIsPushbackEvent;     ///< If event was pushed back to the list
      bool m_bIsPushbackConnected; ///< If event was pushed back to the list
                                   ///< for a connected device
      bool m_bIsPushbackRearrival; ///< If event was pushed back to the list
                                   ///< due to protocol rearrival
      std::string m_description;
      std::string m_deviceName;
      std::string m_parentDevice;
      std::string m_parentLocationInfomation;
      std::string m_serialNumberMsm;
      std::string m_serialNumberAdb;
      std::string m_location;
      std::string m_devicePath;
      std::string m_hwId;
      std::string m_vid;
      std::string m_pid;
      std::string m_socVersion;
      ULONG m_flags;
      ULONG m_protocol;
   };


   DeviceManagerHelper(const ManagerPtr& pManager)
   : Report::Thread::Work("Device Manager Helper Thread", Report::Thread::HealthMonitorPriority::Critical)
   , m_pManager(pManager)
   , m_events()
   , m_mutex()
   , m_discoverEvent()
   , m_bInitialComplete(false)
   , m_bIgnoreSerNum(false)
   , m_initialDiscovery()
   , m_threadId(0)
   {
   }

   virtual ~DeviceManagerHelper()
   {
   }

#ifdef TOOLS_TARGET_WINDOWS
   static void __stdcall devCommDeviceChangeCb(PCB_PARAMS pCbParams, PVOID* pContext)
#else
   static void devCommDeviceChangeCb(PCB_PARAMS pCbParams, PVOID* pContext)
#endif
   {
      if(g_bFirstCallback)
      {
         Tool::setExceptionTranslator();
         g_bFirstCallback = false;
      }

      try
      {
         reinterpret_cast<DeviceManagerHelper*>(*pContext)->addDiscoveryEvent(pCbParams);
      }
      TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
   }

// ----------------------------------------------------------------------------
// logQDS
//
/// Prints QC Device Discovery logs with Qfs logs,
// ----------------------------------------------------------------------------
#ifdef TOOLS_TARGET_WINDOWS
   static void __stdcall logQDS(int32_t level, PCHAR pMessage)
#else
   static void logQDS(int32_t level, PCHAR pMessage)
#endif
   {
      if(pMessage)
      {
         switch(KL::Level(level))
         {
            case KL::Level::Data:
               PTRACE_LOG(("QDS: " + std::string(pMessage)).c_str());
               break;

            case KL::Level::Debug:
               FLOG_DEBUG(("QDS: " + std::string(pMessage)).c_str());
               break;

            case KL::Level::Warn:
               FLOG_WARNING(("QDS: " + std::string(pMessage)).c_str());
               break;

            case KL::Level::Error:
               FLOG_ERROR(("QDS: " + std::string(pMessage)).c_str());
               break;

            case KL::Level::Info:
            default:
               FLOG_INFO(("QDS: " + std::string(pMessage)).c_str());
               break;
         }
      }
   }

   void initialize()
   {
      // Nothing to do
   }

   void waitForInitialDiscovery()
   {
      size_t retryAttampts = 0;
      const size_t MAX_RETRIES = 4; // 4 * INITIAL_DISCOVERY_WAIT_PERIOD = 40 seconds more than enough
      while(!m_bInitialComplete)
      {
         FLOG_INFO(("In waitForInitialDiscovery. "));
         Util::ThisThread::waitForEvent(&m_initialDiscovery, INITIAL_DISCOVERY_WAIT_PERIOD);

         if(!m_bInitialComplete)
         {
            // If nothing has yet even to show up, then nothing is connected
            // Just go ahead and return
            std::lock_guard<std::recursive_mutex> lock(m_mutex);
            if(m_events.empty())
            {
               m_bInitialComplete = true;
            }
            else if(Util::ThisThread::getId() == m_threadId)
            {
               TOOLS_ASSERT_MESSAGE("Should not call waitForInitialDiscovery "
                                    "api from this thread!!");
               FLOG_ERROR(("Should not call waitForInitialDiscovery api from "
                           "this thread: " +
                           std::to_string(m_bInitialComplete) + std::to_string(m_events.size()))
                             .c_str());
               break; // discovery taking longer than 40 sec force stop
                      // something is not right
            }
            else if(retryAttampts >= MAX_RETRIES)
            {
               /*  FLOG_ERROR(
                       ("Timeout, force stop waitForInitialDiscovery: "
                       + std::string(" ,pending m_events =") +
                  std::string(m_events.size())
                       + std::string(" ,retryAttampts =") +
                  std::string(retryAttampts)
                       ).c_str()
                    );*/
               break; // discovery taking longer than 10 sec force stop
                      // something is not right
            }
         }
         ++retryAttampts;
      }
   }

   const std::string STRING_PATTERN_EDL = ".*?EDL|QDLoader.*?";
   const std::string STRING_PATTERN_QCDM = ".*?QCDM.*?";
   const std::string STRING_PATTERN_MHI = ".*?MHI.*?";
   bool isPatternFoundInDescription(const std::string& description, const std::string& pattern)
   {
      std::smatch results;
      return std::regex_search(description, results, std::regex(pattern));
   }

   static std::pair<std::string, std::string> getVidPid(const DeviceManagerHelper::DeviceEvent& currentEvent)
   {
      std::string vidMatchString = USB_VID;
      std::string pidMatchString = USB_PID;
      try
      {
         if(std::string::npos != currentEvent.m_hwId.find(PCI_MATCH))
         {
            vidMatchString = PCI_VID;
            pidMatchString = PCI_PID;
         }

         size_t locBackslash = currentEvent.m_hwId.find("\\"); // Example m_hwId  =
                                                               // "USB\VID_05C6&PID_90DB&REV_0504&MI_03&CID_042A"

         TOOLS_ASSERT_OR_RETURN(std::string::npos != locBackslash, std::make_pair(std::string(), std::string()););

         std::string hwIdTruncated = currentEvent.m_hwId.substr(locBackslash + 1); // remove USB\ or PCI\ prefix

         std::vector<std::string> hwIdEntries = Util::split(hwIdTruncated, '&');

         std::vector<std::string>::const_iterator it = hwIdEntries.begin();
         std::vector<std::string>::const_iterator end = hwIdEntries.end();

         std::unordered_map<std::string, std::string> hwIdEntriesMap;

         for(; end != it; ++it)
         {
            std::vector<std::string> entries = Util::split(*it, '_');
            if(entries.size() == 2)
            {
               hwIdEntriesMap[entries[0]] = entries[1];
            }
         }

         std::string vid, pid;
         std::unordered_map<std::string, std::string>::const_iterator itVid = hwIdEntriesMap.find(vidMatchString);
         if(hwIdEntriesMap.end() != itVid)
         {
            vid = itVid->second;
         }

         std::unordered_map<std::string, std::string>::const_iterator itPid = hwIdEntriesMap.find(pidMatchString);
         if(hwIdEntriesMap.end() != itPid)
         {
            pid = itPid->second;
         }

         return std::make_pair(vid, pid);
      }
      TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
      return std::make_pair(std::string(), std::string());
   }

   std::string getEdlChipId(const DeviceManagerHelper::DeviceEvent& currentEvent)
   {
      // Example: "USB\VID_05C6&PID_90DB&REV_0504&MI_03&CID_042A"
      size_t cidOffset = currentEvent.m_hwId.find(USB_CID);
      if(std::string::npos == cidOffset)
      {
         return std::string();
      }

      std::string cidTruncated = currentEvent.m_hwId.substr(cidOffset + USB_CID.size());
      size_t cidEnd = cidTruncated.find("&");
      if(std::string::npos == cidEnd)
      {
         return cidTruncated;
      }
      else
      {
         return cidTruncated.substr(0, cidEnd - 1);
      }
   }

   virtual void onRun()
   {
      FLOG_INFO("DeviceManagerHelper::onRun, initialDiscovery start");
      m_threadId = Util::ThisThread::getId();
      Device::Protocol::BasePtr pStateChangeProtocol;
      Device::Protocol::Base::State newState = Device::Protocol::Base::STATE_DISCONNECTED;
      auto initialDiscoveryTime = std::chrono::system_clock::now();
      auto previousDiscoveryTime = std::chrono::system_clock::now();
      while(!isStopSignaled())
      {
         try
         {
            if(pStateChangeProtocol != nullptr)
            {
               try
               {
                  pStateChangeProtocol->setState(newState);
               }
               TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
               pStateChangeProtocol = NULL;
            }

            bool bEmpty = true;
            {
               std::lock_guard<std::recursive_mutex> lock(m_mutex);
               bEmpty = m_events.empty();
            }
            if(bEmpty)
            {
               // Finish initial discovery earlier if there is no discovery
               // event at all. This is to prevent failure during
               // startup which also has a 10sec timeout.
               if(!m_bInitialComplete &&
                  std::chrono::system_clock::now() - initialDiscoveryTime > INITIAL_DISCOVERY_WAIT_PERIOD_NO_EVENT)
               {
                  m_bInitialComplete = true;
                  m_initialDiscovery.signal();
               }

               // If nothing in queue, keep waiting for new items
               Util::ThisThread::waitForEvent(&m_discoverEvent, DISCOVERY_WAIT_PERIOD);

               continue;
            }

            auto timeSinceLastDiscovered =
               std::chrono::system_clock::now() - previousDiscoveryTime;
            if (timeSinceLastDiscovered < DISCOVERY_WAIT_PERIOD)
            {
               // Sometimes opening the port too early after discovery
               // causes issues try waiting at least 50 ms before
               // opening
               Util::ThisThread::sleep(std::chrono::duration_cast<
                  std::chrono::milliseconds>(DISCOVERY_WAIT_PERIOD - timeSinceLastDiscovered)
               );
            }
            previousDiscoveryTime = std::chrono::system_clock::now();

            Device::DeviceManagerHelper::DeviceEvent currentEvent;
            {
               std::lock_guard<std::recursive_mutex> lock(m_mutex);
               FLOG_INFO(("Device Discovery Events Total: " + std::string(std::to_string(m_events.size()))).c_str());
               currentEvent = m_events.front();
               m_events.pop_front();
               if(!currentEvent.m_initialProcessingTime.has_value())
               {
                  currentEvent.m_initialProcessingTime = std::chrono::system_clock::now();
               }
            }

            std::string description = currentEvent.m_description;
#ifdef TOOLS_TARGET_WINDOWS
            if((currentEvent.m_flags & QC_FLAG_MASK_DEV_TYPE) >> 8 == QC_DEV_TYPE_PORTS &&
               std::string::npos == description.find("(COM"))
            {
               // If this is a port and it does not include "(COMX)" in the
               // description, then it is about to be renamed and rediscovered;
               // ignore this time because it will soon come back with the
               // "(COMX)"

               FLOG_INFO(("Ignoring incomplete port: " + description + " : " + currentEvent.m_location).c_str());
               continue;
            }
#endif

            UCHAR devState = (UCHAR)((currentEvent.m_flags & QC_FLAG_MASK_DEV_STATE) >> 4);
            UCHAR devType = (UCHAR)((currentEvent.m_flags & QC_FLAG_MASK_DEV_TYPE) >> 8);
            UCHAR devProtocol = (UCHAR)(currentEvent.m_protocol & QC_FLAG_MASK_DEV_PROTOCOL);
            // bool bQcDriver = !!(currentEvent.m_flags &
            // QC_FLAG_MASK_QC_DRIVER); // Unused variable

            std::string parentDescription = currentEvent.m_parentDevice;
            std::string uniqueIdentifier;

            // Use bus location as unique identifier if it is valid, otherwise
            // use the common path at the beginning of device path.
            if(std::string::npos != currentEvent.m_location.find_first_not_of(std::string(".0")))
            {
               uniqueIdentifier = currentEvent.m_location;
            }

            std::string serialNumberMsm = currentEvent.m_serialNumberMsm;
            std::string serialNumberAdb = currentEvent.m_serialNumberAdb;
            std::string socVersion = currentEvent.m_socVersion;

            bool bNotifyDevice = false;

            bool bEventCanceled = false;
            {
               std::lock_guard<std::recursive_mutex> lock(m_mutex);
               for(DeviceEventQueue::iterator it = m_events.begin(); it != m_events.end(); ++it)
               {
                  // Cancel arrival/departure pair except for reversed
                  // departure/arrival instance
                  UCHAR nextEventDevState = (UCHAR)((it->m_flags & QC_FLAG_MASK_DEV_STATE) >> 4);
                  if(((currentEvent.m_bIsPushbackEvent && !currentEvent.m_bIsPushbackRearrival &&
                       QC_DEV_STATE_ARRIVAL == devState && QC_DEV_STATE_DEPARTURE == nextEventDevState) ||
                      (it->m_bIsPushbackEvent && !it->m_bIsPushbackRearrival &&
                       QC_DEV_STATE_ARRIVAL == nextEventDevState && QC_DEV_STATE_DEPARTURE == devState)) &&
                     it->m_description == currentEvent.m_description && it->m_deviceName == currentEvent.m_deviceName &&
                     it->m_parentDevice == currentEvent.m_parentDevice &&
                     (it->m_serialNumberMsm == currentEvent.m_serialNumberMsm || m_bIgnoreSerNum) &&
                     (it->m_serialNumberAdb == currentEvent.m_serialNumberAdb || m_bIgnoreSerNum) &&
                     it->m_location == currentEvent.m_location && it->m_protocol == currentEvent.m_protocol)
                  {
                     FLOG_INFO(("Cancel arrival/departure events: " + currentEvent.m_description + " : " +
                                currentEvent.m_location)
                                  .c_str());
                     m_events.erase(it);
                     bEventCanceled = true;
                     break;
                  }
               }

               // Certain interfaces cannot be supported at this time
               if(isBlacklisted(currentEvent.m_description))
               {
                  FLOG_INFO(("Skip unsupported interface: " + currentEvent.m_description + " : " +
                             currentEvent.m_location)
                               .c_str());
                  continue;
               }

               if(bEventCanceled)
               {
                  continue;
               }
            }

            if(QC_DEV_STATE_ARRIVAL == devState)
            {
               auto stopWatchStartTime = std::chrono::steady_clock::now();
               Communication::CommonIoPtr pNewInterface;

               ImplPtr pDevice;
               Protocol::BasePtr pProtocol;
               std::string deviceDescription = parentDescription;
               if(deviceDescription.empty())
               {
                  if(uniqueIdentifier.empty())
                  {
                     // This means it's not a composite device; just use the
                     // protocol description
                     deviceDescription = currentEvent.m_description;
                  }
                  else
                  {
                     deviceDescription = "USB Location: " + uniqueIdentifier;
                  }
               }

               {
                  std::lock_guard<std::recursive_mutex> lock(Manager::getInstance()->m_deviceManagerMutex);

                  FLOG_INFO(
                     ("DeviceManagerHelper::onRun: Device Arrival: "
                      "Check match within connected devices, desc: " +
                      currentEvent.m_description + ", parentdesc: " + parentDescription)
                  );
                  Manager::DeviceList::const_iterator it = getMatch(
                     Manager::getInstance()->m_devices,
                     Manager::getInstance()->m_disconnectedDevices,
                     currentEvent,
                     uniqueIdentifier,
                     parentDescription,
                     serialNumberMsm,
                     serialNumberAdb
                  );

                  if(Manager::getInstance()->m_devices.end() != it)
                  {
                     pDevice = it->second;
                     if(!currentEvent.m_bIsPushbackEvent)
                     {
                        FLOG_INFO(("Matched existing device: " + pDevice->getDescription() + " : " + serialNumberMsm +
                                   " : " + serialNumberAdb + " : " + currentEvent.m_description)
                                     .c_str());
                     }

                     if(currentEvent.m_bIsPushbackEvent && !currentEvent.m_bIsPushbackConnected)
                     {
                        FLOG_INFO(("Pushback event device connected: " + currentEvent.m_description + " : " +
                                   currentEvent.m_location)
                                     .c_str());
                        currentEvent.m_bIsPushbackConnected = true;
                     }
                  }

                  // Available protocol with the identical description should
                  // not arrive again before departure. If this happens it is
                  // most likely a reversed departure/arrival sequence. Pushback
                  // arrival event until departure event is received and
                  // processed.
                  if(m_bInitialComplete && pDevice != nullptr &&
                     isRearrival(pDevice, currentEvent.m_description, currentEvent.m_deviceName))
                  {
                     if(!currentEvent.m_bIsPushbackRearrival ||
                        std::chrono::system_clock::now() - *currentEvent.m_initialProcessingTime <
                           REVERSED_DEPARTURE_ARRIVAL_WAIT)
                     {
                        currentEvent.m_bIsPushbackRearrival = true;
                        pushBackEvent(currentEvent, pDevice, false);
                        FLOG_INFO(("Pushback rearrival: " + currentEvent.m_description + " : " + currentEvent.m_location
                        )
                                     .c_str());
                     }
                     else
                     {
                        FLOG_INFO(("Pushback rearrival timed out: " + currentEvent.m_description + " : " +
                                   currentEvent.m_location)
                                     .c_str());
                     }
                     continue;
                  }

                  // Do not allow new protocol to be added when device is in
                  // certain mode, e.g. EDL/Memory Dump Collection
                  if(m_bInitialComplete && pDevice != nullptr && isArrivalProhibited(pDevice))
                  {
                     pushBackEvent(currentEvent, pDevice, false);
                     FLOG_INFO(("Pushback arrival prohibited: " + currentEvent.m_description + " : " +
                                currentEvent.m_location)
                                  .c_str());
                     continue;
                  }

                  // EDL should be the only interface present when it
                  // enumerates, but occasionally it may enumerate before device
                  // departure. If EDL is not the first protocol on this device,
                  // wait until all other protocols are departed.
                  if(m_bInitialComplete && pDevice != nullptr && 0 != pDevice->getProtocolCount() &&
                     (std::chrono::system_clock::now() - *currentEvent.m_initialProcessingTime
                     ) < SAHARA_OTHER_PROTOCOL_WAIT &&
                     ((DEV_PROTOCOL_SAHARA_PBL_EMERGENCY_DOWNLOAD == devProtocol ||
                       DEV_PROTOCOL_SAHARA_PBL_FLASHLESS_BOOT == devProtocol || DEV_PROTOCOL_SAHARA == devProtocol ||
                       DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                      isPatternFoundInDescription(parentDescription, STRING_PATTERN_EDL)))
                  {
                     pushBackEvent(currentEvent, pDevice, false);
                     FLOG_INFO(("Pushback EDL: " + currentEvent.m_description + " : " + currentEvent.m_location).c_str()
                     );
                     continue;
                  }

                  if(pDevice == nullptr && currentEvent.m_bIsPushbackConnected &&
                     !((DEV_PROTOCOL_SAHARA_PBL_EMERGENCY_DOWNLOAD == devProtocol ||
                        DEV_PROTOCOL_SAHARA_PBL_FLASHLESS_BOOT == devProtocol || DEV_PROTOCOL_SAHARA == devProtocol ||
                        DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                       isPatternFoundInDescription(parentDescription, STRING_PATTERN_EDL)))
                  {
                     FLOG_INFO(("Pushback event expired: " + currentEvent.m_description + " : " +
                                currentEvent.m_location)
                                  .c_str());
                     continue;
                  }

                  bool bPushbackWithoutSerialNumber = false;
                  if(pDevice == nullptr)
                  {
                     FLOG_INFO(
                        ("DeviceManagerHelper::onRun: Device Arrival: "
                         "Check "
                         "match within disconnected devices, desc: " +
                         currentEvent.m_description + ", parentdesc: " + parentDescription)
                     );
                     Manager::DeviceList::const_iterator itD = getMatch(
                        Manager::getInstance()->m_disconnectedDevices,
                        Manager::getInstance()->m_devices,
                        currentEvent,
                        uniqueIdentifier,
                        parentDescription,
                        serialNumberMsm,
                        serialNumberAdb
                     );

                     if(Manager::getInstance()->m_disconnectedDevices.end() != itD)
                     {
                        auto pDeviceDisconnected = itD->second;
                        FLOG_INFO(("Matched disconnected device: " + pDeviceDisconnected->getDescription() + " : " +
                                   serialNumberMsm + " : " + serialNumberAdb + " : " + currentEvent.m_description)
                                     .c_str());

                        if(serialNumberMsm.empty() && serialNumberAdb.empty() &&
                           (std::chrono::system_clock::now() - *currentEvent.m_initialProcessingTime
                           ) < SIBLING_PROTOCOL_WAIT)
                        {
                           bPushbackWithoutSerialNumber = true;
                        }
                        else
                        {
                           ImplPtr pDeviceCopy = pDevice; // keep a copy to check for dup device
                                                          // when port is switched
                           pDevice = pDeviceDisconnected;

                           // update current uniqueIdentifier in case device
                           // location changed, except for EDL or other
                           // non-composite mode interfaces that has
                           // imcompitible location format
                           if(pDevice->getUniqueIdentifier().empty() ||
                              (std::string::npos == uniqueIdentifier.find("Port_#") &&
                               std::string::npos == uniqueIdentifier.find("Hub_#")))
                           {
                              FLOG_INFO(("Update uniqueIdentifier: " + uniqueIdentifier +
                                         " in device: " + std::to_string(pDevice->getHandle()))
                                           .c_str());
                              pDevice->setUniqueIdentifier(uniqueIdentifier);
                           }

                           Manager::getInstance()->m_disconnectedDevices.erase(itD);
                           Manager::getInstance()->addDevice(pDevice);
                           FLOG_INFO(
                              ("==-==>>>> Device reconnect " + std::to_string(pDevice->getHandle()) +
                               " : "
                               "uniqueIdentifier: " +
                               pDevice->getUniqueIdentifier() +
                               " : "
                               "parentDescription: " +
                               pDevice->getDescription() +
                               " : "
                               "devicePath: " +
                               pDevice->getDevicePath() +
                               " : "
                               "serialNumberMsm: " +
                               pDevice->getSerialNumberMsm() +
                               " : "
                               "serialNumberAdb: " +
                               pDevice->getSerialNumberAdb() +
                               " : "
                               "vid: " +
                               pDevice->getVid() +
                               " : "
                               "pid: " +
                               pDevice->getPid() +
                               " : "
                               "socVersion: " +
                               pDevice->getSocVersion())
                                 .c_str()
                           );

                           bNotifyDevice = true;

                           // we matched to connected device, which means we
                           // should have already removed the disconnect
                           // reference unless previous protocol(s) had no SN to
                           // match so we should remove it and reuse
                           // disconnected device instead
                           if(pDeviceCopy != nullptr)
                           {
                              // update list of protocols from unmatched device,
                              // then remove it
                              bool bExceptionOccurred = false;
                              ToolException tempException;

                              Device::Impl::ProtocolList protocolList = pDeviceCopy->getProtocolList();
                              for(auto& pProtocolToMove: protocolList)
                              {
                                 FLOG_INFO(("Processing protocol = " + pProtocolToMove->getDescription()).c_str());

                                 // remove from dup device
                                 pDeviceCopy->removeProtocol(pProtocolToMove);
                                 try
                                 {
                                    bExceptionOccurred = false;
                                    Manager::getInstance()->m_deviceManagerMutex.unlock();
                                    FLOG_INFO("notifyAsync ProtocolRemovedEvent = " + pProtocolToMove->getDescription());
                                    m_pManager->notifyAsync(std::make_shared<
                                                            ProtocolRemovedEvent>(pDeviceCopy, pProtocolToMove));
                                    Manager::getInstance()->m_deviceManagerMutex.lock();
                                 }
                                 TOOLS_CATCH(e, bExceptionOccurred = true; tempException = e;
                                             // LOG_EXCEPTION( e);
                                             Manager::getInstance()->m_deviceManagerMutex.lock(););
                                 if(bExceptionOccurred)
                                 {
                                    TOOLS_THROW(tempException); // rethrow
                                 }

                                 // add to actual device

                                 pDevice->addProtocol(pProtocolToMove);
                                 FLOG_INFO(("==-==>>>> Adding protocol total " +
                                            std::string(std::to_string(pDevice->getProtocolCount())) + " : " +
                                            pProtocolToMove->getDescription())
                                              .c_str());

                                 try
                                 {
                                    bExceptionOccurred = false;
                                    Manager::getInstance()->m_deviceManagerMutex.unlock();
                                    FLOG_INFO(("notifyAsync ProtocolAddedEvent "
                                               "= " +
                                               pProtocolToMove->getDescription())
                                                 .c_str());
                                    m_pManager
                                       ->notifyAsync(std::make_shared<ProtocolAddedEvent>(pDevice, pProtocolToMove));
                                    Manager::getInstance()->m_deviceManagerMutex.lock();
                                 }
                                 TOOLS_CATCH(e, bExceptionOccurred = true; tempException = e;
                                             // LOG_EXCEPTION( e);
                                             Manager::getInstance()->m_deviceManagerMutex.lock(););
                                 if(bExceptionOccurred)
                                 {
                                    TOOLS_THROW(tempException); // rethrow
                                 }
                              }
                              if(pDeviceCopy != nullptr && 0 < pDeviceCopy->m_protocols.size())
                              {
                                 FLOG_ERROR(("All protocols in the temp "
                                             "DeviceCopy were not cleared = " +
                                             pDeviceCopy->getDescription())
                                               .c_str());
                              }
                              try
                              {
                                 bExceptionOccurred = false;
                                 Manager::getInstance()->m_deviceManagerMutex.unlock();
                                 FLOG_INFO(("notifyAsync DeviceDisconnectEvent "
                                            "= " +
                                            pDeviceCopy->getDescription())
                                              .c_str());
                                 m_pManager->notifyAsync(std::make_shared<DeviceDisconnectEvent>(pDeviceCopy));
                                 Manager::getInstance()->m_deviceManagerMutex.lock();
                              }
                              TOOLS_CATCH(e, bExceptionOccurred = true; tempException = e;
                                          // LOG_EXCEPTION( e);
                                          Manager::getInstance()->m_deviceManagerMutex.lock(););
                              if(bExceptionOccurred)
                              {
                                 TOOLS_THROW(tempException); // rethrow
                              }
                              Manager::getInstance()->removeConnectedDevice(pDeviceCopy->getHandle());
                           }
                        }
                     }
                  }


                  // Always wait for siblings to confirm serial number,
                  // otherwise protocol may be grouped into a wrong device if OS
                  // is configured to ignore serial number
                  if(bPushbackWithoutSerialNumber)
                  {
                     FLOG_INFO(("Pushback protocol without serial number: " + currentEvent.m_description + " : " +
                                currentEvent.m_location)
                                  .c_str());
                     pushBackEvent(currentEvent, pDevice, false);
                     continue;
                  }

                  if(pDevice == nullptr)
                  {
                     DeviceHandle handle = Manager::getInstance()->m_nextDeviceHandle;
                     Manager::getInstance()->m_nextDeviceHandle += DEVICE_HANDLE_INCREMENT;

                     pDevice = std::make_shared<Impl>(handle, deviceDescription, uniqueIdentifier);
                     pDevice->setDevicePhysicalLocation(currentEvent.m_parentLocationInfomation);
                     Manager::getInstance()->addDevice(pDevice);

                     FLOG_INFO(("==-==>>>> New device connected: " + std::string(std::to_string(handle)) + " : " +
                                deviceDescription + " : " + uniqueIdentifier)
                                  .c_str());
                     bNotifyDevice = true;
                  }
                  else
                  {
                     // ADB or EDL interface may have unreliable parent name. Do
                     // not override.
                     if(!parentDescription.empty() &&
                        std::string::npos == std::string(parentDescription).find("ADB ") &&
                        std::string::npos == std::string(parentDescription).find(" KDB ") &&
                        !(DEV_PROTOCOL_SAHARA_PBL_EMERGENCY_DOWNLOAD == devProtocol ||
                          DEV_PROTOCOL_SAHARA_PBL_FLASHLESS_BOOT == devProtocol ||
                          ((DEV_PROTOCOL_SAHARA == devProtocol || DEV_PROTOCOL_UNKNOWN == devProtocol ||
                            DEV_PROTOCOL_RESERVED == devProtocol) &&
                           isPatternFoundInDescription(parentDescription, STRING_PATTERN_EDL))))
                     {
                        FLOG_INFO(("Update parentDescription: " + parentDescription +
                                   " in device: " + std::string(std::to_string(pDevice->getHandle())))
                                     .c_str());
                        pDevice->setDescription(parentDescription);
                     }
                     // update parentLocationInfomation when the same device
                     // with different ports.
                     pDevice->setDevicePhysicalLocation(currentEvent.m_parentLocationInfomation);
                  }

                  if(pDevice != nullptr && (QC_DEV_TYPE_TAC != devType && QC_DEV_TYPE_EPM != devType))
                  {
                     std::pair<std::string, std::string> pairVidPid = getVidPid(currentEvent);
                     pDevice->setVid(pairVidPid.first);
                     pDevice->setPid(pairVidPid.second);

                     pDevice->setEdlChipId(getEdlChipId(currentEvent));
                     pDevice->setHwId(currentEvent.m_hwId.empty() ? std::string() : std::string(currentEvent.m_hwId));
                  }
                  if(!serialNumberMsm.empty() ||
                     // When Device is in ADB Recovery mode, serialNumberMsm is
                     // empty and it should be updated as well. Otherwise, It
                     // will show older serialNumberMsm.
                     (std::string::npos != std::string(parentDescription).find("ADB Recovery")))
                  {
                     FLOG_INFO(("Update serialNumberMsm: " + serialNumberMsm +
                                " in device: " + std::string(std::to_string(pDevice->getHandle())))
                                  .c_str());
                     pDevice->setSerialNumberMsm(serialNumberMsm);
                  }
                  if(!socVersion.empty())
                  {
                     FLOG_INFO(("Update SocVersion: " + socVersion +
                                " in device: " + std::string(std::to_string(pDevice->getHandle())))
                                  .c_str());
                     pDevice->setSocVersion(socVersion);
                  }
                  if(!serialNumberAdb.empty())
                  {
                     FLOG_INFO(("Update serialNumberAdb: " + serialNumberAdb +
                                " in device: " + std::string(std::to_string(pDevice->getHandle())))
                                  .c_str());
                     pDevice->setSerialNumberAdb(serialNumberAdb);
                  }

                  if((pDevice->getDevicePath().empty() && !currentEvent.m_devicePath.empty()) ||
                     (!pDevice->getDevicePath().empty() && !currentEvent.m_devicePath.empty() &&
                      pDevice->getDevicePath().size() >= currentEvent.m_devicePath.size()))
                  {
                     FLOG_INFO(("Update devicePath: " + currentEvent.m_devicePath +
                                " in device: " + std::string(std::to_string(pDevice->getHandle())))
                                  .c_str());
                     pDevice->setDevicePath(currentEvent.m_devicePath);
                  }

                  currentEvent.m_bIsPushbackEvent = false;
                  Impl::ProtocolList::const_iterator itP = pDevice->m_unavailableProtocols.begin();
                  Impl::ProtocolList::const_iterator endP = pDevice->m_unavailableProtocols.end();
                  for(; itP != endP; ++itP)
                  {
                     Communication::CommonIoPtr pComm = (*itP)->getCommonIo();

                     if(pComm != nullptr && isIdentifierMatched(pComm->getIdentifier(), currentEvent.m_deviceName))
                     {
                        bool bMatch = true;
#ifndef TOOLS_TARGET_WINDOWS
                        // Update identifier in case it has changed with a new
                        // index upon arrival
                        if(pComm->getIdentifier() != currentEvent.m_deviceName)
                        {
                           FLOG_ERROR(("Update identifier from: " + pComm->getIdentifier() +
                                       " to: " + currentEvent.m_deviceName)
                                         .c_str());
                           pComm->setIdentifier(currentEvent.m_deviceName);
                        }
#endif

                        pStateChangeProtocol = *itP;
                        newState = Protocol::Base::STATE_INITIALIZING;

                        if(DEV_PROTOCOL_SAHARA == (UCHAR)(devProtocol & QC_FLAG_MASK_DEV_PROTOCOL_CATEGORY))
                        {
                           if((*itP).dynamicCast<Protocol::Sahara>() != nullptr)
                           {
                              pProtocol = *itP;
                           }
                           else
                           {
                              bMatch = false;
                           }
                        }
                        else if(((DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                                 (std::string::npos != currentEvent.m_description.find("Diagnostics") ||
                                  std::string::npos != currentEvent.m_description.find("DIAG"))))
                        {
                           if(0 == pDevice->getProtocolCount() &&
                              (std::chrono::system_clock::now() - *currentEvent.m_initialProcessingTime
                              ) < SAHARA_OTHER_PROTOCOL_WAIT)
                           {
                              // If this is the first protocol on this device,
                              // wait to see if other protocols enumerate.  If
                              // no other protocols come we can assume this is
                              // Sahara, otherwise it is definitely Diag
                              pushBackEvent(currentEvent, pDevice, true);
                              pStateChangeProtocol = NULL;
                              newState = Device::Protocol::Base::STATE_DISCONNECTED;
                              break;
                           }

                           if(0 == pDevice->getProtocolCount())
                           {
                              // For this case it has already waited for other
                              // protocols to arrive, if nothing else did assume
                              // it's in Sahara mode
                              if((*itP).dynamicCast<Protocol::Sahara>() != nullptr)
                              {
                                 pProtocol = *itP;
                              }
                              else
                              {
                                 bMatch = false;
                              }
                           }
                        }
                        else
                        {
                           // Never reuse the firehose protocol on arrival
                           if((*itP).dynamicCast<Protocol::Firehose>() != nullptr)
                           {
                              bMatch = false;
                           }
                           else
                           {
                              pProtocol = *itP;
                           }
                        }

                        if(bMatch)
                        {
                           break;
                        }
                        else
                        {
                           FLOG_ERROR(("Skip protocol with different protocol "
                                       "type: " +
                                       currentEvent.m_deviceName)
                                         .c_str());
                           pStateChangeProtocol = NULL;
                           newState = Device::Protocol::Base::STATE_DISCONNECTED;
                        }
                     }
                  }

                  if(currentEvent.m_bIsPushbackEvent)
                  {
                     continue;
                  }

                  if(pProtocol == nullptr)
                  {
                     // Use device protocol information to decide communication
                     // IO when available, otherwise make best guess based on
                     // other device parmeters
                     switch(devProtocol)
                     {
                        case DEV_PROTOCOL_RMNET:
                           break;
                        case DEV_PROTOCOL_DIAG:
                        case DEV_PROTOCOL_SAHARA:
                        case DEV_PROTOCOL_SAHARA_PBL_EMERGENCY_DOWNLOAD:
                        case DEV_PROTOCOL_SAHARA_PBL_FLASHLESS_BOOT:
                        case DEV_PROTOCOL_SAHARA_SBL_XBL_RAMDUMP:
                        case DEV_PROTOCOL_SAHARA_TN_APPS_Remote_EFS:
                        case DEV_PROTOCOL_FIREHOSE:
                        case DEV_PROTOCOL_DUN:
                        case DEV_PROTOCOL_NMEA:
                        case DEV_PROTOCOL_QDSS:
                        case DEV_PROTOCOL_ADPL:
                           pNewInterface = Communication::Usb::create(
                              currentEvent.m_description,
                              currentEvent.m_deviceName,
                              currentEvent.m_location,
                              serialNumberMsm
                           );
                           break;
                        default:
                           if(devType == QC_DEV_TYPE_NET)
                           {
                           }
                           else if(QC_DEV_TYPE_MBIM == devType || QC_DEV_TYPE_RNDIS == devType)
                           {
                           }
                           else if(std::string::npos != std::string(currentEvent.m_deviceName).find("ADB ") ||
                                   std::string::npos != std::string(currentEvent.m_deviceName).find(" KDB ") ||
                                   std::string::npos != std::string(currentEvent.m_deviceName).find("ADB-IP:"))
                           {
                              if(0 == pDevice->getProtocolCount() &&
                                 (std::chrono::system_clock::now() - *currentEvent.m_initialProcessingTime
                                 ) < SIBLING_PROTOCOL_WAIT)
                              {
                                 // If this is the first protocol on this
                                 // device, wait to see if other protocols
                                 // enumerate.  This may be a reconnect, but ADB
                                 // does not always properly give device info,
                                 // so we can't tell if it's from a device
                                 // that's been previously connected.  Wait for
                                 // a more reliable protocol to enumerate if one
                                 // will come
                                 pushBackEvent(currentEvent, pDevice, true);
                                 continue;
                              }
                              pNewInterface = Communication::Usb::create(
                                 currentEvent.m_description,
                                 currentEvent.m_deviceName,
                                 currentEvent.m_location,
                                 serialNumberMsm
                              );
                           }
                           else if(std::string::npos !=
                                      std::string(currentEvent.m_deviceName)
                                         .find("Android Bootloader "
                                               "Interface") ||
                                   std::string::npos != std::string(currentEvent.m_deviceName).find("fastboot"))
                           {
                           }
                           else
                           {
                              if(0 == pDevice->getProtocolCount() &&
                                 ((DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                                  (std::string::npos != currentEvent.m_description.find("Diagnostics") ||
                                   std::string::npos != currentEvent.m_description.find("DIAG"))) &&
                                 (std::chrono::system_clock::now() - *currentEvent.m_initialProcessingTime
                                 ) < SAHARA_OTHER_PROTOCOL_WAIT)
                              {
                                 // If this is the first protocol on this
                                 // device, wait to see if other protocols
                                 // enumerate.  If no other protocols come we
                                 // can assume this is Sahara, otherwise it is
                                 // definitely Diag
                                 pushBackEvent(currentEvent, pDevice, true);
                                 continue;
                              }


                              pNewInterface = Communication::Usb::create(
                                 currentEvent.m_description,
                                 currentEvent.m_deviceName,
                                 currentEvent.m_location,
                                 serialNumberMsm
                              );
                           }
                           break;
                     }

                     if(!pNewInterface)
                     {
                        // either ignore the event, log a warning, or create a
                        // fallback object
                        FLOG_WARNING(
                           "No interface created for device" + currentEvent.m_description +
                           ", skipping protocol creation"
                        );
                        continue; // or break out of the loop
                     }

                     std::string ifaceDesc = pNewInterface->getDescription();

                     if((DEV_PROTOCOL_UNKNOWN == devProtocol) &&
                             isPatternFoundInDescription(currentEvent.m_description, STRING_PATTERN_SAHARA) &&
                             isPatternFoundInDescription(parentDescription, STRING_PATTERN_MHI))
                     {
                        pProtocol = Util::SharedPointer<
                           Protocol::Sahara>::create(pNewInterface, Protocol::Sahara::MODE_MEMORY_DEBUG);

                        FLOG_INFO(("Discovered MHI Sahara protocol: " + ifaceDesc + " : " + currentEvent.m_location)
                                     .c_str());
                     }
                     else if((DEV_PROTOCOL_UNKNOWN == devProtocol) &&
                             isPatternFoundInDescription(currentEvent.m_description, STRING_PATTERN_FIREHOSE) &&
                             isPatternFoundInDescription(parentDescription, STRING_PATTERN_MHI))
                     {
                        pProtocol = Util::SharedPointer<
                           Protocol::Sahara>::create(pNewInterface, Protocol::Sahara::Mode::MODE_IMAGE_TX_PENDING);

                        FLOG_INFO(("Discovered MHI EDL protocol: " + ifaceDesc + " : " + currentEvent.m_location)
                                     .c_str());
                     }
                     else if(((DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                              (std::string::npos != ifaceDesc.find("Diagnostics") ||
                               std::string::npos != ifaceDesc.find("DIAG"))))
                     {
                        if(0 == pDevice->getProtocolCount())
                        {
                           // For this case it has already waited for other
                           // protocols to arrive, if nothing else did assume
                           // it's in Sahara mode
                           pProtocol = Util::SharedPointer<
                              Protocol::Sahara>::create(pNewInterface, Protocol::Sahara::MODE_MEMORY_DEBUG);

                           FLOG_ERROR(("Discovered Sahara protocol: " + ifaceDesc + " : " + currentEvent.m_location)
                                         .c_str());
                        }
                     }
                     else if(DEV_PROTOCOL_UNKNOWN == (devProtocol & DEV_PROTOCOL_MAJOR_REVISION_MASK) &&
                             (std::string::npos != ifaceDesc.find("Android Bootloader Interface") ||
                              std::string::npos != ifaceDesc.find("fastboot")))
                     {
                        FLOG_INFO(("Discovered Fastboot protocol: " + ifaceDesc + " : " + currentEvent.m_location)
                                     .c_str());
                     }
                     else if(DEV_PROTOCOL_SAHARA_SBL_XBL_RAMDUMP == devProtocol ||
                             (DEV_PROTOCOL_SAHARA == devProtocol &&
                              (std::string::npos != ifaceDesc.find("Diagnostics") ||
                               std::string::npos != ifaceDesc.find("DIAG"))))
                     {
                        pProtocol = Util::SharedPointer<
                           Protocol::Sahara>::create(pNewInterface, Protocol::Sahara::MODE_MEMORY_DEBUG);

                        FLOG_INFO(("Discovered Ramdump Sahara protocol: " + ifaceDesc + " : " + currentEvent.m_location)
                                     .c_str());
                     }
                     else if(DEV_PROTOCOL_SAHARA_TN_APPS_Remote_EFS == devProtocol ||
                             ((DEV_PROTOCOL_SAHARA == devProtocol || DEV_PROTOCOL_UNKNOWN == devProtocol ||
                               DEV_PROTOCOL_RESERVED == devProtocol) &&
                              (std::string::npos != ifaceDesc.find(" SER4") ||
                               std::string::npos != ifaceDesc.find("_SER4"))))
                     {
                        pProtocol = Util::SharedPointer<
                           Protocol::Sahara>::create(pNewInterface, Protocol::Sahara::Mode::MODE_EFS_SYNC);
                        FLOG_INFO(("Discovered Remote EFS Sync Sahara "
                                   "protocol: " +
                                   ifaceDesc + " : " + currentEvent.m_location)
                                     .c_str());
                     }
                     else if(DEV_PROTOCOL_SAHARA_PBL_EMERGENCY_DOWNLOAD == devProtocol ||
                             DEV_PROTOCOL_SAHARA_PBL_FLASHLESS_BOOT == devProtocol ||
                             ((DEV_PROTOCOL_SAHARA == devProtocol || DEV_PROTOCOL_UNKNOWN == devProtocol ||
                               DEV_PROTOCOL_RESERVED == devProtocol) &&
                              isPatternFoundInDescription(ifaceDesc, STRING_PATTERN_EDL)))
                     {
                        pProtocol = Util::SharedPointer<
                           Protocol::Sahara>::create(pNewInterface, Protocol::Sahara::Mode::MODE_IMAGE_TX_PENDING);

                        FLOG_INFO(("Discovered EDL Sahara protocol: " + ifaceDesc + " : " + currentEvent.m_location)
                                     .c_str());
                     }
                     else if(DEV_PROTOCOL_ADPL == devProtocol ||
                             ((DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                              std::string::npos != ifaceDesc.find("DPL")))
                     {
                        FLOG_INFO(("Discovered Adpl protocol: " + ifaceDesc + " : " + currentEvent.m_location).c_str());
                     }
                     else if(DEV_PROTOCOL_QDSS == devProtocol ||
                             ((DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                              std::string::npos != ifaceDesc.find("QDSS")))
                     {
                        FLOG_INFO(("Discovered QDSS protocol: " + ifaceDesc + " : " + currentEvent.m_location).c_str());
                     }
                     else if(DEV_PROTOCOL_NMEA == devProtocol ||
                             ((DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                              std::string::npos != ifaceDesc.find("NMEA")))
                     {
                        FLOG_INFO(("Discovered NMEA protocol: " + ifaceDesc + " : " + currentEvent.m_location).c_str());
                     }
                     else if(((DEV_PROTOCOL_UNKNOWN == devProtocol || DEV_PROTOCOL_RESERVED == devProtocol) &&
                              std::string::npos != ifaceDesc.find("EUD")))
                     {
                        FLOG_INFO(("Discovered EUD protocol: " + ifaceDesc + " : " + currentEvent.m_location).c_str());
                     }
                     else if(DEV_PROTOCOL_DUN == devProtocol)
                     {
                        FLOG_INFO(("Discovered DUN protocol: " + ifaceDesc + " : " + currentEvent.m_location).c_str());
                     }
                     else if(QC_DEV_TYPE_TAC == devType)
                     {
                        FLOG_INFO(("Discovered TAC protocol: " + ifaceDesc + " : " + currentEvent.m_location).c_str());
                     }
                     else if(QC_DEV_TYPE_EPM == devType)
                     {
                        FLOG_INFO(("Discovered EPM protocol: " + ifaceDesc + " : " + currentEvent.m_location).c_str());
                     }
                     else
                     {
                        if(std::string::npos != ifaceDesc.find("YunOS"))
                        {
                           FLOG_WARNING(("Malicious YunOS driver found: " + ifaceDesc + " : " + currentEvent.m_location)
                                           .c_str());
                        }
                     }
                  }
                  if(pProtocol != nullptr && (QC_DEV_TYPE_TAC != devType && QC_DEV_TYPE_EPM != devType))
                  {
                     std::pair<std::string, std::string> pairVidPid = getVidPid(currentEvent);
                     pDevice->setVid(pairVidPid.first);
                     pDevice->setPid(pairVidPid.second);
                     pDevice->setHwId(currentEvent.m_hwId.empty() ? std::string() : std::string(currentEvent.m_hwId));
                  }

                  // ADB interfaces will not receive serial number from device discovery. Assign serial number from
                  // sibling interface when available.

                  // Fastboot interface will not receive serial number from
                  // device discovery. Assign serial number from device cache if
                  // available. NOTE: Fastboot protocol support is currently
                  // disabled/removed.

                  pStateChangeProtocol = pProtocol;
                  newState = Protocol::Base::STATE_INITIALIZING;

                  pDevice->addProtocol(pProtocol);
                  FLOG_INFO(("==-==>>>> Adding protocol total " +
                             std::string(std::to_string(pDevice->getProtocolCount())) + " : " +
                             pProtocol->getDescription())
                               .c_str());

                  ComSet::const_iterator itC = g_forcedDisconnects.find(pProtocol->getCommonIo());
                  if(g_forcedDisconnects.end() != itC)
                  {
                     // If the device manager closed the underlying CommonIO,
                     // automatically reconnect it
                     auto timeSinceDiscovered =
                        std::chrono::system_clock::now() - *currentEvent.m_initialProcessingTime;
                     if(timeSinceDiscovered < DISCOVERY_WAIT_PERIOD)
                     {
                        // Sometimes opening the port too early after discovery
                        // causes issues try waiting at least 50 ms before
                        // opening
                        Util::ThisThread::sleep(std::chrono::duration_cast<
                                                std::chrono::milliseconds>(DISCOVERY_WAIT_PERIOD - timeSinceDiscovered)
                        );
                     }

                     try
                     {
                        (*itC)->open();
                        // If it throws, it may have already been unplugged
                        // again. leave in the force disconnect for the next
                        // discovery.
                        g_forcedDisconnects.erase(itC);
                     }
                     TOOLS_CATCH(e, newState = Protocol::Base::STATE_UNRESPONSIVE; APP_REPORT_EXCEPTION(e));
                  }
               }

               if(!m_bInitialComplete)
               {
                  // Check if it's finished the initial discovery.  This means
                  // it's gotten and processed the first round of discovery
                  // events.  If the code is here, it's gotten them.  If the
                  // queue is empty, it's processed them.
                  std::lock_guard<std::recursive_mutex> lock(m_mutex);
                  m_bInitialComplete = m_events.empty();
                  if(m_bInitialComplete)
                  {
                     m_initialDiscovery.signal();
                     FLOG_INFO("DeviceManagerHelper::onRun, initialDiscovery "
                               "complete");
                  }
               }

               if(bNotifyDevice)
               {
                  try
                  {
                     FLOG_INFO(("==-==>>>> device connect events: " + pProtocol->getDescription()).c_str());
                     m_pManager->notifyAsync(std::make_shared<DeviceConnectEvent>(pDevice));
                  }
                  TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
               }

               try
               {
                  FLOG_INFO(("==-==>>>> protocol added events: " + pProtocol->getDescription()).c_str());
                  m_pManager->notifyAsync(std::make_shared<ProtocolAddedEvent>(pDevice, pProtocol));
               }
               TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));

               if(pStateChangeProtocol != nullptr)
               {
                  try
                  {
                     FLOG_INFO(("==-==>>>> protocol state events: " + pProtocol->getDescription()).c_str());
                     pStateChangeProtocol->setState(newState);
                  }
                  TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
                  pStateChangeProtocol = NULL;
               }

               try
               {
                  FLOG_INFO(("==-==>>>> protocol onAdd: " + pProtocol->getDescription()).c_str());
                  pProtocol->onAdd();
               }
               TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));

               auto stopWatchDuration =
                  std::chrono::duration_cast<Util::duration>(std::chrono::steady_clock::now() - stopWatchStartTime);
               FLOG_INFO(("==-==>>>> Added protocol duration: " + Util::format_duration(stopWatchDuration) +
                          " total: " + std::to_string(pDevice->getProtocolCount()) + " : " + pProtocol->getDescription()
               )
                            .c_str());
            }
            else if(QC_DEV_STATE_DEPARTURE == devState)
            {
               auto stopWatchStartTime = std::chrono::steady_clock::now();
               bool bDepartureEventCanceled = false;
               {
                  std::lock_guard<std::recursive_mutex> lock(m_mutex);
                  // Cancel arrival/departure pair except for reversed
                  // departure/arrival instance
                  for(DeviceEventQueue::iterator it = m_events.begin(); it != m_events.end(); ++it)
                  {
                     if(it->m_bIsPushbackEvent && !it->m_bIsPushbackRearrival &&
                        QC_DEV_STATE_ARRIVAL == (UCHAR)((it->m_flags & QC_FLAG_MASK_DEV_STATE) >> 4) &&
                        it->m_description == currentEvent.m_description &&
                        it->m_deviceName == currentEvent.m_deviceName &&
                        it->m_parentDevice == currentEvent.m_parentDevice &&
                        (Util::toLowerCopy(it->m_serialNumberMsm
                         ) == Util::toLowerCopy(currentEvent.m_serialNumberMsm) ||
                         m_bIgnoreSerNum) &&
                        (Util::toLowerCopy(it->m_serialNumberAdb
                         ) == Util::toLowerCopy(currentEvent.m_serialNumberAdb) ||
                         m_bIgnoreSerNum) &&
                        it->m_location == currentEvent.m_location && it->m_protocol == currentEvent.m_protocol)
                     {
                        FLOG_INFO(("Cancel arrival/departure events: " + currentEvent.m_description + " : " +
                                   currentEvent.m_location)
                                     .c_str());
                        m_events.erase(it);
                        bDepartureEventCanceled = true;
                        break;
                     }
                  }
               }

               if(bDepartureEventCanceled)
               {
                  // Even though it's cancelled we need to make sure the state
                  // of the protocol is set to disconnected.  Look for possible
                  // matches in disconnected items since the add was not
                  // processed
                  Device::ImplPtr pDevice;
                  std::lock_guard<std::recursive_mutex> lock(Manager::getInstance()->m_deviceManagerMutex);
                  FLOG_INFO(
                     ("DeviceManagerHelper::onRun: Device Departure: "
                      "Check match within connected devices, desc: " +
                      currentEvent.m_description + ", parentdesc: " + parentDescription)
                  );
                  Manager::DeviceList::const_iterator it = getMatch(
                     Manager::getInstance()->m_devices,
                     Manager::getInstance()->m_disconnectedDevices,
                     currentEvent,
                     uniqueIdentifier,
                     parentDescription,
                     serialNumberMsm,
                     serialNumberAdb
                  );

                  if(it != Manager::getInstance()->m_devices.end())
                  {
                     FLOG_INFO(("Matched departure protocol to device: " + currentEvent.m_description + " : " +
                                currentEvent.m_location)
                                  .c_str());
                     pDevice = it->second;
                  }
                  if(pDevice == nullptr)
                  {
                     FLOG_INFO(
                        ("DeviceManagerHelper::onRun: Device Departure: "
                         "Check "
                         "match within disconnected devices, desc: " +
                         currentEvent.m_description + ", parentdesc: " + parentDescription)
                     );
                     it = getMatch(
                        Manager::getInstance()->m_disconnectedDevices,
                        Manager::getInstance()->m_devices,
                        currentEvent,
                        uniqueIdentifier,
                        parentDescription,
                        serialNumberMsm,
                        serialNumberAdb
                     );
                     if(it != Manager::getInstance()->m_disconnectedDevices.end())
                     {
                        FLOG_INFO(("Matched departure protocol to disconnected "
                                   "device: " +
                                   currentEvent.m_description + " : " + currentEvent.m_location)
                                     .c_str());
                        pDevice = it->second;
                     }
                  }

                  if(pDevice != nullptr)
                  {
                     Impl::ProtocolList::const_iterator itP = pDevice->m_unavailableProtocols.begin();
                     Impl::ProtocolList::const_iterator endP = pDevice->m_unavailableProtocols.end();
                     for(; itP != endP; ++itP)
                     {
                        Communication::CommonIoPtr pComm = (*itP)->getCommonIo();

                        if(pComm != nullptr && isIdentifierMatched(pComm->getIdentifier(), currentEvent.m_deviceName))
                        {
                           FLOG_ERROR(("Matched departure protocol to "
                                       "disconnected protocol: " +
                                       currentEvent.m_description + " : " + currentEvent.m_location)
                                         .c_str());
                           pStateChangeProtocol = *itP;
                           newState = Protocol::Base::STATE_DISCONNECTED;
                           break;
                        }
                     }
                  }

                  continue;
               }

               Device::Protocol::BasePtr pProtocol;
               Device::Protocol::BasePtr pProtocolFirehose;
               bool bFirehoseOnly = false;
               {
                  std::lock_guard<std::recursive_mutex> lock(Manager::getInstance()->m_deviceManagerMutex);
                  FLOG_INFO(
                     ("DeviceManagerHelper::onRun: Device Departure: Check "
                      "match within connected devices for protocol, desc: " +
                      currentEvent.m_description + ", parentdesc: " + parentDescription)
                  );
                  Manager::DeviceList::const_iterator it = getMatch(
                     Manager::getInstance()->m_devices,
                     Manager::getInstance()->m_disconnectedDevices,
                     currentEvent,
                     uniqueIdentifier,
                     parentDescription,
                     serialNumberMsm,
                     serialNumberAdb
                  );
                  if(Manager::getInstance()->m_devices.end() != it)
                  {
                     bool bProtocolMatchFound = false;
                     Impl::ProtocolList::iterator prot = it->second->m_protocols.begin();
                     Impl::ProtocolList::iterator protEnd = it->second->m_protocols.end();
                     // bool bProtocolDuplicated = true; // Unused variable
                     for(; prot != protEnd; ++prot)
                     {
                        Communication::CommonIoPtr pComm = (*prot)->getCommonIo();
                        if(pComm != nullptr && isIdentifierMatched(pComm->getIdentifier(), currentEvent.m_deviceName))
                        {
                           try
                           {
                              if(pComm->isOpen() || m_pManager->isProtocolUsedByConnection(*prot))
                              {
                                 // QMI IO cannot be recycled
                                 // Sahara should not be auto reconnected
                                 if((*prot).dynamicCast<Protocol::Sahara>() == nullptr)
                                 {
                                    g_forcedDisconnects.insert(pComm);
                                 }
                              }
                              else
                              {
                                 // If it was on the force disconnect
                                 // inappropriately, remove it
                                 ComSet::iterator itComm = g_forcedDisconnects.find(pComm);
                                 if(g_forcedDisconnects.end() != itComm)
                                 {
                                    g_forcedDisconnects.erase(itComm);
                                 }
                              }
                              FLOG_INFO(("<<<<==-== IO close: " + pComm->getDescription()).c_str());
                              pComm->close();
                           }
                           TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));

                           if(((*prot)->getOverrideProtocol()).dynamicCast<Device::Protocol::Sahara>() != nullptr ||
                              ((*prot)->getOverrideProtocol()).dynamicCast<Device::Protocol::Firehose>() != nullptr)
                           {
                              if(((*prot)->getOverrideProtocol()).dynamicCast<Device::Protocol::Sahara>() != nullptr)
                              {
                                 // if sahara or firehose, do not break, since
                                 // there could be another protocol with the
                                 // same name need to be removed as well
                                 pProtocol = *prot;

                                 pStateChangeProtocol = *prot;
                                 bFirehoseOnly = false;
                                 if(pProtocolFirehose != nullptr)
                                 {
                                    // If both protocol found, we don't need a
                                    // dup remove event
                                    pProtocolFirehose->setState(Protocol::Base::STATE_DISCONNECTED);
                                 }
                              }
                              else
                              {
                                 pProtocolFirehose = *prot;
                                 if(pProtocol == nullptr)
                                 {
                                    // In case sahara is null, should not
                                    // happened, keep the firehose ptr as
                                    // pProtocol Otherwise the pProtocol will be
                                    // replace with sahara ptr later
                                    pProtocol = *prot;

                                    pStateChangeProtocol = *prot;
                                    bFirehoseOnly = true;
                                 }
                                 else
                                 {
                                    // If both protocol found, we don't need a
                                    // dup remove event
                                    pProtocolFirehose->setState(Protocol::Base::STATE_DISCONNECTED);
                                 }
                              }
                              newState = Protocol::Base::STATE_DISCONNECTED;
                              bProtocolMatchFound = true;
                           }
                           else
                           {
                              pProtocol = *prot;

                              pStateChangeProtocol = *prot;
                              newState = Protocol::Base::STATE_DISCONNECTED;

                              bProtocolMatchFound = true;

                              break;
                           }
                        }
                     }

                     if(bProtocolMatchFound)
                     {
                        std::vector<Device::Protocol::BasePtr> pProtocolList;
                        {
                           // Move remove protocol process here for continue
                           // above for
                           if(pProtocolFirehose != nullptr && !bFirehoseOnly)
                           {
                              it->second->removeProtocol(pProtocolFirehose);
                           }
                           if(pProtocol != nullptr)
                           {
                              it->second->removeProtocol(pProtocol);
                           }
                           FLOG_INFO(("<<<<==-== Removed protocol total " +
                                      std::string(std::to_string(it->second->getProtocolCount())) + " : " +
                                      pProtocol->getDescription())
                                        .c_str());
                        }
                     }
                     else
                     {
                        FLOG_INFO(("Protocol Departure:  No match found for "
                                   "currentEvent : "
                                   "currentEvent.m_description"));
                     }

                     if(it->second->m_protocols.empty())
                     {
                        bNotifyDevice = true;
                        Device::ImplPtr pDevice = it->second;
                        Manager::getInstance()->m_devices.erase(it);
                        Manager::getInstance()->m_disconnectedDevices[pDevice->getHandle()] = pDevice;

                        FLOG_INFO(
                           ("<<<<==-== Removed device " + std::string(std::to_string(pDevice->getHandle())) +
                            " : "
                            "uniqueIdentifier: " +
                            pDevice->getUniqueIdentifier() +
                            " : "
                            "parentDescription: " +
                            pDevice->getDescription() +
                            " : "
                            "serialNumberMsm: " +
                            pDevice->getSerialNumberMsm() +
                            " : "
                            "serialNumberAdb: " +
                            pDevice->getSerialNumberAdb() +
                            " : "
                            "vid: " +
                            pDevice->getVid() +
                            " : "
                            "pid: " +
                            pDevice->getPid() +
                            " : "
                            "socVersion: " +
                            pDevice->getSocVersion())
                              .c_str()
                        );
                     }
                  }
               }

               try
               {
                  if(pProtocol != nullptr)
                  {
                     FLOG_INFO(("<<<<==-== protocol onDrop: " + pProtocol->getDescription()).c_str());
                     pProtocol->onDrop();
                  }
               }
               TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));

               if(pStateChangeProtocol != nullptr)
               {
                  try
                  {
                     FLOG_INFO(
                        (std::string("<<<<==-== protocol setState: " + pProtocol->getDescription()) +
                         ", newstate = " + std::to_string(newState))
                     );
                     pStateChangeProtocol->setState(newState);
                  }
                  TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
                  pStateChangeProtocol = NULL;
               }

               if(pProtocol != nullptr)
               {
                  try
                  {
                     FLOG_INFO(("<<<<==-== protocol removal event: " + pProtocol->getDescription()).c_str());
                     m_pManager->notifyAsync(std::make_shared<ProtocolRemovedEvent>(pProtocol->getDevice(), pProtocol));
                  }
                  TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));

                  if(bNotifyDevice)
                  {
                     try
                     {
                        FLOG_INFO(("<<<<==-== device disconnect event: " + pProtocol->getDescription()).c_str());
                        m_pManager->notifyAsync(std::make_shared<DeviceDisconnectEvent>(pProtocol->getDevice()));
                     }
                     TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
                  }
               }

               auto stopWatchDuration =
                  std::chrono::duration_cast<Util::duration>(std::chrono::steady_clock::now() - stopWatchStartTime);
               if(pProtocol != nullptr)
               {
                  FLOG_INFO(("<<<<==-== Removed protocol duration: " + Util::format_duration(stopWatchDuration) +
                             ", protocol Description : " + pProtocol->getDescription())
                               .c_str());
               }
               else
               {
                  FLOG_INFO(("<<<<==-== Removed protocol duration (No match "
                             "found): " +
                             Util::format_duration(stopWatchDuration) + ", currentEvent : " + currentEvent.m_description
                  )
                               .c_str());
               }
            }
         }
         TOOLS_CATCH(e, APP_REPORT_EXCEPTION(e));
      }
   }

   // ----------------------------------------------------------------------------
   // getMatch
   //
   /// Check if the passed in currentEvent matches among the devices list.
   //  Returns iterator to the match element if found and devices.end() if not
   //  found
   // ----------------------------------------------------------------------------
   Manager::DeviceList::const_iterator getMatch(
      const Manager::DeviceList& devices,
      const Manager::DeviceList& additionalDevices,
      Device::DeviceManagerHelper::DeviceEvent& currentEvent,
      const std::string& uniqueIdentifier,
      const std::string& parentDescription,
      const std::string& serialNumberMsm,
      const std::string& serialNumberAdb
   )
   {
      std::lock_guard<std::recursive_mutex> lock(Manager::getInstance()->m_deviceManagerMutex);
      Manager::DeviceList::const_iterator it;

      // First, check if exact match exists based on serial numbers only.
      // Note: Always check additional devices to detect double entries, and to
      // avoid false match
      bool bFoundInAdditional = false;
      for(it = additionalDevices.begin(); it != additionalDevices.end(); ++it)
      {
         MatchType snMatch = isSerialNumberMatched(it->second, serialNumberMsm, serialNumberAdb);
         if(MATCH_SUCCESS == snMatch)
         {
            FLOG_INFO((("DeviceManagerHelper::getMatch: serial number match "
                        "FOUND in additional devices for currentEvent = " +
                        currentEvent.m_description + ", device = " + it->second->getDescription())
                          .c_str()));
            bFoundInAdditional = true;
         }
      }
      // UCHAR devState = (UCHAR)((currentEvent.m_flags &
      // QC_FLAG_MASK_DEV_STATE) >> 4); // Unused variable
      if(m_bIgnoreSerNum)
      {
         // only match device location once device arrived and find from
         // disxonnected device
         for(it = devices.begin(); it != devices.end(); ++it)
         {
            if(isPhysicalLocationMatched(it->second, currentEvent.m_parentLocationInfomation))
            {
               FLOG_INFO(("DeviceManagerHelper::getMatch: physical location "
                          "match FOUND for currentEvent = " +
                          currentEvent.m_description + ", device = " + it->second->getDescription())
                            .c_str());
               if(bFoundInAdditional)
               {
                  FLOG_ERROR(("DeviceManagerHelper::getMatch: duplicated "
                              "entries FOUND for currentEvent = " +
                              currentEvent.m_description + ", device = " + it->second->getDescription())
                                .c_str());
               }
               return it;
            }
         }
         // for factory mode no need to match other items
         return devices.end();
      }

      UCHAR devType = (UCHAR)((currentEvent.m_flags & QC_FLAG_MASK_DEV_TYPE) >> 8);
      std::string STRING_PATTERN_TAC_OR_EPM = "";

      // if devType is TAC or EPM assign pattern to STRING_PATTERN_TAC_OR_EPM
      // for others it will be empty string
      if(QC_DEV_TYPE_TAC == devType)
      {
         STRING_PATTERN_TAC_OR_EPM = ".*?TAC.*?";
      }
      else if(QC_DEV_TYPE_EPM == devType)
      {
         STRING_PATTERN_TAC_OR_EPM = ".*?EPM.*?";
      }

      for(it = devices.begin(); it != devices.end(); ++it)
      {
         MatchType snMatch = isSerialNumberMatched(it->second, serialNumberMsm, serialNumberAdb);
         if(MATCH_SUCCESS == snMatch)
         {
            FLOG_INFO((("DeviceManagerHelper::getMatch: serial number match "
                        "FOUND for currentEvent = " +
                        currentEvent.m_description + ", device = " + it->second->getDescription())
                          .c_str()));
            if(bFoundInAdditional)
            {
               FLOG_ERROR(("DeviceManagerHelper::getMatch: duplicated entries "
                           "FOUND for currentEvent = " +
                           currentEvent.m_description + ", device = " + it->second->getDescription())
                             .c_str());
            }
            // For TAC and EPM device serial number can be same.
            // If Epm serial number matched with tac then tac device will be
            // removed. So, to differentiate between them compare description.
            if(QC_DEV_TYPE_TAC == devType || QC_DEV_TYPE_EPM == devType)
            {
               if(!isPatternFoundInDescription(it->second->getDescription(), STRING_PATTERN_TAC_OR_EPM))
               {
                  continue;
               }
               FLOG_INFO(("DeviceManagerHelper::getMatch: TAC/EPM description "
                          "match FOUND for currentEvent = " +
                          currentEvent.m_description + ", device = " + it->second->getDescription())
                            .c_str());
            }
            return it;
         }
      }

      // Second, check if match exists based on unique identifier or device path
      // when there is no serial number match in additional devices
      for(it = devices.begin(); it != devices.end(); ++it)
      {
         // When device is present, always group when serial numbers match or
         // bus locations match
         MatchType snMatch = isSerialNumberMatched(it->second, serialNumberMsm, serialNumberAdb);
         if((MATCH_FAIL != snMatch || m_bIgnoreSerNum) &&
            (isUniqueIdentifierMatched(it->second, uniqueIdentifier) ||
             isDevicePathMatched(it->second, currentEvent.m_devicePath)))
         {
            FLOG_INFO(("DeviceManagerHelper::getMatch: "
                       "uniqueIdentifier/devicePath "
                       "match FOUND for currentEvent = " +
                       currentEvent.m_description + ", device = " + it->second->getDescription())
                         .c_str());
            if(bFoundInAdditional)
            {
               FLOG_ERROR(("DeviceManagerHelper::getMatch: shared "
                           "uniqueIdentifier/devicePath detected for "
                           "currentEvent = " +
                           currentEvent.m_description + ", device = " + it->second->getDescription())
                             .c_str());
            }
            else
            {
               return it;
            }
         }
      }

      // Last, if match didnt exist based on serial number, unique identifier or
      // device path above, now check if can match based on description or
      // parentname
      for(it = devices.begin(); it != devices.end(); ++it)
      {
         // Only match description/parentname when there is no serial number
         // conflict
         MatchType snMatch = isSerialNumberMatched(it->second, serialNumberMsm, serialNumberAdb);
         if((MATCH_FAIL != snMatch || m_bIgnoreSerNum) &&
            (isDescriptionMatched(it->second, currentEvent.m_description) ||
             isParentNameMatched(it->second, parentDescription)))
         {
            FLOG_INFO(("DeviceManagerHelper::getMatch: desc/parentName match "
                       "FOUND for currentEvent = " +
                       currentEvent.m_description + ", device = " + it->second->getDescription())
                         .c_str());
            if(bFoundInAdditional)
            {
               FLOG_ERROR(("DeviceManagerHelper::getMatch: same "
                           "desc/parentName detected for currentEvent = " +
                           currentEvent.m_description + ", device = " + it->second->getDescription())
                             .c_str());
            }
            else
            {
               return it;
            }
         }
      }
      return devices.end();
   }

private:
   typedef std::list<Device::DeviceManagerHelper::DeviceEvent> DeviceEventQueue;

   inline std::string getString(PWCHAR pStr)
   {
      return NULL == pStr ? std::string() : Util::fromWString(std::wstring(pStr));
   }
   inline std::string getString(PCHAR pStr)
   {
      return NULL == pStr ? std::string() : std::string(pStr);
   }

   void addDiscoveryEvent(PCB_PARAMS pCbParams)
   {
      UCHAR devState = (UCHAR)((pCbParams->Flag & QC_FLAG_MASK_DEV_STATE) >> 4);
      std::string direction = (QC_DEV_STATE_ARRIVAL == devState) ? "==-==>>>> " : "<<<<==-== ";
      FLOG_INFO((direction + "DevDesc: " + getString(pCbParams->DevDesc) +
                 " DevName: " + (NULL == pCbParams->DevName ? std::string() : getString(pCbParams->DevName)) +
                 " DevPath: " + getString(pCbParams->DevPath) + " Flag: " + std::to_string(pCbParams->Flag) +
                 " HwId: " + getString(pCbParams->HwId) + " IfName: " + getString(pCbParams->IfName) +
                 " Loc: " + getString(pCbParams->Loc) + " Mtu: " + std::to_string(pCbParams->Mtu) +
                 " ParentDev: " + getString(pCbParams->ParentDev) +
                 " ParentLocationInfomation: " + getString(pCbParams->ParentLocationInfomation) +
                 " Protocol: " + std::to_string(pCbParams->Protocol) + " SerNum: " + getString(pCbParams->SerNum) +
                 " SerNumMsm: " + getString(pCbParams->SerNumMsm) + " SocVersion: " + getString(pCbParams->SocVer))
                   .c_str());


      Device::DeviceManagerHelper::DeviceEvent newEvent;
      newEvent.m_eventTime = std::chrono::system_clock::now();
      newEvent.m_initialProcessingTime = std::nullopt;
      newEvent.m_bIsPushbackEvent = false;
      newEvent.m_bIsPushbackConnected = false;
      newEvent.m_bIsPushbackRearrival = false;
      newEvent.m_description = getString(pCbParams->DevDesc);
      newEvent.m_deviceName = getString(pCbParams->DevName);
      newEvent.m_devicePath = getString(pCbParams->DevPath);
      newEvent.m_location = getString(pCbParams->Loc);
      newEvent.m_parentDevice = getString(pCbParams->ParentDev);
      newEvent.m_parentLocationInfomation = getString(pCbParams->ParentLocationInfomation);
      newEvent.m_serialNumberMsm = getString(pCbParams->SerNumMsm);
      // Ignore dummy adb serial number
      newEvent.m_serialNumberAdb =
         (getString(pCbParams->SerNum) == DUMMY_ADB_SERIAL_NUMBER) ? std::string() : getString(pCbParams->SerNum);
      newEvent.m_flags = pCbParams->Flag;
      newEvent.m_protocol = pCbParams->Protocol;
      newEvent.m_hwId = getString(pCbParams->HwId);
      newEvent.m_socVersion = getString(pCbParams->SocVer);
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      m_events.push_back(newEvent);
      m_discoverEvent.signal();
   }

   // -------------------------------------------------------------------------
   // pushBackEvent
   //
   /// Pushes a discovery event back onto the queue in case it wants to wait
   /// to process it
   // -------------------------------------------------------------------------
   void pushBackEvent(
      Device::DeviceManagerHelper::DeviceEvent& currentEvent,
      const Device::ImplPtr& pDevice,
      bool bHideDevice
   )
   {
      bool bWait = false;
      {
         std::lock_guard<std::recursive_mutex> lock(m_mutex);
         if(m_events.empty() || m_events.front().m_bIsPushbackEvent)
         {
            bWait = true;
         }
         currentEvent.m_bIsPushbackEvent = true;
         m_events.push_back(currentEvent);
      }

      if(bHideDevice)
      {
         std::lock_guard<std::recursive_mutex> lock(Manager::getInstance()->m_deviceManagerMutex);
         // This means it was a new device; put the device back on the
         // disconnected list
         Manager::DeviceList& devices = Manager::getInstance()->m_devices;
         Manager::DeviceList::iterator it = devices.find(pDevice->getHandle());
         if(devices.end() != it)
         {
            devices.erase(it);
         }
         Manager::getInstance()->m_disconnectedDevices[pDevice->getHandle()] = pDevice;
      }
      if(bWait)
      {
         Util::ThisThread::waitForEvent(&m_discoverEvent, DISCOVERY_WAIT_PERIOD);
      }
   }

   Util::CheckedPointer<Manager> m_pManager; ///< Device manager instance

   DeviceEventQueue m_events;        ///< Queue of events to process
   std::recursive_mutex m_mutex;     ///< Protects m_events
   Util::Event m_discoverEvent;      ///< Signals new data
   volatile bool m_bInitialComplete; ///< Whether initial discovery completed
   bool m_bIgnoreSerNum;             ///< Ignore serial numbers for factory setup
   Util::Event m_initialDiscovery;
   Util::ThreadId m_threadId;
};

// ----------------------------------------------------------------------------
// getInstance
//
/// @returns The singleton instance of the device manager
// ----------------------------------------------------------------------------
ManagerPtr Manager::getInstance()
{
   static std::once_flag initFlag;
   static ManagerPtr pTheManager;

   std::call_once(initFlag, [] {
      pTheManager = Util::SharedPointer<Manager>::create();
      FLOG_INFO("created new pTheManager");
   });

   return pTheManager;
}


// ----------------------------------------------------------------------------
// ~Manager
//
// ----------------------------------------------------------------------------
Manager::~Manager()
{
   FLOG_INFO("Manager::~Manager() called");
}

// ----------------------------------------------------------------------------
// setApplicationInfo
//
/// Sets the information for the application
// ----------------------------------------------------------------------------
void Manager::setApplicationInfo(
   const std::filesystem::path& tempFolder,
   const std::string& appName,
   uint16_t appMajorVer,
   uint16_t appMinorVer,
   const std::string& appBuildId,
   const std::filesystem::path& programDataFolder
)
{
   m_tempLogFolder = tempFolder;
   m_appName = appName;
   m_appMajorVer = appMajorVer;
   m_appMinorVer = appMinorVer;
   m_appBuildId = appBuildId;
   m_programDataDir = programDataFolder;

   // m_pLogFileManager = std::make_shared<LogFileManager>(
   //    m_tempLogFolder,
   //    m_appName,
   //    m_appMajorVer,
   //    m_appMinorVer,
   //    m_appBuildId
   //);
    // Needs to be done after m_pLogFileManager is created or a race
    // condition may occur causing an exception during detection
    static DEV_FEATURE_SETTING mySetting;

    //This will only allow Qualcomm Device VID_05C6 - PID 9008
    mySetting.Version = 1;
    mySetting.Settings = DEV_FEATURE_SCAN_USB_WITH_VID;
    mySetting.DeviceClass = 0x0F;
    mySetting.VID = (PTSTR)TEXT("VID_05C6&PID_9008");

    // Set logging callback BEFORE calling SetFeature to avoid cout
    // initialization issues
    QcDevice::SetLoggingCallback(&DeviceManagerHelper::logQDS);

    QcDevice::SetFeature((PVOID)&mySetting);

    m_pDiscoveryWork = std::make_shared<DeviceManagerHelper>(ManagerPtr(shared_from_this()));
    m_pDiscoveryThread = std::make_shared<Util::StdThreadWrapper>(m_pDiscoveryWork);
    m_pDiscoveryThread->start();

    QcDevice::SetDeviceChangeCallback(&DeviceManagerHelper::devCommDeviceChangeCb, m_pDiscoveryWork.get());
    QcDevice::StartDeviceMonitor();
    // Do a sleep 0 to hopefully let the device discovery thread start running
    Util::ThisThread::sleep(std::chrono::milliseconds(0));
}

// ----------------------------------------------------------------------------
// shutDown
//
/// Clears all members to allow a clean application exit
// ----------------------------------------------------------------------------
void Manager::shutDown()
{
   FLOG_INFO("shutDown started");

    TOOLS_IGNORE_EXCEPTIONS(QcDevice::StopDeviceMonitor());
    TOOLS_IGNORE_EXCEPTIONS(m_pDiscoveryThread->stop());
    TOOLS_IGNORE_EXCEPTIONS(m_pDiscoveryThread->waitForStop());

   // TOOLS_IGNORE_EXCEPTIONS(
   //    if (m_pLogFileManager != nullptr) { m_pLogFileManager->close(); }
   //);
   // TOOLS_IGNORE_EXCEPTIONS(stopAllTcpServer());

   TOOLS_IGNORE_EXCEPTIONS(m_connections.clear());
   TOOLS_IGNORE_EXCEPTIONS(m_devices.clear());
   TOOLS_IGNORE_EXCEPTIONS(m_disconnectedDevices.clear());

   // TOOLS_IGNORE_EXCEPTIONS(m_pLogFileManager = NULL);
   FLOG_INFO("shutDown ended");
}

// ----------------------------------------------------------------------------
// getAppMajorVersion
//
/// @returns The major app version of the application running the device manager
// ----------------------------------------------------------------------------
uint16_t Manager::getAppMajorVersion()
{
   return m_appMajorVer;
}

// ----------------------------------------------------------------------------
// getAppMinorVersion
//
/// @returns The minor app version of the application running the device manager
// ----------------------------------------------------------------------------
uint16_t Manager::getAppMinorVersion()
{
   return m_appMinorVer;
}

// ----------------------------------------------------------------------------
// getAppBuildId
//
/// @returns The build ID of the application running the device manager
// ----------------------------------------------------------------------------
std::string Manager::getAppBuildId()
{
   return m_appBuildId;
}

// ----------------------------------------------------------------------------
// getProgramDataDirectory
//
/// @returns The path for storing app cache info
// ----------------------------------------------------------------------------
std::filesystem::path Manager::getProgramDataDirectory()
{
   return m_programDataDir;
}

// ----------------------------------------------------------------------------
// getTempDirectory
//
/// @returns The temporary files directory
// ----------------------------------------------------------------------------
std::filesystem::path Manager::getTempDirectory()
{
   return m_tempLogFolder;
}

// ----------------------------------------------------------------------------
// getDeviceList
//
/// @returns The complete list of currently available devices
// ----------------------------------------------------------------------------
Manager::DeviceList Manager::getDeviceList() const
{
   waitForInitialDeviceList();
   std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);
   return m_devices;
}

// ----------------------------------------------------------------------------
// addDevice
//
/// Add device to the list of devices.
// ----------------------------------------------------------------------------
void Manager::addDevice(const ImplPtr& pDevice)
{
   std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);
   m_devices[pDevice->getHandle()] = pDevice;
}


// ----------------------------------------------------------------------------
// getDeviceByHandle
//
/// @returns The device object for the corresponding handle
// ----------------------------------------------------------------------------
ImplPtr Manager::getDeviceByHandle(DeviceHandle handle)
{
   waitForInitialDeviceList();
   std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);

   DeviceList::iterator it = m_devices.find(handle);

   if(m_devices.end() == it)
   {
      it = m_disconnectedDevices.find(handle);
      TOOLS_ASSERT_OR_THROW(
         m_disconnectedDevices.end() != it,
         Device::Exception(
            Device::Exception::DEVICE_INVALID_DEVICE_HANDLE,
            "Could not find device handle: " + std::to_string(handle)
         )
      );
   }

   return it->second;
}

// ----------------------------------------------------------------------------
// getProtocolByHandle
//
/// @returns The protocol object for the corresponding handle
// ----------------------------------------------------------------------------
Protocol::BasePtr Manager::getProtocolByHandle(Protocol::Handle handle)
{
   std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);
   DeviceList::iterator it = m_devices.begin();
   DeviceList::iterator end = m_devices.end();

   for(; it != end; ++it)
   {
      Device::Impl::ProtocolList::const_iterator itP = it->second->m_protocols.begin();
      Device::Impl::ProtocolList::const_iterator endP = it->second->m_protocols.end();
      for(; itP != endP; ++itP)
      {
         if((*itP)->getHandle() == handle)
         {
            return *itP;
         }
      }

      itP = it->second->m_unavailableProtocols.begin();
      endP = it->second->m_unavailableProtocols.end();
      for(; itP != endP; ++itP)
      {
         if((*itP)->getHandle() == handle)
         {
            return *itP;
         }
      }
   }

   it = m_disconnectedDevices.begin();
   end = m_disconnectedDevices.end();

   for(; it != end; ++it)
   {
      Device::Impl::ProtocolList::const_iterator itP = it->second->m_protocols.begin();
      Device::Impl::ProtocolList::const_iterator endP = it->second->m_protocols.end();
      for(; itP != endP; ++itP)
      {
         if((*itP)->getHandle() == handle)
         {
            return *itP;
         }
      }

      itP = it->second->m_unavailableProtocols.begin();
      endP = it->second->m_unavailableProtocols.end();
      for(; itP != endP; ++itP)
      {
         if((*itP)->getHandle() == handle)
         {
            return *itP;
         }
      }
   }

   TOOLS_THROW(Device::Exception(
      Device::Exception::DEVICE_INVALID_PROTOCOL_HANDLE,
      "Could not find protocol handle: " + std::to_string(handle)
   ));
}

// ----------------------------------------------------------------------------
// getProtocolType
//
/// @returns The type of protocol assigned to the given handle
// ----------------------------------------------------------------------------
ProtocolType Manager::getProtocolType(Protocol::Handle handle)
{
   return getProtocolType(getProtocolByHandle(handle));
}

// ----------------------------------------------------------------------------
// getProtocolType
//
/// @returns The type of protocol assigned to the given handle
// ----------------------------------------------------------------------------
ProtocolType Manager::getProtocolType(const Protocol::BasePtr& pProt)
{
   Protocol::BasePtr pProtocol = pProt;
   if(pProtocol != nullptr)
   {
      pProtocol = pProtocol->getOverrideProtocol();
   }

   if((pProtocol).dynamicCast<Protocol::Sahara>() != nullptr)
   {
      return ProtocolType::PROT_SAHARA;
   }
   else if((pProtocol).dynamicCast<Protocol::Firehose>() != nullptr)
   {
      return ProtocolType::PROT_FIREHOSE;
   }
   else
   {
      return ProtocolType::PROT_UNKNOWN;
   }
}

// ----------------------------------------------------------------------------
// isProtocolUsedByConnection
//
// Check client opened connection
// ----------------------------------------------------------------------------
bool Device::Manager::isProtocolUsedByConnection(const Device::Protocol::BasePtr& pProtocol)
{
   std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);
   ConnectionList::iterator it = m_connections.begin();
   ConnectionList::iterator end = m_connections.end();
   for(; it != end; ++it)
   {
      if(it->second.m_pConnection != nullptr && it->second.m_pConnection->getProtocol() == pProtocol)
      {
         return true;
      }
   }
   return false;
}
// ----------------------------------------------------------------------------
// openConnection
//
/// Checks the permissions and opens a connection on the given protocol
// ----------------------------------------------------------------------------
ConnectionPtr Manager::openConnection(
   const Protocol::BasePtr& pProtocol,
   const Protocol::Base::Access& access,
   const Protocol::Base::Share& share,
   int32_t clientId,
   std::shared_ptr<Util::IMessagePublisher> pPublisher,
   bool& bNewConnection,
   uint16_t contextHandle
)
{
   std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);

   ConnectionList::iterator it = m_connections.begin();
   ConnectionList::iterator end = m_connections.end();
   for(; it != end; ++it)
   {
      if(it->second.m_pConnection->getProtocol() == pProtocol && ((it->first >> 16) & 0x0000FFFF) != clientId)
      {
         // This means the protocol is already open by someone else, check
         // the permissions to see if both can open it


         // If both want read access, make sure they have the share set up
         if(!!(Protocol::Base::Access::READ & access) &&
            !!(Protocol::Base::Access::READ & it->second.m_pConnection->getAccess()) &&
            (!(Protocol::Base::Share::READ & share) ||
             !(Protocol::Base::Share::READ & it->second.m_pConnection->getShare())))
         {
            // Both want read access, but one doesn't want to share
            TOOLS_THROW(Device::Exception(
               Device::Exception::DEVICE_CONNECTION_LOCKED,
               "Read access locked for protocol by another client: " + std::to_string(it->second.m_clientId) +
                  " ,protocol: " + pProtocol->getDescription()

            ));
         }

         // If both want write access, make sure they have the share set up
         if(!!(Protocol::Base::Access::WRITE & access) &&
            !!(Protocol::Base::Access::WRITE & it->second.m_pConnection->getAccess()) &&
            (!(Protocol::Base::Share::WRITE & share) ||
             !(Protocol::Base::Share::WRITE & it->second.m_pConnection->getShare())))
         {
            // Both want write access, but one doesn't want to share
            TOOLS_THROW(Device::Exception(
               Device::Exception::DEVICE_CONNECTION_LOCKED,
               "Write access locked for protocol by another client: " + std::to_string(it->second.m_clientId) +
                  " ,protocol: " + pProtocol->getDescription()

            ));
         }
      }
   }

   ConnectionPtr pConnection;
   int64_t key = pProtocol->getHandle() + (clientId << 16) | contextHandle;
   it = m_connections.find(key);
   if(m_connections.end() == it)
   {
      // m_pLogFileManager->registerClientForProtocol(clientId, pProtocol);

      pConnection = pProtocol->createConnection(access, share, clientId, pPublisher);
      ConnectionInfo connection;
      connection.m_pConnection = pConnection;
      connection.m_openCount = 1;
      connection.m_clientId = clientId;
      m_connections[key] = connection;
      bNewConnection = true;

      FLOG_INFO(("Opened connection: " + pProtocol->getDescription()).c_str());
   }
   else
   {
      pConnection = it->second.m_pConnection;
      ++it->second.m_openCount;
      pConnection->elevateAccess(access, share);
      bNewConnection = false;
   }

   return pConnection;
}


// ----------------------------------------------------------------------------
// closeConnection
//
/// Closes the given connection and releases it
// ----------------------------------------------------------------------------
void Manager::closeConnection(const ConnectionPtr& pConnection)
{
   bool bDoDisconnect = false;
   {
      std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);

      FLOG_INFO(("closeConnection begin , Connection size: " + std::to_string(m_connections.size()) +
                 ", closing connection: " + pConnection->getProtocol()->getDescription())
                   .c_str());

      ConnectionList::iterator it = m_connections.begin();
      ConnectionList::iterator end = m_connections.end();
      for(; it != end; ++it)
      {
         if(it->second.m_pConnection == pConnection)
         {
            break;
         }
      }
      TOOLS_ASSERT_OR_RETURN(end != it, TOOLS_VOID);

      FLOG_INFO(("it->second.m_openCount: " + std::to_string(it->second.m_openCount)).c_str());
      --it->second.m_openCount;

      if(0 == it->second.m_openCount)
      {
         FLOG_INFO(("UnregisterClientForProtocol m_clientId: " + std::to_string(it->second.m_clientId)).c_str());

         // m_pLogFileManager->unregisterClientForProtocol(
         //    it->second.m_clientId,
         //    pConnection->getProtocol()
         //);

         bDoDisconnect = true;
         m_connections.erase(it);
         FLOG_INFO("Connection erased.");
      }
   }

   if(bDoDisconnect)
   {
      FLOG_INFO("Calling disconnect.");
      pConnection->disconnect();
      FLOG_INFO(("Closed connection: " + pConnection->getProtocol()->getDescription()).c_str());
   }

   FLOG_INFO("CloseConnection End.");
}

// ----------------------------------------------------------------------------
// sendImageManagementSerivceEvent
//
/// @returns Notifies all clients
// ----------------------------------------------------------------------------
void Manager::sendImageManagementServiceEvent(
   const std::string& serviceName,
   int64_t deviceHandle,
   int64_t protocolHandle,
   int64_t eventId,
   const std::string& description
)
{
   notifyAsync(std::make_shared<
               ImageManagementServiceEvent>(serviceName, deviceHandle, protocolHandle, eventId, description));
}

// ----------------------------------------------------------------------------
// reportCriticalEvent
//
/// @returns Notifies all clients of a critical event
// ----------------------------------------------------------------------------
void Manager::reportCriticalEvent(const CriticalEventId id, const std::string& location)
{
   notifyAsync(std::make_shared<CriticalEvent>(
      id,
      location,
      g_criticalEventMap.find(id) != g_criticalEventMap.end()
         ? g_criticalEventMap.find(id)->second
         : "Unknown Critical Event"
   ));
}

// ----------------------------------------------------------------------------
// onProtocolStateChange
//
/// Notifies that a protocol's state has changed
// ----------------------------------------------------------------------------
void Manager::onProtocolStateChange(Protocol::StateChangeEvent* pEvent)
{
   notifyAsync(std::shared_ptr<Protocol::StateChangeEvent>(pEvent, [](Protocol::StateChangeEvent*) {}));
}

// ----------------------------------------------------------------------------
// onMessageEvent
//
/// Notifies client that an event has occurred
// ----------------------------------------------------------------------------
void Manager::onCriticalEvent(Device::CriticalEvent* pEvent)
{
   notifyAsync(std::shared_ptr<Device::CriticalEvent>(pEvent, [](Device::CriticalEvent*) {}));
}

// ----------------------------------------------------------------------------
// getAccessiblePath
//
/// Checks if the file is accessible and tries to copy it local if it isn't
/// @returns The path used to access the file (possibly a copy)
// ----------------------------------------------------------------------------
std::filesystem::path Manager::getAccessiblePath(
   const std::filesystem::path& filePath,
   const std::filesystem::path& destFolder,
   const bool bIgnoreDirectories,
   const int32_t clientId,
   const bool bSearchCache
)
{
   (void)destFolder; // Suppress unused parameter warning
   (void)clientId;   // Suppress unused parameter warning

   TOOLS_ASSERT_OR_THROW(
      !filePath.is_relative(),
      Device::Exception(
         Device::Exception::DEVICE_INVALID_PARAMETERS,
         "Relative file path; all paths must be absolute: " + std::string(filePath.string().c_str())
      )
   );

   if(bSearchCache)
   {
      std::lock_guard<std::recursive_mutex> lock(m_cachedAccessibleFilesMutex);
      AccessibleFileMap::iterator it = m_cachedAccessibleFiles.find(filePath);
      if(m_cachedAccessibleFiles.end() != it)
      {
         if(std::filesystem::exists(it->second))
         {
            return it->second;
         }
         else
         {
            m_cachedAccessibleFiles.erase(it);
         }
      }
   }

   bool pathIsDirectory = false;
   try
   {
      if(std::filesystem::exists(filePath))
      {
         if(std::filesystem::is_regular_file(filePath))
         {
            // Try to open the file; if success just return this path.  That
            // means the system has access to it; no need to copy it.
            auto file = std::make_shared<std::fstream>();
            file->exceptions(std::ios::failbit | std::ios::badbit);
            file->open(filePath.string().c_str(), std::ios::in | std::ios::binary);
            file->close();
            FLOG_INFO(("Able to access file: " + filePath.string()).c_str());

            return filePath;
         }
         else if(std::filesystem::is_directory(filePath))
         {
            // ignore directories and just return the path back
            if(bIgnoreDirectories)
            {
               return filePath;
            }

            FLOG_INFO(("Able to access directory: " + filePath.string()).c_str());

            pathIsDirectory = true;

            // check whether the directory can be iterated
            std::error_code ec;
            std::filesystem::directory_iterator it(filePath, ec);
            std::filesystem::directory_iterator end;
            if(it == end)
            {
               FLOG_INFO(("Unable to iterate the directory: " + filePath.string()).c_str());
               throw("Unable to iterate the directory");
            }

            // It's recognized as a directory and can be iterated
            return filePath;
         }
         else
         {
            FLOG_ERROR(("Unknown file type on this platform: " + filePath.string()).c_str());
         }
      }
      else
      {
         {
            FLOG_ERROR(("File does not exist or current user does not have "
                        "permissions to access path: " +
                        filePath.string())
                          .c_str());
            return filePath;
         }
      }
   }
   TOOLS_CATCH(e,
               FLOG_ERROR(("Exception : " + std::string(e.what()) + " " + e.where()).c_str());
               APP_REPORT_EXCEPTION(e);
               return filePath;
               ;
   );

   return filePath;
}

// ----------------------------------------------------------------------------
// saveFile
//
/// Attempts to move the file from the given location to the specified location.
/// If the specified location is unavailable, spawn a process in the user
/// space to attempt to get the right permissions.
// ----------------------------------------------------------------------------
void Manager::saveFile(
   const std::filesystem::path& sourcePath,
   const std::filesystem::path& destinationPath,
   const bool bKeepSourceFile
)
{
   FLOG_INFO(("Saving file: " + sourcePath.string() + " to " + destinationPath.string()).c_str());

   if(sourcePath == destinationPath)
   {
      FLOG_WARNING(("The source path is the same as the destination path, not "
                    "need to save again and the source path is: " +
                    sourcePath.string())
                      .c_str());
      return;
   }
   // This may need to be updated for the other platforms similar to Windows
   // depending on user privileges
   bool bSucceed = true;
   try
   {
      Util::createPath(destinationPath.parent_path());
      if(!bKeepSourceFile)
      {
         FLOG_INFO(
            ("trying to save file. Not keeping source file, thus just "
             "rename it. bKeepSourceFile=" +
             std::to_string(bKeepSourceFile))
         );
         std::filesystem::rename(sourcePath, destinationPath);
      }
      else
      {
         FLOG_INFO("trying to save file. No renaming, thus skip to copying");
         TOOLS_THROW(ToolException("No renaming logs, skip to copying"));
      }
   }
   TOOLS_CATCH(e, {
      FLOG_ERROR("Exception when trying to save file. Exception= " + std::string(e.what()) + " " + e.where());
      try
      {
         FLOG_INFO("start copying temp file to destination");
         std::filesystem::copy(sourcePath, destinationPath);
         // std::filesystem::copy(sourcePath, destinationPath, false);
         FLOG_INFO("end copying temp file to destination");

         // don't delete if only coping files
         if(!bKeepSourceFile)
         {
            FLOG_INFO(("Start deleting source file=" + sourcePath.string()).c_str());
            std::filesystem::remove(sourcePath);
            FLOG_INFO(("End deleting source file=" + sourcePath.string()).c_str());
         }
      }
      TOOLS_CATCH(innerException, {
         FLOG_ERROR(
            "Exception when copying/deleteFile. Exception= " + std::string(innerException.what()) + " " +
            innerException.where()
         );
         bSucceed = false;
      });
   });


   if(!bSucceed)
   {
      TOOLS_THROW(Device::Exception(
         Device::Exception::DEVICE_PERMISSIONS_ERROR,
         "Unable to save file: " + std::string(destinationPath.string().c_str())
      ));
   }
}

// ----------------------------------------------------------------------------
// Manager
//
// ----------------------------------------------------------------------------
Manager::Manager()
: Util::EventPublisher()
, Util::AsyncEventPublisher()
, m_tempLogFolder()
, m_appName()
, m_appMajorVer(0)
, m_appMinorVer(0)
, m_appBuildId()
, m_programDataDir()
, m_devices()
, m_connections()
, m_nextDeviceHandle(DEVICE_HANDLE_INCREMENT)
, m_deviceManagerMutex()
, m_pStatusReportManager(std::make_shared<Report::StatusManager>())
, m_pDiscoveryWork()
, m_pDiscoveryThread()
, m_clientFilePathMutex()
, m_cachedAccessibleFilesMutex()
, m_cachedAccessibleFiles()
{
}

// ----------------------------------------------------------------------------
// waitForInitialDeviceList
//
/// Closes the given connection and releases it
// ----------------------------------------------------------------------------
void Manager::waitForInitialDeviceList() const
{
   if(m_pDiscoveryWork != nullptr)
   {
      m_pDiscoveryWork->waitForInitialDiscovery();
   }
   else
   {
      FLOG_INFO("Device Discovery Worker not enabled skipping "
                "waitForInitialDeviceList, check if client is in post "
                "processing mode?");
   }
}

// ----------------------------------------------------------------------------
// initialize
//
/// Does nothing
// ----------------------------------------------------------------------------
void Manager::initialize()
{
   m_pStatusReportManager->beginMonitoringStatus(STATUS_CHECK_PERIOD);
}

// ----------------------------------------------------------------------------
// finalize
//
/// Ensures the manager was shut down
// ----------------------------------------------------------------------------
void Manager::finalize()
{
   TOOLS_IGNORE_EXCEPTIONS(m_pStatusReportManager->endMonitoringStatus());
}


// ----------------------------------------------------------------------------
// mhiForceEdl
//
/// MHI kick start
// ----------------------------------------------------------------------------
void Manager::mhiForceEdl(int32_t instance, const std::string& programmerPath)
{
   TOOLS_UNUSED_PARAMETER(instance);
   TOOLS_UNUSED_PARAMETER(programmerPath);
   TOOLS_THROW(ToolException("MHI EDL switch feature not supported"));
}

// ----------------------------------------------------------------------------
// isMemoryUsageCritical
//
/// Check if system memory usage is critical.
// ----------------------------------------------------------------------------
bool Manager::isMemoryUsageCritical()
{
   return m_pStatusReportManager->isMemoryUsageCritical();
}

// ----------------------------------------------------------------------------
// isCpuUsageCritical
//
/// Check if CPU usage is critical.
// ----------------------------------------------------------------------------
bool Manager::isCpuUsageCritical()
{
   return m_pStatusReportManager->isCpuUsageCritical();
}

// ----------------------------------------------------------------------------
// removeConnectedDevice
//
/// Remove connected device
// ----------------------------------------------------------------------------
void Manager::removeConnectedDevice(const DeviceHandle deviceHandle)
{
   FLOG_INFO("Started cleanup for connected device Handle: " + std::to_string(deviceHandle));

   std::lock_guard<std::recursive_mutex> lock(m_deviceManagerMutex);

   Manager::DeviceList::const_iterator it;
   it = m_devices.find(deviceHandle);
   if(m_devices.end() == it)
   {
      FLOG_ERROR("Device not found in connected devices list =  " + std::to_string(deviceHandle));
      return;
   }

   m_devices.erase(it);
   FLOG_INFO("Removed from connected devices list");
}

} // namespace Device

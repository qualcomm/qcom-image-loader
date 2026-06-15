// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "device/Impl.h"

#include "device/Exception.h"
#include "device/Manager.h"
#include "protocol/Base.h"


namespace Device {

static const Protocol::Handle PROTOCOL_HANDLE_INCREMENT = TOOLS_UI64(0x0000000100000000);

// ----------------------------------------------------------------------------
// Impl
//
// ----------------------------------------------------------------------------
Impl::Impl(DeviceHandle handle, const std::string& description, const std::string& uniqueIdentifier)
: m_handle(handle)
, m_description(description)
, m_uniqueIdentifier(uniqueIdentifier)
, m_serialNumberMsm()
, m_serialNumberAdb()
, m_socVersion()
, m_vid()
, m_pid()
, m_edlChipId()
, m_protocols()
, m_unavailableProtocols()
, m_nextProtocolHandle(handle + PROTOCOL_HANDLE_INCREMENT)
, m_mutex()
, m_hwId()
{
}

// ----------------------------------------------------------------------------
// ~Impl
//
// ----------------------------------------------------------------------------
Impl::~Impl()
{
   ProtocolList::const_iterator it = m_protocols.begin();
   ProtocolList::const_iterator end = m_protocols.end();
   for(; it != end; ++it)
   {
      TOOLS_IGNORE_EXCEPTIONS(
         (*it)->unsubscribeAsyncEvents(Device::Manager::getInstance().get(), &Device::Manager::onProtocolStateChange)
      );
      /*   TOOLS_IGNORE_EXCEPTIONS(
            (*it)->unsubscribe(
               Device::Manager::getInstance().get(),
               &Device::Manager::onProtocolFlowControlStatusChange
            )
         );
         TOOLS_IGNORE_EXCEPTIONS(
            (*it)->unsubscribe(
               Device::Manager::getInstance().get(),
               &Device::Manager::onProtocolLockStatusChange
            )
         );
         TOOLS_IGNORE_EXCEPTIONS(
            (*it)->unsubscribeAsyncEvents(
               Device::Manager::getInstance().get(),
               &Device::Manager::onProtocolMbnDownloadStatusChange
            )
         );*/
   }
   it = m_unavailableProtocols.begin();
   end = m_unavailableProtocols.end();
   for(; it != end; ++it)
   {
      TOOLS_IGNORE_EXCEPTIONS(
         (*it)->unsubscribeAsyncEvents(Device::Manager::getInstance().get(), &Device::Manager::onProtocolStateChange)
      );
      /*   TOOLS_IGNORE_EXCEPTIONS(
            (*it)->unsubscribe(
               Device::Manager::getInstance().get(),
               &Device::Manager::onProtocolFlowControlStatusChange
            )
         );
         TOOLS_IGNORE_EXCEPTIONS(
            (*it)->unsubscribe(
               Device::Manager::getInstance().get(),
               &Device::Manager::onProtocolLockStatusChange
            )
         );
         TOOLS_IGNORE_EXCEPTIONS(
            (*it)->unsubscribeAsyncEvents(
               Device::Manager::getInstance().get(),
               &Device::Manager::onProtocolMbnDownloadStatusChange
            )
         );*/
   }
}

// ----------------------------------------------------------------------------
// setHwId
//
/// Sets the hwId for the device
// ----------------------------------------------------------------------------
void Impl::setHwId(const std::string& hwId)
{
   m_hwId = hwId;
}

// ----------------------------------------------------------------------------
// getDescription
//
/// @returns The description of the device
// ----------------------------------------------------------------------------
std::string Impl::getDescription() const
{
   return m_description;
}

// ----------------------------------------------------------------------------
// setDescription
//
/// Sets the description for the device
// ----------------------------------------------------------------------------
void Impl::setDescription(const std::string& description)
{
   m_description = description;
}

// ----------------------------------------------------------------------------
// getUniqueIdentifier
//
/// @returns A unique identifier for this device to help with grouping protocols
// ----------------------------------------------------------------------------
std::string Impl::getUniqueIdentifier() const
{
   return m_uniqueIdentifier;
}

// ----------------------------------------------------------------------------
// setUniqueIdentifier
//
/// Set the unique identifier for this device to help with grouping protocols
// ----------------------------------------------------------------------------
void Impl::setUniqueIdentifier(const std::string& uniqueIdentifier)
{
   m_uniqueIdentifier = uniqueIdentifier;
}

// ----------------------------------------------------------------------------
// getDevicePath
//
/// @returns Device path for this device to help with grouping protocols
// ----------------------------------------------------------------------------
std::string Impl::getDevicePath() const
{
   return m_devicePath;
}

// ----------------------------------------------------------------------------
// setDevicePath
//
/// Set device path for this device to help with grouping protocols
// ----------------------------------------------------------------------------
void Impl::setDevicePath(const std::string& devicePath)
{
   m_devicePath = devicePath;
}

// ----------------------------------------------------------------------------
// getSerialNumberMsm
//
/// @returns The serial number of the device
// ----------------------------------------------------------------------------
std::string Impl::getSerialNumberMsm() const
{
   return m_serialNumberMsm;
}

// ----------------------------------------------------------------------------
// setSerialNumberMsm
//
/// Set the device serial number
// ----------------------------------------------------------------------------
void Impl::setSerialNumberMsm(const std::string& serialNumberMsm)
{
   m_serialNumberMsm = serialNumberMsm;
}

// ----------------------------------------------------------------------------
// getSerialNumberAdb
//
/// @returns The ADB serial number of the device
// ----------------------------------------------------------------------------
std::string Impl::getSerialNumberAdb() const
{
   return m_serialNumberAdb;
}

// ----------------------------------------------------------------------------
// setSerialNumberAdb
//
/// Set the device serial number
// ----------------------------------------------------------------------------
void Impl::setSerialNumberAdb(const std::string& serialNumberAdb)
{
   m_serialNumberAdb = serialNumberAdb;
}


// ----------------------------------------------------------------------------
// getSocVersion
//
/// @returns The SocVersion of the device
// ----------------------------------------------------------------------------
std::string Impl::getSocVersion() const
{
   return m_socVersion;
}

// ----------------------------------------------------------------------------
// setSocVersion
//
/// Set the device SocVersion
// ----------------------------------------------------------------------------
void Impl::setSocVersion(const std::string& socVersion)
{
   m_socVersion = socVersion;
}

// ----------------------------------------------------------------------------
// getVid
//
/// @returns The vendor id (vid) of the device
// ----------------------------------------------------------------------------
std::string Impl::getVid() const
{
   return m_vid;
}

// ----------------------------------------------------------------------------
// setVid
//
/// Set the vendor id (vid)
// ----------------------------------------------------------------------------
void Impl::setVid(const std::string& vid)
{
   m_vid = vid;
}


// ----------------------------------------------------------------------------
// getPid
//
/// @returns The product id (pid) of the device
// ----------------------------------------------------------------------------
std::string Impl::getPid() const
{
   return m_pid;
}

// ----------------------------------------------------------------------------
// setPid
//
/// Set the product id (pid)
// ----------------------------------------------------------------------------
void Impl::setPid(const std::string& pid)
{
   m_pid = pid;
}

// ----------------------------------------------------------------------------
// getPid
//
/// @returns The EDL chip ID of the device
// ----------------------------------------------------------------------------
std::string Impl::getEdlChipId() const
{
   return m_edlChipId;
}

// ----------------------------------------------------------------------------
// setPid
//
/// Set the EDL chip ID
// ----------------------------------------------------------------------------
void Impl::setEdlChipId(const std::string& edlChipId)
{
   if(!edlChipId.empty())
   {
      if(!m_edlChipId.empty() && m_edlChipId.compare(edlChipId) != 0)
      {
         FLOG_WARNING("EDL chip ID updated from: " + m_edlChipId + " to: " + edlChipId);
      }
      m_edlChipId = edlChipId;
   }
}

// ----------------------------------------------------------------------------
// getDevicePhysicalLocation
//
/// @returns Device Physical Location
// ----------------------------------------------------------------------------
std::string Impl::getDevicePhysicalLocation() const
{
   return m_devicePhysicalLocation;
}

// ----------------------------------------------------------------------------
// setDevicePhysicalLocation
//
/// Set the Device Physical Location
// ----------------------------------------------------------------------------
void Impl::setDevicePhysicalLocation(const std::string& devicePhysicalLocation)
{
   if(!devicePhysicalLocation.empty())
   {
      if(!m_devicePhysicalLocation.empty() && m_devicePhysicalLocation.compare(devicePhysicalLocation) != 0)
      {
         FLOG_WARNING(
            "Device Physical Location updated from: " + m_devicePhysicalLocation + " to: " + devicePhysicalLocation
         );
      }
      m_devicePhysicalLocation = devicePhysicalLocation;
   }
}

// ----------------------------------------------------------------------------
// getHandle
//
/// @returns The handle for this device object
// ----------------------------------------------------------------------------
DeviceHandle Impl::getHandle() const
{
   return m_handle;
}

// ----------------------------------------------------------------------------
// getProtocolCount
//
/// @returns The number of available protocols on this device
// ----------------------------------------------------------------------------
size_t Impl::getProtocolCount() const
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   return m_protocols.size();
}

// ----------------------------------------------------------------------------
// getProtocolList
//
/// @returns The number of available protocols on this device
// ----------------------------------------------------------------------------
Impl::ProtocolList Impl::getProtocolList() const
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   return m_protocols;
}

// ----------------------------------------------------------------------------
// getUnavailableProtocolList
//
/// @returns The number of unavailable protocols on this device
// ----------------------------------------------------------------------------
Impl::ProtocolList Impl::getUnavailableProtocolList() const
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   return m_unavailableProtocols;
}

// ----------------------------------------------------------------------------
// searchAvailableProtocolByDescriptionAndIdentifier
//
/// Search the protocol based on common IO description and identifier
// ----------------------------------------------------------------------------
bool Impl::
   searchAvailableProtocolByDescriptionAndIdentifier(const std::string& description, const std::string& identifier)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   for(ProtocolList::iterator it = m_protocols.begin(); m_protocols.end() != it; ++it)
   {
      Protocol::BasePtr pProtocol = *it;
      if(!description.empty() && !pProtocol->getCommonIoDescription().empty() &&
         description == pProtocol->getCommonIoDescription().c_str() &&
         identifier == pProtocol->getCommonIoIdentifier().c_str())
      {
         return true;
      }
   }

   return false;
}

// ----------------------------------------------------------------------------
// searchUnavailableProtocolByDescription
//
/// Search the protocol based on common IO description
// ----------------------------------------------------------------------------
bool Impl::searchUnavailableProtocolByDescription(const std::string& description)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   for(ProtocolList::iterator it = m_unavailableProtocols.begin(); m_unavailableProtocols.end() != it; ++it)
   {
      Protocol::BasePtr pProtocol = *it;
      if(!description.empty() && !pProtocol->getCommonIoDescription().empty() &&
         description == pProtocol->getCommonIoDescription().c_str())
      {
         return true;
      }
   }

   return false;
}

// ----------------------------------------------------------------------------
// addProtocol
//
/// Adds the protocol and updates the handle
// ----------------------------------------------------------------------------
void Impl::addProtocol(const Protocol::BasePtr& pProtocol)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   ProtocolList::iterator it = std::find(m_unavailableProtocols.begin(), m_unavailableProtocols.end(), pProtocol);

   if(m_unavailableProtocols.end() == it)
   {
      if(Protocol::Base::INVALID_HANDLE == pProtocol->getHandle())
      {
         pProtocol->setHandle(shared_from_this(), m_nextProtocolHandle);
         m_nextProtocolHandle += PROTOCOL_HANDLE_INCREMENT;
      }
      else
      {
         pProtocol->setHandle(shared_from_this(), pProtocol->getHandle());
      }
      TOOLS_ASSUMING(pProtocol->subscribeForAsyncEvents(
         Device::Manager::getInstance().get(),
         &Device::Manager::onProtocolStateChange
      ));
      /*  TOOLS_ASSUMING(
           pProtocol->subscribe(
              Device::Manager::getInstance().get(),
              &Device::Manager::onProtocolFlowControlStatusChange
           )
        );
        TOOLS_ASSUMING(
           pProtocol->subscribe(
              Device::Manager::getInstance().get(),
              &Device::Manager::onProtocolLockStatusChange
           )
        );
        TOOLS_IGNORE_EXCEPTIONS(
           pProtocol->subscribeForAsyncEvents(
              Device::Manager::getInstance().get(),
              &Device::Manager::onProtocolMbnDownloadStatusChange
           )
        );*/
      m_protocols.push_back(pProtocol);
   }
   else
   {
      Protocol::BasePtr pExistingProtocol = *it;
      m_unavailableProtocols.erase(it);
      m_protocols.push_back(pExistingProtocol);
   }

   FLOG_INFO(("==-==>>>> Device: " + toString(getHandle()) + ", device description: " + toString(getDescription()) +
              " : protocol added: " + pProtocol->getDescription() +
              " : protocol handle: " + toString(pProtocol->getHandle()))
                .c_str());
}

// ----------------------------------------------------------------------------
// removeProtocol
//
/// Moves the protocol to being unavailable
// ----------------------------------------------------------------------------
void Impl::removeProtocol(const Protocol::BasePtr& pProtocol)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_protocols.remove(pProtocol);
   m_unavailableProtocols.push_back(pProtocol);

   FLOG_INFO(("<<<<==-== Device: " + toString(getHandle()) + ", device description: " + toString(getDescription()) +
              " : protocol removed: " + pProtocol->getDescription() +
              " : protocol handle: " + toString(pProtocol->getHandle()))
                .c_str());
}

// ----------------------------------------------------------------------------
// moveProtocolIn
//
/// Add a protocol removed from another device
// ----------------------------------------------------------------------------
void Impl::moveProtocolIn(const Protocol::BasePtr& pProtocol, bool bIsConnected)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   if(bIsConnected)
   {
      m_protocols.push_back(pProtocol);
   }
   else
   {
      m_unavailableProtocols.push_back(pProtocol);
   }

   FLOG_INFO(("==-==>>>> Device: " + toString(getHandle()) + ", device description: " + toString(getDescription()) +
              " : protocol moved in: " + pProtocol->getDescription() +
              " : protocol handle: " + toString(pProtocol->getHandle()))
                .c_str());
}

// ----------------------------------------------------------------------------
// moveProtocolOut
//
/// Remove a protocol out so that it can be added to another device
/// using moveProtocolIn()
// ----------------------------------------------------------------------------
void Impl::moveProtocolOut(const Protocol::BasePtr& pProtocol, bool isConnected)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   if(isConnected)
   {
      m_protocols.remove(pProtocol);
   }
   else
   {
      m_unavailableProtocols.remove(pProtocol);
   }

   FLOG_INFO(("<<<<==-== Device: " + toString(getHandle()) + ", device description: " + toString(getDescription()) +
              " : protocol move out: " + pProtocol->getDescription() +
              " : protocol handle: " + toString(pProtocol->getHandle()))
                .c_str());
}


} // namespace Device

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "Definitions.h"
#include "device/Exception.h"
#include "device/Fwd.h"
#include "util/AppEvent.h"
#include "util/AppMessage.h"

typedef int64_t EventId;
class ServiceEvent;

// ----------------------------------------------------------------------------
// ServiceStatusChangeEvent
//
/// Event from a service being added or removed
// ----------------------------------------------------------------------------
class ServiceEvent : public Util::AsyncEvent
{
   TOOLS_FORBID_COPY(ServiceEvent);

public:
   ServiceEvent(const std::string& serviceName, EventId eventId, const std::string& eventDescription)
   : Util::AsyncEvent()
   , m_serviceName(serviceName)
   , m_eventId(eventId)
   , m_eventDescription(eventDescription)
   {
   }
   virtual ~ServiceEvent()
   {
   }

   std::string getServiceName() const
   {
      return m_serviceName;
   }
   EventId getEventId() const
   {
      return m_eventId;
   }
   std::string getEventDescription() const
   {
      return m_eventDescription;
   }

private:
   std::string m_serviceName;      ///< Service that invoked the event
   EventId m_eventId;              ///< Event specific ID
   std::string m_eventDescription; ///< Optional description
};

namespace Service {
class ServiceHandlerBase
: public Util::EventPublisher
, public Util::AsyncEventPublisher
{
   TOOLS_FORBID_COPY(ServiceHandlerBase);

public:
   ~ServiceHandlerBase()
   {
   }

   virtual QC::ErrorCode::type initializeService() = 0;
   virtual QC::ErrorCode::type destroyService() = 0;

   std::string getName() const
   {
      return m_serviceName;
   }

   // ----------------------------------------------------------------------------
   // isInitialized
   //
   /// @returns Whether the initializeService() has been called
   // ----------------------------------------------------------------------------
   bool isInitialized() const
   {
      return m_bInitialized;
   }

protected:
   ServiceHandlerBase(std::string serviceName)
   : m_bInitialized(false)
   , m_serviceName(serviceName)
   {
   }

   std::string m_serviceName;

   std::shared_ptr<Util::IMessagePublisher> m_pPublisher;
   Device::Exception::ErrorCode m_lastError = Device::Exception::ErrorCode::DEVICE_NO_ERROR; ///< Error tracked by the
                                                                                             ///< class calling
   std::string m_lastErrorString = "No Error string";
   bool m_bInitialized; ///< Check whether initializeService() was called

   // ----------------------------------------------------------------------------
   // setInitialized
   //
   /// Marks the service as being initialized
   // ----------------------------------------------------------------------------
   void setInitialized()
   {
      TOOLS_ASSERT_OR_THROW(
         !m_bInitialized,
         Device::Exception(
            Device::Exception::DEVICE_SERVICE_ALREADY_INITIALIZED,
            std::string("Service already initialized. If initialization is "
                        "needed on a "
                        "different protocol, create a new service for it.")
         )
      );
      m_bInitialized = true;
   }

   // ----------------------------------------------------------------------------
   // sendEvent
   //
   // ----------------------------------------------------------------------------
   void sendEvent(EventId eventId, const std::string& description)
   {
      notifyAsync(std::make_shared<ServiceEvent>(getName(), eventId, description));
   }
};
} // namespace Service

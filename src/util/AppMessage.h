// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "device/Exception.h"
#include "util/AppEvent.h"

#include <memory>
#include <string>

namespace Util {

class MessageLevel
{
public:
   enum Enum
   {
      INFO = 0,
      WARNING = 1,
      EXCEPTION = 2,
      CRITICAL = 3,
      TRACE = 4
   };

   MessageLevel()
   : m_value(INFO)
   {
   }
   MessageLevel(Enum v)
   : m_value(v)
   {
   } // intentionally implicit

   int toStorage() const noexcept
   {
      return static_cast<int>(m_value);
   }
   Enum getValue() const noexcept
   {
      return m_value;
   }

   bool operator==(const MessageLevel& o) const noexcept
   {
      return m_value == o.m_value;
   }
   bool operator!=(const MessageLevel& o) const noexcept
   {
      return m_value != o.m_value;
   }
   bool operator<(const MessageLevel& o) const noexcept
   {
      return m_value < o.m_value;
   }
   bool operator<=(const MessageLevel& o) const noexcept
   {
      return m_value <= o.m_value;
   }
   bool operator>(const MessageLevel& o) const noexcept
   {
      return m_value > o.m_value;
   }
   bool operator>=(const MessageLevel& o) const noexcept
   {
      return m_value >= o.m_value;
   }

private:
   Enum m_value;
};

class Message : public Util::Event
{
public:
   using Level = MessageLevel;

   static std::shared_ptr<Message> create(
      const Level& level = Level::INFO,
      const std::string& location = {},
      const std::string& title = {},
      const std::string& description = {}
   )
   {
      return std::make_shared<Message>(level, location, title, description);
   }

   explicit Message(
      const Level& level = Level::INFO,
      const std::string& location = {},
      const std::string& title = {},
      const std::string& description = {}
   )
   : m_level(level)
   , m_location(location)
   , m_title(title)
   , m_description(description)
   {
   }

   ~Message() override = default;

   Message& setLevel(const Level& l)
   {
      m_level = l;
      return *this;
   }
   const Level& getLevel() const
   {
      return m_level;
   }

   Message& setLocation(const std::string& s)
   {
      m_location = s;
      return *this;
   }
   const std::string& getLocation() const
   {
      return m_location;
   }

   Message& setTitle(const std::string& s)
   {
      m_title = s;
      return *this;
   }
   const std::string& getTitle() const
   {
      return m_title;
   }

   Message& setDescription(const std::string& s)
   {
      m_description = s;
      return *this;
   }
   const std::string& getDescription() const
   {
      return m_description;
   }

   std::string toString() const
   {
      return "[" + std::to_string(m_level.toStorage()) + "] " + m_location + ": " + m_title + " - " + m_description;
   }

private:
   Level m_level;
   std::string m_location;
   std::string m_title;
   std::string m_description;
};

using MessagePtr = std::shared_ptr<Message>;
using const_MessagePtr = std::shared_ptr<const Message>;

class IMessagePublisher
{
public:
   IMessagePublisher() = default;
   virtual ~IMessagePublisher() = default;
   IMessagePublisher(const IMessagePublisher&) = delete;
   IMessagePublisher& operator=(const IMessagePublisher&) = delete;

   virtual void send(const MessagePtr& pMessage) = 0;

   virtual void send(
      const Message::Level& level,
      const std::string& location,
      const std::string& title,
      const std::string& description = {}
   ) = 0;

   virtual void report(const ToolException& e, const std::string& catchLocation) = 0;
};

class MessagePublisher
: public Publisher<Message>
, public IMessagePublisher
{
public:
   static MessagePublisher& getInstance()
   {
      static MessagePublisher instance;
      return instance;
   }

   void send(const MessagePtr& pMessage) override
   {
      notify(pMessage);
   }

   void send(
      const Message::Level& level,
      const std::string& location,
      const std::string& title,
      const std::string& description = {}
   ) override
   {
      send(Message::create(level, location, title, description));
   }

   void report(const ToolException& e, const std::string& catchLocation) override
   {
      send(Message::Level::EXCEPTION, e.where(), e.what(), "Caught at: " + catchLocation);
   }

private:
   MessagePublisher() = default;
   ~MessagePublisher() = default;
   MessagePublisher(const MessagePublisher&) = delete;
   MessagePublisher& operator=(const MessagePublisher&) = delete;
};

} // namespace Util

#ifndef APP_REPORT_EXCEPTION
#define APP_REPORT_EXCEPTION(name) ::Util::MessagePublisher::getInstance().report((name), __FILE__)
#endif

#ifndef APP_PUBLISH_EXCEPTION
#define APP_PUBLISH_EXCEPTION(pPublisher, name) (pPublisher)->report((name), __FILE__)
#endif

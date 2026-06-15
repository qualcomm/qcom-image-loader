// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once
#include "platform/LangMacros.h"
#if defined TOOLS_TARGET_WINDOWS
#include <eh.h>
#include <windows.h>
#endif
#include <exception>
#include <string>
#include <typeinfo>

#define ToolException Tool::ErrorException
#define TOOLS_TRACE Tool::Trace(TOOLS_SOURCE_LOCATION())
// --------------------------------------------------------------------------
// TOOLS_CATCH
//
/// Catch most commonly thrown exception types
// --------------------------------------------------------------------------
#define TOOLS_CATCH(name, action)                                                                                      \
   catch(const ToolException& name)                                                                                    \
   {                                                                                                                   \
      TOOLS_IGNORE_EXCEPTIONS(action);                                                                                 \
   }                                                                                                                   \
   catch(const std::exception& TOOLS_ANONYMOUS_IDENTIFIER(ex))                                                         \
   {                                                                                                                   \
      TOOLS_IGNORE_EXCEPTIONS(ToolException name(TOOLS_ANONYMOUS_IDENTIFIER(ex), ToolException::defaultLocation());    \
                              action);                                                                                 \
   }                                                                                                                   \
   catch(...)                                                                                                          \
   {                                                                                                                   \
      TOOLS_IGNORE_EXCEPTIONS(ToolException name; action);                                                             \
   }

// --------------------------------------------------------------------------
// TOOLS_IGNORE_EXCEPTIONS
//
/// In debug mode, issues a trace message when an exception is caught
/// In release mode, silently consumes exceptions
// --------------------------------------------------------------------------
#ifdef TOOLS_MODE_DEBUG
#define TOOLS_IGNORE_EXCEPTIONS(stmt)                                                                                  \
   do                                                                                                                  \
   {                                                                                                                   \
      try                                                                                                              \
      {                                                                                                                \
         stmt;                                                                                                         \
      }                                                                                                                \
      catch(const std::exception& TOOLS_ANONYMOUS_IDENTIFIER(igEx))                                                    \
      {                                                                                                                \
         try                                                                                                           \
         {                                                                                                             \
            TOOLS_TRACE(                                                                                               \
               _T("%s: Ignored exception: %s\r\n"),                                                                    \
               TOOLS_SOURCE_LOCATION(),                                                                                \
               TOOLS_ANONYMOUS_IDENTIFIER(igEx).what()                                                                 \
            );                                                                                                         \
         }                                                                                                             \
         catch(...)                                                                                                    \
         {                                                                                                             \
         }                                                                                                             \
      }                                                                                                                \
      catch(...)                                                                                                       \
      {                                                                                                                \
         try                                                                                                           \
         {                                                                                                             \
            TOOLS_TRACE(_T("%s: Ignored unknown exception\r\n"), TOOLS_SOURCE_LOCATION());                             \
         }                                                                                                             \
         catch(...)                                                                                                    \
         {                                                                                                             \
         }                                                                                                             \
      }                                                                                                                \
   } while(TOOLS_FALSE())
#else
#define TOOLS_IGNORE_EXCEPTIONS(stmt)                                                                                  \
   do                                                                                                                  \
   {                                                                                                                   \
      try                                                                                                              \
      {                                                                                                                \
         stmt;                                                                                                         \
      }                                                                                                                \
      catch(...)                                                                                                       \
      {                                                                                                                \
      }                                                                                                                \
   } while(TOOLS_FALSE())
#endif

// --------------------------------------------------------------------------
// TOOLS_THROW
//
/// Throw the given exception.
// --------------------------------------------------------------------------
#ifdef TOOLS_MODE_DEBUG
#define TOOLS_THROW(except)                                                                                            \
   TOOLS_TRACE(_T("Exception: %s"), except.what());                                                                    \
   TOOLS_TRACE(_T(" ,%s%s"), TOOLS_SOURCE_LOCATION(), _T(": Throwing ") _T(TOOLS_STR(except)) _T("\r\n"));             \
   throw Tool::throwHelper(except, TOOLS_SOURCE_LOCATION())
#else
#define TOOLS_THROW(except) throw Tool::throwHelper(except, TOOLS_SOURCE_LOCATION())
#endif

namespace Tool {
// --------------------------------------------------------------------------
// Exception
//
// --------------------------------------------------------------------------

class ErrorException : public std::exception
{
public:
   static const int32_t defaultErrorCode()
   {
      return -1;
   }
   static const std::string& defaultMessage()
   {
      static const std::string UNKNOWN = "Unknown Exception";
      return UNKNOWN;
   }
   static const std::string& defaultLocation()
   {
      static const std::string UNKNOWN = "Unknown Location";
      return UNKNOWN;
   }
   static const std::string& defaultCallstack()
   {
      static const std::string UNKNOWN = "Unknown Stack";
      return UNKNOWN;
   }
   ErrorException(
      const std::string& message = defaultMessage(),
      const std::string& location = defaultLocation(),
      const std::string& callstack = defaultCallstack()
   )
   : m_errorCode(-1)
   , m_what(message)
   , m_where(location)
   , m_callStack(callstack)
   {
   }

   ErrorException(
      const int32_t code,
      const std::string& message = defaultMessage(),
      const std::string& location = defaultLocation(),
      const std::string& callstack = defaultCallstack()
   )
   : m_errorCode(code)
   , m_what(message)
   , m_where(location)
   , m_callStack(callstack)
   {
   }

   ErrorException(
      const std::exception& src,
      const std::string& location = defaultLocation(),
      const std::string& callstack = defaultCallstack()

   )
   : std::exception(src)
   , m_errorCode(defaultErrorCode())
   , m_what(NULL != src.what() ? src.what() : "")
   , m_where(location)
   , m_callStack(callstack)
   {
   }
   ErrorException& operator=(const ErrorException& src)
   {
      if(this == &src)
      {
         return *this;
      }

      std::exception::operator=(src);
      m_errorCode = src.errorCode();
      m_what = std::string(src.what());
      m_where = std::string(src.where());
      m_callStack = std::string(src.callStack());
      return *this;
   }

   ErrorException& operator=(const std::exception& src)
   {
      if(this == &src)
      {
         return *this;
      }

      std::exception::operator=(src);
      m_errorCode = defaultErrorCode();
      m_what = src.what();
      m_where = defaultLocation();

      return *this;
   }

   virtual const int32_t errorCode() const
   {
      return m_errorCode;
   }

   // Override what() to provide error description
   virtual const char* what() const throw()
   {
      try
      {
         return m_what.c_str();
      }
      catch(...)
      {
         return "Exception while getting exception message.";
      }
   }

   virtual const char* where() const throw()
   {
      try
      {
         return m_where.c_str();
      }
      catch(...)
      {
         return "Exception while getting exception location.";
      }
   }

   virtual ErrorException& where(const std::string& location)
   {
      m_where = location;
      return *this;
   }

   virtual const char* callStack() const throw()
   {
      try
      {
         return m_callStack.c_str();
      }
      catch(...)
      {
         return _T("Exception while getting callStack.");
      }
   }

private:
   int32_t m_errorCode;
   std::string m_what;
   std::string m_where;
   std::string m_callStack;
};

class NullPointerException : public ErrorException
{
public:
   NullPointerException(const std::type_info& type, const std::string& location = defaultLocation())
   : ErrorException(std::string("NULL ") + std::string(type.name()) + std::string(" pointer"), location)
   {
   }
};

// --------------------------------------------------------------------------
// Util namespace
//
/// Utility functions for exception handling
// --------------------------------------------------------------------------
#if defined TOOLS_TARGET_WINDOWS

// --------------------------------------------------------------------------
// seTranslator
//
/// Translates Structured exceptions to C++ Device exceptions
// --------------------------------------------------------------------------
inline void seTranslator(unsigned code, void* pInfo)
{
   EXCEPTION_POINTERS* pExInfo = static_cast<EXCEPTION_POINTERS*>(pInfo);
   char msg[128] = {0};

   switch(code)
   {
      case EXCEPTION_ACCESS_VIOLATION:
         sprintf_s(msg, sizeof(msg), "Access violation at 0x%p", pExInfo->ExceptionRecord->ExceptionAddress);
         throw ToolException(msg);
         break;

      case EXCEPTION_INT_DIVIDE_BY_ZERO:
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
         sprintf_s(msg, sizeof(msg), "Divide by zero at 0x%p", pExInfo->ExceptionRecord->ExceptionAddress);
         throw ToolException(msg);
         break;

      default:
         sprintf_s(
            msg,
            sizeof(msg),
            "Structured exception at 0x%p: 0x%X",
            pExInfo->ExceptionRecord->ExceptionAddress,
            code
         );
         throw ToolException(msg);
   }
}

// --------------------------------------------------------------------------
// setExceptionTranslator
//
/// Sets the structured exception handler translator
/// Converts Windows structured exceptions (SEH) into C++ Device::Exception
// --------------------------------------------------------------------------
inline void setExceptionTranslator()
{
   _set_se_translator(reinterpret_cast<_se_translator_function>(&seTranslator));
}

#else

// --------------------------------------------------------------------------
// setExceptionTranslator
//
/// No-Op for non-Windows platforms
// --------------------------------------------------------------------------
inline void setExceptionTranslator()
{
   // No structured exception handling on non-Windows platforms
}

#endif
// #TODO tobe finished
class Trace
{
public:
   Trace(const char* pLocation)
   : m_location(pLocation)
   {
   }

   void operator()(const char* fmt, ...) const
   {
      (void)fmt; // Suppress unused parameter warning
      // va_list pArgs;
      // va_start(pArgs, fmt);
      // Lang::Debugger::getInstance().output(
      //    Lang::formatv(fmt, pArgs)
      //);
   }

private:
   std::string m_location;
};

// #TODO: tobe finished
template <typename _ExceptionT>
inline const _ExceptionT& throwHelper(const _ExceptionT& e, const char* location)
{
   const_cast<_ExceptionT&>(e).where(location);

   return e;
}

// #TODO: tobe finished
inline void assertion(const char* msg, const char* fileName, const int32_t line, const char* function) throw()
{
   // Suppress unused parameter warnings
   (void)msg;
   (void)fileName;
   (void)line;
   (void)function;

   try
   {
   }
   catch(...)
   {
      // Not very likely, not much we can do
   }
}
} // namespace Tool

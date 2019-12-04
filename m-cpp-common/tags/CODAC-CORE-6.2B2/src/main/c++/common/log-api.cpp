/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/common/log-api.cpp $
* $Id: log-api.cpp 100872 2019-07-18 09:23:57Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <stdarg.h> // va_start, etc.
#include <syslog.h> // Log message severity

// Local header files

#include "BasicTypes.h" // Misc. type definition, e.g. RET_STATUS
#include "SysTools.h" // Misc. helper functions, e.g. ccs::HelperTools::SafeStringCopy, etc.

#include "log-api.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::log"

// Type definition

namespace ccs {

namespace log {

// Function declaration

// Global variables

// WARNING - The following implies that static initialization can not use the logging callback since
//           the linking does not guarantee the initialization order and the use of the callback pointer
//           can therefore happen before the pointer is initialized.

static Func_t func_ptr = &vMessage2Syslog;
static Severity_t filter_level = LOG_INFO;

// Function definition

void vMessage2Stdout (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, va_list args) 
{ 

  if (severity > filter_level)
    {
      return;
    }

  ccs::types::char8 buffer [1024];
  ccs::types::char8* p_buf = static_cast<ccs::types::char8*>(buffer);
  ccs::types::uint32 size = 1024u;

  switch (severity) 
    {
      case LOG_TRACE:   snprintf(p_buf, size, "[%s] ", "TRACE");     break;
      case LOG_DEBUG:   snprintf(p_buf, size, "[%s] ", "DEBUG");     break;
      case LOG_INFO:    snprintf(p_buf, size, "[%s] ", "INFO");      break;
      case LOG_NOTICE:  snprintf(p_buf, size, "[%s] ", "NOTICE");    break;
      case LOG_WARNING: snprintf(p_buf, size, "[%s] ", "WARNING");   break;
      case LOG_ERR:     snprintf(p_buf, size, "[%s] ", "ERROR");     break;
      case LOG_CRIT:    snprintf(p_buf, size, "[%s] ", "CRITICAL");  break;
      case LOG_ALERT:   snprintf(p_buf, size, "[%s] ", "ALERT");     break;
      case LOG_EMERG:   snprintf(p_buf, size, "[%s] ", "EMERGENCY"); break;
      default: snprintf(p_buf, size, "[%s] ", "UNDEF"); break;
    }

  // Re-align pointer
  size -= static_cast<ccs::types::uint32>(strlen(p_buf)); 
  p_buf += static_cast<ccs::types::uint32>(strlen(p_buf)); 

  (void)vsnprintf(p_buf, size, message, args);

  // Re-align pointer 
  size -= static_cast<ccs::types::uint32>(strlen(p_buf)); 
  p_buf += static_cast<ccs::types::uint32>(strlen(p_buf)); 

  if (true != ccs::HelperTools::IsUndefinedString(source))
    {
      (void)snprintf(p_buf, size, "[%s]", source);
    }

  (void)ccs::HelperTools::SafeStringCopy(p_buf, "\n", size); 

  // Re-align pointer
  size -= static_cast<ccs::types::uint32>(strlen(p_buf)); 
  p_buf += static_cast<ccs::types::uint32>(strlen(p_buf)); 

  // Ensures proper termination even in case of overlong buffer
  buffer[1022] = '\n';
  buffer[1023] = '\0';

  (void)fprintf(stdout, buffer);

  return; 

}

void vMessage2Syslog (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, va_list args) 
{ 

  if (severity > filter_level)
    {
      return;
    }

  ccs::types::char8 buffer [1024];
  ccs::types::char8* p_buf = static_cast<ccs::types::char8*>(buffer);
  ccs::types::uint32 size = 1024u;

  (void)snprintf(p_buf, size, "log-lib:%d]", ccs::HelperTools::GetProcessId()); 

  // Re-align pointer
  size -= strlen(p_buf); 
  p_buf += strlen(p_buf); 

  if (true != ccs::HelperTools::IsUndefinedString(source))
    {
      (void)snprintf(p_buf, size, "[%s]", source);
    }
  else 
    {
      (void)ccs::HelperTools::SafeStringCopy(p_buf, "[]", size);
    }

  // Re-align pointer
  size -= strlen(p_buf); 
  p_buf += strlen(p_buf); 

  switch (severity)
    {
      case LOG_TRACE:   snprintf(p_buf, size, "[%s] ", "trace");     break;
      case LOG_DEBUG:   snprintf(p_buf, size, "[%s] ", "debug");     break;
      case LOG_INFO:    snprintf(p_buf, size, "[%s] ", "info");      break;
      case LOG_NOTICE:  snprintf(p_buf, size, "[%s] ", "notice");    break;
      case LOG_WARNING: snprintf(p_buf, size, "[%s] ", "warning");   break;
      case LOG_ERR:     snprintf(p_buf, size, "[%s] ", "error");     break;
      case LOG_CRIT:    snprintf(p_buf, size, "[%s] ", "critical");  break;
      case LOG_ALERT:   snprintf(p_buf, size, "[%s] ", "alert");     break;
      case LOG_EMERG:   snprintf(p_buf, size, "[%s] ", "emergency"); break;
      default: snprintf(p_buf, size, "[%s] ", "undef"); break;
    }

  // Re-align pointer
  size -= strlen(p_buf); 
  p_buf += strlen(p_buf); 

  (void)vsnprintf(p_buf, size, message, args);

  // Re-align pointer
  size -= strlen(p_buf); 
  p_buf += strlen(p_buf); 

  if (severity == LOG_TRACE) // Does not exist with syslog
    {
      severity = LOG_DEBUG;
    }

  (void)syslog(severity, buffer); 

  return; 

}

// cppcheck-suppress unusedFunction // Used by logging library which replaces the backend method
void vMessage (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, va_list args)
{

  if (NULL_PTR_CAST(Func_t) != func_ptr)
    {
      (*func_ptr)(severity, source, message, args); 
    }

  return; 

}

void Message (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, ...)
{ 

  if (NULL_PTR_CAST(Func_t) != func_ptr) 
    { 
      va_list args;

      va_start(args, message); 
      (*func_ptr)(severity, source, message, args); 
      va_end(args); 
    } 

  return; 

}

// Initializer methods

// Accessor methods

Func_t SetCallback (Func_t cb)
{ 

  Func_t old_ptr = func_ptr; 
  func_ptr = cb; 

  return old_ptr; 

}

Severity_t SetFilter (Severity_t level)
{ 

  Severity_t old_level = filter_level; 
  filter_level = level; 

  return old_level; 

}

// Miscellaneous methods

// Constructor methods

// Destructor method

// Display methods

} // namespace log

} // namespace ccs

// Bug 11846 - Duplicated symbols with logging library
void ccs_log_msg (int severity, const char * const source, const char * const message, ...)
{
  
  if (NULL_PTR_CAST(ccs::log::Func_t) != ccs::log::func_ptr) 
    { 
      va_list args; 

      va_start(args, message); 
      (*ccs::log::func_ptr)(static_cast<ccs::log::Severity_t>(severity), source, message, args); 
      va_end(args); 
    }

  return;

}

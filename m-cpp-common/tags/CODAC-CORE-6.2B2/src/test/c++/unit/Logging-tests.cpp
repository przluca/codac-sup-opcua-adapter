/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Logging-tests.cpp $
* $Id: Logging-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Unit test code
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <stdio.h> // sscanf, printf, etc.
#include <string.h> // strncpy, etc.
#include <stdarg.h> // va_start, etc.
#include <signal.h> // sigset, etc.

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "types.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#define LOG_TRACE_ENABLE
#define LOG_DEBUG_ENABLE
#include "log-api.h" // Syslog wrapper routines

// Constants

// Type definition

// Global variables

static ccs::types::uint32 __counter = 0u;
static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

void UserDefinedHandler (ccs::log::Severity_t severity, const char* source, const char* message, va_list args);

// Function definition

void UserDefinedHandler (ccs::log::Severity_t severity, const char* source, const char* message, va_list args)
{
  __counter += 1u;
  ccs::log::vMessage2Stdout(severity, source, message, args);
  return;
}

TEST(Logging_Test, SetCallback)
{

  ccs::log::Func_t handler = ccs::log::SetCallback(&UserDefinedHandler);
  __counter = 0u;

  bool ret = (&ccs::log::vMessage2Stdout && handler); // Static initialisation .. see above

  if (ret)
    {
      ccs::log::Message(LOG_TRACE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_DEBUG, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_INFO, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_NOTICE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_WARNING, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ERROR, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_CRITICAL, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ALERT, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_EMERGENCY, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ret = (9u == __counter);
    }

  ccs::log::SetSyslog();

  if (ret)
    {
      ccs::log::Message(LOG_TRACE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_DEBUG, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_INFO, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_NOTICE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_WARNING, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ERROR, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_CRITICAL, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ALERT, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_EMERGENCY, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(1234u, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
    }

  ccs::log::SetCallback(handler); // Restore

  if (ret)
    {
      ccs::log::Message(LOG_TRACE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_DEBUG, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_INFO, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_NOTICE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_WARNING, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ERROR, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_CRITICAL, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ALERT, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_EMERGENCY, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(1234u, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
    }

  ASSERT_EQ(true, ret);
}

TEST(Logging_Test, SetFilter)
{

  ccs::log::Severity_t filter = ccs::log::SetFilter(LOG_TRACE);

  bool ret = (LOG_INFO == filter);

  if (ret)
    {
      ccs::log::Message(LOG_TRACE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_DEBUG, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_INFO, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_NOTICE, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_WARNING, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ERROR, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_CRITICAL, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_ALERT, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
      ccs::log::Message(LOG_EMERGENCY, "ccs::tests", "Log message at '%lu'", ccs::HelperTools::GetCurrentTime());
    }

  ccs::log::SetFilter(filter); // Restore

  ASSERT_EQ(true, ret);
}


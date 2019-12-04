/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10833-test.cpp $
* $Id: Bug10833-test.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

/* Global header files */

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include <gtest/gtest.h> /* Google test framework */

/* Local header files */

#include "types.h" /* Misc. type definition */

#define LOG_TRACE_ENABLE
#define LOG_DEBUG_ENABLE
#include "log-api.h"

/* Constants */

/* Type definition */

class Bug10833_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug10833_Test, Default)
{

  log_trace("TEST(Bug10833_Test, Default) - Entering test");

  bool ret = true;

  log_info("TEST(Bug10833_Test, Default) - Actual test");

  log_trace("TEST(Bug10833_Test, Default) - Finishing test");

  ASSERT_EQ(ret, true);
}

TEST(Bug10833_Test, Seven)
{

  bool ret = true;

  log_debug("TEST(Bug10833_Test, Seven) - '%d %d %d %d %d %d %d'", 0, 1, 2, 3, 4, 5, 6);

  ASSERT_EQ(ret, true);
}

TEST(Bug10833_Test, Eight)
{

  bool ret = true;

  log_debug("TEST(Bug10833_Test, Eight) - '%d %d %d %d %d %d %d %d'", 0, 1, 2, 3, 4, 5, 6, 7);

  ASSERT_EQ(ret, true);
}

TEST(Bug10833_Test, Nine)
{

  bool ret = true;

  log_info("TEST(Bug10833_Test, Nine) - '%d %d %d %d %d %d %d %d %d'", 0, 1, 2, 3, 4, 5, 6, 7, 8);

  ASSERT_EQ(ret, true);
}

TEST(Bug10833_Test, Ten)
{

  bool ret = true;

  log_info("TEST(Bug10833_Test, Ten) - '%d %d %d %d %d %d %d %d %d %d'", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

  ASSERT_EQ(ret, true);
}

TEST(Bug10833_Test, Twelve)
{

  bool ret = true;

  log_info("TEST(Bug10833_Test, Twelve) - '%d %d %d %d %d %d %d %d %d %d %d %d'", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);

  ASSERT_EQ(ret, true);
}


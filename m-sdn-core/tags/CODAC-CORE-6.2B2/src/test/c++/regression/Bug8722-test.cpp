/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug8722-test.cpp $
* $Id: Bug8722-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-types.h" /* Misc. type definition */
#include "sdn-tools.h" /* Misc. helper functions */

#include "sdn-base.h" /* Misc. class definition */

#include "sdn-api.h" /* SDN core library - API definition */

/* Constants */

/* Type definition */

class Bug8722_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug8722_Test, Success)
{
#if 0 // Unsure about environment variables on Jenkins
  bool ret = (sdn::HelperTools::IsInterfaceValid() == true); // Default environment variable
#else
  bool ret = true;
#endif
  if (ret)
    {
      ret = (setenv(SDN_INTERFACE_ENVVAR, DEFAULT_IFACE_NAME, 1) == 0); // Valid environment variable
    }

  if (ret)
    {
      ret = (sdn::HelperTools::IsInterfaceValid() == true);
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug8722_Test, Failure)
{
#if 0 // Unsure about environment variables on Jenkins
  bool ret = (sdn::HelperTools::IsInterfaceValid() == true); // Default environment variable
#else
  bool ret = true;
#endif
  if (ret)
    {
      ret = (setenv(SDN_INTERFACE_ENVVAR, "invalid", 1) == 0); // Invalid environment variable
    }

  if (ret)
    {
      ret = (sdn::HelperTools::IsInterfaceValid() == false);
    }

  if (ret)
    {
      ret = (setenv(SDN_INTERFACE_ENVVAR, DEFAULT_IFACE_NAME, 1) == 0); // Valid environment variable
    }

  ASSERT_EQ(ret, true);
}


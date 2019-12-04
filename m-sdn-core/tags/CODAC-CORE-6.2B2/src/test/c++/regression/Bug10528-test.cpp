/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10528-test.cpp $
* $Id: Bug10528-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-api.h" /* SDN core library - API definition */
#include "sdn-topic-xml.h"

/* Constants */

/* Type definition */

class Bug10528_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug10528_Test, Success)
{
  bool ret = (sdn::TopicHelper::LocateDefinitionFile("payload-64") == STATUS_SUCCESS);

  ASSERT_EQ(ret, true);
}

TEST(Bug10528_Test, Failure)
{
  bool ret = (sdn::TopicHelper::LocateDefinitionFile("payload-64.xml") == STATUS_ERROR);

  ASSERT_EQ(ret, true);
}


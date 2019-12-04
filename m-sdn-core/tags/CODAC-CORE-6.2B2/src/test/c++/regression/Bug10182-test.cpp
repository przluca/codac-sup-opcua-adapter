/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10182-test.cpp $
* $Id: Bug10182-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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
#include "sdn-header.h"

/* Constants */

/* Type definition */

class Bug10182_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug10182_Test, LegacyAPI)
{

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "TopicName", 64u);

  /* Create topic from name and size */
  log_info("TEST(Bug10182_Test, LegacyAPI) - Create '%s' topic with size '%u'", mdata.name, mdata.size);

  sdn::Topic* topic = static_cast<sdn::Topic*>(NULL);

  bool ret = ((topic = new (std::nothrow) sdn::Topic (mdata)) != static_cast<sdn::Topic*>(NULL));

  if (ret)
    {
      log_info("TEST(Bug9354_Test, LegacyAPI) - Using topic mapping '%s:%u' for '%s'", topic->GetMCastGroup(), topic->GetMCastPort(), topic->GetName());
    }

  /* Generate mapping from name */
  char group [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  uint_t port = 0u;

  if (ret)
    {
      ret = (sdn::TopicHelper::GenerateMCastAddress(mdata.name, group, port) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((strcmp(topic->GetMCastGroup(), group) == 0) && (topic->GetMCastPort() == port));
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10182_Test, IntrospectXML)
{
  bool ret = true;

  /* Deleted - New mechanism covered by unit tests */ 

  ASSERT_EQ(ret, true);
}


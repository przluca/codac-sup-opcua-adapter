/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug11538-test.cpp $
* $Id: Bug11538-test.cpp 97187 2019-01-24 20:02:39Z bauvirb $
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
#include "sdn-topic-xml.h"

/* Constants */

/* Type definition */

class Bug11538_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug11538_Test, Default)
{

  ccs::log::Severity_t old_sev = ccs::log::SetFilter(LOG_TRACE);

  sdn::Topic topic;

  char file_path [256] = STRING_UNDEFINED;

  bool ret = (sdn::TopicHelper::LocateDefinitionFile("BUG-11538:TOPIC", file_path) == STATUS_SUCCESS);

  if (ret)
    {
      log_info("TEST(Bug11538_Test, Default) - Using topic definition file '%s'", file_path);
    }

  // Create topic
  if (ret)
    {
      ret = (sdn::TopicHelper::LoadDefinitionFile(&topic, file_path) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic.Configure() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((topic.GetSize() == 11538u) &&
	     (strcmp(topic.GetName(), "BUG-11538:TOPIC") == 0));
    }

  ccs::log::SetFilter(old_sev);

  if (ret)
    {
      char buffer [2048]; topic.SerializeType(buffer, 2048);
    }

  /* Create Publisher using topic instance */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (topic)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  if (ret)
    {
      ret = (pub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (pub->Configure() == STATUS_SUCCESS);
    }

  /* Create Subscriber using topic instance */
  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (topic)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = (sub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sub->Configure() == STATUS_SUCCESS);
    }

  /* ToDo - Test communication */

  delete pub;
  delete sub;

  ASSERT_EQ(ret, true);
}


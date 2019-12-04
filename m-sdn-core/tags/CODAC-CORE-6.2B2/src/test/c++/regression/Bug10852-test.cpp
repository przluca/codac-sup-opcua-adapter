/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10852-test.cpp $
* $Id: Bug10852-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-types.h" // Misc. type definition
#include "sdn-tools.h" // Misc. helper functions

#include "sdn-base.h" // Misc. class definition
#include "sdn-header.h"
#include "sdn-packet.h"

#include "sdn-api.h" // SDN core library - API definition

#include "sdn-topic-xml.h"

// Constants

// Type definition

class Bug10852_Test
{

  public:

  protected:

};

// Global variables

// Function declaration

// Function definition

TEST(Bug10852_Test, Success)
{

  char type_def [] = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<topic name=\"Bug10852-topic\" description=\"Topic definition file (see https://bugzilla.iter.org/codac/show_bug.cgi?id=10852)\" version=\"1\" uid=\"0\">"
    "  <attributes>"
    "    <attribute name=\"reserved\" dataType=\"uint8\" multiplicity=\"1024\"/>"
    "  </attributes>"
    "</topic>";

  sdn::Topic* p_topic = sdn::TopicHelper::InstantiateFromXMLContent(type_def);

  bool ret = (static_cast<sdn::Topic*>(NULL) != p_topic);

  if (ret)
    {
      ret = (p_topic->GetSize() == 1024u);
    }

  if (ret)
    {
      ret = (p_topic->HasDescription() == true);
    }

  if (ret)
    {
      log_info("TEST(Bug10852_Test, Success) - Description '%s' of size '%u'", p_topic->GetDescription(), strlen(p_topic->GetDescription()));
      ret = ((strlen(p_topic->GetDescription()) > (STRING_MAX_LENGTH - 1u)) &&
	     (strlen(p_topic->GetDescription()) == strlen("Topic definition file (see https://bugzilla.iter.org/codac/show_bug.cgi?id=10852)")));
    }

  if (ret)
    {
      delete p_topic;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10852_Test, Failure)
{

  char type_def [] = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<topic name=\"Bug10852-topic\" version=\"1\" uid=\"0\">"
    "  <attributes>"
    "    <attribute name=\"reserved\" dataType=\"uint8\" multiplicity=\"1024\"/>"
    "  </attributes>"
    "</topic>";

  sdn::Topic* p_topic = sdn::TopicHelper::InstantiateFromXMLContent(type_def);

  bool ret = (static_cast<sdn::Topic*>(NULL) != p_topic);

  if (ret)
    {
      ret = (p_topic->GetSize() == 1024u);
    }

  if (ret)
    {
      ret = (p_topic->HasDescription() == false);
    }

  if (ret)
    {
      log_info("TEST(Bug10852_Test, Failure) - Description '%s' of size '%u'", p_topic->GetDescription(), strlen(p_topic->GetDescription()));
      ret = (IsUndefined(p_topic->GetDescription()) == true);
    }

  if (ret)
    {
      delete p_topic;
    }

  ASSERT_EQ(ret, true);
}


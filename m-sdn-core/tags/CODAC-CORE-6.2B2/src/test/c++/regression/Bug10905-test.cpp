/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10905-test.cpp $
* $Id: Bug10905-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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
#include "sdn-header.h"
#include "sdn-packet.h"

#include "sdn-api.h" /* SDN core library - API definition */

#include "sdn-topic-xml.h"

/* Constants */

/* Type definition */

class Bug10905_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug10905_Test, Success)
{

  char content [] = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<topic name=\"Bug10905-topic\" description=\"Topic definition file (see https://bugzilla.iter.org/codac/show_bug.cgi?id=10905)\" version=\"1\" uid=\"10905\" mapping=\"239.0.109.5:10905\">"
    "  <attributes>"
    "    <attribute name=\"timestamp\" description=\"Instance time\" qualifier=\"timestamp\" unit=\"ns\" dataType=\"uint64\"/>"
    "    <attribute name=\"reserved\" dataType=\"uint8\" multiplicity=\"1016\"/>"
    "  </attributes>"
    "</topic>";

  sdn::Topic* p_topic = sdn::TopicHelper::InstantiateFromXMLContent(content);

  bool ret = (static_cast<sdn::Topic*>(NULL) != p_topic);

  if (ret)
    {
      ret = (strcmp(p_topic->GetName(), "Bug10905-topic")  == 0);
    }

  if (ret)
    {
      ret = (p_topic->HasDescription() == true);
    }

  if (ret)
    {
      ret = (p_topic->GetVersion() == 1u);
    }

  if (ret)
    {
      ret = (p_topic->GetSize() == 1024u);
    }

  if (ret)
    {
      ret = (p_topic->GetUID() == 10905u);
    }

  if (ret)
    {
      ret = ((strcmp(p_topic->GetMCastGroup(), "239.0.109.5") == 0) && (p_topic->GetMCastPort() == 10905u));
    }

  sdn::base::AnyType* type_def = static_cast<sdn::base::AnyType*>(NULL);

  if (ret)
    {
      type_def = p_topic->GetTypeDefinition();
      ret = (static_cast<sdn::base::AnyType*>(NULL) != type_def);
    }

  if (ret)
    {
      ret = ((type_def->HasAttribute("timestamp") == true) && 
	     (type_def->GetAttributeRank("timestamp") == 0u) &&
	     (strcmp(type_def->GetAttributeDescription("timestamp"), "Instance time") == 0) &&
	     (strcmp(type_def->GetAttributeQualifier("timestamp"), "timestamp") == 0) &&
	     (strcmp(type_def->GetAttributeUnit("timestamp"), "ns") == 0) &&
	     (type_def->GetAttributeType("timestamp") == ANYTYPE_ATTR_UINT64) && // ToDo - Provide better way to identify type
	     (type_def->ccs::base::AnyType::GetAttributeMultiplicity("timestamp") == 1u) &&
	     (type_def->GetAttributeSize("timestamp") == 8u));
    }

  if (ret)
    {
      ret = ((type_def->HasAttribute("reserved") == true) && 
	     (type_def->GetAttributeRank("reserved") == 1u) &&
	     (type_def->GetAttributeDescription("reserved") == NULL) &&
	     (type_def->GetAttributeQualifier("reserved") == NULL) &&
	     (type_def->GetAttributeUnit("reserved") == NULL) &&
	     (type_def->GetAttributeType("reserved") == ANYTYPE_ATTR_UINT8) && // ToDo - Provide better way to identify type
	     (type_def->ccs::base::AnyType::GetAttributeMultiplicity("reserved") == 1016u) &&
	     (type_def->GetAttributeSize("reserved") == 1016u));
    }

  if (ret)
    {
      delete p_topic;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10905_Test, Failure_empty)
{

  char type_def [] = "";
  sdn::Topic* p_topic = sdn::TopicHelper::InstantiateFromXMLContent(type_def);

  bool ret = (static_cast<sdn::Topic*>(NULL) == p_topic);

  ASSERT_EQ(ret, true);
}

TEST(Bug10905_Test, Failure_schema)
{

  char type_def [] = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<topic name=\"Bug10905-topic\" description=\"Topic definition file (see https://bugzilla.iter.org/codac/show_bug.cgi?id=10905)\" version=\"1\" uid=\"10905\">"
    "  <attributes>"
    "    <attribute name=\"reserved\" dataType=\"uint8\" multiplicity=\"1024\"/>"
    "  </attributes>"
    "<topic>"; // Note the missing '/'

  sdn::Topic* p_topic = sdn::TopicHelper::InstantiateFromXMLContent(type_def);

  bool ret = (static_cast<sdn::Topic*>(NULL) == p_topic);

  ASSERT_EQ(ret, true);
}


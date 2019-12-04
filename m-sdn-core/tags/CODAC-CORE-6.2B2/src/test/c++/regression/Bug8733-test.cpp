/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug8733-test.cpp $
* $Id: Bug8733-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

class Bug8733_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug8733_Test, Default) // Bug 8733 - Issue with attributes at the same rank
{

  char content [] = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<topic name=\"Bug8733-topic\" description=\"Topic definition file (see https://bugzilla.iter.org/codac/show_bug.cgi?id=8733)\" version=\"1\">"
    "  <attributes>"
    "    <attribute rank=\"2\" name=\"packet-counter\" dataType=\"uint64_t\" qualifier=\"samplenb\"/>"
    "    <attribute rank=\"3\" name=\"packet-timestamp\" dataType=\"uint64_t\" qualifier=\"timestamp\"/>"
    "    <attribute rank=\"4\" name=\"f1@rank4\" dataType=\"float\"/>"
    "    <attribute rank=\"4\" name=\"f2@rank4\" dataType=\"float\"/>"
    "    <attribute rank=\"5\" name=\"f3@rank5\" dataType=\"float\"/>"
    "    <attribute rank=\"7\" name=\"f4@rank7\" dataType=\"float\"/>"
    "  </attributes>"
    "</topic>";

  sdn::Topic* p_topic = sdn::TopicHelper::InstantiateFromXMLContent(content);

  bool ret = (static_cast<sdn::Topic*>(NULL) == p_topic); // Note test criterion

  if (ret)
    {
      // Force topic creation .. ignoring 
      sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "Bug9833-topic", 0u);

      p_topic = new (std::nothrow) sdn::Topic; p_topic->SetMetadata(mdata);
      ret = (static_cast<sdn::Topic*>(NULL) != p_topic);
    }

  if (ret)
    {
      ret = ((p_topic->AddAttribute<uint64_t>(2u, "packet-counter") == STATUS_SUCCESS) && 
	     (p_topic->AddAttribute<uint64_t>(3u, "packet-timestamp") == STATUS_SUCCESS) &&
	     (p_topic->AddAttribute<float>(4u, "f1@rank4") == STATUS_SUCCESS) &&
	     (p_topic->AddAttribute<float>(4u, "f2@rank4") != STATUS_SUCCESS) && // Note test criterion
	     (p_topic->AddAttribute<float>(5u, "f3@rank5") == STATUS_SUCCESS) &&
	     (p_topic->AddAttribute<float>(7u, "f4@rank7") == STATUS_SUCCESS));
    }

  sdn::base::AnyType* type_def = static_cast<sdn::base::AnyType*>(NULL);

  if (ret)
    {
      type_def = p_topic->GetTypeDefinition();
      ret = (static_cast<sdn::base::AnyType*>(NULL) != type_def);
    }

  if (ret)
    {
      ret = ((type_def->AddExtAttribute("2", "packet-counter", NULL, "samplenb", NULL, NULL) == STATUS_SUCCESS) && 
	     (type_def->AddExtAttribute("3", "packet-timestamp", NULL, "timestamp", NULL, NULL) == STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = (p_topic->Configure() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (p_topic->IsInitialized() == true);
    }

  if (ret)
    {
      fprintf(stdout, "%% INFO - Topic '%s' size '%u'\n", p_topic->GetName(), p_topic->GetSize());

      for (uint_t index = 0u; index < type_def->GetRank(); index += 1u)
	{
	  fprintf(stdout, "%% %u (index) - %s %s", index, ccs::base::AnyType_GetTypeName(type_def->GetAttributeType(index)), type_def->GetAttributeName(index)); 

	  if (type_def->ccs::base::AnyType::GetAttributeMultiplicity(index) > 1) fprintf(stdout, " [%u];", type_def->ccs::base::AnyType::GetAttributeMultiplicity(index)); 
	  else fprintf(stdout, ";");

	  if (type_def->GetAttributeQualifier(index) != NULL) fprintf(stdout, " /* Qualifier '%s' */", type_def->GetAttributeQualifier(index)); 

	  fprintf(stdout, "\n");
	}
    }

  const char* attr_name = NULL;

  if (ret)
    {
      attr_name = p_topic->HasInstanceIndex();
      ret = ((NULL != attr_name) && (strcmp(attr_name, "packet-counter") == 0));
    }

   if (ret)
    {
      attr_name = p_topic->HasInstanceTimestamp();
      ret = ((NULL != attr_name) && (strcmp(attr_name, "packet-timestamp") == 0));
    }

  if (ret)
    {
      ret = ((type_def->HasAttribute("f1@rank4") == true) && (type_def->HasAttribute("f2@rank4") == false));
    }

  if (ret)
    {
      delete p_topic;
    }

  ASSERT_EQ(ret, true);
}

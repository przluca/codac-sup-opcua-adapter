/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug9524-test.cpp $
* $Id: Bug9524-test.cpp 99885 2019-05-21 14:27:58Z bauvirb $
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

#include "sdn-topic-xml.h"

/* Constants */

/* Type definition */

typedef struct Bug9524_Data {

  char content [1024];
  bool terminate; // WARNING - Asynchronous thread goes through the body once more upon AnyThread::Terminate
  bool status;

} Bug9524_Data_t;

class Bug9524_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

void Bug9524_Subscriber (Bug9524_Data_t* data)
{

  if (data->terminate) return;

  sdn::Topic* sub_topic = sdn::TopicHelper::InstantiateFromXMLContent(data->content);

  bool ret = (static_cast<sdn::Topic*>(NULL) != sub_topic);

  if (ret)
    {
      ret = (sub_topic->IsInitialized() == true);
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = (static_cast<sdn::Subscriber*>(NULL) != (sub = new (std::nothrow) sdn::Subscriber (*sub_topic)));
    }

  if (ret)
    {
      ret = ((sub->SetInterface("lo") == STATUS_SUCCESS) && (sub->Configure() == STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = (sub->Receive(1000000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      log_info("Bug9524_Subscriber - Receive successful");
    }
  else
    {
      log_error("Bug9524_Subscriber - Failed to receive");
    }

  if (ret)
    {
      delete sub;
      delete sub_topic;
    }

  data->status = ret;

  return;

}

TEST(Bug9524_Test, Default) // Bug 9524 - Issue with re-entrancy of the XML parser
{

  char content [] = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<topic name=\"Bug9524-topic\" description=\"Topic definition file (see https://bugzilla.iter.org/codac/show_bug.cgi?id=9524)\" version=\"1\">"
    "  <attributes>"
    "    <attribute name=\"reserved\" dataType=\"uint8_t\" multiplicity=\"128\"/>"
    "  </attributes>"
    "</topic>";

  sdn::Topic* pub_topic = sdn::TopicHelper::InstantiateFromXMLContent(content);
  sdn::Topic* sub_topic = sdn::TopicHelper::InstantiateFromXMLContent(content);

  bool ret = ((static_cast<sdn::Topic*>(NULL) != pub_topic) && (static_cast<sdn::Topic*>(NULL) != sub_topic));

  if (ret)
    {
      ret = ((pub_topic->IsInitialized() == true) && (sub_topic->IsInitialized() == true));
    }

  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = (static_cast<sdn::Publisher*>(NULL) != (pub = new (std::nothrow) sdn::Publisher (*pub_topic)));
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = (static_cast<sdn::Subscriber*>(NULL) != (sub = new (std::nothrow) sdn::Subscriber (*sub_topic)));
    }

  if (ret)
    {
      ret = ((pub->SetInterface("lo") == STATUS_SUCCESS) && (sub->SetInterface("lo") == STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = ((pub->Configure() == STATUS_SUCCESS) && (sub->Configure() == STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = (pub->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sub->Receive(0ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      delete pub;
      delete sub;
      delete pub_topic;
      delete sub_topic;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug9524_Test, Multithreaded) // Bug 9524 - Issue with re-entrancy of the XML parser
{

  char content [] = 
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<topic name=\"Bug9524-topic\" description=\"Topic definition file (see https://bugzilla.iter.org/codac/show_bug.cgi?id=9524)\" version=\"1\">"
    "  <attributes>"
    "    <attribute name=\"reserved\" dataType=\"uint8_t\" multiplicity=\"128\"/>"
    "  </attributes>"
    "</topic>";

  sdn::TopicHelper::InitializeParser();

  sdn::Topic* pub_topic = sdn::TopicHelper::InstantiateFromXMLContent(content);

  bool ret = (static_cast<sdn::Topic*>(NULL) != pub_topic);

  if (ret)
    {
      ret = (pub_topic->IsInitialized() == true);
    }

  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = (static_cast<sdn::Publisher*>(NULL) != (pub = new (std::nothrow) sdn::Publisher (*pub_topic)));
    }

  if (ret)
    {
      ret = ((pub->SetInterface("lo") == STATUS_SUCCESS) && (pub->Configure() == STATUS_SUCCESS));
    }

  // Start subscriber threads
  Bug9524_Data_t t1_data; t1_data.terminate = false; t1_data.status = false;
  sstrncpy(t1_data.content, content, 1024u);

  ccs::base::TriggeredThreadWithCallback* t1 = static_cast<ccs::base::TriggeredThreadWithCallback*>(NULL);

  if (ret)
    {
      ret = (static_cast<ccs::base::TriggeredThreadWithCallback*>(NULL) != (t1 = new (std::nothrow) ccs::base::TriggeredThreadWithCallback));
    }

  if (ret)
    {
      ret = (t1->SetCallback((void(*)(void*)) &Bug9524_Subscriber, static_cast<void*>(&t1_data)) == STATUS_SUCCESS);
    }

  Bug9524_Data_t t2_data; t2_data.terminate = false; t2_data.status = false;
  sstrncpy(t2_data.content, content, 1024u);

  ccs::base::TriggeredThreadWithCallback* t2 = static_cast<ccs::base::TriggeredThreadWithCallback*>(NULL);

  if (ret)
    {
      ret = (static_cast<ccs::base::TriggeredThreadWithCallback*>(NULL) != (t2 = new (std::nothrow) ccs::base::TriggeredThreadWithCallback));
    }

  if (ret)
    {
      ret = (t2->SetCallback((void(*)(void*)) &Bug9524_Subscriber, static_cast<void*>(&t2_data)) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (t1->Launch() && t2->Launch());
    }

  if (ret)
    {
      ret = (t1->Trigger() && t2->Trigger());
    }

  if (ret)
    {
      wait_for(100000000ul);
      log_info("TEST(Bug9524_Test, Multithreaded) - Publish ..");
      ret = (pub->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    { // WARNING - Asynchronous thread goes through the body once more upon AnyThread::Terminate
      t1_data.terminate = true; t2_data.terminate = true;
      ret = (t1->Terminate() && t2->Terminate());
    }

  if (ret)
    {
      ret = ((t1_data.status == true) && (t2_data.status == true));
    }

  sdn::TopicHelper::ReleaseParser();

  if (ret)
    {
      delete pub;
      delete pub_topic;
    }

  ASSERT_EQ(ret, true);
}

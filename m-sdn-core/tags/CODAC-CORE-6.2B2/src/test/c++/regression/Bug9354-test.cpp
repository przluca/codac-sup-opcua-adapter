/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug9354-test.cpp $
* $Id: Bug9354-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

typedef struct {

  uint64_t index;
  uint64_t timestamp;
  char buffer [STRING_MAX_LENGTH];
  uint8_t reserved [8];

} LegacyTopic_t;

class Bug9354_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug9354_Test, LegacyAPI)
{

  ccs::log::Severity_t old_sev = ccs::log::SetFilter(LOG_TRACE);

  /* 
   * Case 1 - SDN legacy code is creating topic and participants by name and size and
   *          uses static cast on the payload to map to a C-like structure.
   */
    
  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "LegacyTopic", sizeof(LegacyTopic_t));

  /* Create topic */
  log_info("TEST(Bug9354_Test, LegacyAPI) - Create '%s' topic with size '%u'", mdata.name, mdata.size);

  sdn::Topic* topic = static_cast<sdn::Topic*>(NULL);

  bool ret = ((topic = new (std::nothrow) sdn::Topic (mdata)) != static_cast<sdn::Topic*>(NULL));

  if (ret)
    {
      char buffer [2048]; topic->SerializeType(buffer, 2048);
      log_info("TEST(Bug9354_Test, LegacyAPI) - Using topic datatype definition - '%s'", buffer);
    }

  /* Create Publisher using metadata */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  if (ret)
    {
      ret = (pub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (pub->Configure() == STATUS_SUCCESS);
    }

  /* Create Subscriber using metadata */
  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = (sub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sub->Configure() == STATUS_SUCCESS);
    }

  /* Publish topic instance */
  LegacyTopic_t pub_t; memset(&pub_t, 0, sizeof(LegacyTopic_t));

  if (ret)
    {      
      pub_t.index = 1ul;
      pub_t.timestamp = get_time();
      sstrncpy(pub_t.buffer, "Hello world!", STRING_MAX_LENGTH);

      ret = (pub->CopyTopicInstance(&pub_t, sizeof(LegacyTopic_t)) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (pub->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    {
      log_info("TEST(Bug9354_Test, LegacyAPI) - Published '%s' topic with payload '%lu %lu %s'", mdata.name, pub_t.index, pub_t.timestamp, pub_t.buffer);
    }
  else
    {
      log_error("TEST(Bug9354_Test, LegacyAPI) - sdn::Publisher::Publish failed");
    }

  /* Receive topic instance */
  if (ret)
    {
      ret = (sub->Receive(1000000000ul) == STATUS_SUCCESS);
    }

  LegacyTopic_t sub_t; memset(&sub_t, 0, sizeof(LegacyTopic_t));

  if (ret)
    {
      ret = (sub->CopyTopicInstance(&sub_t, sizeof(LegacyTopic_t)) == STATUS_SUCCESS);
    }

  if (ret)
    {
      log_info("TEST(Bug9354_Test, LegacyAPI) - Received '%s' topic with payload '%lu %lu %s'", mdata.name, sub_t.index, sub_t.timestamp, sub_t.buffer);
    }
  else
    {
      log_error("TEST(Bug9354_Test, LegacyAPI) - sdn::Subscriber::Receive failed");
    }

  if (ret)
    {
      ret = ((pub_t.index == sub_t.index) && (pub_t.timestamp == sub_t.timestamp));
    }

  delete pub;
  delete sub;
  delete topic;

  ccs::log::SetFilter(old_sev);

  ASSERT_EQ(ret, true);
}

TEST(Bug9354_Test, MARTeDataSource_publisher)
{

  ccs::log::Severity_t old_sev = ccs::log::SetFilter(LOG_TRACE);

  /*
   * Case 2 - MARTe publisher uses own parser to define topic from application-specific configuration
   *          file.
   */

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MARTeTopic", 0u);

  /* Create topic */
  log_info("TEST(Bug9354_Test, MARTeDataSource_publisher) - Create '%s' topic with size '%u'", mdata.name, mdata.size);

  sdn::Topic* topic = static_cast<sdn::Topic*>(NULL);

  bool ret = ((topic = new (std::nothrow) sdn::Topic) != static_cast<sdn::Topic*>(NULL));

  if (ret)
    {
      topic->SetMetadata(mdata);
    }

  /* Programatically define topic datatype */
  if (ret)
    {
      ret = (topic->AddAttribute<uint64_t>(0u, "index") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic->AddAttribute<uint64_t>(1u, "timestamp") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic->AddAttribute<char>(2u, "buffer", STRING_MAX_LENGTH) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic->AddAttribute<uint8_t>(3u, "reserved", 8) == STATUS_SUCCESS);
    }

  if (ret)
    {
      char buffer [2048]; topic->SerializeType(buffer, 2048);
      log_info("TEST(Bug9354_Test, MARTeDataSource_publisher) - Using topic datatype definition - '%s'", buffer);
    }

  if (ret)
    {
      ret = (topic->Configure() == STATUS_SUCCESS);
    }

  /* Create Publisher with topic instance */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (*topic)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  if (ret)
    {
      ret = (pub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (pub->Configure() == STATUS_SUCCESS);
    }

  /* Create ccs::mcast::Subscriber */
  ccs::mcast::Subscriber* sub = static_cast<ccs::mcast::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) ccs::mcast::Subscriber ("lo", topic->GetMCastGroup(), topic->GetMCastPort())) != static_cast<ccs::mcast::Subscriber*>(NULL)); 
    }

  /* Publish topic instance */
  LegacyTopic_t pub_t; memset(&pub_t, 0, sizeof(LegacyTopic_t));

  if (ret)
    {      
      pub_t.index = 1ul;
      pub_t.timestamp = get_time();
      sstrncpy(pub_t.buffer, "Hello world!", STRING_MAX_LENGTH);

      memcpy(topic->GetTypeDefinition()->GetAttributeReference(0u), &(pub_t.index), sizeof(uint64_t));
      memcpy(topic->GetTypeDefinition()->GetAttributeReference(1u), &(pub_t.timestamp), sizeof(uint64_t));
      memcpy(topic->GetTypeDefinition()->GetAttributeReference(2u), &(pub_t.buffer), STRING_MAX_LENGTH);

      char buffer [2048]; topic->SerializeInstance(buffer, 2048);
      log_info("TEST(Bug9354_Test, MARTeDataSource_publisher) - Using topic instance - '%s'", buffer);
    }

  if (ret)
    {
      ret = (pub->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    {
      log_info("TEST(Bug9354_Test, MARTeDataSource_publisher) - Published '%s' topic with payload '%lu %lu %s'", mdata.name, pub_t.index, pub_t.timestamp, pub_t.buffer);
    }
  else
    {
      log_error("TEST(Bug9354_Test, MARTeDataSource_publisher) - sdn::Publisher::Publish failed");
    }

  /* Receive topic instance */
  uint_t size = sizeof(sdn::Header_t) + sizeof(LegacyTopic_t);
  uint8_t buffer [size]; memset(buffer, 0, size);

  if (ret)
    {
      uint_t recv_size = size;
      ret = (sub->Receive(static_cast<void*>(buffer), recv_size, 1000000000ul) == STATUS_SUCCESS);
    }

  LegacyTopic_t sub_t; memset(&sub_t, 0, sizeof(LegacyTopic_t));

  if (ret)
    {
      memcpy(&sub_t, &buffer[sizeof(sdn::Header_t)], sizeof(LegacyTopic_t));
      log_info("TEST(Bug9354_Test, MARTeDataSource_publisher) - Received '%s' topic with payload '%lu %lu %s'", mdata.name, sub_t.index, sub_t.timestamp, sub_t.buffer);
    }
  else
    {
      log_error("TEST(Bug9354_Test, MARTeDataSource_publisher) - ccs::mcast::Subscriber::Receive failed");
    }

  if (ret)
    {
      ret = ((pub_t.index == sub_t.index) && (pub_t.timestamp == sub_t.timestamp));
    }

  delete pub;
  delete sub;
  delete topic;

  ccs::log::SetFilter(old_sev);

  ASSERT_EQ(ret, true);
}

TEST(Bug9354_Test, MARTeDataSource_subscriber)
{

  ccs::log::Severity_t old_sev = ccs::log::SetFilter(LOG_TRACE);

  /*
   * Case 2 - MARTe subscriber uses own parser to define topic from application-specific configuration
   *          file.
   */

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MARTeTopic", 0u);

  /* Create topic */
  log_info("TEST(Bug9354_Test, MARTeDataSource_subscriber) - Create '%s' topic with size '%u'", mdata.name, mdata.size);

  sdn::Topic* topic = static_cast<sdn::Topic*>(NULL);

  bool ret = ((topic = new (std::nothrow) sdn::Topic) != static_cast<sdn::Topic*>(NULL));

  if (ret)
    {
      topic->SetMetadata(mdata);
    }

  /* Programatically define topic datatype */
  if (ret)
    {
      ret = (topic->AddAttribute<uint64_t>(0u, "index") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic->AddAttribute<uint64_t>(1u, "timestamp") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic->AddAttribute<char>(2u, "buffer", STRING_MAX_LENGTH) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic->AddAttribute<uint8_t>(3u, "reserved", 8) == STATUS_SUCCESS);
    }

  if (ret)
    {
      char buffer [2048]; topic->SerializeType(buffer, 2048);
      log_info("TEST(Bug9354_Test, MARTeDataSource_subscriber) - Using topic datatype definition - '%s'", buffer);
    }

  if (ret)
    {
      ret = (topic->Configure() == STATUS_SUCCESS);
    }

  /* Create ccs::mcast::Publisher */
  ccs::mcast::Publisher* pub = static_cast<ccs::mcast::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) ccs::mcast::Publisher ("lo", topic->GetMCastGroup(), topic->GetMCastPort())) != static_cast<ccs::mcast::Publisher*>(NULL)); 
    }

  /* Create Subscriber with topic instance */
  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (*topic)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = (sub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sub->Configure() == STATUS_SUCCESS);
    }

  /* Publish topic instance */
  uint_t size = sizeof(sdn::Header_t) + sizeof(LegacyTopic_t);
  uint8_t buffer [size]; memset(buffer, 0, size);

  LegacyTopic_t pub_t; memset(&pub_t, 0, sizeof(LegacyTopic_t));

  if (ret)
    {
      sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(buffer);

      sstrncpy(ref->header_uid, "SDNv2.x", 8);
      ref->header_size = htobe32(sizeof(sdn::Header_t));
      ref->topic_uid = htobe32(topic->GetUID());
      ref->topic_version = htobe32(topic->GetVersion());
      ref->topic_size = htobe32(topic->GetSize());
      ref->topic_counter = 0ul;
      ref->send_time = htobe64(get_time());
      ref->recv_time = 0ul;
    }

  if (ret)
    {
      pub_t.index = 1ul;
      pub_t.timestamp = get_time();
      sstrncpy(pub_t.buffer, "Hello world!", STRING_MAX_LENGTH);

      memcpy(&buffer[sizeof(sdn::Header_t)], &pub_t, sizeof(LegacyTopic_t));
    }

  if (ret)
    {
      ret = (pub->Publish(buffer, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      log_info("TEST(Bug9354_Test, MARTeDataSource_subscriber) - Published '%s' topic with payload '%lu %lu %s'", mdata.name, pub_t.index, pub_t.timestamp, pub_t.buffer);
    }
  else
    {
      log_error("TEST(Bug9354_Test, MARTeDataSource_subscriber) - ccs::mcast::Publisher::Publish failed");
    }

  /* Receive topic instance */
  if (ret)
    {
      ret = (sub->Receive(1000000000ul) == STATUS_SUCCESS);
    }

   if (ret)
    {
      char buffer [2048]; topic->SerializeInstance(buffer, 2048);
      log_info("TEST(Bug9354_Test, MARTeDataSource_subscriber) - Received topic instance - '%s' ", buffer);
    }
  else
    {
      log_error("TEST(Bug9354_Test, MARTeDataSource_subscriber) - sdn::Subscriber::Receive failed");
    }

  LegacyTopic_t sub_t; memset(&sub_t, 0, sizeof(LegacyTopic_t));

  if (ret)
    {
      memcpy(&(sub_t.index), topic->GetTypeDefinition()->GetAttributeReference(0u), sizeof(uint64_t));
      memcpy(&(sub_t.timestamp), topic->GetTypeDefinition()->GetAttributeReference(1u), sizeof(uint64_t));
      memcpy(&(sub_t.buffer), topic->GetTypeDefinition()->GetAttributeReference(2u), STRING_MAX_LENGTH);

      log_info("TEST(Bug9354_Test, MARTeDataSource_subscriber) - Received '%s' topic with payload '%lu %lu %s'", mdata.name, sub_t.index, sub_t.timestamp, sub_t.buffer);
    }

  if (ret)
    {
      ret = ((pub_t.index == sub_t.index) && (pub_t.timestamp == sub_t.timestamp));
    }

  delete pub;
  delete sub;
  delete topic;

  ccs::log::SetFilter(old_sev);

  ASSERT_EQ(ret, true);
}

TEST(Bug9354_Test, Introspect)
{

  ccs::log::Severity_t old_sev = ccs::log::SetFilter(LOG_TRACE);

  /* 
   * Case 3 - Topic configured by name through XML definition file.
   */
    
  /* Create topic */
  sdn::Topic topic;

  char file_path [256] = STRING_UNDEFINED;

  bool ret = (sdn::TopicHelper::LocateDefinitionFile("Bug9354-topic", file_path) == STATUS_SUCCESS);

  if (ret)
    {
      ret = (sdn::TopicHelper::LoadDefinitionFile(&topic, file_path) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (topic.Configure() == STATUS_SUCCESS);
    }

  ccs::log::SetFilter(old_sev);

  if (ret)
    {
      char buffer [2048]; topic.SerializeType(buffer, 2048);
      log_info("TEST(Bug9354_Test, Introspect) - Using topic datatype definition - '%s'", buffer);
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


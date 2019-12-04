/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10327-test.cpp $
* $Id: Bug10327-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

/* Constants */

/* Type definition */

typedef struct Bug10327_Topic {

  sdn::types::uint64 counter;
  sdn::types::uint64 timestamp;
  sdn::types::string buffer; // char [64]
  sdn::types::uint8 reserved [16];

} Bug10327_Topic_t;

class Bug10327_Test
{

  public:

    sdn::Topic* topic;
    sdn::Publisher* sdn_pub;
    sdn::Subscriber* sdn_sub;
    ccs::mcast::Publisher* mcast_pub;
    ccs::mcast::Subscriber* mcast_sub;

    bool Initialise (void);

    virtual ~Bug10327_Test (void);

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

bool Bug10327_Test::Initialise (void)
{
  // Initialise attributes
  topic = static_cast<sdn::Topic*>(NULL);
  sdn_pub = static_cast<sdn::Publisher*>(NULL);
  sdn_sub = static_cast<sdn::Subscriber*>(NULL);
  mcast_pub = static_cast<ccs::mcast::Publisher*>(NULL);
  mcast_sub = static_cast<ccs::mcast::Subscriber*>(NULL);

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "Bug10327", sizeof(Bug10327_Topic_t), "239.0.103.27", 10327);

  if (ret)
    {
      ret = ((topic = new (std::nothrow) sdn::Topic (mdata)) != static_cast<sdn::Topic*>(NULL));
    }

  if (ret)
    {
      topic->SetUID(0x01234567u);
      topic->SetVersion(0x89abcdefu);

      ret = ((topic->GetUID() == 0x01234567u) && (topic->GetVersion() == 0x89abcdefu));
    }

  // Instantiate SDN participants
  if (ret)
    {
      ret = ((sdn_pub = new (std::nothrow) sdn::Publisher (*topic)) != static_cast<sdn::Publisher*>(NULL));
    }

  if (ret)
    {
      ret = (sdn_pub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sdn_pub->Configure() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((sdn_sub = new (std::nothrow) sdn::Subscriber (*topic)) != static_cast<sdn::Subscriber*>(NULL));
    }

  if (ret)
    {
      ret = (sdn_sub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sdn_sub->Configure() == STATUS_SUCCESS);
    }

  // Instantiate MCAST participants
  if (ret)
    {
      ret = ((mcast_pub = new (std::nothrow) ccs::mcast::Publisher ("lo", "239.0.103.27", 10327)) != static_cast<ccs::mcast::Publisher*>(NULL));
    }

  if (ret)
    {
      ret = ((mcast_sub = new (std::nothrow) ccs::mcast::Subscriber ("lo", "239.0.103.27", 10327)) != static_cast<ccs::mcast::Subscriber*>(NULL));
    }

  return ret;

};

Bug10327_Test::~Bug10327_Test (void)
{

  // Release resources
  if (mcast_pub != static_cast<ccs::mcast::Publisher*>(NULL))
    {
      delete mcast_pub;
      mcast_pub = static_cast<ccs::mcast::Publisher*>(NULL);
    }

  if (mcast_sub != static_cast<ccs::mcast::Subscriber*>(NULL))
    {
      delete mcast_sub;
      mcast_sub = static_cast<ccs::mcast::Subscriber*>(NULL);
    }

  if (sdn_pub != static_cast<sdn::Publisher*>(NULL))
    {
      delete sdn_pub;
      sdn_pub = static_cast<sdn::Publisher*>(NULL);
    }

  if (sdn_sub != static_cast<sdn::Subscriber*>(NULL))
    {
      delete sdn_sub;
      sdn_sub = static_cast<sdn::Subscriber*>(NULL);
    }

  if (topic != static_cast<sdn::Topic*>(NULL))
    {
      delete topic;
      topic = static_cast<sdn::Topic*>(NULL);
    }

  return;

};

TEST(Bug10327_Test, Homogeneous)
{

  Bug10327_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = ((test.sdn_pub)->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->IsPayloadOrdered() == true);
    }

  if (ret)
    {
      sdn::Header* header = (test.sdn_pub)->m_header;

      uint64_t send_time = header->GetTimestamp();
      uint64_t curr_time = get_time();

      ret = ((curr_time > send_time) && ((curr_time - send_time) < 10000000ul));
    }

  if (ret)
    {
      ret = ((test.sdn_sub)->Receive(10000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_sub)->IsPayloadOrdered() == true);
    }

  if (ret)
    {
      sdn::Header* header = (test.sdn_sub)->m_header;

      uint64_t recv_time = header->GetReceiveTimestamp();
      uint64_t curr_time = get_time();

      ret = ((curr_time > recv_time) && ((curr_time - recv_time) < 10000000ul));
    }

  if (ret)
    {
      sdn::Header* pub_header = (test.sdn_pub)->m_header;
      sdn::Header* sub_header = (test.sdn_sub)->m_header;
      // Publisher updates counter for next packet
      ret = ((pub_header->GetTimestamp() == sub_header->GetTimestamp()) && (pub_header->GetCounter() == sub_header->GetCounter() + 1ul));
    }

  if (ret)
    {
      sdn::Header* header = (test.sdn_pub)->m_header;
      uint64_t counter = header->GetCounter();

      ret = (counter == 1ul);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    {
      sdn::Header* header = (test.sdn_pub)->m_header;
      uint64_t counter = header->GetCounter();

      ret = (counter == 2ul);
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10327_Test, Heterogeneous)
{

  Bug10327_Test test;

  bool ret = test.Initialise();

  uint_t size = 1024 + sizeof(sdn::Header_t);
  uint8_t payload [size];

  if (ret)
    {
      memset(payload, 0, size);

      sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(payload);

      sstrncpy(ref->header_uid, "SDNv2.x", 8);
      ref->header_size = sizeof(sdn::Header_t);
      ref->topic_uid = 0x01234567u;
      ref->topic_version = 0x89abcdefu;
      ref->topic_size = 1024u;
      ref->topic_counter = 0ul;
      ref->send_time = get_time();
      ref->recv_time = 0ul;

      // Convert to network byte order
      ref->topic_uid = sdn::HelperTools::ToNetworkByteOrder(ref->topic_uid);
      ref->topic_version = sdn::HelperTools::ToNetworkByteOrder(ref->topic_version);
      ref->topic_size = sdn::HelperTools::ToNetworkByteOrder(ref->topic_size);
      ref->topic_counter = sdn::HelperTools::ToNetworkByteOrder(ref->topic_counter);
      ref->send_time = sdn::HelperTools::ToNetworkByteOrder(ref->send_time);

      log_info("TEST(Bug10327_Test, Heterogeneous) - Part 1");
      ret = ((test.mcast_pub)->Publish(payload, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      memset(payload, 0, size);

      log_info("TEST(Bug10327_Test, Heterogeneous) - Part 2");
      ret = ((test.mcast_sub)->Receive(payload, size, 10000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(payload);

      if (sdn::HelperTools::IsLittleEndian())
	{
	  // This platform is little-endian - Should have swapped 
	  log_info("TEST(Bug10327_Test, Heterogeneous) - Part 3 - Little-endian");
	  ret = ((ref->topic_uid == 0x67452301u) && (ref->topic_version == 0xefcdab89u));
	}
      else
	{
	  // This platform is big-endian - No swap to network byte order
	  log_info("TEST(Bug10327_Test, Heterogeneous) - Part 3 - Big-endian");
	  ret = ((ref->topic_uid == 0x01234567u) && (ref->topic_version == 0x89abcdefu));
	}
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10327_Test, Heterogeneous_publisher)
{

  Bug10327_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      // Simulacre of big-endian publisher
      uint_t size = 1024 + sizeof(sdn::Header_t);
      uint8_t payload [size]; memset(payload, 0, size);
      sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(payload);

      if (sdn::HelperTools::IsLittleEndian())
	{
	  // This platform is little-endian - Send out swapped values
	  sstrncpy(ref->header_uid, "SDNv2.x", 8);
	  ref->header_size = htobe16(sizeof(sdn::Header_t));
	  ref->topic_uid = 0x67452301u;
	  ref->topic_version = 0xefcdab89u;
	  ref->topic_size = 0x0004u; // 0x00.04
	  ref->topic_counter = 0ul;
	  ref->send_time = htobe64(get_time());
	  ref->recv_time = 0ul;
	}
      else
	{
	  // This platform is big-endian - Send out actual values
	  sstrncpy(ref->header_uid, "SDNv2.x", 8);
	  ref->header_size = sizeof(sdn::Header_t);
	  ref->topic_uid = 0x01234567u;
	  ref->topic_version = 0x89abcdefu;
	  ref->topic_size = 1024u; // 0x04.00
	  ref->topic_counter = 0ul;
	  ref->send_time = get_time();
	  ref->recv_time = 0ul;
	}

      log_info("TEST(Bug10327_Test, Heterogeneous_publisher) - Part 1");
      ret = ((test.mcast_pub)->Publish(payload, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      log_info("TEST(Bug10327_Test, Heterogeneous_publisher) - Part 2");
      ret = ((test.sdn_sub)->Receive(10000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      sdn::Header* header = (test.sdn_sub)->m_header;

      if (sdn::HelperTools::IsLittleEndian())
	{
	  // This platform is little-endian - It should detect out-of-order payload as the publisher emulates big-endian
	  log_info("TEST(Bug10327_Test, Heterogeneous_publisher) - Part 3 - Out-of-order payload");
	  ret = ((header->IsPayloadOrdered() == false) && ((test.sdn_sub)->IsPayloadOrdered() == false));
	}
      else
	{
	  // This platform is big-endian - It should detect ordered payload
	  log_info("TEST(Bug10327_Test, Heterogeneous_publisher) - Part 3 - Ordered payload");
	  ret = ((header->IsPayloadOrdered() == true) && ((test.sdn_sub)->IsPayloadOrdered() == true));
	}
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10327_Test, Heterogeneous_subscriber)
{

  Bug10327_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      log_info("TEST(Bug10327_Test, Heterogeneous_subscriber) - Part 1 - sdn::Publisher::Publish");
      ret = ((test.sdn_pub)->Publish() == STATUS_SUCCESS);
    }

  uint_t size = 1024 + sizeof(sdn::Header_t);
  uint8_t payload [size];

  if (ret)
    {
      memset(payload, 0, size);

      log_info("TEST(Bug10327_Test, Heterogeneous_subscriber) - Part 2 - mcast::Subscriber::Receive");
      ret = ((test.mcast_sub)->Receive(payload, size, 10000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(payload);
#if 0 /* Revert on Bug 10327 */
      if (sdn::HelperTools::IsLittleEndian())
	{
	  // This platform is little-endian - Should have swapped 
	  log_info("TEST(Bug10327_Test, Heterogeneous_subscriber) - Part 3 - Little-endian");
	  ret = ((ref->topic_uid == 0x67452301u) && (ref->topic_version == 0xefcdab89u));
	}
      else
	{
	  // This platform is big-endian - No swap to network byte order
	  log_info("TEST(Bug10327_Test, Heterogeneous_subscriber) - Part 3 - Big-endian");
	  ret = ((ref->topic_uid == 0x01234567u) && (ref->topic_version == 0x89abcdefu));
	}
#else
	  // No swap foreseen
	  log_info("TEST(Bug10327_Test, Heterogeneous_subscriber) - Part 3");
	  ret = ((ref->topic_uid == 0x01234567u) && (ref->topic_version == 0x89abcdefu));
#endif
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10327_Test, SetPayloadOrder)
{

  Bug10327_Test test;

  bool ret = test.Initialise();

  Bug10327_Topic_t p_topic; memset(&p_topic, 0, sizeof(Bug10327_Topic_t));

  if (ret)
    {
      p_topic.counter = 1ul;
      p_topic.timestamp = get_time();
      sstrncpy(p_topic.buffer, "Hello world!", STRING_MAX_LENGTH);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->Publish(&p_topic) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->IsPayloadOrdered() == true);
    }

  Bug10327_Topic_t s_topic; memset(&s_topic, 0, sizeof(Bug10327_Topic_t));

  if (ret)
    {
      ret = ((test.sdn_sub)->Receive(&s_topic, 10000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_sub)->IsPayloadOrdered() == true);
    }

  if (ret)
    {
      ret = (s_topic.counter == 1u);
    }

  if (ret)
    {
      p_topic.counter += 1ul;
      p_topic.timestamp = get_time();

      if (sdn::HelperTools::IsLittleEndian())
	{
	  // This platform is little-endian - Select network byte order
	  (test.sdn_pub)->SetPayloadOrder(sdn::types::BigEndian);
	  p_topic.counter = sdn::HelperTools::ToNetworkByteOrder(p_topic.counter);
	  p_topic.timestamp = sdn::HelperTools::ToNetworkByteOrder(p_topic.timestamp);
	}
      else
	{
	  // This platform is big-endian - Should have no effect
	  (test.sdn_pub)->SetPayloadOrder(sdn::types::BigEndian);
	}
     }

  if (ret)
    {
      ret = ((test.sdn_pub)->Publish(&p_topic) == STATUS_SUCCESS);
    }

  if (ret)
    {
      if (sdn::HelperTools::IsLittleEndian())
	{
	  ret = ((test.sdn_pub)->IsPayloadOrdered() == false);
	}
      else
	{
	  ret = ((test.sdn_pub)->IsPayloadOrdered() == true);
	}
    }

  if (ret)
    {
      ret = ((test.sdn_sub)->Receive(&s_topic, 10000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      if (sdn::HelperTools::IsLittleEndian())
	{
	  // This platform is little-endian - Disordered payload and header
	  ret = ((test.sdn_sub)->IsPayloadOrdered() == false);
	}
      else
	{
	  // This platform is big-endian - Should have no effect
	  ret = ((test.sdn_sub)->IsPayloadOrdered() == true);
	}
    }

  if (ret)
    {
      if (sdn::HelperTools::IsLittleEndian())
	{
	  // Convert payload to native byte ordering
	  s_topic.counter = sdn::HelperTools::FromNetworkByteOrder(s_topic.counter);
	  s_topic.timestamp = sdn::HelperTools::FromNetworkByteOrder(s_topic.timestamp);
	  ret = (s_topic.counter == 2u);
	}
    }

  if (ret)
    {
      if (sdn::HelperTools::IsLittleEndian())
	{
	  // Force header to be now in host byte ordering
	  (((test.sdn_sub)->m_packet)->GetHeader())->FromNetworkByteOrder();
	  ret = ((test.sdn_sub)->IsPayloadOrdered() == true);
	}
    }

  ASSERT_EQ(ret, true);
}


/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10687-test.cpp $
* $Id: Bug10687-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include <functional> /* std::function, etc. */
//#include <boost/bind.hpp> /* boost::bind, etc. */
//#include <boost/function.hpp> /* boost::function, etc. */

#include <gtest/gtest.h> /* Google test framework */

/* Local header files */

#include "sdn-types.h" /* Misc. type definition */
#include "sdn-tools.h" /* Misc. helper functions */

#include "sdn-base.h" /* Misc. class definition */

#include "sdn-header.h"
#include "sdn-api.h"

/* Constants */

/* Type definition */

struct MyPayload {
  sdn::types::uint64 timestamp;
  sdn::types::float64 signal; 
};

sdn::Metadata_t __mdata = { "Bug10687", "Regression test", 0u, sizeof(MyPayload), 0u, "239.0.106.87", 10687 };   

class AsynchronousSubscriber_Bug10687 : public sdn::AsynchronousSubscriber {
  private:
    uint32_t counter;
    MyPayload payload;

  public:
    AsynchronousSubscriber_Bug10687() : sdn::AsynchronousSubscriber(__mdata) { 
      // Initialise attributes
      this->counter = 0u;
      this->payload.timestamp = 0ul;
      this->payload.signal = 0.0;

      this->SetInterface("lo"); 
      this->Configure(); 
    };
    virtual void HandleReceive() {
      log_info("AsynchronousSubscriber_Bug10687::HandleReceive called"); 
      counter += 1u;
      // Take a local copy of the payload
      this->sdn::AsynchronousSubscriber::CopyTopicInstance(static_cast<void*>(&payload));
    };
    uint32_t GetCounter (void) { return counter; };
    uint64_t GetTimestamp (void) { return payload.timestamp; };
};

class Bug10687_Test
{

  private:

  //

  public:

    sdn::Topic* topic;
    sdn::AsynchronousSubscriber* sdn_sub;
    ccs::mcast::Publisher* mcast_pub;

    bool Initialise (void);

    virtual ~Bug10687_Test (void);

};

/* Global variables */

uint32_t __counter = 0u;

/* Internal function declaration */

/* Internal function definition */

void __callback (sdn::AsynchronousSubscriber* sub) { log_info("Static callback called"); __counter += 1u; };

bool Bug10687_Test::Initialise (void)
{
  // Initialise attributes
  topic = static_cast<sdn::Topic*>(NULL);
  sdn_sub = static_cast<sdn::AsynchronousSubscriber*>(NULL);
  mcast_pub = static_cast<ccs::mcast::Publisher*>(NULL);

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "Bug10687", 1024, "239.0.106.87", 10687);

  if (ret)
    {
      ret = (static_cast<sdn::Topic*>(NULL) != (topic = new (std::nothrow) sdn::Topic (mdata)));
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
      ret = (static_cast<sdn::AsynchronousSubscriber*>(NULL) != (sdn_sub = new (std::nothrow) sdn::AsynchronousSubscriber (*topic)));
    }

  if (ret)
    {
      ret = (sdn_sub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sdn_sub->SetCallback((void(*)(void*)) &__callback, static_cast<void*>(sdn_sub)) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sdn_sub->Configure() == STATUS_SUCCESS);
    }

  // Instantiate MCAST participants
  if (ret)
    {
      ret = ((mcast_pub = new (std::nothrow) ccs::mcast::Publisher ("lo", "239.0.106.87", 10687)) != static_cast<ccs::mcast::Publisher*>(NULL));
    }

  // Reset globally scoped counter
  __counter = 0u;

  return ret;

};

Bug10687_Test::~Bug10687_Test (void)
{

  // Release resources
  if (mcast_pub != static_cast<ccs::mcast::Publisher*>(NULL))
    {
      delete mcast_pub;
      mcast_pub = static_cast<ccs::mcast::Publisher*>(NULL);
    }

  if (sdn_sub != static_cast<sdn::AsynchronousSubscriber*>(NULL))
    {
      delete sdn_sub;
      sdn_sub = static_cast<sdn::AsynchronousSubscriber*>(NULL);
    }

  if (topic != static_cast<sdn::Topic*>(NULL))
    {
      delete topic;
      topic = static_cast<sdn::Topic*>(NULL);
    }

  return;

};

TEST(Bug10687_Test, Constructor)
{

  log_info("TEST(Bug10687_Test, Constructor) - Starting test");

  Bug10687_Test test;

  bool ret = test.Initialise();

  log_info("TEST(Bug10687_Test, Constructor) - Stopping test");

  ASSERT_EQ(ret, true);
}

TEST(Bug10687_Test, Receive)
{

  log_info("TEST(Bug10687_Test, Receive) - Starting test");

  Bug10687_Test test;

  bool ret = test.Initialise();

  uint64_t send_time = get_time();

  if (ret)
    {
      uint_t size = 1024 + sizeof(sdn::Header_t);
      uint8_t payload [size]; memset(payload, 0, size);
      sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(payload);

      sstrncpy(ref->header_uid, "SDNv2.x", 8);
      ref->header_size = sizeof(sdn::Header_t);
      ref->topic_uid = 0x01234567u;
      ref->topic_version = 0x89abcdefu;
      ref->topic_size = 1024u;
      ref->topic_counter = 0ul;
      ref->send_time = send_time;
      ref->recv_time = 0ul;

      char* buffer = reinterpret_cast<char*>(payload + sizeof(sdn::Header_t));
      sstrncpy(buffer, "Hello world!", 1024);

      ret = ((test.mcast_pub)->Publish(payload, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      wait_for(10000000ul);
    }

  if (ret)
    {
      const sdn::Header_t* ref = static_cast<const sdn::Header_t*>((test.sdn_sub)->GetTopicHeader());
      ret = ((ref->send_time == send_time) && (ref->recv_time > send_time));
    }

  if (ret)
    {
      const char* buffer = static_cast<const char*>((test.sdn_sub)->GetTopicInstance());
      ret = (strcmp(buffer, "Hello world!") == 0);
    }

  log_info("TEST(Bug10687_Test, Receive) - Stopping test");

  ASSERT_EQ(ret, true);
}

TEST(Bug10687_Test, Callback)
{

  log_info("TEST(Bug10687_Test, Callback) - Starting test");

  Bug10687_Test test;

  bool ret = test.Initialise();

  uint_t size = 1024 + sizeof(sdn::Header_t);
  uint8_t payload [size]; memset(payload, 0, size);
  sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(payload);

  if (ret)
    {
      sstrncpy(ref->header_uid, "SDNv2.x", 8);
      ref->header_size = sizeof(sdn::Header_t);
      ref->topic_uid = 0x01234567u;
      ref->topic_version = 0x89abcdefu;
      ref->topic_size = 1024u;
      ref->topic_counter = 0ul;
      ref->send_time = get_time();
      ref->recv_time = 0ul;

      ret = ((test.mcast_pub)->Publish(payload, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      wait_for(10000000ul);
    }

  if (ret)
    {
      ref->topic_counter = 1ul;
      ref->send_time = get_time();
      ref->recv_time = 0ul;

      ret = ((test.mcast_pub)->Publish(payload, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      wait_for(10000000ul);
    }

  if (ret)
    {
      ret = (__counter == 2u);
    }

  log_info("TEST(Bug10687_Test, Callback) - Stopping test");

  ASSERT_EQ(ret, true);
}

TEST(Bug10687_Test, Specialisation)
{

  log_info("TEST(Bug10687_Test, Specialisation) - Starting test");

  Bug10687_Test test;

  bool ret = test.Initialise();

  AsynchronousSubscriber_Bug10687 my_sub;

  uint_t size = sizeof(MyPayload) + sizeof(sdn::Header_t);
  uint8_t payload [size]; memset(payload, 0, size);
  sdn::Header_t* ref = reinterpret_cast<sdn::Header_t*>(payload);

  if (ret)
    {
      sstrncpy(ref->header_uid, "SDNv2.x", 8);
      ref->header_size = sizeof(sdn::Header_t);
      ref->topic_uid = 0u;
      ref->topic_version = 0u;
      ref->topic_size = sizeof(MyPayload);
      ref->topic_counter = 0ul;
      ref->send_time = get_time();
      ref->recv_time = 0ul;

      ret = ((test.mcast_pub)->Publish(payload, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      wait_for(10000000ul);
    }

  if (ret)
    {
      ref->topic_counter = 1ul;
      ref->send_time = get_time();
      ref->recv_time = 0ul;

      ret = ((test.mcast_pub)->Publish(payload, size) == STATUS_SUCCESS);
    }

  if (ret)
    {
      wait_for(10000000ul);
    }

  if (ret)
    {
      ret = (my_sub.GetCounter() == 2u);
    }

  log_info("TEST(Bug10687_Test, Specialisation) - Stopping test");

  ASSERT_EQ(ret, true);
}

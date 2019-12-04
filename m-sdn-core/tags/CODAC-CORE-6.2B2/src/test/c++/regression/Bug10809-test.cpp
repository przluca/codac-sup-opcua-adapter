/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10809-test.cpp $
* $Id: Bug10809-test.cpp 99202 2019-04-19 11:16:16Z bauvirb $
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

#include <gtest/gtest.h> /* Google test framework */

/* Local header files */

#include "sdn-types.h" /* Misc. type definition */
#include "sdn-tools.h" /* Misc. helper functions */

#include "sdn-base.h" /* Misc. class definition */
#include "sdn-header.h"
#include "sdn-packet.h"

#include "mcast-api.h"
#include "ucast-api.h"

#include "sdn-api.h" /* SDN core library - API definition */

/* Constants */

/* Type definition */

class Bug10809_Test
{

  public:

    ccs::mcast::Publisher* mcast_pub;
    ccs::ucast::Publisher* ucast_pub;

    sdn::Topic* topic;
    sdn::Publisher* sdn_pub;

    int mcast_socket;

    bool Initialise (void);

    virtual ~Bug10809_Test (void);

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

bool Bug10809_Test::Initialise (void)
{
  // Initialise attributes
  mcast_pub = static_cast<ccs::mcast::Publisher*>(NULL);
  ucast_pub = static_cast<ccs::ucast::Publisher*>(NULL);

  topic = static_cast<sdn::Topic*>(NULL);
  sdn_pub = static_cast<sdn::Publisher*>(NULL);

  mcast_socket = 0;

  bool ret = true;

  // Instantiate MCAST participants
  if (ret)
    {
      ret = ((mcast_pub = new (std::nothrow) ccs::mcast::Publisher ()) != static_cast<ccs::mcast::Publisher*>(NULL));
    }

  if (ret)
    {
      mcast_pub->SetInterface("lo");
      mcast_pub->SetMCastAddr("239.0.108.9");
      mcast_pub->SetMCastPort(10809);
    }

  // Instantiate UCAST participants
  if (ret)
    {
      ret = ((ucast_pub = new (std::nothrow) ccs::ucast::Publisher ()) != static_cast<ccs::ucast::Publisher*>(NULL));
    }

  if (ret)
    {
      ucast_pub->SetInterface("lo");
      ucast_pub->SetUCastAddr("127.0.0.1");
      ucast_pub->SetUCastPort(10809);
    }

  // Instantiate topic
  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "Bug10809", 1024, "239.0.108.9", 10809);

  if (ret)
    {
      ret = ((topic = new (std::nothrow) sdn::Topic (mdata)) != static_cast<sdn::Topic*>(NULL));
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

  // Create test sockets
  if (ret)
    {
      ret = ((mcast_socket = socket(AF_INET, SOCK_DGRAM, 0)) > 0);
    }

  if (ret)
    {
      struct in_addr if_addr; if_addr.s_addr = inet_addr("127.0.0.1");

      ret = (setsockopt(mcast_socket, IPPROTO_IP, IP_MULTICAST_IF, (char*) &if_addr, sizeof(if_addr)) == 0);
    }

  if (ret)
    {
      int reuse = 1;
    
      ret = (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse)) == 0);
    }

  if (ret)
    {
      struct sockaddr_in mcast_addr;

      memset(&mcast_addr, 0, sizeof(mcast_addr));
      mcast_addr.sin_family = AF_INET;
      mcast_addr.sin_addr.s_addr = inet_addr("239.0.108.9");
      mcast_addr.sin_port = htons(10809);
    
      ret = (bind(mcast_socket, (struct sockaddr*) &mcast_addr, sizeof(mcast_addr)) == 0);
    }

  if (ret)
    {
      struct ip_mreq ip_req;

      ip_req.imr_multiaddr.s_addr = inet_addr("239.0.108.9");
      ip_req.imr_interface.s_addr = inet_addr("127.0.0.1");

      ret = (setsockopt(mcast_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &ip_req, sizeof(ip_req)) == 0);
    }

  return ret;

};

Bug10809_Test::~Bug10809_Test (void)
{

  // Release resources
  if (mcast_pub != static_cast<ccs::mcast::Publisher*>(NULL))
    {
      delete mcast_pub;
      mcast_pub = static_cast<ccs::mcast::Publisher*>(NULL);
    }

  if (ucast_pub != static_cast<ccs::ucast::Publisher*>(NULL))
    {
      delete ucast_pub;
      ucast_pub = static_cast<ccs::ucast::Publisher*>(NULL);
    }

  if (sdn_pub != static_cast<sdn::Publisher*>(NULL))
    {
      delete sdn_pub;
      sdn_pub = static_cast<sdn::Publisher*>(NULL);
    }

  if (topic != static_cast<sdn::Topic*>(NULL))
    {
      delete topic;
      topic = static_cast<sdn::Topic*>(NULL);
    }

  close(mcast_socket);

  return;

};

TEST(Bug10809_Test, SetSourcePort_default)
{

  Bug10809_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = ((test.mcast_pub)->Open() == STATUS_SUCCESS);
    }

  if (ret)
    {
      char send_buffer [STRING_MAX_LENGTH] = "Hello world!";
      ret = ((test.mcast_pub)->Publish(send_buffer, STRING_MAX_LENGTH) == STATUS_SUCCESS);
    }

  struct sockaddr_in ucast_addr; /* The source identifier */
  int addr_size = sizeof(ucast_addr);
  memset(&ucast_addr, 0, addr_size);

  int recv_size = 1024;
  uint8_t recv_buffer [recv_size]; memset(recv_buffer, 0, recv_size);

  if (ret)
    {
      ret = ((recv_size = recvfrom(test.mcast_socket, recv_buffer, recv_size, 0, (struct sockaddr*) &ucast_addr, (socklen_t*) &addr_size)) >= 0);
    }

  if (ret)
    {
      int recv_port = ntohs(ucast_addr.sin_port);
      log_info("TEST(Bug10809_Test, SetSourcePort_default) - Receive '%u' size message from '%d' port", recv_size, recv_port);
      //ret = (recv_port == 10809); 
      ret = (recv_size == STRING_MAX_LENGTH); 
    }

  if (ret)
    {
      log_notice("TEST(Bug10809_Test, SetSourcePort_default) - Success");
    }
  else
    {
      log_error("TEST(Bug10809_Test, SetSourcePort_default) - Failure");
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10809_Test, SetSourcePort_success)
{

  Bug10809_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      (test.mcast_pub)->SetSourcePort(1024);
      ret = ((test.mcast_pub)->Open() == STATUS_SUCCESS);
    }

  if (ret)
    {
      char send_buffer [STRING_MAX_LENGTH] = "Hello world!";
      ret = ((test.mcast_pub)->Publish(send_buffer, STRING_MAX_LENGTH) == STATUS_SUCCESS);
    }

  struct sockaddr_in ucast_addr; /* The source identifier */
  int addr_size = sizeof(ucast_addr);
  memset(&ucast_addr, 0, addr_size);

  int recv_size = 1024;
  uint8_t recv_buffer [recv_size]; memset(recv_buffer, 0, recv_size);

  if (ret)
    {
      ret = ((recv_size = recvfrom(test.mcast_socket, recv_buffer, recv_size, 0, (struct sockaddr*) &ucast_addr, (socklen_t*) &addr_size)) >= 0);
    }

  if (ret)
    {
      int recv_port = ntohs(ucast_addr.sin_port);
      log_info("TEST(Bug10809_Test, SetSourcePort_default) - Receive '%u' size message from '%d' port", recv_size, recv_port);
      ret = (recv_port == 1024);
    }

  if (ret)
    {
      log_notice("TEST(Bug10809_Test, SetSourcePort_success) - Success");
    }
  else
    {
      log_error("TEST(Bug10809_Test, SetSourcePort_success) - Failure");
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10809_Test, SetSourcePort_error)
{

  Bug10809_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      (test.mcast_pub)->SetSourcePort(10);
      ret = ((test.mcast_pub)->Open() == STATUS_ERROR);
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10809_Test, SetInterface_default)
{

  Bug10809_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = ((test.sdn_pub)->Configure() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->Publish() == STATUS_SUCCESS);
    }

  struct sockaddr_in ucast_addr; /* The source identifier */
  int addr_size = sizeof(ucast_addr);
  memset(&ucast_addr, 0, addr_size);

  int recv_size = 1024 + sizeof(sdn::Header_t);
  uint8_t recv_buffer [recv_size]; memset(recv_buffer, 0, recv_size);

  if (ret)
    {
      ret = ((recv_size = recvfrom(test.mcast_socket, recv_buffer, recv_size, 0, (struct sockaddr*) &ucast_addr, (socklen_t*) &addr_size)) >= 0);
    }

  if (ret)
    {
      int recv_port = ntohs(ucast_addr.sin_port);
      log_info("TEST(Bug10809_Test, SetInterface_default) - Receive '%u' size message from '%d' port", recv_size, recv_port);
      //ret = (recv_port == 10809); 
      ret = (recv_size == 1024 + sizeof(sdn::Header_t)); 
    }

  if (ret)
    {
      log_notice("TEST(Bug10809_Test, SetInterface_default) - Success");
    }
  else
    {
      log_error("TEST(Bug10809_Test, SetInterface_default) - Failure");
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10809_Test, SetInterface_success)
{

  Bug10809_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = ((test.sdn_pub)->SetInterface("lo", 1024) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->Configure() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->Publish() == STATUS_SUCCESS);
    }

  struct sockaddr_in ucast_addr; /* The source identifier */
  int addr_size = sizeof(ucast_addr);
  memset(&ucast_addr, 0, addr_size);

  int recv_size = 1024 + sizeof(sdn::Header_t);
  uint8_t recv_buffer [recv_size]; memset(recv_buffer, 0, recv_size);

  if (ret)
    {
      ret = ((recv_size = recvfrom(test.mcast_socket, recv_buffer, recv_size, 0, (struct sockaddr*) &ucast_addr, (socklen_t*) &addr_size)) >= 0);
    }

  if (ret)
    {
      int recv_port = ntohs(ucast_addr.sin_port);
      log_info("TEST(Bug10809_Test, SetInterface_success) - Receive '%u' size message from '%d' port", recv_size, recv_port);
      ret = (recv_port == 1024); 
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10809_Test, SetInterface_error)
{

  Bug10809_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = ((test.sdn_pub)->SetInterface("lo", 10) == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_pub)->Configure() == STATUS_ERROR);
    }

  ASSERT_EQ(ret, true);
}


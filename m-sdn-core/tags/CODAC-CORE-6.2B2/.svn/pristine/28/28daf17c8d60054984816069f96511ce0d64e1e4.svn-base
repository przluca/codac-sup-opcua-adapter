/******************************************************************************
* $HeadURL$
* $Id$
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

#include "mcast-api.h"
#include "mcast-constants.h"

/* Constants */

/* Type definition */

class MCast_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

void MCast_CB (void* dummy)
{
  log_info("MCast_CB - Function called at '%lu'", ccs::HelperTools::GetCurrentTime());
}


TEST(MCast_Test, Constructor_default)
{
  bool ret = true;

  log_info("TEST(MCastPublisher_Test, Constructor_default) - Starting test");

  ccs::mcast::Publisher* publisher = static_cast<ccs::mcast::Publisher*>(NULL);
  ccs::mcast::Subscriber* subscriber = static_cast<ccs::mcast::Subscriber*>(NULL);

  if (ret)
    {
      publisher = new (std::nothrow) ccs::mcast::Publisher;
      ret = (publisher != static_cast<ccs::mcast::Publisher*>(NULL));
    }

  if (ret)
    {
      subscriber = new (std::nothrow) ccs::mcast::Subscriber;
      ret = (subscriber != static_cast<ccs::mcast::Subscriber*>(NULL));
    }

  if (ret)
    {
      ret = publisher->SetCallback(&MCast_CB);
    }

  if (ret)
    {
      ret = subscriber->SetCallback(&MCast_CB);
    }

  if (ret)
    {
      const char* interface = publisher->GetInterface();
      log_info("TEST(MCast_Test, Constructor_default) - Interface '%s'", interface);
      ret = (IsUndefined(interface) == true);
    }

  if (ret)
    {
      const char* interface = subscriber->GetInterface();
      log_info("TEST(MCast_Test, Constructor_default) - Interface '%s'", interface);
      ret = (IsUndefined(interface) == true);
    }

  if (ret)
    {
      const char* address = publisher->GetInterfaceAddress();
      log_info("TEST(MCast_Test, Constructor_default) - Address '%s'", address);
      ret = (IsUndefined(address) == true);
    }

  if (ret)
    {
      const char* address = subscriber->GetInterfaceAddress();
      log_info("TEST(MCast_Test, Constructor_default) - Address '%s'", address);
      ret = (IsUndefined(address) == true);
    }

  if (ret)
    {
      const char* address = publisher->GetMCastAddr();
      log_info("TEST(MCast_Test, Constructor_default) - Address '%s'", address);
      ret = (IsUndefined(address) == true);
    }

  if (ret)
    {
      const char* address = subscriber->GetMCastAddr();
      log_info("TEST(MCast_Test, Constructor_default) - Address '%s'", address);
      ret = (IsUndefined(address) == true);
    }

  if (ret)
    {
      uint_t port = publisher->GetMCastPort();
      ret = (port == 0u);
    }

  if (ret)
    {
      uint_t port = subscriber->GetMCastPort();
      ret = (port == 0u);
    }

  if (ret)
    {
      char* p_buf = (char*) publisher->GetBuffer();
	  uint_t max_size = publisher->GetSize();
    }

  if (ret)
    {
      char* p_buf = (char*) subscriber->GetBuffer();
	  uint_t max_size = subscriber->GetSize();
    }

  if (ret)
    {
      delete publisher;
      delete subscriber;
    }

  log_info("TEST(MCast_Test, Constructor_default) - Finished test");

  ASSERT_EQ(ret, true);
}

TEST(MCast_Test, Constructor_param)
{
  bool ret = true;

  log_info("TEST(MCast_Test, Constructor_param) - Starting test");

  ccs::mcast::Publisher* publisher = static_cast<ccs::mcast::Publisher*>(NULL);
  ccs::mcast::Subscriber* subscriber = static_cast<ccs::mcast::Subscriber*>(NULL);

  if (ret)
    {
      publisher = new (std::nothrow) ccs::mcast::Publisher (ccs::mcast::DefaultIfaceName, ccs::mcast::DefaultMCastAddr, ccs::mcast::DefaultMCastPort);
      ret = (publisher != static_cast<ccs::mcast::Publisher*>(NULL));
    }

  if (ret)
    {
      subscriber = new (std::nothrow) ccs::mcast::Subscriber (ccs::mcast::DefaultIfaceName, ccs::mcast::DefaultMCastAddr, ccs::mcast::DefaultMCastPort);
      ret = (subscriber != static_cast<ccs::mcast::Subscriber*>(NULL));
    }

  if (ret)
    {
      ret = publisher->SetCallback(&MCast_CB, static_cast<void*>(&publisher));
    }

  if (ret)
    {
      ret = subscriber->SetCallback(&MCast_CB, static_cast<void*>(&subscriber));
    }

  if (ret)
    {
      const char* interface = publisher->GetInterface();
      ret = (strcmp(interface, ccs::mcast::DefaultIfaceName) == 0);
    }

  if (ret)
    {
      const char* address = publisher->GetInterfaceAddress();
      char if_addr [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::GetIPAddress(publisher->GetInterface(), if_addr);
      ret = (strcmp(address, if_addr) == 0);
    }

  if (ret)
    {
      const char* address = publisher->GetMCastAddr();
      ret = (strcmp(address, ccs::mcast::DefaultMCastAddr) == 0);
    }

  if (ret)
    {
      uint_t port = publisher->GetMCastPort();
      ret = (port == ccs::mcast::DefaultMCastPort);
    }

  if (ret)
    {
      char* p_buf = (char*) publisher->GetBuffer();
	  uint_t max_size = publisher->GetSize();
    }

  if (ret)
    {
      subscriber->SetInterface(ccs::mcast::DefaultIfaceName);
      subscriber->SetMCastAddr(ccs::mcast::DefaultMCastAddr);
      subscriber->SetMCastPort(ccs::mcast::DefaultMCastPort);
      ret = (subscriber->Open() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (subscriber->Close() == STATUS_SUCCESS);
    }

  if (ret)
    {
      delete publisher;
      delete subscriber;
    }

  log_info("TEST(MCast_Test, Constructor_param) - Finished test");

  ASSERT_EQ(ret, true);
}

TEST(MCast_Test, Multiple)
{
  bool ret = true;

  log_info("TEST(MCast_Test, Multiple) - Starting test");

  ccs::mcast::Publisher* publisher_1 = static_cast<ccs::mcast::Publisher*>(NULL);
  ccs::mcast::Publisher* publisher_2 = static_cast<ccs::mcast::Publisher*>(NULL);

  if (ret)
    {
      publisher_1 = new (std::nothrow) ccs::mcast::Publisher;
      publisher_2 = new (std::nothrow) ccs::mcast::Publisher;
      ret = ((publisher_1 != static_cast<ccs::mcast::Publisher*>(NULL)) && 
	     (publisher_2 != static_cast<ccs::mcast::Publisher*>(NULL)));
    }

  if (ret)
    {
      publisher_1->SetInterface(ccs::mcast::DefaultIfaceName);
      publisher_2->SetInterface(ccs::mcast::DefaultIfaceName);
    }

  if (ret)
    {
      publisher_1->SetMCastAddr(ccs::mcast::DefaultMCastAddr);
      publisher_2->SetMCastAddr(ccs::mcast::DefaultMCastAddr);
    }

  if (ret)
    {
      publisher_1->SetMCastPort(ccs::mcast::DefaultMCastPort);
      publisher_2->SetMCastPort(ccs::mcast::DefaultMCastPort);
    }

  if (ret)
    {
      ret = ((publisher_1->Open() == STATUS_SUCCESS) &&
	     (publisher_2->Open() == STATUS_SUCCESS));
    }

  uint8_t buffer [1024]; memset(buffer, 0, 1024);

  if (ret)
    {
      ret = ((publisher_1->Publish(static_cast<void*>(buffer), 1024) == STATUS_SUCCESS) &&
	     (publisher_2->Publish(static_cast<void*>(buffer), 1024) == STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = ((publisher_1->Close() == STATUS_SUCCESS) &&
	     (publisher_2->Close() == STATUS_SUCCESS));
    }

  if (ret)
    {
      delete publisher_1; delete publisher_2;
    }

  log_info("TEST(MCast_Test, Multiple) - Finished test");

  ASSERT_EQ(ret, true);
}

TEST(MCast_Test, Messenger_pub)
{
  bool ret = true;

  log_info("TEST(MCast_Test, Messenger_pub) - Starting test");

  ccs::mcast::Messenger* messenger = static_cast<ccs::mcast::Messenger*>(NULL);

  if (ret)
    {
      messenger = new (std::nothrow) ccs::mcast::Messenger;
      ret = (messenger != static_cast<ccs::mcast::Messenger*>(NULL));
    }

  if (ret)
    {
      messenger->SetBufferDepth(2u);
    }

  if (ret)
    {
      ret = messenger->SetCallback(&MCast_CB);
    }

  if (ret)
    {
      ret = messenger->SetCallback(&MCast_CB, static_cast<void*>(&messenger));
    }

  if (ret)
    {
      char* p_buf = (char*) messenger->GetBuffer();
	  uint_t max_size = messenger->GetSize();
//	  int sock = messenger->GetSocket();
    }

  if (ret)
    {
      messenger->SetInterface(ccs::mcast::DefaultIfaceName);
    }

  if (ret)
    {
      messenger->SetMCastAddr(ccs::mcast::DefaultMCastAddr);
    }

  if (ret)
    {
      messenger->SetMCastPort(ccs::mcast::DefaultMCastPort);
    }

  if (ret)
    {
      ret = (messenger->Open() == STATUS_SUCCESS);
    }

  uint8_t pub_buffer [1024]; memset(pub_buffer, 0, 1024);

  if (ret)
    {
      ret = (messenger->Publish(static_cast<void*>(pub_buffer), 1024u) == STATUS_SUCCESS);
    }
/*
  uint8_t rev_buffer [1024]; memset(rev_buffer, 0, 1024);
  if (ret)
    {
		uint_t recv_size = 1024;
		uint64_t timeout = 1;
    	ret = (messenger->Receive(static_cast<void*>(rev_buffer), recv_size, timeout) == STATUS_SUCCESS);
    }

  uint8_t reply_buffer [1024]; memset(reply_buffer, 0, 1024);
  if (ret)
    {
      ret = (messenger->Reply(static_cast<void*>(reply_buffer), 1024u) == STATUS_SUCCESS);
    }
*/


  if (ret)
    {
      ret = (messenger->Close() == STATUS_SUCCESS);
    }

  if (ret)
    {
      delete messenger;
    }


  log_info("TEST(MCast_Test, Messenger_pub) - Finished test");

  ASSERT_EQ(ret, true);
}
/*
TEST(MCast_Test, Messenger_rcv)
{
  bool ret = true;

  log_info("TEST(MCast_Test, Messenger_rcv) - Starting test");

  ccs::mcast::Messenger* messenger = static_cast<ccs::mcast::Messenger*>(NULL);

  if (ret)
    {
      messenger = new (std::nothrow) ccs::mcast::Messenger;
      ret = (messenger != static_cast<ccs::mcast::Messenger*>(NULL));
    }

  if (ret)
    {
      messenger->SetBufferDepth(2u);
    }

  if (ret)
    {
      ret = messenger->SetCallback(&MCast_CB);
    }

  if (ret)
    {
      ret = messenger->SetCallback(&MCast_CB, static_cast<void*>(&messenger));
    }

  if (ret)
    {
      char* p_buf = (char*) messenger->GetBuffer();
	  uint_t max_size = messenger->GetSize();
//	  int sock = messenger->GetSocket();
    }

  if (ret)
    {
      messenger->SetInterface(ccs::mcast::DefaultIfaceName);
    }

  if (ret)
    {
      messenger->SetMCastAddr(ccs::mcast::DefaultMCastAddr);
    }

  if (ret)
    {
      messenger->SetMCastPort(ccs::mcast::DefaultMCastPort);
    }

  if (ret)
    {
      ret = (messenger->Open() == STATUS_SUCCESS);
    }


  uint8_t rev_buffer [1024]; memset(rev_buffer, 0, 1024);
  if (ret)
    {
		uint_t recv_size = 1024;
		uint64_t timeout = 10;
    	ASSERT_EQ(messenger->Receive(static_cast<void*>(rev_buffer), recv_size, timeout), STATUS_SUCCESS);
    }

  uint8_t reply_buffer [1024]; memset(reply_buffer, 0, 1024);
  if (ret)
    {
	   	ASSERT_EQ(messenger->Reply(static_cast<void*>(reply_buffer), 1024u), STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (messenger->Close() == STATUS_SUCCESS);
    }

  if (ret)
    {
      delete messenger;
    }


  log_info("TEST(MCast_Test, Messenger_rcv) - Finished test");

  ASSERT_EQ(ret, true);
}
*/


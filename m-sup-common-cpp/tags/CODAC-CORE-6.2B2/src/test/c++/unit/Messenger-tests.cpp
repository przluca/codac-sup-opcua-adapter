/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Messenger-tests.cpp $
* $Id: Messenger-tests.cpp 100236 2019-06-21 11:00:02Z bauvirb $
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

#include <gtest/gtest.h> // Google test framework

#include <common/BasicTypes.h> // Misc. type definition
#include <common/SysTools.h> // Misc. helper functions

#include <common/log-api.h> // Syslog wrapper routines

// Local header files

#include "Messenger.h"

// Constants

// Type definition

class MyMessenger : public sup::core::Messenger {
  
  private:

    ccs::types::uint32 __count = 0u;
    ccs::types::uint32 __value = 0u;

  public:
  
    MyMessenger (sup::core::Channel_t channel = sup::core::Channel::TCN) : sup::core::Messenger(channel, sup::core::Role::MONITOR) {};
    virtual ~MyMessenger (void) {};
  
    // Implementation of sup::core::Messenger interface
    virtual bool HandleReceive (const std::string& msg) {

      bool status = true;

      __count += 1u;

      if (msg == "echo") status = Send(msg);
      if (msg == "false") status = false;
      if (msg == "reset") __value = 0u;
      if (msg == "incr") __value += 1u;

      return status;

    };

    ccs::types::uint32 GetCount (void) { return __count; };
    ccs::types::uint32 GetValue (void) { return __value; };

};
  
// Function declaration

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();
static MyMessenger __messenger;

// Function definition
  
TEST(Messenger_Test, Constructor)
{
  sup::core::Messenger* messenger = new (std::nothrow) MyMessenger ();

  bool ret = (static_cast<sup::core::Messenger*>(NULL) != messenger);

  if (ret)
    {
      delete messenger;
    }

  ASSERT_EQ(true, ret);
}

TEST(Messenger_Test, Send)
{
  sup::core::Messenger* messenger = new (std::nothrow) MyMessenger ();

  bool ret = (static_cast<sup::core::Messenger*>(NULL) != messenger);

  if (ret)
    {
      ret = messenger->Send("reset");
    }

  if (ret)
    {
      ret = messenger->Send("incr");
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
      ret = ((1u == __messenger.GetValue()) &&
	     (0u == dynamic_cast<MyMessenger*>(messenger)->GetValue())); // Make sure messages are not received by own senders .. MCAST traffic
    }

  if (ret)
    {
      ret = messenger->Send("echo");
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
#if 0 // There cound be TCN agent traffic picked-up during the test
      ret = (1u == dynamic_cast<MyMessenger*>(messenger)->GetCount()); // Received only one of these messages
#else
      ret = (1u <= dynamic_cast<MyMessenger*>(messenger)->GetCount()); // Received only one of these messages
#endif
    }

  if (ret)
    {
      ret = messenger->Send("reset");
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
      ret = ((0u == __messenger.GetValue()) &&
	     (0u == dynamic_cast<MyMessenger*>(messenger)->GetValue())); // Make sure messages are not received by own senders .. MCAST traffic
    }

  if (ret)
    {
      delete messenger;
    }

  ASSERT_EQ(true, ret);
}

TEST(Messenger_Test, Send_error)
{
  sup::core::Messenger* messenger = new (std::nothrow) MyMessenger (sup::core::Channel::SDN);

  bool ret = (static_cast<sup::core::Messenger*>(NULL) != messenger);

  if (ret)
    {
      ret = messenger->Send("reset");
    }

  if (ret)
    {
      ret = messenger->Send("incr");
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
      ret = ((1u != __messenger.GetValue()) && // Expect failure .. distinct channels
	     (0u == dynamic_cast<MyMessenger*>(messenger)->GetValue())); // Make sure messages are not received by own senders .. MCAST traffic
    }

  if (ret)
    {
      delete messenger;
    }

  ASSERT_EQ(true, ret);
}


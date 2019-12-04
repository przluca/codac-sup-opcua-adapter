/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/DelegatedFunctionHandler-tests.cpp $
* $Id: DelegatedFunctionHandler-tests.cpp 100159 2019-06-18 07:13:41Z bauvirb $
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
#include <common/TimeTools.h> // Misc. helper functions

#include <common/log-api.h> // Syslog wrapper routines

// Local header files

#include "DelegatedFunctionHandler.h"

// Constants

// Type definition

class DelegatedFunctionHandler_Test
{

  private:

  public:

    struct Data_t {
      ccs::types::boolean status = true;
      ccs::types::uint64 timeout = 0ul;
      ccs::types::uint64 current = 0ul;
    };

    bool Execute (void* attr) {

      bool status = (static_cast<Data_t*>(NULL) != attr);

      Data_t dflt;
      Data_t* data = &dflt;

      if (status)
	{
	  data = static_cast<Data_t*>(attr);
	  data->current = ccs::HelperTools::SleepFor(data->timeout);
	}
      
      status = data->status; // Default to true

      return status;
      
    };
  
};

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition
  
TEST(DelegatedFunctionHandler_Test, Constructor)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, Launch_default)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), NULL);
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  log_info("TEST(DelegatedFunctionHandler_Test, Launch_default) - IsValid('%u') == '%s'", uid, (handler->IsValid(uid) ? "true" : "false"));
	  log_info("TEST(DelegatedFunctionHandler_Test, Launch_default) - IsRunning('%u') == '%s'", uid, (handler->IsRunning(uid) ? "true" : "false"));
	  log_info("TEST(DelegatedFunctionHandler_Test, Launch_default) - GetStatus('%u') == '%s'", uid, (handler->GetStatus(uid) ? "true" : "false"));
	  ret = (handler->IsValid(uid) && !handler->IsRunning(uid) && handler->GetStatus(uid));
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, Launch_attribute)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      data.timeout = 1000000000ul;

      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  ret = (handler->IsValid(uid) && handler->IsRunning(uid) && !handler->GetStatus(uid));
	}

      if (ret)
	{
	  ccs::HelperTools::SleepFor(1000000000ul);
	  ret = (handler->IsValid(uid) && !handler->IsRunning(uid) && handler->GetStatus(uid));
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, Launch_concurrent)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      data.timeout = 1000000000ul;

      for (ccs::types::uint32 index = 0u; ret && (index < 5u); index += 1u)
	{
	  ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
	  ret = (0u != uid);
	}

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	}

      if (ret)
	{
	  log_info("TEST(DelegatedFunctionHandler_Test, Launch concurrent) - Check that the threads exist ..");
	  // Compose system command
	  std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
	  
	  ret = (std::system(command.c_str()) == 0);
	  
	  std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
	  std::string result = sstr.str();
	  std::cout << std::endl << result << std::endl;
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  if (ret)
    { // Let this settle
      ccs::HelperTools::SleepFor(1000000000ul);
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, GetAttribute_default)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  ret = (handler->IsValid(uid) && !handler->IsRunning(uid) && handler->GetStatus(uid));
	}

      if (ret)
	{
	  ret = ((NULL != handler->GetAttribute(uid)) &&
		 (&data == static_cast<DelegatedFunctionHandler_Test::Data_t*>(handler->GetAttribute(uid))));
	}

      if (ret)
	{
	  ret = (0ul != (*(static_cast<DelegatedFunctionHandler_Test::Data_t*>(handler->GetAttribute(uid)))).current);
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, GetAttribute_error)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  ret = (handler->IsValid(uid) && !handler->IsRunning(uid) && handler->GetStatus(uid));
	}

      if (ret)
	{
	  ret = (NULL == handler->GetAttribute(uid+1u)); // Invalid uid
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, GetStatus_default)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      data.status = false;

      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  ret = (handler->IsValid(uid) && !handler->IsRunning(uid) && !handler->GetStatus(uid));
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, GetStatus_error)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  ret = (handler->IsValid(uid) && !handler->IsRunning(uid) && handler->GetStatus(uid));
	}

      if (ret)
	{
	  ret = !handler->GetStatus(uid+1u); // Invalid uid
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, Discard_default)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  ret = (handler->IsValid(uid) && !handler->IsRunning(uid) && handler->GetStatus(uid));
	}

      if (ret)
	{
	  ret = handler->Discard(uid);
	}

      if (ret)
	{
	  ret = !handler->IsValid(uid);
	}
    }
  
  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(DelegatedFunctionHandler_Test, Abort_default)
{
  sup::core::DelegatedFunctionHandler* handler = new (std::nothrow) sup::core::DelegatedFunctionHandler ();

  bool ret = (static_cast<sup::core::DelegatedFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      DelegatedFunctionHandler_Test test;
      DelegatedFunctionHandler_Test::Data_t data;

      data.timeout = 10000000000ul; // 10sec

      ccs::types::uint32 uid = handler->Launch(std::bind(&DelegatedFunctionHandler_Test::Execute, test, std::placeholders::_1), static_cast<void*>(&data));
      ret = (0u != uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	  ret = (handler->IsValid(uid) && handler->IsRunning(uid)); // Must be running
	}

      if (ret)
	{
	  log_info("TEST(DelegatedFunctionHandler_Test, Abort_default) - Check that the thread exists ..");
	  // Compose system command
	  std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
	  
	  ret = (std::system(command.c_str()) == 0);
	  
	  std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
	  std::string result = sstr.str();
	  std::cout << std::endl << result << std::endl;
	}

      if (ret)
	{
	  ret = handler->Abort(uid);
	}

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	}

      if (ret)
	{
	  log_info("TEST(DelegatedFunctionHandler_Test, Abort_default) - Check that the thread disappeared ..");
	  // Compose system command
	  std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
	  
	  ret = (std::system(command.c_str()) == 0);
	  
	  std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
	  std::string result = sstr.str();
	  std::cout << std::endl << result << std::endl;
	  //ret = !handler->IsValid(uid);
	}

      if (ret)
	{
	  ret = !handler->IsValid(uid);
	}
      }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}


/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/CVVFClient-tests.cpp $
* $Id: CVVFClient-tests.cpp 100236 2019-06-21 11:00:02Z bauvirb $
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

#include <common/AnyTypeDatabase.h>

#include <common/RPCClient.h>
#include <common/RPCTypes.h>

// Local header files

#include "CVVFFunctionHandler.h"

#include "CVVFClient.h"

// Constants

// Type definition

// Function declaration

extern bool RegisterUserSuppliedCode (void); // Provided by the CVVFServer tests

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();
static bool __user_code_registered = RegisterUserSuppliedCode();

// Function definition

TEST(CVVFClient_Test, Constructor)
{
  sup::core::CVVFClient* client = new (std::nothrow) sup::core::CVVFClient ("rpc@cvvf-service");

  bool ret = (static_cast<sup::core::CVVFClient*>(NULL) != client);

  if (ret)
    {
      delete client;
    }

  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, IsConnected)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      ret = handler->SetParameter("service", "rpc@cvvf-service");

      if (ret)
	{
	  ccs::HelperTools::SleepFor(500000000ul);
	}
      else
	{
	  // Already running
	  ret = true;
	}
    }

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, IsValid)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = (client.IsValid("test::cvvf::ComputeRadius/v1.0") &&
	     client.IsValid("test::types::Input_t/v1.0"));
    }
  
  if (ret)
    {
      ret = (!client.IsValid("UndefinedFunction") &&
	     !client.IsValid("UndefinedType"));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, HasFunction)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = (client.HasFunction("test::cvvf::ComputeRadius/v1.0") &&
	     !client.HasFunction("test::types::Input_t/v1.0") &&
	     !client.HasFunction("UndefinedFunction"));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, HasType)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = (!client.HasType("test::cvvf::ComputeRadius/v1.0") &&
	     client.HasType("test::types::Input_t/v1.0") &&
	     !client.HasType("UndefinedType"));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, GetFunctionDefinition)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = client.HasFunction("test::cvvf::ComputeRadius/v1.0");
    }
  
  sup::core::CVVFFunctionDefinition fundef;

  if (ret)
    {
      fundef = client.GetFunctionDefinition("test::cvvf::ComputeRadius/v1.0");
      ret = (strcmp(fundef.name, "test::cvvf::ComputeRadius/v1.0") == 0);
    }
  
  if (ret && static_cast<bool>(fundef.cfg))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.cfg, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, GetFunctionDefinition) - Retrieved config type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.inp))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.inp, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, GetFunctionDefinition) - Retrieved input type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.out))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.out, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, GetFunctionDefinition) - Retrieved output type '%s'", buffer);
	}
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, GetFunctionDefinition_error)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = !client.HasFunction("UndefinedFunction");
    }
  
  sup::core::CVVFFunctionDefinition fundef;

  if (ret)
    {
      fundef = client.GetFunctionDefinition("UndefinedFunction");
      ret = (ccs::HelperTools::IsUndefinedString(fundef.name) &&
	     !static_cast<bool>(fundef.cfg) &&
	     !static_cast<bool>(fundef.inp) &&
	     !static_cast<bool>(fundef.out));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, GetTypeDefinition)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = static_cast<bool>(client.GetTypeDefinition("test::types::Input_t/v1.0"));
    }
  
  if (ret)
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(client.GetTypeDefinition("test::types::Input_t/v1.0"), buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, GetTypeDefinition) - Retrieved '%s'", buffer);
	}
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, GetTypeDefinition_error)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = !static_cast<bool>(client.GetTypeDefinition("UndefinedType"));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, Execute)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = client.HasFunction("test::cvvf::ComputeRadius/v1.0");
    }
  
  sup::core::CVVFFunctionDefinition fundef;

  if (ret)
    {
      fundef = client.GetFunctionDefinition("test::cvvf::ComputeRadius/v1.0");
      ret = (strcmp(fundef.name, "test::cvvf::ComputeRadius/v1.0") == 0);
    }
  
  if (ret && static_cast<bool>(fundef.cfg))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.cfg, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Execute) - Retrieved config type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.inp))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.inp, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Execute) - Retrieved input type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.out))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.out, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Execute) - Retrieved output type '%s'", buffer);
	}
    }

  if (ret)
    {
      ccs::types::AnyValue cfg (fundef.cfg);
      ccs::types::AnyValue inp (fundef.inp);
      ccs::types::AnyValue out; // Placeholder

      ret = client.Execute(fundef.name, cfg, inp, out);

      char buffer [1024]; 
      out.SerialiseInstance(buffer, 1024u);
      log_info("TEST(CVVFClient_Test, Execute) - Returned output '%s'", buffer);

    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, Execute_error)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    { // Undefined function
      ccs::types::AnyValue cfg, inp, out;
      ret = !client.Execute("UndefinedFunction", cfg, inp, out);
    }
  
  sup::core::CVVFFunctionDefinition fundef;

  if (ret)
    {
      fundef = client.GetFunctionDefinition("test::cvvf::ComputeRadius/v1.0");
      ret = (strcmp(fundef.name, "test::cvvf::ComputeRadius/v1.0") == 0);
    }
  
  if (ret)
    { // Incompatible types
      ccs::types::AnyValue cfg (fundef.cfg);
      ccs::types::AnyValue inp (fundef.inp);
      ccs::types::AnyValue out; // Placeholder

      ret = !client.Execute(fundef.name, inp, cfg, out);
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, Launch)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = client.HasFunction("test::cvvf::ComputeRadius/v1.0");
    }
  
  sup::core::CVVFFunctionDefinition fundef;

  if (ret)
    {
      fundef = client.GetFunctionDefinition("test::cvvf::ComputeRadius/v1.0");
      ret = (strcmp(fundef.name, "test::cvvf::ComputeRadius/v1.0") == 0);
    }
  
  if (ret && static_cast<bool>(fundef.cfg))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.cfg, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Launch) - Retrieved config type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.inp))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.inp, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Launch) - Retrieved input type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.out))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.out, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Launch) - Retrieved output type '%s'", buffer);
	}
    }

  if (ret)
    {
      ccs::types::AnyValue cfg (fundef.cfg);
      ccs::types::AnyValue inp (fundef.inp);

      ccs::types::uint32 uid = client.Launch(fundef.name, cfg, inp);

      ret = (0u < uid);

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	}

      if (ret)
	{
	  ccs::types::AnyValue out; // Placeholder
	  ret = client.GetResult(uid, out);

	  char buffer [1024]; 
	  out.SerialiseInstance(buffer, 1024u);
	  log_info("TEST(CVVFClient_Test, Launch) - Returned output '%s'", buffer);
	}
    }
  
  ASSERT_EQ(true, ret);
}

TEST(CVVFClient_Test, Abort)
{
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance(); // Make sure CVVF service is running

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  sup::core::CVVFClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }

  if (ret)
    {
      ret = client.HasFunction("test::cvvf::ComputeRadius/v1.0");
    }
  
  sup::core::CVVFFunctionDefinition fundef;

  if (ret)
    {
      fundef = client.GetFunctionDefinition("test::cvvf::ComputeRadius/v1.0");
      ret = (strcmp(fundef.name, "test::cvvf::ComputeRadius/v1.0") == 0);
    }
  
  if (ret && static_cast<bool>(fundef.cfg))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.cfg, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Abort) - Retrieved config type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.inp))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.inp, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Abort) - Retrieved input type '%s'", buffer);
	}
    }
  
  if (ret && static_cast<bool>(fundef.out))
    {
      char buffer [1024];
      ret = ccs::HelperTools::Serialise(fundef.out, buffer, 1024u);

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Abort) - Retrieved output type '%s'", buffer);
	}
    }

  if (ret)
    {
      ccs::types::AnyValue cfg (fundef.cfg);
      ccs::types::AnyValue inp (fundef.inp);

      ccs::types::uint64 sleep = 10000000000ul; // 10s

      ret = ccs::HelperTools::SetAttributeValue(&cfg, "sleep", sleep);

      ccs::types::uint32 uid = 0u;

      if (ret)
	{
	  uid = client.Launch(fundef.name, cfg, inp);
	  ret = (0u < uid);

	  if (ret)
	    {
	      log_info("TEST(CVVFClient_Test, Abort) - Retrieved key '%u'", uid);
	    }
	}

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	}

      if (ret)
	{
	  ccs::types::AnyValue out; // Placeholder
	  ret = !client.GetResult(uid, out); // Expect failure .. running
	}

      if (ret)
	{
	  log_info("TEST(CVVFClient_Test, Abort) - Abort('%u') ..", uid);
	  ret = client.Abort(uid);

	  if (ret)
	    {
	      log_info("TEST(CVVFClient_Test, Abort) - .. success");
	    }
	  else
	    {
	      log_error("TEST(CVVFClient_Test, Abort) - .. failure");
	    }
	}

      if (ret)
	{
	  ccs::HelperTools::SleepFor(100000000ul);
	}

      if (ret)
	{
	  ccs::types::AnyValue out; // Placeholder
	  ret = !client.GetResult(uid, out); // Expect failure .. not existing
	}

      if (ret)
	{
	  ret = !client.Abort(uid); // Expect failure .. unknown uid
	}
    }
  
  ASSERT_EQ(true, ret);
}


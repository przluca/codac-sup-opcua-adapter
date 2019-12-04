/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/CVVFFunctionHandler-tests.cpp $
* $Id: CVVFFunctionHandler-tests.cpp 100159 2019-06-18 07:13:41Z bauvirb $
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
#include <common/SquareRoot.h> // Misc. helper functions

#include <common/log-api.h> // Syslog wrapper routines

#include <common/AnyTypeDatabase.h>

#include <common/RPCClient.h>
#include <common/RPCTypes.h>

// Local header files

#include "FunctionDatabase.h"
#include "CVVFFunctionHandler.h"

// Constants

// Type definition

typedef struct __attribute__((packed)) Config {
  ccs::types::boolean enabled = true;
  ccs::types::uint64 sleep = 0ul;
} Config_t;

typedef struct __attribute__((packed)) Input {
  ccs::types::float32 x = 0.f;
  ccs::types::float32 y = 0.f;
  ccs::types::float32 z = 0.f;
} Input_t;

typedef struct __attribute__((packed)) Output {
  ccs::types::float32 r = 0.f;
} Output_t;

// Function declaration

bool ComputeRadius (const Config_t* cfg, const Input_t* inp, Output_t* out);
bool RegisterUserSuppliedCode (void);

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();
static bool __initialised = RegisterUserSuppliedCode();

// Function definition

bool ComputeRadius (const Config_t* cfg, const Input_t* inp, Output_t* out)
{

  log_info("test::cvvf::ComputeRadius/v1.0 - Entering method");

  bool status = ((static_cast<Config_t*>(NULL) != cfg) && 
		 (static_cast<Input_t*>(NULL) != inp) && 
		 (static_cast<Output_t*>(NULL) != out));

  if (status)
    {
      out->r = 0.f;
    }

  if (status && cfg->enabled)
    {
      log_info("test::cvvf::ComputeRadius/v1.0 - Computation enabled with inputs ['%f %f %f']", inp->x, inp->y, inp->z);

      ccs::types::float32 xsq = inp->x*inp->x;
      ccs::types::float32 ysq = inp->y*inp->y;
      ccs::types::float32 zsq = inp->z*inp->z;
      
      out->r = SquareRoot<ccs::types::float32>(xsq+ysq+zsq);
    }

  if (status)
    {
      ccs::HelperTools::SleepFor(cfg->sleep);
    }

  log_info("test::cvvf::ComputeRadius/v1.0 - Leaving method");

  return status;

}

bool RegisterUserSuppliedCode (void)
{

  // Declare application types
  std::shared_ptr<ccs::types::CompoundType> __cfg = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("test::types::Config_t/v1.0"))
											      ->AddAttribute<ccs::types::boolean>("enabled")
											      ->AddAttribute<ccs::types::uint64>("sleep"));
  std::shared_ptr<ccs::types::CompoundType> __inp = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("test::types::Input_t/v1.0"))
											      ->AddAttribute<ccs::types::float32>("x")
											      ->AddAttribute<ccs::types::float32>("y")
											      ->AddAttribute<ccs::types::float32>("z"));
  std::shared_ptr<ccs::types::CompoundType> __out = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("test::types::Output_t/v1.0"))
											      ->AddAttribute<ccs::types::float32>("r"));

  bool status = ((__cfg ? true : false) && (__inp ? true : false) && (__out ? true : false));

  // Register application types
  if (status)
    {
      status = (ccs::types::GlobalTypeDatabase::Register(__cfg) && 
		ccs::types::GlobalTypeDatabase::Register(__inp) && 
		ccs::types::GlobalTypeDatabase::Register(__out));
    }

  // Register application code
  if (status)
    {
      ccs::types::FunctionDefinition_t def;
      ccs::HelperTools::SafeStringCopy(def.name, "test::cvvf::ComputeRadius/v1.0", ccs::types::MaxStringLength);
      ccs::HelperTools::SafeStringCopy(def.input, __inp->GetName(), ccs::types::MaxStringLength);
      ccs::HelperTools::SafeStringCopy(def.output, __out->GetName(), ccs::types::MaxStringLength);
      ccs::HelperTools::SafeStringCopy(def.config, __cfg->GetName(), ccs::types::MaxStringLength);
      def.function = reinterpret_cast<void*>(&ComputeRadius);
      status = ccs::types::GlobalFunctionDatabase::Register(def);
    }

  return status;

}

  
TEST(CVVFFunctionHandler_Test, Constructor)
{
  sup::core::CVVFFunctionHandler* handler = new (std::nothrow) sup::core::CVVFFunctionHandler ();

  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(CVVFFunctionHandler_Test, SetParameter)
{
#if 0 // Singleton pattern with global scope
  sup::core::CVVFFunctionHandler* handler = new (std::nothrow) sup::core::CVVFFunctionHandler ();
#else
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance();
#endif
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
  
  if (ret)
    {
      // Test service exists
      ccs::base::RPCClient client ("rpc@cvvf-service");
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }
#if 0 // Singleton pattern with global scope
  if (ret)
    {
      delete handler;
    }
#endif
  ASSERT_EQ(true, ret);
}

TEST(CVVFFunctionHandler_Test, ResolveType_default)
{
#if 0 // Singleton pattern with global scope
  sup::core::CVVFFunctionHandler* handler = new (std::nothrow) sup::core::CVVFFunctionHandler ();
#else
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance();
#endif
  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  ccs::base::RPCClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }
  
  if (ret)
    {
      // Resolve symbol .. Request with string 'value' field
      ccs::types::CompoundType resolve_request_struct (*ccs::base::RPCTypes::Request_int);
      resolve_request_struct.AddAttribute<ccs::types::string>("value");

      ccs::types::AnyValue request (resolve_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve");
      ccs::HelperTools::SetAttributeValue(&request, "value", "test::types::Input_t/v1.0");

      log_info("TEST(CVVFFunctionHandler_Test, ResolveType_default) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, ResolveType_default) - .. received reply with status '%s'", (reply_status ? "true" : "false"));
  
      if (reply_status)
	{
#if 0
	  std::shared_ptr<const ccs::types::AnyType> type = ccs::HelperTools::GetAttributeType(&reply, "value");
	  ret = (type ? true : false);
#else
	  ccs::types::AnyValue type = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&reply, "value");
	  ret = (type.GetType()->GetSize() == sizeof(Input_t));

	  if (ret)
	    {
	      char buffer [1024]; type.SerialiseInstance(buffer, 1024u);
	      log_info("TEST(CVVFFunctionHandler_Test, ResolveType_default) - .. and type '%s'", buffer);
	    }
#endif
	}
      else
	{
	  ret = reply_status;
	}
    }
#if 0 // Singleton pattern with global scope  
  if (ret)
    {
      delete handler;
    }
#endif
  ASSERT_EQ(true, ret);
}

TEST(CVVFFunctionHandler_Test, ResolveType_error)
{
#if 0 // Singleton pattern with global scope
  sup::core::CVVFFunctionHandler* handler = new (std::nothrow) sup::core::CVVFFunctionHandler ();
#else
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance();
#endif
  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  ccs::base::RPCClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }
  
  if (ret)
    {
      // Resolve symbol .. Request with string 'value' field
      ccs::types::CompoundType resolve_request_struct (*ccs::base::RPCTypes::Request_int);
      resolve_request_struct.AddAttribute<ccs::types::string>("value");

      ccs::types::AnyValue request (resolve_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve");
      ccs::HelperTools::SetAttributeValue(&request, "value", "UnknownType");

      log_info("TEST(CVVFFunctionHandler_Test, ResolveType_error) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, ResolveType_error) - .. received reply with status '%s'", (reply_status ? "true" : "false"));
  
      ret = (false == reply_status);
    }
#if 0 // Singleton pattern with global scope  
  if (ret)
    {
      delete handler;
    }
#endif
  ASSERT_EQ(true, ret);
}

TEST(CVVFFunctionHandler_Test, ResolveFunction_default)
{
#if 0 // Singleton pattern with global scope
  sup::core::CVVFFunctionHandler* handler = new (std::nothrow) sup::core::CVVFFunctionHandler ();
#else
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance();
#endif
  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  ccs::base::RPCClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }
  
  if (ret)
    {
      // Resolve symbol .. Request with string 'value' field
      ccs::types::CompoundType resolve_request_struct (*ccs::base::RPCTypes::Request_int);
      resolve_request_struct.AddAttribute<ccs::types::string>("value");

      ccs::types::AnyValue request (resolve_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve");
      ccs::HelperTools::SetAttributeValue(&request, "value", "test::cvvf::ComputeRadius/v1.0");

      log_info("TEST(CVVFFunctionHandler_Test, ResolveFunction_default) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, ResolveFunction_default) - .. received reply with status '%s'", (reply_status ? "true" : "false"));
  
      ret = (true == reply_status);
    }
#if 0 // Singleton pattern with global scope  
  if (ret)
    {
      delete handler;
    }
#endif
  ASSERT_EQ(true, ret);
}

TEST(CVVFFunctionHandler_Test, ResolveFunction_error)
{
#if 0 // Singleton pattern with global scope
  sup::core::CVVFFunctionHandler* handler = new (std::nothrow) sup::core::CVVFFunctionHandler ();
#else
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance();
#endif
  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  ccs::base::RPCClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }
  
  if (ret)
    {
      // Resolve symbol .. Request with string 'value' field
      ccs::types::CompoundType resolve_request_struct (*ccs::base::RPCTypes::Request_int);
      resolve_request_struct.AddAttribute<ccs::types::string>("value");

      ccs::types::AnyValue request (resolve_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve");
      ccs::HelperTools::SetAttributeValue(&request, "value", "UnknownFunction");

      log_info("TEST(CVVFFunctionHandler_Test, ResolveFunction_error) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, ResolveFunction_error) - .. received reply with status '%s'", (reply_status ? "true" : "false"));
  
      ret = (false == reply_status);
    }
#if 0 // Singleton pattern with global scope  
  if (ret)
    {
      delete handler;
    }
#endif
  ASSERT_EQ(true, ret);
}

TEST(CVVFFunctionHandler_Test, CallFunction_default)
{
#if 0 // Singleton pattern with global scope
  sup::core::CVVFFunctionHandler* handler = new (std::nothrow) sup::core::CVVFFunctionHandler ();
#else
  sup::core::CVVFFunctionHandler* handler = sup::cvvf::GlobalFunctionHandler::GetInstance();
#endif
  bool ret = (static_cast<sup::core::CVVFFunctionHandler*>(NULL) != handler);

  ccs::base::RPCClient client ("rpc@cvvf-service");

  if (ret)
    {
      // Test service exists
      ccs::HelperTools::SleepFor(500000000ul);
      ret = client.IsConnected();
    }
  
  std::shared_ptr<const ccs::types::AnyType> __cfg;
  std::shared_ptr<const ccs::types::AnyType> __inp;

  if (ret)
    {
      // Resolve symbol .. Request with string 'value' field
      ccs::types::CompoundType resolve_request_struct (*ccs::base::RPCTypes::Request_int);
      resolve_request_struct.AddAttribute<ccs::types::string>("value");

      ccs::types::AnyValue request (resolve_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve");
      ccs::HelperTools::SetAttributeValue(&request, "value", "test::types::Config_t/v1.0");

      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - .. received reply with status '%s'", (reply_status ? "true" : "false"));
  
      if (reply_status)
	{
	  __cfg = ccs::HelperTools::GetAttributeType(&reply, "value");
	  ret = (__cfg ? true : false);
	}
     else
	{
	  ret = reply_status;
	}
    }

  if (ret)
    {
      // Resolve symbol .. Request with string 'value' field
      ccs::types::CompoundType resolve_request_struct (*ccs::base::RPCTypes::Request_int);
      resolve_request_struct.AddAttribute<ccs::types::string>("value");

      ccs::types::AnyValue request (resolve_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve");
      ccs::HelperTools::SetAttributeValue(&request, "value", "test::types::Input_t/v1.0");

      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - .. received reply with status '%s'", (reply_status ? "true" : "false"));
  
      if (reply_status)
	{
	  __inp = ccs::HelperTools::GetAttributeType(&reply, "value");
	  ret = (__inp ? true : false);
	}
     else
	{
	  ret = reply_status;
	}
    }

  ccs::types::uint32 key = 0u;

  if (ret)
    {
      // Call function with appropriate input arguments
      ccs::types::CompoundType arguments ("cvvf::Combined_t");
      arguments.AddAttribute("name", "string");
      arguments.AddAttribute("config", __cfg);
      arguments.AddAttribute("input", __inp);

      ccs::types::CompoundType call_request_struct (*ccs::base::RPCTypes::Request_int);
      call_request_struct.AddAttribute("value", arguments);

      ccs::types::AnyValue request (call_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "launch");
      ccs::HelperTools::SetAttributeValue(&request, "value.name", "test::cvvf::ComputeRadius/v1.0");
      ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&request, "value.config").ParseInstance("{\"enabled\":true,\"sleep\":0}");
      ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&request, "value.input").ParseInstance("{\"x\":1,\"y\":1,\"z\":1}");

      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - .. received reply with status '%s'", (reply_status ? "true" : "false"));
  
      if (reply_status)
	{
	  key = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&reply, "value");
	  ret = (0u != key);
	}
      else
	{
	  ret = reply_status;
	}
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(500000000ul);
    }

  if (ret)
    {
      // Collect result .. Request with integer 'value' field
      ccs::types::CompoundType collect_request_struct (*ccs::base::RPCTypes::Request_int);
      collect_request_struct.AddAttribute<ccs::types::uint32>("value");

      ccs::types::AnyValue request (collect_request_struct);

      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "result");
      ccs::HelperTools::SetAttributeValue(&request, "value", key);

      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - Send RPC request ..");
      ccs::types::AnyValue reply = client.SendRequest(request);
      ccs::types::boolean reply_status = false;
      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - .. received reply with status '%s'", (reply_status ? "true" : "false"));

      if (reply_status)
	{
	  ccs::types::AnyValue value = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&reply, "value");
	  ret = (value.GetType()->GetSize() == sizeof(Output_t));

	  if (ret)
	    {
	      char buffer [1024]; value.SerialiseInstance(buffer, 1024u);
	      log_info("TEST(CVVFFunctionHandler_Test, CallFunction_default) - .. and value '%s'", buffer);
	    }
	}
      else
	{
	  ret = reply_status;
	}
    }
#if 0 // Singleton pattern with global scope  
  if (ret)
    {
      delete handler;
    }
#endif
  ASSERT_EQ(true, ret);
}


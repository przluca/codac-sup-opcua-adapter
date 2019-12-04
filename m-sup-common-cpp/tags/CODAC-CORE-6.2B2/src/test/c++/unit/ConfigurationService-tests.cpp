/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/ConfigurationService-tests.cpp $
* $Id: ConfigurationService-tests.cpp 100236 2019-06-21 11:00:02Z bauvirb $
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

#include <common/ObjectDatabase.h>
#include <common/ObjectFactory.h>

#include <common/RPCClient.h>
#include <common/RPCTypes.h>

#include <common/CyclicRedundancyCheck.h>
#include <common/Hash.h>

// Local header files

#include "ConfigurationHandler.h"
#include "ConfigurationHasher.h"
#include "ConfigurationService.h"

// Constants

// Type definition

namespace csrv {

class Handler : public sup::core::ConfigurationHandler {
  
  private:

    std::shared_ptr<const ccs::types::AnyType> __config_type;
    std::shared_ptr<const ccs::types::AnyType> __limits_type;
  
  public:
  
    struct __attribute__((packed)) Config_t {
      ccs::types::boolean enabled = true;
      ccs::types::float64 setpoint = 0.0;
    } __config_data;
  
    struct __attribute__((packed)) Limits_t {
      ccs::types::float64 max = +10.0;
      ccs::types::float64 min = -10.0;
    } __limits_data;
  
    Handler (void) {
      // Introspectable type definition
      __config_type = std::shared_ptr<const ccs::types::AnyType>(((new (std::nothrow) ccs::types::CompoundType ("user::Config_t"))
								  ->AddAttribute("enabled", "bool")
								  ->AddAttribute("setpoint", "float64")));
      __limits_type = std::shared_ptr<const ccs::types::AnyType>(((new (std::nothrow) ccs::types::CompoundType ("user::Limits_t"))
								  ->AddAttribute("maximum", "float64")
								  ->AddAttribute("minimum", "float64")));
    };

    virtual ~Handler (void) {};
  
    // Read configuration data set
    virtual bool GetSeed (const std::string& name, ccs::types::uint32& value) const {
    
      bool status = true;

      value = ccs::HelperTools::Hash<ccs::types::uint32>(name.c_str());

      if (name == "base")
	{
	  status = sup::core::ConfigurationHandler::GetSeed(name, value);
	}
      else if (name == "throw")
	{
	  throw std::runtime_error("Invalid alias");
	}
      else if (name == "uint")
	{
	  throw 1u;
	}

      return status;
    
    };

    // Read configuration data set
    virtual bool ReadConfiguration (const std::string& name, ccs::types::AnyValue& value) const {
    
      bool status = !name.empty();

      log_info("Handler::ReadConfiguration - Entering method");
      
      if (name == "config")
	{
	  ccs::types::AnyValue config (__config_type);
	  value = config;
	  value = __config_data;
	}
      else if (name == "limits")
	{
	  ccs::types::AnyValue config (__limits_type);
	  value = config;
	  value = __limits_data;
	}
      else if (name == "throw")
	{
	  throw std::runtime_error("Invalid alias");
	}
      else if (name == "uint")
	{
	  throw 1u;
	}
      else
	{
	  status = false;
	}
      
      log_info("Handler::ReadConfiguration - Leaving method");
      
      return status;
    
    };

    // Load configuration data set
    virtual bool LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum) {

      bool status = !name.empty();
      
      log_info("Handler::LoadConfiguration - Entering method");
      
      if (name == "config")
	{
	  __config_data = static_cast<Config_t>(value);
	  status = (checksum == ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(&__config_data), sizeof(Config_t), seed));
	}
      else if (name == "limits")
	{
	  __limits_data = static_cast<Limits_t>(value);
	  status = (checksum == ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(&__limits_data), sizeof(Limits_t), seed));
	}
      else if (name == "throw")
	{
	  throw std::runtime_error("Invalid alias");
	}
      else if (name == "uint")
	{
	  throw 1u;
	}
      else
	{
	  status = false;
	}
      
      log_info("Handler::LoadConfiguration - Leaving method");
      
      return status;
      
    };

  bool TestConfiguration (struct Config_t value) {

    bool status = ((value.enabled == __config_data.enabled) &&
		   (value.setpoint == __config_data.setpoint));

    return status;

  }
  
  bool TestConfiguration (struct Limits_t value) {

    bool status = ((value.max == __limits_data.max) &&
		   (value.min == __limits_data.min));

    return status;

  }
  
};
  
// Function declaration

static inline bool ConfigurationService_Initialise (void);

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();
static sup::core::ConfigurationService* loader = static_cast<sup::core::ConfigurationService*>(NULL);
static ccs::base::RPCClient* client = new (std::nothrow) ccs::base::RPCClient ("Service@SomePlantSystem");

static Handler* handler = static_cast<Handler*>(NULL);
static sup::core::ConfigurationHasher* hasher = static_cast<sup::core::ConfigurationHasher*>(NULL);

static bool is_initialised = ConfigurationService_Initialise();

// Function definition

static inline bool ConfigurationService_Initialise (void)
{

  bool status = ((static_cast<Handler*>(NULL) != handler) && (static_cast<sup::core::ConfigurationHasher*>(NULL) != hasher));

  if (!status)
    {
      handler = new (std::nothrow) Handler ();
      hasher = new (std::nothrow) sup::core::ConfigurationHasher ();
      status = ((static_cast<Handler*>(NULL) != handler) && (static_cast<sup::core::ConfigurationHasher*>(NULL) != hasher));
    }

  if (status)
    {
      log_info("Initialise - Register instances ..");
      status = (ccs::base::GlobalObjectDatabase::Register("MyHandler", dynamic_cast<ccs::base::AnyObject*>(handler)) &&
		ccs::base::GlobalObjectDatabase::Register("MyHasher", dynamic_cast<ccs::base::AnyObject*>(hasher)));

      if (status)
	{
	  log_info("Initialise - .. success");
	}
      else
	{
	  log_error("Initialise - .. failure");
	}
    }

  return status;

}

// cppcheck-suppress syntaxError // Obvious false positive for unidentified reason .. name expansion through TEST macro ?
TEST(ConfigurationService_Test, Constructor)
{
  sup::core::ConfigurationService* service = new (std::nothrow) sup::core::ConfigurationService ();

  bool ret = (static_cast<sup::core::ConfigurationService*>(NULL) != service);

  if (ret)
    {
      delete service;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, Factory)
{
  bool ret = ccs::base::GlobalObjectFactory::IsValid("sup::core::ConfigurationService");

  sup::core::ConfigurationService* service = static_cast<sup::core::ConfigurationService*>(NULL);

  if (ret)
    {
      service = dynamic_cast<sup::core::ConfigurationService*>(ccs::base::GlobalObjectFactory::Instantiate("sup::core::ConfigurationService"));
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != service);
    }

  if (ret)
    {
      ccs::base::GlobalObjectFactory::Terminate(dynamic_cast<ccs::base::AnyObject*>(service));
    }

  ASSERT_EQ(true, ret);
}
#if 0 // Suspect the PVA service does not disappear upon deleting the instance ...
TEST(ConfigurationService_Test, SetService)
{
  sup::core::ConfigurationService* service = new (std::nothrow) sup::core::ConfigurationService ();

  bool ret = (static_cast<sup::core::ConfigurationService*>(NULL) != service);

  if (ret)
    {
      ret = service->SetService("Service@SomePlantSystem");
      ccs::HelperTools::SleepFor(500000000ul);
    }

  if (ret)
    {
      ret = client->IsConnected();
    }

  if (ret)
    {
      delete service;
    }

  ASSERT_EQ(true, ret);
}
#endif
TEST(ConfigurationService_Test, SetService_error)
{
  sup::core::ConfigurationService* service = new (std::nothrow) sup::core::ConfigurationService ();

  bool ret = (static_cast<sup::core::ConfigurationService*>(NULL) != service);

  if (ret)
    {
      ret = (false == service->SetService(static_cast<const char*>(NULL))); // Expect failure
    }

  if (ret)
    {
      ret = (false == service->SetService("")); // Expect failure
    }

  if (ret)
    {
      delete service;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, SetParameter_verbose)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<Handler*>(NULL) != handler));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetParameter("service", "Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = loader->SetParameter("verbose", "true");
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, SetParameter)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<Handler*>(NULL) != handler));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetParameter("service", "Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = loader->SetParameter("undefined", "undefined");
    }

  if (ret)
    {
      ret = (false == loader->SetParameter("", "")); // Expect failure
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, ProcessMessage)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<Handler*>(NULL) != handler));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetParameter("service", "Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = loader->ProcessMessage("undefined");
    }

  if (ret)
    {
      ret = (false == loader->ProcessMessage("")); // Expect failure
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, SetService_parameter)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<Handler*>(NULL) != handler));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetParameter("service", "Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RegisterHandler)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<Handler*>(NULL) != handler));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetParameter("service", "Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = loader->RegisterHandler(handler);
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RegisterHandler_name)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<Handler*>(NULL) != handler));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetParameter("service", "Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = loader->SetParameter("handler", "MyHandler");
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RegisterHandler_error)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<Handler*>(NULL) != handler));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetParameter("service", "Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = (false == loader->RegisterHandler(static_cast<sup::core::ConfigurationHandler*>(NULL)));
    }

  if (ret)
    {
      ret = (false == loader->RegisterHandler("UndefinedHandler"));
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RegisterHasher)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<sup::core::ConfigurationHasher*>(NULL) != hasher));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetService("Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = loader->RegisterHasher(hasher);
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RegisterHasher_name)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<sup::core::ConfigurationHasher*>(NULL) != hasher));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetService("Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = loader->SetParameter("hasher", "MyHasher");
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RegisterHasher_error)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<sup::core::ConfigurationHasher*>(NULL) != hasher));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = loader->SetService("Service@SomePlantSystem");
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = (false == loader->RegisterHasher(static_cast<sup::core::ConfigurationHasher*>(NULL)));
    }

  if (ret)
    {
      ret = (false == loader->RegisterHasher("UndefinedHasher"));
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RPCClient_eget)
{
  bool ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = (loader->SetService("Service@SomePlantSystem") && loader->RegisterHandler(handler));
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    { // Compose system command
      std::string command = std::string("/usr/bin/eget -s 'Service@SomePlantSystem' -a qualifier=init > /tmp/eget.out");
      ret = (std::system(command.c_str()) == 0);
      std::stringstream sstr; sstr << std::ifstream("/tmp/eget.out").rdbuf();
      std::string result = sstr.str();
      log_info("TEST(ConfigurationService_Test, RPCClient_eget) - Check service exist ..");
      std::cout << result << std::endl;
    }

  if (ret)
    { // Compose system command
      std::string command = std::string("/usr/bin/eget -s 'Service@SomePlantSystem' -a qualifier=echo > /tmp/eget.out");
      ret = (std::system(command.c_str()) == 0);
      std::stringstream sstr; sstr << std::ifstream("/tmp/eget.out").rdbuf();
      std::string result = sstr.str();
      log_info("TEST(ConfigurationService_Test, RPCClient_eget) - ..");
      std::cout << result << std::endl;
    }

  if (ret)
    { // Compose system command
      std::string command = std::string("/usr/bin/eget -s 'Service@SomePlantSystem' -a undefined=undefined > /tmp/eget.out");
      ret = (std::system(command.c_str()) == 0);
      std::stringstream sstr; sstr << std::ifstream("/tmp/eget.out").rdbuf();
      std::string result = sstr.str();
      log_info("TEST(ConfigurationService_Test, RPCClient_eget) - ..");
      std::cout << result << std::endl;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RPCClient_SendRequest_seed)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<ccs::base::RPCClient*>(NULL) != client));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = (loader->SetService("Service@SomePlantSystem") && loader->RegisterHandler(handler));
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = client->IsConnected();
    }

  // Copy the base request type ..
  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Default RPC request type

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "init");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::UnsignedInteger32 == ccs::HelperTools::GetAttributeType(&reply, "value")) &&
	     (ccs::HelperTools::Hash<ccs::types::uint32>("") == ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&reply, "value")));
    }

  // .. and add the missing bit
  request_type.AddAttribute<ccs::types::string>("alias");

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "init");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "config");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::UnsignedInteger32 == ccs::HelperTools::GetAttributeType(&reply, "value")) &&
	     (ccs::HelperTools::Hash<ccs::types::uint32>("config") == ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&reply, "value")));
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "init");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "base");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = (ccs::types::UnsignedInteger32 == ccs::HelperTools::GetAttributeType(&reply, "value"));
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "init");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "throw");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&reply, "status")) &&
	     (false == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status")));
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "init");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "uint");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&reply, "status")) &&
	     (false == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status")));
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RPCClient_SendRequest_read)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<ccs::base::RPCClient*>(NULL) != client));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = (loader->SetService("Service@SomePlantSystem") && loader->RegisterHandler(handler));
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = client->IsConnected();
    }

  // Copy the base request type ..
  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Default RPC request type
  // .. and add the missing bit
  request_type.AddAttribute<ccs::types::string>("alias");

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "read");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "config");

      ccs::types::AnyValue reply = client->SendRequest(request);

      try
	{
	  ret = ((sizeof(Handler::Config_t) == ccs::HelperTools::GetAttributeType(&reply, "value")->GetSize()) &&
		 (ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&reply, "value.enabled")) &&
		 (ccs::types::Float64 == ccs::HelperTools::GetAttributeType(&reply, "value.setpoint")) &&
		 (true == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "value.enabled")) &&
		 (0.0 == ccs::HelperTools::GetAttributeValue<ccs::types::float64>(&reply, "value.setpoint")));
	}
      catch (const std::exception& e)
	{
	  log_error("TEST(ConfigurationService_Test, RPCClient_SendRequest_read) - '%s' exception", e.what());
	  ret = false;
	}
      catch (const ccs::types::uint32& e)
	{
	  log_error("TEST(ConfigurationService_Test, RPCClient_SendRequest_read) - '%u' exception", e);
	  ret = false;
	}
      catch (...)
	{
	  log_error("TEST(ConfigurationService_Test, RPCClient_SendRequest_read) - Unknown exception");
	  ret = false;
	}
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "read");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "limits");

      ccs::types::AnyValue reply = client->SendRequest(request);

      try
	{
	  ret = ((sizeof(Handler::Limits_t) == ccs::HelperTools::GetAttributeType(&reply, "value")->GetSize()) &&
		 (ccs::types::Float64 == ccs::HelperTools::GetAttributeType(&reply, "value.maximum")) &&
		 (ccs::types::Float64 == ccs::HelperTools::GetAttributeType(&reply, "value.minimum")) &&
		 (+10.0 == ccs::HelperTools::GetAttributeValue<ccs::types::float64>(&reply, "value.maximum")) &&
		 (-10.0 == ccs::HelperTools::GetAttributeValue<ccs::types::float64>(&reply, "value.minimum")));
	}
      catch (const std::exception& e)
	{
	  log_error("TEST(ConfigurationService_Test, RPCClient_SendRequest_read) - '%s' exception", e.what());
	  ret = false;
	}
      catch (const ccs::types::uint32& e)
	{
	  log_error("TEST(ConfigurationService_Test, RPCClient_SendRequest_read) - '%u' exception", e);
	  ret = false;
	}
      catch (...)
	{
	  log_error("TEST(ConfigurationService_Test, RPCClient_SendRequest_read) - Unknown exception");
	  ret = false;
	}
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "read");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "throw");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&reply, "status")) &&
	     (false == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status")));
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "read");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "uint");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&reply, "status")) &&
	     (false == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status")));
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationService_Test, RPCClient_SendRequest_load)
{
  bool ret = ((static_cast<sup::core::ConfigurationService*>(NULL) != loader) &&
	      (static_cast<ccs::base::RPCClient*>(NULL) != client));

  if (!ret) // Static initialisation
    {
      loader = new (std::nothrow) sup::core::ConfigurationService ();
      ret = (static_cast<sup::core::ConfigurationService*>(NULL) != loader);

      if (ret)
	{
	  ret = (loader->SetService("Service@SomePlantSystem") && loader->RegisterHandler(handler));
	  ccs::HelperTools::SleepFor(500000000ul);
	} 
    }

  if (ret)
    {
      ret = client->IsConnected();
    }

  // Copy the base request type ..
  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Default RPC request type
  // .. and add the missing bit
  request_type.AddAttribute<ccs::types::string>("alias");

  ccs::types::AnyValue config_value;

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "read");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "config");

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status");

      if (ret)
	{
	  config_value = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&reply, "value");
	}
    }

  Handler::Config_t config_struct; 
  
  if (ret)
    {
      config_struct.enabled = true; config_struct.setpoint = 5.0;
      config_value = config_struct;
    }

  // .. and add the missing bit
  request_type.AddAttribute<ccs::types::uint32>("seed");
  request_type.AddAttribute("value", config_value.GetType());
  request_type.AddAttribute<ccs::types::uint32>("hash");

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "load");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "config");
      ccs::HelperTools::SetAttributeValue(&request, "seed", 0u);
      ccs::HelperTools::SetAttributeValue(&request, "value", config_value);
      ccs::HelperTools::SetAttributeValue(&request, "hash", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(config_value.GetInstance()), config_value.GetType()->GetSize()));

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status");
    }

  if (ret)
    {
      ret = handler->TestConfiguration(config_struct);
    }

  if (ret) // Wrong CRC
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "load");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "config");
      ccs::HelperTools::SetAttributeValue(&request, "seed", 0u);
      ccs::HelperTools::SetAttributeValue(&request, "value", config_value);
      ccs::HelperTools::SetAttributeValue(&request, "hash", 0u);

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = !ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status"); // Expect failure
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "load");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "throw");
      ccs::HelperTools::SetAttributeValue(&request, "seed", 0u);
      ccs::HelperTools::SetAttributeValue(&request, "value", config_value);
      ccs::HelperTools::SetAttributeValue(&request, "hash", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(config_value.GetInstance()), config_value.GetType()->GetSize()));

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&reply, "status")) &&
	     (false == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status")));
    }

  if (ret)
    {
      ccs::types::AnyValue request (request_type);
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "load");
      ccs::HelperTools::SetAttributeValue(&request, "alias", "uint");
      ccs::HelperTools::SetAttributeValue(&request, "seed", 0u);
      ccs::HelperTools::SetAttributeValue(&request, "value", config_value);
      ccs::HelperTools::SetAttributeValue(&request, "hash", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(config_value.GetInstance()), config_value.GetType()->GetSize()));

      ccs::types::AnyValue reply = client->SendRequest(request);

      ret = ((ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&reply, "status")) &&
	     (false == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status")));
    }

  ASSERT_EQ(true, ret);
}

} // namespace csrv

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/ConfigurationLoader-tests.cpp $
* $Id: ConfigurationLoader-tests.cpp 100236 2019-06-21 11:00:02Z bauvirb $
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
#include "ConfigurationLoader.h"

// Constants

// Type definition

namespace clod {

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
  
class Hasher : public sup::core::ConfigurationHasher {
  
  private:

  public:
  
    Hasher (void) {};

    virtual ~Hasher (void) {};
  
    // Compute checksum
    virtual bool ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum) {
    
      bool status = true;

      checksum = ccs::HelperTools::Hash<ccs::types::uint32>(name.c_str());

      if (name == "false")
	{
	  status = false;
	}

      return status;
    
    };

};
  
// Function declaration

static inline bool ConfigurationService_Initialise (void);

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// ToDo - Replace with RPCService to verify the protocol ...
static sup::core::ConfigurationService* service = static_cast<sup::core::ConfigurationService*>(NULL);

static Handler* handler = static_cast<Handler*>(NULL);

// ToDo - Own Hasher implementation ..
static sup::core::ConfigurationHasher* hasher = static_cast<sup::core::ConfigurationHasher*>(NULL);

static bool is_initialised = ConfigurationService_Initialise();

// Function definition

static inline bool ConfigurationService_Initialise (void)
{

  bool status = ((static_cast<sup::core::ConfigurationService*>(NULL) != service) && 
		 (static_cast<Handler*>(NULL) != handler) && 
		 (static_cast<sup::core::ConfigurationHasher*>(NULL) != hasher));

  if (!status)
    {
      service = new (std::nothrow) sup::core::ConfigurationService ();
      handler = new (std::nothrow) Handler ();
      hasher = new (std::nothrow) Hasher ();

      status = ((static_cast<sup::core::ConfigurationService*>(NULL) != service) && 
		(static_cast<Handler*>(NULL) != handler) && 
		(static_cast<Hasher*>(NULL) != hasher));
    }

  if (status)
    {
      status = service->SetService("ForLoader@SomePlantSystem");
    }

  if (status)
    {
      status = service->RegisterHandler(handler);
    }

  // Do not yet register hasher ..

  return status;

}

// cppcheck-suppress syntaxError // Obvious false positive for unidentified reason .. name expansion through TEST macro ?
TEST(ConfigurationLoader_Test, Constructor)
{
  sup::core::ConfigurationLoader* loader = new (std::nothrow) sup::core::ConfigurationLoader ("ForLoader@SomePlantSystem");

  bool ret = (static_cast<sup::core::ConfigurationLoader*>(NULL) != loader);

  if (ret)
    {
      delete loader;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationLoader_Test, IsConnected)
{
  sup::core::ConfigurationLoader* loader = new (std::nothrow) sup::core::ConfigurationLoader ("ForLoader@SomePlantSystem");

  bool ret = (static_cast<sup::core::ConfigurationLoader*>(NULL) != loader);

  if (ret)
    {
      ccs::HelperTools::SleepFor(500000000ul);
      ret = loader->IsConnected();
    }

  if (ret)
    {
      delete loader;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationLoader_Test, ReadConfiguration)
{
  sup::core::ConfigurationLoader* loader = new (std::nothrow) sup::core::ConfigurationLoader ("ForLoader@SomePlantSystem");

  bool ret = (static_cast<sup::core::ConfigurationLoader*>(NULL) != loader);

  if (ret)
    {
      ccs::HelperTools::SleepFor(500000000ul);
      ret = loader->IsConnected();
    }

  if (ret)
    {
      ccs::types::AnyValue config;
      ret = !loader->ReadConfiguration("", config); // Expect failure
    }

  if (ret)
    {
      ccs::types::AnyValue config;
      ret = !loader->ReadConfiguration("throw", config); // Expect failure
    }

  if (ret)
    {
      ccs::types::AnyValue config;
      ret = loader->ReadConfiguration("config", config);

      if (ret)
	{
	  ret = (NULL != config.GetInstance());
	}
    }

  if (ret)
    {
      ccs::types::AnyValue config;
      ret = loader->ReadConfiguration("limits", config);

      if (ret)
	{
	  ret = (NULL != config.GetInstance());
	}
    }

  // ToDo - Test returned structures

  if (ret)
    {
      delete loader;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationLoader_Test, LoadConfiguration)
{
  sup::core::ConfigurationLoader* loader = new (std::nothrow) sup::core::ConfigurationLoader ("ForLoader@SomePlantSystem");

  bool ret = (static_cast<sup::core::ConfigurationLoader*>(NULL) != loader);

  if (ret)
    {
      ccs::HelperTools::SleepFor(500000000ul);
      ret = loader->IsConnected();
    }

  ccs::types::AnyValue config;

  if (ret)
    {
      ret = loader->ReadConfiguration("config", config);
    }

  if (ret)
    {
      ret = (NULL != config.GetInstance());
    }

  if (ret)
    {
      ret = !loader->LoadConfiguration("", config); // Expect failure
    }

  if (ret)
    {
      ret = !loader->LoadConfiguration("throw", config); // Expect failure
    }

  // ToDo - Fill configuration

  if (ret)
    {
      ret = loader->LoadConfiguration("config", config);
    }

  // ToDo - Test configuration

  if (ret)
    {
      delete loader;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationLoader_Test, RegisterHasher)
{
  sup::core::ConfigurationLoader* loader = new (std::nothrow) sup::core::ConfigurationLoader ("ForLoader@SomePlantSystem");

  bool ret = (static_cast<sup::core::ConfigurationLoader*>(NULL) != loader);

  if (ret)
    {
      ccs::HelperTools::SleepFor(500000000ul);
      ret = loader->IsConnected();
    }

  ccs::types::AnyValue config;

  if (ret)
    {
      ret = loader->ReadConfiguration("config", config);
    }

  if (ret)
    {
      ret = (NULL != config.GetInstance());
    }

  if (ret)
    {
      ret = (loader->RegisterHasher(hasher) && service->RegisterHasher(hasher));
    }

  // ToDo - Fill configuration

  if (ret)
    {
      ret = loader->LoadConfiguration("config", config);
    }

  // ToDo - Test configuration

  if (ret)
    {
      delete loader;
    }

  ASSERT_EQ(true, ret);
}

} // namespace clod

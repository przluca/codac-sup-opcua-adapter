/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/ConfigurationHandler-tests.cpp $
* $Id: ConfigurationHandler-tests.cpp 100236 2019-06-21 11:00:02Z bauvirb $
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

#include <common/CyclicRedundancyCheck.h>
#include <common/Hash.h>

// Local header files

#include "ConfigurationHandler.h"
#include "ConfigurationService.h"

// Constants

// Type definition

namespace chnd {
 
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

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function definition
 
// cppcheck-suppress syntaxError // Obvious false positive for unidentified reason .. name expansion through TEST macro ?
TEST(ConfigurationHandler_Test, Constructor)
{
  sup::core::ConfigurationHandler* handler = dynamic_cast<sup::core::ConfigurationHandler*>(new (std::nothrow) Handler ());

  bool ret = (static_cast<sup::core::ConfigurationHandler*>(NULL) != handler);

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ConfigurationHandler_Test, GetSeed)
{
  sup::core::ConfigurationHandler* handler = new (std::nothrow) Handler ();

  bool ret = (static_cast<sup::core::ConfigurationHandler*>(NULL) != handler);

  ccs::types::uint32 seed = 0u;

  if (ret)
    {
      ret = handler->GetSeed("", seed);
    }

  if (ret)
    {
      ret = (0u != seed);
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

} // namespace chnd
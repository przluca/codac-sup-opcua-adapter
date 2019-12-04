/******************************************************************************
* $HeadURL: https://svnpub.codac.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/trunk/src/test/c++/unit/ConfigurationLoader-tests.cpp $
* $Id: ConfigurationLoader-tests.cpp 99923 2019-05-23 13:05:58Z bauvirb $
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

#include <cadef.h>

#include <gtest/gtest.h> // Google test framework

#include <common/BasicTypes.h> // Misc. type definition
#include <common/SysTools.h> // Misc. helper functions
#include <common/TimeTools.h> // Misc. helper functions

#include <common/log-api.h> // Syslog wrapper routines

#include <common/ObjectDatabase.h> 
#include <common/ObjectFactory.h> 

#include <common/ChannelAccessClient.h>

// Local header files

#include "ConfigurationLoader.h"
#include "ConfigurationService.h"
#include "EPICSv3PlantSystemAdapter.h"

// Constants

// Function declaration

// Global variables

static sup::core::ConfigurationService* service = new (std::nothrow) sup::core::ConfigurationService ();
static sup::core::ConfigurationLoader* loader = new (std::nothrow) sup::core::ConfigurationLoader ("Service@v3Channels");

// Function definition

static inline bool Initialise (void)
{

  ccs::log::SetStdout();

  bool status = ((static_cast<sup::core::ConfigurationService*>(NULL) != service) && 
		 (static_cast<sup::core::ConfigurationLoader*>(NULL) != loader));

  if (status)
    {
      status = ccs::HelperTools::Exist("./unit/EPICSv3PlantSystemAdapter-tests.db");

      if (status)
	{ // Start test IOC
	  // Compose system command
	  std::string command = std::string("/usr/bin/screen -d -m /usr/bin/softIoc -d ./unit/EPICSv3PlantSystemAdapter-tests.db > /tmp/ioc.out");
	  status = (std::system(command.c_str()) == 0);
	}
      else
	{ // Try alternate location
	  // Compose system command
	  std::string command = std::string("/usr/bin/screen -d -m /usr/bin/softIoc -d ./target/test/c++/unit/EPICSv3PlantSystemAdapter-tests.db > /tmp/ioc.out");
	  status = (std::system(command.c_str()) == 0);
	}
    }

  if (status)
    {
      ccs::HelperTools::SleepFor(1000000000ul);
    }

  if (status)
    { // Compose system command
      std::string command = std::string("/usr/bin/caget D1-A0-CFG:S0-ENABLE D1-A0-CFG:S1-ENABLE D1-A0-CFG:S2-ENABLE D1-A0-CFG:S3-ENABLE D1-A0-CFG:S0-ANGLE > /tmp/caget.out");
      status = (std::system(command.c_str()) == 0);
      std::stringstream sstr; sstr << std::ifstream("/tmp/caget.out").rdbuf();
      std::string result = sstr.str();
      log_info("TEST(EPICSv3PlantSystemAdapter, Initialise) - Check variables exist ..");
      std::cout << result << std::endl;
     }

  if (status)
    {
      log_info("TEST(EPICSv3PlantSystemAdapter, Initialise) - ConfigurationService::SetService");
      status = service->SetService("Service@v3Channels");
    }

  if (status)
    {
      ccs::HelperTools::SleepFor(5000000000ul);
    }

  if (status)
    {
      log_info("TEST(EPICSv3PlantSystemAdapter, Initialise) - ConfigurationLoader::IsConnected");
      status = loader->IsConnected();
    }

  if (status)
    {
      log_info("TEST(EPICSv3PlantSystemAdapter, Initialise) - GOFY::IsValid");
      status = ccs::base::GlobalObjectFactory::IsValid("sup::core::EPICSv3PlantSystemAdapter");
    }

  if (status)
    {
      log_info("TEST(EPICSv3PlantSystemAdapter, Initialise) - GOFY::Instanctiate");
      ccs::base::AnyObject* ref = ccs::base::GlobalObjectFactory::Instantiate("sup::core::EPICSv3PlantSystemAdapter");
      status = (static_cast<ccs::base::AnyObject*>(NULL) != ref);

      if (status && ref->IsType())
	{ 
	  ref->SetInstanceName("EPICSv3PlantSystemAdapter");
	}

      if (status)
	{
	  log_info("TEST(EPICSv3PlantSystemAdapter, Initialise) - Register to GODB");
	  status = ccs::base::GlobalObjectDatabase::Register("EPICSv3PlantSystemAdapter", ref);
	}
    }

  if (status)
    {
      status = service->RegisterHandler("EPICSv3PlantSystemAdapter");
    }

  return status;

}

static inline bool Terminate (void)
{

  bool status = true;

  ccs::base::GlobalObjectFactory::Terminate(ccs::base::GlobalObjectDatabase::GetInstance("EPICSv3PlantSystemAdapter"));

  if (status)
    { // Stop test IOC
      // Compose system command
      std::string command = std::string("/usr/bin/kill -9 `/usr/sbin/pidof softIoc`");
      status = (std::system(command.c_str()) == 0);
     }

  return status;

}

TEST(EPICSv3PlantSystemAdapter, Default) // Static initialisation
{
  bool ret = Initialise();

  sup::core::EPICSv3PlantSystemAdapter* handler = dynamic_cast<sup::core::EPICSv3PlantSystemAdapter*>(ccs::base::GlobalObjectDatabase::GetInstance("EPICSv3PlantSystemAdapter"));

  if (ret)
    {
      ret = static_cast<sup::core::EPICSv3PlantSystemAdapter*>(NULL) != handler;
    }

  if (ret)
    {
      ret = handler->SetParameter("verbose", "true");
    }

  if (ret)
    {
      ret = !handler->ProcessMessage("Create('{\"name\":\"\",\"type\":\"55a0::SensorsGeometry/v1.0\"}')"); // Expect failure
    }

  if (ret)
    {
      char buffer [] = 
"{\"type\":\"55a0::SensorGeometry/v1.0\","
" \"attributes\":[{\"enabled\":{\"type\":\"bool\",\"size\":1}},"
                 "{\"geometry\":{\"type\":\"55a0::Geometry\","
                                "\"attributes\":[{\"r\":{\"type\":\"float64\",\"size\":8}},"
                                                "{\"z\":{\"type\":\"float64\",\"size\":8}},"
                                                "{\"angle\":{\"type\":\"float64\",\"size\":8}}"
                                               "]"
                               "}"
                 "}"
                "]"
"}";
      ret = ccs::types::GlobalTypeDatabase::Register(buffer);
    }

  if (ret)
    {
      char buffer [] = 
"{\"type\":\"55a0::SensorsGeometry/v1.0\","
" \"multiplicity\":4,"
" \"element\":{\"type\":\"55a0::SensorGeometry/v1.0\"}"
"}";
      ret = ccs::types::GlobalTypeDatabase::Register(buffer);
    }

  if (ret)
    {
      ret = handler->ProcessMessage("Create('{\"name\":\"\",\"type\":\"55a0::SensorsGeometry/v1.0\"}')");
    }

  if (ret)
    {
      ret = (handler->ProcessMessage("Associate('{\"name\":\"[0].enabled\",\"channel\":\"D1-A0-CFG:S0-ENABLE\"}')") &&
	     handler->ProcessMessage("Associate('{\"name\":\"[1].enabled\",\"channel\":\"D1-A0-CFG:S1-ENABLE\"}')") &&
	     handler->ProcessMessage("Associate('{\"name\":\"[2].enabled\",\"channel\":\"D1-A0-CFG:S2-ENABLE\"}')") &&
	     handler->ProcessMessage("Associate('{\"name\":\"[3].enabled\",\"channel\":\"D1-A0-CFG:S3-ENABLE\"}')"));
    }

  if (ret)
    {
      ret = handler->ProcessMessage("Associate('{\"name\":\"[0].geometry.angle\",\"channel\":\"D1-A0-CFG:S0-ANGLE\"}')");
    }

  if (ret)
    {
      ret = handler->ProcessMessage("Start()");
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (ret)
    { // Variables are created in the cache
      ret = (ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsValid("D1-A0-CFG:S0-ENABLE") &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsValid("D1-A0-CFG:S1-ENABLE") &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsValid("D1-A0-CFG:S2-ENABLE") &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsValid("D1-A0-CFG:S3-ENABLE"));
    }
#if 0 // Missing API
  if (ret)
    { // Variables are created in the cache
      ret = (ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsConnected("D1-A0-CFG:S0-ENABLE") &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsConnected("D1-A0-CFG:S1-ENABLE") &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsConnected("D1-A0-CFG:S2-ENABLE") &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->IsConnected("D1-A0-CFG:S3-ENABLE"));
    }
#endif
  if (ret)
    {
      ccs::types::boolean variable = false;
      ret = (ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S0-ENABLE", variable) &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S1-ENABLE", variable) &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S2-ENABLE", variable) &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S3-ENABLE", variable));      
    }
  
  if (ret)
    {
      ret = loader->IsConnected();
    }

  if (ret)
    {
      log_info("TEST(EPICSv3PlantSystemAdapter, Default) - Read configuration ..");
      ccs::types::AnyValue config;
      ret = loader->ReadConfiguration("", config);

      if (ret)
	{
	  char buffer [1024] = STRING_UNDEFINED; 
	  config.SerialiseType(buffer, 1024u);
	  log_info("TEST(EPICSv3PlantSystemAdapter, Default) - .. type '%s' ..", buffer);
	}

      if (ret)
	{
	  char buffer [1024] = STRING_UNDEFINED; 
	  config.SerialiseInstance(buffer, 1024u);
	  log_info("TEST(EPICSv3PlantSystemAdapter, Default) - .. and value '%s'", buffer);
	}
    }

  // Change EPICSv3 record

  if (ret)
    {
      ccs::types::boolean variable = true;
      ret = (ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S0-ENABLE", variable) &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S1-ENABLE", variable) &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S2-ENABLE", variable) &&
	     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->SetVariable("D1-A0-CFG:S3-ENABLE", variable));      
    }
  
  if (ret)
    {
      ccs::types::AnyValue config;
      ret = loader->ReadConfiguration("", config);

      if (ret)
	{
	  char buffer [1024] = STRING_UNDEFINED; 
	  config.SerialiseInstance(buffer, 1024u);
	  log_info("TEST(EPICSv3PlantSystemAdapter, Default) - .. and value '%s'", buffer);
	}

      if (ret)
	{
	  ccs::types::boolean variable = false;
	  ret = (ccs::HelperTools::GetAttributeValue(&config, "[0].enabled", variable) &&
		 (true == variable)); // Etc.
	}
    }

  // Load configuration

  if (ret)
    {
      ccs::types::AnyValue config (ccs::types::GlobalTypeDatabase::GetType("55a0::SensorsGeometry/v1.0")); // Zero'd buffer
      ret = loader->LoadConfiguration("", config);

      if (ret)
	{
	  ccs::types::boolean bvar = true;
	  ccs::types::float64 fvar = 0.1;
	  ret = ((ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable("D1-A0-CFG:S0-ENABLE", bvar) &&
		 (false == bvar)) &&
		 (ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable("D1-A0-CFG:S0-ANGLE", fvar) &&
		  (0.0 == fvar))); // Etc.
	}
    }

  if (ret)
    {
      ccs::types::boolean bvar = true;
      ccs::types::float64 fvar = 2.5;

      log_info("TEST(EPICSv3PlantSystemAdapter, Default) - Load configuration '%s' and '%f' ..", (bvar ? "true" : "false"), fvar);

      ccs::types::AnyValue config (ccs::types::GlobalTypeDatabase::GetType("55a0::SensorsGeometry/v1.0")); // Zero'd buffer
      ccs::HelperTools::SetAttributeValue(&config, "[0].enabled", bvar);
      ccs::HelperTools::SetAttributeValue(&config, "[0].geometry.angle", fvar);

      ret = loader->LoadConfiguration("", config);

      if (ret)
	{
	  bvar = false;
	  fvar = 0.0;
	  ret = ((ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable("D1-A0-CFG:S0-ENABLE", bvar) &&
		 (true == bvar)) &&
		 (ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable("D1-A0-CFG:S0-ANGLE", fvar) &&
		  (2.5 == fvar))); // Etc.
	}

      log_info("TEST(EPICSv3PlantSystemAdapter, Default) - .. read on IOC '%s' and '%f' ..", (bvar ? "true" : "false"), fvar);
    }

  if (ret)
    {
      ccs::types::boolean bvar = true;
      ccs::types::float64 fvar = 7.5;

      log_info("TEST(EPICSv3PlantSystemAdapter, Default) - Load configuration '%s' and '%f' ..", (bvar ? "true" : "false"), fvar);

      ccs::types::AnyValue config (ccs::types::GlobalTypeDatabase::GetType("55a0::SensorsGeometry/v1.0")); // Zero'd buffer
      ccs::HelperTools::SetAttributeValue(&config, "[0].enabled", bvar);
      ccs::HelperTools::SetAttributeValue(&config, "[0].geometry.angle", fvar);

      ret = !loader->LoadConfiguration("", config); // Expect failure .. truncated angle .. see .db file

      if (ret) // Check variable indeed not written !!
	{
	  bvar = false;
	  fvar = 0.0;
	  ret = ((ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable("D1-A0-CFG:S0-ENABLE", bvar) &&
		 (true == bvar)) &&
		 (ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable("D1-A0-CFG:S0-ANGLE", fvar) &&
		  (2.5 == fvar))); // Etc.
	}

      log_info("TEST(EPICSv3PlantSystemAdapter, Default) - .. read on IOC '%s' and '%f' ..", (bvar ? "true" : "false"), fvar);
    }

  // Read configuration

  if (ret)
    {
      ccs::types::AnyValue config;
      ret = loader->ReadConfiguration("", config);

      if (ret)
	{
	  char buffer [1024] = STRING_UNDEFINED; 
	  config.SerialiseInstance(buffer, 1024u);
	  log_info("TEST(EPICSv3PlantSystemAdapter, Default) - .. and value '%s'", buffer);
	}

      if (ret)
	{
	  ccs::types::boolean bvar = false;
	  ccs::types::float64 fvar = 0.0;
	  ret = ((ccs::HelperTools::GetAttributeValue(&config, "[0].enabled", bvar) &&
		 (true == bvar)) &&
		 (ccs::HelperTools::GetAttributeValue(&config, "[0].geometry.angle", fvar) &&
		  (2.5 == fvar))); // Etc.
	}
    }

  // Load configuration error .. wrong type

  if (ret)
    {
      ccs::types::AnyValue config (ccs::types::Boolean);
      ret = !loader->LoadConfiguration("", config); // Expect failure
    }

  Terminate();

  ASSERT_EQ(true, ret);
}


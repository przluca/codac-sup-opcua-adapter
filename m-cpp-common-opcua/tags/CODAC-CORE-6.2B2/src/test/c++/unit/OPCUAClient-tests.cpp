/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common-opcua/tags/CODAC-CORE-6.2B2/src/test/c++/unit/OPCUAClient-tests.cpp $
* $Id: OPCUAClient-tests.cpp 101452 2019-08-08 10:38:23Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Unit test code
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*                                 CS 90 046
*                                 13067 St. Paul-lez-Durance Cedex
*                                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <new> // std::nothrow

#include <gtest/gtest.h> // Google test framework

#include <common/BasicTypes.h> // Misc. type definition
#include <common/SysTools.h> // Misc. helper functions
#include <common/TimeTools.h> // Misc. helper functions

#include <common/log-api.h> // Syslog wrapper routines

// Local header files

#include "OPCUAClient.h"

// Constants

// Type definition

// Function declaration

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();
static ccs::base::OPCUAClient* __ua_client = static_cast<ccs::base::OPCUAClient*>(NULL);

static ccs::types::uint32 __cb_count = 0u;

// Function definition

static inline bool Initialise (void)
{

  __ua_client = new (std::nothrow) ccs::base::OPCUAClient ("opc.tcp://localhost:4840");

  bool status = (static_cast<ccs::base::OPCUAClient*>(NULL) != __ua_client);
#if 0 // Included in 'mvn test'
  if (status)
    { // Start test server
      // Compose system command
      std::string command = std::string("/usr/bin/screen -d -m ${MARTe2_DIR}/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f ./unit/OPCUAClient-tests.marte -s Running > /tmp/srv.out");
      status = (std::system(command.c_str()) == 0);
     }
#endif
  if (status)
    {
      ccs::HelperTools::SleepFor(1000000000ul);
    }

  return status;

}

static inline bool Terminate (void)
{

  bool status = (static_cast<ccs::base::OPCUAClient*>(NULL) != __ua_client);

  if (status)
    {
      delete __ua_client;
      __ua_client = static_cast<ccs::base::OPCUAClient*>(NULL);
    }
#if 0 // Included in 'mvn test'
  if (status)
    { // Stop test server
      // Compose system command
      std::string command = std::string("/usr/bin/kill -9 `/usr/sbin/pidof MARTeApp.ex`");
      status = (std::system(command.c_str()) == 0);
     }
#endif
  return status;

}

void HandleUpdate (const ccs::types::char8 * const name, const ccs::types::AnyValue& value)
{

  log_info("HandleUpdate - OPC UA variable '%s' updated ..", name);

  // Handle update ..
  ccs::HelperTools::LogSerialisedType(&value);
  ccs::HelperTools::LogSerialisedInstance(&value);

  __cb_count++;

  return;

}

TEST(OPCUAClient, Default) // Static initialisation
{
  bool ret = Initialise();

  if (ret)
    { // Current time
      ret = __ua_client->AddVariable("ns=1;i=3000", ccs::types::AnyputVariable, ccs::types::UnsignedInteger64);
    }

  if (ret)
    {
      ret = __ua_client->AddVariable("ns=1;i=3002", ccs::types::AnyputVariable, ccs::types::Boolean);
    }

  if (ret)
    {
      ret = __ua_client->AddVariable("ns=1;i=3003", ccs::types::AnyputVariable, ccs::types::SignedInteger8);
    }

  if (ret)
    {
      ret = __ua_client->AddVariable("ns=1;i=3004", ccs::types::AnyputVariable, ccs::types::UnsignedInteger8);
    }

  if (ret)
    {
      ret = __ua_client->Launch();
    }

  ccs::HelperTools::SleepFor(1000000000ul);

  if (ret)
    {
      ret = __ua_client->IsValid("ns=1;i=3000");
    }

  if (ret)
    {
      ccs::types::uint64 variable = 0ul;
      ret = __ua_client->GetVariable("ns=1;i=3000", variable);

      if (ret)
        {
          log_info("TEST(OPCUAClient, Default) - Get variable '%lu'", variable);
        }
    }

  ccs::HelperTools::SleepFor(1000000000ul);

  if (ret)
    {
      ccs::types::uint64 variable = 0ul;
      ret = __ua_client->GetVariable("ns=1;i=3000", variable);

      if (ret)
        {
          log_info("TEST(OPCUAClient, Default) - Get variable '%lu'", variable);
        }
    }

  // Install callback
  if (ret)
    {
      __cb_count = 0u;
      //ret = __ua_client->SetCallback("ns=1;i=3004", std::bind(&HandleUpdate, std::placeholders::_1, std::placeholders::_2));
      ret = __ua_client->SetCallback("ns=1;i=3004", HandleUpdate);
    }

  // Test if callback is being invoked upon change of variable on the server
  if (ret)
    {
      ccs::types::uint8 variable = 10;
      ret = __ua_client->SetVariable("ns=1;i=3004", variable);
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (ret)
    {
      ret = (1u == __cb_count);
    }

  if (ret)
    {
      ccs::types::uint8 variable = 0;
      ret = __ua_client->GetVariable("ns=1;i=3004", variable);

      if (ret)
        {
          log_info("TEST(OPCUAClient, Default) - Get variable '%hhu'", variable);
        }
    }

  if (ret)
    {
      ret = Terminate();
    }

  ASSERT_EQ(true, ret);
}


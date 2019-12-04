/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/L0-component/L0-component.cpp $
* $Id: L0-component.cpp 96477 2019-01-09 14:53:55Z sivecm $
*
* Project	: CODAC Core System
*
* Description	: SDN Software - Communication API - Simple examples
*
* Author        : Bertrand Bauvir
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

#include <stdio.h> // sscanf, printf, etc.
#include <string.h> // strncpy, etc.
#include <stdarg.h> // va_start, etc.
#include <signal.h> // sigset, etc.

#include <new> // std::nothrow

// Local header files

#include "types.h" // Misc. type definition, e.g. RET_STATUS
#include "tools.h" // Misc. helper functions, e.g. hash, etc.

#include "log-api.h" // Logging helper functions
#include "CyclicRedundancyCheck.h"

#include "AnyTypeDatabase.h"

#include "PVAccessRPCServer.h"
#include "PVAccessRPCTypes.h"

// Constants

#define DEFAULT_AFFINITY       0
#define DEFAULT_ITERATIONS     1
#define DEFAULT_PAYLOAD     1024 // 1kB
#define DEFAULT_PERIOD 1000000000ul

// Type definition

typedef struct __attribute__((packed)) Config {
  ccs::types::string ipaddr = "";
  ccs::types::float64 setpoint;
} Config_t;

class RPCServiceImpl : public ccs::base::PVAccessRPCServer
{

  private:

    std::shared_ptr<ccs::types::CompoundType> __config_struct;

    Config_t __config;
 
  public:

    RPCServiceImpl (const char* service);
    virtual ~RPCServiceImpl (void) {};

    virtual ccs::types::AnyValue HandleRequest (const ccs::types::AnyValue& request); // Overloaded virtual method

};

// Global variables

bool _terminate = false;

// Function definition

RPCServiceImpl::RPCServiceImpl (const char* service) : ccs::base::PVAccessRPCServer (service)
{

  // Initialise attributes
  __config_struct = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("app::Config_t/v1.0"))
							      ->AddAttribute<ccs::types::string>("ipaddr")
							      ->AddAttribute<ccs::types::float64>("setpoint"));

  ccs::HelperTools::SafeStringCopy(__config.ipaddr, STRING_UNDEFINED, ccs::types::MaxStringLength); 
  __config.setpoint = 0.0; 

  return;

}

ccs::types::AnyValue RPCServiceImpl::HandleRequest (const ccs::types::AnyValue& request)
{

  const ccs::types::AnyValue* __query_value = &request;

  // WARNING - eget command line tool embeds the attributes in a 'query' structure
  if (ccs::HelperTools::HasAttribute(&request, "query"))
    {
      __query_value = new (std::nothrow) ccs::types::AnyValue (ccs::HelperTools::GetAttributeType(&request, "query"), 
							       ccs::HelperTools::GetAttributeReference(&request, "query"));
    }

  std::string qualifier;

  bool status = (ccs::HelperTools::HasAttribute(&request, "qualifier") &&
		 (ccs::types::String == ccs::HelperTools::GetAttributeType(&request, "qualifier")));

  if (status)
    {
      qualifier = std::string(static_cast<char*>(ccs::HelperTools::GetAttributeReference(&request, "qualifier")));
    }

  if (qualifier == "read")
    {
      // Reply contains the expected structure
      log_info("RPCServiceImpl::HandleRequest('%s') - Build configuration structure into the reply ..", qualifier.c_str());
 
      status = (ccs::base::PVAccessRPC::Reply_int ? true : false);

      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      // .. and add the missing bit
      reply_type.AddAttribute("value", __config_struct);
 
      ccs::base::PVAccessRPC::ReplyWithValue<Config_t> reply;

      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.value = __config;

      ccs::types::AnyValue reply_value (reply_type);
      reply_value = reply;

      char buffer [1024]; reply_value.SerialiseInstance(buffer, 1024u);
      log_info("RPCServiceImpl::HandleRequest('%s') - .. '%s' reply", qualifier.c_str(), buffer);
 
      return reply_value;
    }

  if (qualifier == "init")
    {
      // Reply contains the seed
      log_info("RPCServiceImpl::HandleRequest('%s') - Build seed into the reply ..", qualifier.c_str());

      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      // .. and add the missing bit
      reply_type.AddAttribute<ccs::types::uint32>("value");

      ccs::base::PVAccessRPC::ReplyWithValue<ccs::types::uint32> reply;

      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.value = ccs::HelperTools::CyclicRedundancyCheck<uint32_t>((uint8_t*)&reply.timestamp, 8u);;

      ccs::types::AnyValue reply_value (reply_type);
      reply_value = reply;

      return reply_value;
    }

  if (qualifier == "load")
    {
      ccs::types::uint32 seed = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&request, "seed");
      ccs::types::uint32 hash = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&request, "hash");

      ccs::types::uint32 size = 
	ccs::HelperTools::GetAttributeType(&request, "seed")->GetSize() +
	ccs::HelperTools::GetAttributeType(&request, "value")->GetSize();

      // Extract configuration data
      __config = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&request, "value");

      char buffer [1024] = STRING_UNDEFINED;
      ccs::HelperTools::GetAttributeType(&request, "value")->SerialiseInstance(ccs::HelperTools::GetAttributeReference(&request, "value"), buffer, 1024u);

      log_info("Received '%s' configuration", buffer);

      ccs::types::uint32 check = 
	ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>((ccs::types::uint8*) ccs::HelperTools::GetAttributeReference(&request, "seed"), size);

      ccs::base::PVAccessRPC::Reply_t reply;

      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = (hash == check);

      if (reply.status)
	{
	  snprintf(reply.reason, STRING_MAX_LENGTH, "Correct hash '%u' for seed '%u'", hash, seed);
	}
      else
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "error", STRING_MAX_LENGTH);
	  snprintf(reply.reason, STRING_MAX_LENGTH, "Wrong hash '%u %u' for seed '%u'", hash, check, seed);
	}

      ccs::types::AnyValue reply_value (ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      reply_value = reply;

      return reply_value;
    }

  // Default reply
  ccs::base::PVAccessRPC::Reply_t reply;

  reply.timestamp = ccs::HelperTools::GetCurrentTime();
  reply.status = false;
  ccs::HelperTools::SafeStringCopy(reply.qualifier, "error", STRING_MAX_LENGTH);
  snprintf(reply.reason, STRING_MAX_LENGTH, "Unknown qualifier '%s'", qualifier.c_str());

  ccs::types::AnyValue reply_error (ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
  reply_error = reply;

  if (__query_value != &request)
    {
      delete __query_value;
    }

  return reply_error;

}

void print_usage (void)
{

  char prog_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  get_program_name((char*) prog_name);

  fprintf(stdout, "Usage: %s <options>\n", prog_name);
  fprintf(stdout, "Options: -h|--help: Print usage.\n");
  fprintf(stdout, "         -c|--count <sample_nb>: Stop after <sample_nb> are published, -1 for undefined number of counts (stops with Ctrl-C), defaults to 10000.\n");
  fprintf(stdout, "         -p|--period <period_ns>: Publication period, defaults to 1000000 (1kHz).\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "The program instantiates a MCAST publisher streams <payload> bytes packets on <iface_name> with configurable rate, etc.\n");
  fprintf(stdout, "\n");

  return;

};

void signal_handler (int signal)
{

  log_info("Received signal '%d' to terminate", signal);
  _terminate = true;

};

int main (int argc, char** argv) 
{

  // Install signal handler to support graceful termination
  sigset(SIGTERM, signal_handler);
  sigset(SIGINT,  signal_handler);
  sigset(SIGHUP,  signal_handler);

  uint_t count = DEFAULT_ITERATIONS;
  uint64_t period = DEFAULT_PERIOD;

  char service [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(service, "rpc@L0-Component", STRING_MAX_LENGTH);

  if (argc > 1)
    {
      for (uint_t index = 1; index < (uint_t) argc; index++)
	{
          if ((strcmp(argv[index], "-h") == 0) || (strcmp(argv[index], "--help") == 0))
	    {
	      // Display usage
	      print_usage();
	      return (0);
	    }
	  else if ((strcmp(argv[index], "-c") == 0) || (strcmp(argv[index], "--count") == 0))
	    {
	      // Get record count
	      if ((index + 1) < (uint_t) argc) sscanf(argv[index + 1], "%u", &count);
	      else { print_usage(); return (0); } // Display usage
	      index += 1;
            
	    }
	  else if ((strcmp(argv[index], "-p") == 0) || (strcmp(argv[index], "--period") == 0))
	    {
	      // Get publication period
	      if ((index + 1) < (uint_t) argc) sscanf(argv[index + 1], "%lu", &period);
	      else { print_usage(); return (0); } // Display usage
	      index += 1;
            
	    }
	  else if ((strcmp(argv[index], "-s") == 0) || (strcmp(argv[index], "--service") == 0))
	    {
	      // Get publication period
	      if ((index + 1) < (uint_t) argc) ccs::HelperTools::SafeStringCopy(service, argv[index + 1], STRING_MAX_LENGTH);
	      else { print_usage(); return (0); } // Display usage
	      index += 1;
            
	    }
	  else if ((strcmp(argv[index], "-v") == 0) || (strcmp(argv[index], "--verbose") == 0))
	    {
	      // Log to stdout
	      ccs::log::Func_t old_plugin = ccs::log::SetStdout();
	      ccs::log::Severity_t old_filter = ccs::log::SetFilter(LOG_INFO);

	    }
	}
    }
  else
    {
    }

  // Create RPC server
  log_info("Create RPC server");
  RPCServiceImpl server (service);

  uint64_t curr_time = get_time();
  uint64_t till_time = ceil_time(curr_time);

  while (_terminate != true)
    {

      curr_time = wait_until(till_time);
      till_time += period;

    }

  // Terminate
  log_info("Terminate program");

  return (0);

}

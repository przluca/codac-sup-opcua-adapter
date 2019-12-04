/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/L3-component/L3-component.cpp $
* $Id: L3-component.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include <iostream> // std::cout
#include <list> // std::list
#include <map> // std::map
#include <new> // std::nothrow
#include <utility> // std::pair
#include <vector> // std::vector

#include <csignal> // signal

// Local header files

#include "types.h" // Misc. type definition, e.g. RET_STATUS
#include "tools.h" // Misc. helper functions, e.g. hash, etc.

#include "log-api.h" // Logging helper functions

#include "PVAccessRPCClient.h"
#include "PVAccessRPCServer.h"
#include "PVAccessRPCTypes.h"

#include "ChannelAccessClient.h"

// Constants

#define DEFAULT_AFFINITY       0
#define DEFAULT_ITERATIONS     1
#define DEFAULT_PAYLOAD     1024 // 1kB
#define DEFAULT_PERIOD 1000000000ul

// Type definition

class RPCServiceImpl : public ccs::base::PVAccessRPCServer
{

  private:

    std::shared_ptr<const ccs::types::CompoundType> __config_struct; // Retrieved from L0 component
    ccs::types::AnyValue* __config_cache;

    ccs::base::PVAccessRPCClient* __rpc_clnt;
    ccs::base::ChannelAccessClient* __ca_clnt;

  public:

    RPCServiceImpl (const char* service);
    virtual ~RPCServiceImpl (void) {};

    bool Connect (const char* service);

    virtual ccs::types::AnyValue HandleRequest (const ccs::types::AnyValue& request); // Overloaded virtual method

};

// Global variables

bool _terminate = false;

std::vector<std::tuple<std::string,std::string, const std::shared_ptr<const ccs::types::ScalarType>>> __assoc;

// Function definition

RPCServiceImpl::RPCServiceImpl (const char* service) : ccs::base::PVAccessRPCServer (service)
{

  // Initialise attributes
  __rpc_clnt = static_cast<ccs::base::PVAccessRPCClient*>(NULL);
  __config_cache = static_cast<ccs::types::AnyValue*>(NULL);

  // Create CA client
  __ca_clnt = ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>();

  bool status = (static_cast<ccs::base::ChannelAccessClient*>(NULL) != __ca_clnt);

  // Configure CA client
  for (ccs::types::uint32 index = 0u; (status && (index < __assoc.size())); index += 1u)
    {
      using namespace std::placeholders;
      status = __ca_clnt->AddVariable(std::get<1>(__assoc[index]).c_str(), ccs::types::OutputVariable, std::get<2>(__assoc[index]));
    }

  // Start CA client
  __ca_clnt->Launch();

  return;

}

bool RPCServiceImpl::Connect (const char* service)
{

  bool status = (static_cast<ccs::base::PVAccessRPCClient*>(NULL) == __rpc_clnt);

  if (status)
    {
      // Create RPC client
      __rpc_clnt = new (std::nothrow) ccs::base::PVAccessRPCClient (service);
      status = (static_cast<ccs::base::PVAccessRPCClient*>(NULL) != __rpc_clnt);
    }
  
  if (status)
    {
      // Launch RPC client
      status = __rpc_clnt->Launch();
    }

  return status;
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

  bool status = (ccs::HelperTools::HasAttribute(__query_value, "qualifier") &&
		 (ccs::types::String == ccs::HelperTools::GetAttributeType(__query_value, "qualifier")));

  if (status)
    {
      qualifier = std::string(static_cast<char*>(ccs::HelperTools::GetAttributeReference(__query_value, "qualifier")));
    }

  // After instantiation

  if (qualifier == "connect") // Connect to named L1 component
    {
      std::string l1_service;

      status = (ccs::HelperTools::HasAttribute(__query_value, "value") &&
		(ccs::types::String == ccs::HelperTools::GetAttributeType(__query_value, "value")));

      if (status)
	{
	  status = this->Connect(l1_service.c_str());
	}

      ccs::base::PVAccessRPC::Reply_t reply;

      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = status;

      if (status)
	{
	  snprintf(reply.reason, STRING_MAX_LENGTH, "Connected to '%s' RPC service", l1_service.c_str());
	}
      else
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "error", STRING_MAX_LENGTH);
	  snprintf(reply.reason, STRING_MAX_LENGTH, "Error with client to '%s' RPC service", l1_service.c_str());
	}

      ccs::types::AnyValue reply_value (ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      reply_value = reply;

      return reply_value;
    }

  if (qualifier == "read")
    {
      log_info("RPCServiceImpl::HandleRequest('%s') - Query from '%s' RPC service  ..", qualifier.c_str(), __rpc_clnt->GetService());
      ccs::types::AnyValue reply_value = __rpc_clnt->SendRequest(*__query_value);

      ccs::types::boolean reply_status = false;
      status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply_value, "status", reply_status);

      if (reply_status)
	{
	  log_info("RPCServiceImpl::HandleRequest('%s') - .. extract configuration datatype from '%s' reply  ..", qualifier.c_str(), __rpc_clnt->GetService());
	  __config_struct = std::dynamic_pointer_cast<const ccs::types::CompoundType>(ccs::HelperTools::GetAttributeType(&reply_value, "value"));
	  status = (__config_struct ? true : false);
	}
      else
	status = reply_status;
  
      if (status)
	{
	  if (static_cast<ccs::types::AnyValue*>(NULL) != __config_cache)
	    {
	      delete __config_cache;
	    }

	  __config_cache = new (std::nothrow) ccs::types::AnyValue (__config_struct);
	  status = (static_cast<ccs::types::AnyValue*>(NULL) != __config_cache);
	}

      return reply_value;
    }

  if (qualifier == "init")
    {
      log_info("RPCServiceImpl::HandleRequest('%s') - Query from '%s' RPC service  ..", qualifier.c_str(), __rpc_clnt->GetService());
      return __rpc_clnt->SendRequest(*__query_value);
    }

  if (qualifier == "load")
    {
      // Seed and hash from the RPC message .. config written to CA client
      ccs::types::uint32 seed = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(__query_value, "seed");
      ccs::types::uint32 hash = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(__query_value, "hash");

      // Extract configuration data
      memcpy(__config_cache->GetInstance(),
	     ccs::HelperTools::GetAttributeReference(__query_value, "value"),
	     __config_cache->GetSize());

      for (ccs::types::uint32 index = 0u; (status && (index < __assoc.size())); index += 1u)
	{
	  status = ccs::HelperTools::HasAttribute(__config_cache, std::get<0>(__assoc[index]).c_str());

	  if (status)
	    {
	      // Update cache
	      log_info("RPCServiceImpl::HandleRequest('%s') - Update from cache '%s' attribute", qualifier.c_str(), std::get<0>(__assoc[index]).c_str());
	      memcpy(ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable(std::get<1>(__assoc[index]).c_str())->GetInstance(), 
		     ccs::HelperTools::GetAttributeReference(__config_cache, std::get<0>(__assoc[index]).c_str()), 
		     ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable(std::get<1>(__assoc[index]).c_str())->GetType()->GetSize());
	      
	      status = ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->UpdateVariable(std::get<1>(__assoc[index]).c_str());
	    }
	  else
	    {
	      log_error("RPCServiceImpl::HandleRequest('%s') - No '%s' attribute in cache", qualifier.c_str(), std::get<0>(__assoc[index]).c_str());
	    }
	}

      // Make sure L2 component updates
      ccs::HelperTools::SleepFor(1000000000ul);

      // Copy the base request type ..
      ccs::types::CompoundType request_type (*ccs::base::PVAccessRPC::Request_int); // Default RPC request type
      // .. and add the missing bit
      request_type.AddAttribute<ccs::types::uint32>("seed");
      request_type.AddAttribute<ccs::types::uint32>("hash");

      ccs::types::AnyValue l1_request (request_type);

      ccs::HelperTools::SetAttributeValue(&l1_request, "qualifier", "load");
      ccs::HelperTools::SetAttributeValue(&l1_request, "seed", seed);
      ccs::HelperTools::SetAttributeValue(&l1_request, "hash", hash);

      log_info("RPCServiceImpl::HandleRequest('%s') - Query from '%s' RPC service  ..", qualifier.c_str(), __rpc_clnt->GetService());
      return __rpc_clnt->SendRequest(l1_request);
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

  char service [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(service, "rpc@L3-Component", STRING_MAX_LENGTH);

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
	      ccs::log::Severity_t old_filter = ccs::log::SetFilter(LOG_DEBUG);

	    }
	}
    }
  else
    {
    }

  // Initialise association .. to be retrieved from configuration
  __assoc.push_back(std::make_tuple("ipaddr", "ChannelAccessClient-test:scalars-string", ccs::types::String));
  __assoc.push_back(std::make_tuple("setpoint", "ChannelAccessClient-test:scalars-float", ccs::types::Float64));


  // Create RPC server
  log_info("Create RPC server");
  RPCServiceImpl server (service); server.Connect("rpc@L1-Component");

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

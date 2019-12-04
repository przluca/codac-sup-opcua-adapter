/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/3to4-gateway/v3to4-gateway.cpp $
* $Id: v3to4-gateway.cpp 96477 2019-01-09 14:53:55Z sivecm $
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
#include "CyclicRedundancyCheck.h"

#include "PVAccessRPCServer.h"
#include "PVAccessServer.h"
#include "ChannelAccessClient.h"

// Constants

#define DEFAULT_AFFINITY       0
#define DEFAULT_ITERATIONS     1
#define DEFAULT_PAYLOAD     1024 // 1kB
#define DEFAULT_PERIOD 1000000000ul

// Type definition

typedef struct __attribute__((packed)) Reply {
  ccs::types::uint64 timestamp;
  ccs::types::string qualifier = "reply";
  ccs::types::boolean status = true;
  ccs::types::string reason = "";
} Reply_t;

typedef struct __attribute__((packed)) Config {
  ccs::types::string ipaddr = "";
  ccs::types::float64 setpoint = 0.0;
} Config_t;

class RPCServiceImpl : public ccs::base::PVAccessRPCServer
{

  private:

    std::shared_ptr<ccs::types::CompoundType> __reply_struct;
    std::shared_ptr<ccs::types::CompoundType> __config_struct;
    ccs::base::PVAccessServer* __pva_srv;
    ccs::base::ChannelAccessClient* __ca_clnt;

    Config_t __config;
 
  public:

    RPCServiceImpl (const char* service);
    virtual ~RPCServiceImpl (void) {};

    virtual ccs::types::AnyValue HandleRequest (const ccs::types::AnyValue& request); // Overloaded virtual method
    void HandleCAMonitor (const char* name, const ccs::types::AnyValue& request);

};

// Global variables

bool _terminate = false;
#if 0
std::map<std::string,std::string> assoc {
#else
std::vector<std::pair<std::string,std::string>> assoc {
#endif
  {"value.ipaddr", "ChannelAccessClient-test:scalars-string"}, {"value.setpoint", "ChannelAccessClient-test:scalars-float"}

};

// Function definition

RPCServiceImpl::RPCServiceImpl (const char* service) : ccs::base::PVAccessRPCServer (service)
{

  __reply_struct = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("ccs::RPCReply_t/v1.0"))
							     ->AddAttribute<ccs::types::uint64>("timestamp")
							     ->AddAttribute<ccs::types::string>("qualifier")
							     ->AddAttribute<ccs::types::boolean>("status")
							      ->AddAttribute<ccs::types::string>("reason"));

  __config_struct = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("app::Config_t/v1.0"))
							      ->AddAttribute<ccs::types::string>("ipaddr")
							       ->AddAttribute<ccs::types::float64>("setpoint"));

  // Extend configuration data type
  ccs::types::CompoundType* record_type = (new (std::nothrow) ccs::types::CompoundType ("app::Record_t/v1.0"))
                                            ->AddAttribute("value", __config_struct);
  // Start PVA server
  __pva_srv = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>();
  __pva_srv->AddVariable("TEST-TPL-CFG:STRUCT", ccs::types::OutputVariable, record_type);
  __pva_srv->Launch();

  // Start CA client
  __ca_clnt = ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>();
  __ca_clnt->AddVariable(assoc[0u].second.c_str(), ccs::types::AnyputVariable, __config_struct->GetAttributeType("ipaddr"));
  __ca_clnt->AddVariable(assoc[1u].second.c_str(), ccs::types::AnyputVariable, __config_struct->GetAttributeType("setpoint"));
  __ca_clnt->Launch();

  // Default configuration
  *(__pva_srv->GetVariable("TEST-TPL-CFG:STRUCT")) = __config; __pva_srv->UpdateVariable("TEST-TPL-CFG:STRUCT");

  *(__ca_clnt->GetVariable(assoc[0u].second.c_str())) = __config.ipaddr; __ca_clnt->UpdateVariable("TEST-TPL-CFG:IPADDR");
  *(__ca_clnt->GetVariable(assoc[1u].second.c_str())) = __config.setpoint; __ca_clnt->UpdateVariable("TEST-TPL-CFG:SETPOINT");

  // Install CA monitor callback
  using namespace std::placeholders;
  __ca_clnt->SetCallback(assoc[0u].second.c_str(), std::bind(&RPCServiceImpl::HandleCAMonitor, this, _1, _2));
  __ca_clnt->SetCallback(assoc[1u].second.c_str(), std::bind(&RPCServiceImpl::HandleCAMonitor, this, _1, _2));

  return;

}

ccs::types::AnyValue RPCServiceImpl::HandleRequest (const ccs::types::AnyValue& request)
{

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
      log_info("Build configuration structure into the reply ..");
      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*__reply_struct);
      // .. and add the missing bit
      reply_type.AddAttribute("value", __config_struct);

      struct __attribute__((packed)) ReadReply : public Reply {
	Config_t value;
      } reply;

      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.value = __config;

      ccs::types::AnyValue reply_value (reply_type);
      reply_value = reply;

      return reply_value;
    }

  if (qualifier == "init")
    {
      // Reply contains the seed
      log_info("Build seed into the reply ..");
      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*__reply_struct);
      // .. and add the missing bit
      reply_type.AddAttribute<ccs::types::uint32>("value");

      struct __attribute__((packed)) InitReply : public Reply {
	ccs::types::uint32 value;
      } reply;

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

      char buffer [1024] = STRING_UNDEFINED;
      ccs::HelperTools::GetAttributeType(&request, "value")->SerialiseInstance(ccs::HelperTools::GetAttributeReference(&request, "value"), buffer, 1024u);

      log_info("Received '%s' configuration", buffer);

      ccs::types::uint32 check = 
	ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>((ccs::types::uint8*) ccs::HelperTools::GetAttributeReference(&request, "seed"), size);

      log_info("Computed hash '%u'", check);

      status = (hash == check);

      log_info("Compare to '%u (%s)'", hash, (status ? "true" : "false"));

      if (status)
	{
	  // Extract configuration data
	  log_info("Update variable cache ..");
	  __config = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&request, "value");
	  // Update PVA record
	  *(__pva_srv->GetVariable("TEST-TPL-CFG:STRUCT")) = __config; __pva_srv->UpdateVariable("TEST-TPL-CFG:STRUCT");
	  *(__ca_clnt->GetVariable("TEST-TPL-CFG:IPADDR")) = __config.ipaddr; __ca_clnt->UpdateVariable("TEST-TPL-CFG:IPADDR");
	  *(__ca_clnt->GetVariable("TEST-TPL-CFG:SETPOINT")) = __config.setpoint; __ca_clnt->UpdateVariable("TEST-TPL-CFG:SETPOINT");
	}
      else
	{
	  log_warning("Load rejected due to CRC check");
	}

      Reply_t reply;

      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = status;

      if (status)
	{
	  snprintf(reply.reason, STRING_MAX_LENGTH, "Correct hash '%u' for seed '%u'", hash, seed);
	}
      else
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "error", STRING_MAX_LENGTH);
	  snprintf(reply.reason, STRING_MAX_LENGTH, "Wrong hash '%u %u' for seed '%u'", hash, check, seed);
	}

      ccs::types::AnyValue reply_value (__reply_struct);
      reply_value = reply;

      return reply_value;
    }

  // Default reply
  Reply_t reply;

  reply.timestamp = ccs::HelperTools::GetCurrentTime();
  reply.status = false;
  ccs::HelperTools::SafeStringCopy(reply.qualifier, "error", STRING_MAX_LENGTH);
  snprintf(reply.reason, STRING_MAX_LENGTH, "Unknown qualifier '%s'", qualifier.c_str());

  ccs::types::AnyValue reply_error (__reply_struct);
  reply_error = reply;

  return reply_error;

}

void RPCServiceImpl::HandleCAMonitor (const char* name, const ccs::types::AnyValue& request)
{

  log_info("RPCServiceImpl::HandleCAMonitor - Monitor '%s' channel received", name);

  bool status = (static_cast<const char*>(NULL) != name);
  bool found = false;

  for (ccs::types::uint32 index = 0u; (status && (index < assoc.size())); index += 1u)
    {
      if (strcmp(assoc[index].second.c_str(), name) == 0)
	{
	  found = true;
	  memcpy(ccs::HelperTools::GetAttributeReference(ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()->GetVariable("TEST-TPL-CFG:STRUCT"), assoc[index].first.c_str()), 
		 ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable(name)->GetInstance(), 
		 ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()->GetVariable(name)->GetType()->GetSize());
	}
    }

  if (found)
    {
      ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()->UpdateVariable("TEST-TPL-CFG:STRUCT");
    }

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

  char service [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(service, "RPC-SERVICE", STRING_MAX_LENGTH);

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

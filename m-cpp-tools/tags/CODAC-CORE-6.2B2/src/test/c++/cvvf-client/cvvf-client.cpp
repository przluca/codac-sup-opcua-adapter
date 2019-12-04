/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/cvvf-client/cvvf-client.cpp $
* $Id: cvvf-client.cpp 98970 2019-04-10 12:37:49Z bauvirb $
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

#include "PVAccessRPCClient.h"

// Constants

#define DEFAULT_AFFINITY       0
#define DEFAULT_ITERATIONS     1
#define DEFAULT_PAYLOAD     1024 // 1kB
#define DEFAULT_PERIOD   1000000000ul

// Type definition

// Global variables

bool _terminate = false;

// Function definition

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

  char service [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(service, "rpc@cvvf-service", STRING_MAX_LENGTH);
  char buffer [1024] = STRING_UNDEFINED;
  ccs::types::uint32 hash = 0u;

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
	  else if ((strcmp(argv[index], "-c") == 0) || (strcmp(argv[index], "--config") == 0))
	    {
	      if ((index + 1) < (uint_t) argc) 
		{
		  ccs::HelperTools::SafeStringCopy(buffer, argv[index + 1], 1024u);
		}
	      else { print_usage(); return (0); } // Display usage
	      index += 1;
            
	    }
	  else if ((strcmp(argv[index], "-h") == 0) || (strcmp(argv[index], "--hash") == 0))
	    {
	      if ((index + 1) < (uint_t) argc) 
		{
		  sscanf(argv[index + 1], "%u", &hash);
		}
	      else { print_usage(); return (0); } // Display usage
	      index += 1;

	    }
	  else if ((strcmp(argv[index], "-s") == 0) || (strcmp(argv[index], "--service") == 0))
	    {
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

  // Declare RPC structure
  log_info("Declare RPC request structure");
  ccs::types::CompoundType rpc_request_struct ("epics:nt/RPCRequest:1.0");
  rpc_request_struct.AddAttribute<ccs::types::uint64>("timestamp");
  rpc_request_struct.AddAttribute<ccs::types::string>("qualifier");
  rpc_request_struct.AddAttribute("value",((new (std::nothrow) ccs::types::CompoundType ("sup::cvvf::Launch_t:1.0"))
					   ->AddAttribute<ccs::types::string>("name")));

  ccs::types::CompoundType config_struct ("app:Config_t/1.0");
  config_struct.AddAttribute<ccs::types::uint32>("parameter");
  config_struct.AddAttribute<ccs::types::uint64>("delay");
  config_struct.AddAttribute<ccs::types::string>("operation");

  ccs::types::CompoundType cfg_request_struct (rpc_request_struct);
  cfg_request_struct.AddAttribute("value", config_struct);

  // Create RPC client
  log_info("Create RPC client on '%s' service", service);
  ccs::base::PVAccessRPCClient client (service);

  // Connect to RPC server
  bool status = client.Launch();

  if (status)
    {
      ccs::types::CompoundType exe_request_struct (rpc_request_struct);

      ccs::types::AnyValue exe_request (exe_request_struct);
      status = (ccs::HelperTools::SetAttributeValue(&exe_request, "qualifier", "process") &&
		ccs::HelperTools::SetAttributeValue(&exe_request, "value.name", "echo"));

      log_info("Send RPC request ..");
      ccs::types::AnyValue exe_reply = client.SendRequest(exe_request);
      ccs::types::boolean reply_status = false;
      status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&exe_reply, "status", reply_status);

      {
	char buffer [1024]; exe_reply.SerialiseInstance(buffer, 1024u);
	log_error("..  reply '%s'", buffer);
	status = reply_status;
      }
    }

   // Terminate
  log_info("Terminate program");

  return (0);

}

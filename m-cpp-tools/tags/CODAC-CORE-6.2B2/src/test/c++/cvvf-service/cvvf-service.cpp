/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/cvvf-service/cvvf-service.cpp $
* $Id: cvvf-service.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include <functional> // std::function
#include <map> // std::map
#include <new> // std::nothrow
#include <utility> // std::pair
#include <thread> // std::thread

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

class DPCHandler
{

  private:

    ccs::types::uint32 __seed = 0u;

    struct DPC_t {
      bool running;
      bool status;
      std::function<bool(void*)> process;
      void* attribute;
    };

    std::map<ccs::types::uint32,DPC_t> __processes;

    bool IsValid (ccs::types::uint32 id) const { return (__processes.end() != __processes.find(id)); };
    ccs::types::uint32 GenerateKey (void) { __seed += 1u; return __seed; };
    DPC_t GetData (ccs::types::uint32 id)
    {
      DPC_t data;

      bool status = IsValid(id);

      if (status)
	{
	  data = __processes.find(id)->second;
	}

      return data;

    };

    bool Execute (ccs::types::uint32 id)
    {

      log_info("DPCHandler::Execute('%u') - Starting", id);

      bool status = IsValid(id);

      if (status)
	{
	  DPC_t data = GetData(id);

	  log_info("DPCHandler::Execute('%u') - Running", id);

	  data.running = true;

	  // store for future use
	  log_info("DPCHandler::Execute('%u') - Replace", id);
	  __processes.find(id)->second = data;

	  data.status = data.process(data.attribute);

	  log_info("DPCHandler::Execute('%u') - Finished", id);

	  data.running = false;

	  // store for future use
	  log_info("DPCHandler::Execute('%u') - Replace", id);
	  __processes.find(id)->second = data;
	}

      return status;

    };

  public:

    DPCHandler (void) {};
    virtual ~DPCHandler (void) {};

    ccs::types::uint32 Launch (std::function<bool(void*)> cb, void* attr)
    {

      ccs::types::uint32 key = this->GenerateKey();

      DPC_t data;

      data.running = false;
      data.status = false;
      data.process = cb;
      data.attribute = attr;

      bool status = __processes.insert(std::pair<ccs::types::uint32,DPC_t>(key,data)).second;

      using namespace std::placeholders;
      std::thread thread (std::bind(&DPCHandler::Execute, this, _1), key); thread.detach();

      return key;

    };

    bool IsRunning (ccs::types::uint32 id) 
    { 

      bool running = false; 

      bool status = IsValid(id); 

      if (status) 
	{ 
	  DPC_t data = GetData(id); 
	  running = data.running; 
	}
      else
	{
	  log_warning("DPCHandler::IsRunning('%u') - Invalid key", id);
	}

      if (running)
	{
	  log_info("DPCHandler::IsRunning('%u') - Still running", id);
	}
      else
	{
	  log_info("DPCHandler::IsRunning('%u') - Stopped", id);
	}

      return running; 
    
    };
    bool GetStatus (ccs::types::uint32 id) { bool status = IsValid(id); if (status) { DPC_t data = GetData(id); status = data.status; } return status; };
    void* GetAttribute (ccs::types::uint32 id) { void* attribute = NULL; bool status = IsValid(id); if (status) { DPC_t data = GetData(id); attribute = data.attribute; } return attribute; };

};

typedef struct __attribute__((packed)) Config {
  ccs::types::uint32 parameter = 0u;
  ccs::types::uint64 delay = 0ul;
  //ccs::types::string operation = "";
} Config_t;

typedef struct __attribute__((packed)) Input {
  ccs::types::uint32 value = 0u;
} Input_t;

typedef struct __attribute__((packed)) Output {
  ccs::types::uint32 value = 0u;
} Output_t;

class RPCServiceImpl : public ccs::base::PVAccessRPCServer, public DPCHandler
{

  private:

    std::shared_ptr<ccs::types::CompoundType> __config_struct;
    std::shared_ptr<ccs::types::CompoundType> __output_struct;

    Config_t __config;
 
  public:

    struct AsyncData_t {
      //Config_t config;
      Input_t input;
      Output_t output;
    };

    RPCServiceImpl (const char* service);
    virtual ~RPCServiceImpl (void) {};

    bool Configure (Config_t& config) { __config = config; return true; };
    Output_t Process (Input_t& input) { Output_t output; output.value = __config.parameter * input.value; ccs::HelperTools::SleepFor(__config.delay); return output; };
    bool AsynchronousProcess (void* data);

    virtual ccs::types::AnyValue HandleRequest (const ccs::types::AnyValue& request); // Overloaded virtual method

};

// Global variables

bool _terminate = false;

// Function definition

bool RPCServiceImpl::AsynchronousProcess (void* data)
{

  log_info("RPCServiceImpl::AsynchronousProcess - Entering method");

  bool status = true;

  try
    {
      static_cast<RPCServiceImpl::AsyncData_t*>(data)->output = this->Process(static_cast<RPCServiceImpl::AsyncData_t*>(data)->input);
    }
  catch (std::exception& error)
    {
      status = false;
    }

  log_info("RPCServiceImpl::AsynchronousProcess - Leaving method");

  return status;

}

RPCServiceImpl::RPCServiceImpl (const char* service) : ccs::base::PVAccessRPCServer (service)
{

  // Initialise attributes
  __config_struct = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("app::Config_t/v1.0"))
							      ->AddAttribute<ccs::types::uint32>("parameter")
							      ->AddAttribute<ccs::types::uint64>("delay"));
  __output_struct = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("app::Output_t/v1.0"))
							      ->AddAttribute<ccs::types::uint32>("value"));

  //ccs::HelperTools::SafeStringCopy(__config.operation, "multiply", ccs::types::MaxStringLength); 
  __config.parameter = 0; 

  return;

}

ccs::types::AnyValue RPCServiceImpl::HandleRequest (const ccs::types::AnyValue& request)
{

  const ccs::types::AnyValue* __query_value = &request;
#if 0
  // WARNING - eget command line tool embeds the attributes in a 'query' structure
  if (ccs::HelperTools::HasAttribute(&request, "query"))
    {
      __query_value = new (std::nothrow) ccs::types::AnyValue (ccs::HelperTools::GetAttributeType(&request, "query"), 
							       ccs::HelperTools::GetAttributeReference(&request, "query"));
    }
#endif
  std::string qualifier;

  bool status = (ccs::HelperTools::HasAttribute(__query_value, "qualifier") &&
		 (ccs::types::String == ccs::HelperTools::GetAttributeType(__query_value, "qualifier")));

  if (status)
    {
      qualifier = std::string(static_cast<char*>(ccs::HelperTools::GetAttributeReference(__query_value, "qualifier")));
    }

  // We have to support the following operations:
  // - config - Provide service configuration
  // - process - Synchronous invocation
  // - launch - Asynchronous with
  //   - abort('id') - Abort execution
  //   - status('id') - Poll status
  //   - result('id') - Get result

  if (qualifier == "config")
    {

      status = (ccs::HelperTools::HasAttribute(__query_value, "value") &&
		(sizeof(Config_t) == ccs::HelperTools::GetAttributeType(__query_value, "value")->GetSize()));

      if (status)
	{
	  // Extract configuration data
	  log_info("RPCServiceImpl::HandleRequest('%s') - Extract configuration data ..", qualifier.c_str());
	  Config_t config = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(__query_value, "value");
	  status = this->Configure(config);
	}

      // Default reply
      ccs::base::PVAccessRPC::Reply_t reply;
      
      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = status;

      if (status)
	{
	  char buffer [ccs::types::MaxStringLength]; 
	  ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(__query_value, "value").SerialiseInstance(buffer, ccs::types::MaxStringLength);

	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "success", ccs::types::MaxStringLength);
	  snprintf(reply.reason, ccs::types::MaxStringLength, "Using '%s'", buffer);
	}
      else
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "failure", ccs::types::MaxStringLength);
	  snprintf(reply.reason, ccs::types::MaxStringLength, "Invalid configuration");
	}

      ccs::types::AnyValue reply_value (ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      reply_value = reply;

      return reply_value;

    }

  if (qualifier == "process")
    {

      status = ccs::HelperTools::HasAttribute(__query_value, "value");

      if (!status)
	{
	  log_error("RPCServiceImpl::HandleRequest('%s') - No such attribute", qualifier.c_str());
	}

      if (status)
	{
	  status = (sizeof(Input_t) == ccs::HelperTools::GetAttributeType(__query_value, "value")->GetSize());

	  if (!status)
	    {
	      log_error("RPCServiceImpl::HandleRequest('%s') - Size issue '%u'", qualifier.c_str(), ccs::HelperTools::GetAttributeType(__query_value, "value")->GetSize());
	    }
	}

      Input_t input;
      Output_t output;

      if (status)
	{
	  // Extract input data
	  log_info("RPCServiceImpl::HandleRequest('%s') - Extract input data ..", qualifier.c_str());
	  input = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(__query_value, "value");

	  try
	    {
	      output = this->Process(input);
	    }
	  catch (std::exception& error)
	    {
	      status = false;
	    }
	}

      // Reply with parameter
      ccs::base::PVAccessRPC::ReplyWithValue<Output_t> reply;
      
      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = status;
      reply.value = output;

      if (status)
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "success", ccs::types::MaxStringLength);
	}
      else
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "failure", ccs::types::MaxStringLength);
	  snprintf(reply.reason, ccs::types::MaxStringLength, "Exception raised");
	}

      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      // .. and add the missing bit
      reply_type.AddAttribute("value", __output_struct);
 
      ccs::types::AnyValue reply_value (reply_type);
      reply_value = reply;

      return reply_value;

    }

  if (qualifier == "launch")
    {

      status = (ccs::HelperTools::HasAttribute(__query_value, "value") &&
		(sizeof(Input_t) == ccs::HelperTools::GetAttributeType(__query_value, "value")->GetSize()));

      ccs::types::uint32 key = 0u;

      if (status)
	{
	  // Extract input data
	  log_info("RPCServiceImpl::HandleRequest('%s') - Extract input data ..", qualifier.c_str());

	  RPCServiceImpl::AsyncData_t* data = new (std::nothrow) RPCServiceImpl::AsyncData_t;

	  data->input = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(__query_value, "value");

	  using namespace std::placeholders;
	  key = this->Launch(std::bind(&RPCServiceImpl::AsynchronousProcess, this, _1), static_cast<void*>(data));
	}

      // Reply with parameter .. the key
      ccs::base::PVAccessRPC::ReplyWithValue<ccs::types::uint32> reply;
      
      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = status;
      reply.value = key;

      if (status)
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "success", ccs::types::MaxStringLength);
	}
      else
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "failure", ccs::types::MaxStringLength);
	  snprintf(reply.reason, ccs::types::MaxStringLength, "Unknown failure");
	}

      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      // .. and add the missing bit
      reply_type.AddAttribute<ccs::types::uint32>("value");
 
      ccs::types::AnyValue reply_value (reply_type);
      reply_value = reply;

      return reply_value;

    }
#if 0
  if (qualifier == "abort")
    {
    }
#endif
  if (qualifier == "result")
    {

      status = ccs::HelperTools::HasAttribute(__query_value, "value");

      if (!status)
	{
	  log_error("RPCServiceImpl::HandleRequest('%s') - No such attribute", qualifier.c_str());
	}

      if (status)
	{
	  status = (ccs::types::UnsignedInteger32 == ccs::HelperTools::GetAttributeType(__query_value, "value"));

	  if (!status)
	    {
	      log_error("RPCServiceImpl::HandleRequest('%s') - Type issue '%s'", qualifier.c_str(), ccs::HelperTools::GetAttributeType(__query_value, "value")->GetName());
	    }
	}

      ccs::types::uint32 key;

      if (status)
	{
	  // Extract input data
	  log_info("RPCServiceImpl::HandleRequest('%s') - Extract key ..", qualifier.c_str());
	  key = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(__query_value, "value");
	  status = !this->IsRunning(key);
	}

      Output_t output;

      if (status)
	{
	  RPCServiceImpl::AsyncData_t* data = static_cast<RPCServiceImpl::AsyncData_t*>(this->GetAttribute(key));
	  output = data->output;
	  delete data;
	  // Remove from DPCHandler
	}

      // Reply with parameter
      ccs::base::PVAccessRPC::ReplyWithValue<Output_t> reply;
      
      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = status;
      reply.value = output;

      if (status)
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "success", ccs::types::MaxStringLength);
	}
      else
	{
	  ccs::HelperTools::SafeStringCopy(reply.qualifier, "failure", ccs::types::MaxStringLength);
	  snprintf(reply.reason, ccs::types::MaxStringLength, "Try again later");
	}

      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*ccs::base::PVAccessRPC::Reply_int); // Default RPC reply type
      // .. and add the missing bit
      reply_type.AddAttribute("value", __output_struct);
 
      ccs::types::AnyValue reply_value (reply_type);
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
#if 0
  if (__query_value != &request)
    {
      delete __query_value;
    }
#endif
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

  char service [ccs::types::MaxStringLength] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(service, "rpc@cvvf-service", ccs::types::MaxStringLength);

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

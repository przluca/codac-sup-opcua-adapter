/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/pva-client/pva-client.cpp $
* $Id: pva-client.cpp 99248 2019-04-23 10:41:47Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Simple test program
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

/* Global header files */

#include <errno.h>
#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */

#include <signal.h> /* sigset, etc. */

/* Local header files */

#include "BasicTypes.h" /* Misc. type definition */
#include "SysTools.h" /* Misc. helper functions */
#include "TimeTools.h" /* Misc. helper functions */

#include "log-api.h" // Syslog wrapper routines

#include "PVAccessClient.h"

/* Constants */

#define DEFAULT_AFFINITY       0
#define DEFAULT_PERIOD   1000000000 /* 1kHz */

/* Type definition */

/* Global variables */

bool _terminate = false;

/* Internal function definition */

void print_usage (void)
{

  char prog_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  get_program_name((char*) prog_name);

  fprintf(stdout, "Usage: %s <options>\n", prog_name);
  fprintf(stdout, "Options: -h|--help: Print usage.\n");
  fprintf(stdout, "         -a|--affinity <core_id>: Run thread on <core_id> CPU core, defaults to 0.\n");
  fprintf(stdout, "         -c|--count <cycle_nb>: Stop after <cycle_nb> cycles, defaults to 10000.\n");
  fprintf(stdout, "         -p|--period <period_in_ns>: Execute cycles every <period_in_ns>, defaults to 1000000, i.e. 1kHz loop.\n");
  fprintf(stdout, "         -v|--verbose: Verbose mode, statistics and measurmeent data are printed on stdout.\n");
  fprintf(stdout, "\n");

  return;

};

void signal_handler (int signal)
{

  log_info("Received signal '%d' to terminate", signal);
  _terminate = true;

};

/* Public function definition */

int main(int argc, char **argv)
{

  using namespace ccs::base;
  using namespace ccs::types;

  /* Install signal handler to support graceful termination */
  sigset(SIGTERM, signal_handler);
  sigset(SIGINT,  signal_handler);
  sigset(SIGHUP,  signal_handler);

  uint32 core = 0;
  uint64 period = DEFAULT_PERIOD;
  bool verbose = false;

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
	  else if ((strcmp(argv[index], "-a") == 0) || (strcmp(argv[index], "--affinity") == 0))
	    {
	      // Get core identifier
	      if ((index + 1) < (uint_t) argc) 
		{
		  sscanf(argv[index + 1], "%u", &core);
		  log_info("Setting thread affinity to core '%d'", core);
		  ccs::HelperTools::SetAffinityToCore(core);
		}
	      else { /* Display usage */ print_usage(); return (0); }
	      index += 1;
            
	    }
	  else if ((strcmp(argv[index], "-p") == 0) || (strcmp(argv[index], "--period") == 0))
	    {
	      // Get period in ns
	      sscanf(argv[index + 1], "%lu", &period);
	      index += 1;
            
	    }
	  else if ((strcmp(argv[index], "-v") == 0) || (strcmp(argv[index], "--verbose") == 0))
	    {
	      // Set verbose mode
	      verbose = true;
	      ccs::log::SetFilter(LOG_INFO);
	      ccs::log::SetStdout();
            
	    }
	}
    }
  else
    {
    }

  // PVAccess client and cache
  PVAccessClient* p_srv = PVAccessInterface::GetInstance<PVAccessClient>();

  bool status = (static_cast<PVAccessClient*>(NULL) != p_srv);

  if (status)
    {
      status = p_srv->AddVariable("TEST-TPL-CFG:STRUCT", ccs::types::InputVariable, static_cast<const ccs::types::AnyType*>(NULL)); // Retrieve type from the server
    }

  if (status)
    {
      status = p_srv->AddVariable("TEST-TPL-CFG:INPUT", ccs::types::OutputVariable, static_cast<const ccs::types::AnyType*>(NULL)); // Retrieve type from the server
    }

  if (status)
    {
      p_srv->SetPeriod(5000000000ul);
      status = p_srv->Launch();
    }

  if (status)
    {
      status = p_srv->IsValid("TEST-TPL-CFG:STRUCT");
    }
      
  while (status && (_terminate == false))
    {

      uint64 curr_time = ccs::HelperTools::SleepFor(period);

      char string [4096];

      status = p_srv->GetVariable("TEST-TPL-CFG:STRUCT")->SerialiseInstance(string, 4096u);

      if (status)
	{
	  log_info("Variable is '%s'", string);
	}

      if (status)
	{
	  status = p_srv->GetVariable("TEST-TPL-CFG:INPUT")->ParseInstance(string);
	}

      if (status)
	{
	  p_srv->UpdateVariable("TEST-TPL-CFG:INPUT");
	}

    }

  if (!status)
    {
      log_error("Wrong termination");
    }

  // Release resources
  delete p_srv;

  return 0;

};

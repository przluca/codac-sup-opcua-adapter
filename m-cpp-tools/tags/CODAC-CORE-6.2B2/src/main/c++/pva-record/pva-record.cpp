/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva-record/pva-record.cpp $
* $Id: pva-record.cpp 99248 2019-04-23 10:41:47Z bauvirb $
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

#include <fstream> // File access

#include <signal.h> /* sigset, etc. */

#include <BasicTypes.h> /* Misc. type definition */
#include <SysTools.h> /* Misc. helper functions */
#include <TimeTools.h> /* Misc. helper functions */

#include <log-api.h> // Syslog wrapper routines

/* Local header files */

#include "PVAccessMonitor.h"

/* Constants */

#define DEFAULT_AFFINITY       0u
#define DEFAULT_RECORD_COUNT  10u
#define DEFAULT_CHANNEL_NAME "TEST-TPL-CFG:STRUCT"

/* Type definition */

class PVAccessMonitorImpl : public ccs::base::PVAccessMonitor
{

  private:

    ccs::types::uint32 __count;
    std::ofstream __file;

  public:

    PVAccessMonitorImpl (const char* name);
    virtual ~PVAccessMonitorImpl (void);

    bool IsComplete (void) { return (0u == __count); };
    bool SetCount (ccs::types::uint32 count) { __count = count; return true; };

    virtual void HandleMonitor (const ccs::types::AnyValue& value);

};

/* Global variables */

bool _terminate = false;

/* Function definition */

void PVAccessMonitorImpl::HandleMonitor (const ccs::types::AnyValue& value)
{

  bool status = (0u != __count);

  char buffer [4096u] = STRING_UNDEFINED;

  if (status)
    {
      status = value.SerialiseInstance(buffer, 4096u);
    }

  if (status)
    {
      __file << buffer << std::endl;
    }

  if (status)
    {
      __count -= 1u;
    }

  return;

}

PVAccessMonitorImpl::PVAccessMonitorImpl (const char* name) : ccs::base::PVAccessMonitor(name)
{

  ccs::types::uint64 time = ccs::HelperTools::Floor(ccs::HelperTools::GetCurrentTime()); // Current time with second resolution
  ccs::types::string iso8601; ccs::HelperTools::ToISO8601(time, iso8601);

  std::string file = std::string("/tmp/") + std::string(this->GetChannel()) + std::string("_") + std::string(iso8601) + std::string(".record");

  __file.open(file, { std::ios::out | std::ios::trunc });

  return;

}

PVAccessMonitorImpl::~PVAccessMonitorImpl (void)
{

  __file.close();

  return;

}

void print_usage (void)
{

  char prog_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  get_program_name((char*) prog_name);

  fprintf(stdout, "Usage: %s <options>\n", prog_name);
  fprintf(stdout, "Options: -h|--help: Print usage.\n");
  fprintf(stdout, "         -a|--affinity <core_id>: Run thread on <core_id> CPU core, defaults to 0.\n");
  fprintf(stdout, "         -c|--count <cycle_nb>: Stop after <cycle_nb> cycles, defaults to 10000.\n");
  fprintf(stdout, "         -n|--name <channel>: Channel name, defaults to 'TEST-TPL-CFG:STRUCT'.\n");
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

  uint32 core = DEFAULT_AFFINITY;
  uint32 count = DEFAULT_RECORD_COUNT;

  char name [STRING_MAX_LENGTH] = DEFAULT_CHANNEL_NAME;

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
	  else if ((strcmp(argv[index], "-c") == 0) || (strcmp(argv[index], "--count") == 0))
	    {
	      // Get core identifier
	      if ((index + 1) < (uint_t) argc) sscanf(argv[index + 1], "%u", &count);
	      else { /* Display usage */ print_usage(); return (0); }
	      index += 1;
            
	    }
	  else if ((strcmp(argv[index], "-n") == 0) || (strcmp(argv[index], "--name") == 0))
	    {
	      // Get channel name
	      if ((index + 1) < (uint_t) argc) ccs::HelperTools::SafeStringCopy(name, argv[index + 1], STRING_MAX_LENGTH);
	      else { /* Display usage */ print_usage(); return (0); }
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
  PVAccessMonitorImpl monitor (name);
  monitor.SetCount(count);

  while ((_terminate == false) && (monitor.IsComplete() == false)) { uint64 curr_time = ccs::HelperTools::SleepFor(100000000ul); }

  return 0;

};
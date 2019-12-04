/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/obj-factory/obj-factory.cpp $
* $Id: obj-factory.cpp 100878 2019-07-18 11:14:46Z bauvirb $
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

// Global header files

#include <stdio.h> // sscanf, printf, etc.
#include <string.h> // strncpy, etc.
#include <stdarg.h> // va_start, etc.
#include <signal.h> // sigset, etc.

#include <log-api.h> // CCS logging library

#include <BasicTypes.h> // Global type definition
#include <SysTools.h> // Misc. helper functions
#include <TimeTools.h> // Misc. helper functions

#include <ObjectDatabase.h>
#include <ObjectFactory.h>

// Local header files

#include "ObjectHandler.h"
#include "ObjectHandlerHelper.h"

// Constants

// Type definition



// Function declaration

// Global variables

bool _terminate = false;

void print_usage (void)
{

  char prog_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  ccs::HelperTools::GetProcessName(prog_name);

  fprintf(stdout, "Usage: %s <options>\n", prog_name);
  fprintf(stdout, "Options: -h|--help: Print usage.\n");
  fprintf(stdout, "         -f|--file <file_name>: Load and parse <file_name>.\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "The program parses <file_name>, loads identified shared libraries and instantiates classes by name.\n");
  fprintf(stdout, "See ${CODAC_ROOT}/examples/cpp-common/obj-factory.xml for an example of what is supported.\n");
  fprintf(stdout, "\n");

  return;

}

void signal_handler (int signal) { _terminate = true; }

// Public function definition

int main (int argc, char** argv) 
{

  // Install signal handler to support graceful termination
  sigset(SIGTERM, signal_handler);
  sigset(SIGINT,  signal_handler);
  sigset(SIGHUP,  signal_handler);

  char file_path [PATH_MAX_LENGTH] = STRING_UNDEFINED;

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
          else if ((strcmp(argv[index], "-f") == 0) || (strcmp(argv[index], "--file") == 0))
	    {
	      // Get parameter
	      ccs::HelperTools::SafeStringCopy(file_path, argv[index+1], PATH_MAX_LENGTH);
	      index += 1;
	    }
	}
    }
  else
    {
    }

  log_info("Instantiate object engine");
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool status = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (status)
    {
      log_info("Parsing '%s' file ..", file_path);
      status = sup::HelperTools::ParseXMLFile(handler, file_path);
    }

  if (!status)
    {
      log_error(".. failure");
      _terminate = true;
    }

  while (status && (_terminate != true)) ccs::HelperTools::SleepFor(100000000ul); // 10Hz

  if (static_cast<sup::core::ObjectHandler*>(NULL) != handler)
    {
      log_info("Terminate object engine");
      delete handler;
    }

  return (0);

}

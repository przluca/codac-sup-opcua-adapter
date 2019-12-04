/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/pva-server/pva-server.cpp $
* $Id: pva-server.cpp 99248 2019-04-23 10:41:47Z bauvirb $
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

#include "PVAccessServer.h"

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

}

void signal_handler (int signal)
{

  log_info("Received signal '%d' to terminate", signal);
  _terminate = true;

}

void HandleMonitor (const ccs::types::AnyValue& value)
{
  char buffer [1024] = STRING_UNDEFINED;
  value.SerialiseInstance(buffer, 1024u);
  log_info("HandleMonitor - Update '%s'", buffer);
}

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

  // Instrospectable type definition
  CompoundType* p_scalar = new (std::nothrow) CompoundType ("Scalars_t");
  CompoundType* p_arrays = new (std::nothrow) CompoundType ("Arrays_t");
  CompoundType* p_status = new (std::nothrow) CompoundType ("Status_t");
  CompoundType* p_struct = new (std::nothrow) CompoundType ("Struct_t");

  bool status = ((static_cast<CompoundType*>(NULL) != p_scalar) && 
		 (static_cast<CompoundType*>(NULL) != p_arrays) && 
		 (static_cast<CompoundType*>(NULL) != p_status) && 
		 (static_cast<CompoundType*>(NULL) != p_struct));

  if (status)
    {
      p_scalar->AddAttribute<boolean>("bool");
      p_scalar->AddAttribute<int8>("int8");
      p_scalar->AddAttribute<uint8>("uint8");
      p_scalar->AddAttribute<int16>("int16");
      p_scalar->AddAttribute<uint16>("uint16");
      p_scalar->AddAttribute<int32>("int32");
      p_scalar->AddAttribute<uint32>("uint32");
      p_scalar->AddAttribute<int64>("int64");
      p_scalar->AddAttribute<uint64>("uint64");
      p_scalar->AddAttribute<float32>("float32");
      p_scalar->AddAttribute<float64>("float64");
      p_scalar->AddAttribute<string>("string");
    }

  if (status)
    {
      p_arrays->AddAttribute("bool", (new (std::nothrow) ScalarArray ("BooleanArray_t", Boolean, 8)));
      p_arrays->AddAttribute("int8", (new (std::nothrow) ScalarArray ("SignedInteger8Array_t", SignedInteger8, 8)));
      p_arrays->AddAttribute("uint8", (new (std::nothrow) ScalarArray ("UnsignedInteger8Array_t", UnsignedInteger8, 8)));
      p_arrays->AddAttribute("int16", (new (std::nothrow) ScalarArray ("SignedInteger16Array_t", SignedInteger16, 8)));
      p_arrays->AddAttribute("uint16", (new (std::nothrow) ScalarArray ("UnsignedInteger16Array_t", UnsignedInteger16, 8)));
      p_arrays->AddAttribute("int32", (new (std::nothrow) ScalarArray ("SignedInteger32Array_t", SignedInteger32, 8)));
      p_arrays->AddAttribute("uint32", (new (std::nothrow) ScalarArray ("UnsignedInteger32Array_t", UnsignedInteger32, 8)));
      p_arrays->AddAttribute("int64", (new (std::nothrow) ScalarArray ("SignedInteger64Array_t", SignedInteger64, 8)));
      p_arrays->AddAttribute("uint64", (new (std::nothrow) ScalarArray ("UnsignedInteger64Array_t", UnsignedInteger64, 8)));
      p_arrays->AddAttribute("float32", (new (std::nothrow) ScalarArray ("Float32Array_t", Float32, 8)));
      p_arrays->AddAttribute("float64", (new (std::nothrow) ScalarArray ("Float64Array_t", Float64, 8)));
      p_arrays->AddAttribute("string", (new (std::nothrow) ScalarArray ("StringArray_t", String, 8)));
    }

  if (status)
    {
      p_struct->AddAttribute<string>("host")
	      ->AddAttribute<string>("version")
	      ->AddAttribute<uint64>("time");
    }

  CompoundType* p_e1 = (new (std::nothrow) CompoundType)
    ->AddAttribute("value", ccs::HelperTools::NewArrayType("array::uint", ccs::types::UnsignedInteger32, 10));
  CompoundType* p_eb = (new (std::nothrow) CompoundType)
    ->AddAttribute("ElementB1", (new (std::nothrow) CompoundType)->AddAttribute("value", ccs::HelperTools::NewArrayType("array::uint", ccs::types::UnsignedInteger32, 4)))
    ->AddAttribute("ElementB2", (new (std::nothrow) CompoundType)->AddAttribute("value", ccs::HelperTools::NewArrayType("array::uint", ccs::types::Float32, 5)));

  if (status)
    {
      p_status->AddAttribute("scalars", p_scalar)
	->AddAttribute("arrays", p_arrays)
	->AddAttribute("struct", p_struct);
    }

  // PVAccess server and cache
  PVAccessServer* p_srv = PVAccessInterface::GetInstance<PVAccessServer>();

  CompoundType* p_record = new (std::nothrow) CompoundType ("MyRecord_t");

  if (status)
    {
      p_record->AddAttribute("value", p_status);
      //p_record->AddAttribute("timeStamp", );
    }

  if (status)
    {
      status = p_srv->AddVariable("TEST-TPL-CFG:STRUCT", ccs::types::AnyputVariable, p_record);
    }

  if (status)
    {
      status = p_srv->AddVariable("TEST-TPL-CFG:INPUT", ccs::types::InputVariable, p_record);
    }

  if (status)
    {
      //p_srv->SetPeriod(10000000ul);
      status = p_srv->Launch();
    }

  if (status)
    {
      status = p_srv->IsValid("TEST-TPL-CFG:STRUCT");
    }
      
  if (status)
    {
      // Provide compile-time default
      char string [] = 
	"<value>"
	"  <scalars>"
	"    <bool>false</bool>"
	"    <int8>0</int8>"
	"    <uint8>0</uint8>"
	"    <int16>0</int16>"
	"    <uint16>0</uint16>"
	"    <int32>0</int32>"
	"    <uint32>0</uint32>"
	"    <int64>0</int64>"
	"    <uint64>0</uint64>"
	"    <float32>0</float32>"
	"    <float64>0</float64>"
	"    <string></string>"
	"  </scalars>"
	"  <arrays>"
	"    <bool>[ false, false, false, false, false, false, false, false ]</bool>"
	"    <int8>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</int8>"
	"    <uint8>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</uint8>"
	"    <int16>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</int16>"
	"    <uint16>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</uint16>"
	"    <int32>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</int32>"
	"    <uint32>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</uint32>"
	"    <int64>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</int64>"
	"    <uint64>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</uint64>"
	"    <float32>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</float32>"
	"    <float64>[ 0, 0, 0, 0, 0, 0, 0, 0 ]</float64>"
	"    <string>[ 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight' ]</string>"
	"  </arrays>"
	"  <struct>"
	"    <host>Undefined</host>"
	"    <version>Undefined</version>"
	"    <time>0</time>"
	"  </struct>"
	"  <Element1>"
	"    <value>[ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]</value>"
	"  </Element1>"
	"  <ElementsB>"
	"    <ElementB1>"
	"      <value>[ 0, 0, 0, 0 ]</value>"
	"    </ElementB1>"
	"    <ElementB2>"
	"      <value>[ 0, 0, 0, 0, 0 ]</value>"
	"    </ElementB2>"
	"  </ElementsB>"
	"</value>";

      log_info("Provide compile-time default '%s'", string);
      status = p_srv->GetVariable("TEST-TPL-CFG:STRUCT")->ParseInstance(string); 
    }

  if (status)
    {
      p_srv->UpdateVariable("TEST-TPL-CFG:STRUCT");
    }

  if (status)
    {
      status = p_srv->IsValid("TEST-TPL-CFG:INPUT");
    }
      
  if (status)
    {
      status = p_srv->SetCallback("TEST-TPL-CFG:INPUT", &HandleMonitor);
    }
      
  while (status && (_terminate == false))
    {

      uint64 curr_time = ccs::HelperTools::SleepFor(period);

      AnyValue* p_val = p_srv->GetVariable("TEST-TPL-CFG:STRUCT");
      
      // boolean
      log_info("Read boolean flag ..");
      bool flag = ccs::HelperTools::GetAttributeValue<boolean>(p_val, "value.scalars.bool");
      log_info(".. '%s'", (flag ? "true" : "false"));
      status = ccs::HelperTools::SetAttributeValue<boolean>(p_val, "value.scalars.bool",
							   !flag);

      try
	{
	  log_info("Write boolean array ..");
	  AnyValue bool_array = ccs::HelperTools::GetAttributeValue<AnyValue>(p_val, "value.arrays.bool");
	  status = ccs::HelperTools::SetElementValue<boolean>(&bool_array, 0u, flag);
	  status = ccs::HelperTools::SetElementValue<boolean>(&bool_array, 1u, !flag);
	  status = ccs::HelperTools::SetElementValue<boolean>(&bool_array, 2u, flag);
	  status = ccs::HelperTools::SetElementValue<boolean>(&bool_array, 3u, !flag);
	}
      catch (const uint32 error)
	{
	  log_error("Exception '%u'", error);
	}

      // string
      log_info("Write string ..");
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED;
      snprintf(buffer, STRING_MAX_LENGTH, "Hello world at '%lu (0x%16.16lx)'", curr_time, curr_time);
      ccs::HelperTools::SafeStringCopy(static_cast<char*>(ccs::HelperTools::GetAttributeReference(p_val, "value.scalars.string")), 
				       buffer, 
				       STRING_MAX_LENGTH);

      // uint8
      status = ccs::HelperTools::SetAttributeValue<uint8>(p_val, "value.scalars.uint8",
							  ((uint8*)&curr_time)[0]);

      // uint16
      status = ccs::HelperTools::SetAttributeValue<uint16>(p_val, "value.scalars.uint16",
							  ((uint16*)&curr_time)[0]);

      // int32
      uint64 sec = curr_time / 1000000000ul;
      status = ccs::HelperTools::SetAttributeValue<int32>(p_val, "value.scalars.int32",
							  (int32) sec);

      // uint32
      uint64 nsec = curr_time - 1000000000ul * sec;
      status = ccs::HelperTools::SetAttributeValue<uint32>(p_val, "value.scalars.uint32",
							   (uint32) nsec);

      // uint64
      status = ccs::HelperTools::SetAttributeValue<uint64>(p_val, "value.scalars.uint64",
							   curr_time);

      // float32
      status = ccs::HelperTools::SetAttributeValue<float32>(p_val, "value.scalars.float32",
							    ((float32) curr_time * 1e-9));

      // float64
      status = ccs::HelperTools::SetAttributeValue<float64>(p_val, "value.scalars.float64",
							    ((float64) curr_time * 1e-9));

      // Assignment operator
      AnyValue sub_struct = ccs::HelperTools::GetAttributeValue<AnyValue>(p_val, "value.struct");
      struct {
	string host;
	string version;
	uint64 time;
      } c_struct;

      ccs::HelperTools::GetHostName(c_struct.host);
      ccs::HelperTools::GetEnvironmentVariable("CODAC_VERSION", c_struct.version);
      c_struct.time = curr_time;

      sub_struct = c_struct;

      log_info("Updating variable - '%lu'", curr_time);
      p_srv->UpdateVariable("TEST-TPL-CFG:STRUCT");

    }

  if (!status)
    {
      log_error("Wrong termination");
    }

  // Release resources
  delete p_srv;
  delete p_record;
  delete p_status;

  return 0;

};

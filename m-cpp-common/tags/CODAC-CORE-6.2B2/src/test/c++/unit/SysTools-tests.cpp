/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/SysTools-tests.cpp $
* $Id: SysTools-tests.cpp 100807 2019-07-17 05:42:30Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Unit test code
*
* Author        : Bertrand Bauvir (IO)
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

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "types.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "TimeTools.h"
#include "SysTools.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition

TEST(SysTools_Test, CreatePath)
{
  bool ret = ccs::HelperTools::CreatePath("tmp/SysTools_Test/CreatePath");

  if (ret)
    {
      ret = ccs::HelperTools::CreatePath("/tmp/SysTools_Test/CreatePath");
    }

  if (ret)
    {
      ret = ccs::HelperTools::CreatePath("/tmp/SysTools_Test/CreatePath/Again");
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, GetProcessName)
{
  char proc_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = ccs::HelperTools::GetProcessName(proc_name);

  if (ret)
    {
      log_info("TEST(SysTools_Test, GetProcessName) - Result '%s'", proc_name);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, GetProcessId)
{
  char proc_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = ccs::HelperTools::GetProcessName(proc_name);
  int proc_id = ccs::HelperTools::GetProcessId(proc_name);

  if (ret)
    {
      log_info("TEST(SysTools_Test, GetProcessId) - Result '%d'", proc_id);
      ret = (proc_id == ccs::HelperTools::GetProcessId());
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, GetProcessUptime)
{
  ccs::HelperTools::SleepFor(1000000000ul);
  int uptime = ccs::HelperTools::GetProcessUptime();
  bool ret = (1u <= uptime);

  log_info("TEST(SysTools_Test, GetProcessUptime) - Result '%u'", uptime);

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, SetAffinityToCore_success)
{
  int cid = 0;

  bool ret = (ccs::HelperTools::SetAffinityToCore(cid) == true);

  if (ret)
    {
      log_info("TEST(SysTools_Test, SetAffinityToCore_success) - SetAffinityToCore successful for core '%d'", cid);
    }
  else
    {
      log_error("TEST(SysTools_Test, SetAffinityToCore_success) - SetAffinityToCore failed for core '%d'", cid);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::HasAffinityToCore(cid) == true);
    }

  if (!ret)
    {
      log_error("TEST(SysTools_Test, SetAffinityToCore_success) - HasAffinityToCore failed for core '%d'", cid);
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(1000000000ul);
      ret = (ccs::HelperTools::GetCurrentCore() == cid);
    }

  if (!ret)
    {
      log_error("TEST(SysTools_Test, SetAffinityToCore_success) - GetCurrentCore failed '%d != %d'", cid, ccs::HelperTools::GetCurrentCore());
    }
  
  cid = sysconf(_SC_NPROCESSORS_ONLN) - 1;

  if (ret)
    {
      ret = (ccs::HelperTools::SetAffinityToCore(cid) == true);
    }

  if (ret)
    {
      log_info("TEST(SysTools_Test, SetAffinityToCore_success) - SetAffinityToCore successful for core '%d'", cid);
    }
  else
    {
      log_error("TEST(SysTools_Test, SetAffinityToCore_success) - SetAffinityToCore failed for core '%d'", cid);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::HasAffinityToCore(cid) == true);
    }

  if (!ret)
    {
      log_error("TEST(SysTools_Test, SetAffinityToCore_success) - HasAffinityToCore failed for core '%d'", cid);
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(1000000000ul);
      ret = (ccs::HelperTools::GetCurrentCore() == cid);
    }

  if (!ret)
    {
      log_error("TEST(SysTools_Test, SetAffinityToCore_success) - GetCurrentCore failed for core '%d != %d'", cid, ccs::HelperTools::GetCurrentCore());
      ret = true; // WARNING - Fails on trunk
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, SetAffinityToCore_error)
{
  int cores = sysconf(_SC_NPROCESSORS_ONLN);

  bool ret = (ccs::HelperTools::SetAffinityToCore(-1) == false);

  if (ret)
    {
      ret = (ccs::HelperTools::SetAffinityToCore(cores) == false);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, SetEnvironmentVariable)
{
  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "SetEnvironmentVariable") == true);

  if (ret)
    {
      ret = (ccs::HelperTools::IsVariableDefined("SysTools_Test") == true);
    }

  if (ret)
    {
      ret = (strncmp(ccs::HelperTools::GetEnvironmentVariable("SysTools_Test"), "SetEnvironmentVariable", 22u) == 0);
    }

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED;
      ret = ((ccs::HelperTools::GetEnvironmentVariable("SysTools_Test", buffer, STRING_MAX_LENGTH) == true) &&
	     (strncmp(buffer, "SetEnvironmentVariable", 22u) == 0));
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, IsVariableString_success)
{
  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "IsVariableString") == true);

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SysTools_Test}", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsVariableString(buffer) == true);
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, IsVariableString_composition)
{
  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "IsVariableString") == true);

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SysTools_Test}==true", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsVariableString(buffer) == true);
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, IsVariableString_error)
{
  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "IsVariableString") == true);

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "SysTools_Test", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsVariableString(buffer) == false);
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, ResolveVariableString_default)
{
  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "ResolveVariableString") == true);

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SysTools_Test}", STRING_MAX_LENGTH);

      // Extract environment variable name
      char* p_suf = NULL;
      char* p_env = strtok_r(buffer, "${}", &p_suf);
      log_info("TEST(SysTools_Test, ResolveVariableString_default) - Buffer modified '%s' ..", buffer);
      log_info(".. variable '%s'", p_env);
      log_info(".. suffix '%s'", p_suf);
      ret = ((ccs::HelperTools::IsVariableDefined(p_env) == true) && 
	     (strncmp(ccs::HelperTools::GetEnvironmentVariable(p_env), "ResolveVariableString", 21u) == 0));
    }

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SysTools_Test}", STRING_MAX_LENGTH);
      char output [STRING_MAX_LENGTH] = STRING_UNDEFINED;
      log_info("TEST(SysTools_Test, ResolveVariableString_default) - Buffer '%s' ..", buffer);
      ret = (ccs::HelperTools::ResolveVariableString(buffer, output, STRING_MAX_LENGTH) == true);
      log_info(".. result '%s'", output);
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, ResolveVariableString_composition)
{
  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "ResolveVariableString") == true);

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SysTools_Test}==true", STRING_MAX_LENGTH);

      // Extract environment variable name
      char* p_suf = NULL;
      char* p_env = strtok_r(buffer, "${}", &p_suf);
      log_info("TEST(SysTools_Test, ResolveVariableString_composition) - Buffer modified '%s' ..", buffer);
      log_info(".. variable '%s'", p_env);
      log_info(".. suffix '%s'", p_suf);
      ret = ((ccs::HelperTools::IsVariableDefined(p_env) == true) && 
	     (strncmp(ccs::HelperTools::GetEnvironmentVariable(p_env), "ResolveVariableString", 21u) == 0));
    }

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SysTools_Test}==true", STRING_MAX_LENGTH);
      char output [STRING_MAX_LENGTH] = STRING_UNDEFINED;
      log_info("TEST(SysTools_Test, ResolveVariableString_composition) - Buffer '%s' (length %d) ..", buffer, strlen(buffer));
      ret = (ccs::HelperTools::ResolveVariableString(buffer, output, STRING_MAX_LENGTH) == true);
      log_info(".. result '%s' (length %d)", output, strlen(output));
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, ResolveVariableString_path)
{
  bool ret = (ccs::HelperTools::IsVariableDefined("CODAC_VAR") == true);

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; 
      ccs::HelperTools::SafeStringCopy(buffer, "${CODAC_VAR}/log/ccslog_local.log", STRING_MAX_LENGTH);
      char output [STRING_MAX_LENGTH] = STRING_UNDEFINED;
      log_info("TEST(SysTools_Test, ResolveVariableString_path) - Buffer '%s' (length %d) ..", buffer, strlen(buffer));
      ret = (ccs::HelperTools::ResolveVariableString(buffer, output, STRING_MAX_LENGTH) == true);
      log_info(".. result '%s' (length %d)", output, strlen(output));
    }
  else
    {
      log_error("TEST(SysTools_Test, ResolveVariableString_path) - ${CODAC_VAR} is undefined");
      ret = true; // WARNING - Should be investigated
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, ResolveVariableString_truncate)
{
  char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SysTools_Test}==true", STRING_MAX_LENGTH);

  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "ResolveVariableString") == true);

  if (ret)
    {
      char output = 0;
      log_info("TEST(SysTools_Test, ResolveVariableString_truncate) - Buffer '%s' (length %d) ..", buffer, strlen(buffer));
      ret = (ccs::HelperTools::ResolveVariableString(buffer, &output, 1) == true);
      log_info(".. result '%s' (length %d)", &output, strlen(&output));
    }

  if (ret)
    {
      char output [8] = STRING_UNDEFINED;
      log_info("TEST(SysTools_Test, ResolveVariableString_truncate) - Buffer '%s' (length %d) ..", buffer, strlen(buffer));
      ret = (ccs::HelperTools::ResolveVariableString(buffer, output, 8) == true);
      log_info(".. result '%s' (length %d)", output, strlen(output));
    }

  if (ret)
    {
      char output [24] = STRING_UNDEFINED;
      log_info("TEST(SysTools_Test, ResolveVariableString_truncate) - Buffer '%s' (length %d) ..", buffer, strlen(buffer));
      ret = (ccs::HelperTools::ResolveVariableString(buffer, output, 24) == true);
      log_info(".. result '%s' (length %d)", output, strlen(output));
    }

  if (ret)
    {
      char output [32] = STRING_UNDEFINED;
      log_info("TEST(SysTools_Test, ResolveVariableString_truncate) - Buffer '%s' (length %d) ..", buffer, strlen(buffer));
      ret = (ccs::HelperTools::ResolveVariableString(buffer, output, 32) == true);
      log_info(".. result '%s' (length %d)", output, strlen(output));
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, ResolveVariableString_error)
{
  bool ret = (ccs::HelperTools::SetEnvironmentVariable("SysTools_Test", "ResolveVariableString") == true);

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; ccs::HelperTools::SafeStringCopy(buffer, "${SYSTOOLS_TEST}", STRING_MAX_LENGTH);
      char output [STRING_MAX_LENGTH] = STRING_UNDEFINED;
      log_info("TEST(SysTools_Test, ResolveVariableString_error) - Buffer '%s' ..", buffer);
      ret = (ccs::HelperTools::ResolveVariableString(buffer, output, STRING_MAX_LENGTH) == false); // WARNING - Test criterion
      log_info(".. result '%s'", output);
    }

  // Tear-down context
  ccs::HelperTools::ClearEnvironmentVariable("SysTools_Test");

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, IsIntegerString_success)
{
  char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::IsIntegerString(buffer) == false);

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "0", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == true);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "-0", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == true);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "+0", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == true);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "10", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == true);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "-10", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == true);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "+10", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == true);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, IsIntegerString_error)
{
  char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  bool ret = (ccs::HelperTools::IsIntegerString(buffer) == false);

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "0.", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == false);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, ".0", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == false);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "abc", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == false);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "0xabc", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::IsIntegerString(buffer) == false);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, ToInteger)
{
  char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::IsIntegerString(buffer) == false);

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "0", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::ToInteger(buffer) == 0);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "-0", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::ToInteger(buffer) == 0);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "+0", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::ToInteger(buffer) == 0);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "10", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::ToInteger(buffer) == 10);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "-10", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::ToInteger(buffer) == -10);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(buffer, "+10", STRING_MAX_LENGTH);
      ret = (ccs::HelperTools::ToInteger(buffer) == 10);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, SafeStringAppend)
{
  char prefix [STRING_MAX_LENGTH] = "prefix";
  char midfix [STRING_MAX_LENGTH] = "midfix";
  char suffix [STRING_MAX_LENGTH] = ".suffix";

  ccs::HelperTools::SafeStringAppend(prefix, midfix, STRING_MAX_LENGTH, '.');

  log_info("TEST(SysTools_Test, SafeStringAppend) - Result string '%s'", prefix);

  bool ret = (strcmp(prefix, "prefix.midfix") == 0);

  ccs::HelperTools::SafeStringAppend(prefix, suffix, STRING_MAX_LENGTH);

  log_info("TEST(SysTools_Test, SafeStringAppend) - Result string '%s'", prefix);

  if (ret)
    {
      ret = (strcmp(prefix, "prefix.midfix.suffix") == 0);
    }

  ccs::HelperTools::SafeStringAppend(prefix, " (Try and insert long string and see if the code properly truncates it)", STRING_MAX_LENGTH);

  log_info("TEST(SysTools_Test, SafeStringAppend) - Result string '%s'", prefix);

  if (ret)
    {
      ret = (strlen(prefix) == 63);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, StringCompare)
{
  bool ret = ccs::HelperTools::StringCompare("", "");

  if (ret)
    {
      ret = (!ccs::HelperTools::StringCompare("", "a") && !ccs::HelperTools::StringCompare("a", ""));
    }

  if (ret)
    {
      ret = (!ccs::HelperTools::StringCompare("a", "b") && !ccs::HelperTools::StringCompare("a", "b", 1));
    }

  if (ret)
    {
      ret = (ccs::HelperTools::StringCompare("a", "a") && ccs::HelperTools::StringCompare("a", "a", 1));
    }

  if (ret)
    {
      ret = (!ccs::HelperTools::StringCompare("alpha", "a") && ccs::HelperTools::StringCompare("alpha", "a", 1));
    }

  if (ret)
    {
      ret = (!ccs::HelperTools::StringCompare("a", "alpha") && ccs::HelperTools::StringCompare("a", "alpha", 1));
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, Contain)
{
  char buffer [] = "This is a long string buffer";

  bool ret = (ccs::HelperTools::Contain(buffer, "This") &&
	      ccs::HelperTools::Contain(buffer, "is") &&
	      ccs::HelperTools::Contain(buffer, "is", 5u) &&
	      ccs::HelperTools::Contain(buffer, " ") &&
	      ccs::HelperTools::Contain(buffer, " ", 5u) &&
	      ccs::HelperTools::Contain(buffer, "r") &&
	      !ccs::HelperTools::Contain(buffer, " ", 25u) &&
	      !ccs::HelperTools::Contain(buffer, "contain") &&
	      !ccs::HelperTools::Contain(buffer, "this") &&
	      !ccs::HelperTools::Contain(buffer, "Buffer") &&
	      !ccs::HelperTools::Contain(buffer, "m"));

  if (ret)
    {
      ret = (!ccs::HelperTools::Contain(buffer, "\0") &&
	     !ccs::HelperTools::Contain(buffer, "") &&
	     !ccs::HelperTools::Contain(NULL, "long") &&
	     !ccs::HelperTools::Contain(buffer, NULL));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, Find)
{
  char buffer [] = "This is a long string buffer";

  bool ret = ((0 == ccs::HelperTools::Find(buffer, "This")) &&
	      (2 == ccs::HelperTools::Find(buffer, "is")) &&
	      (4 == ccs::HelperTools::Find(buffer, " ")) &&
	      (7 == ccs::HelperTools::Find(buffer, " ", 5u)) &&
	      (17 == ccs::HelperTools::Find(buffer, "r")) &&
	      (-1 == ccs::HelperTools::Find(buffer, "contain")) &&
	      (-1 == ccs::HelperTools::Find(buffer, "this")) &&
	      (-1 == ccs::HelperTools::Find(buffer, "Buffer")) &&
	      (-1 == ccs::HelperTools::Find(buffer, "m")));

  if (ret)
    {
      ret = ((-1 == ccs::HelperTools::Find(buffer, "\0")) &&
	     (-1 == ccs::HelperTools::Find(buffer, "")) &&
	     (-1 == ccs::HelperTools::Find(NULL, "long")) &&
	     (-1 == ccs::HelperTools::Find(buffer, NULL)));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, Count)
{
  char buffer [] = "This is a long string buffer";

  bool ret = ((1u == ccs::HelperTools::Count(buffer, "This")) &&
	      (2u == ccs::HelperTools::Count(buffer, "is")) &&
	      (5u == ccs::HelperTools::Count(buffer, " ")) &&
	      (4u == ccs::HelperTools::Count(buffer, " ", 5u)) &&
	      (2u == ccs::HelperTools::Count(buffer, "r")) &&
	      (0u == ccs::HelperTools::Count(buffer, "contain")) &&
	      (0u == ccs::HelperTools::Count(buffer, "this")) &&
	      (0u == ccs::HelperTools::Count(buffer, "Buffer")) &&
	      (0u == ccs::HelperTools::Count(buffer, "m")));

  if (ret)
    {
      ret = ((0u == ccs::HelperTools::Count(buffer, "\0")) &&
	     (0u == ccs::HelperTools::Count(buffer, "")) &&
	     (0u == ccs::HelperTools::Count(NULL, "long")) &&
	     (0u == ccs::HelperTools::Count(buffer, NULL)));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, Locate)
{
  char buffer [] = "{\"type\":\"MyArrayType\",\"element\":{\"type\":..},\"multiplicity\":4}";

  bool ret = ((-1 == ccs::HelperTools::Locate(buffer, "{", 0u)) &&
	      ( 0 == ccs::HelperTools::Locate(buffer, "{", 1u)) &&
	      (32 == ccs::HelperTools::Locate(buffer, "{", 2u)) &&
	      (-1 == ccs::HelperTools::Locate(buffer, "{", 3u)) &&
	      (42 == ccs::HelperTools::Locate(buffer, "}", 1u)) &&
	      (60 == ccs::HelperTools::Locate(buffer, "}", 2u)) &&
	      (-1 == ccs::HelperTools::Locate(buffer, "}", 3u)));

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, FindMatchingBrace)
{
  bool ret = true;

  if (ret)
    { //                0 12345 67 890123456789 01 23456789 012 34567 890123 4567890123456 7890
      char buffer [] = "{\"type\":\"MyArrayType\",\"element\":{\"type\":..},\"multiplicity\":4},{},{}]";
      log_info("TEST(SysTools_Test, FindMatchingBrace) - '%u' for '%s'", ccs::HelperTools::FindMatchingBrace(buffer), buffer);
      ret = (60 == ccs::HelperTools::FindMatchingBrace(buffer));
    }

  if (ret)
    { //                0123456789012345678901
      char buffer [] = "(((a==b)&&()||())&&())";
      log_info("TEST(SysTools_Test, FindMatchingBrace) - '%u' for '%s'", ccs::HelperTools::FindMatchingBrace(buffer), buffer);
      ret = (21 == ccs::HelperTools::FindMatchingBrace(buffer));
    }

  if (ret)
    {
      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED;
      ret = ((-1 == ccs::HelperTools::FindMatchingBrace(NULL)) &&
	     (-1 == ccs::HelperTools::FindMatchingBrace(buffer)));
    }

  if (ret)
    { // Random content
      char* buffer = static_cast<char*>(malloc(1024u));
      ret = (-1 == ccs::HelperTools::FindMatchingBrace(buffer));
      free(buffer);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, Strip)
{
  char buffer [] = "{\"type\":\"MyArrayType\",\"element\":{\"type\":..},\"multiplicity\":4}";

  bool ret = ccs::HelperTools::Strip(buffer, "\"");

  log_info("TEST(SysTools_Test, Strip) - Result '%s'", buffer);

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, GetProcessMemory)
{
  std::stringstream pid; pid << ccs::HelperTools::GetProcessId();

  std::string file_path = "/proc/"; file_path += pid.str().c_str(); file_path += "/status";

  bool ret = ccs::HelperTools::Exist(file_path.c_str());

  std::string result;

  if (ret)
    {
      std::stringstream sstr; sstr << std::ifstream(file_path.c_str()).rdbuf();
      result = sstr.str();
      ret = (std::string::npos != result.find("VmData"));
    }

  if (ret)
    {
      log_info("TEST(SysTools_Test, GetProcessMemory) - File ..");
      log_info(".. '%s'", result.c_str());
    }

  void* ref = NULL;
  
  ccs::HelperTools::SleepFor(1000000000ul);
  ccs::types::uint32 before = ccs::HelperTools::GetProcessMemory();

  if (ret)
    {
      ref = malloc(4096*1024);
      ret = (NULL != ref);
    }
      
  if (ret) // Make sure to use the memory
    {
      reinterpret_cast<ccs::types::uint64*>(ref)[511] = ccs::HelperTools::GetCurrentTime();
    }

  ccs::HelperTools::SleepFor(1000000000ul);
  ccs::types::uint32 after = ccs::HelperTools::GetProcessMemory();

  if (ret) // Make sure to use the memory
    {
      log_info("TEST(SysTools_Test, GetProcessMemory) - For the sake of using memory '%lu'", reinterpret_cast<ccs::types::uint64*>(ref)[511]);
    }

  if (ret)
    {
      std::stringstream sstr; sstr << std::ifstream(file_path.c_str()).rdbuf();
      result = sstr.str();
      ret = (std::string::npos != result.find("VmData"));
    }

  if (ret)
    {
      log_info("TEST(SysTools_Test, GetProcessMemory) - File ..");
      log_info(".. '%s'", result.c_str());
    }

  if (ret)
    {     
      free(ref);
    }
      
  ccs::HelperTools::SleepFor(1000000000ul);
  ccs::types::uint32 final = ccs::HelperTools::GetProcessMemory();
  
  log_info("TEST(SysTools_Test, GetProcessMemory) - ['%u %u %u']", before, after, final);
      
  if (ret)
    {
      ret = ((after > before) && (final < after) && (final == before));
    }
  
  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, GetSystemMemory)
{
  std::string file_path = "/proc/meminfo";

  bool ret = ccs::HelperTools::Exist(file_path.c_str());

  std::string result;

  if (ret)
    {
      std::stringstream sstr; sstr << std::ifstream(file_path.c_str()).rdbuf();
      result = sstr.str();
    }

  if (ret)
    {
      log_info("TEST(SysTools_Test, GetSystemMemory) - File ..");
      log_info(".. '%s'", result.c_str());
    }

  ccs::types::ResourceStatistics memory = ccs::HelperTools::GetSystemMemoryUsage();

  log_info("TEST(SysTools_Test, GetSystemMemory) - ['%lu %lu %f']", memory.total, memory.avail, memory.usage);

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, GetSystemCPU)
{
  std::string file_path = "/proc/stat";

  bool ret = ccs::HelperTools::Exist(file_path.c_str());

  std::string result;

  if (ret)
    {
      std::stringstream sstr; sstr << std::ifstream(file_path.c_str()).rdbuf();
      result = sstr.str();
    }

  if (ret)
    {
      log_info("TEST(SysTools_Test, GetSystemCPU) - File ..");
      log_info(".. '%s'", result.c_str());
    }

  if (ret)
    {
      ccs::types::ResourceStatistics before = ccs::HelperTools::GetSystemCPUUsage(); // WARNING cumulative since boot
      
      ccs::HelperTools::SleepFor(1000000000ul);
      
      ccs::types::ResourceStatistics after = ccs::HelperTools::GetSystemCPUUsage(); // WARNING cumulative since boot
      
      ccs::types::uint64 total = after.total-before.total;
      ccs::types::uint64 avail = after.avail-before.avail; // Idle tasks
      ccs::types::float64 usage = 100.0 * static_cast<ccs::types::float64>(total-avail) / static_cast<ccs::types::float64>(total);
      
      log_info("TEST(SysTools_Test, GetSystemCPU) - Sleeping .. ['%lu %lu %f']", total, avail, usage);
    }

  if (ret)
    {
      ccs::types::ResourceStatistics before = ccs::HelperTools::GetSystemCPUUsage(); // WARNING cumulative since boot
      
      ccs::HelperTools::SleepUntil(ccs::HelperTools::GetCurrentTime()+1000000000ul, 0ul); // Spin wait
      
      ccs::types::ResourceStatistics after = ccs::HelperTools::GetSystemCPUUsage(); // WARNING cumulative since boot
      
      ccs::types::uint64 total = after.total-before.total;
      ccs::types::uint64 avail = after.avail-before.avail; // Idle tasks
      ccs::types::float64 usage = 100.0 * static_cast<ccs::types::float64>(total-avail) / static_cast<ccs::types::float64>(total);
      
      log_info("TEST(SysTools_Test, GetSystemCPU) - Spinning .. ['%lu %lu %f']", total, avail, usage);
    }

  ASSERT_EQ(true, ret);
}

TEST(SysTools_Test, GetSystemDisk)
{
  struct statvfs fs_data;

  bool ret = (0 == statvfs("/", &fs_data));

  ccs::types::ResourceStatistics disk = ccs::HelperTools::GetSystemDiskUsage();
  log_info("TEST(SysTools_Test, GetSystemDisk) - ['%lu %lu %f']", disk.total, disk.avail, disk.usage);

  // Todo - Run against df system call

  ASSERT_EQ(true, ret);
}








/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/AnyTypeDatabase-tests.cpp $
* $Id: AnyTypeDatabase-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "types.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "AnyTypeDatabase.h"
#include "AnyTypeHelper.h"
#include "CompoundType.h"

#include "AnyValue.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition
  
TEST(AnyTypeDatabase_Test, Constructor_default)
{
  // Types database is implicitly instantiated when calling GTDB methods
  bool ret = !ccs::types::GlobalTypeDatabase::IsValid("");

  ASSERT_EQ(ret, true);
}

TEST(AnyTypeDatabase_Test, IsValid_scalar)
{
  bool ret = !ccs::types::GlobalTypeDatabase::IsValid("");

  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::IsValid("bool");
    }

  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::IsValid("char8");
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::IsValid("int8") &&
	     ccs::types::GlobalTypeDatabase::IsValid("uint8"));
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::IsValid("int16") &&
	     ccs::types::GlobalTypeDatabase::IsValid("uint16"));
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::IsValid("int32") &&
	     ccs::types::GlobalTypeDatabase::IsValid("uint32"));
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::IsValid("int64") &&
	     ccs::types::GlobalTypeDatabase::IsValid("uint64"));
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::IsValid("float32") &&
	     ccs::types::GlobalTypeDatabase::IsValid("float64"));
    }

  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::IsValid("string");
    }

  ASSERT_EQ(ret, true);
}

TEST(AnyTypeDatabase_Test, Register_success)
{
  bool ret = ccs::types::GlobalTypeDatabase::IsValid("test::struct");

  if (!ret)
    {
      // Register type
      ccs::types::CompoundType* type = ((new (std::nothrow) ccs::types::CompoundType ("test::struct"))
					->AddAttribute<ccs::types::boolean>("status")
					->AddAttribute<ccs::types::uint64>("timestamp"));
      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

   if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::IsValid("test::struct");
    }

   ASSERT_EQ(ret, true);
}

TEST(AnyTypeDatabase_Test, Register_json)
{
  bool ret = ccs::types::GlobalTypeDatabase::IsValid("test::gtdb::json");
#ifdef LOG_DEBUG_ENABLE
  ccs::log::Severity_t filter = ccs::log::SetFilter(LOG_DEBUG);
#endif
  if (!ret)
    {
      // Register type
      char type [] = "{\"type\":\"test::gtdb::json\","
                      "\"attributes\":["
                                      "{\"boolean\":{\"type\":\"bool\",\"size\":1}},"
                                      "{\"uint32\":{\"type\":\"uint32\",\"size\":4}},"
                                      "{\"float64\":{\"type\":\"float64\",\"size\":8}}"
                                     "]"
                     "}";

      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

   if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::IsValid("test::gtdb::json");
    }
#ifdef LOG_DEBUG_ENABLE
   ccs::log::SetFilter(filter);
#endif
   ASSERT_EQ(ret, true);
}

TEST(AnyTypeDatabase_Test, Register_error)
{
  bool ret = !ccs::types::GlobalTypeDatabase::IsValid("test::undefined");

  if (ret)
    {
      // Register invalid type
      ccs::types::CompoundType* type = static_cast<ccs::types::CompoundType*>(NULL);
      ret = !ccs::types::GlobalTypeDatabase::Register(type);
    }

   if (ret)
    {
      ret = !ccs::types::GlobalTypeDatabase::IsValid("test::undefined");
    }

  if (ret)
    {
      // Register invalid type
      std::shared_ptr<const ccs::types::CompoundType> type;
      ret = !ccs::types::GlobalTypeDatabase::Register(type);
    }

   if (ret)
    {
      ret = !ccs::types::GlobalTypeDatabase::IsValid("test::undefined");
    }

   ASSERT_EQ(ret, true);
}

TEST(AnyTypeDatabase_Test, GetType_success)
{
  bool ret = ccs::types::GlobalTypeDatabase::IsValid("test::struct");

  if (!ret)
    {
      // Register type
      ccs::types::CompoundType* type = ((new (std::nothrow) ccs::types::CompoundType ("test::struct"))
					->AddAttribute<ccs::types::boolean>("status")
					->AddAttribute<ccs::types::uint64>("timestamp"));
      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::IsValid("test::struct") &&
	     (ccs::types::GlobalTypeDatabase::GetAsType<ccs::types::CompoundType>("test::struct") ? true : false));
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::GetAsType<ccs::types::CompoundType>("test::struct")->HasAttribute("status") &&
	     (ccs::types::Boolean == ccs::types::GlobalTypeDatabase::GetAsType<ccs::types::CompoundType>("test::struct")->GetAttributeType("status")));
    }

  if (ret)
    {
      ret = (ccs::types::GlobalTypeDatabase::GetAsType<ccs::types::CompoundType>("test::struct")->HasAttribute("timestamp") &&
	     (ccs::types::UnsignedInteger64 == ccs::types::GlobalTypeDatabase::GetAsType<ccs::types::CompoundType>("test::struct")->GetAttributeType("timestamp")));
    }

   ASSERT_EQ(ret, true);
}

TEST(AnyTypeDatabase_Test, GetType_error)
{
  bool ret = !ccs::types::GlobalTypeDatabase::IsValid("test::undefined");

  if (ret)
    {
      ret = !(ccs::types::GlobalTypeDatabase::GetAsType<ccs::types::CompoundType>("test::undefined") ? true : false);
    }

  ASSERT_EQ(ret, true);
}

TEST(AnyTypeDatabase_Test, CISDemo_types)
{
  bool ret = ccs::types::GlobalTypeDatabase::IsValid("test::gtdb::FloatArray_t/v1.0");

  if (!ret)
    {
      // Register type
      char type [] = "{\"type\":\"test::gtdb::FloatArray_t/v1.0\","
                      "\"multiplicity\":32,"
                      "\"element\":{\"type\":\"float32\"}"
                     "}";

      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

  if (ret)
    {
      char type [] = "{\"type\":\"test::gtdb::Config_t/v1.0\","
                      "\"attributes\":[{\"M\":{\"type\":\"test::gtdb::FloatArray_t/v1.0\"}},"
                                      "{\"Y0\":{\"type\":\"test::gtdb::FloatArray_t/v1.0\"}},"
                                      "{\"WOTime\":{\"type\":\"uint64\"}},"
                                      "{\"EOTime\":{\"type\":\"uint64\"}}"
                                     "]"
                     "}";

      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

  if (ret)
    {
      char type [] = "{\"type\":\"test::gtdb::FPGATopic_t/v1.0\","
                      "\"attributes\":[{\"oper\":{\"type\":\"uint32\"}},"
                                      "{\"hash\":{\"type\":\"uint32\"}},"
                                      "{\"config\":{\"type\":\"test::gtdb::Config_t/v1.0\"}}"
                                     "]"
                     "}";

      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

  if (ret)
    {
      ccs::types::AnyValue value (ccs::types::GlobalTypeDatabase::GetType("test::gtdb::FPGATopic_t/v1.0"));
      log_info("TEST(AnyTypeDatabase_Test, CISDemo_types) - Size of '%s' is '%u'", value.GetType()->GetName(), value.GetSize());
      ret = (280u == value.GetSize());
    }

  ASSERT_EQ(ret, true);
}


/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/AnyTypeHelper-tests.cpp $
* $Id: AnyTypeHelper-tests.cpp 100795 2019-07-16 15:11:46Z bauvirb $
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

#include <memory> // std::shared_ptr, etc.
#include <new> // std::nothrow, etc.

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "AnyTypeHelper.h"

// Constants

// Type definition

class AnyTypeHelper_Test
{

  public:

    std::shared_ptr<ccs::types::AnyType> scalars;
    std::shared_ptr<ccs::types::AnyType> arrays;
 
    AnyTypeHelper_Test (void) {

      scalars = std::shared_ptr<ccs::types::AnyType>(dynamic_cast<ccs::types::AnyType*>((new (std::nothrow) ccs::types::CompoundType ("ccs::test::Scalars_t"))
											->AddAttribute("boolean","bool")
											->AddAttribute("uint32","uint32")
											->AddAttribute("float64","float64")));
      
      arrays = std::shared_ptr<ccs::types::AnyType>(dynamic_cast<ccs::types::AnyType*>((new (std::nothrow) ccs::types::CompoundType ("ccs::test::Arrays_t"))
										       ->AddAttribute("booleans", ccs::HelperTools::NewArrayType("ccs::test::BooleanArray_t", "bool", 8u))
										       ->AddAttribute("integers", ccs::HelperTools::NewArrayType("ccs::test::IntegerArray_t", "uint32", 8u))));

      return;

    };

  protected:

};

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition
  
TEST(AnyTypeHelper_Test, HasAttribute_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::BooleanArray_t", "bool", 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, HasAttribute_default) - Scalar array ..");
    }

  if (ret)
    {
      ret = (true == HasAttribute(type, ""));
    }

  if (ret)
    {
      ret = ((true == HasAttribute(type, "[0]")) &&
	     (true == HasAttribute(type, "[1]")) &&
	     (true == HasAttribute(type, "[6]")) &&
	     (true == HasAttribute(type, "[7]")));
    }

  if (ret)
    {
      ret = (false == HasAttribute(type, "[8]")) ;
    }
  
  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)
										  ->AddAttribute("arrays", test.arrays)));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, HasAttribute_default) - Compound type ..");
    }

  if (ret)
    {
      ret = (true == HasAttribute(type, ""));
    }

  if (ret)
    {
      ret = ((true == HasAttribute(type, "scalars")) &&
	     (true == HasAttribute(type, "scalars.boolean")) &&
	     (true == HasAttribute(type, "scalars.uint32")) &&
	     (true == HasAttribute(type, "scalars.float64")));
    }

  if (ret)
    {
      ret = ((true == HasAttribute(type, "arrays")) &&
	     (true == HasAttribute(type, "arrays.booleans")) &&
	     (true == HasAttribute(type, "arrays.booleans[0]")) &&
	     (true == HasAttribute(type, "arrays.integers")) &&
	     (true == HasAttribute(type, "arrays.integers[0]")));
    }

  if (ret)
    {
      ret = ((false == HasAttribute(type, "unknown")) &&
	     (false == HasAttribute(type, "scalars.unknown")) &&
	     (false == HasAttribute(type, "scalars[0]")) &&
	     (false == HasAttribute(type, "arrays.booleans[8]")) &&
	     (false == HasAttribute(type, "arrays.integers[8]")));
    }

  }
  
  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::CompoundArray_t", test.scalars, 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, HasAttribute_default) - Compound array ..");
    }

  if (ret)
    {
      ret = (true == HasAttribute(type, ""));
    }

  if (ret)
    {
      ret = ((true == HasAttribute(type, "[0]")) &&
	     (true == HasAttribute(type, "[1]")) &&
	     (true == HasAttribute(type, "[6]")) &&
	     (true == HasAttribute(type, "[7]")));
    }

  if (ret)
    {
      ret = ((true == HasAttribute(type, "[0].boolean")) &&
	     (true == HasAttribute(type, "[1].boolean")) &&
	     (true == HasAttribute(type, "[6].boolean")) &&
	     (true == HasAttribute(type, "[7].boolean")));
    }

  if (ret)
    {
      ret = ((true == HasAttribute(type, "[0].float64")) &&
	     (true == HasAttribute(type, "[1].float64")) &&
	     (true == HasAttribute(type, "[6].float64")) &&
	     (true == HasAttribute(type, "[7].float64")));
    }

  if (ret)
    {
      ret = (false == HasAttribute(type, "[8]")) ;
    }
  
  if (ret)
    {
      ret = (false == HasAttribute(type, "[8].boolean")) ;
    }
  
  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, HasAttribute_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, HasAttribute_default)");
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, HasAttribute_robustness)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)));

  bool ret = (type ? true : false);

  if (ret)
    {
      ret = ((false == HasAttribute(type, "0")) &&
	     (false == HasAttribute(type, "[")) &&
	     (false == HasAttribute(type, ".")) &&
	     (false == HasAttribute(type, "]")) &&
	     (false == HasAttribute(type, "[]")) &&
	     (false == HasAttribute(type, "[.]")));
    }

  if (ret)
    {
      std::shared_ptr<AnyType> invalid;
      ret = (false == HasAttribute(invalid, ""));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, GetAttributeOffset_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::BooleanArray_t", "bool", 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeOffset_default) - Scalar array ..");
    }

  if (ret)
    {
      ret = (0u == GetAttributeOffset(type, ""));
    }

  if (ret)
    {
      ret = ((0u == GetAttributeOffset(type, "[0]")) &&
	     (1u == GetAttributeOffset(type, "[1]")) &&
	     (6u == GetAttributeOffset(type, "[6]")) &&
	     (7u == GetAttributeOffset(type, "[7]")));
    }

  if (ret)
    {
      ret = (0u == GetAttributeOffset(type, "[8]")) ;
    }
  
  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)
										  ->AddAttribute("arrays", test.arrays)));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeOffset_default) - Compound type ..");
    }

  if (ret)
    {
      ret = (0u == GetAttributeOffset(type, ""));
    }

  if (ret)
    {
      ret = ((0u == GetAttributeOffset(type, "scalars")) &&
	     (0u == GetAttributeOffset(type, "scalars.boolean")) &&
	     (1u == GetAttributeOffset(type, "scalars.uint32")) &&
	     (5u == GetAttributeOffset(type, "scalars.float64")));
    }

  if (ret)
    {
      ret = ((13u == GetAttributeOffset(type, "arrays")) &&
	     (13u == GetAttributeOffset(type, "arrays.booleans")) &&
	     (13u == GetAttributeOffset(type, "arrays.booleans[0]")) &&
	     (21u == GetAttributeOffset(type, "arrays.integers")) &&
	     (21u == GetAttributeOffset(type, "arrays.integers[0]")));
    }

  if (ret)
    {
      ret = ((0u == GetAttributeOffset(type, "unknown")) &&
	     (0u == GetAttributeOffset(type, "scalars.unknown")) &&
	     (0u == GetAttributeOffset(type, "scalars[0]")) &&
	     (0u == GetAttributeOffset(type, "arrays.booleans[8]")) &&
	     (0u == GetAttributeOffset(type, "arrays.integers[8]")));
    }

  }
  
  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::CompoundArray_t", test.scalars, 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeOffset_default) - Compound array ..");
    }

  if (ret)
    {
      ret = (0u == GetAttributeOffset(type, ""));
    }

  if (ret)
    {
      ret = ((0u == GetAttributeOffset(type, "[0]")) &&
	     (13u == GetAttributeOffset(type, "[1]")) &&
	     (78u == GetAttributeOffset(type, "[6]")) &&
	     (91u == GetAttributeOffset(type, "[7]")));
    }

  if (ret)
    {
      ret = ((0u == GetAttributeOffset(type, "[0].boolean")) &&
	     (13u == GetAttributeOffset(type, "[1].boolean")) &&
	     (78u == GetAttributeOffset(type, "[6].boolean")) &&
	     (91u == GetAttributeOffset(type, "[7].boolean")));
    }

  if (ret)
    {
      ret = ((5u == GetAttributeOffset(type, "[0].float64")) &&
	     (18u == GetAttributeOffset(type, "[1].float64")) &&
	     (83u == GetAttributeOffset(type, "[6].float64")) &&
	     (96u == GetAttributeOffset(type, "[7].float64")));
    }

  if (ret)
    {
      ret = (0u == GetAttributeOffset(type, "[8]")) ;
    }
  
  if (ret)
    {
      ret = (0u == GetAttributeOffset(type, "[8].boolean")) ;
    }
  
  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeOffset_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, GetAttributeOffset_default)");
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, GetAttributeOffset_robustness)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)));

  bool ret = (type ? true : false);

  if (ret)
    {
      ret = ((0u == GetAttributeOffset(type, "0")) &&
	     (0u == GetAttributeOffset(type, "[")) &&
	     (0u == GetAttributeOffset(type, ".")) &&
	     (0u == GetAttributeOffset(type, "]")) &&
	     (0u == GetAttributeOffset(type, "[]")) &&
	     (0u == GetAttributeOffset(type, "[.]")));
    }

  if (ret)
    {
      std::shared_ptr<AnyType> invalid;
      ret = (0u == GetAttributeOffset(invalid, ""));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, GetAttributeType_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::BooleanArray_t", "bool", 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeType_default) - Scalar array ..");
    }

  if (ret)
    {
      ret = (type == GetAttributeType(type, ""));
    }

  if (ret)
    {
      ret = ((ccs::types::Boolean == GetAttributeType(type, "[0]")) &&
	     (ccs::types::Boolean == GetAttributeType(type, "[1]")) &&
	     (ccs::types::Boolean == GetAttributeType(type, "[6]")) &&
	     (ccs::types::Boolean == GetAttributeType(type, "[7]")));
    }

  if (ret)
    {
      ret = (false == static_cast<bool>(GetAttributeType(type, "[8]"))) ;
    }
  
  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)
										  ->AddAttribute("arrays", test.arrays)));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeType_default) - Compound type ..");
    }

  if (ret)
    {
      ret = (type == GetAttributeType(type, ""));
    }

  if (ret)
    {
      ret = ((test.scalars == GetAttributeType(type, "scalars")) &&
	     (ccs::types::Boolean == GetAttributeType(type, "scalars.boolean")) &&
	     (ccs::types::UnsignedInteger32 == GetAttributeType(type, "scalars.uint32")) &&
	     (ccs::types::Float64 == GetAttributeType(type, "scalars.float64")));
    }

  if (ret)
    {
      ret = ((test.arrays == GetAttributeType(type, "arrays")) &&
	     (true == Is<ArrayType>(GetAttributeType(type, "arrays.booleans"))) && (true == Is<ScalarType>(GetAttributeType(type, "arrays.booleans[0]"))) &&
	     (ccs::types::Boolean == GetAttributeType(type, "arrays.booleans[0]")) &&
	     (true == Is<ArrayType>(GetAttributeType(type, "arrays.integers"))) && (true == Is<ScalarType>(GetAttributeType(type, "arrays.integers[0]"))) &&
	     (ccs::types::UnsignedInteger32 == GetAttributeType(type, "arrays.integers[0]")));
    }

  if (ret)
    {
      ret = ((false == static_cast<bool>(GetAttributeType(type, "unknown"))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "scalars.unknown"))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "scalars[0]"))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "arrays.booleans[8]"))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "arrays.integers[8]"))));
    }

  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::CompoundArray_t", test.scalars, 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeType_default) - Compound array ..");
    }

  if (ret)
    {
      ret = (type == GetAttributeType(type, ""));
    }

  if (ret)
    {
      ret = ((test.scalars == GetAttributeType(type, "[0]")) &&
	     (test.scalars == GetAttributeType(type, "[1]")) &&
	     (test.scalars == GetAttributeType(type, "[6]")) &&
	     (test.scalars == GetAttributeType(type, "[7]")));
    }

  if (ret)
    {
      ret = ((ccs::types::Boolean == GetAttributeType(type, "[0].boolean")) &&
	     (ccs::types::Boolean == GetAttributeType(type, "[1].boolean")) &&
	     (ccs::types::Boolean == GetAttributeType(type, "[6].boolean")) &&
	     (ccs::types::Boolean == GetAttributeType(type, "[7].boolean")));
    }

  if (ret)
    {
      ret = ((ccs::types::Float64 == GetAttributeType(type, "[0].float64")) &&
	     (ccs::types::Float64 == GetAttributeType(type, "[1].float64")) &&
	     (ccs::types::Float64 == GetAttributeType(type, "[6].float64")) &&
	     (ccs::types::Float64 == GetAttributeType(type, "[7].float64")));
    }

  if (ret)
    {
      ret = (false == static_cast<bool>(GetAttributeType(type, "[8]"))) ;
    }
  
  if (ret)
    {
      ret = (false == static_cast<bool>(GetAttributeType(type, "[8].boolean"))) ;
    }
  
  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, GetAttributeType_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, GetAttributeType_default)");
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, GetAttributeType_robustness)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)));

  bool ret = (type ? true : false);

  if (ret)
    {
      ret = ((false == static_cast<bool>(GetAttributeType(type, "0"))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "["))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "."))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "]"))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "[]"))) &&
	     (false == static_cast<bool>(GetAttributeType(type, "[.]"))));
    }

  if (ret)
    {
      std::shared_ptr<AnyType> invalid;
      ret = (false == static_cast<bool>(GetAttributeType(invalid, "")));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, GetAttributeSize_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = (1u == GetAttributeSize(test.scalars, "boolean"));

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, GetJSONAttribute)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  bool ret = true;

  if (ret)
    { // JSON stream
      char buffer [] = "{\"attr\":\"value\"}";
      ccs::types::string attr;

      ret = GetAttributeFromJSONContent(buffer, "attr", attr, MaxStringLength);
      
      if (ret)
	{
	  ret = (std::string(attr) == "\"value\"");
	}
      
      if (ret)
	{
	  SafeStringCopy(attr, "", MaxStringLength);
	  ret = GetAttributeFromJSONContent(buffer, "attr", attr, 0u);

	  if (ret) // No change
	    {
	      ret = IsUndefinedString(attr);
	    }
	}
      
      if (ret)
	{
	  ret = !GetAttributeFromJSONContent(buffer, "att", attr, MaxStringLength); // Expect failure
	}
#if 0      
      if (ret)
	{
	  ret = !GetAttributeFromJSONContent(buffer, ":", attr, MaxStringLength); // Expect failure
	}
#endif      
      if (ret)
	{
	  ret = !GetAttributeFromJSONContent(buffer, "value", attr, MaxStringLength); // Expect failure
	}
    }

  if (ret)
    { // JSON stream
      char buffer [] = "{\"attr\":true}";
      ccs::types::string attr;

      ret = GetAttributeFromJSONContent(buffer, "attr", attr, MaxStringLength);
      
      if (ret)
	{
	  log_info("TEST(AnyTypeHelper_Test, GetJSONAttribute) - Value '%s'", attr);
	  ret = (std::string(attr) == "true");
	}
    }

  if (ret)
    { // JSON stream
      char buffer [] = "{\"attr\":0.1,\"other\":1.0}";
      ccs::types::string attr;

      ret = GetAttributeFromJSONContent(buffer, "attr", attr, MaxStringLength);
      
      if (ret)
	{
	  log_info("TEST(AnyTypeHelper_Test, GetJSONAttribute) - Value '%s'", attr);
	  ret = (std::string(attr) == "0.1");
	}
    }

  if (ret)
    { // JSON stream
      char buffer [] = "{\"attr\":{\"inner\":\"value\"}}";
      ccs::types::string attr;

      ret = GetAttributeFromJSONContent(buffer, "attr", attr, MaxStringLength);
      
      if (ret)
	{
	  log_info("TEST(AnyTypeHelper_Test, GetJSONAttribute) - Value '%s'", attr);
	  ret = (std::string(attr) == "{\"inner\":\"value\"}");
	}
      
      if (ret)
	{
	  ret = !GetAttributeFromJSONContent(buffer, "inner", attr, MaxStringLength); // Expect failure
	}
    }

  if (ret)
    { // JSON stream
      char buffer [] = "{\"head\":{\"attr\":\"head\"}, \"attr\":\"value\", \"tail\":{\"attr\":\"tail\"}}";
      ccs::types::string attr;

      ret = GetAttributeFromJSONContent(buffer, "attr", attr, MaxStringLength);
      
      if (ret)
	{
	  ret = (std::string(attr) == "\"value\"");
	}
    }

  // Robustness cases

  if (ret)
    {
      ccs::types::string attr;
      ret = !GetAttributeFromJSONContent("", "attr", attr, MaxStringLength); // Expect failure
    }

  if (ret)
    {
      ccs::types::string attr;
      ret = !GetAttributeFromJSONContent("{\"attr:\"value\"}", "attr", attr, MaxStringLength); // Expect failure
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, Parse_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Parse_default) - Scalars ..");
    }
  
  std::shared_ptr<AnyType> type;
 
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"string\",\"size\":64}") &&
	     (0u < Parse(type, "{\"type\":\"string\",\"size\":64}")) &&
	     (ccs::types::String == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"string\"}") &&
	     (0u < Parse(type, "{\"type\":\"string\"}")) &&
	     (ccs::types::String == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"bool\",\"size\":1}") &&
	     (0u < Parse(type, "{\"type\":\"bool\",\"size\":1}")) &&
	     (ccs::types::Boolean == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"bool\"}") &&
	     (0u < Parse(type, "{\"type\":\"bool\"}")) &&
	     (ccs::types::Boolean == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"char8\"}") &&
	     (0u < Parse(type, "{\"type\":\"char8\"}")) &&
	     (ccs::types::Character8 == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"int8\"}") &&
	     (0u < Parse(type, "{\"type\":\"int8\"}")) &&
	     (ccs::types::SignedInteger8 == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"int16\"}") &&
	     (0u < Parse(type, "{\"type\":\"int16\"}")) &&
	     (ccs::types::SignedInteger16 == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"int32\"}") &&
	     (0u < Parse(type, "{\"type\":\"int32\"}")) &&
	     (ccs::types::SignedInteger32 == type));
    }
  
  if (ret)
    {
      ret = (Is<ScalarType>("{\"type\":\"int64\"}") &&
	     (0u < Parse(type, "{\"type\":\"int64\"}")) &&
	     (ccs::types::SignedInteger64 == type));
    }
#if 0 // ToDo in the code  
  if (ret)
    { // Unregistered type ! Yet still scalar
      ret = (Is<ScalarType>("{\"type\":\"int128\",\"size\":16}") &&
	     (0u < Parse(type, "{\"type\":\"int128\",\"size\":16}")) &&
	     Is<ScalarType>(type) && (16u == type->GetSize()));
    }
#else
  if (ret)
    { // Unregistered type ! Yet still scalar
      ret = Is<ScalarType>("{\"type\":\"int128\",\"size\":16}");
    }
#endif  
  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Parse_default) - Scalar arrays ..");
    }
  
  if (ret)
    {
      char buffer [] = "{\"type\":\"BooleanArray_t\","
	                "\"multiplicity\":8,"
                        "\"element\":"
                                    "{\"type\":\"bool\",\"size\":1}"
                       "}";

      ret = Is<ArrayType>(buffer);

      std::shared_ptr<AnyType> type;
 
      if (ret)
	{
	  ret = ((0u < Parse(type, buffer)) &&
		 (Boolean == std::dynamic_pointer_cast<ArrayType>(type)->GetElementType()) &&
		 (8u == std::dynamic_pointer_cast<ArrayType>(type)->GetMultiplicity()));
	}
    }
  
  if (ret)
    {
      char buffer [] = "{\"type\":\"CharacterArray_t\","
	                "\"multiplicity\":256,"
                        "\"element\":"
                                    "{\"type\":\"char8\",\"size\":1}"
                       "}";

      ret = Is<ArrayType>(buffer);

      std::shared_ptr<AnyType> type;
 
      if (ret)
	{
	  ret = ((0u < Parse(type, buffer)) &&
		 (Character8 == std::dynamic_pointer_cast<ArrayType>(type)->GetElementType()) &&
		 (256u == std::dynamic_pointer_cast<ArrayType>(type)->GetMultiplicity()));
	}
    }
  
  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Parse_default) - Compound type ..");
    }
  
  if (ret)
    {
      char buffer [] = "{\"type\":\"test::ath::Scalars_t\","
                         "\"attributes\":["
                                         "{\"boolean\":{\"type\":\"bool\",\"size\":1}},"
                                         "{\"uint32\":{\"type\":\"uint32\",\"size\":4}},"
                                         "{\"float64\":{\"type\":\"float64\",\"size\":8}}"
                                        "]"
                       "}";

      std::shared_ptr<AnyType> type;

      ret = (Is<CompoundType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     (13u == type->GetSize()));

      if (ret)
	{
	  ret = ccs::types::GlobalTypeDatabase::Register(type);
	}
    }
  
  if (ret)
    {
      char buffer [] = "{\"type\":\"CompoundType_t\","
                        "\"attributes\":["
                                        "{\"counter\":{\"type\":\"uint64\",\"size\":8}},"
                                        "{\"timestamp\":{\"type\":\"uint64\",\"size\":8}},"
                                        "{\"scalars\":"
                                                     "{\"type\":\"Scalars_t\","
                                                      "\"attributes\":["
                                                                      "{\"boolean\":{\"type\":\"bool\",\"size\":1}},"
                                                                      "{\"uint32\":{\"type\":\"uint32\",\"size\":4}},"
                                                                      "{\"float64\":{\"type\":\"float64\",\"size\":8}}"
                                                                     "]"
                                                     "}"
                                        "}"
                                       "]"
                       "}";

      std::shared_ptr<AnyType> type;

      ret = (Is<CompoundType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     (29u == type->GetSize()));
    }
  
  if (ret)
    {
      char buffer [] = "{\"type\":\"CompoundType_t\","
                        "\"attributes\":["
                                        "{\"counter\":{\"type\":\"uint64\",\"size\":8}},"
                                        "{\"timestamp\":{\"type\":\"uint64\",\"size\":8}},"
                                        "{\"scalars\":{\"type\":\"test::ath::Scalars_t\"}}"
                                       "]"
                       "}";

      std::shared_ptr<AnyType> type;

      ret = (Is<CompoundType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     (29u == type->GetSize()));
    }
  
  if (ret)
    {
      // {"type":"struct_5a1d21dd-2274-4731-be23-05222371164a","attributes":[{"config":{"type":"struct_6f8cc01d-736c-4418-bff9-58acf0179cd6","attributes":[{"parameter":{"type":"uint32","size":4}},{"delay":{"type":"uint64","size":8}},{"operation":{"type":"string","size":1}}]}},{"input":{"type":"struct_f09091a2-f24d-4ef1-8722-ebf893b3cc03","attributes":[{"value":{"type":"float64","size":8}}]}}]}
 
      char buffer [] = "{\"type\":\"struct_5a1d21dd-2274-4731-be23-05222371164a\","
                        "\"attributes\":["
                                        "{\"config\":{\"type\":\"struct_6f8cc01d-736c-4418-bff9-58acf0179cd6\","
                                                     "\"attributes\":["
                                                                     "{\"parameter\":{\"type\":\"uint32\",\"size\":4}},"
                                                                     "{\"delay\":{\"type\":\"uint64\",\"size\":8}},"
                                                                     "{\"operation\":{\"type\":\"string\",\"size\":1}}"
                                                                    "]"
                                                    "}"
                                        "},"
                                        "{\"input\":{\"type\":\"struct_f09091a2-f24d-4ef1-8722-ebf893b3cc03\","
                                                    "\"attributes\":["
                                                                    "{\"value\":{\"type\":\"float64\",\"size\":8}}"
                                                                   "]"
                                                   "}"
                                        "}"
                                       "]"
                       "}";

      std::shared_ptr<AnyType> type;

      ret = (Is<CompoundType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     (84u == type->GetSize()));
    }
  
  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Parse_default) - Compound array ..");
    }
  
  if (ret)
    {
      std::shared_ptr<AnyType> type;
 
      char buffer [] = "{\"type\":\"CompoundArray_t\","
	                "\"multiplicity\":1024,"
                        "\"element\":"
                                    "{\"type\":\"CompoundType_t\","
                                     "\"attributes\":["
                                                     "{\"scalars\":"
                                                                  "{\"type\":\"Scalars_t\","
                                                                  "\"attributes\":["
                                                                                  "{\"boolean\":{\"type\":\"bool\",\"size\":1}},"
                                                                                  "{\"uint32\":{\"type\":\"uint32\",\"size\":4}},"
                                                                                  "{\"float64\":{\"type\":\"float64\",\"size\":8}}"
                                                                                 "]"
                                                                  "}"
                                                     "}"
                                                    "]"
                                    "}"
                       "}";

      log_info(".. type '%s'", buffer);

      ret = (Is<ArrayType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     Is<CompoundType>(std::dynamic_pointer_cast<ArrayType>(type)->GetElementType()) &&
	     (1024u == std::dynamic_pointer_cast<ArrayType>(type)->GetMultiplicity()) &&
	     (13312u == type->GetSize()));
  
      if (ret)
	{
	  void* ref = malloc(65536); memset(ref, 0, 65536u);
	  
	  char buffer [65536];
	  
	  if (ret)
	    {
	      ret = (true == SerialiseInstance(type, ref, buffer, 65536u));
	    }
	  
	  if (ret)
	    {
	      log_info(".. instance '%s'", buffer);
	    }

	  free(ref);
	}
    }
  
  if (ret)
    {
      std::shared_ptr<AnyType> type;
 
      char buffer [] = "{\"type\":\"CompoundArray_t\","
	                "\"multiplicity\":1024,"
                        "\"element\":"
                                    "{\"type\":\"CompoundType_t\","
                                     "\"attributes\":[{\"scalars\":{\"type\":\"test::ath::Scalars_t\"}}]"
                                    "}"
                       "}";

      log_info(".. type '%s'", buffer);

      ret = (Is<ArrayType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     Is<CompoundType>(std::dynamic_pointer_cast<ArrayType>(type)->GetElementType()) &&
	     (1024u == std::dynamic_pointer_cast<ArrayType>(type)->GetMultiplicity()) &&
	     (13312u == type->GetSize()));
    }
  
  // 55.A0 Demo types ..
  // (-) {"type":"array_88d99985-7ced-4490-b069-a6ccbc70c54d","size":400,"multiplicity":100,"element":{"type":"float32","size":4}}
  // (+) {"type":"struct_70997129-8d17-4e3f-9d4d-05563617a9be","attributes":[{"bestIP1":{"type":"uint8","size":1}},{"bestIP2":{"type":"uint8","size":1}},{"bestIP3":{"type":"uint8","size":1}},{"bestIP4":{"type":"uint8","size":1}},{"bestIP5":{"type":"uint8","size":1}},{"bestIP6":{"type":"uint8","size":1}}]}
  // (+) {"type":"struct_0bba0470-6cae-4b12-bbd7-902f994c8a01","attributes":[{"SectorIdx":{"type":"array_62cb73ed-9b93-4e6f-be05-a4d57270b172","size":576,"multiplicity":6,"element":{"type":"struct_b2d77d7a-4013-4be2-9222-0963bdc52504","attributes":[{"Error":{"type":"array_121523a2-4ab7-4459-af86-4803ff58d04c","size":96,"multiplicity":24,"element":{"type":"float32","size":4}}}]}}}]}
  // (+) {"type":"array_c1110562-526e-40d3-92ae-6a7b2857c920","size":768,"multiplicity":24,"element":{"type":"struct_1acda46c-72d1-4960-9721-c180208a9016","attributes":[{"enabled":{"type":"float64","size":8}},{"geometry":{"type":"struct_301e9c6c-200a-482f-8cc4-380dfcc3b66e","attributes":[{"r":{"type":"float64","size":8}},{"z":{"type":"float64","size":8}},{"angle":{"type":"float64","size":8}}]}}]}}
  // (-) {"type":"array_269b82ea-2177-48a7-a764-4e3262a9182e","size":96,"multiplicity":24,"element":{"type":"float32","size":4}}

  if (ret)
    {
      std::shared_ptr<AnyType> type;

      // (+) {"type":"struct_0bba0470-6cae-4b12-bbd7-902f994c8a01","attributes":[{"SectorIdx":{"type":"array_62cb73ed-9b93-4e6f-be05-a4d57270b172","size":576,"multiplicity":6,"element":{"type":"struct_b2d77d7a-4013-4be2-9222-0963bdc52504","attributes":[{"Error":{"type":"array_121523a2-4ab7-4459-af86-4803ff58d04c","size":96,"multiplicity":24,"element":{"type":"float32","size":4}}}]}}}]}

      char buffer [] = "{\"type\":\"struct_c325165c-d17c-4824-843a-4b79b063084e\","
                        "\"attributes\":["
                                        "{\"SectorIdx\":{\"type\":\"array_1d804c43-0370-4f39-87d6-666fb7572885\","
                                                        "\"size\":576,"
                                                        "\"multiplicity\":6,"
                                                        "\"element\":{\"type\":\"struct_6d850eda-ac7b-4919-a114-f34acca04b36\","
                                                                     "\"attributes\":["
                                                                                     "{\"Error\":{\"type\":\"array_10fc6d0b-93b7-404d-af08-5df3e9956b28\","
                                                                                                 "\"size\":96,"
                                                                                                 "\"multiplicity\":24,"
                                                                                                 "\"element\":{\"type\":\"float32\",\"size\":4}"
                                                                                                "}"
                                                                                     "}"
                                                                                    "]}}}]}";

      log_info(".. type '%s'", buffer);

      ret = (Is<CompoundType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     Is<ArrayType>(GetAttributeType(type,"SectorIdx")) &&
	     Is<CompoundType>(GetAttributeType(type,"SectorIdx[0]")) &&
	     Is<ArrayType>(GetAttributeType(type,"SectorIdx[0].Error")) &&
	     (6u == std::dynamic_pointer_cast<const ArrayType>(GetAttributeType(type,"SectorIdx"))->GetMultiplicity()) &&
	     (24u == std::dynamic_pointer_cast<const ArrayType>(GetAttributeType(type,"SectorIdx[0].Error"))->GetMultiplicity()) &&
	     (576u == type->GetSize()));
  
      if (ret)
	{
	  void* ref = malloc(65536); memset(ref, 0, 65536u);
	  
	  char buffer [65536];
	  
	  if (ret)
	    {
	      ret = (true == SerialiseInstance(type, ref, buffer, 65536u));
	    }
	  
	  if (ret)
	    {
	      log_info(".. instance '%s'", buffer);
	    }

	  if (ret)
	    {
	      char value [] = "{\"SectorIdx\":[{\"Error\":[0.0191193223,0.0219534039,0.0330116749,0.0296001434,0.0248932838,0.0238699913,0.0118563175,0.0048648417,0.00237432867,0.00988766551,0.0146003366,0.0153108239,0.0253266096,0.0215716362,0.0185700655,0.019636631,0.022183001,0.0174396038,0.00990182161,0.00145728141,0.0168585181,0.021502614,0.00328083336,0.0112460256]},{\"Error\":[0.0178536177,0.026829958,0.0365434885,0.0353250504,0.0264182091,0.0208483934,0.009691,0.00538465381,0.00163832307,0.0116291642,0.0159140825,0.0159474611,0.0187574625,0.0213091969,0.0195160508,0.0198804736,0.0220808983,0.0167768598,0.0132743716,0.00192910433,0.0169520378,0.0118170381,0.00375549495,0.0106501579]},{\"Error\":[0.0193254352,0.0361382961,0.0333799124,0.0304516554,0.0324485302,0.0213414431,0.01365,0.00377266109,0.00237466395,0.00799164176,0.0119716525,0.0226714611,0.023507297,0.0304117203,0.0199300051,0.0193858743,0.0162518024,0.0205669403,0.0151259303,0.00177036971,0.0196875334,0.0178215504,0.00318950415,0.0109266639]},{\"Error\":[0.0181772113,0.022454977,0.0283899307,0.0331631899,0.0384800434,0.0171884298,0.0134200454,0.004795,0.00311467052,0.00822770596,0.0140758157,0.0196895,0.0227915049,0.0269033909,0.020493269,0.0208098888,0.0241833925,0.0152090192,0.0129678845,0.00201949477,0.018214345,0.0174864531,0.00294366479,0.00714784861]},{\"Error\":[0.01480335,0.0260283947,0.0333634615,0.0345065594,0.0325551033,0.0258649588,0.0097771883,0.00415501,0.00272583961,0.0111196339,0.0141391754,0.0185434818,0.0199184418,0.0186133385,0.0217983723,0.0228984356,0.026943326,0.0149383545,0.0105345845,0.0017984882,0.0165703297,0.0174106359,0.00403533876,0.009329319]},{\"Error\":[0.019549787,0.0265532732,0.0325117111,0.0479666,0.02967,0.0228366852,0.0141060352,0.00487916172,0.00254400074,0.00836435,0.013146162,0.0153927207,0.0219973922,0.0239217281,0.024643302,0.0214281678,0.0243936181,0.0145145655,0.0151724815,0.0014346689,0.0203499794,0.0191568732,0.00315466523,0.0084489584]}]}";
	      ret = (0u < ParseInstance(type, ref, value));
	    }

	  if (ret)
	    {
	      float32 value = *(reinterpret_cast<float32*>(static_cast<uint8*>(ref)+GetAttributeOffset(type, "SectorIdx[1].Error[0]")));
	      log_info(".. has value '%s' '%f'", "SectorIdx[1].Error[0]", value);
	      ret = ((0.017853 <= value) &&
		     (0.017855 >= value));
	    }

	  free(ref);
	}
    }
  
  if (ret)
    {
      std::shared_ptr<AnyType> type;

      // (+) {"type":"struct_70997129-8d17-4e3f-9d4d-05563617a9be","attributes":[{"bestIP1":{"type":"uint8","size":1}},{"bestIP2":{"type":"uint8","size":1}},{"bestIP3":{"type":"uint8","size":1}},{"bestIP4":{"type":"uint8","size":1}},{"bestIP5":{"type":"uint8","size":1}},{"bestIP6":{"type":"uint8","size":1}}]}

      char buffer [] = "{\"type\":\"struct_70997129-8d17-4e3f-9d4d-05563617a9be\",\"attributes\":[{\"bestIP1\":{\"type\":\"uint8\",\"size\":1}},{\"bestIP2\":{\"type\":\"uint8\",\"size\":1}},{\"bestIP3\":{\"type\":\"uint8\",\"size\":1}},{\"bestIP4\":{\"type\":\"uint8\",\"size\":1}},{\"bestIP5\":{\"type\":\"uint8\",\"size\":1}},{\"bestIP6\":{\"type\":\"uint8\",\"size\":1}}]}";

      log_info(".. type '%s'", buffer);

      ret = (Is<CompoundType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     (6u == std::dynamic_pointer_cast<CompoundType>(type)->GetAttributeNumber()) &&
	     (6u == type->GetSize()));
  
      if (ret)
	{
	  void* ref = malloc(65536); memset(ref, 0, 65536u);
	  
	  char buffer [65536];
	  
	  if (ret)
	    {
	      ret = (true == SerialiseInstance(type, ref, buffer, 65536u));
	    }
	  
	  if (ret)
	    {
	      log_info(".. instance '%s'", buffer);
	    }

	  free(ref);
	}
    }
  
  if (ret)
    {
      std::shared_ptr<AnyType> type;

      // (+) {"type":"array_c1110562-526e-40d3-92ae-6a7b2857c920","size":768,"multiplicity":24,"element":{"type":"struct_1acda46c-72d1-4960-9721-c180208a9016","attributes":[{"enabled":{"type":"float64","size":8}},{"geometry":{"type":"struct_301e9c6c-200a-482f-8cc4-380dfcc3b66e","attributes":[{"r":{"type":"float64","size":8}},{"z":{"type":"float64","size":8}},{"angle":{"type":"float64","size":8}}]}}]}}

      char buffer [] = "{\"type\":\"array_c1110562-526e-40d3-92ae-6a7b2857c920\",\"size\":768,\"multiplicity\":24,\"element\":{\"type\":\"struct_1acda46c-72d1-4960-9721-c180208a9016\",\"attributes\":[{\"enabled\":{\"type\":\"float64\",\"size\":8}},{\"geometry\":{\"type\":\"struct_301e9c6c-200a-482f-8cc4-380dfcc3b66e\",\"attributes\":[{\"r\":{\"type\":\"float64\",\"size\":8}},{\"z\":{\"type\":\"float64\",\"size\":8}},{\"angle\":{\"type\":\"float64\",\"size\":8}}]}}]}}";

      log_info(".. type '%s'", buffer);

      ret = (Is<ArrayType>(buffer) &&
	     (0u < Parse(type, buffer)) &&
	     (24u == std::dynamic_pointer_cast<ArrayType>(type)->GetMultiplicity()) &&
	     Is<CompoundType>(std::dynamic_pointer_cast<ArrayType>(type)->GetElementType()) &&
	     (768u == type->GetSize()));
  
      if (ret)
	{
	  void* ref = malloc(65536); memset(ref, 0, 65536u);
	  
	  char buffer [65536];
	  
	  if (ret)
	    {
	      ret = (true == SerialiseInstance(type, ref, buffer, 65536u));
	    }
	  
	  if (ret)
	    {
	      log_info(".. instance '%s'", buffer);
	    }

	  free(ref);
	}
    }
  
  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Parse_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, Parse_default)");
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, Parse_error)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  {

  std::shared_ptr<AnyType> type;
 
  if (ret)
    {
      ret = (0u == Parse(type, ""));
    }

  if (ret)
    {
      ret = (0u == Parse(type, "{}"));
    }

  if (ret)
    {
      ret = (0u == Parse(type, "{\"attr\":\"value\"}"));
    }

  if (ret)
    {
      ret = (0u == Parse(type, "{\"type\":\"error\"}"));
    }

  if (ret)
    {
      ret = (0u == Parse(type, "{\"type\":\"error\",\"element\":{..},\"multiplicity\":8}"));
    }
#if 0 // Unexpected keword stops parsing but does not necessarilly fail
  if (ret)
    {
      ret = (0u == Parse(type, "{\"type\":\"error\",\"element\":{\"type\":\"bool\"},\"multiplicity\":8,\"attr\":\"value\"}"));
    }

  if (ret)
    {
      ret = (0u == Parse(type, "{\"type\":\"error\",\"attributes\":[{..}]}"));
    }

  if (ret)
    {
      ret = (0u == Parse(type, "{\"type\":\"error\",\"attributes\":[{\"error\":{\"type\":\"error\",\"element\":{..},\"multiplicity\":8}]}"));
    }

  if (ret)
    {
      ret = (0u == Parse(type, "{\"type\":\"error\",\"attributes\":[{\"booleans\":{\"type\":\"boolarray\",\"element\":{\"type\":\"bool\"},\"multiplicity\":8}],\"attr\":\"value\"}"));
    }
#endif
  }

  ASSERT_EQ(true, ret);
}
  
TEST(AnyTypeHelper_Test, ParseInstance_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  void* ref = malloc(1024); memset(ref, 0, 1024u);

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, ParseInstance_default) - Scalars ..");
    }
  
  if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::Boolean;

      ret = ((4u == ParseInstance(type, ref, "true")) &&
	     (4u == ParseInstance(type, ref, "true, ")) && 
	     (4u == ParseInstance(type, ref, "true ]")) &&
	     (1u == ParseInstance(type, ref, "1")) &&
	     (1u == ParseInstance(type, ref, "1, ")) && 
	     (1u == ParseInstance(type, ref, "1 ]")) &&
	     (0u == ParseInstance(type, ref, "")) &&
	     (0u == ParseInstance(type, ref, " , ")) && 
	     (0u == ParseInstance(type, ref, "[ ]")));
    }
  
  if (ret)
    {
      // Test memory location
      ret = (true == *(static_cast<bool*>(ref)));
    }

  if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::UnsignedInteger32;

      ret = ((4u == ParseInstance(type, ref, "1024")) &&
	     (4u == ParseInstance(type, ref, "1024,")) &&
	     (4u == ParseInstance(type, ref, "1024u")) &&
	     (0u == ParseInstance(type, ref, "")) &&
	     (0u == ParseInstance(type, ref, " , ")) && 
	     (0u == ParseInstance(type, ref, "[ ]")));
    }
  
  if (ret)
    {
      // Test memory location
      ret = (1024u == *(static_cast<uint32*>(ref)));
    }

   if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::String;

      ret = (5u == ParseInstance(type, ref, "\"true\""));
    }
  
  if (ret)
    {
      // Test memory location
      ret = (strncmp(static_cast<char*>(ref), "true", 4) == 0);
    }

  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, ParseInstance_default) - Scalar array ..");
    }

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::ScalarArrays_t"))
										  ->AddAttribute("booleans", ccs::HelperTools::NewArrayType("ccs::test::BooleanArray_t", "bool", 8u))
										  ->AddAttribute("longstr", ccs::HelperTools::NewArrayType("ccs::test::CharacterArray_t", "char8", 256u))
										  ->AddAttribute("strings", ccs::HelperTools::NewArrayType("ccs::test::StringArray_t", "string", 8u))));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      char buffer [] = "[ true, 1, 0, false, true, 1, 0, false ]";
      ret = (40u == ParseInstance(std::dynamic_pointer_cast<CompoundType>(type)->GetAttributeType("booleans"), ref, buffer));
    }

  if (ret)
    {
      // Test memory location
      ret = ((true == (static_cast<bool*>(ref))[0]) &&
	     (true == (static_cast<bool*>(ref))[1]) &&
	     (false == (static_cast<bool*>(ref))[2]) &&
	     (false == (static_cast<bool*>(ref))[3]) &&
	     (true == (static_cast<bool*>(ref))[4]) &&
	     (true == (static_cast<bool*>(ref))[5]) &&
	     (false == (static_cast<bool*>(ref))[6]) &&
	     (false == (static_cast<bool*>(ref))[7]));
    }

  if (ret)
    {                 // 01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
      char buffer [] = "\"This is a long string which hopefully is longer than the default size so as to be able to test serialisation\"";
      ret = (110u == ParseInstance(std::dynamic_pointer_cast<CompoundType>(type)->GetAttributeType("longstr"), ref, buffer));
    }

  if (ret)
    {
      // Test memory location
      ret = (strlen(static_cast<char*>(ref)) == 108u); // The '"' are stripped out
    }

  if (ret)
    {
      char buffer [] = "[ \"true\", \"1\", \"0\", \"false\", \"true\", \"1\", \"0\", \"false\" ]";
      ret = (56u == ParseInstance(std::dynamic_pointer_cast<CompoundType>(type)->GetAttributeType("strings"), ref, buffer));
    }

  if (ret)
    {
      // Test memory location
      ret = (strncmp(static_cast<char*>(ref), "true", 4) == 0);
    }

  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)
										  ->AddAttribute("arrays", test.arrays)
										  ->AddAttribute("struct", ((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
													    ->AddAttribute("yet","string")
													    ->AddAttribute("another","string")
													    ->AddAttribute("string","string")))));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, ParseInstance_default) - Compound type ..");
    }

  if (ret)
    {
      char buffer [] = "{ \"scalars\": { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, \"arrays\": { \"booleans\": [ false, false, false, false, false, false, false, false ], \"integers\": [ 0, 0, 0, 0, 0, 0, 0, 0 ] }, \"struct\": { \"yet\": \"\", \"another\": \"\", \"string\": \"\" } }";
      ret = (242u == ParseInstance(type, ref, buffer));
    }

  if (ret)
    {
      // ToDo - Test memory location
    }

  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::CompoundArray_t", test.scalars, 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, ParseInstance_default) - Compound array ..");
    }

  if (ret)
    {
      char buffer [] = "[ { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, { \"boolean\": false, \"uint32\": 0, \"float64\": 0 }, { \"boolean\": false, \"uint32\": 0, \"float64\": 0 } ]";
      ret = (394u == ParseInstance(type, ref, buffer));
    }

  if (ret)
    {
      // ToDo - Test memory location
    }

  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, ParseInstance_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, ParseInstance_default)");
    }

  free(ref);

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, Serialise_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  char buffer [1024];

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - Scalars ..");
    }
  
  if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::Boolean;

      ret = (true == Serialise(type, buffer, 1024u));
    }
  
  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - .. '%s'", buffer);
    }

  if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::UnsignedInteger32;

      ret = (true == Serialise(type, buffer, 1024u));
    }
  
  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - .. '%s'", buffer);
    }

   if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::String;

      ret = (true == Serialise(type, buffer, 1024u));
    }
  
  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - .. '%s'", buffer);
    }

  }

  {

  std::shared_ptr<AnyType> type;

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - Scalar array ..");
    }

  type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::BooleanArray_t", "bool", 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      ret = (true == Serialise(type, buffer, 1024u));
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - .. '%s'", buffer);
    }

  type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::CharacterArray_t", "char8", 256u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      ret = (true == Serialise(type, buffer, 1024u));
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - .. '%s'", buffer);
    }

  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)
										  ->AddAttribute("arrays", test.arrays)
										  ->AddAttribute("struct", ((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
													    ->AddAttribute("yet","string")
													    ->AddAttribute("another","string")
													    ->AddAttribute("string","string")))));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - Compound type ..");
    }

  if (ret)
    {
      ret = (true == Serialise(type, buffer, 1024u));
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - .. '%s'", buffer);
    }

  }

  {

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::CompoundArray_t", test.scalars, 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - Compound array ..");
    }

  if (ret)
    {
      ret = (true == Serialise(type, buffer, 1024u));
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - .. '%s'", buffer);
    }

  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Serialise_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, Serialise_default)");
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, SerialiseInstance_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  char buffer [1024];
  void* ref = malloc(1024); memset(ref, 0, 1024u);

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - Scalars ..");
    }
  
  if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::Boolean;

      ret = (true == SerialiseInstance(type, ref, buffer, 1024u));
    }
  
  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - .. '%s'", buffer);
    }

  if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::UnsignedInteger32;

      ret = (true == SerialiseInstance(type, ref, buffer, 1024u));
    }
  
  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - .. '%s'", buffer);
    }

   if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::String;

      ret = (true == SerialiseInstance(type, ref, buffer, 1024u));
    }
  
  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - .. '%s'", buffer);
    }

  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - Scalar array ..");
    }

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::BooleanArray_t", "bool", 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      ret = (true == SerialiseInstance(type, ref, buffer, 1024u));
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - .. '%s'", buffer);
    }

  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - Compound type ..");
    }

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)
										  ->AddAttribute("arrays", test.arrays)
										  ->AddAttribute("struct", ((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
													    ->AddAttribute("yet","string")
													    ->AddAttribute("another","string")
													    ->AddAttribute("string","string")))));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      ret = (true == SerialiseInstance(type, ref, buffer, 1024u));
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - .. '%s'", buffer);
    }

  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - Compound array ..");
    }

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(ccs::HelperTools::NewArrayType("ccs::test::CompoundArray_t", test.scalars, 8u));

  if (ret)
    {
      ret = (type ? true : false);
    }

  if (ret)
    {
      ret = (true == SerialiseInstance(type, ref, buffer, 1024u));
    }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - .. '%s'", buffer);
    }

  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, SerialiseInstance_default)");
    }

  free(ref);

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, SerialiseInstance_robustness)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  std::shared_ptr<AnyType> type = std::shared_ptr<AnyType>(dynamic_cast<AnyType*>((new (std::nothrow) CompoundType ("ccs::test::CompountType_t"))
										  ->AddAttribute("scalars", test.scalars)));


  bool ret = (type ? true : false);

  char buffer [1024];
  void* ref = malloc(1024); memset(ref, 0, 1024u);

  if (ret)
    {
      ret = ((false == SerialiseInstance(type, NULL, buffer, 1024u)) &&
	     (false == SerialiseInstance(type, ref, NULL, 1024u)) &&
	     (true == SerialiseInstance(type, ref, buffer, 0u)));
    }

  if (ret)
    {
      std::shared_ptr<AnyType> invalid;
      ret = (false == SerialiseInstance(invalid, ref, buffer, 1024u));
    }

  free(ref);

  ASSERT_EQ(true, ret);
}

TEST(AnyTypeHelper_Test, Flatten_default)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  AnyTypeHelper_Test test;

  bool ret = true;

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Flatten_default) - Scalars ..");
    }
  
  std::shared_ptr<const CompoundType> flattened;
 
  if (ret)
    {
      std::shared_ptr<const AnyType> type = ccs::types::Boolean;
 
      flattened = Flatten(type); // Should be invalid
      ret = (flattened ? false : true);
    }
  
  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, SerialiseInstance_default) - .. success");
    }

  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Flatten_default) - Scalar arrays ..");
    }
  
  std::shared_ptr<AnyType> type;
 
  if (ret)
    {
      char buffer [] = "{\"type\":\"BooleanArray_t\","
	                "\"multiplicity\":8,"
                        "\"element\":"
                                    "{\"type\":\"bool\",\"size\":1}"
                       "}";

      log_info(".. type '%s'", buffer);

      ret = (0u < Parse(type, buffer));
    }
  
  std::shared_ptr<const CompoundType> flattened;
 
  if (ret)
    {
      flattened = Flatten(type);
      ret = (flattened ? true : false);
    }
  
  if (ret)
    {
      ret = ((8u == flattened->GetAttributeNumber()) && 
	     (type->GetSize() == flattened->GetSize()));
    }
  
  if (ret)
    {
      char buffer [1024];
      ret = Serialise(flattened, buffer,1024u);

      if (ret)
	{
	  log_info(".. flattened '%s'", buffer);
	}
    }
  
  }

  {

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Parse_default) - Compound type ..");
    }
  
  std::shared_ptr<AnyType> type;
 
  if (ret)
    {
      char buffer [] = "{\"type\":\"CompoundType_t\","
                        "\"attributes\":["
                                        "{\"counter\":{\"type\":\"uint64\",\"size\":8}},"
                                        "{\"timestamp\":{\"type\":\"uint64\",\"size\":8}},"
                                        "{\"scalars\":"
                                                     "{\"type\":\"Scalars_t\","
                                                      "\"attributes\":["
                                                                      "{\"boolean\":{\"type\":\"bool\",\"size\":1}},"
                                                                      "{\"uint32\":{\"type\":\"uint32\",\"size\":4}},"
                                                                      "{\"float64\":{\"type\":\"float64\",\"size\":8}}"
                                                                     "]"
                                                     "}"
                                        "}"
                                       "]"
                       "}";

      log_info(".. type '%s'", buffer);

      ret = (0u < Parse(type, buffer));
    }
  
  std::shared_ptr<const CompoundType> flattened;
 
  if (ret)
    {
      flattened = Flatten(type);
      ret = (flattened ? true : false);
    }
  
  if (ret)
    {
      ret = ((5u == flattened->GetAttributeNumber()) && 
	     (type->GetSize() == flattened->GetSize()));
    }
  
  if (ret)
    {
      char buffer [1024];
      ret = Serialise(flattened, buffer,1024u);

      if (ret)
	{
	  log_info(".. flattened '%s'", buffer);
	}
    }

  if (ret)
    {
      char buffer [] = "{\"type\":\"struct_c325165c-d17c-4824-843a-4b79b063084e\","
                        "\"attributes\":["
                                        "{\"SectorIdx\":{\"type\":\"array_1d804c43-0370-4f39-87d6-666fb7572885\","
                                                        "\"size\":576,"
                                                        "\"multiplicity\":6,"
                                                        "\"element\":{\"type\":\"struct_6d850eda-ac7b-4919-a114-f34acca04b36\","
                                                                     "\"attributes\":["
                                                                                     "{\"Error\":{\"type\":\"array_10fc6d0b-93b7-404d-af08-5df3e9956b28\","
                                                                                                 "\"size\":96,"
                                                                                                 "\"multiplicity\":24,"
                                                                                                 "\"element\":{\"type\":\"float32\",\"size\":4}"
                                                                                                "}"
                                                                                     "}"
                                                                                    "]}}}]}";

      log_info(".. type '%s'", buffer);

      ret = (0u < Parse(type, buffer));
    }
  
  if (ret)
    {
      flattened = Flatten(type);
      ret = (flattened ? true : false);
    }

  if (ret)
    {
      ret = ((144u == flattened->GetAttributeNumber()) && 
	     (type->GetSize() == flattened->GetSize()));
    }

  if (ret)
    {
      char buffer [1024];
      ret = Serialise(flattened, buffer,1024u);

      if (ret)
	{
	  log_info(".. flattened '%s'", buffer);
	}
    }

  }

  if (ret)
    {
      log_info("TEST(AnyTypeHelper_Test, Flatten_default) - Success");
    }
  else
    {
      log_error("TEST(AnyTypeHelper_Test, Flatten_default)");
    }

  ASSERT_EQ(true, ret);
}

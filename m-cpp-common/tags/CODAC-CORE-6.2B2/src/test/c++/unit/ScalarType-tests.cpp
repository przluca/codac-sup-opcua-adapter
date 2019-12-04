/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/ScalarType-tests.cpp $
* $Id: ScalarType-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include "lu-table.h"

#include "AnyType.h"
#include "AnyTypeHelper.h"
#include "ScalarType.h"

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition

TEST(ScalarType_Test, RTTI_string)
{
  bool ret = ccs::HelperTools::Is<ccs::types::ScalarType>(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::String));

  if (ret)
    {
      std::shared_ptr<const ccs::types::ScalarType> type = ccs::types::String;
      log_info("TEST(ScalarType_Test, RTTI_string) - Scalar type '%s' of size '%u'", type->GetName(), type->GetSize());
      ret = ((type->GetSize() == ccs::types::String->GetSize()) &&
	     (type->GetSize() == sizeof(ccs::types::string)) &&
	     (type->GetSize() == 64u));
    }

  const ccs::types::AnyType* type = static_cast<const ccs::types::AnyType*>(NULL);

  if (ret)
    {
      type = dynamic_cast<const ccs::types::AnyType*>(ccs::types::String.get());
      ret = (static_cast<const ccs::types::AnyType*>(NULL) != type);
    }

  if (ret)
    {
      ret = ((type->GetSize() == ccs::types::String->GetSize()) &&
	     (type->GetSize() == sizeof(ccs::types::string)) &&
	     (type->GetSize() == 64u));
    }

  if (ret)
    {
      ret = ccs::HelperTools::Is<ccs::types::ScalarType>(type);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, Operator_comparison)
{
  ccs::types::ScalarType type = *ccs::types::SignedInteger32;

  bool ret = (type.GetSize() == 4u);

  if (ret)
    {
      ret = (type == *ccs::types::SignedInteger32);
    }

  if (ret)
    {
      ret = !(type == *ccs::types::UnsignedInteger32);
    }

  if (ret)
    {
      ret = !(type == *ccs::types::SignedInteger64);
    }

  if (ret)
    {
      ret = !(type == *ccs::types::UnsignedInteger64);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ToNetworkByteOrder_bool)
{
  bool value = true;

  bool ret = (ccs::types::Boolean->ToNetworkByteOrder(static_cast<void*>(&value)) == true);

  if (ret)
    {
      ret = (value == true);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ToNetworkByteOrder_int8)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int8", 1u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int8_t value = 0x01;

  if (ret)
    {
      ret = (type->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x01);
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger8->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x01);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ToNetworkByteOrder_int16)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int16", 2u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int16_t value = 0x0123;

  if (ret)
    {
      ret = (type->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsBigEndian())
	{
	  ret = (value == 0x0123);
	}
      else
	{
	  ret = (value == 0x2301);
	}
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger16->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x0123);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ToNetworkByteOrder_int32)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int32", 4u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int32_t value = 0x01234567;

  if (ret)
    {
      ret = (type->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsBigEndian())
	{
	  ret = (value == 0x01234567);
	}
      else
	{
	  ret = (value == 0x67452301);
	}
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger32->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x01234567);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ToNetworkByteOrder_int64)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int64", 8u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int64_t value = 0x0123456789abcdefl;

  if (ret)
    {
      ret = (type->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsBigEndian())
	{
	  ret = (value == 0x0123456789abcdefl);
	}
      else
	{
	  ret = (value == 0xefcdab8967452301l);
	}
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger64->ToNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x0123456789abcdefl);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, FromNetworkByteOrder_bool)
{
  bool value = true;

  bool ret = (ccs::types::Boolean->FromNetworkByteOrder(static_cast<void*>(&value)) == true);

  if (ret)
    {
      ret = (value == true);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, FromNetworkByteOrder_int8)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int8", 1u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int8_t value = 0x01;

  if (ret)
    {
      ret = (type->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x01);
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger8->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x01);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, FromNetworkByteOrder_int16)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int16", 2u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int16_t value = 0x0123;

  if (ret)
    {
      ret = (type->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsBigEndian())
	{
	  ret = (value == 0x0123);
	}
      else
	{
	  ret = (value == 0x2301);
	}
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger16->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x0123);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, FromNetworkByteOrder_int32)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int32", 4u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int32_t value = 0x01234567;

  if (ret)
    {
      ret = (type->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsBigEndian())
	{
	  ret = (value == 0x01234567);
	}
      else
	{
	  ret = (value == 0x67452301);
	}
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger32->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x01234567);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, FromNetworkByteOrder_int64)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("ccs::types::int64", 8u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  int64_t value = 0x0123456789abcdefl;

  if (ret)
    {
      ret = (type->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsBigEndian())
	{
	  ret = (value == 0x0123456789abcdefl);
	}
      else
	{
	  ret = (value == 0xefcdab8967452301l);
	}
    }

  if (ret)
    {
      ret = (ccs::types::SignedInteger64->FromNetworkByteOrder(static_cast<void*>(&value)) == true);
    }

  if (ret)
    {
      ret = (value == 0x0123456789abcdefl);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ParseInstance_bool)
{
  bool ret = true;

  if (ret)
    {
      char buffer [32] = "1";
      bool value = false;
      ret = (ccs::HelperTools::ParseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::Boolean), static_cast<void*>(&value), buffer) > 0u);
      log_info("TEST(ScalarType_Test, ParseInstance_bool) - '%s' as '%s'", buffer, (value ? "true" : "false"));
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ParseInstance_int8)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int8", 1u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = "0x80";
      int8_t value = 0;
      ret = ((ccs::HelperTools::ParseInstance(type, static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128));
      log_info("TEST(ScalarType_Test, ParseInstance_int8) - '%s' as '%hhd'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = "-128";
      int8_t value = 0;
      ret = ((ccs::HelperTools::ParseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger8), static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128));
      log_info("TEST(ScalarType_Test, ParseInstance_int8) - '%s' as '%hhd'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ParseInstance_int16)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int16", 2u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = "0xff80";
      int16_t value = 0;
      ret = ((ccs::HelperTools::ParseInstance(type, static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128));
      log_info("TEST(ScalarType_Test, ParseInstance_int16) - '%s' as '%hd'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = "-128";
      int16_t value = 0;
      ret = ((ccs::HelperTools::ParseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger16), static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128));
      log_info("TEST(ScalarType_Test, ParseInstance_int16) - '%s' as '%hd'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ParseInstance_int32)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int32", 4u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = "0xffffff80";
      int32_t value = 0;
      ret = ((ccs::HelperTools::ParseInstance(type, static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128));
      log_info("TEST(ScalarType_Test, ParseInstance_int32) - '%s' as '%d'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = "-128";
      int32_t value = 0;
      ret = ((ccs::HelperTools::ParseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger32), static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128));
      log_info("TEST(ScalarType_Test, ParseInstance_int32) - '%s' as '%d'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, ParseInstance_int64)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int64", 8u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = "0xffffffffffffff80";
      int64_t value = 0l;
      ret = ((ccs::HelperTools::ParseInstance(type, static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128l));
      log_info("TEST(ScalarType_Test, ParseInstance_int64) - '%s' as '%ld'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = "-128";
      int64_t value = 0l;
      ret = ((ccs::HelperTools::ParseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger64), static_cast<void*>(&value), buffer) > 0u) && 
	     (value == -128l));
      log_info("TEST(ScalarType_Test, ParseInstance_int64) - '%s' as '%ld'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, SerialiseInstance_bool)
{
  bool ret = true;

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      bool value = false;
      ret = (ccs::HelperTools::SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::Boolean), static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_bool) - '%s' as '%s'", buffer, (value ? "true" : "false"));
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, SerialiseInstance_int8)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int8", 1u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int8_t value = -128;
      ret = (ccs::HelperTools::SerialiseInstance(type, static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int8) - '%s' as '%hhd'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int8_t value = -128;
      ret = (ccs::HelperTools::SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger8), static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int8) - '%s' as '%hhd'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, SerialiseInstance_int16)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int16", 2u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int16_t value = -128;
      ret = (ccs::HelperTools::SerialiseInstance(type, static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int16) - '%s' as '%hd'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int16_t value = -128;
      ret = (ccs::HelperTools::SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger16), static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int16) - '%s' as '%hd'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, SerialiseInstance_int32)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int32", 4u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int32_t value = -128;
      ret = (ccs::HelperTools::SerialiseInstance(type, static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int32) - '%s' as '%d'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int32_t value = -128;
      ret = (ccs::HelperTools::SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger32), static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int32) - '%s' as '%d'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, SerialiseInstance_int64)
{
  std::shared_ptr<const ccs::types::AnyType> type (new (std::nothrow) ccs::types::ScalarType ("ccs::types::int64", 8u));

  bool ret = (type ? true : false);

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int64_t value = -128l;
      ret = (ccs::HelperTools::SerialiseInstance(type, static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int64) - '%s' as '%ld'", buffer, value);
    }

  if (ret)
    {
      char buffer [32] = STRING_UNDEFINED;
      int64_t value = -128l;
      ret = (ccs::HelperTools::SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger64), static_cast<void*>(&value), buffer, 32) == true);
      log_info("TEST(ScalarType_Test, SerialiseInstance_int64) - '%s' as '%ld'", buffer, value);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, SmartPointers_bool)
{
  // Static definition and registration in GlobalTypeDatabase
  bool ret = (ccs::types::Boolean.use_count() == 1);

  if (!ret)
    {
      ret = (ccs::types::Boolean.use_count() == 2);
    }

  ASSERT_EQ(true, ret);
}

TEST(ScalarType_Test, SmartPointers_uint64)
{
  // Static definition and registration in GlobalTypeDatabase
  bool ret = (ccs::types::UnsignedInteger64.use_count() == 1);

  if (!ret)
    {
      ret = (ccs::types::UnsignedInteger64.use_count() == 2);
    }

  ASSERT_EQ(true, ret);
}

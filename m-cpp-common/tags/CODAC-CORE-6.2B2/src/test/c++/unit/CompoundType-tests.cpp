/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/CompoundType-tests.cpp $
* $Id: CompoundType-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include "lu-table.h"

#include "AnyType.h" // Introspectable type definition (base class) ..
#include "AnyTypeHelper.h" // .. associated helper routines
#include "AnyTypeDatabase.h" // .. associated helper class

#include "CompoundType.h"

// Constants

// Type definition

class CompoundType_Test : public ccs::types::CompoundType
{

  public:

    CompoundType_Test (void);
    virtual ~CompoundType_Test (void) {};

  protected:

};

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition
  
CompoundType_Test::CompoundType_Test (void) : ccs::types::CompoundType("struct:flat") 
{ 

  this->AddAttribute("counter", ccs::types::UnsignedInteger64)
      ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
      ->AddAttribute("reserved", ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u)); 

}

TEST(CompoundType_Test, Constructor_default)
{
  ccs::types::CompoundType* type = new (std::nothrow) ccs::types::CompoundType;

  bool ret = (type != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, Constructor_copy)
{
  CompoundType_Test base;
  ccs::types::CompoundType* type = new (std::nothrow) ccs::types::CompoundType (base);

  bool ret = (type != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      ret = (type->GetSize() == base.GetSize());
    }

  if (ret)
    {
      ret = type->HasAttribute("counter");
    }

  // Etc.

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, Operator_comparison)
{
  CompoundType_Test base;
  ccs::types::CompoundType* type = (new (std::nothrow)  ccs::types::CompoundType("test::ctt::Compare_t"))
    ->AddAttribute("a", ccs::types::UnsignedInteger64)
    ->AddAttribute("b", ccs::types::UnsignedInteger64)
    ->AddAttribute("c", ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u));

  bool ret = (base == *type); // Comparison

  // Comparison from base type
  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

  if (ret) 
    {
      std::shared_ptr<const ccs::types::AnyType> ref = ccs::types::GlobalTypeDatabase::GetType("test::ctt::Compare_t");
      ret = static_cast<bool>(ref);

      if (ret)
	{
	  ret = (base == *ref);
	}
    }

  // Etc.

  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::Remove("test::ctt::Compare_t");
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, Operator_copy)
{
  CompoundType_Test base;
  ccs::types::CompoundType type; 

  type = base;

  bool ret = (type.GetSize() == base.GetSize());

  if (ret)
    {
      ret = type.HasAttribute("counter");
    }

  // Etc.

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, RTTI_frombase)
{
  ccs::types::AnyType* base = new (std::nothrow) ccs::types::CompoundType ("struct:flat");

  bool ret = (base != static_cast<ccs::types::AnyType*>(NULL));

  if (ret)
    {
      ret = ccs::HelperTools::Is<ccs::types::CompoundType>(base);
    }

  if (ret)
    {
      ccs::types::CompoundType* type = dynamic_cast<ccs::types::CompoundType*>(base);
      ret = (type != static_cast<ccs::types::CompoundType*>(NULL));
    }

  if (ret)
    {
      delete base;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, AddAttribute_success)
{
  ccs::types::CompoundType* type = new (std::nothrow) ccs::types::CompoundType ("struct:flat");

  bool ret = (static_cast<ccs::types::CompoundType*>(NULL) != type);

  if (ret)
    {
      type->AddAttribute("counter", ccs::types::UnsignedInteger64);
      ret = (type->GetSize() ==  8u);
      log_info("TEST(CompoundType_Test, AddAttribute_success) - Type '%s' with '%u' attributes has size of '%u'", type->GetName(), type->GetAttributeNumber(), type->GetSize());
    }

  if (ret)
    {
      type->AddAttribute("timestamp", ccs::types::UnsignedInteger64);
      ret = (type->GetSize() == 16u);
      log_info("TEST(CompoundType_Test, AddAttribute_success) - Type '%s' with '%u' attributes has size of '%u'", type->GetName(), type->GetAttributeNumber(), type->GetSize());
    }

  std::shared_ptr<const ccs::types::AnyType> array = std::shared_ptr<const ccs::types::AnyType>(ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u));

  if (ret)
    {
      type->AddAttribute("reserved", array);
      ret = (type->GetSize() == 24u);
      log_info("TEST(CompoundType_Test, AddAttribute_success) - Type '%s' with '%u' attributes has size of '%u'", type->GetName(), type->GetAttributeNumber(), type->GetSize());
    }

  // Since v1.3.3 - Type composition is managed through smart pointers ..
  if (ret)
    {
      log_info("TEST(CompoundType_Test, AddAttribute_success) - Type '%s' use count '%u'", array->GetName(), array.use_count());
      ret = (array.use_count() == 2);
    }

  if (ret)
    {
      delete type;
    }

  // .. array will get out of scope (and hence deleted) at the end of this method
  if (ret)
    {
      log_info("TEST(CompoundType_Test, AddAttribute_success) - Type '%s' use count '%u'", array->GetName(), array.use_count());
      ret = (array.use_count() == 1);
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, AddAttribute_error)
{
  ccs::types::CompoundType* type = new (std::nothrow) ccs::types::CompoundType ("struct:flat");

  bool ret = (type != static_cast<ccs::types::CompoundType*>(NULL));

  std::shared_ptr<const ccs::types::AnyType> invalid;

  if (ret)
    {
      type->AddAttribute("invalid", invalid);
      ret = (0u == type->GetSize());
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, AddAttribute_byTypeName)
{
  CompoundType_Test* base = new (std::nothrow) CompoundType_Test;
  ccs::types::CompoundType* type = new (std::nothrow) ccs::types::CompoundType;

  bool ret = (static_cast<ccs::types::CompoundType*>(NULL) != type);

  if (ret)
    {
      log_info("TEST(CompoundType_Test, AddAttribute_byTypeName) - Register in type database");
      ret = ccs::types::GlobalTypeDatabase::Register(base);
    }

  if (ret)
    {
      type->AddAttribute("value", "struct:flat");
      log_info("TEST(CompoundType_Test, AddAttribute_byTypeName) - Test size '%u'", type->GetSize());
      ret = (24u == type->GetSize());
    }

  if (ret)
    {
      log_info("TEST(CompoundType_Test, AddAttribute_byTypeName) - Remove from type database");
      ret = ccs::types::GlobalTypeDatabase::Remove("struct:flat");
    }

  if (ret)
    {
      type->AddAttribute("value", "struct:flat"); // Failure
      ret = (24u == type->GetSize()); // No change
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, HasAttribute_success)
{
  CompoundType_Test* type = new (std::nothrow) CompoundType_Test;

  bool ret = (type != static_cast<CompoundType_Test*>(NULL));

  for (ccs::types::uint32 index = 0u; ret && (index < type->GetAttributeNumber()); index += 1u)
    {
      ret = type->HasAttribute(index);
    }

  if (ret)
    {
      ret = type->HasAttribute("counter");
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsScalarType(type->GetAttributeType("counter")) && 
	     (type->GetAttributeIndex("counter") == 0u) &&
	     (type->GetAttributeType("counter") == ccs::types::UnsignedInteger64));
    }

  if (ret)
    {
      ret = type->HasAttribute("timestamp");
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsScalarType(type->GetAttributeType("timestamp")) && 
	     (type->GetAttributeIndex("timestamp") == 1u) &&
	     (type->GetAttributeType("timestamp") == ccs::types::UnsignedInteger64));
    }

  if (ret)
    {
      ret = type->HasAttribute("reserved");
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsArrayType(type->GetAttributeType("reserved")) && 
	     (type->GetAttributeIndex("reserved") == 2u) &&
	     (std::dynamic_pointer_cast<const ccs::types::ArrayType>(type->GetAttributeType("reserved"))->GetElementType() == ccs::types::UnsignedInteger8));
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, GetAttributeReference_success)
{
  CompoundType_Test* type = new (std::nothrow) CompoundType_Test;
  void* instance = static_cast<void*>(NULL);

  bool ret = (type != static_cast<CompoundType_Test*>(NULL));

  if (ret)
    {
      ret = ((instance = type->CreateInstance()) != static_cast<void*>(NULL)); 
    }

  if (ret)
    {
      log_info("TEST(CompoundType_Test, GetAttributeReference_success) - Attribute '%s' at address '%p'", "counter", type->GetAttributeReference(instance, "counter"));
      ret = (type->GetAttributeReference(instance, "counter") != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = (type->GetAttributeReference(instance, "counter") == type->GetAttributeReference(instance, 0u));
    }

  if (ret)
    {
      ret = (type->GetAttributeReference(instance, "counter") == static_cast<void*>(static_cast<uint8_t*>(instance) + type->GetAttributeOffset("counter")));
    }

  if (ret)
    {
      log_info("TEST(CompoundType_Test, GetAttributeReference_success) - Attribute '%s' at address '%p'", "timestamp", type->GetAttributeReference(instance, "timestamp"));
      ret = (type->GetAttributeReference(instance, "timestamp") != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = (type->GetAttributeReference(instance, "timestamp") == type->GetAttributeReference(instance, 1u));
    }

  if (ret)
    {
      ret = (type->GetAttributeReference(instance, "timestamp") == static_cast<void*>(static_cast<uint8_t*>(instance) + type->GetAttributeOffset("timestamp")));
    }

  if (ret)
    {
      ret = (type->GetAttributeReference(instance, "reserved") != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = (type->GetAttributeReference(instance, "reserved") == type->GetAttributeReference(instance, 2u));
    }

  if (ret)
    {
      ret = (type->GetAttributeReference(instance, "reserved") == static_cast<void*>(static_cast<uint8_t*>(instance) + type->GetAttributeOffset("reserved")));
    }

  if (ret)
    {
      type->DeleteInstance(instance);
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, GetAttributeSize_success)
{
  CompoundType_Test* type = new (std::nothrow) CompoundType_Test;

  bool ret = (type != static_cast<CompoundType_Test*>(NULL));

  if (ret)
    {
      ret = (type->HasAttribute("counter") && 
	     (type->GetAttributeSize("counter") == type->GetAttributeSize(0u)) &&
	     (type->GetAttributeSize("counter") == ccs::types::UnsignedInteger64->GetSize()));
    }

  if (ret)
    {
      ret = (type->HasAttribute("timestamp") && 
	     (type->GetAttributeSize("timestamp") == type->GetAttributeSize(1u)) &&
	     (type->GetAttributeSize("timestamp") == ccs::types::UnsignedInteger64->GetSize()));
    }

  if (ret)
    {
      ret = (type->HasAttribute("reserved") && 
	     (type->GetAttributeSize("reserved") == type->GetAttributeSize(2u)) &&
	     (type->GetAttributeSize("reserved") == (std::dynamic_pointer_cast<const ccs::types::ArrayType>(type->GetAttributeType("reserved"))->GetElementNumber() * ccs::types::UnsignedInteger8->GetSize())));
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, Flatten)
{
  std::shared_ptr<const ccs::types::CompoundType> type = std::shared_ptr<const ccs::types::CompoundType>(new (std::nothrow) CompoundType_Test);

  bool ret = ((type ? true : false) && (3u == type->GetAttributeNumber()));

  if (ret)
    {
      log_info("TEST(CompoundType_Test, Flatten) - Original type has '%u' attributes ..", type->GetAttributeNumber());
      char buffer [1024] = STRING_UNDEFINED; ccs::HelperTools::Serialise(type, buffer, 1024u); 
      log_info("TEST(CompoundType_Test, Flatten) - .. '%s'", buffer);
    }

  if (ret)
    {
      std::shared_ptr<const ccs::types::CompoundType> flattened = ccs::HelperTools::Flatten(type);
      ret = (flattened->GetAttributeNumber() == 10u);

      log_info("TEST(CompoundType_Test, Flatten) - Flattened type has '%u' attributes", flattened->GetAttributeNumber());
      char buffer [1024] = STRING_UNDEFINED; ccs::HelperTools::Serialise(flattened, buffer, 1024u); 
      log_info("TEST(CompoundType_Test, Flatten) - .. '%s'", buffer);

      for (uint_t index = 0u; index < flattened->GetAttributeNumber() && ret; index += 1u)
	{
	  log_info("TEST(CompoundType_Test, Flatten) - Type has attribute '%s' of type '%s' at offset '%u'", flattened->GetAttributeName(index), flattened->GetAttributeType(index)->GetName(), flattened->GetAttributeOffset(index));
	  ret = (ccs::HelperTools::Is<ccs::types::ScalarType>(flattened->GetAttributeType(index)));
	}
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, Flatten_nested)
{
  std::shared_ptr<ccs::types::CompoundType> type = std::shared_ptr<ccs::types::CompoundType>(new (std::nothrow) ccs::types::CompoundType ("struct:nested"));

  bool ret = (type ? true : false);

  if (ret)
    {
      type->AddAttribute("counter", ccs::types::UnsignedInteger64);
      ret = (type->GetSize() ==  8u);
    }

  if (ret)
    {
      type->AddAttribute("timestamp", ccs::types::UnsignedInteger64);
      ret = (type->GetSize() == 16u);
    }

  if (ret)
    {
      type->AddAttribute("payload", (new ccs::types::CompoundType ("struct:flat"))->AddAttribute("key", (new ccs::types::CompoundType ("struct:flat"))->AddAttribute("id", ccs::types::UnsignedInteger16))->AddAttribute("value", (new ccs::types::CompoundType ("struct:flat"))->AddAttribute("avg", ccs::types::Float32)->AddAttribute("rms", ccs::types::Float32)));
      ret = (type->GetSize() == 26u);
    }

  if (ret)
    {
      std::shared_ptr<const ccs::types::CompoundType> flattened = ccs::HelperTools::Flatten(std::const_pointer_cast<const ccs::types::CompoundType>(type));
      ret = (flattened->GetAttributeNumber() == 5u);

      for (uint_t index = 0u; index < flattened->GetAttributeNumber() && ret; index += 1u)
	{
	  log_info("TEST(CompoundType_Test, Flatten_nested) - Type has attribute '%s' of type '%s' at offset '%u'", flattened->GetAttributeName(index), flattened->GetAttributeType(index)->GetName(), flattened->GetAttributeOffset(index));
	  ret = (ccs::HelperTools::IsScalarType(flattened->GetAttributeType(index)));
	}
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, Flatten_array)
{
  std::shared_ptr<ccs::types::CompoundType> type = std::shared_ptr<ccs::types::CompoundType>(new (std::nothrow) ccs::types::CompoundType ("struct:nested"));

  bool ret = (type ? true : false);

  if (ret)
    {
      type->AddAttribute("counter", ccs::types::UnsignedInteger64);
      ret = (type->GetSize() ==  8u);
    }

  if (ret)
    {
      type->AddAttribute("timestamp", ccs::types::UnsignedInteger64);
      ret = (type->GetSize() == 16u);
    }

  if (ret)
    {
      type->AddAttribute("payload", ccs::HelperTools::NewArrayType("array", (new ccs::types::CompoundType ("struct:flat"))->AddAttribute("key", (new ccs::types::CompoundType ("struct:flat"))->AddAttribute("id", ccs::types::UnsignedInteger16))->AddAttribute("value", (new ccs::types::CompoundType ("struct:flat"))->AddAttribute("avg", ccs::types::Float32)->AddAttribute("rms", ccs::types::Float32)))->SetMultiplicity(8u));
      ret = (type->GetSize() == 96u);
    }

  if (ret)
    {
      std::shared_ptr<const ccs::types::CompoundType> flattened = ccs::HelperTools::Flatten(std::const_pointer_cast<const ccs::types::CompoundType>(type));
      ret = (flattened->GetAttributeNumber() == 26u);

      for (uint_t index = 0u; index < flattened->GetAttributeNumber() && ret; index += 1u)
	{
	  log_info("TEST(CompoundType_Test, Flatten_array) - Type has attribute '%s' of type '%s'at offset '%u'", flattened->GetAttributeName(index), flattened->GetAttributeType(index)->GetName(), flattened->GetAttributeOffset(index));
	  ret = (ccs::HelperTools::IsScalarType(flattened->GetAttributeType(index)));
	}
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, ToNetworkByteOrder)
{

  ccs::types::CompoundType* header = static_cast<ccs::types::CompoundType*>(NULL);

  bool ret = ((header = new (std::nothrow) ccs::types::CompoundType ("header")) != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      header->AddAttribute("uid", ccs::types::UnsignedInteger64);
      header->AddAttribute("size", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_uid", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_version", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_size", ccs::types::UnsignedInteger32);
      header->AddAttribute("counter", ccs::types::UnsignedInteger64);
      header->AddAttribute("send_time", ccs::types::UnsignedInteger64);
      header->AddAttribute("recv_time", ccs::types::UnsignedInteger64);
      ret = (header->GetSize() == 48u);
    }
#if 0 // Requires -std=c++11
  struct { 
    uint64_t uid = 0x0123456789abcdeful;
    uint32_t size = 48u;
    uint32_t t_uid = 0x01234567u;
    uint32_t t_version = 0x89abcdefu;
    uint32_t t_size = 1024u;
    uint64_t counter = 0ul;
    uint64_t send_t = 0ul;
    uint64_t recv_t = 0ul;
  } buffer;
#else
  struct { 
    uint64_t uid;
    uint32_t size;
    uint32_t t_uid;
    uint32_t t_version;
    uint32_t t_size;
    uint64_t counter;
    uint64_t send_t;
    uint64_t recv_t;
  } buffer;

  buffer.uid = 0x0123456789abcdeful;
  buffer.size = 48u;
  buffer.t_uid = 0x01234567u;
  buffer.t_version = 0x89abcdefu;
  buffer.t_size = 1024u;
  buffer.counter = 0ul;
  buffer.send_t = 0ul;
  buffer.recv_t = 0ul;
#endif  
  if (ret)
    {
      ret = (header->GetSize() == sizeof(buffer));
    }

  if (ret)
    {
      ret = (header->ToNetworkByteOrder(static_cast<void*>(&buffer)) == true); 
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.uid == 0xefcdab8967452301ul);
      else
	ret = (buffer.uid == 0x0123456789abcdeful);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.size == 805306368u); // 0x30.00.00.00
      else
	ret = (buffer.size == 48u);        // 0x00.00.00.30
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.t_uid == 0x67452301u);
      else
	ret = (buffer.t_uid == 0x01234567u);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.t_version == 0xefcdab89u);
      else
	ret = (buffer.t_version == 0x89abcdefu);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.t_size == 262144u); // 0x00.04.00.00
      else
	ret = (buffer.t_size == 1024u);   // 0x00.00.04.00
    }

  delete header;

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, FromNetworkByteOrder)
{

  ccs::types::CompoundType* header = static_cast<ccs::types::CompoundType*>(NULL);

  bool ret = ((header = new (std::nothrow) ccs::types::CompoundType ("header")) != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      header->AddAttribute("uid", ccs::types::UnsignedInteger64);
      header->AddAttribute("size", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_uid", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_version", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_size", ccs::types::UnsignedInteger32);
      header->AddAttribute("counter", ccs::types::UnsignedInteger64);
      header->AddAttribute("send_time", ccs::types::UnsignedInteger64);
      header->AddAttribute("recv_time", ccs::types::UnsignedInteger64);
      ret = (header->GetSize() == 48u);
    }
#if 0 // Requires -std=c++11
  struct { 
    uint64_t uid = 0x0123456789abcdeful;
    uint32_t size = 48u;
    uint32_t t_uid = 0x01234567u;
    uint32_t t_version = 0x89abcdefu;
    uint32_t t_size = 1024u;
    uint64_t counter = 0ul;
    uint64_t send_t = 0ul;
    uint64_t recv_t = 0ul;
  } buffer;
#else
  struct { 
    uint64_t uid;
    uint32_t size;
    uint32_t t_uid;
    uint32_t t_version;
    uint32_t t_size;
    uint64_t counter;
    uint64_t send_t;
    uint64_t recv_t;
  } buffer;

  buffer.uid = 0x0123456789abcdeful;
  buffer.size = 48u;
  buffer.t_uid = 0x01234567u;
  buffer.t_version = 0x89abcdefu;
  buffer.t_size = 1024u;
  buffer.counter = 0ul;
  buffer.send_t = 0ul;
  buffer.recv_t = 0ul;
#endif  
  if (ret)
    {
      ret = (header->FromNetworkByteOrder(static_cast<void*>(&buffer)) == true); 
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.uid == 0xefcdab8967452301ul);
      else
	ret = (buffer.uid == 0x0123456789abcdeful);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.size == 805306368u); // 0x30.00.00.00
      else
	ret = (buffer.size == 48u);        // 0x00.00.00.30
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.t_uid == 0x67452301u);
      else
	ret = (buffer.t_uid == 0x01234567u);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.t_version == 0xefcdab89u);
      else
	ret = (buffer.t_version == 0x89abcdefu);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer.t_size == 262144u); // 0x00.04.00.00
      else
	ret = (buffer.t_size == 1024u);   // 0x00.00.04.00
    }

  delete header;

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, ParseInstance)
{

  std::shared_ptr<ccs::types::CompoundType> header (new (std::nothrow) ccs::types::CompoundType ("header"));

  bool ret = (header ? true : false);

  if (ret)
    {
      header->AddAttribute("uid", ccs::types::UnsignedInteger64);
      header->AddAttribute("size", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_uid", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_version", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_size", ccs::types::UnsignedInteger32);
      header->AddAttribute("counter", ccs::types::UnsignedInteger64);
      header->AddAttribute("send_time", ccs::types::UnsignedInteger64);
      header->AddAttribute("recv_time", ccs::types::UnsignedInteger64);
      ret = (header->GetSize() == 48u);
    }

  struct { 
    uint64_t uid;
    uint32_t size;
    uint32_t t_uid;
    uint32_t t_version;
    uint32_t t_size;
    uint64_t counter;
    uint64_t send_t;
    uint64_t recv_t;
  } buffer;

  buffer.uid = 0ul;
  buffer.size = 0u;
  buffer.t_uid = 0u;
  buffer.t_version = 0u;
  buffer.t_size = 0u;
  buffer.counter = 0ul;
  buffer.send_t = 0ul;
  buffer.recv_t = 0ul;

  if (ret)
    {
      ret = (header->GetSize() == sizeof(buffer));
    }

  if (ret)
    {
      char string [] =
        "{"
	"\"uid\":0x0123456789abcdef,"
	"\"size\":0x00000030,"
	"\"topic_uid\":0x01234567,"
	"\"topic_version\":0x89abcdef,"
	"\"topic_size\":0x00000400,"
	"\"counter\":0x0000000000000000,"
	"\"send_time\":0x0000000000000000,"
	"\"recv_time\":0x0000000000000000"
        "}";

      log_info("TEST(CompoundType_Test, ParseInstance) - Try and parse string '%s'", string);
      ret = (0u < ccs::HelperTools::ParseInstance(std::const_pointer_cast<const ccs::types::CompoundType>(header), &buffer, string));
    }

  if (!ret)
    {
      log_error("TEST(CompoundType_Test, ParseInstance) - ParseInstance failed");
    }

  if (ret)
    {
      ret = ((buffer.uid == 0x0123456789abcdeful) &&
	     (buffer.size == 48u) &&
	     (buffer.t_uid == 0x01234567u) &&
	     (buffer.t_version == 0x89abcdefu) &&
	     (buffer.t_size == 1024u) &&
	     (buffer.counter == 0ul) &&
	     (buffer.send_t == 0ul) &&
	     (buffer.recv_t == 0ul));
    }

  if (!ret)
    {
      log_error("TEST(CompoundType_Test, ParseInstance) - Parsed attributes ..");
      log_error("TEST(CompoundType_Test, ParseInstance) - .. '%lu' '%u' '%u' '%u' '%u' '%lu' '%lu' '%lu'", 
		buffer.uid,
		buffer.size,
		buffer.t_uid,
		buffer.t_version,
		buffer.t_size,
		buffer.counter,
		buffer.send_t,
		buffer.recv_t);
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, SerialiseInstance)
{

  std::shared_ptr<ccs::types::CompoundType> header (new (std::nothrow) ccs::types::CompoundType ("header"));

  bool ret = (header ? true : false);

  if (ret)
    {
      header->AddAttribute("uid", ccs::types::UnsignedInteger64);
      header->AddAttribute("size", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_uid", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_version", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_size", ccs::types::UnsignedInteger32);
      header->AddAttribute("counter", ccs::types::UnsignedInteger64);
      header->AddAttribute("send_time", ccs::types::UnsignedInteger64);
      header->AddAttribute("recv_time", ccs::types::UnsignedInteger64);
      ret = (header->GetSize() == 48u);
    }
#if 0 // Requires -std=c++11
  struct { 
    uint64_t uid = 0x0123456789abcdeful;
    uint32_t size = 48u;
    uint32_t t_uid = 0x01234567u;
    uint32_t t_version = 0x89abcdefu;
    uint32_t t_size = 1024u;
    uint64_t counter = 0ul;
    uint64_t send_t = 0ul;
    uint64_t recv_t = 0ul;
  } buffer;
#else
  struct { 
    uint64_t uid;
    uint32_t size;
    uint32_t t_uid;
    uint32_t t_version;
    uint32_t t_size;
    uint64_t counter;
    uint64_t send_t;
    uint64_t recv_t;
  } buffer;

  buffer.uid = 0x0123456789abcdeful;
  buffer.size = 48u;
  buffer.t_uid = 0x01234567u;
  buffer.t_version = 0x89abcdefu;
  buffer.t_size = 1024u;
  buffer.counter = 0ul;
  buffer.send_t = 0ul;
  buffer.recv_t = 0ul;
#endif  
  if (ret)
    {
      ret = (header->GetSize() == sizeof(buffer));
    }

  char string [1024] = STRING_UNDEFINED;

  if (ret)
    {
      log_info("TEST(CompoundType_Test, SerialiseInstance) - Convert to string ..");
      ret = (ccs::HelperTools::SerialiseInstance(std::const_pointer_cast<const ccs::types::CompoundType>(header), static_cast<void*>(&buffer), string, 1024u) == true); 
    }

  if (ret)
    {
      log_info("TEST(CompoundType_Test, SerialiseInstance) - .. '%s'", string);
    }

  ASSERT_EQ(true, ret);
}

TEST(CompoundType_Test, SDNPacket)
{

  ccs::types::CompoundType* header = static_cast<ccs::types::CompoundType*>(NULL);
  ccs::types::CompoundType* corpus = static_cast<ccs::types::CompoundType*>(NULL);
  ccs::types::CompoundType* footer = static_cast<ccs::types::CompoundType*>(NULL);

  bool ret = ((header = new (std::nothrow) ccs::types::CompoundType ("header")) != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      header->AddAttribute("uid", ccs::types::UnsignedInteger64);
      header->AddAttribute("size", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_uid", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_version", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_size", ccs::types::UnsignedInteger32);
      header->AddAttribute("counter", ccs::types::UnsignedInteger64);
      header->AddAttribute("send_time", ccs::types::UnsignedInteger64);
      header->AddAttribute("recv_time", ccs::types::UnsignedInteger64);
      ret = (header->GetSize() == 48u);
    }

  if (ret)
    {
      ret = ((corpus = new (std::nothrow) ccs::types::CompoundType ("corpus")) != static_cast<ccs::types::CompoundType*>(NULL));
    }

  if (ret)
    {
      corpus->AddAttribute("index", ccs::types::UnsignedInteger64)->AddAttribute("timestamp", ccs::types::UnsignedInteger64);
      corpus->AddAttribute("buffer", ccs::HelperTools::NewArrayType("array", ccs::types::UnsignedInteger8, 64u));
      ret = (corpus->GetSize() == 80u);
    }

  if (ret)
    {
      ret = ((footer = new (std::nothrow) ccs::types::CompoundType ("footer")) != static_cast<ccs::types::CompoundType*>(NULL));
    }

  if (ret)
    {
      footer->AddAttribute("uid", ccs::types::UnsignedInteger64)->AddAttribute("size", ccs::types::UnsignedInteger32);
      footer->AddAttribute("source", ccs::types::UnsignedInteger32)->AddAttribute("crc", ccs::types::UnsignedInteger32);
      ret = (footer->GetSize() == 20u);
    }

  std::shared_ptr<ccs::types::CompoundType> packet (new (std::nothrow) ccs::types::CompoundType ("packet"));

  if (ret)
    {
      ret = (packet ? true : false);
    }

  if (ret)
    {
      packet->AddAttribute("header", header)->AddAttribute("corpus", corpus)->AddAttribute("footer", footer);
      ret = (packet->GetSize() == 148u);
    }
#if 0
  if (ret)
    {
      ccs::types::CompoundType flattened = ccs::HelperTools::Flatten(packet.get());
      ret = (flattened.GetAttributeNumber() == 78u);

      for (uint_t index = 0u; index < flattened.GetAttributeNumber() && ret; index += 1u)
	{
	  log_info("TEST(CompoundType_Test, SDNPacket) - Type has attribute '%s' of type '%s' at offset '%u'", flattened.GetAttributeName(index), flattened.GetAttributeType(index)->GetName(), flattened.GetAttributeOffset(index));
	  ret = (ccs::HelperTools::IsScalarType(flattened.GetAttributeType(index)));
	}
    }
#endif
  void* instance = static_cast<void*>(NULL);

  if (ret)
    {
      log_info("TEST(CompoundType_Test, SDNPacket) - Instantiate packet");
      ret = ((instance = packet->CreateInstance()) != static_cast<void*>(NULL)); 
    }

  // ToDo - Verify offsets, etc.

  char string [1024] = STRING_UNDEFINED;

  if (ret)
    {
      log_info("TEST(CompoundType_Test, SDNPacket) - Convert to string ..");
      ret = (ccs::HelperTools::SerialiseInstance(std::const_pointer_cast<const ccs::types::CompoundType>(packet), instance, string, 1024u) == true); 
    }

  if (ret)
    {
      log_info("TEST(CompoundType_Test, SDNPacket) - .. '%s'", string);
    }

  if (ret)
    {
      log_info("TEST(CompoundType_Test, SDNPacket) - Delete packet instance");
      packet->DeleteInstance(instance); 
    }

  // Since v1.3.3 - Type composition is managed through smart pointers ..
  // .. AddAttribute(AnyType*) converts to a smart pointer which gets deleted
  // autonomously. Deleting the top most type instance is sufficient.
  //delete header;
  //delete corpus;
  //delete footer;
  //delete packet;

  ASSERT_EQ(true, ret);
}


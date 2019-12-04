/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/ArrayType-tests.cpp $
* $Id: ArrayType-tests.cpp 100795 2019-07-16 15:11:46Z bauvirb $
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

#include "AnyType.h" // Introspectable type definition (base class) ..
#include "AnyTypeHelper.h" // .. associated helper routines

#include "ArrayType.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition
  
TEST(ArrayType_Test, Constructor_default)
{
  ccs::types::ArrayType type;

  bool ret = ((static_cast<bool>(type.GetElementType()) == false) &&
	      (0u == type.GetSize()) &&
	      (0u == type.GetMultiplicity()));

  if (ret)
    {
      type.SetName("array:uint8");
      type.SetElementType(ccs::types::UnsignedInteger8);
      type.SetMultiplicity(8u);

      ret = ((type.IsType("array:uint8") == true) &&
	     (type.GetMultiplicity() == 8u) &&
	     (type.GetSize() == 8u) &&
	     (ccs::types::UnsignedInteger8 == type.GetElementType()));
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Constructor_array)
{
  ccs::types::ArrayType type ("array:uint8", ccs::types::UnsignedInteger8, 8u);

  bool ret = ((type.IsType("array:uint8") == true) &&
	     (type.GetMultiplicity() == 8u) &&
	     (type.GetSize() == 8u) &&
	     (ccs::types::UnsignedInteger8 == type.GetElementType()));

  if (ret) // With reference
    {
      ccs::types::ArrayType multi ("array:array", type, 8u);

      ret = (multi.GetSize() == 64u);
    }

  if (ret) // With pointer
    {
      ccs::types::ArrayType* copy = new (std::nothrow) ccs::types::ArrayType (type);
      ccs::types::ArrayType multi ("array:array", copy, 8u);

      ret = (multi.GetSize() == 64u);
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Constructor_struct)
{
  ccs::types::CompoundType type; 
  type.AddAttribute("counter", ccs::types::UnsignedInteger64);
  type.AddAttribute("timestamp", ccs::types::UnsignedInteger64);
  type.AddAttribute("reserved", ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u));

  bool ret = true;

  if (ret) // With reference
    {
      ccs::types::ArrayType multi ("array:struct", type, 8u);

      ret = (multi.GetSize() == 192u);
    }

  if (ret) // With pointer
    {
      ccs::types::CompoundType* copy = new (std::nothrow) ccs::types::CompoundType (type);
      ccs::types::ArrayType multi ("array:struct", copy, 8u);

      ret = (multi.GetSize() == 192u);
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Constructor_scalar)
{
  ccs::types::ArrayType type ("array:uint8", ccs::types::UnsignedInteger8, 8u);

  bool ret = ((type.IsType("array:uint8") == true) &&
	     (type.GetMultiplicity() == 8u) &&
	     (type.GetSize() == 8u) &&
	     (ccs::types::UnsignedInteger8 == type.GetElementType()));

  if (ret) // With reference
    {
      ccs::types::ScalarType scalar ("bitmap", 4u);
      ccs::types::ArrayType bitmap ("array:bitmap", scalar, 8u);

      ret = (bitmap.GetSize() == 32u);
    }

  if (ret) // With pointer
    {
      ccs::types::AnyType* scalar = new (std::nothrow)  ccs::types::ScalarType ("bitmap", 4u);
      ccs::types::ArrayType bitmap ("array:bitmap", scalar, 8u);

      ret = (bitmap.GetSize() == 32u);
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Constructor_copy)
{
  ccs::types::ArrayType base ("array:uint8", ccs::types::UnsignedInteger8, 8u);
  ccs::types::ArrayType* type = new (std::nothrow) ccs::types::ArrayType (base);

  bool ret = (type != static_cast<ccs::types::ArrayType*>(NULL));

  if (ret)
    {
      ret = ((type->IsType("array:uint8") == true) &&
	     (type->GetMultiplicity() == base.GetMultiplicity()) &&
	     (type->GetSize() == base.GetSize()) &&
	     (ccs::types::UnsignedInteger8 == type->GetElementType()));
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Operator_comparison)
{
  ccs::types::ArrayType base ("array:uint8", ccs::types::UnsignedInteger8, 8u);
  ccs::types::ArrayType* type = ccs::HelperTools::NewArrayType("test::att::Compare_t", ccs::types::UnsignedInteger8, 8u);

  bool ret = (base == *type); // Comparison

  // Comparison from base type
  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::Register(type);
    }

  if (ret) 
    {
      std::shared_ptr<const ccs::types::AnyType> ref = ccs::types::GlobalTypeDatabase::GetType("test::att::Compare_t");
      ret = static_cast<bool>(ref);

      if (ret)
	{
	  ret = (base == *ref);
	}
    }

  // Etc.

  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::Remove("test::att::Compare_t");
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Operator_copy)
{
  ccs::types::ArrayType base ("array:uint8", ccs::types::UnsignedInteger8, 8u);
  ccs::types::ArrayType type;

  type = base;

  bool ret = ((type.IsType("array:uint8") == true) &&
	     (type.GetMultiplicity() == base.GetMultiplicity()) &&
	     (type.GetSize() == base.GetSize()) &&
	     (ccs::types::UnsignedInteger8 == type.GetElementType()));

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, RTTI_frombase)
{
  ccs::types::AnyType* base = ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8);

  bool ret = (base != static_cast<ccs::types::AnyType*>(NULL));

  if (ret)
    {
      ret = ccs::HelperTools::Is<ccs::types::ArrayType>(base);
    }

  if (ret)
    {
      ccs::types::ArrayType* type = dynamic_cast<ccs::types::ArrayType*>(base);
      ret = (type != static_cast<ccs::types::ArrayType*>(NULL));
    }

  if (ret)
    {
      delete base;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, HasElement)
{
  ccs::types::ArrayType* type = new (std::nothrow) ccs::types::ArrayType ("array:uint8", ccs::types::UnsignedInteger8, 8u);

  bool ret = (type != static_cast<ccs::types::ArrayType*>(NULL));

  for (uint_t index = 0u; index < type->GetElementNumber() && ret; index += 1u)
    {
      ret = (type->HasElement(index) == true);
    }

  if (ret)
    {
      ret = (type->HasElement(type->GetElementNumber()) == false);
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, GetElementOffset)
{
  ccs::types::ArrayType* type = new (std::nothrow) ccs::types::ArrayType ("array:uint8", ccs::types::UnsignedInteger8, 8u);

  bool ret = (type != static_cast<ccs::types::ArrayType*>(NULL));

  for (uint_t index = 0u; index < type->GetElementNumber() && ret; index += 1u)
    {
      ret = (type->GetElementOffset(index) == index * type->GetElementType()->GetSize());
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, GetElementReference)
{
  ccs::types::ArrayType* type = new (std::nothrow) ccs::types::ArrayType ("array:uint8", ccs::types::UnsignedInteger8, 8u);

  bool ret = (type != static_cast<ccs::types::ArrayType*>(NULL));

  void* buffer = static_cast<void*>(NULL);

  if (ret)
    {
      ret = (static_cast<void*>(NULL) != (buffer = type->CreateInstance()));
    }

  for (uint_t index = 0u; index < type->GetElementNumber() && ret; index += 1u)
    {
      ret = (type->GetElementReference(buffer, index) == 
	     static_cast<void*>(static_cast<uint8_t*>(buffer) + index * type->GetElementType()->GetSize()));
    }

  if (ret)
    {
      type->DeleteInstance(buffer);
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, ToNetworkByteOrder_struct)
{
  ccs::types::CompoundType* comp = (new (std::nothrow) ccs::types::CompoundType)->AddAttribute("counter", ccs::types::UnsignedInteger64)->AddAttribute("timestamp", ccs::types::UnsignedInteger64)->AddAttribute("reserved", ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u));
  ccs::types::ArrayType* type = ccs::HelperTools::NewArrayType("array:struct", comp)->SetMultiplicity(8u);

  bool ret = (type != static_cast<ccs::types::ArrayType*>(NULL));

  struct { 
    uint64_t counter;
    uint64_t timestamp;
    uint8_t reserved [8];
  } buffer [8] = { { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } } };
  
  if (ret)
    {
      ret = (type->GetSize() == sizeof(buffer));
    }

  if (ret)
    {
      log_info("TEST(ArrayType_Test, ToNetworkByteOrder_struct) - Convert to network byte order");
      ret = (type->ToNetworkByteOrder(static_cast<void*>(buffer)) == true); 
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[0].counter == 0xefcdab8967452301ul);
      else
	ret = (buffer[0].counter == 0x0123456789abcdeful);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[0].timestamp == 0x0123456789abcdeful);
      else
	ret = (buffer[0].timestamp == 0xefcdab8967452301ul);
    }

  if (ret)
    {
      for (uint_t index = 0u; ((index < 8u) && ret); index += 1u)
	ret = (buffer[0].reserved[index] == static_cast<uint8_t>(index));
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[7].counter == 0xefcdab8967452301ul);
      else
	ret = (buffer[7].counter == 0x0123456789abcdeful);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[7].timestamp == 0x0123456789abcdeful);
      else
	ret = (buffer[7].timestamp == 0xefcdab8967452301ul);
    }

  if (ret)
    {
      for (uint_t index = 0u; ((index < 8u) && ret); index += 1u)
	ret = (buffer[7].reserved[index] == static_cast<uint8_t>(index));
    }

  if (ret)
    {
      // Since v1.3.3 - Type composition is managed through smart pointers ..
      // .. SetElementType(AnyType*) converts to a smart pointer which gets
      // deleted autonomously. Deleting the top most type instance is sufficient.
      delete type;
      //delete comp;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, FromNetworkByteOrder_struct)
{
  ccs::types::CompoundType* comp = (new (std::nothrow) ccs::types::CompoundType)->AddAttribute("counter", ccs::types::UnsignedInteger64)->AddAttribute("timestamp", ccs::types::UnsignedInteger64)->AddAttribute("reserved", ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u));
  ccs::types::ArrayType* type = ccs::HelperTools::NewArrayType("array:struct", comp)->SetMultiplicity(8u);

  bool ret = (type != static_cast<ccs::types::ArrayType*>(NULL));

  struct { 
    uint64_t counter;
    uint64_t timestamp;
    uint8_t reserved [8];
  } buffer [8] = { { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x0123456789abcdeful, 0xefcdab8967452301ul, { 0, 1, 2, 3, 4, 5, 6, 7 } } };
  
  if (ret)
    {
      ret = (type->GetSize() == sizeof(buffer));
    }

  if (ret)
    {
      log_info("TEST(ArrayType_Test, FromNetworkByteOrder_struct) - Convert from network byte order");
      ret = (type->FromNetworkByteOrder(static_cast<void*>(buffer)) == true); 
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[0].counter == 0xefcdab8967452301ul);
      else
	ret = (buffer[0].counter == 0x0123456789abcdeful);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[0].timestamp == 0x0123456789abcdeful);
      else
	ret = (buffer[0].timestamp == 0xefcdab8967452301ul);
    }

  if (ret)
    {
      for (uint_t index = 0u; ((index < 8u) && ret); index += 1u)
	ret = (buffer[0].reserved[index] == static_cast<uint8_t>(index));
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[7].counter == 0xefcdab8967452301ul);
      else
	ret = (buffer[7].counter == 0x0123456789abcdeful);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (buffer[7].timestamp == 0x0123456789abcdeful);
      else
	ret = (buffer[7].timestamp == 0xefcdab8967452301ul);
    }

  if (ret)
    {
      for (uint_t index = 0u; ((index < 8u) && ret); index += 1u)
	ret = (buffer[7].reserved[index] == static_cast<uint8_t>(index));
    }

  if (ret)
    {
      // Since v1.3.3 - Type composition is managed through smart pointers ..
      // .. SetElementType(AnyType*) converts to a smart pointer which gets
      // deleted autonomously. Deleting the top most type instance is sufficient.
      delete type;
      //delete comp;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, ParseInstance_scalar)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  std::shared_ptr<const ArrayType> type (NewArrayType("array:uint8", UnsignedInteger8)->SetMultiplicity(8u));

  bool ret = (type ? true : false);

  uint8 buffer [8];
  
  if (ret)
    {
      ret = (type->GetSize() == sizeof(buffer));
    }

 if (ret)
    {
      char string [] =
	"[ 0, 1, 2, 3, 4, 5, 6, 7 ]";

      ret = (0u < ParseInstance(type, &buffer, string));
    }

  if (ret)
    {
      ret = ((buffer[0] == 0) &&
	     (buffer[7] == 7));
    }

  // Etc.

 if (ret)
    {
      char string [] =
	"[ 0, 1, 2, Three, Four, 5, 6, 7 ]";

      ret = (0u == ParseInstance(type, &buffer, string));
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, ParseInstance_struct)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  CompoundType* comp = (new (std::nothrow) CompoundType)->AddAttribute("counter", UnsignedInteger64)->AddAttribute("timestamp", UnsignedInteger64)->AddAttribute("reserved", NewArrayType("array:uint8", UnsignedInteger8)->SetMultiplicity(8u));
  std::shared_ptr<const ArrayType> type (NewArrayType("array:struct", comp)->SetMultiplicity(8u));

  bool ret = (type ? true : false);

  struct { 
    uint64_t counter;
    uint64_t timestamp;
    uint8_t reserved [8];
  } buffer [8];
  
  if (ret)
    {
      ret = (type->GetSize() == sizeof(buffer));
    }

 if (ret)
    {
      char string [] =
	"["
	"  { \"counter\":0,\"timestamp\":8,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":1,\"timestamp\":9,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":2,\"timestamp\":10,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":3,\"timestamp\":11,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":4,\"timestamp\":12,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":5,\"timestamp\":13,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":6,\"timestamp\":14,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":7,\"timestamp\":15,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] }"
	"]";

      log_info("TEST(ArrayType_Test, ParseInstance_struct) - .. '%s'", string);
      ret = (0u < ParseInstance(type, &buffer, string));
    }

  if (ret)
    {
      ret = ((buffer[0].counter == 0x0ul) && 
	     (buffer[0].timestamp == 0x8ul) && 
	     (buffer[0].reserved[0] == 0));
    }

  if (ret)
    {
      ret = ((buffer[1].counter == 0x1ul) && 
	     (buffer[1].timestamp == 0x9ul) && 
	     (buffer[1].reserved[1] == 1));
    }

  // Etc.

 if (ret)
    {
      char string [] =
	"["
	"  { \"counter\":0,\"timestamp\":8,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":1,\"timestamp\":9,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] },"
	"  { \"counter\":2,\"timestamp\":10,\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] }"
	"]"; // WARNING

      log_info("TEST(ArrayType_Test, ParseInstance_struct) - .. '%s'", string);
      ret = (0u == ParseInstance(type, &buffer, string));
    }

  if (ret)
    {
      // Since v1.3.3 - Type composition is managed through smart pointers ..
      // .. SetElementType(AnyType*) converts to a smart pointer which gets
      // deleted autonomously. Deleting the top most type instance is sufficient.
      //delete type;
      //delete comp;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, SerialiseInstance_struct)
{
  using namespace ccs::types;
  using namespace ccs::HelperTools;

  CompoundType* comp = (new (std::nothrow) CompoundType)->AddAttribute("counter", UnsignedInteger64)->AddAttribute("timestamp", UnsignedInteger64)->AddAttribute("reserved", NewArrayType("array:uint8", UnsignedInteger8)->SetMultiplicity(8u));
  std::shared_ptr<const ArrayType> type (NewArrayType("array:struct", comp)->SetMultiplicity(8u));

  bool ret = (type ? true : false);

  struct { 
    uint64_t counter;
    uint64_t timestamp;
    uint8_t reserved [8];
  } buffer [8] = { { 0x0ul, 0x8ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x1ul, 0x9ul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x2ul, 0xaul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x3ul, 0xbul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x4ul, 0xcul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x5ul, 0xdul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x6ul, 0xeul, { 0, 1, 2, 3, 4, 5, 6, 7 } },
		   { 0x7ul, 0xful, { 0, 1, 2, 3, 4, 5, 6, 7 } } };
  
  if (ret)
    {
      ret = (type->GetSize() == sizeof(buffer));
    }

  char string [1024] = STRING_UNDEFINED;

  if (ret)
    {
      log_info("TEST(ArrayType_Test, SerialiseInstance_struct) - Convert to string ...");
      ret = (true == SerialiseInstance(type, static_cast<void*>(&buffer), string, 1024u)); 
    }

  if (ret)
    {
      log_info("TEST(ArrayType_Test, SerialiseInstance_struct) - .. '%s'", string);
    }

  if (ret)
    {
      // Since v1.3.3 - Type composition is managed through smart pointers ..
      // .. SetElementType(AnyType*) converts to a smart pointer which gets
      // deleted autonomously. Deleting the top most type instance is sufficient.
      //delete type;
      //delete comp;
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Flatten)
{
  std::shared_ptr<ccs::types::ArrayType> type = std::shared_ptr<ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u));

  bool ret = (type ? true : false);

  if (ret)
    {
      std::shared_ptr<const ccs::types::CompoundType> flattened = ccs::HelperTools::Flatten(std::const_pointer_cast<const ccs::types::ArrayType>(type));
      ret = (flattened->GetAttributeNumber() == 8u);

      for (uint_t index = 0u; index < flattened->GetAttributeNumber() && ret; index += 1u)
	{
	  log_info("TEST(ArrayType_Test, Flatten) - Type has attribute '%s' of type '%s' at offset '%u'", flattened->GetAttributeName(index), flattened->GetAttributeType(index)->GetName(), flattened->GetAttributeOffset(index));
	  ret = (ccs::HelperTools::IsScalarType(flattened->GetAttributeType(index)));
	}
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Flatten_2D)
{
  std::shared_ptr<ccs::types::ArrayType> type = std::shared_ptr<ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("array:array", ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u))->SetMultiplicity(8u));

  bool ret = (type ? true : false);

  if (ret)
    {
      std::shared_ptr<const ccs::types::CompoundType> flattened = ccs::HelperTools::Flatten(std::const_pointer_cast<const ccs::types::ArrayType>(type));
      ret = (flattened->GetAttributeNumber() == 64u);

      for (uint_t index = 0u; index < flattened->GetAttributeNumber() && ret; index += 1u)
	{
	  log_info("TEST(ArrayType_Test, Flatten_2D) - Type has attribute '%s' of type '%s' at offset '%u'", flattened->GetAttributeName(index), flattened->GetAttributeType(index)->GetName(), flattened->GetAttributeOffset(index));
	  ret = (ccs::HelperTools::IsScalarType(flattened->GetAttributeType(index)));
	}
    }

  ASSERT_EQ(true, ret);
}

TEST(ArrayType_Test, Flatten_struct) // Array of CompoundType which includes itself as attribute a ScalarArray
{
  ccs::types::CompoundType* comp = (new (std::nothrow) ccs::types::CompoundType)->AddAttribute("counter", ccs::types::UnsignedInteger64)->AddAttribute("timestamp", ccs::types::UnsignedInteger64)->AddAttribute("reserved", ccs::HelperTools::NewArrayType("array:uint8", ccs::types::UnsignedInteger8)->SetMultiplicity(8u));
  std::shared_ptr<ccs::types::ArrayType> type = std::shared_ptr<ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("array:struct", comp)->SetMultiplicity(8u));

  bool ret = (type ? true : false);

  if (ret)
    {
      std::shared_ptr<const ccs::types::CompoundType> flattened = ccs::HelperTools::Flatten(std::const_pointer_cast<const ccs::types::ArrayType>(type));
      ret = (flattened->GetAttributeNumber() == 80u);

      for (uint_t index = 0u; index < flattened->GetAttributeNumber() && ret; index += 1u)
	{
	  log_info("TEST(ArrayType_Test, Flatten_struct) - Type has attribute '%s' of type '%s' at offset '%u'", flattened->GetAttributeName(index), flattened->GetAttributeType(index)->GetName(), flattened->GetAttributeOffset(index));
	  ret = (ccs::HelperTools::IsScalarType(flattened->GetAttributeType(index)));
	}
    }

  ASSERT_EQ(true, ret);
}


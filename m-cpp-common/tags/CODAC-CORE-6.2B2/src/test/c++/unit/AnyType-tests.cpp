/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/AnyType-tests.cpp $
* $Id: AnyType-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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
#include "AnyType.h"
#include "CompoundType.h"

// Constants

// Type definition

class AnyType_Test : public ccs::types::AnyType // Need to specialise abstract class
{

  public:
    virtual bool ToNetworkByteOrder (void* ref) const { return false; };
    virtual bool FromNetworkByteOrder (void* ref) const { return false; };
    virtual bool ParseInstance (void* ref, const char* buffer) const { return false; };
    virtual bool SerialiseInstance (const void* ref, char* buffer, ccs::types::uint32 size = STRING_MAX_LENGTH) const { return false; };
    virtual bool operator== (const AnyType& type) const { 
      return (this->AnyType::GetSize() == type.GetSize()); 
    };
    AnyType_Test& operator= (const AnyType& type) { 
      this->AnyType::SetSize(type.GetSize());
      this->AnyType::SetName(type.GetName());
    };
    AnyType_Test (void) : AnyType() {};
    AnyType_Test (const ccs::types::AnyType& type) : AnyType() {
      this->AnyType::SetSize(type.GetSize());
      this->AnyType::SetName(type.GetName());
    };
    AnyType_Test (const char* type, ccs::types::uint32 size) : AnyType(type, size) {};

  protected:

};

// Global variables

// Function declaration

// Function definition
  
TEST(AnyType_Test, Constructor_default)
{
  ccs::types::AnyType* type = new (std::nothrow) AnyType_Test;

  bool ret = (static_cast<ccs::types::AnyType*>(NULL) != type);

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyType_Test, Constructor_init)
{
  ccs::types::AnyType* type = new (std::nothrow) AnyType_Test ("ccs::types::AnyType_Test", 64u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  if (ret)
    {
      ret = ((type->GetSize() == 64u) && (type->IsType("ccs::types::AnyType_Test") == true));
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyType_Test, Constructor_copy)
{
  ccs::types::AnyType* type = new (std::nothrow) AnyType_Test ("ccs::types::AnyType_Test", 64u);

  bool ret = (static_cast<ccs::types::AnyType*>(NULL) != type);

  if (ret)
    {
      AnyType_Test copy (*type);
      ret = ((copy.GetSize() == 64u) && 
	     (copy.IsType("ccs::types::AnyType_Test") == true) && 
	     (copy == *type));
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyType_Test, SetSize)
{
  ccs::types::AnyType* type = new (std::nothrow) AnyType_Test;

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  if (ret)
    {
      type->SetSize(64u);
      ret = ((type->GetSize() != 0u) && (type->GetSize() == 64u));
    }

   if (ret)
    {
      delete type;
    }

 ASSERT_EQ(true, ret);
}

TEST(AnyType_Test, SetName)
{
  ccs::types::AnyType* type = new (std::nothrow) AnyType_Test;

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  if (ret)
    {
      type->SetName("ccs::types::AnyType_Test");
      ret = ((type->IsType("AnyType") == false) && 
	     (type->IsType("ccs::types::AnyType_Test") == true) &&
	     (strcmp(type->GetName(), "ccs::types::AnyType_Test") == 0));
    }

   if (ret)
    {
      delete type;
    }

 ASSERT_EQ(true, ret);
}

TEST(AnyType_Test, Operator_copy)
{
  ccs::types::AnyType* type = new (std::nothrow) AnyType_Test ("ccs::types::AnyType_Test", 64u);

  bool ret = (type != static_cast<ccs::types::AnyType*>(NULL));

  if (ret)
    {
      AnyType_Test copy; copy = *type;
      ret = ((copy.GetSize() == 64u) && 
	     (copy.IsType("ccs::types::AnyType_Test") == true) && 
	     (copy == *type));
    }

  if (ret)
    {
      delete type;
    }

  ASSERT_EQ(true, ret);
}

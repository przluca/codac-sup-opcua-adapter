/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/BasicTypes-tests.cpp $
* $Id: BasicTypes-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include "BasicTypes.h" // Misc. type definition

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition

TEST(BasicTypes_Test, Size)
{

  bool ret = true;

  if (ret)
    {
      ret = (sizeof(ccs::types::boolean) == 1);
    }

  if (ret)
    {
      ret = ((sizeof(ccs::types::char8) == 1) && (sizeof(ccs::types::int8) == 1) && (sizeof(ccs::types::uint8) == 1));
    }

  if (ret)
    {
      ret = ((sizeof(ccs::types::int16) == 2) && (sizeof(ccs::types::uint16) == 2));
    }

  if (ret)
    {
      ret = ((sizeof(ccs::types::int32) == 4) && (sizeof(ccs::types::uint32) == 4));
    }

  if (ret)
    {
      ret = ((sizeof(ccs::types::int64) == 8) && (sizeof(ccs::types::uint64) == 8));
    }

  if (ret)
    {
      ret = (sizeof(ccs::types::float32) == 4);
    }

  if (ret)
    {
      ret = (sizeof(ccs::types::float64) == 8);
    }

  if (ret)
    {
      ret = (sizeof(ccs::types::string) == ccs::types::MaxStringLength * sizeof(ccs::types::char8));
    }

  ASSERT_EQ(ret, true);
}

TEST(BasicTypes_Test, GetSize_name)
{

  bool ret = true;

  if (ret)
    {
      ret = (ccs::types::GetSize("bool") == 1);
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize("char8") == 1) && (ccs::types::GetSize("int8") == 1) && (ccs::types::GetSize("uint8") == 1));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize("int16") == 2) && (ccs::types::GetSize("uint16") == 2));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize("int32") == 4) && (ccs::types::GetSize("uint32") == 4));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize("int64") == 8) && (ccs::types::GetSize("uint64") == 8));
    }

  if (ret)
    {
      ret = (ccs::types::GetSize("float32") == 4);
    }

  if (ret)
    {
      ret = (ccs::types::GetSize("float64") == 8);
    }

  if (ret)
    {
      ret = (ccs::types::GetSize("string") == ccs::types::MaxStringLength * sizeof(ccs::types::char8));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize("") == 0) && (ccs::types::GetSize("undefined") == 0) && (ccs::types::GetSize(NULL) == 0));
    }

  ASSERT_EQ(ret, true);
}

TEST(BasicTypes_Test, GetSize_identifier)
{

  bool ret = true;

  if (ret)
    {
      ret = (ccs::types::GetSize(ccs::types::GetIdentifier("bool")) == 1);
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize(ccs::types::GetIdentifier("char8")) == 1) && (ccs::types::GetSize(ccs::types::GetIdentifier("int8")) == 1) && (ccs::types::GetSize(ccs::types::GetIdentifier("uint8")) == 1));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize(ccs::types::GetIdentifier("int16")) == 2) && (ccs::types::GetSize(ccs::types::GetIdentifier("uint16")) == 2));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize(ccs::types::GetIdentifier("int32")) == 4) && (ccs::types::GetSize(ccs::types::GetIdentifier("uint32")) == 4));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize(ccs::types::GetIdentifier("int64")) == 8) && (ccs::types::GetSize(ccs::types::GetIdentifier("uint64")) == 8));
    }

  if (ret)
    {
      ret = (ccs::types::GetSize(ccs::types::GetIdentifier("float32")) == 4);
    }

  if (ret)
    {
      ret = (ccs::types::GetSize(ccs::types::GetIdentifier("float64")) == 8);
    }

  if (ret)
    {
      ret = (ccs::types::GetSize(ccs::types::GetIdentifier("string")) == ccs::types::MaxStringLength * sizeof(ccs::types::char8));
    }

  if (ret)
    {
      ret = ((ccs::types::GetSize(ccs::types::GetIdentifier("")) == 0) && (ccs::types::GetSize(ccs::types::GetIdentifier("undefined")) == 0) && (ccs::types::GetSize(ccs::types::GetIdentifier(NULL)) == 0));
    }

  ASSERT_EQ(ret, true);
}


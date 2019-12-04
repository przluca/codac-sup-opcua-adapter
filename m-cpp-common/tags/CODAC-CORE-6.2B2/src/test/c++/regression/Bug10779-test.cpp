/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10779-test.cpp $
* $Id: Bug10779-test.cpp 101290 2019-08-02 06:55:03Z bauvirb $
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

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition

TEST(Bug10779_Test, Size)
{
  bool ret = true;

  if (ret)
    {
      ret = ((sizeof(ccs::types::int8) == 1) && (sizeof(ccs::types::uint8) == 1));
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

  ASSERT_EQ(ret, true);
}

TEST(Bug10779_Test, Float32)
{
  bool ret = (sizeof(ccs::types::float32) == sizeof(float));

  if(ret)
    {
      float value = 1.0F / 3.0F;
      ccs::types::float32 cast = static_cast<ccs::types::float32>(1.0F / 3.0F);
      ret = (cast == value);
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10779_Test, Float64)
{
  bool ret = (sizeof(ccs::types::float64) == sizeof(double));

  if(ret)
    {
      double value = 1.0 / 3.0;
      ccs::types::float64 cast = static_cast<ccs::types::float64>(1.0 / 3.0);
      ret = (cast == value);
    }

  ASSERT_EQ(ret, true);
}

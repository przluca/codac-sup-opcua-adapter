/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug12047-test.cpp $
* $Id: Bug12047-test.cpp 101290 2019-08-02 06:55:03Z bauvirb $
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

TEST(Bug12047_Test, GetSize_name)
{
  bool ret = (1u == ccs::types::GetSize("char"));

  ASSERT_EQ(ret, true);
}

TEST(Bug12047_Test, GetIdentifier_name)
{
  bool ret = (ccs::types::CHAR8 == ccs::types::GetIdentifier("char"));

  ASSERT_EQ(ret, true);
}


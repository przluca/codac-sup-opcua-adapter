/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug11743-tests.cpp $
* $Id: Bug11743-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include "tools.h" // Misc. type definition

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition

TEST(Bug11743_Test, legacy)
{

  uint64_t curr_time = get_time();

  struct timespec timestruct; ns_to_timespec(curr_time, timestruct);

  uint64_t conv_time = timespec_to_ns(timestruct);

  bool ret = (curr_time == conv_time);

  ASSERT_EQ(true, ret);
}

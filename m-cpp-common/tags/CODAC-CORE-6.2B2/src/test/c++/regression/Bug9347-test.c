/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug9347-test.c $
* $Id: Bug9347-test.c 100703 2019-07-11 15:23:56Z bauvirb $
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

// Local header files

#include "types.h"
#include "tools.h"
#include "log-api.h"

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition

bool bug9347_test (void)
{

  uint64_t curr_time = get_time();
  uint64_t till_time = ceil_time(curr_time);

  log_info("bug9347_test - Current time is '%lu'", curr_time);

  curr_time = wait_until(till_time, 10000ul);

  log_info("bug9347_test - wait_until '%lu' returned at '%lu'", till_time, curr_time);

  return true;

}

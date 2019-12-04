/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug12069-test.cpp $
* $Id: Bug12069-test.cpp 101603 2019-08-19 11:31:33Z bauvirb $
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

#include "AnyThread.h"
#include "log-api.h"
#include "SysTools.h"
#include "TimeTools.h"

// Constants

// Type definition

class Bug12069Thread : public ccs::base::SynchronisedThread
{

  public:

    Bug12069Thread (void) : ccs::base::SynchronisedThread("bug-12069") {};
    virtual ~Bug12069Thread (void) {};

    virtual void Execute (void) {

      ccs::types::int32 pid = ccs::HelperTools::GetProcessId("systemd");
      log_info("TEST(Bug12069_Test, Re_entrancy) - Process id is '%d'", pid);

    };

};

// Global variables

// Function declaration

// Function definition

TEST(Bug12069_Test, Re_entrancy)
{
  Bug12069Thread t1;
  Bug12069Thread t2;
  Bug12069Thread t3;

  bool ret = (t1.Launch() && t2.Launch() && t3.Launch());

  if (ret)
    {
      (void)ccs::HelperTools::SleepFor(1000000000ul);
    }

  ASSERT_EQ(ret, true);
}


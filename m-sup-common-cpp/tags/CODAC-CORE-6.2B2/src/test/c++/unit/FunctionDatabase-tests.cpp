/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/FunctionDatabase-tests.cpp $
* $Id: FunctionDatabase-tests.cpp 99275 2019-04-25 06:34:12Z bauvirb $
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

#include <common/BasicTypes.h> // Misc. type definition
#include <common/SysTools.h> // Misc. helper functions

#include <common/log-api.h> // Syslog wrapper routines

// Local header files

#include "FunctionDatabase.h"

// Constants

// Type definition

class FunctionDatabase_Test
{
};

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition
  
TEST(FunctionDatabase_Test, Constructor_default)
{
  // Function database is implicitly instantiated when calling GFDB methods
  bool ret = !ccs::types::GlobalFunctionDatabase::IsValid("");

  ASSERT_EQ(true, ret);
}


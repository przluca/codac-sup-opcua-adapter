/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Hash-tests.cpp $
* $Id: Hash-tests.cpp 100870 2019-07-18 06:08:39Z bauvirb $
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

#include "Hash.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition

TEST(Hash_Test, SDNTopics_map)
{
  log_info("TEST(Hash_Test, SDNTopics_map) - Hash of '%s' is '%hu'", "TEST-CPS-PF1:QVAL", ccs::HelperTools::Hash<ccs::types::uint16>("TEST-CPS-PF1:QVAL"));
  log_info("TEST(Hash_Test, SDNTopics_map) - Hash of '%s' is '%hu'", "TEST-CPS-PF2:QVAL", ccs::HelperTools::Hash<ccs::types::uint16>("TEST-CPS-PF2:QVAL"));
  log_info("TEST(Hash_Test, SDNTopics_map) - Hash of '%s' is '%hu'", "TEST-CPS-MCS:QSUM", ccs::HelperTools::Hash<ccs::types::uint16>("TEST-CPS-MCS:QSUM"));

  bool ret = ((ccs::HelperTools::Hash<ccs::types::uint16>("TEST-CPS-PF1:QVAL") == 17279u) &&
	      (ccs::HelperTools::Hash<ccs::types::uint16>("TEST-CPS-PF2:QVAL") == 59660u) && 
	      (ccs::HelperTools::Hash<ccs::types::uint16>("TEST-CPS-MCS:QSUM") == 49628u));

  ASSERT_EQ(true, ret);
}


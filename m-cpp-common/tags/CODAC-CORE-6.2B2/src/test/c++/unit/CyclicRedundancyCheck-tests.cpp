/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/CyclicRedundancyCheck-tests.cpp $
* $Id: CyclicRedundancyCheck-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include "CyclicRedundancyCheck.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition

TEST(CyclicRedundancyCheck_Test, Default)
{
  bool ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(static_cast<ccs::types::uint8*>(NULL), 0u) == 0u);

  ASSERT_EQ(true, ret);
}

TEST(CyclicRedundancyCheck_Test, Buffer_zero)
{
  ccs::types::uint8 buffer [64]; memset(buffer, 0, 64u);

  log_info("TEST(CyclicRedundancyCheck_Test, Buffer_zero) - CyclicRedundancyCheck<ccs::types::uint32> is '%u'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u));

  bool ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u) == 0x758d6336u);

  ASSERT_EQ(true, ret);
}

TEST(CyclicRedundancyCheck_Test, Buffer_msg)
{
  ccs::types::uint8 buffer [64]; memset(buffer, 0, 64u);
  ccs::HelperTools::SafeStringCopy((char*) buffer, "This is a test message", 64u); 

  log_info("TEST(CyclicRedundancyCheck_Test, Buffer_msg) - CyclicRedundancyCheck<ccs::types::uint32> is '%u'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u));

  bool ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u) == 0x81775202u);

  ASSERT_EQ(true, ret);
}

TEST(CyclicRedundancyCheck_Test, Buffer_seed)
{
  ccs::types::uint8 buffer [64]; memset(buffer, 0, 64u);

  log_info("TEST(CyclicRedundancyCheck_Test, Buffer_seed) - CyclicRedundancyCheck<ccs::types::uint32> is '%u'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u, 0u));
  log_info("TEST(CyclicRedundancyCheck_Test, Buffer_seed) - CyclicRedundancyCheck<ccs::types::uint32> is '%u'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u, 1u));
  log_info("TEST(CyclicRedundancyCheck_Test, Buffer_seed) - CyclicRedundancyCheck<ccs::types::uint32> is '%u'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u, 2u));
  log_info("TEST(CyclicRedundancyCheck_Test, Buffer_seed) - CyclicRedundancyCheck<ccs::types::uint32> is '%u'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u, 3u));

  bool ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 64u, 1u) == 0x75eaf68fu);

  ASSERT_EQ(true, ret);
}

TEST(CyclicRedundancyCheck_Test, Rosetta)
{
  bool ret = true;

  ccs::types::uint8 buffer [64]; 

  /* From https://rosettacode.org/wiki/CRC-32
"The quick brown fox jumps over the lazy dog"
(to hex ...)
54686520717569636B2062726F776E20666F78206A756D7073206F76657220746865206C617A7920646F67 414FA339
[other useful test vectors]
0000000000000000000000000000000000000000000000000000000000000000 190A55AD
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF FF6CAB0B
000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F 91267E8A
  */

  if (ret)
    {
      memset(buffer, 0, 64u);
      
      log_info("TEST(CyclicRedundancyCheck_Test, Rosetta) - CyclicRedundancyCheck<ccs::types::uint32> is '0x%x'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 32u));
      
      ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 32u) == 0x190a55adu);
    }

  if (ret)
    {
      memset(buffer, 255, 64u);
      
      log_info("TEST(CyclicRedundancyCheck_Test, Rosetta) - CyclicRedundancyCheck<ccs::types::uint32> is '0x%x'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 32u));
      
      ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 32u) == 0xff6cab0bu);
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < 64u; index += 1u)
	{
	  buffer[index] = index;
	}
      
      log_info("TEST(CyclicRedundancyCheck_Test, Rosetta) - CyclicRedundancyCheck<ccs::types::uint32> is '0x%x'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 32u));
      
      ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, 32u) == 0x91267e8au);
    }

  if (ret)
    {
      ccs::HelperTools::SafeStringCopy(reinterpret_cast<char*>(buffer), "The quick brown fox jumps over the lazy dog", 64u);      

      log_info("TEST(CyclicRedundancyCheck_Test, Rosetta) - CyclicRedundancyCheck<ccs::types::uint32> is '0x%x'", ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, strlen(reinterpret_cast<char*>(buffer))));
      
      ret = (ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(buffer, strlen(reinterpret_cast<char*>(buffer))) == 0x414fa339u);
    }

  ASSERT_EQ(true, ret);
}


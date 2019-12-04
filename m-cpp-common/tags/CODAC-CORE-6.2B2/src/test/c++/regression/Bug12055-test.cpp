/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug12055-test.cpp $
* $Id: Bug12055-test.cpp 101457 2019-08-08 12:51:16Z bauvirb $
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

#include "log-api.h"
#include "LookUpTable.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

static ccs::base::NameValuePair<ccs::types::uint32> __table [] = 
{

  { "zero", 0u },
  { "one",  1u },
  { "two",  2u },
  { "three", 3u },
  { "four",  4u },
  { EOT_KEYWORD, 0 }

};

// Function declaration

// Function definition

TEST(Bug12055_Test, Remove_index)
{
  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> (__table);

  bool ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

  if (ret)
    {
      ret = (5u == table->GetSize());
    }

  if (ret)
    {
      ret = table->IsValid(2u);
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < table->GetSize(); index += 1u)
	{
	  ccs::types::uint32 value = 0u;
	  ret = table->GetElement(index, value);

	  if (ret)
	    {
	      log_info("TEST(Bug12055_Test, Remove_index) - Element ['%s, %u'] at index '%u'", table->GetName(index), value, index);
	    }
	}
    }

  if (ret)
    {
      log_info("TEST(Bug12055_Test, Remove_index) - Remove element ..");
      ret = table->Remove(2u);
    }

  if (ret)
    {
      ret = table->IsValid(2u); // Just another entry now
    }

  if (ret)
    {
      ret = (4u == table->GetSize());
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < table->GetSize(); index += 1u)
	{
	  ccs::types::uint32 value = 0u;
	  ret = table->GetElement(index, value);

	  if (ret)
	    {
	      log_info("TEST(Bug12055_Test, Remove_index) - Element ['%s, %u'] at index '%u'", table->GetName(index), value, index);
	    }
	}
    }

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug12055_Test, Remove_name)
{
  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> (__table);

  bool ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

  if (ret)
    {
      ret = (5u == table->GetSize());
    }

  if (ret)
    {
      ret = table->IsValid("two");
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < table->GetSize(); index += 1u)
	{
	  ccs::types::uint32 value = 0u;
	  ret = table->GetElement(index, value);

	  if (ret)
	    {
	      log_info("TEST(Bug12055_Test, Remove_name) - Element ['%s, %u'] at index '%u'", table->GetName(index), value, index);
	    }
	}
    }

  if (ret)
    {
      log_info("TEST(Bug12055_Test, Remove_name) - Remove element ..");
      ret = table->Remove("two");
    }

  if (ret)
    {
      ret = !table->IsValid("two");
    }

  if (ret)
    {
      ret = (4u == table->GetSize());
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < table->GetSize(); index += 1u)
	{
	  ccs::types::uint32 value = 0u;
	  ret = table->GetElement(index, value);

	  if (ret)
	    {
	      log_info("TEST(Bug12055_Test, Remove_name) - Element ['%s, %u'] at index '%u'", table->GetName(index), value, index);
	    }
	}
    }

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}

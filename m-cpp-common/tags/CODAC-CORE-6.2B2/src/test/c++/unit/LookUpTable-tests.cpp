/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/LookUpTable-tests.cpp $
* $Id: LookUpTable-tests.cpp 101457 2019-08-08 12:51:16Z bauvirb $
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
#include "tools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

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

TEST(LookUpTable_Test, Constructor)
{
  bool ret = false;

  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> ();

  ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}

TEST(LookUpTable_Test, Register)
{
  bool ret = false;

  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> ();

  ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

  if (ret)
    {
      ccs::types::uint32 value = 0u;
      ret = table->Register("zero",  value);
    }

  if (ret)
    {
      ccs::types::uint32 value = 1u;
      ret = table->Register("one",  value);
    }

  if (ret)
    {
      ccs::types::uint32 value = 2u;
      ret = table->Register("two",  value);
    }

  if (ret)
    {
      ccs::types::uint32 value = 3u;
      ret = table->Register("three",  value);
    }

  if (ret)
    {
      ccs::types::uint32 value = 4u;
      ret = table->Register("four",  value);
    }

  if (ret)
    {
      ccs::types::uint32 value = 4u;
      ret = !table->Register("four",  value); // Expect failure
    }

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
      ret = (2u == table->GetElement("two"));
    }

  if (ret)
    {
      ret = !table->IsValid("unexpected");
    }

  if (ret)
    {
      try
	{
	  ret = !(0u == table->GetElement("unexpected"));
	}
      catch (const std::exception& e)
	{
	  ret = true;
	}
    }

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}

TEST(LookUpTable_Test, GetIndex)
{
  bool ret = false;

  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> (__table);

  ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

  if (ret)
    {
      ret = (5u == table->GetSize());
    }

  if (ret)
    {
      ret = table->IsValid("zero");
    }

  if (ret)
    {
      log_info("TEST(LookUpTable_Test, GetIndex) - Index is '%u'", table->GetIndex("zero"));
      ret = (0u == table->GetElement(table->GetIndex("zero"))); // Do not assume indexes are particularly ordered
    }

  if (ret)
    {
      ret = table->IsValid("two");
    }

  if (ret)
    {
      log_info("TEST(LookUpTable_Test, GetIndex) - Index is '%u'", table->GetIndex("two"));
      ret = (2u == table->GetElement(table->GetIndex("two"))); // Do not assume indexes are particularly ordered
    }

  if (ret)
    {
      ret = !table->IsValid("unexpected");
    }

  if (ret)
    {
      try
	{
	  ret = !(0u == table->GetIndex("unexpected"));
	}
      catch (const std::exception& e)
	{
	  ret = true;
	}
    }

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}

TEST(LookUpTable_Test, GetElement_index)
{
  bool ret = false;

  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> (__table);

  ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

  if (ret)
    {
      ret = (5u == table->GetSize());
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < table->GetSize(); index += 1u)
	{
	  log_info("TEST(LookUpTable_Test, GetElement_index) - Element ['%s, %u'] at index '%u'", table->GetName(index), table->GetElement(index), index);
	}
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < table->GetSize(); index += 1u)
	{
	  ccs::types::uint32 value = 0u;
	  ret = table->GetElement(index, value);

	  if (ret)
	    {
	      log_info("TEST(LookUpTable_Test, GetElement_index) - Element ['%s, %u'] at index '%u'", table->GetName(index), value, index);
	    }
	}
    }

  if (ret)
    {
      try
	{
	  ret = !(0u == table->GetElement(table->GetSize()));
	}
      catch (const std::exception& e)
	{
	  ret = true;
	}
    }

  if (ret)
    {
      ccs::types::uint32 value = 0u;
      ret = !table->GetElement(table->GetSize(), value);
    }

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}

TEST(LookUpTable_Test, Remove_index)
{
  bool ret = false;

  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> (__table);

  ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

  if (ret)
    {
      ret = (5u == table->GetSize());
    }

  if (ret)
    {
      ret = table->IsValid(table->GetIndex("two"));
    }

  if (ret)
    {
      ret = table->Remove(table->GetIndex("two"));
    }

  if (ret)
    {
      try
	{
	  ret = !table->IsValid(table->GetIndex("two"));
	}
      catch (const std::exception& e)
	{
	  ret = true;
	}
    }

  if (ret)
    {
      ret = (4u == table->GetSize());
    }

  if (ret)
    {
      ret = table->Remove();
    }

  if (ret)
    {
      ret = (0u == table->GetSize());
    }

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}

TEST(LookUpTable_Test, Remove_name)
{
  bool ret = false;

  ccs::base::LookUpTable<ccs::types::uint32>* table = new (std::nothrow) ccs::base::LookUpTable<ccs::types::uint32> (__table);

  ret = (table != static_cast<ccs::base::LookUpTable<ccs::types::uint32>*>(NULL));

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
	      log_info("TEST(LookUpTable_Test, Remove_name) - Element ['%s, %u'] at index '%u'", table->GetName(index), value, index);
	    }
	}
    }

  if (ret)
    {
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
	      log_info("TEST(LookUpTable_Test, Remove_name) - Element ['%s, %u'] at index '%u'", table->GetName(index), value, index);
	    }
	}
    }

  if (ret)
    {
      delete table;
    }

  ASSERT_EQ(ret, true);
}


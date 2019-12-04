/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/AnyObject-tests.cpp $
* $Id: AnyObject-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include <new> // std::nothrow, etc.

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "AnyObject.h"
#include "ObjectDatabase.h"
#include "ObjectFactory.h"

// Constants

// Type definition

class AnyObject_Test : public ccs::base::AnyObject
{

  public:

  AnyObject_Test (void) : ccs::base::AnyObject() {

      this->ccs::base::AnyObject::SetInstanceType("ccs::test::AnyObject");
      this->ccs::base::AnyObject::SetInstanceName("ccs::test::AnyObject");

      ccs::base::GlobalObjectDatabase::Register(ccs::base::AnyObject::GetInstanceName(), this);

      return;

    };

  virtual ~AnyObject_Test (void) {

      log_info("AnyObject_Test::~AnyObject_Test - Removing instance ..");

      bool status = ccs::base::GlobalObjectDatabase::Remove(ccs::base::AnyObject::GetInstanceName());

      if (status)
	{
	  log_info("AnyObject_Test::~AnyObject_Test - .. success");
	}
      else
	{
	  log_error("AnyObject_Test::~AnyObject_Test - .. failure");
	}

      return;

    };

  protected:

};

// Global variables

REGISTER_CLASS_WITH_OBJECT_FACTORY(AnyObject_Test) // Auto-register constructor with the ObjectFactory

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition
  
TEST(AnyObject_Test, Constructor_default)
{
  using namespace ccs::base;
  using namespace ccs::HelperTools;

  AnyObject_Test test;

  bool ret = test.IsType("ccs::test::AnyObject");

  log_info("TEST(AnyObject_Test, Constructor_default) - Instance type '%u'", test.GetInstanceType());
  log_info("TEST(AnyObject_Test, Constructor_default) - Instance uid '%lu'", test.GetInstanceUId());

  ASSERT_EQ(true, ret);
}

TEST(AnyObject_Test, Constructor_copy)
{
  using namespace ccs::base;
  using namespace ccs::HelperTools;

  AnyObject_Test test;
  AnyObject copy (test);

  bool ret = copy.IsType("ccs::test::AnyObject");

  log_info("TEST(AnyObject_Test, Constructor_copy) - Instance type '%u'", copy.GetInstanceType());
  log_info("TEST(AnyObject_Test, Constructor_copy) - Instance uid '%lu'", copy.GetInstanceUId());

  ASSERT_EQ(true, ret);
}

TEST(AnyObject_Test, Database)
{
  using namespace ccs::base;
  using namespace ccs::HelperTools;

  AnyObject_Test* ref = new (std::nothrow) AnyObject_Test;

  bool ret = (static_cast<AnyObject_Test*>(NULL) != ref);

  if (ret)
    {
      ret = ccs::base::GlobalObjectDatabase::IsValid("ccs::test::AnyObject");
    }

  if (ret)
    {
      for (ccs::types::uint32 index = 0u; index < ccs::base::GlobalObjectDatabase::GetSize(); index += 1u)
	{
	  log_info("TEST(AnyObject_Test, Database) - Instance '%s' and index '%u'", ccs::base::GlobalObjectDatabase::GetInstance(index)->GetInstanceName(), index);
	}
    }

  if (ret)
    {
      delete ref;
      // cppcheck-suppress unreadVariable //  No static analysis on test code
      ref = static_cast<AnyObject_Test*>(NULL);
    }

  if (ret)
    {
      ret = !ccs::base::GlobalObjectDatabase::IsValid("ccs::test::AnyObject");
    }

  if (ret)
    {
      ret = (static_cast<ccs::base::AnyObject*>(NULL) == ccs::base::GlobalObjectDatabase::GetInstance("ccs::test::AnyObject"));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyObject_Test, Factory)
{
  using namespace ccs::base;
  using namespace ccs::HelperTools;

  bool ret = ccs::base::GlobalObjectFactory::IsValid("AnyObject_Test");

  ccs::base::AnyObject* ref = static_cast<ccs::base::AnyObject*>(NULL);

  if (ret)
    {
      ref = ccs::base::GlobalObjectFactory::Instantiate("AnyObject_Test");
      ret = (static_cast<ccs::base::AnyObject*>(NULL) != ref);
    }

  if (ret)
    {
      ret = (static_cast<ccs::base::AnyObject*>(NULL) != ccs::base::GlobalObjectDatabase::GetInstance("ccs::test::AnyObject"));
    }

  if (ret)
    {
      ret = ref->IsType("AnyObject_Test");
    }

  if (ret)
    {
      ret = (static_cast<AnyObject_Test*>(NULL) != dynamic_cast<AnyObject_Test*>(ref));
    }

  if (ret)
    {
      ret = ccs::base::GlobalObjectFactory::Terminate(ref);
    }

  if (ret)
    {
      ret = (static_cast<ccs::base::AnyObject*>(NULL) == ccs::base::GlobalObjectDatabase::GetInstance("ccs::test::AnyObject"));
    }

  ASSERT_EQ(true, ret);
}


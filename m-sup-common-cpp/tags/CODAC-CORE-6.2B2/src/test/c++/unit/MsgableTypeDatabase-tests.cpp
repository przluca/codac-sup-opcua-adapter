/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/MsgableTypeDatabase-tests.cpp $
* $Id: MsgableTypeDatabase-tests.cpp 100195 2019-06-19 13:35:27Z bauvirb $
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

#include <common/ObjectFactory.h>
#include <common/AnyTypeDatabase.h>

// Local header files

#include "MsgableTypeDatabase.h"

// Constants

// Type definition

// Function declaration

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function definition
 
TEST(MsgableTypeDatabase_Test, Constructor)
{
  sup::core::MsgableTypeDatabase* types = new (std::nothrow) sup::core::MsgableTypeDatabase ();

  bool ret = (static_cast<sup::core::MsgableTypeDatabase*>(NULL) != types);

  if (ret)
    {
      delete types;
    }

  ASSERT_EQ(true, ret);
}

TEST(MsgableTypeDatabase_Test, Factory)
{
  bool ret = ccs::base::GlobalObjectFactory::IsValid("sup::core::MsgableTypeDatabase");

  sup::core::MsgableTypeDatabase* types = static_cast<sup::core::MsgableTypeDatabase*>(NULL);

  if (ret)
    {
      types = dynamic_cast<sup::core::MsgableTypeDatabase*>(ccs::base::GlobalObjectFactory::Instantiate("sup::core::MsgableTypeDatabase"));
      ret = (static_cast<sup::core::MsgableTypeDatabase*>(NULL) != types);
    }

  if (ret)
    {
      ccs::base::GlobalObjectFactory::Terminate(dynamic_cast<ccs::base::AnyObject*>(types));
    }

  ASSERT_EQ(true, ret);
}

TEST(MsgableTypeDatabase_Test, SetParameter)
{
  sup::core::MsgableTypeDatabase* types = new (std::nothrow) sup::core::MsgableTypeDatabase ();

  bool ret = (static_cast<sup::core::MsgableTypeDatabase*>(NULL) != types);

  if (ret)
    {
      ret = types->SetParameter("verbose", "true");
    }

  if (ret)
    {
      ret = (false == types->SetParameter("", "")); // Expect failure
    }

  if (ret)
    {
      delete types;
    }

  ASSERT_EQ(true, ret);
}

TEST(MsgableTypeDatabase_Test, ProcessMessage)
{
  sup::core::MsgableTypeDatabase* types = new (std::nothrow) sup::core::MsgableTypeDatabase ();

  bool ret = (static_cast<sup::core::MsgableTypeDatabase*>(NULL) != types);

  if (ret)
    {
char buffer [] = 
"Register('"
"{\"type\":\"55a0::Sensor\","
" \"attributes\":[{\"enabled\":{\"type\":\"bool\",\"size\":1}},"
                 "{\"geometry\":{\"type\":\"55a0::Geometry\","
                                "\"attributes\":[{\"r\":{\"type\":\"float64\",\"size\":8}},"
                                                "{\"z\":{\"type\":\"float64\",\"size\":8}},"
                                                "{\"angle\":{\"type\":\"float64\",\"size\":8}}"
                                               "]"
                               "}"
                 "}"
                "]"
"}"
"')";
      ret = types->ProcessMessage(buffer);
    }

  if (ret)
    {
      ret = ccs::types::GlobalTypeDatabase::IsValid("55a0::Sensor");
    }

  if (ret)
    {
      ret = (false == types->ProcessMessage("")); // Expect failure
    }

  if (ret)
    {
      delete types;
    }

  ASSERT_EQ(true, ret);
}


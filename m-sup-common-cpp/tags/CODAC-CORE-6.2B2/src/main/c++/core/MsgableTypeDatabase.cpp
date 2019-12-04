/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/core/MsgableTypeDatabase.cpp $
* $Id: MsgableTypeDatabase.cpp 101405 2019-08-06 14:39:31Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
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

#include <new> // std::nothrow

#include <BasicTypes.h> // Misc. type definition
#include <SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.

#include <log-api.h> // Logging helper functions

#include <ObjectFactory.h>

#include <AnyType.h>
#include <AnyTypeHelper.h>

#include <AnyTypeDatabase.h>

// Local header files

#include "MsgableTypeDatabase.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

// Function declaration

ccs::base::AnyObject* MsgableTypeDatabase_Constructor (void) ;

// Global variables

bool MsgableTypeDatabase_IsRegistered =
  ccs::base::GlobalObjectFactory::Register("sup::core::MsgableTypeDatabase",
					   (ccs::base::AnyObject_Constructor_t) &MsgableTypeDatabase_Constructor);

// Function definition
  
ccs::base::AnyObject* MsgableTypeDatabase_Constructor (void) 
{

  sup::core::MsgableTypeDatabase* ref = new (std::nothrow) sup::core::MsgableTypeDatabase ();

  return dynamic_cast<ccs::base::AnyObject*>(ref);

}

bool MsgableTypeDatabase::SetParameter (const char* name, const char* value)
{

  bool status = (!ccs::HelperTools::IsUndefinedString(name) && !ccs::HelperTools::IsUndefinedString(value));

  if (status)
    {
      if ((std::string(name) == "verbose") && (std::string(value) == "true"))
	{
	  ccs::log::SetStdout();
	}
    }

  return status;

}

bool MsgableTypeDatabase::ProcessMessage (const char* msg)
{

  bool status = !ccs::HelperTools::IsUndefinedString(msg);

  if (status)
    {
      if (true == ccs::HelperTools::Contain(msg, "Register"))
	{
	  const char* p_buf = msg;
      
	  while ((*p_buf != 0) && (*p_buf != '{')) { p_buf += 1; }
	  status = ((*p_buf != 0) && (-1 != ccs::HelperTools::FindMatchingBrace(p_buf)));

	  if (status)
	    {
	      std::string type (p_buf); type.substr(0u, ccs::HelperTools::FindMatchingBrace(p_buf));
	      log_info("MsgableTypeDatabase::ProcessMessage - Register type '%s' ..", type.c_str());
	      status = ccs::types::GlobalTypeDatabase::Register(type.c_str());
	    }
	}
    }

  return status;

}

MsgableTypeDatabase::MsgableTypeDatabase (void) 
{

  ccs::base::AnyObject::SetInstanceType("sup::core::MsgableTypeDatabase");

  return;

}
  
MsgableTypeDatabase::~MsgableTypeDatabase (void) {}
  
} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC

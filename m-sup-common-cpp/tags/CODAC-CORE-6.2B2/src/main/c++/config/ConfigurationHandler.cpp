/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationHandler.cpp $
* $Id: ConfigurationHandler.cpp 100206 2019-06-19 17:49:17Z bauvirb $
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

#include <AnyObject.h>

#include <Hash.h>
#include <CyclicRedundancyCheck.h>

#include <log-api.h> // Logging helper functions

// Local header files

#include "ConfigurationHandler.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

// Global variables

// Function declaration

// Function definition
  
bool ConfigurationHandler::GetSeed (const std::string& name, ccs::types::uint32& seed) const
{

  log_info("ConfigurationHandler::GetSeed('%s') - Entering method", name.c_str());

  ccs::types::uint64 time = ccs::HelperTools::GetCurrentTime();
  
  seed = ccs::HelperTools::Hash<ccs::types::uint32>(name.c_str());
  seed = ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(&time), sizeof(ccs::types::uint64), seed);

  log_info("ConfigurationHandler::GetSeed('%s') - Returning '%u'", name.c_str(), seed);

  log_info("ConfigurationHandler::GetSeed('%s') - Leaving method", name.c_str());

  return true;

}

ConfigurationHandler::ConfigurationHandler (void) 
{

  ccs::base::AnyObject::SetInstanceType("sup::core::ConfigurationHandler");

  return;

}  

ConfigurationHandler::~ConfigurationHandler (void) {}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC
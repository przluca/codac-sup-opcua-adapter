/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationHasher.cpp $
* $Id: ConfigurationHasher.cpp 100195 2019-06-19 13:35:27Z bauvirb $
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

#include <BasicTypes.h> // Misc. type definition
#include <SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.

#include <AnyObject.h>

#include <CyclicRedundancyCheck.h>

#include <log-api.h> // Logging helper functions

// Local header files

#include "ConfigurationHasher.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

// Function declaration

// Global variables

// Function definition
  
bool ConfigurationHasher::ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum) const
{

  bool status = (NULL != value.GetInstance());
  
  if (status)
    {
      checksum = ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(value.GetInstance()), value.GetSize(), seed);
    }
  
  return status;

}

bool ConfigurationHasher::VerifyChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum) const
{

  bool status = (NULL != value.GetInstance());
  
  ccs::types::uint32 sum;

  if (status)
    {
      status = ComputeChecksum(name, value, seed, sum);
    }

  if (status)
    {
      status = (sum == checksum);
    }
  
  return status;

}

ConfigurationHasher::ConfigurationHasher (void)
{

  ccs::base::AnyObject::SetInstanceType("sup::core::ConfigurationHasher");

  return;

}  

ConfigurationHasher::~ConfigurationHasher (void) {}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyType.cpp $
* $Id: AnyType.cpp 100890 2019-07-19 07:14:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*                                 CS 90 046
*                                 13067 St. Paul-lez-Durance Cedex
*                                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. helper functions

#include "NetTools.h" 

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE 
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE 
#include "log-api.h" // Syslog wrapper routines

#include "AnyType.h" // Introspectable type definition (base class) ..
#include "AnyTypeHelper.h" // .. associated helper routines

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::types"

// Type definition

namespace ccs {

namespace types {

// Global variables

// Function declaration

// Function definition
  
uint32 AnyType::GetSize (void) const { return __size; }
AnyType* AnyType::SetSize (const uint32 size) { __size = size; return this; }

const char8 * AnyType::GetName (void) const { return __type; }
AnyType* AnyType::SetName (const char8 * const type) { ccs::HelperTools::SafeStringCopy(__type, type, STRING_MAX_LENGTH); return this; }

void* AnyType::CreateInstance (void) const
{ 

  ccs::types::uint8* ref = NULL_PTR_CAST(ccs::types::uint8*);

  bool status = (AnyType::GetSize() > 0u);

  if (status) 
    {
      log_debug("AnyType::CreateInstance - Create instance of size '%u'", AnyType::GetSize());
      ref = new (std::nothrow) ccs::types::uint8 [AnyType::GetSize()];
      status = (NULL_PTR_CAST(ccs::types::uint8*) != ref); 
    }

  if (status) 
    {
      (void)memset(ref, 0, AnyType::GetSize());
    }

  return static_cast<void*>(ref); 

}

void AnyType::DeleteInstance (const void * const ref) const
{ 

  bool status = (NULL_PTR_CAST(const void * const) != ref);

  if (status) 
    {
      delete [] static_cast<const ccs::types::uint8 * const>(ref); 
    }

  return; 

}

bool AnyType::IsType (const char8 * const type) const { return ccs::HelperTools::StringCompare(__type, type, STRING_MAX_LENGTH); }

AnyType::AnyType (void) 
{ 

  (void)AnyType::SetSize(0u); 
  (void)AnyType::SetName("undefined"); 

  return; 

}

AnyType::AnyType (const char8 * const type)
{ 

  (void)AnyType::SetSize(0u); 
  (void)AnyType::SetName(type); 

  return; 

}

AnyType::AnyType (const char8 * const type, const uint32 size)
{ 

  (void)AnyType::SetSize(size); 
  (void)AnyType::SetName(type); 

  return; 

}

AnyType::~AnyType (void) {}

} // namespace types

} // namespace ccs
/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessTypes.cpp $
* $Id: PVAccessTypes.cpp 98900 2019-04-08 06:24:58Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*		  CS 90 046
*		  13067 St. Paul-lez-Durance Cedex
*		  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <new> // std::nothrow

#include <BasicTypes.h> // Global type definition
#include <SysTools.h> // Misc. helper functions

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE 
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE 
#include <log-api.h> // Syslog wrapper routines

#include <AnyType.h> // Introspectable data type ..
#include <AnyTypeHelper.h> // .. associated helper routines
#include <AnyTypeDatabase.h>

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

#include "PVAccessTypes.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "pva-if"

// Type definition

namespace ccs {

namespace base {

namespace PVAccessTypes {

// Function declaration

// Global variables

std::shared_ptr<const ccs::types::CompoundType> RecordBase_int; // Introspectable type definition

// Function definition

bool Initialise (void)
{

  using namespace ccs::types;

  bool status = (RecordBase_int ? true : false);

  if (!status)
    {
      CompoundType* timestamp = (new (std::nothrow) CompoundType (PVAccess_Timestamp_TypeName))
	->AddAttribute("nanosec", ccs::types::UnsignedInteger64)
	->AddAttribute("iso8601", (new (std::nothrow) ArrayType ("CharArray_t", ccs::types::Character8, 32u)));

      CompoundType* severity = (new (std::nothrow) CompoundType (PVAccess_Severity_TypeName))
	->AddAttribute("level", ccs::types::UnsignedInteger8)
	->AddAttribute("level_hr", (new (std::nothrow) ArrayType ("CharArray_t", ccs::types::Character8, 12u)))
	->AddAttribute("reason", ccs::types::String);

      // Introspectable type definition
      RecordBase_int = std::shared_ptr<const CompoundType> ((new (std::nothrow) CompoundType (PVAccess_RecordBase_TypeName))
							    ->AddAttribute("counter", ccs::types::UnsignedInteger64)
							    ->AddAttribute("timestamp", timestamp)
							    ->AddAttribute("severity", severity));
      
      status = ((RecordBase_int ? true : false) && GlobalTypeDatabase::Register(RecordBase_int));
    }

  if (status)
    {
      status = GlobalTypeDatabase::IsValid(PVAccess_RecordBase_TypeName);
    }

  if (status)
    {
      status = (sizeof(RecordBase_t) == RecordBase_int->GetSize());
    }

  return status;

}

} // namespace PVAccessTypes

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC
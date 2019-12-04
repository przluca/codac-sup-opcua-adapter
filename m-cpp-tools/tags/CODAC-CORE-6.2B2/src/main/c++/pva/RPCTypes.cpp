/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCTypes.cpp $
* $Id: RPCTypes.cpp 99922 2019-05-23 13:05:51Z bauvirb $
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

#include "RPCTypes.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "rpc-if"

// Type definition

namespace ccs {

namespace base {

namespace RPCTypes {

// Function declaration

// Global variables

std::shared_ptr<const ccs::types::CompoundType> Reply_int; // Introspectable type definition
std::shared_ptr<const ccs::types::CompoundType> Request_int; // Introspectable type definition

// Function definition

bool Initialise (void)
{

  using namespace ccs::types;

  bool status = ((Reply_int ? true : false) &&
		 (Request_int ? true : false));

  if (!status)
    {
      // Introspectable type definition
      Reply_int = std::shared_ptr<const CompoundType> ((new (std::nothrow) CompoundType (RPCReply_TypeName))
						       ->AddAttribute<uint64>("timestamp")
						       ->AddAttribute<string>("qualifier")
						       ->AddAttribute<boolean>("status")
						       ->AddAttribute<string>("reason"));
      
      Request_int = std::shared_ptr<const CompoundType> ((new (std::nothrow) CompoundType (RPCRequest_TypeName))
							 ->AddAttribute<uint64>("timestamp")
							 ->AddAttribute<string>("qualifier"));

      status = (((Reply_int ? true : false) && GlobalTypeDatabase::Register(Reply_int)) &&
		((Request_int ? true : false) && GlobalTypeDatabase::Register(Request_int)));
    }

  if (status)
    {
      status = (GlobalTypeDatabase::IsValid(RPCReply_TypeName) &&
		GlobalTypeDatabase::IsValid(RPCRequest_TypeName));
    }

  if (status)
    {
      status = ((sizeof(Reply_t) == Reply_int->GetSize()) &&
		(sizeof(Request_t) == Request_int->GetSize()));
    }

  return status;

}

} // namespace RPCTypes

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

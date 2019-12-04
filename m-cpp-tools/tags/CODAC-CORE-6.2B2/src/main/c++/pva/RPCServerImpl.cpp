/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCServerImpl.cpp $
* $Id: RPCServerImpl.cpp 99922 2019-05-23 13:05:51Z bauvirb $
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
#include <functional> // std::function

#include <BasicTypes.h> // Global type definition
#include <SysTools.h> // Misc. helper functions

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE
#include <log-api.h> // Syslog wrapper routines

#include <AnyType.h> // Introspectable data type ..
#include <AnyTypeHelper.h> // .. associated helper routines

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

#include "RPCServerImpl.h" // This class definition
#include "RPCTypes.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "rpc-if"

// Type definition

namespace ccs {

namespace base {

// Global variables

// Function declaration

// Function definition

const char* RPCServerImpl::GetService (void) const { return __service; }
bool RPCServerImpl::SetService (const char* service) { ccs::HelperTools::SafeStringCopy(__service, service, STRING_MAX_LENGTH); return true; }

bool RPCServerImpl::RegisterHandler (std::function<ccs::types::AnyValue(const ccs::types::AnyValue&)> cb) { __cb = cb; return true; }
ccs::types::AnyValue RPCServerImpl::CallHandler (const ccs::types::AnyValue& request) const
{

  bool status = (NULL != __cb);
  ccs::types::AnyValue reply;

  if (status)
    {
      try
	{
	  log_debug("RPCServerImpl::CallHandler - Calling registered handler .. ");
	  reply = __cb(request);
	  log_debug("RPCServerImpl::CallHandler - .. returned");
	}
      catch (...)
	{
	  log_debug("RPCServerImpl::CallHandler - .. exception caught");
	  status = false;
	}
    }

  if (!status)
    {
      // Copy the base reply type ..
      ccs::types::CompoundType __reply_type (*ccs::base::RPCTypes::Reply_int); // Default RPC reply type
      // Instantiate RPC reply ..
      ccs::types::AnyValue __reply_value (__reply_type);
  
      ccs::HelperTools::SetAttributeValue<ccs::types::uint64>(&__reply_value, "timestamp", ccs::HelperTools::GetCurrentTime());
      ccs::HelperTools::SetAttributeValue<ccs::types::boolean>(&__reply_value, "status", status);
      ccs::HelperTools::SetAttributeValue(&__reply_value, "qualifier", "failure");
      ccs::HelperTools::SetAttributeValue(&__reply_value, "reason", "Unregistered handler or exception caught");

      // Provide error reply
      reply = __reply_value;
    }

  return reply;

}

bool RPCServerImpl::Terminate (void) { return true; }

RPCServerImpl::RPCServerImpl (void)
{ 
  // Initialise attributes
  __cb = NULL;
  ccs::HelperTools::SafeStringCopy(__service, STRING_UNDEFINED, STRING_MAX_LENGTH); 

  // Register types in GlobalTypeDatabase
  RPCTypes::Initialise(); 

  return; 

}

RPCServerImpl::~RPCServerImpl (void)
{ 

  Terminate();

  return;

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

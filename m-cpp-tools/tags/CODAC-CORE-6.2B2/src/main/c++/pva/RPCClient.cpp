/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCClient.cpp $
* $Id: RPCClient.cpp 99922 2019-05-23 13:05:51Z bauvirb $
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

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

#include "RPCClientImpl.h"
#include "RPCTypes.h" // .. associated types

#include "PVAccessRPCClient.h" // Only one transport supported for now

#include "RPCClient.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "rpc-if"

// Type definition

namespace ccs {

namespace base {

// Global variables

// Function declaration

// Function definition

bool RPCClient::Launch (void) { return __impl->Launch(); }

bool RPCClient::IsConnected (void) const { return __impl->IsConnected(); }

const char * RPCClient::GetService (void) const { return __impl->GetService(); }
bool RPCClient::SetService (const char* service) { return __impl->SetService(service); }

ccs::types::AnyValue RPCClient::SendRequest (const ccs::types::AnyValue& request) const { return __impl->SendRequest(request); }

RPCClient::RPCClient (void) { __impl = dynamic_cast<RPCClientImpl*>(new (std::nothrow) PVAccessRPCClient ()); return; }

RPCClient::RPCClient (const char* service)
{ 

  // Instantiate implementation class .. single transport for now
  __impl = dynamic_cast<RPCClientImpl*>(new (std::nothrow) PVAccessRPCClient ());

  bool status = (static_cast<RPCClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->SetService(service);
    }

  if (status)
    {
      status = __impl->Launch();
    }

  return; 

}

RPCClient::~RPCClient (void)
{ 

  if (static_cast<RPCClientImpl*>(NULL) != __impl)
    { 
      delete __impl; 
    }

  __impl= static_cast<RPCClientImpl*>(NULL);

  return;

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

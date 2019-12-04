/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCServer.cpp $
* $Id: RPCServer.cpp 99922 2019-05-23 13:05:51Z bauvirb $
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

#include "RPCServerImpl.h"
#include "RPCTypes.h"

#include "PVAccessRPCServer.h" // Only one transport supported for now

#include "RPCServer.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "rpc-if"

// Type definition

namespace ccs {

namespace base {

// Global variables

// Function declaration

// Function definition

const char* RPCServer::GetService (void) const
{

  const char* service = static_cast<const char*>(NULL);

  if (static_cast<RPCServerImpl*>(NULL) != __impl)
    {
      service = __impl->GetService();
    }

  return service;

}

RPCServer::RPCServer (const char* service)
{ 

  // Instantiate implementation class .. single transport for now
  __impl = dynamic_cast<RPCServerImpl*>(new (std::nothrow) PVAccessRPCServer ());

  bool status = (static_cast<RPCServerImpl*>(NULL) != __impl);

  if (status)
    {
      using namespace std::placeholders;
      status = __impl->RegisterHandler(std::bind(&RPCServer::HandleRequest, this, _1));
    }

  if (status)
    {
      status = __impl->SetService(service);
    }

  if (status)
    {
      status = __impl->Launch();
    }

  if (status)
    {
      log_info("RPCServer::RPCServer('%s') - Initialisation successful", service);
    }

  return; 

}

RPCServer::~RPCServer (void)
{ 

  if (static_cast<RPCServerImpl*>(NULL) != __impl)
    { 
      delete __impl; 
    }

  __impl= static_cast<RPCServerImpl*>(NULL);

  return;

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessRPCServer.cpp $
* $Id: PVAccessRPCServer.cpp 99922 2019-05-23 13:05:51Z bauvirb $
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

#include <memory> // std::shared_ptr
#include <new> // std::nothrow
#include <functional> // std::function

#include <pv/pvData.h>
#include <pv/rpcServer.h>
#include <pv/serverContext.h>

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

#include "AnyTypeToPVA.h" // .. associated helper routines
#include "PVAToAnyType.h" // .. associated helper routines

#include "AnyValueToPVA.h" // .. associated helper routines
#include "PVAToAnyValue.h" // .. associated helper routines

#include "PVAccessRPCServer.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "pva-if"

// Type definition

namespace ccs {

namespace base {

// Global variables

// Function declaration

// Function definition

bool PVAccessRPCServer::Initialise (void)
{ 

  __context = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, true); 
  __server = std::shared_ptr<epics::pvAccess::RPCServer>(new epics::pvAccess::RPCServer ()); 

  __server->registerService(std::string(this->GetService()), std::shared_ptr<epics::pvAccess::RPCService>(this)); 

  __initialised = true; 

  return true; 

}

bool PVAccessRPCServer::Launch (void)
{ 

  if (true != __initialised)
    {
      this->Initialise(); 
    }

  __server->runInNewThread(); 

  return true; 

}

std::shared_ptr<epics::pvData::PVStructure> PVAccessRPCServer::request (std::shared_ptr<epics::pvData::PVStructure> const & __request)
  throw (epics::pvAccess::RPCRequestException)
{

  bool status = true;

  std::shared_ptr<const ccs::types::CompoundType> request_type;

  if (status)
    {
      //request_type = ccs::HelperTools::PVStructToAnyType(__request->getStructure());
      request_type = ccs::HelperTools::PVStructToAnyType<std::shared_ptr<const epics::pvData::PVStructure>>(__request); // In order to support arrays of structure
      status = (request_type ? true : false);
    }

  ccs::types::AnyValue request (std::dynamic_pointer_cast<const ccs::types::AnyType>(request_type));

  if (status)
    {
      status = ccs::HelperTools::PVStructToAnyValue(&request, __request);
    }

  std::shared_ptr<epics::pvData::PVStructure> __reply;

  if (status)
    {
      log_debug("PVAccessRPCServer::request - Calling registered handler .. ");
      ccs::types::AnyValue reply = CallHandler(request);
      log_debug("PVAccessRPCServer::request - .. returned");

      try
	{
	  __reply = epics::pvData::getPVDataCreate()->createPVStructure(ccs::HelperTools::AnyTypeToPVStruct(reply.GetType()));
	  status = ccs::HelperTools::AnyValueToPVStruct(&reply, __reply);
	}
      catch (const std::exception& e)
	{
	  log_error("PVAccessRPCServer::request - .. exception caught");
	  status = false;
	}
     }

  return __reply;

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC
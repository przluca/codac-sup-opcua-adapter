/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessRPCClient.cpp $
* $Id: PVAccessRPCClient.cpp 99922 2019-05-23 13:05:51Z bauvirb $
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

#include <pv/pvData.h>
#include <pv/rpcClient.h>
//#include <pv/rpcServer.h>
#include <pv/clientFactory.h>

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
#include "RPCTypes.h"

#include "AnyTypeToPVA.h" // .. associated helper routines
#include "PVAToAnyType.h" // .. associated helper routines

#include "AnyValueToPVA.h" // .. associated helper routines
#include "PVAToAnyValue.h" // .. associated helper routines

#include "PVAccessRPCClient.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "pva-if"

// Type definition

namespace ccs {

namespace base {

// Global variables

// Function declaration

// Function definition

bool PVAccessRPCClient::Initialise (void)
{ 

  epics::pvAccess::ClientFactory::start(); 
  __client = epics::pvAccess::RPCClient::create(std::string(GetService())); 

  __initialised = true; 
  __connected = false; 

  return true; 

}

bool PVAccessRPCClient::Launch (void)
{ 

  if (true != __initialised) 
    {
      this->Initialise(); 
    }

  __client->issueConnect(); 

  return true; 

}

bool PVAccessRPCClient::IsConnected (void) const
{ 

  if (true != __connected)
    {
      *(const_cast<bool*>(&__connected)) = __client->waitConnect(0.001); 
    }

  return __connected; 

}

ccs::types::AnyValue PVAccessRPCClient::SendRequest (const ccs::types::AnyValue& request) const
{

  log_debug("PVAccessRPCClient::SendRequest - Create PVA RPC request with appropriate type");
  epics::pvData::PVStructure::shared_pointer __request = epics::pvData::getPVDataCreate()->createPVStructure(ccs::HelperTools::AnyTypeToPVStruct(request.GetType()));

  bool status = ccs::HelperTools::AnyValueToPVStruct(&request, __request);

  epics::pvData::PVStructure::shared_pointer __reply;

  if (status)
    {
      try
	{
	  log_debug("PVAccessRPCClient::SendRequest - Send PVA RPC request ..");
	  __reply = __client->request(__request, 10.0);
	}
      catch (const std::exception& e)
	{
	  log_error("PVAccessRPCClient::SendRequest - .. '%s' exception caught", e.what());
	  *(const_cast<bool*>(&__connected)) = false;
	  status = false;
	}
    }

  std::shared_ptr<const ccs::types::CompoundType> reply_type;

  if (status)
    {
      //reply_type = ccs::HelperTools::PVStructToAnyType(__reply->getStructure());
      reply_type = ccs::HelperTools::PVStructToAnyType<std::shared_ptr<const epics::pvData::PVStructure>>(__reply); // In order to support arrays of structure
      status = (reply_type ? true : false);
    }
  else
    {
      reply_type = ccs::base::RPCTypes::Reply_int; // Default RPC reply type
    }

  ccs::types::AnyValue reply (std::dynamic_pointer_cast<const ccs::types::AnyType>(reply_type));

  if (status)
    {
      status = ccs::HelperTools::PVStructToAnyValue(&reply, __reply);
    }
  else
    {
      ccs::base::RPCTypes::Reply_t reply_error;

      reply_error.timestamp = ccs::HelperTools::GetCurrentTime();
      reply_error.status = status;

      ccs::HelperTools::SafeStringCopy(reply_error.qualifier, "error", STRING_MAX_LENGTH);
      snprintf(reply_error.reason, STRING_MAX_LENGTH, "Error with '%s' RPC service", this->GetService());

      reply = reply_error;
    }

  char buffer [1024];

  if (status)
    {
      status = reply.SerialiseInstance(buffer, 1024u);
    }

  if (status)
    {
      log_debug("PVAccessRPCClient::SendRequest - .. reply '%s'", buffer);
    }

 return reply;

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

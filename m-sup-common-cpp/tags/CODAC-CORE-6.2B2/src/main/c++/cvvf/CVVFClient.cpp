/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/cvvf/CVVFClient.cpp $
* $Id: CVVFClient.cpp 100236 2019-06-21 11:00:02Z bauvirb $
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

#include <log-api.h> // Logging helper functions

#include <AnyTypeDatabase.h>

#include <RPCClient.h>
#include <RPCTypes.h>

// Local header files

#include "CVVFFunctionHandler.h" // Types at interface
#include "CVVFClient.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

class CVVFClientImpl : public ccs::base::RPCClient
{

  private:

  public:

    CVVFClientImpl (const char* service);
    virtual ~CVVFClientImpl (void);

};

// Global variables

// Function declaration

// Function definition
  
bool CVVFClient::IsConnected (void) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  return status;

}

bool CVVFClient::IsValid (const char* name) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute<ccs::types::string>("value"); // Extend with value field

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve") &&
		ccs::HelperTools::SetAttributeValue(&request, "value", name));
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  if (status)
	    {
	      status = reply_status;
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::IsValid('%s') - .. '%s' exception", name, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::IsValid('%s') - .. undefined exception", name);
	  status = false;
	}
#endif
    }

  return status;

}

bool CVVFClient::HasFunction (const char* name) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute<ccs::types::string>("value"); // Extend with value field

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve") &&
		ccs::HelperTools::SetAttributeValue(&request, "value", name));
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  if (status)
	    { // Known symbol
	      status = (reply_status && ccs::HelperTools::HasAttribute(&reply, "value") && 
			ccs::HelperTools::GetAttributeType(&reply, "value")->IsType(CVVF_FunctionDefinition_TypeName));
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::HasFunction('%s') - .. '%s' exception", name, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::HasFunction('%s') - .. undefined exception", name);
	  status = false;
	}
#endif
    }

  return status;

}

bool CVVFClient::HasType (const char* name) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute<ccs::types::string>("value"); // Extend with value field

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve") &&
		ccs::HelperTools::SetAttributeValue(&request, "value", name));
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  if (status)
	    { // Known symbol
	      status = (reply_status && ccs::HelperTools::HasAttribute(&reply, "value") && 
			!(ccs::HelperTools::GetAttributeType(&reply, "value")->IsType(CVVF_FunctionDefinition_TypeName)));
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::HasType('%s') - .. '%s' exception", name, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::HasType('%s') - .. undefined exception", name);
	  status = false;
	}
#endif
    }

  return status;

}

const CVVFFunctionDefinition CVVFClient::GetFunctionDefinition (const char* name) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute<ccs::types::string>("value"); // Extend with value field

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve") &&
		ccs::HelperTools::SetAttributeValue(&request, "value", name));
    }

  CVVFFunctionDefinition ret;

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  if (status)
	    { // Known symbol with pre-determined value field
	      status = (reply_status && ccs::HelperTools::HasAttribute(&reply, "value") && 
			ccs::HelperTools::GetAttributeType(&reply, "value")->IsType(CVVF_FunctionDefinition_TypeName));
	    }

	  if (status)
	    {
	      ccs::HelperTools::SafeStringCopy(ret.name, name, ccs::types::MaxStringLength);
	      ccs::types::string config; ccs::HelperTools::SafeStringCopy(config, static_cast<const char*>(ccs::HelperTools::GetAttributeReference(&reply, "value.config")), ccs::types::MaxStringLength);
	      ret.cfg = this->GetTypeDefinition(config);
	      ccs::types::string input; ccs::HelperTools::SafeStringCopy(input, static_cast<const char*>(ccs::HelperTools::GetAttributeReference(&reply, "value.input")), ccs::types::MaxStringLength);
	      ret.inp = this->GetTypeDefinition(input);
	      ccs::types::string output; ccs::HelperTools::SafeStringCopy(output, static_cast<const char*>(ccs::HelperTools::GetAttributeReference(&reply, "value.output")), ccs::types::MaxStringLength);
	      ret.out = this->GetTypeDefinition(output);
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::GetFunctionDefinition('%s') - .. '%s' exception", name, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::GetFunctionDefinition('%s') - .. undefined exception", name);
	  status = false;
	}
#endif
    }

  if (status)
    {
      log_debug("CVVFClient::GetFunctionDefinition('%s') - .. success", name);
    }

  return ret;

}
  
std::shared_ptr<const ccs::types::AnyType> CVVFClient::GetTypeDefinition (const char* name) const
{

  std::shared_ptr<const ccs::types::AnyType> ret;

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute<ccs::types::string>("value"); // Extend with value field

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "resolve") &&
		ccs::HelperTools::SetAttributeValue(&request, "value", name));
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  if (status)
	    { // Known symbol
	      status = (reply_status && ccs::HelperTools::HasAttribute(&reply, "value"));
	    }

	  if (status)
	    {
	      ret = ccs::HelperTools::GetAttributeType(&reply, "value");
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::GetTypeDefinition('%s') - .. '%s' exception", name, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::GetTypeDefinition('%s') - .. undefined exception", name);
	  status = false;
	}
#endif
    }

  if (status)
    {
      log_debug("CVVFClient::GetTypeDefinition('%s') - .. success", name);
    }

  return ret;

}

bool CVVFClient::Execute (const char* name, const ccs::types::AnyValue& cfg, const ccs::types::AnyValue& inp, ccs::types::AnyValue& out) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  if (status)
    {
      status = this->HasFunction(name);
    }

  ccs::types::CompoundType value_type (CVVF_FunctionInstance_TypeName);
  value_type.AddAttribute<ccs::types::string>("name");

  if (true == static_cast<bool>(cfg.GetType()))
    {
      value_type.AddAttribute("config", cfg.GetType());
    }

  if (true == static_cast<bool>(inp.GetType()))
    {
      value_type.AddAttribute("input", inp.GetType());
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute("value", value_type);

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "process") &&
		ccs::HelperTools::SetAttributeValue(&request, "value.name", name));

      if (status && static_cast<bool>(cfg.GetType()))
	{
	  status = ccs::HelperTools::SetAttributeValue(&request, "value.config", cfg);
	}

      if (status && static_cast<bool>(inp.GetType()))
	{
	  status = ccs::HelperTools::SetAttributeValue(&request, "value.input", inp);
	}
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  log_info("CVVFClient::Execute('%s') - Send request ..", name);

	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  char buffer [1024] = STRING_UNDEFINED; reply.SerialiseInstance(buffer, 1024u);
	  log_info("CVVFClient::Execute('%s') - .. reply '%s'", name, buffer);

	  if (status)
	    {
	      status = (reply_status && ccs::HelperTools::HasAttribute(&reply, "value"));
	    }

	  if (status)
	    {
	      out = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&reply, "value");
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::Execute('%s') - .. '%s' exception", name, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::Execute('%s') - .. undefined exception", name);
	  status = false;
	}
#endif
    }

  return status;

}

ccs::types::uint32 CVVFClient::Launch (const char* name, const ccs::types::AnyValue& cfg, const ccs::types::AnyValue& inp) const
{

  ccs::types::uint32 ret = 0u;

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  if (status)
    {
      status = this->HasFunction(name);
    }

  ccs::types::CompoundType value_type (CVVF_FunctionInstance_TypeName);
  value_type.AddAttribute<ccs::types::string>("name");

  if (true == static_cast<bool>(cfg.GetType()))
    {
      value_type.AddAttribute("config", cfg.GetType());
    }

  if (true == static_cast<bool>(inp.GetType()))
    {
      value_type.AddAttribute("input", inp.GetType());
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute("value", value_type);

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "launch") &&
		ccs::HelperTools::SetAttributeValue(&request, "value.name", name));

      if (status && static_cast<bool>(cfg.GetType()))
	{
	  status = ccs::HelperTools::SetAttributeValue(&request, "value.config", cfg);
	}

      if (status && static_cast<bool>(inp.GetType()))
	{
	  status = ccs::HelperTools::SetAttributeValue(&request, "value.input", inp);
	}
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  log_info("CVVFClient::Launch('%s') - Send request ..", name);

	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  char buffer [1024] = STRING_UNDEFINED; reply.SerialiseInstance(buffer, 1024u);
	  log_info("CVVFClient::Launch('%s') - .. reply '%s' ..", name, buffer);

	  if (status)
	    {
	      status = (reply_status && ccs::HelperTools::HasAttribute(&reply, "value"));
	    }

	  if (status)
	    {
	      status = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&reply, "value", ret);
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::Launch('%s') - .. '%s' exception", name, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::Launch('%s') - .. undefined exception", name);
	  status = false;
	}
#endif
    }

  if (status)
    {
      log_info("CVVFClient::Launch('%s') - .. success", name);
    }

  return ret;

}

bool CVVFClient::GetResult (const ccs::types::uint32 uid, ccs::types::AnyValue& out) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute<ccs::types::uint32>("value");

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "result") &&
		ccs::HelperTools::SetAttributeValue(&request, "value", uid));
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  log_info("CVVFClient::GetResult('%u') - Send request ..", uid);

	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  char buffer [1024] = STRING_UNDEFINED; reply.SerialiseInstance(buffer, 1024u);
	  log_info("CVVFClient::GetResult('%u') - .. reply '%s'", uid, buffer);

	  if (status)
	    {
	      status = (reply_status && ccs::HelperTools::HasAttribute(&reply, "value"));
	    }

	  if (status)
	    {
	      out = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&reply, "value");
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::GetResult('%u') - .. '%s' exception", uid, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::GetResult('%u') - .. undefined exception", uid);
	  status = false;
	}
#endif
    }

  return status;

}

bool CVVFClient::Abort (const ccs::types::uint32 uid) const
{

  bool status = (static_cast<CVVFClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->IsConnected();
    }

  ccs::types::CompoundType request_type (*ccs::base::RPCTypes::Request_int); // Copy base reply type
  request_type.AddAttribute<ccs::types::uint32>("value");

  ccs::types::AnyValue request (request_type);

  if (status)
    {
      // Compose request
      status = (ccs::HelperTools::SetAttributeValue(&request, "qualifier", "abort") &&
		ccs::HelperTools::SetAttributeValue(&request, "value", uid));
    }

  if (status)
    {
#ifdef RPC_CLIENT_EXCEPTIONS
      // Send request
      try 
	{
#endif
	  log_info("CVVFClient::Abort('%u') - Send request ..", uid);

	  ccs::types::AnyValue reply = __impl->SendRequest(request);
	  ccs::types::boolean reply_status = false;
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);

	  char buffer [1024] = STRING_UNDEFINED; reply.SerialiseInstance(buffer, 1024u);
	  log_info("CVVFClient::Abort('%u') - .. reply '%s'", uid, buffer);

	  if (status)
	    {
	      status = reply_status;
	    }
#ifdef RPC_CLIENT_EXCEPTIONS
	}
      catch (const std::exception& e)
	{
	  log_error("CVVFClient::Abort('%u') - .. '%s' exception", uid, e.what());
	  status = false;
	}
      catch (...)
	{
	  log_error("CVVFClient::Abort('%u') - .. undefined exception", uid);
	  status = false;
	}
#endif
    }

  return status;

}

CVVFClient::CVVFClient (const char* service)
{

  __impl = new (std::nothrow) CVVFClientImpl (service);

  return;

}
  
CVVFClientImpl::CVVFClientImpl (const char* service) : ccs::base::RPCClient(service)
{

  return;

}

CVVFClient::~CVVFClient (void)
{

  if (__builtin_expect((static_cast<CVVFClientImpl*>(NULL) != __impl), 1)) // Likely
    {
      delete __impl;
      __impl = static_cast<CVVFClientImpl*>(NULL);
    }

  return;

}
  
CVVFClientImpl::~CVVFClientImpl (void)
{

  return;

}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC

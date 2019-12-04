/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationService.cpp $
* $Id: ConfigurationService.cpp 101430 2019-08-07 13:43:30Z bauvirb $
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

#include <ObjectDatabase.h>
#include <ObjectFactory.h>

#include <log-api.h> // Logging helper functions

#include <RPCServer.h>
#include <RPCTypes.h>

// Local header files

#include "ConfigurationHasher.h"
#include "ConfigurationHandler.h"
#include "ConfigurationService.h"

// Constants

#define OVERRIDE_HASH_MISMATCH
//#undef OVERRIDE_HASH_MISMATCH

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

class ConfigurationServiceImpl : public ccs::base::RPCServer
{

  private:

    ConfigurationHasher* __hasher = static_cast<ConfigurationHasher*>(NULL);
    ConfigurationHandler* __handler = static_cast<ConfigurationHandler*>(NULL);
#ifdef OVERRIDE_HASH_MISMATCH
    bool ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum);
#endif
    bool VerifyChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum);

    bool GetSeed (const std::string& name, ccs::types::uint32& seed);
    bool ReadConfiguration (const std::string& name, ccs::types::AnyValue& value);
    bool LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum);

  public:

    ConfigurationServiceImpl (const char* service);
    virtual ~ConfigurationServiceImpl (void);

    bool RegisterHandler (ConfigurationHandler* handler);
    bool RegisterHasher (ConfigurationHasher* hasher);

    virtual ccs::types::AnyValue HandleRequest (const ccs::types::AnyValue& request); // Specialises virtual method of ccs::base::RPCServer

};

// Function declaration

ccs::base::AnyObject* ConfigurationService_Constructor (void);

// Global variables

bool ConfigurationService_IsRegistered = 
  ccs::base::GlobalObjectFactory::Register("sup::core::ConfigurationService",
					   (ccs::base::AnyObject_Constructor_t) &ConfigurationService_Constructor);

// Function definition
  
ccs::base::AnyObject* ConfigurationService_Constructor (void) 
{

  sup::core::ConfigurationService* ref = new (std::nothrow) sup::core::ConfigurationService ();

  return dynamic_cast<ccs::base::AnyObject*>(ref);

}

bool ConfigurationService::SetService (const char* service)
{

  bool status = (!ccs::HelperTools::IsUndefinedString(service) &&
		 (static_cast<ConfigurationServiceImpl*>(NULL) == __impl));

  if (status)
    {
      __impl = new (std::nothrow) ConfigurationServiceImpl (service);
      status = (static_cast<ConfigurationServiceImpl*>(NULL) != __impl);
    }

  return status;

}

bool ConfigurationService::RegisterHandler (ConfigurationHandler* handler)
{

  bool status = (static_cast<ConfigurationServiceImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->RegisterHandler(handler);
    }

  return status;

}

bool ConfigurationService::RegisterHandler (const char* name)
{

  bool status = ccs::base::GlobalObjectDatabase::IsValid(name);

  if (!status)
    {
      log_error("ConfigurationService::RegisterHandler('%s') - Invalid name", name);
    }

  ConfigurationHandler* ref = static_cast<ConfigurationHandler*>(NULL);
  
  if (status)
    {
      ref = dynamic_cast<ConfigurationHandler*>(ccs::base::GlobalObjectDatabase::GetInstance(name));
      status = (static_cast<ConfigurationHandler*>(NULL) != ref);
    }
  
  if (status)
    {
      log_info("ConfigurationService::RegisterHandler - Create association ..");
      status = RegisterHandler(ref);
    }
  else
    {
      log_error("ConfigurationService::RegisterHandler('%s') - Failure", name);
    }

  return status;

}

bool ConfigurationService::RegisterHasher (ConfigurationHasher* hasher)
{

  bool status = (static_cast<ConfigurationServiceImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->RegisterHasher(hasher);
    }

  return status;

}

bool ConfigurationService::RegisterHasher (const char* name)
{

  bool status = ccs::base::GlobalObjectDatabase::IsValid(name);

  ConfigurationHasher* ref = static_cast<ConfigurationHasher*>(NULL);
  
  if (status)
    {
      ref = dynamic_cast<ConfigurationHasher*>(ccs::base::GlobalObjectDatabase::GetInstance(name));
      status = (static_cast<ConfigurationHasher*>(NULL) != ref);
    }
  
  if (status)
    {
      log_info("ConfigurationService::RegisterHasher - Create association ..");
      status = RegisterHasher(ref);
    }

  return status;

}

bool ConfigurationService::SetParameter (const char* name, const char* value)
{

  bool status = (!ccs::HelperTools::IsUndefinedString(name) &&
		 !ccs::HelperTools::IsUndefinedString(value));

  if (status)
    {
      if (std::string(name) == "service")
	{
	  status = SetService(value);
	}

      if (std::string(name) == "handler")
	{
	  status = RegisterHandler(value);
	}

      if (std::string(name) == "hasher")
	{
	  status = RegisterHasher(value);
	}

      if ((std::string(name) == "verbose") && (std::string(value) == "true"))
	{
	  ccs::log::SetStdout();
	}
    }

  return status;

}

bool ConfigurationService::ProcessMessage (const char* msg)
{

  bool status = !ccs::HelperTools::IsUndefinedString(msg);

  return status;

}

bool ConfigurationServiceImpl::ReadConfiguration (const std::string& name, ccs::types::AnyValue& value)
{

  bool status = (static_cast<ConfigurationHandler*>(NULL) != __handler);

  if (status)
    {
      status = __handler->ReadConfiguration(name, value);
    }

  return status;

}

bool ConfigurationServiceImpl::LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum)
{

  bool status = (static_cast<ConfigurationHandler*>(NULL) != __handler);

  if (status)
    {
      status = __handler->LoadConfiguration(name, value, seed, checksum);
    }

  return status;

}

bool ConfigurationServiceImpl::GetSeed (const std::string& name, ccs::types::uint32& seed)
{

  bool status = (static_cast<ConfigurationHandler*>(NULL) != __handler);

  if (status)
    {
      status = __handler->GetSeed(name, seed);
    }

  return status;

}

bool ConfigurationServiceImpl::VerifyChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum)
{

  bool status = (static_cast<ConfigurationHasher*>(NULL) != __hasher);

  if (!status)
    { // Instantiate default implementation
      __hasher = new (std::nothrow) ConfigurationHasher ();
      status = (static_cast<ConfigurationHasher*>(NULL) != __hasher);
    }

  if (status)
    {
      status = __hasher->VerifyChecksum(name, value, seed, checksum);
    }

  return status;

}
#ifdef OVERRIDE_HASH_MISMATCH
bool ConfigurationServiceImpl::ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum)
{

  bool status = (static_cast<ConfigurationHasher*>(NULL) != __hasher);

  if (!status)
    { // Instantiate default implementation
      __hasher = new (std::nothrow) ConfigurationHasher ();
      status = (static_cast<ConfigurationHasher*>(NULL) != __hasher);
    }

  if (status)
    {
      status = __hasher->ComputeChecksum(name, value, seed, checksum);
    }

  return status;

}
#endif
bool ConfigurationServiceImpl::RegisterHandler (ConfigurationHandler* handler)
{

  bool status = (static_cast<ConfigurationHandler*>(NULL) != handler);

  if (status)
    {
      __handler = handler;
    }

  return status;

}

bool ConfigurationServiceImpl::RegisterHasher (ConfigurationHasher* hasher)
{

  bool status = (static_cast<ConfigurationHasher*>(NULL) != hasher);

  if (status)
    {
      __hasher = hasher;
    }

  return status;

}

// cppcheck-suppress unusedFunction // Callback associated to ccs::base::RPCService
ccs::types::AnyValue ConfigurationServiceImpl::HandleRequest (const ccs::types::AnyValue& request)
{

  const ccs::types::AnyValue* __query_value = &request;

  // WARNING - eget command line tool embeds the attributes in a 'query' structure
  if (ccs::HelperTools::HasAttribute(&request, "query"))
    {
      __query_value = new (std::nothrow) ccs::types::AnyValue (ccs::HelperTools::GetAttributeType(&request, "query"), 
							       ccs::HelperTools::GetAttributeReference(&request, "query"));
    }

  std::string qualifier;

  bool status = (ccs::HelperTools::HasAttribute(__query_value, "qualifier") &&
		 (ccs::types::String == ccs::HelperTools::GetAttributeType(__query_value, "qualifier")));

  if (status)
    {
      qualifier = std::string(static_cast<char*>(ccs::HelperTools::GetAttributeReference(__query_value, "qualifier")));
    }
  else
    {
      qualifier = std::string("");
    }

  std::string alias; // Named data set

  if (status)
    {
      alias = std::string(((ccs::HelperTools::HasAttribute(__query_value, "alias")) ? static_cast<char*>(ccs::HelperTools::GetAttributeReference(__query_value, "alias")) : ""));
    }

  ccs::types::AnyValue __reply_value; // Placeholder for return structure

  ccs::types::string reason; ccs::HelperTools::SafeStringCopy(reason, "Success", ccs::types::MaxStringLength);

  if (qualifier == "read")
    {
      log_info("ConfigurationService::HandleRequest('%s') - Query from handler ..", qualifier.c_str());

      ccs::types::AnyValue config; // Placeholder

      if (status)
	{
	  try
	    {
	      status = ReadConfiguration(alias, config);
	    }
	  catch (const std::exception& e)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. '%s' exception caught", qualifier.c_str(), e.what());
	      ccs::HelperTools::SafeStringCopy(reason, e.what(), ccs::types::MaxStringLength);
	      status = false;
	    }
	  catch (...)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. unknown exception caught", qualifier.c_str());
	      ccs::HelperTools::SafeStringCopy(reason, "Unknown exception", ccs::types::MaxStringLength);
	      status = false;
	    }
	}

      if (status)
	{
	  char buffer [1024] = STRING_UNDEFINED; config.SerialiseInstance(buffer, 1024u);
	  log_info(".. '%s' ..", buffer);
	}

      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*ccs::base::RPCTypes::Reply_int); // Default RPC reply type
      // .. and add the missing bit

      reply_type.AddAttribute("value", config.GetType());

      ccs::types::AnyValue reply_value (reply_type);
      ccs::HelperTools::SetAttributeValue(&reply_value, "timestamp", ccs::HelperTools::GetCurrentTime());
      ccs::HelperTools::SetAttributeValue(&reply_value, "qualifier", "read");
      ccs::HelperTools::SetAttributeValue(&reply_value, "status", status);
      ccs::HelperTools::SetAttributeValue(&reply_value, "reason", reason);
      ccs::HelperTools::SetAttributeValue(&reply_value, "value", config);

      __reply_value = reply_value;
    }
  else if ((qualifier == "init") || (qualifier == "seed"))
    {
      log_info("ConfigurationService::HandleRequest('%s') - Query from handler ..", qualifier.c_str());

      ccs::types::uint32 seed = 0u; // Placeholder

      if (status)
	{
	  try
	    {
	      status = GetSeed(alias, seed);
	    }
	  catch (const std::exception& e)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. '%s' exception caught", qualifier.c_str(), e.what());
	      ccs::HelperTools::SafeStringCopy(reason, e.what(), ccs::types::MaxStringLength);
	      status = false;
	    }
	  catch (...)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. unknown exception caught", qualifier.c_str());
	      ccs::HelperTools::SafeStringCopy(reason, "Unknown exception", ccs::types::MaxStringLength);
	      status = false;
	    }
	}

      if (status)
	{
	  log_info(".. '%u' ..", seed);
	}

      // Copy the base reply type ..
      ccs::types::CompoundType reply_type (*ccs::base::RPCTypes::Reply_int); // Default RPC reply type
      // .. and add the missing bit
      reply_type.AddAttribute("value", "uint32");

      ccs::types::AnyValue reply_value (reply_type);
      ccs::HelperTools::SetAttributeValue(&reply_value, "timestamp", ccs::HelperTools::GetCurrentTime());
      ccs::HelperTools::SetAttributeValue(&reply_value, "qualifier", "read");
      ccs::HelperTools::SetAttributeValue(&reply_value, "status", status);
      ccs::HelperTools::SetAttributeValue(&reply_value, "reason", reason);
      ccs::HelperTools::SetAttributeValue(&reply_value, "value", seed);

      __reply_value = reply_value;
    }
  else if (qualifier == "load")
    {
      log_info("ConfigurationService::HandleRequest('%s') - Process request ..", qualifier.c_str());

      // Seed .. 
      ccs::types::uint32 seed;

      if (status)
	{
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(__query_value, "seed", seed);
	}

      // Configuration data ..
      ccs::types::AnyValue config;

      if (status)
	{
	  // Extract configuration data ..
	  config = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(__query_value, "value");
	}

      if (status)
	{
	  char buffer [1024] = STRING_UNDEFINED; config.SerialiseInstance(buffer, 1024u);
	  log_info(".. '%s' ..", buffer);
	}

      // Checksum ..
      ccs::types::uint32 hash;

      if (status)
	{
	  status = ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(__query_value, "hash", hash);
	}

      log_info("ConfigurationService::HandleRequest('%s') - .. fit to expected type ..", qualifier.c_str());

      ccs::types::AnyValue copy; // Placeholder

      if (status)
	{
	  try
	    {
	      status = ReadConfiguration(alias, copy);
	    }
	  catch (const std::exception& e)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. '%s' exception caught", qualifier.c_str(), e.what());
	      status = false;
	    }
	  catch (...)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. unknown exception caught", qualifier.c_str());
	      status = false;
	    }
	}

      if (status)
	{
	  char buffer [1024] = STRING_UNDEFINED; copy.SerialiseInstance(buffer, 1024u);
	  log_info(".. '%s' ..", buffer);
	}

      log_info("ConfigurationService::HandleRequest('%s') - .. loose copy ..", qualifier.c_str());

      if (status)
	{
	  status = ccs::HelperTools::CopyOver(&copy, &config);
	}

      if (status)
	{
	  log_info("ConfigurationService::HandleRequest('%s') - .. verify hash ..", qualifier.c_str());

	  status = VerifyChecksum(alias, copy, seed, hash);
#ifdef OVERRIDE_HASH_MISMATCH
	  if (!status)
	    {
	      log_warning("ConfigurationService::HandleRequest('%s') - .. mismatch ..", qualifier.c_str());
	      status = ComputeChecksum(alias, copy, seed, hash);
	      log_warning("ConfigurationService::HandleRequest('%s') - .. override with '%u' ..", qualifier.c_str(), hash);
	    }
#endif
	}

      if (status)
	{
	  try
	    {
	      log_info("ConfigurationService::HandleRequest('%s') - .. and provide to handler ..", qualifier.c_str());
	      status = LoadConfiguration(alias, copy, seed, hash);

	      if (!status)
		{
		  ccs::HelperTools::SafeStringCopy(reason, "ConfigurationHandler::LoadConfiguration", ccs::types::MaxStringLength);
		}
	    }
	  catch (const std::exception& e)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. '%s' exception caught", qualifier.c_str(), e.what());
	      ccs::HelperTools::SafeStringCopy(reason, e.what(), ccs::types::MaxStringLength);
	      status = false;
	    }
	  catch (...)
	    {
	      log_notice("ConfigurationService::HandleRequest('%s') - .. unknown exception caught", qualifier.c_str());
	      ccs::HelperTools::SafeStringCopy(reason, "Unknown exception", ccs::types::MaxStringLength);
	      status = false;
	    }
	}

      ccs::types::AnyValue reply_value (ccs::base::RPCTypes::Reply_int); // Default RPC reply type
      ccs::HelperTools::SetAttributeValue(&reply_value, "timestamp", ccs::HelperTools::GetCurrentTime());
      ccs::HelperTools::SetAttributeValue(&reply_value, "qualifier", "load");
      ccs::HelperTools::SetAttributeValue(&reply_value, "status", status);
      ccs::HelperTools::SetAttributeValue(&reply_value, "reason", reason);

      __reply_value = reply_value;
    }
  else
    {
      // Default reply
      ccs::base::RPCTypes::Reply_t reply;

      reply.timestamp = ccs::HelperTools::GetCurrentTime();
      reply.status = false;
      ccs::HelperTools::SafeStringCopy(reply.qualifier, "error", ccs::types::MaxStringLength);
      snprintf(reply.reason, STRING_MAX_LENGTH, "Unknown qualifier '%s'", qualifier.c_str());
      
      ccs::types::AnyValue reply_value (ccs::base::RPCTypes::Reply_int); // Default RPC reply type
      reply_value = reply;

      __reply_value = reply_value;
    }

  if (status)
    {
      log_info("ConfigurationService::HandleRequest('%s') - .. success", qualifier.c_str());
    }
  else
    {
      log_error("ConfigurationService::HandleRequest('%s') - .. failure", qualifier.c_str());
    }
  
  if (__query_value != &request)
    {
      delete __query_value;
    }
      
  return __reply_value;

}

// Constructor methods

ConfigurationService::ConfigurationService (void)
{

  __impl = static_cast<ConfigurationServiceImpl*>(NULL);

  ccs::base::AnyObject::SetInstanceType("sup::core::ConfigurationService");

  return;

}
  
ConfigurationServiceImpl::ConfigurationServiceImpl (const char* service) : ccs::base::RPCServer(service) {}

// Destructor methods

ConfigurationService::~ConfigurationService (void)
{

  if (__builtin_expect((static_cast<ConfigurationServiceImpl*>(NULL) != __impl), 1)) // Likely
    {
      delete __impl;
      __impl = static_cast<ConfigurationServiceImpl*>(NULL);
    }

  return;

}
  
ConfigurationServiceImpl::~ConfigurationServiceImpl (void) {}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationLoader.cpp $
* $Id: ConfigurationLoader.cpp 100206 2019-06-19 17:49:17Z bauvirb $
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

#include <Hash.h>
#include <CyclicRedundancyCheck.h>

#include <ObjectDatabase.h>

#include <log-api.h> // Logging helper functions

#include <RPCClient.h>
#include <RPCTypes.h>

// Local header files

#include "ConfigurationHasher.h"
#include "ConfigurationLoader.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

class ConfigurationLoaderImpl : public ccs::base::RPCClient
{

  private:

    ConfigurationHasher* __hasher = static_cast<ConfigurationHasher*>(NULL);

    bool ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum);

  public:

    ConfigurationLoaderImpl (const char* service);
    virtual ~ConfigurationLoaderImpl (void);

    bool RegisterHasher (ConfigurationHasher* hasher);
  //bool RegisterHasher (const char* name); // Get instance from GlobalObjectDatabase and ..

    bool ReadConfiguration (const std::string& name, ccs::types::AnyValue& value);
    bool LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value);

};

// Global variables

// Function declaration

// Function definition
  
bool ConfigurationLoader::RegisterHasher (ConfigurationHasher* hasher)
{

  bool status = (static_cast<ConfigurationLoaderImpl*>(NULL) != __impl);

  if (__builtin_expect(status, 1)) // Likely
    {
      status = __impl->RegisterHasher(hasher);
    }

  return status;

}

bool ConfigurationLoaderImpl::ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum)
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

bool ConfigurationLoaderImpl::ReadConfiguration (const std::string& name, ccs::types::AnyValue& value)
{

  bool status = ccs::base::RPCClient::IsConnected();

  // Copy the base request type ..
  ccs::types::CompoundType request_t (*ccs::base::RPCTypes::Request_int); // Default RPC request type

  if (status && !name.empty())
    {
      request_t.AddAttribute("alias","string");
    }

  ccs::types::AnyValue request (request_t);

  if (status)
    {
      // Snapshot request ..
      status = ccs::HelperTools::SetAttributeValue(&request, "qualifier", "read");
    }

  if (status && !name.empty())
    {
      // .. named data set
      status = ccs::HelperTools::SetAttributeValue(&request, "alias", name.c_str());
    }

  if (status)
    {
      // Send RPC request ..
      ccs::types::AnyValue reply = ccs::base::RPCClient::SendRequest(request);
      ccs::types::boolean reply_status = false;
      status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
  
      if (reply_status)
	{
	  value = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&reply, "value");
	}

      status = reply_status;
    }

  return status;

}

bool ConfigurationLoaderImpl::LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value)
{

  bool status = ccs::base::RPCClient::IsConnected();

  // Copy the base request type ..
  ccs::types::CompoundType request_t (*ccs::base::RPCTypes::Request_int); // Default RPC request type

  if (status && !name.empty())
    {
      request_t.AddAttribute("alias","string");
    }

  ccs::types::uint32 seed;

  if (status)
    {
      ccs::types::AnyValue request (request_t);

      // Seed request ..
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "init");
  
      if (!name.empty())
	{
	  // .. named data set
	  ccs::HelperTools::SetAttributeValue(&request, "alias", name.c_str());
	}
  
      // Send RPC request ..
      ccs::types::AnyValue reply = ccs::base::RPCClient::SendRequest(request);
      ccs::types::boolean reply_status = false;
      status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status", reply_status);
  
      if (reply_status)
	{
	  status = ((ccs::types::UnsignedInteger32 == ccs::HelperTools::GetAttributeType(&reply, "value")) &&
		    ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&reply, "value", seed));
	} 
      else
	status = reply_status;
    }

  if (status)
    {
      // .. add the missing bit
      request_t.AddAttribute<ccs::types::uint32>("seed");
      request_t.AddAttribute("value", value.GetType());
      request_t.AddAttribute<ccs::types::uint32>("hash");
    }

  ccs::types::uint32 hash;

  if (status)
    {
      status = ComputeChecksum(name, value, seed, hash);
    }

  if (status)
    {
      ccs::types::AnyValue request (request_t);

      // Load request ..
      ccs::HelperTools::SetAttributeValue(&request, "qualifier", "load");
      ccs::HelperTools::SetAttributeValue(&request, "seed", seed);
      ccs::HelperTools::SetAttributeValue(&request, "value", value);
      ccs::HelperTools::SetAttributeValue(&request, "hash", hash);

      if (!name.empty())
	{
	  // .. named data set
	  ccs::HelperTools::SetAttributeValue(&request, "alias", name.c_str());
	}
  
      // Send RPC request ..
      ccs::types::AnyValue reply = ccs::base::RPCClient::SendRequest(request);
      status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status");
    }

  return status;

}

bool ConfigurationLoader::IsConnected (void) const
{

  bool status = (static_cast<ConfigurationLoaderImpl*>(NULL) != __impl);

  if (__builtin_expect(status, 1)) // Likely
    {
      status = __impl->IsConnected();
    }

  return status;

}

bool ConfigurationLoader::ReadConfiguration (const std::string& name, ccs::types::AnyValue& value) const
{

  bool status = (static_cast<ConfigurationLoaderImpl*>(NULL) != __impl);

  if (__builtin_expect(status, 1)) // Likely
    {
      status = __impl->ReadConfiguration(name, value);
    }

  if (status)
    {
      char buffer [1024] = STRING_UNDEFINED; value.SerialiseInstance(buffer, 1024u);
      log_info("ConfigurationLoader::ReadConfiguration('%s') - Success ..", name.c_str());
      log_info(".. '%s'", buffer);
    }

  return status;

}

bool ConfigurationLoader::LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value) const
{

  bool status = (static_cast<ConfigurationLoaderImpl*>(NULL) != __impl);

  if (__builtin_expect(status, 1)) // Likely
    {
      status = __impl->LoadConfiguration(name, value);
    }

  if (status)
    {
      char buffer [1024] = STRING_UNDEFINED; value.SerialiseInstance(buffer, 1024u);
      log_info("ConfigurationLoader::LoadConfiguration('%s') - Success ..", name.c_str());
      log_info(".. '%s'", buffer);
    }

  return status;

}

bool ConfigurationLoaderImpl::RegisterHasher (ConfigurationHasher* hasher)
{

  bool status = (static_cast<ConfigurationHasher*>(NULL) != hasher);

  if (status)
    {
      __hasher = hasher;
    }

  return status;

}
#if 0
bool ConfigurationLoaderImpl::RegisterHasher (const char* name)
{

  //bool status = ccs::base::GlobalObjectDatabase::IsValid(name);
  bool status = (static_cast<ccs::base::AnyObject*>(NULL) != ccs::base::GlobalObjectDatabase::GetInstance(name));

  ConfigurationHasher* ref = static_cast<ConfigurationHasher*>(NULL);
  
  if (status)
    {
      ref = dynamic_cast<ConfigurationHasher*>(ccs::base::GlobalObjectDatabase::GetInstance(name));
      status = (static_cast<ConfigurationHasher*>(NULL) != ref);
    }
  
  if (status)
    {
      log_info("ConfigurationLoaderImpl::RegisterHasher - Create association ..");
      status = RegisterHasher(ref);
    }

  return status;

}
#endif
ConfigurationLoader::ConfigurationLoader (const std::string& service)
{

  __impl = new (std::nothrow) ConfigurationLoaderImpl (service.c_str());

  return;

}
  
ConfigurationLoaderImpl::ConfigurationLoaderImpl (const char* service) : ccs::base::RPCClient(service)
{

  return;

}

ConfigurationLoader::~ConfigurationLoader (void)
{

  if (__builtin_expect((static_cast<ConfigurationLoaderImpl*>(NULL) != __impl), 1)) // Likely
    {
      delete __impl;
      __impl = static_cast<ConfigurationLoaderImpl*>(NULL);
    }

  return;

}
  
ConfigurationLoaderImpl::~ConfigurationLoaderImpl (void)
{

  return;

}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC

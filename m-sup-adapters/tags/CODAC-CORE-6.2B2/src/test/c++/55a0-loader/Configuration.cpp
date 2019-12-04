/******************************************************************************
* $HeadURL: https://svnpub.codac.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/trunk/src/test/c++/cvvf-service/UserSuppliedCode.cpp $
* $Id: UserSuppliedCode.cpp 99275 2019-04-25 06:34:12Z bauvirb $
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

#include <BasicTypes.h> // Misc. type definition

#include <SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.
#include <log-api.h> // Logging helper routines

#include <AnyObject.h>
#include <ObjectDatabase.h>
#include <ObjectFactory.h>

#include <RPCClient.h>

#include <sdn-topic.h>
#include <Publisher.h>
#include <AsynchronousSubscriber.h>

#include <Core/BareMetal/L0Types/CRC.h>

#include <ConfigurationHandler.h>
#include <ConfigurationHasher.h>

// Local header files

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "55a0"

// Type definition

namespace _55a0 {

class Publisher : public ccs::base::AnyObject, public ccs::base::CfgableObject
{

  private:

    ccs::types::AnyValue* __payload = static_cast<ccs::types::AnyValue*>(NULL);
    sdn::Publisher* __pub = static_cast<sdn::Publisher*>(NULL);

  public:

    Publisher (void);
    virtual ~Publisher (void);

    // ccs::base::CfgablObject implementation
    virtual bool SetParameter (const char* name, const char* value); // SDN configuration, i.e. interface name, topic name, datatype, etc.

    void* GetPayload (void);
    ccs::types::uint32 GetSize (void);
    bool SetHash (ccs::types::uint32 value);
    bool SetOper (ccs::types::uint32 value);

    bool SetConfig (const ccs::types::AnyValue& value);

    bool Publish (void);

};

class Subscriber : public ccs::base::AnyObject, public ccs::base::CfgableObject
{

  private:

    ccs::types::AnyValue* __payload = static_cast<ccs::types::AnyValue*>(NULL);
    sdn::AsynchronousSubscriber* __sub = static_cast<sdn::AsynchronousSubscriber*>(NULL);

  public:

    Subscriber (void);
    virtual ~Subscriber (void);

    // ccs::base::CfgablObject implementation
    virtual bool SetParameter (const char* name, const char* value); // SDN configuration, i.e. interface name, topic name, datatype, etc.

    void* GetPayload (void);
    ccs::types::uint32 GetHash (void);
    ccs::types::uint32 GetSeed (void);
    ccs::types::uint32 GetSize (void);

    ccs::types::AnyValue GetConfig (void);

};

class ConfigurationHandler : public sup::core::ConfigurationHandler, public ccs::base::AnyObject, public ccs::base::CfgableObject 
{

  private:

    // Interfaces to FPGA ..
    // .. data types
    // .. addresses
    ccs::types::string __target;
    Publisher* __pub = static_cast<Publisher*>(NULL);
    Subscriber* __sub = static_cast<Subscriber*>(NULL);

    // Interfaces to CIS-PPM
    ccs::base::RPCClient* __client = static_cast<ccs::base::RPCClient*>(NULL);

  public:

    ConfigurationHandler (void);
    virtual ~ConfigurationHandler (void);

    // sup::core::ConfigurationHandler implementation
    virtual bool ReadConfiguration (const std::string& name, ccs::types::AnyValue& value) const;
    virtual bool LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum);
    virtual bool GetSeed (const std::string& name, ccs::types::uint32& seed) const;

    // ccs::base::CfgablObject implementation
    virtual bool SetParameter (const char* name, const char* value); // FPGA addressing

};

class ConfigurationHasher : public sup::core::ConfigurationHasher, public ccs::base::AnyObject, public ccs::base::CfgableObject 
{

  private:

    // Keys 
    ccs::types::string __target;
    ccs::types::uint32 __key = 0u;

    MARTe::CRC<MARTe::uint32> __crc;

  public:

    ConfigurationHasher (void);
    virtual ~ConfigurationHasher (void);
#if 1
    // sup::core::ConfigurationHasher implementation
    virtual bool ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum) const;
    virtual bool VerifyChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum) const;
#endif
    // ccs::base::CfgablObject implementation
    virtual bool SetParameter (const char* name, const char* value); // Private keys

};

// Function declaration

ccs::base::AnyObject* ConfigurationPublisher_Constructor (void);
ccs::base::AnyObject* ConfigurationSubscriber_Constructor (void);
ccs::base::AnyObject* ConfigurationHandler_Constructor (void);
ccs::base::AnyObject* ConfigurationHasher_Constructor (void);

// Global variables

bool ConfigurationPublisher_IsRegistered =
  	(ccs::base::GlobalObjectFactory::Register("55a0::Publisher",
	(ccs::base::AnyObject_Constructor_t) &ConfigurationPublisher_Constructor) == STATUS_SUCCESS);

bool ConfigurationSubscriber_IsRegistered =
  	(ccs::base::GlobalObjectFactory::Register("55a0::Subscriber",
	(ccs::base::AnyObject_Constructor_t) &ConfigurationSubscriber_Constructor) == STATUS_SUCCESS);

bool ConfigurationHandler_IsRegistered =
  	(ccs::base::GlobalObjectFactory::Register("55a0::ConfigurationHandler",
	(ccs::base::AnyObject_Constructor_t) &ConfigurationHandler_Constructor) == STATUS_SUCCESS);

bool ConfigurationHasher_IsRegistered =
  	(ccs::base::GlobalObjectFactory::Register("55a0::ConfigurationHasher",
	(ccs::base::AnyObject_Constructor_t) &ConfigurationHasher_Constructor) == STATUS_SUCCESS);

// Function definition

ccs::base::AnyObject* ConfigurationPublisher_Constructor (void) 
{

  Publisher* ref = new (std::nothrow) Publisher ();

  return dynamic_cast<ccs::base::AnyObject*>(ref);

}

ccs::base::AnyObject* ConfigurationSubscriber_Constructor (void) 
{

  Subscriber* ref = new (std::nothrow) Subscriber ();

  return dynamic_cast<ccs::base::AnyObject*>(ref);

}

ccs::base::AnyObject* ConfigurationHandler_Constructor (void) 
{

  sup::core::ConfigurationHandler* ref = new (std::nothrow) ConfigurationHandler ();

  return dynamic_cast<ccs::base::AnyObject*>(ref);

}

ccs::base::AnyObject* ConfigurationHasher_Constructor (void) 
{

  sup::core::ConfigurationHasher* ref = new (std::nothrow) ConfigurationHasher ();

  return dynamic_cast<ccs::base::AnyObject*>(ref);

}

void* Publisher::GetPayload (void)
{

  bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);

  void* ref = NULL;

  if (status)
    {
      ref = __payload->GetInstance();
    }

  return ref;

}

ccs::types::uint32 Publisher::GetSize (void)
{

  bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);

  ccs::types::uint32 size = 0u;

  if (status)
    {
      size = __payload->GetSize();
    }

  return size;

}

bool Publisher::SetHash (ccs::types::uint32 value)
{

  bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);

  if (status)
    {
      status = ccs::HelperTools::SetAttributeValue(__payload, "hash", value);
    }

  return status;

}

bool Publisher::SetOper (ccs::types::uint32 value)
{

  bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);

  if (status)
    {
      status = ccs::HelperTools::HasAttribute(__payload, "oper");
    }

  if (status)
    {
      status = ccs::HelperTools::SetAttributeValue(__payload, "oper", value);
    }

  return status;

}

bool Publisher::SetConfig (const ccs::types::AnyValue& value)
{

  bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);

  if (status)
    {
      status = ccs::HelperTools::SetAttributeValue(__payload, "config", value);
    }

  return status;

}

bool Publisher::Publish (void)
{

  bool status = (static_cast<sdn::Publisher*> (NULL) != __pub);

  if (status)
    {
      char buffer [1024] = STRING_UNDEFINED; __payload->SerialiseInstance(buffer, 1024u);
      log_info("Publisher::Publish - '%s' ..", buffer);
    }

  if (status)
    {
      status = (STATUS_SUCCESS == __pub->CopyTopicInstance(GetPayload(), GetSize()));
    }

  if (status)
    {
      status = (STATUS_SUCCESS == __pub->Publish());
    }

  if (status)
    {
      log_info("Publisher::Publish - .. success");
    }
  else
    {
      log_error("Publisher::Publish - .. failure");
    }

  return status;

}

void* Subscriber::GetPayload (void)
{

  bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);

  void* ref = NULL;

  if (status)
    {
      ref = __payload->GetInstance();
    }

  return ref;

}

ccs::types::uint32 Subscriber::GetSize (void)
{

  bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);

  ccs::types::uint32 size = 0u;

  if (status)
    {
      size = __payload->GetSize();
    }

  return size;

}

ccs::types::uint32 Subscriber::GetHash (void)
{

  bool status = ((static_cast<sdn::AsynchronousSubscriber*>(NULL) != __sub) &&
		 (static_cast<ccs::types::AnyValue*>(NULL) != __payload));

  ccs::types::uint32 value = 0u;

  if (status)
    {
      status = (STATUS_SUCCESS == __sub->CopyTopicInstance(GetPayload(), GetSize()));
    }

  if (status)
    {
      status = ccs::HelperTools::GetAttributeValue(__payload, "hash", value);
    }

  if (status)
    {
      log_info("Subscriber::GetHash - .. success");
    }
  else
    {
      log_error("Subscriber::GetHash - .. failure");
    }

  return value;

}

ccs::types::uint32 Subscriber::GetSeed (void)
{

  bool status = ((static_cast<sdn::AsynchronousSubscriber*>(NULL) != __sub) &&
		 (static_cast<ccs::types::AnyValue*>(NULL) != __payload));

  ccs::types::uint32 value = 0u;

  if (status)
    {
      status = (STATUS_SUCCESS == __sub->CopyTopicInstance(GetPayload(), GetSize()));
    }

  if (status)
    {
      status = ccs::HelperTools::GetAttributeValue(__payload, "seed", value);
    }

  if (status)
    {
      log_info("Subscriber::GetSeed - .. success");
    }
  else
    {
      log_error("Subscriber::GetSeed - .. failure");
    }

  return value;

}

ccs::types::AnyValue Subscriber::GetConfig (void)
{

  bool status = ((static_cast<sdn::AsynchronousSubscriber*>(NULL) != __sub) &&
		 (static_cast<ccs::types::AnyValue*>(NULL) != __payload));

  ccs::types::AnyValue value;

  if (status)
    {
      status = ccs::HelperTools::HasAttribute(__payload, "status");
    }

  if (status)
    {
      value = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(__payload, "status");
    }

  return value;

}

bool ConfigurationHandler::ReadConfiguration (const std::string& name, ccs::types::AnyValue& value) const
{ 

  log_info("ConfigurationHandler::ReadConfiguration('%s') - Entering method", name.c_str());

  bool status = ((static_cast<Publisher*>(NULL) != __pub) &&
		 (static_cast<Subscriber*>(NULL) != __sub));

  if (status)
    {
      status = (name.empty() || (name == std::string(__target)));
    }

  // Reset command

  if (status)
    {
      status = __pub->SetOper(0u);
    }

  if (status)
    {
      status = __pub->Publish();
      ccs::HelperTools::SleepFor(100000000ul);
    }

  // As received from the FPGA
  value = __sub->GetConfig();

  if (status)
    {
      log_info("ConfigurationHandler::ReadConfiguration('%s') - .. success", name.c_str());
    }
  else
    {
      log_error("ConfigurationHandler::ReadConfiguration('%s') - .. failure", name.c_str());
    }

  log_info("ConfigurationHandler::ReadConfiguration('%s') - Leaving method", name.c_str());

  return status; 

}

bool ConfigurationHandler::LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum)
{ 

  bool status = ((static_cast<Publisher*>(NULL) != __pub) &&
		 (static_cast<Subscriber*>(NULL) != __sub));

  if (status)
    {
      status = (name.empty() || (name == std::string(__target)));
    }

  // Provide to CIS-PPM

  if (status)
    {
      status = __client->IsConnected();
    }

  if (status)
    {
      ccs::types::CompoundType request_t;
      request_t.AddAttribute("name", "string");
      request_t.AddAttribute("hash", "uint32");

      ccs::types::AnyValue request (request_t);
      ccs::HelperTools::SetAttributeValue(&request, "name", name.c_str());
      ccs::HelperTools::SetAttributeValue(&request, "hash", checksum);

      ccs::types::AnyValue reply = __client->SendRequest(request);

      status = ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&reply, "status");
    }

  // Reset command

  if (status)
    {
      status = __pub->SetOper(0u);
    }

  if (status)
    {
      status = __pub->Publish();
      ccs::HelperTools::SleepFor(100000000ul);
    }

  // Publish command = 2u

  if (status)
    {
      status = __pub->SetOper(2u);
    }

  if (status)
    {
      status = __pub->SetHash(checksum);
    }

  if (status)
    {
      status = __pub->SetConfig(value);
    }

  if (status)
    {
      status = __pub->Publish();
      ccs::HelperTools::SleepFor(100000000ul);
    }

  // Reset command

  if (status)
    {
      status = __pub->SetOper(0u);
    }

  if (status)
    {
      status = __pub->Publish();
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (status)
    { // Asynchronous handling of status message
      ccs::HelperTools::SleepFor(100000000ul);
      ccs::types::uint32 hash = __sub->GetHash();
      status = (hash == checksum);
    }

  if (status)
    {
      log_info("ConfigurationHandler::LoadConfiguration('%s') - .. success", name.c_str());
    }
  else
    {
      log_error("ConfigurationHandler::LoadConfiguration('%s') - .. failure", name.c_str());
    }

  return status; 

}

bool ConfigurationHandler::GetSeed (const std::string& name, ccs::types::uint32& seed) const 
{ 

  bool status = ((static_cast<Publisher*>(NULL) != __pub) &&
		 (static_cast<Subscriber*>(NULL) != __sub));

  if (status)
    {
      status = (name.empty() || (name == std::string(__target)));
    }

  // Reset command

  if (status)
    {
      status = __pub->SetOper(0u);
    }

  if (status)
    {
      status = __pub->Publish();
      ccs::HelperTools::SleepFor(100000000ul);
    }

  // Publish command = 1u

  if (status)
    {
      status = __pub->SetOper(1u);
    }

  if (status)
    {
      status = __pub->Publish();
      ccs::HelperTools::SleepFor(100000000ul);
    }

  // Reset command

  if (status)
    {
      status = __pub->SetOper(0u);
    }

  if (status)
    {
      status = __pub->Publish();
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (status)
    { // Asynchronous handling of status message
      ccs::HelperTools::SleepFor(100000000ul);
      seed = __sub->GetSeed();
    }

  if (status)
    {
      log_info("ConfigurationHandler::GetSeed('%s') - .. success", name.c_str());
    }
  else
    {
      log_error("ConfigurationHandler::GetSeed('%s') - .. failure", name.c_str());
    }

  return status;

}
#if 1
bool ConfigurationHasher::ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum) const
{ 

  bool status = (NULL != value.GetInstance());

  ccs::types::uint32 size = sizeof(seed) + value.GetSize();

  MARTe::uint8* buffer = static_cast<MARTe::uint8*>(NULL);

  if (status)
    {
      buffer = new (std::nothrow) MARTe::uint8 [size];
      status = (static_cast<MARTe::uint8*>(NULL) != buffer);
    }

  if (status)
    {
      memcpy(buffer, &seed, sizeof(seed));
      memcpy(&(buffer[sizeof(seed)]), value.GetInstance(), value.GetSize());
    }

  if (status)
    { // Compute CRC
      checksum = __crc.Compute(buffer, size, 0u, false);
    }
  
  return status; 

}

bool ConfigurationHasher::VerifyChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum) const
{ 

  bool status = (NULL != value.GetInstance());
  
  ccs::types::uint32 sum;

  if (status)
    {
      status = ComputeChecksum(name, value, seed, sum);
    }

  if (status)
    {
      status = (sum == checksum);
    }
  
  return status;

}
#endif
bool Publisher::SetParameter (const char* name, const char* value)
{ 

  bool status = (!ccs::HelperTools::IsUndefinedString(name) && !ccs::HelperTools::IsUndefinedString(value));

  if (status)
    {
      if (std::string(name) == "type") // Payload datatype
	{
	  status = (static_cast<ccs::types::AnyValue*>(NULL) == __payload);

	  std::shared_ptr<const ccs::types::AnyType> type;

	  if (status)
	    {
	      log_info("Publisher::SetParameter - Get type '%s' description from database ..", value);
	      type = ccs::types::GlobalTypeDatabase::GetType(value);
	      status = static_cast<bool>(type);
	    }

	  if (status)
	    {
	      char buffer [1024] = STRING_UNDEFINED; ccs::HelperTools::Serialise(type, buffer, 1024u);
	      log_info("Publisher::SetParameter - .. '%s' ..", buffer);
	    }

	  if (status)
	    {
	      log_info("Publisher::SetParameter - Instantiate payload of size '%u'", type->GetSize());
	      __payload = new (std::nothrow) ccs::types::AnyValue (type);
	      status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);
	    }

	  if (status)
	    {
	      char buffer [1024] = STRING_UNDEFINED; __payload->SerialiseInstance(buffer, 1024u);
	      log_info("Publisher::SetParameter - .. '%s'", buffer);
	    }
	}

      if (std::string(name) == "name") // Topic name
	{
	  status = ((static_cast<ccs::types::AnyValue*>(NULL) != __payload) &&
		    (static_cast<sdn::Publisher*>(NULL) == __pub));

	  if (status)
	    {
	      log_info("Publisher::SetParameter - Instantiate publisher");
	      sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, value, __payload->GetSize(), 0u);
	      __pub = new (std::nothrow) sdn::Publisher (mdata);
	      status = (static_cast<sdn::Publisher*>(NULL) != __pub);
	    }
	}

      if (std::string(name) == "interface") // SDN network interface
	{
	  status = (static_cast<sdn::Publisher*>(NULL) != __pub);

	  if (status)
	    {
	      status = (STATUS_SUCCESS == __pub->SetInterface(value));
	    }

	  if (status)
	    {
	      status = (STATUS_SUCCESS == __pub->Configure());
	    }
	}

      if ((std::string(name) == "verbose") && (std::string(value) == "true"))
	{
	  ccs::log::SetStdout();
	}
    }

  return status; 

}

bool Subscriber::SetParameter (const char* name, const char* value)
{ 

  bool status = (!ccs::HelperTools::IsUndefinedString(name) && !ccs::HelperTools::IsUndefinedString(value));

  if (status)
    {
      if (std::string(name) == "type") // Payload datatype
	{
	  status = (static_cast<ccs::types::AnyValue*>(NULL) == __payload);

	  std::shared_ptr<const ccs::types::AnyType> type;

	  if (status)
	    {
	      type = ccs::types::GlobalTypeDatabase::GetType(value);
	      status = static_cast<bool>(type);
	    }

	  if (status)
	    {
	      __payload = new (std::nothrow) ccs::types::AnyValue (type);
	      status = (static_cast<ccs::types::AnyValue*>(NULL) != __payload);
	    }
	}

      if (std::string(name) == "name") // Topic name
	{
	  status = ((static_cast<ccs::types::AnyValue*>(NULL) != __payload) &&
		    (static_cast<sdn::AsynchronousSubscriber*>(NULL) == __sub));

	  if (status)
	    {
	      sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, value, __payload->GetSize(), 0u);
	      __sub = new (std::nothrow) sdn::AsynchronousSubscriber (mdata);
	      status = (static_cast<sdn::AsynchronousSubscriber*>(NULL) != __sub);
	    }
	}

      if (std::string(name) == "interface") // SDN network interface
	{
	  status = (static_cast<sdn::AsynchronousSubscriber*>(NULL) != __sub);

	  if (status)
	    {
	      status = (STATUS_SUCCESS == __sub->SetInterface(value));
	    }

	  if (status)
	    {
	      status = (STATUS_SUCCESS == __sub->Configure());
	    }
	}

      if ((std::string(name) == "verbose") && (std::string(value) == "true"))
	{
	  ccs::log::SetStdout();
	}
    }

  return status; 

}

bool ConfigurationHandler::SetParameter (const char* name, const char* value)
{ 

  bool status = (!ccs::HelperTools::IsUndefinedString(name) && !ccs::HelperTools::IsUndefinedString(value));

  if (status)
    {
      if (std::string(name) == "target") // Named dataset
	{
	  ccs::HelperTools::SafeStringCopy(__target, value, ccs::types::MaxStringLength);
	}

      if (std::string(name) == "config") // Publisher instance
	{
	  status = (static_cast<ccs::base::AnyObject*>(NULL) != ccs::base::GlobalObjectDatabase::GetInstance(value));

	  if (status)
	    {
	      __pub = dynamic_cast<Publisher*>(ccs::base::GlobalObjectDatabase::GetInstance(value));
	      status = (static_cast<Publisher*>(NULL) != __pub);
	    }
	  
	  // Reset command
	  
	  if (status)
	    {
	      status = __pub->SetOper(0u);
	    }
	  
	  if (status)
	    {
	      status = __pub->Publish();
	    }
	}

      if (std::string(name) == "status") // Subscriber instance
	{
	  status = (static_cast<ccs::base::AnyObject*>(NULL) != ccs::base::GlobalObjectDatabase::GetInstance(value));

	  if (status)
	    {
	      __sub = dynamic_cast<Subscriber*>(ccs::base::GlobalObjectDatabase::GetInstance(value));
	      status = (static_cast<Subscriber*>(NULL) != __sub);
	    }
	}

      if (std::string(name) == "ppm-service") // CIS PPM client
	{
	  status = (static_cast<ccs::base::RPCClient*>(NULL) == __client);

	  if (status)
	    {
	      __client = new (std::nothrow) ccs::base::RPCClient (value);
	      status = (static_cast<ccs::base::RPCClient*>(NULL) != __client);
	    }
	}

      if ((std::string(name) == "verbose") && (std::string(value) == "true"))
	{
	  ccs::log::SetStdout();
	}
    }

  return status; 

}

bool ConfigurationHasher::SetParameter (const char* name, const char* value)
{ 

  bool status = (!ccs::HelperTools::IsUndefinedString(name) && !ccs::HelperTools::IsUndefinedString(value));

  if (status)
    {
      if (std::string(name) == "target") // Named dataset
	{
	  ccs::HelperTools::SafeStringCopy(__target, value, ccs::types::MaxStringLength);
	}

      if (std::string(name) == "key") // CRC polynomial
	{
	  sscanf(value, "%u", &__key);
	  __crc.ComputeTable(__key);
	}

      if ((std::string(name) == "verbose") && (std::string(value) == "true"))
	{
	  ccs::log::SetStdout();
	}
    }

  return status; 

}

Publisher::Publisher (void) {}
Publisher::~Publisher (void) {}

Subscriber::Subscriber (void) {}
Subscriber::~Subscriber (void) {}

ConfigurationHandler::ConfigurationHandler (void) {}
ConfigurationHandler::~ConfigurationHandler (void) {}

ConfigurationHasher::ConfigurationHasher (void) {}
ConfigurationHasher::~ConfigurationHasher (void) {}

} // namespace _55a0

#undef LOG_ALTERN_SRC

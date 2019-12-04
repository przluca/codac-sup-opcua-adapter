/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/sdn-topic.cpp $
* $Id: sdn-topic.cpp 100239 2019-06-23 15:40:27Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: SDN - Infrastructure tools - Prototype
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

/* Global header files */

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <new> /* std::nothrow */

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */
#include "sdn-tools.h" /* Misc. helper functions, e.g. Hash<uint16_t>, etc. */

#include "sdn-base.h" /* Privately scoped base classes definition */

#include "sdn-topic.h" /* This class definition */

/* Constants */

#undef DEFAULT_TOPIC_NAME
#define DEFAULT_TOPIC_NAME "reserved"

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::topic"

namespace sdn {

/* Type definition */

/* Global variables */

/* Function declaration */

/* Function definition */

namespace TopicHelper {

/* Global variables */

uint_t g_parser_init = 0u; // ToDo - Look into this as atomic variable

/* Function definition */

RET_STATUS GenerateMCastAddress (const char* name, char* group, uint_t& port) // Bug 9729 - This method missing for final porting of SDN API
{

  log_trace("%s - Entering method", __FUNCTION__);

  RET_STATUS status = STATUS_ERROR;

  if (IsUndefined(name) == true) return status;

  uint16_t key = HelperTools::Hash<uint16_t>(name); 
  uint8_t c = (key >> 8) ^ 255; 
  uint8_t d = key ^ 255;

  snprintf(group, MAX_IP_ADDR_LENGTH, "239.0.%d.%d", c, d); // Assume memory size is appropriate
#if 0 /* Bug 9225 - Ports below 1024 require superuser priviledges */
  if (key == 0) key = 10000; /* Port 0 is invalid */
#else
  if (key < 1024) 
    { 
      log_notice("Interoperability risk - The generated port '%u' will be altered with respect to previous unit versions (< v1.0.6)", key);
      key += 10000;
    }
#endif
  port = key;

  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

}

bool IsMetadataValid (Metadata_t& mdata)
{

  log_trace("%s - Entering method", __FUNCTION__);

  if (IsUndefined((char*) mdata.name) == true) return false; /* Must have name defined */

  if (mdata.size == 0) return false; /* Must have size defined */

  /* Bug 9249 - Natural payload size limit for SDN over UDP/IPv4 */
  if (mdata.size > MAX_TOPIC_SIZE)
    {
      log_error("Topic size '%u' is invalid - Payload limited to '%u'", mdata.size, MAX_TOPIC_SIZE); 
      return false;
    }

  /* Generate MCastAddress if necessary */
  if ((IsUndefined((char*) mdata.mcast_group) == true) || (IsAddressValid(mdata) != true))
    {
      log_notice("Generate topic mapping from hash table");
      GenerateMCastAddress(mdata);
    }

  if (mdata.mcast_port == 0)
    {
      log_error("Topic mapping can not be using port '0'");
      return false;
    }

  /* Bug 9225 - Ports below 1024 require superuser priviledges */
  if (mdata.mcast_port < 1024)
    {
      log_warning("Topic mapping is using port '%u' in a restricted range which requires superuser priviledges", mdata.mcast_port);
    }

  log_trace("%s - Leaving method", __FUNCTION__);

  return true;

}

void InitializeMetadata (Metadata_t& mdata) { memset((void*) &mdata, 0, sizeof(Metadata_t)); return; }
void InitializeMetadata (Metadata_t& mdata, const char* name) 
{ 

  InitializeMetadata(mdata); 

  if (strncmp(name, "sdn://", 6) == 0) 
    {

      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; sstrncpy((char*) buffer, (char*) name + 6, STRING_MAX_LENGTH);

      char* p_addr = buffer;

      /* Put a zero in lieu of ':' */
      char* p_char = buffer; while ((*p_char != ':') && (*p_char != 0)) p_char += 1; *p_char = 0;

      if (sdn_is_address_valid(p_addr) == true) 
	{
	  //log_debug("%s - Address '%s' is valid", __FUNCTION__, p_addr);
	  sstrncpy(mdata.mcast_group, p_addr, MAX_IP_ADDR_LENGTH); 
	}

      p_addr = p_char + 1; /* The port number */

      /* Put a zero in lieu of '/' */
      p_char = p_addr; while ((*p_char != '/') && (*p_char != 0)) p_char += 1; *p_char = 0;

      //log_debug("%s - Parsing '%s' as port", __FUNCTION__, p_addr);
      sscanf(p_addr, "%u", &(mdata.mcast_port));

      p_addr = p_char + 1; /* The actual topic name */

      //log_debug("%s - Parsing '%s' as topic name", __FUNCTION__, p_addr);
      sstrncpy((char*) mdata.name, p_addr, STRING_MAX_LENGTH);

    }
  else sstrncpy((char*) mdata.name, (char*) name, STRING_MAX_LENGTH); 

  return; 

}

void InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size) { InitializeMetadata(mdata, name); mdata.size = size; return; }
void InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size, const char* addr, uint_t port)
{ 

  InitializeMetadata(mdata, name, size); mdata.mcast_port = port; 

  if (sdn_is_address_valid(addr) == true)
    {

      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; sstrncpy((char*) buffer, (char*) addr, STRING_MAX_LENGTH);

      /* The address may include the port in the form '<addr>:<port>' */
      bool found = false; char* p_char = buffer; while ((*p_char != ':') && (*p_char != 0)) p_char += 1; if (*p_char ==  ':') found = true;

      if (found == true)
	{
	  /* Put a zero in lieu of ':' */
	  *p_char = 0; 
	  char* port_str = p_char + 1; /* The port number */

	  sstrncpy(mdata.mcast_group, (char*) buffer, MAX_IP_ADDR_LENGTH); 
	  sscanf(port_str, "%u", &(mdata.mcast_port));
	}
      else sstrncpy(mdata.mcast_group, (char*) addr, MAX_IP_ADDR_LENGTH); 

    }

  return; 

}

void InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size, uint_t vers) { InitializeMetadata(mdata, name, size); mdata.vers = vers; return; }

} /* namespace TopicHelper */

/* Initializer methods */

RET_STATUS Topic::AddAttribute (uint_t rank, const char* name, const char* type, uint_t mult) 
{ 

  RET_STATUS status = STATUS_ERROR; 

  if (this->m_type_def != NULL) 
    { 
      char rank_str [STRING_MAX_LENGTH] = STRING_UNDEFINED; 
      char mult_str [STRING_MAX_LENGTH] = STRING_UNDEFINED; 

      // Not using the rank attribute
      (void) rank;

      snprintf((char*) mult_str, STRING_MAX_LENGTH, "%u", mult); 

      status = (this->m_type_def)->AddAttribute(rank_str, (char*) name, (char*) type, mult_str); 

    } 

  this->SetUID(); 

  return status; 

}

void Topic::ClearInstance (void)
{ 

  if (this->m_instance != NULL) memset(this->m_instance, 0, this->GetSize()); 

  /* Delegate to AnyType instance, if possible */
  if ((this->m_type_def != NULL) && ((this->m_type_def)->IsInitialized() == true)) (this->m_type_def)->ClearInstance();

  return; 

}

RET_STATUS Topic::CreateInstance (void) 
{

  RET_STATUS status = STATUS_ERROR;

  log_trace("Topic::CreateInstance - Entering method");

  /* Delegate to AnyType instance, if possible */
  if ((this->m_type_def != NULL) && ((this->m_type_def)->IsInitialized() == true) && ((status = (this->m_type_def)->CreateInstance()) == STATUS_SUCCESS))
    {
      this->m_instance = (uint8_t*) (this->m_type_def)->GetInstance();
      status = STATUS_SUCCESS;
    }

  /* Do it if only if size is known */
  if ((this->GetSize() != 0) && (this->m_instance == NULL))
    {
      this->m_instance = new uint8_t [this->GetSize()];
      this->m_allocated = true;
      status = STATUS_SUCCESS;
    }

  if (status != STATUS_SUCCESS) log_error("Unable to instantiate '%s' topic - Not enough information available", this->GetName());

  log_trace("Topic::CreateInstance - Leaving method");

  return status;

}

RET_STATUS Topic::Configure (void) // Called after the topic is initialised
{

  /* 
     Done - Parse XML file as static inline helper method.
     Done - Re-design to let the calling application decide whether the topic is
            intialized from name/XML file or through the API.
  */

  RET_STATUS status = STATUS_ERROR;

  log_trace("Topic::Configure - Entering method");

  if ((this->m_type_def)->IsDefined() != true)
    {
      //log_warning("Unable to locate '%s' type definition file", this->GetName());
      this->SetUID(); /* Needed for subscriber who fail to load XML file */
      
      log_notice("Topic '%s' defined by size '%u' only", this->GetName(), this->GetSize());
      (this->m_type_def)->ccs::base::AnyType::AddAttribute(0, (char*) "undefined", this->GetSize());
      (this->m_type_def)->AnyType::AddExtAttribute(NULL, (char*) "undefined", NULL, NULL, NULL, NULL);
    }
  else
    {

      log_notice("Topic '%s' type definition done through API", this->GetName());

      uint_t size = (this->m_type_def)->GetSize();

      if ((this->GetSize() == 0) && (size != 0))
	{
	  this->SetSize(size);
	  log_notice("Topic '%s' size inferred from datatype definition '%u'", this->GetName(), this->GetSize());
	}

      if ((this->GetSize() != 0) && (size != 0) && (this->GetSize() != size))
	{
	  log_warning("Topic '%s' size mismatch between metadata '%u' and datatype definition '%u'", this->GetName(), this->GetSize(), size);
	  this->SetSize(size);
	  log_notice("Topic '%s' size inferred from datatype definition '%u'", this->GetName(), this->GetSize());
	}

    }

  this->m_instance = NULL;

  this->m_allocated = false;

  /* Check validity of metadata */
  if (TopicHelper::IsMetadataValid(this->m_meta) != true)
    {
      log_error("Invalid '%s' topic definition", this->GetName());
      return status;
    }

  status = STATUS_SUCCESS;

  log_trace("Topic::Configure - Leaving method");

  return status;

}

void Topic::Initialize (void)
{

  log_trace("Topic::Initialize - Entering method");

  /* Initialize attributes */
  TopicHelper::InitializeMetadata(this->m_meta);

  this->m_type_def = new (std::nothrow) sdn::base::AnyType;;
  this->m_instance = NULL;

  this->m_allocated = false;

  log_trace("Topic::Initialize - Leaving method");

  return;

}

/* Accessor methods */

const char* Topic::HasInstanceIndex (void) const
{ 

  const char* p_attr = NULL;

  bool ok = (static_cast<base::AnyType*>(NULL) != this->m_type_def);

  for (uint_t index = 0u; ((index < (this->m_type_def)->GetRank()) && (ok)); index += 1u) 
    { 
      const char* p_qual = (this->m_type_def)->GetAttributeQualifier(index); 

      if ((p_qual != NULL) && (strcmp(p_qual, "samplenb") == 0)) 
	{
	  p_attr = (this->m_type_def)->GetAttributeName(index); 
	  log_debug("Topic::HasInstanceIndex - Found attribute '%s' at '%u'", p_attr, index);
	  break;
	}
    } 

  return p_attr; 

}

const char* Topic::HasInstanceTimestamp (void) const
{ 

  const char* p_attr = NULL;

  bool ok = (static_cast<base::AnyType*>(NULL) != this->m_type_def);

  for (uint_t index = 0u; ((index < (this->m_type_def)->GetRank()) && (ok)); index += 1u) 
    { 
      const char* p_qual = (this->m_type_def)->GetAttributeQualifier(index); 

      if ((p_qual != NULL) && (strcmp(p_qual, "timestamp") == 0)) 
	{
	  p_attr = (this->m_type_def)->GetAttributeName(index); 
	  log_debug("Topic::HasInstanceTimestamp - Found attribute '%s' at '%u'", p_attr, index);
	  break;
	}
    } 

  return p_attr; 

}

void Topic::GetMetadata (Metadata_t& mdata) const { mdata = this->m_meta; return; }
void Topic::SetMetadata (Metadata_t& mdata) { this->m_meta = mdata; return; }

bool Topic::IsName (const char* name) const { return ((strcmp((char*) this->m_meta.name, name) == 0) ? true : false); }
const char* Topic::GetName (void) const { return this->m_meta.name; }
void Topic::SetName (const char* name) { sstrncpy(this->m_meta.name, name, STRING_MAX_LENGTH); return; }

uint_t Topic::GetInstanceChecksum (void) const
{ 

  uint32_t chksum = ((this->GetInstance() != NULL) ? HelperTools::CyclicRedundancyCheck<uint32_t>((uint8_t*) this->GetInstance(), this->GetSize()) : 0); 

  log_debug("Topic::GetInstanceChecksum - Instance checksum is '%u' (over '%p' reference and '%u' size)", chksum, this->GetInstance(), this->m_meta.size); 

  return (uint_t) chksum; 

}

bool Topic::HasDescription (void) const { return (IsUndefined(this->m_meta.desc) == false); }
const char* Topic::GetDescription (void) const { return this->m_meta.desc; }
bool Topic::GetDescription (char* desc, types::uint32 size) const { bool status = HasDescription(); if (status) ccs::HelperTools::SafeStringCopy(desc, GetDescription(), size); return status; }
void Topic::SetDescription (const char* desc) { ccs::HelperTools::SafeStringCopy(this->m_meta.desc, desc, 128u); return; } /* Warning - Size of string */

uint_t Topic::GetSize (void) const { return this->m_meta.size; }
void Topic::SetSize (uint_t size) { this->m_meta.size = size; return; }
void Topic::SetSize (const char* size_str) { sscanf(size_str, "%u", &(this->m_meta.size)); return; }

uint_t Topic::GetUID (void) const { return this->m_meta.hash; }
void Topic::SetUID (uint_t hash) { this->m_meta.hash = hash; return; }
void Topic::SetUID (const char* uid_str) { sscanf(uid_str, "%u", &(this->m_meta.hash)); return; }

void Topic::SetUID (void) 
{ 

  log_trace("Topic::SetUID - Entering method");
#if 0
  uint_t hash = ((this->m_type_def == NULL) ? 0 : (this->m_type_def)->SerializeType()); 
#else
  char buffer [2048] = STRING_UNDEFINED;
  uint_t hash = ((this->m_type_def == NULL) ? 0 : (this->m_type_def)->SerializeType(buffer, 2048));
  log_info("Topic::SetUID - Serialized datatype is '%s'", buffer);
#endif
  log_info("Topic::SetUID - Hash is '%u'", hash);

  log_trace("Topic::SetUID - Leaving method");

  return this->SetUID(hash); 

}

uint_t Topic::GetVersion (void) const { return this->m_meta.vers; }
void Topic::SetVersion (const char* vers) { sscanf(vers, "%u", &(this->m_meta.vers)); return; }
void Topic::SetVersion (uint_t vers) { this->m_meta.vers = vers; return; }

const char* Topic::GetMCastGroup (void) const { return this->m_meta.mcast_group; }
void Topic::SetMCastGroup (const char* group) { sstrncpy(this->m_meta.mcast_group, group, MAX_IP_ADDR_LENGTH); return; } /* Warning - Size of string */
  
uint_t Topic::GetMCastPort (void) const { return this->m_meta.mcast_port; }
void Topic::SetMCastPort (uint_t port) { this->m_meta.mcast_port = port; return; }
void Topic::SetMCastPort (const char* port_str) { sscanf(port_str, "%u", &(this->m_meta.mcast_port)); return; }

void* Topic::GetInstance (void) const { return (void*) this->m_instance; }

void Topic::SetInstance (void* instance) 
{ 

  log_trace("Topic::SetInstance - Entering method");

  this->m_instance = (uint8_t*) instance; if (this->m_type_def != NULL) (this->m_type_def)->SetInstance(instance); 

  log_trace("Topic::SetInstance - Leaving method");

  return;

}

/* Miscellaneous methods */

/* Constructor methods */

Topic::Topic (void)
{

  log_trace("Topic::Topic - Entering method");

  /* Initialize attributes */
  this->Initialize();

  this->SetName((char*) DEFAULT_TOPIC_NAME); /* Topic name */
  this->SetDescription((char*) STRING_UNDEFINED); /* Topic name */
  this->SetSize((uint_t) 0); /* Topic datatype size  */
  this->SetUID((uint_t) 0); /* Topic UID  */
  this->SetVersion((uint_t) 0); /* Topic version  */
  this->SetMCastGroup((char*) DEFAULT_MCAST_GROUP); /* Topic multicast group */
  this->SetMCastPort(DEFAULT_MCAST_PORT); /* Topic multicast port */
#if 0 /* That above may be defined programmatically and sdn::Topic::Configure called as well */
  /* Initialize topic instance */
  if (this->Configure() != STATUS_SUCCESS) log_warning("Unable to initialize '%s' topic", this->GetName());
#endif
  log_trace("Topic::Topic - Leaving method");

  return;

}

Topic::Topic (Metadata_t& mdata)
{

  log_trace("Topic::Topic('%s') - Entering method", mdata.name);

  /* Initialize attributes */
  this->Initialize();

  TopicHelper::InitializeMetadata(this->m_meta);

  this->m_meta = mdata; /* Set metadata */

  /* Initialize topic instance */
  if (this->Configure() != STATUS_SUCCESS) log_warning("Unable to initialize '%s' topic", this->GetName());
  else mdata = this->m_meta;

  log_trace("Topic::Topic('%s') - Leaving method", mdata.name);

  return;

}

/* Destructor method */

Topic::~Topic (void)
{

  log_trace("Topic::~Topic - Entering method");

  /* Delete topic instance, if necessary */
  log_info("Delete '%s' topic instance", this->GetName()); this->DeleteInstance();

  /* Release resources */
  if (this->m_type_def != NULL) delete this->m_type_def;

  /* Clear attributes */
  TopicHelper::InitializeMetadata(this->m_meta);
  this->m_type_def = NULL;

  log_trace("Topic::~Topic - Leaving method");

  return;

}

/* Display methods */

uint_t Topic::SerializeType (char* buffer, int max_size)
{

  log_trace("Topic::SerializeType - Entering method");

  uint_t hash = 0;

  char* p_buf = buffer;
  char* p_tmp = buffer;

  if (buffer == NULL) /* Only interested in hash key */
    {
      p_tmp = static_cast<char*>(malloc(2048));
      p_buf = p_tmp;
      max_size = 2048;
    }

  /* WARNING - Check if size could be negative - yes in case snprintf truncates ... does not append zero  ... and the buffer is not cleared */

  /* Zero the buffer */
  memset(p_buf, 0, max_size); max_size -= 1; /* Make sure the buffer is zero'ed (and the trailing zero won't get overwrittent) so as to avoid strlen failure in case snprintf truncates */

  if (this->IsInitialized() != true) 
    {
      sstrncpy(p_buf, (char*) "</topic>", max_size);
      return hash;
    }

  snprintf(p_buf, max_size, "<topic name=\"%s\"", this->GetName()); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

  if (this->GetUID() != 0)
    {
      snprintf(p_buf, max_size, " uid=\"%u\"", this->GetUID()); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }

  if (this->GetVersion() != 0)
    {
      snprintf(p_buf, max_size, " version=\"%u\"", this->GetVersion()); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }

  snprintf(p_buf, max_size, " size=\"%u\" mapping=\"%s:%u\"", this->GetSize(), this->GetMCastGroup(), this->GetMCastPort()); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

  if ((this->m_type_def != NULL) && ((this->m_type_def)->IsInitialized() == true))
    {
      sstrncpy(p_buf, (char*) ">", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      /* Serialize datatype definition */
      hash = (this->m_type_def)->SerializeType(p_buf, max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
      
      /* Terminate buffer */
      sstrncpy(p_buf, (char*) "</topic>", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }
  else /* The topic datatype has not been defined */
    {
      /* Terminate buffer */
      sstrncpy(p_buf, (char*) "/>", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }

  if (buffer == NULL)
    {
      free(p_tmp);
    }

  log_trace("Topic::SerializeType - Leaving method");

  return hash; /* Hask key from datatype definition, if any */

}

RET_STATUS Topic::SerializeInstance (char* buffer, int max_size) 
{

  log_trace("Topic::SerializeInstance - Entering method");

  if (this->IsInitialized() != true)
    {
      sstrncpy(buffer, (char*) "</value>", max_size); 
      return STATUS_SUCCESS;
    }

  /* WARNING - Check if size could be negative */

  if ((this->m_type_def != NULL) && ((this->m_type_def)->IsInitialized() == true))
    {
      (this->m_type_def)->SerializeInstance(buffer, max_size); 
    }
  else
    {
      /* Use base64 encoding */
      char* p_buf = buffer; sstrncpy(p_buf, (char*) "<value encoding=\"base64\">", max_size); max_size -= strlen(p_buf); p_buf = buffer + strlen(buffer); /* Re-align pointer */
      ccs::HelperTools::Base64Encode((uint8_t*) this->m_instance, this->GetSize(), p_buf, max_size); max_size -= strlen(p_buf); p_buf = buffer + strlen(buffer); /* Re-align pointer */
      sstrncpy(p_buf, (char*) "</value>", max_size); 
    }

  log_trace("Topic::SerializeInstance - Leaving method");

  return STATUS_SUCCESS;

}

} /* namespace sdn */

#undef LOG_ALTERN_SRC


/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/disc-messenger.cpp $
* $Id: disc-messenger.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */
#include "sdn-tools.h" /* Misc. helper functions, e.g. hash, etc. */

#include "sdn-base.h" /* Privately scoped base classes definition */

#include "mcast-api.h"

#include "core-participant.h"
#include "core-publisher.h"
#include "core-subscriber.h"

#include "disc-participant.h"
#include "disc-messenger.h" /* This class definition */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::disc"

/* Type definition */

namespace sdn {

namespace disc {

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initializer methods */

void MessengerImpl::Initialize (void)
{

  log_trace("MessengerImpl::Initialize - Entering method"); 

  /* Initialize attributes */
  this->m_base = NULL; /* ccs::mcast::Messenger instance */
  this->m_participant = NULL; /* core::Participant instance */

  log_trace("MessengerImpl::Initialize - Leaving method"); 

  return;

}

/* Accessor methods */

/* Miscellaneous methods */

RET_STATUS MessengerImpl::Configure (void)
{

  log_trace("MessengerImpl::Configure - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  /* Configure base class */
  if (Participant_Configure((Participant*) this) != STATUS_SUCCESS)
    {
      log_error("MessengerImpl::Configure - Failed to configure participant");
      return status;
    }

  /* Create generic messenger, if necessary */
  if ((this->m_base != NULL) && 
      ((strcmp((this->m_base)->GetAddress(), this->GetMCastGroup()) != 0) ||
       ((this->m_base)->GetPort() != this->GetMCastPort()) ||
       ((this->m_base)->GetInterface() != this->GetInterface())))
    {
      log_debug("MessengerImpl::Configure - Closing ccs::mcast::Messenger instance");
      (this->m_base)->Close();
    }

  if (this->m_base == NULL) { log_debug("Messenger::Configure - Creating ccs::mcast::Messenger instance"); this->m_base = new ccs::mcast::Messenger (); }
  log_debug("MessengerImpl::Configure - Configure ccs::mcast::Messenger instance");
  (this->m_base)->SetInterface(this->GetInterface()); (this->m_base)->SetAddress(this->GetMCastGroup()); (this->m_base)->SetPort(this->GetMCastPort()); (this->m_base)->Open();

  log_debug("MessengerImpl::Configure - Defining ccs::mcast::Messenger buffer and size");
  (this->m_base)->SetBuffer(this->GetBuffer(), this->GetSize());

  status = STATUS_SUCCESS; 
  
  log_trace("MessengerImpl::Configure - Leaving method"); 

  return status;

}

RET_STATUS MessengerImpl::Publish (void) 
{ 

  log_trace("MessengerImpl::Publish - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  sdn::core::ParticipantImpl* ref = this->GetParticipant();

  if (ref != NULL)
    {

      char* p_buf = (char*) this->GetBuffer(); uint_t max_size = this->GetSize();

      /* Bug 10327 - Provide platform endianness as part of the discovery message */
      snprintf(p_buf, max_size, "<message group=\"discovery\" qualifier=\"response\" schemaVersion=\"1.0\"><participant hostName=\"%s\" applicationName=\"%s\" pid=\"%u\" interfaceName=\"%s\" ipAddress=\"%s\" byteOrder=\"%s\" role=\"", 
	       this->m_host_name, 
	       this->m_prog_name, 
	       this->m_prog_id, 
	       this->m_base->GetInterface(), 
	       this->m_base->GetInterfaceAddress(),
	       ((HelperTools::IsLittleEndian() == true) ? "little-endian" : "big-endian")); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      if (ref->IsType(OBJTYPE_CORE_PUBLISHER) == true)
	{
	  strncpy(p_buf, (char*) "publisher\">", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}
      else if (ref->IsType(OBJTYPE_CORE_SUBSCRIBER) == true)
	{
	  strncpy(p_buf, (char*) "subscriber\">", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}

      /* Serialize topic definition */
      (ref->m_topic)->SerializeType(p_buf, max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      strncpy(p_buf, (char*) "</participant></message>", max_size);

      /* Publish discovery message on the network */
      log_info("MessengerImpl::Publish - Message '%s'", (char*) this->GetBuffer());
      if (this->m_base != NULL) (this->m_base)->Publish();

    }

  status = STATUS_SUCCESS;

  log_trace("MessengerImpl::Publish - Leaving method"); 

  return status; 

}

RET_STATUS MessengerImpl::Receive (const uint64_t timeout) 
{ 

  log_trace("MessengerImpl::Receive - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  /* Receive discovery message on the network */
  if (this->m_base != NULL) status = (this->m_base)->Receive(timeout);

  if (status == STATUS_SUCCESS)
    {
      log_info("MessengerImpl::Receive - Message '%s'", (char*) this->GetBuffer());

      if (this->IsQuery((char*) this->GetBuffer()) == true)
	{
	  status = this->Publish();
	}
    }

  log_trace("MessengerImpl::Receive - Leaving method"); 

  return status; 

}

RET_STATUS MessengerImpl::Receive (void) 
{ 

  log_trace("MessengerImpl::Receive - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  /* Receive discovery message on the network */
  if (this->m_base != NULL) status = (this->m_base)->Receive();

  if (status == STATUS_SUCCESS)
    {
      log_info("MessengerImpl::Receive - Message '%s'", (char*) this->GetBuffer());

      if (this->IsQuery((char*) this->GetBuffer()) == true)
	{
	  status = this->Publish();
	}
    }

  log_trace("MessengerImpl::Receive - Leaving method"); 

  return status; 

}

RET_STATUS MessengerImpl::Register (sdn::core::ParticipantImpl* ref) 
{ 

  log_trace("MessengerImpl::Register - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  if (this->m_base != NULL) 
    {

      char* p_buf = (char*) this->GetBuffer(); uint_t max_size = this->GetSize();

      /* Bug 10327 - Provide platform endianness as part of the discovery message */
      snprintf(p_buf, max_size, "<message group=\"status\" qualifier=\"startup\" schemaVersion=\"1.0\"><participant hostName=\"%s\" applicationName=\"%s\" pid=\"%u\" interfaceName=\"%s\" ipAddress=\"%s\" byteOrder=\"%s\" role=\"", 
	       this->m_host_name, 
	       this->m_prog_name, 
	       this->m_prog_id, 
	       this->m_base->GetInterface(), 
	       this->m_base->GetInterfaceAddress(),
	       ((HelperTools::IsLittleEndian() == true) ? "little-endian" : "big-endian")); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      if (ref->IsType(OBJTYPE_CORE_PUBLISHER) == true)
	{
	  strncpy(p_buf, (char*) "publisher\">", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}
      else if (ref->IsType(OBJTYPE_CORE_SUBSCRIBER) == true)
	{
	  strncpy(p_buf, (char*) "subscriber\">", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}

      /* Serialize topic definition */
      (ref->m_topic)->SerializeType(p_buf, max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      strncpy(p_buf, (char*) "</participant></message>", max_size);

      /* Publish discovery message on the network */
      log_notice("MessengerImpl::Register - Message '%s'", (char*) this->GetBuffer());
      (this->m_base)->Publish();

    }

  this->m_participant = ref; 

  status = STATUS_SUCCESS;

  log_trace("MessengerImpl::Register - Leaving method"); 

  return status; 

}

RET_STATUS MessengerImpl::Remove (sdn::core::ParticipantImpl* ref) 
{ 

  log_trace("MessengerImpl::Remove - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

    {

      char* p_buf = (char*) this->GetBuffer(); uint_t max_size = this->GetSize();

      snprintf(p_buf, max_size, "<message group=\"status\" qualifier=\"shutdown\" schemaVersion=\"1.0\"><participant hostName=\"%s\" pid=\"%u\" role=\"", this->m_host_name, this->m_prog_id); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      if (ref->IsType(OBJTYPE_CORE_PUBLISHER) == true)
	{
	  strncpy(p_buf, (char*) "publisher\">", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}
      else if (ref->IsType(OBJTYPE_CORE_SUBSCRIBER) == true)
	{
	  strncpy(p_buf, (char*) "subscriber\">", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}

      /* Serialize topic definition */
      (ref->m_topic)->SerializeType(p_buf, max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      strncpy(p_buf, (char*) "</participant></message>", max_size);

      /* Publish discovery message on the network */
      log_notice("MessengerImpl::Remove - Message '%s'", (char*) this->GetBuffer());
      if (this->m_base != NULL) (this->m_base)->Publish();

    }

  this->m_participant = NULL; 

  status = STATUS_SUCCESS;

  log_trace("MessengerImpl::Remove - Leaving method"); 

  return status; 

}

bool MessengerImpl::IsQuery (const char* p_msg)
{

  return Participant_IsQuery(p_msg);

}

/* Constructor methods */

MessengerImpl::MessengerImpl (void) { this->SetInstanceType(OBJTYPE_DISC_MESSENGER); /* Initialize attributes */ this->Initialize(); return; }
MessengerImpl::MessengerImpl (char* iface) { this->SetInstanceType(OBJTYPE_DISC_MESSENGER); /* Initialize attributes */ this->Initialize(); this->SetInterface(iface); /* Create mcast::Messenger */ this->Configure(); return; }

/* Destructor method */

MessengerImpl::~MessengerImpl (void) 
{ 

  log_trace("MessengerImpl::~MessengerImpl - Entering method"); 

  /* Release resources */
  if (this->m_base != NULL) delete this->m_base; this->m_base = NULL; 

  log_trace("MessengerImpl::~MessengerImpl - Leaving method"); 

  return; 

}

} /* namespace disc */

} /* namespace sdn */

#undef LOG_ALTERN_SRC

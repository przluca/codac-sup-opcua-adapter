/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/core-participant.cpp $
* $Id: core-participant.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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
//#include "sdn-core.h" /* SDN core library - API definition (sdn::core) */

#include "sdn-topic.h"
#include "sdn-packet.h"

#include "core-participant.h" /* This class definition */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::core"

/* Type definition */

namespace sdn {

namespace core {

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initializer methods */

void ParticipantImpl::Initialize (void) 
{ 

  log_trace("ParticipantImpl::Initialize - Entering method");

  /* Initialize attributes */ 
  this->m_ext_topic = false;
  this->m_packet = NULL; this->m_topic = NULL; TopicHelper::InitializeMetadata(this->m_mdata); 
  this->m_cb = NULL; this->m_attr = NULL; /* Routine called as part of message handling */
  this->m_depth = 0u;
  
  /* Bug 8722 - Test validity of SDN_INTERFACE_NAME */
  if (HelperTools::IsInterfaceValid() != true) log_notice("ParticipantImpl::Initialize - SDN_INTERFACE_NAME is either not defined or wrongly defined");

  get_env_variable((char*) "SDN_INTERFACE_NAME", (char*) this->m_iface_name); 

  log_trace("ParticipantImpl::Initialize - Leaving method");

  return; 

}

/* Accessor methods */

/* Bug 7919 - Return error in case interface is invalid */
RET_STATUS ParticipantImpl::SetInterface (const char* name) { RET_STATUS status = STATUS_ERROR; if (HelperTools::IsInterfaceValid(name) == true) { sstrncpy((char*) this->m_iface_name, name, STRING_MAX_LENGTH); status = STATUS_SUCCESS; } return status; }
RET_STATUS ParticipantImpl::SetMetadata (Metadata_t& mdata) { RET_STATUS status = STATUS_ERROR; this->m_mdata = mdata; if (TopicHelper::IsMetadataValid(this->m_mdata) == true) status = STATUS_SUCCESS; return status; }
// Deleted - RET_STATUS ParticipantImpl::SetTopicName (const char* name) { sstrncpy((char*) this->m_mdata.name, name, STRING_MAX_LENGTH); return STATUS_SUCCESS; }
RET_STATUS ParticipantImpl::SetTopic (Topic& topic) 
{ 

  log_trace("ParticipantImpl::SetTopic - Entering method");

  RET_STATUS status = STATUS_ERROR; 

  this->m_mdata = topic.m_meta; 
  this->m_topic = &topic; 
  this->m_ext_topic = true; 

  if (TopicHelper::IsMetadataValid(this->m_mdata) == true) 
    {
      status = STATUS_SUCCESS; 
    }

  log_trace("ParticipantImpl::SetTopic - Leaving method");

  return status; 

}

RET_STATUS ParticipantImpl::SetBufferDepth (uint_t depth) { this->m_depth = depth; return STATUS_SUCCESS; }

/* Miscellaneous methods */

RET_STATUS Participant_Configure (ParticipantImpl* self)
{

  log_trace("Participant_Configure - Entering method");

  RET_STATUS status = STATUS_ERROR;

  if ((self->m_topic != NULL) && ((self->m_topic)->IsName((char*) self->m_mdata.name) != true)) /* Topic name has been changed */
    {
      log_debug("Participant::Configure - Deleting topic and packet instances");
      delete self->m_topic; self->m_topic = NULL;
      delete self->m_packet; self->m_packet = NULL;
    }

  /* Instantiate topic using metadata */
  if (self->m_topic == NULL) self->m_topic = new Topic (self->m_mdata);

  if ((self->m_topic)->IsInitialized() != true) /* Incomplete metadata: name only must be supported by XML topic definition file, else metadata must be complete enough */
    {
      /* Initialization failed */
      log_error("Participant_Configure - Failed to initialize topic '%s'", (self->m_topic)->GetName()); 
      return status;
    }
  else
    {
      char buffer [256]; 
      (self->m_topic)->SerializeType((char*) buffer, 256);
      log_info("Participant_Configure - Using topic definition - %s", buffer);
    }

  /* Instantiate packet from payload size */
  if (self->m_packet == NULL) self->m_packet = new Packet ((self->m_topic)->GetSize()); (self->m_packet)->CreateInstance();

  {
    char buffer [256]; 
    (self->m_packet)->SerializeType((char*) buffer, 256);
    log_debug("Participant_Configure - Using packet definition - %s", buffer);
  }

  /* Take the reference to the topic instance */
  (self->m_topic)->SetInstance((self->m_packet)->GetPayload()); (self->m_topic)->ClearInstance();

  /* Prepare header fields */
  ((self->m_packet)->GetHeader())->SetTopicSize((self->m_topic)->GetSize());
  ((self->m_packet)->GetHeader())->SetTopicUID((self->m_topic)->GetUID());
  ((self->m_packet)->GetHeader())->SetTopicVersion((self->m_topic)->GetVersion());
#if 0
  {
    char buffer [256]; 
    ((self->m_packet)->GetHeader())->SerializeInstance((char*) buffer, 256);
    log_debug("Participant_Configure - Using header definition - %s", buffer);
  }
#endif
  status = STATUS_SUCCESS;

  log_trace("Participant_Configure - Leaving method");

  return status;

}

/* Constructor methods */

ParticipantImpl::ParticipantImpl (void) 
{ 

  /* Initialize resources */ 
  this->Initialize(); 

  /* Register to the object database */ 
  //ccs::base::GlobalObjectDatabase::Register((AnyObject*) this); 

  return; 

}

/* Destructor method */

ParticipantImpl::~ParticipantImpl (void) 
{ 

  /* Release resources */ 
  if ((this->m_ext_topic != true) && (this->m_topic != NULL)) delete this->m_topic; 
  if (this->m_packet != NULL) delete this->m_packet; 

  /* Remove from the object database */ 
  //ccs::base::GlobalObjectDatabase::Remove((AnyObject*) this); 

  return; 

}

/* Display methods */

} /* namespace core */

} /* namespace sdn */

#undef LOG_ALTERN_SRC

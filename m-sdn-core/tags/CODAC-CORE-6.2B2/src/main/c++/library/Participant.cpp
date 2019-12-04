/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/Participant.cpp $
* $Id: Participant.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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

#include "core-participant.h"
#include "disc-messenger.h"

#include "ParticipantImpl.h" /* This class definition */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn"

/* Type definition */

namespace sdn {

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initialiser methods */

void ParticipantImpl::Initialise (void) 
{ 

  log_trace("ParticipantImpl::Initialise - Entering method");

  /* Initialise attributes */ 
  this->m_base = static_cast<core::ParticipantImpl*>(NULL);
  this->m_disc = static_cast<disc::MessengerImpl*>(NULL);

  log_trace("ParticipantImpl::Initialise - Leaving method");

  return; 

}

/* Accessor methods */

RET_STATUS ParticipantImpl::SetInterface (const char* name) 
{ 

  RET_STATUS status = STATUS_ERROR; 

  if (static_cast<core::ParticipantImpl*>(NULL) != this->m_base) 
    status = (this->m_base)->SetInterface(name); 

  return status; 

}
  
RET_STATUS ParticipantImpl::SetMetadata (Metadata_t& mdata) 
{ 

  RET_STATUS status = STATUS_ERROR; 

  if (static_cast<core::ParticipantImpl*>(NULL) != this->m_base) 
    status = (this->m_base)->SetMetadata(mdata); 

  return status; 

}

RET_STATUS ParticipantImpl::SetTopic (Topic& topic) 
{ 

  log_trace("ParticipantImpl::SetTopic - Entering method");

  RET_STATUS status = STATUS_ERROR; 

  if (static_cast<core::ParticipantImpl*>(NULL) != this->m_base) 
      status = (this->m_base)->SetTopic(topic); 

  log_trace("ParticipantImpl::SetTopic - Leaving method");

  return status; 

}

RET_STATUS ParticipantImpl::SetBufferDepth (uint_t depth)
{ 
 
  log_trace("ParticipantImpl::SetBufferDepth - Entering method");

  RET_STATUS status = STATUS_ERROR; 

  if (static_cast<core::ParticipantImpl*>(NULL) != this->m_base) 
      status = (this->m_base)->SetBufferDepth(depth); 

  log_trace("ParticipantImpl::SetBufferDepth - Leaving method");

  return status; 

}

/* Miscellaneous methods */

RET_STATUS ParticipantImpl::DoBackgroundActivity (void)
{

  log_trace("ParticipantImpl::DoBackgroundActivity - Entering method");

  RET_STATUS status = STATUS_ERROR;

  if (this->m_disc != NULL) (this->m_disc)->Do(); /* Non-blocking operation - Handle discovery queries, gratuitous status update, etc. */

  status = STATUS_SUCCESS;

  log_trace("ParticipantImpl::DoBackgroundActivity - Leaving method");

  return status;

}

RET_STATUS Participant_Configure (ParticipantImpl* self)
{

  log_trace("%s - Entering method", __FUNCTION__);

  RET_STATUS status = STATUS_ERROR;

  /* Instantiate discovery publisher */
  if (self->m_disc == NULL) self->m_disc = new (std::nothrow) disc::Messenger(); (self->m_disc)->SetInterface(self->GetInterface()); (self->m_disc)->Configure();
 
  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

}

/* Constructor methods */

ParticipantImpl::ParticipantImpl (void)
{
    
  log_trace("ParticipantImpl::ParticipantImpl - Entering method");
    
  /* Initialise resources */
  this->Initialise();
    
  /* Register to the object database */
  //ccs::base::GlobalObjectDatabase::Register((AnyObject*) this);
    
  log_trace("ParticipantImpl::ParticipantImpl - Leaving method");
    
  return;

}

/* Destructor method */

ParticipantImpl::~ParticipantImpl (void) 
{ 

  log_trace("ParticipantImpl::~ParticipantImpl - Entering method");
    
  /* Release resources */
  if (this->m_disc != NULL) delete this->m_disc; this->m_disc = NULL; 

  /* Remove from the object database */ 
  //ccs::base::GlobalObjectDatabase::Remove((AnyObject*) this); 

  log_trace("ParticipantImpl::~ParticipantImpl - Leaving method");
    
  return;

}

/* Display methods */

} /* namespace sdn */

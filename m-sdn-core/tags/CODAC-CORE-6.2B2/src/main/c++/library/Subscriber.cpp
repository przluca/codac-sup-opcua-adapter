/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/Subscriber.cpp $
* $Id: Subscriber.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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

/* Done - Implement Receive with timeout
	
   ToDo - Implement SubscriberThread with spinlock in lieu of blocking reception 
        - Implement packet and latency statistics computation after callback 
 */


/* Global header files */

#include <new> /* std::nothrow */

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */
#include "sdn-tools.h" /* Misc. helper functions, e.g. hash, etc. */

#include "sdn-base.h" /* Privately scoped base classes definition */

#include "sdn-header.h"
#include "sdn-footer.h"
#include "sdn-topic.h"
#include "sdn-packet.h"

#include "core-participant.h"
#include "core-subscriber.h"

#include "disc-messenger.h"

#include "ParticipantImpl.h"

#include "Subscriber.h" /* Interface class definition */
#include "SubscriberImpl.h" /* This class definition */

/* Constants */

#define OBJTYPE_SDN_SUBSCRIBER (char*) "sdn::Subscriber_Base"

#define DEFAULT_SUBSCRIBER_TIMEOUT 1000000000 /* 1s */
#define DEFAULT_STATISTICS_WINDOW  1024

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::sub"

/* Type definition */

namespace sdn {

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initialiser methods */

void SubscriberImpl::Initialise (void) 
{ 

  log_trace("SubscriberImpl::Initialise - Entering method");

  /* Initialise attributes */
  this->m_base = static_cast<core::ParticipantImpl*>(NULL); 

  /* Acquire resources */
  log_debug("SubscriberImpl::Initialise - Creating core::Subscriber instance"); 
  this->m_base = dynamic_cast<core::ParticipantImpl*>(new (std::nothrow) core::SubscriberImpl ());

  log_trace("SubscriberImpl::Initialise - Leaving method");

  return; 

}

/* Accessor methods */

bool SubscriberIface::IsInitialized (void) const { return (this->p_impl)->IsInitialized(); }
bool SubscriberIface::IsPayloadOrdered (void) const { bool ret = true; if (this->m_header != NULL) ret = (this->m_header)->IsPayloadOrdered(); return ret; } /* Bug 10327 - Same byte ordering as host */

const char* SubscriberIface::GetInterface (void) const { return (this->p_impl)->GetInterface(); }
const char* SubscriberIface::GetTopicName (void) const { return (this->p_impl)->GetTopicName(); }
uint_t SubscriberIface::GetTopicSize (void) const { return (this->p_impl)->GetTopicSize(); }
    
void* SubscriberIface::GetTopicHeader (void) const { return (this->p_impl)->GetTopicHeader(); }
void* SubscriberIface::GetTopicFooter (void) const { return (this->p_impl)->GetTopicFooter(); }
void* SubscriberIface::GetTopicInstance (void) const { return (this->p_impl)->GetTopicInstance(); }

RET_STATUS SubscriberIface::SetInterface (const char* name) { return (this->p_impl)->SetInterface(name); }
// Deleted - RET_STATUS SubscriberIface::SetTopicName (const char* name) { return (this->p_impl)->SetTopicName(name); }
RET_STATUS SubscriberIface::SetMetadata (Metadata_t& mdata) { return (this->p_impl)->SetMetadata(mdata); }
RET_STATUS SubscriberIface::SetTopic (Topic& topic) { return (this->p_impl)->SetTopic(topic); }
RET_STATUS SubscriberIface::SetBufferDepth (uint_t depth) { return (this->p_impl)->SetBufferDepth(depth); }

RET_STATUS SubscriberIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb); }
RET_STATUS SubscriberIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); }

int SubscriberIface::GetSocket (void) { return (this->p_impl)->GetSocket(); }

/* Miscellaneous methods */

RET_STATUS SubscriberIface::Configure (void)
{ 

  RET_STATUS status = STATUS_ERROR; 

  status = (this->p_impl)->Configure();

  /* For backward compatibility purposes with v1.0 */
  this->m_topic = ((this->p_impl)->m_base)->m_topic;
  this->m_packet = ((this->p_impl)->m_base)->m_packet; 
  if (this->m_packet != NULL) this->m_header = (this->m_packet)->GetHeader();
  if (this->m_packet != NULL) this->m_footer = (this->m_packet)->GetFooter();

  return status; 

}

RET_STATUS SubscriberIface::CopyTopicInstance (void* instance, uint_t size) { return (this->p_impl)->CopyTopicInstance(instance, size); }
RET_STATUS SubscriberIface::DoBackgroundActivity (void) { return (this->p_impl)->DoBackgroundActivity(); }
RET_STATUS SubscriberIface::Receive (uint64_t timeout) { return (this->p_impl)->Receive(timeout); }
RET_STATUS SubscriberIface::Receive (void) { return (this->p_impl)->Receive(); }
    
RET_STATUS SubscriberImpl::Configure (void)
{ 

  log_trace("SubscriberImpl::Configure - Entering method");

  RET_STATUS status = STATUS_ERROR;

  /* Configure base class */
  if (Participant_Configure(dynamic_cast<ParticipantImpl*>(this)) != STATUS_SUCCESS)
    {
      log_error("SubscriberImpl::Configure - Failed to configure participant"); 
      return status;
    }
  
  log_debug("SubscriberImpl::Configure - Configure core::Subscriber instance"); 
  if ((this->m_base)->Configure() != STATUS_SUCCESS)
    {
      log_error("SubscriberImpl::Configure - Failed to configure core::Subscriber instance"); 
      return status;
    }

  log_debug("SubscriberImpl::Configure - Notify disc::Messenger instance");
  (this->m_disc)->Register(this->m_base);

  status = STATUS_SUCCESS; 
  
  return status; 

}

RET_STATUS SubscriberImpl::Receive (uint64_t timeout)
{ 

  log_trace("SubscriberImpl::Receive - Entering method");

  RET_STATUS status = STATUS_ERROR; 

  status = dynamic_cast<core::SubscriberImpl*>(this->m_base)->Receive(timeout);

  log_trace("SubscriberImpl::Receive - Leaving method");

  return status; 

}

RET_STATUS SubscriberImpl::Receive (void) 
{ 

  log_trace("SubscriberImpl::Receive - Entering method");

  RET_STATUS status = STATUS_ERROR; 

  status = dynamic_cast<core::SubscriberImpl*>(this->m_base)->Receive();
      
  log_trace("SubscriberImpl::Receive - Leaving method");

  return status; 

}

/* Constructor methods */

SubscriberIface::SubscriberIface (void)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  /* Initialise attributes */
  this->p_impl = new (std::nothrow) SubscriberImpl ();

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}

SubscriberIface::SubscriberIface (Metadata_t& mdata)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  /* Initialise attributes */
  this->p_impl = static_cast<SubscriberImpl*>(NULL);

  bool ok = (static_cast<SubscriberImpl*>(NULL) != 
	     (this->p_impl = new (std::nothrow) SubscriberImpl ()));

  if (ok)
    {
      //ok = (this->SetMetadata(mdata) == STATUS_SUCCESS);
      this->SetMetadata(mdata);
    }

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}

SubscriberIface::SubscriberIface (Topic& topic)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  /* Initialise attributes */
  this->p_impl = static_cast<SubscriberImpl*>(NULL);

  bool ok = (static_cast<SubscriberImpl*>(NULL) != 
	     (this->p_impl = new (std::nothrow) SubscriberImpl ()));

  if (ok)
    {
      //ok = (this->SetTopic(topic) == STATUS_SUCCESS);
      this->SetTopic(topic);
    }

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}

SubscriberImpl::SubscriberImpl (void) 
{ 

  log_trace("SubscriberImpl::SubscriberImpl - Entering method"); 

  /* Initialise resources */ 
  this->Initialise(); 

  log_trace("SubscriberImpl::SubscriberImpl - Leaving method"); 

  return; 

}

SubscriberImpl::SubscriberImpl (Metadata_t& mdata) 
{ 

  log_trace("SubscriberImpl::SubscriberImpl - Entering method"); 

  /* Initialise resources */ 
  this->Initialise(); 
  this->SetMetadata(mdata);

  log_trace("SubscriberImpl::SubscriberImpl - Leaving method"); 

  return; 

}

SubscriberImpl::SubscriberImpl (Topic& topic) 
{ 

  log_trace("SubscriberImpl::SubscriberImpl - Entering method"); 

  /* Initialise resources */ 
  this->Initialise(); 
  this->SetTopic(topic);

  log_trace("SubscriberImpl::SubscriberImpl - Leaving method"); 

  return; 

}

/* Destructor method */

SubscriberIface::~SubscriberIface (void)
{
        
  log_trace("SubscriberIface::~SubscriberIface - Entering method");
        
  /* Release resources */
  if (static_cast<SubscriberImpl*>(NULL) != this->p_impl) 
    {
      delete this->p_impl; this->p_impl= static_cast<SubscriberImpl*>(NULL);
    }
        
  log_trace("SubscriberIface::~SubscriberIface - Leaving method");
        
  return;
        
}

SubscriberImpl::~SubscriberImpl (void) 
{ 

  /* Release resources */
  if (this->m_disc != NULL) (this->m_disc)->Remove(this->m_base); 

  if (this->m_base != NULL) delete dynamic_cast<core::SubscriberImpl*>(this->m_base); this->m_base = NULL; 

  return; 

}

} /* namespace sdn */

#undef LOG_ALTERN_SRC

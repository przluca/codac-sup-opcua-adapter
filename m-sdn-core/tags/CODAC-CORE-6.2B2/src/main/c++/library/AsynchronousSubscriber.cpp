/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/AsynchronousSubscriber.cpp $
* $Id: AsynchronousSubscriber.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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
#if __cplusplus > 199711L
#include <functional> /* std::function, etc. */
#endif
/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */
#include "sdn-tools.h" /* Misc. helper functions, e.g. hash, etc. */

#include "sdn-base.h" /* Privately scoped base classes definition */
#include "sdn-api.h" /* SDN core library - API definition */

#include "sdn-header.h"
#include "sdn-footer.h"
#include "sdn-topic.h"
#include "sdn-packet.h"

#include "core-participant.h"
#include "core-subscriber.h"

#include "disc-messenger.h"

#include "ParticipantImpl.h"
#include "SubscriberImpl.h"
#if __cplusplus > 197711L
#else
#include "AsynchronousSubscriber.h"
#endif
#include "AsynchronousSubscriberImpl.h" /* This class definition */

/* Constants */

#define OBJTYPE_SDN_SUBSCRIBER (char*) "sdn::Subscriber_Base"

#define DEFAULT_SUBSCRIBER_TIMEOUT 1000000000ul /* 1s */
#define DEFAULT_STATISTICS_WINDOW  1024

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::sub"

/* Type definition */

namespace sdn {

/* Global variables */

/* Function declaration */

/* Function definition */

static void AsynchronousSubscriberImpl_Preamble (AsynchronousSubscriberImpl* self) { if (self != NULL) self->Preamble(); return; }
static void AsynchronousSubscriberImpl_CB (AsynchronousSubscriberImpl* self) { if (self != NULL) self->Do(); return; }

/* Initializer methods */

void AsynchronousSubscriberImpl::Initialize (void)
{

  log_trace("AsynchronousSubscriberImpl::Initialize - Entering method");

  /* Create thread instance */
  char thread_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  snprintf(thread_name, STRING_MAX_LENGTH, "sdn::Subscriber(%s)", this->GetTopicName());

  this->m_thread = new base::AnyThread ((char*) thread_name);
  (this->m_thread)->SetPreamble((void (*) (void*)) &AsynchronousSubscriberImpl_Preamble, (void*) this); 
  (this->m_thread)->SetCallback((void (*) (void*)) &AsynchronousSubscriberImpl_CB, (void*) this);
  (this->m_thread)->SetPeriod(0L); /* Blocking callback */

  this->m_timeout = DEFAULT_SUBSCRIBER_TIMEOUT;

  this->m_stats = new base::Statistics<uint64_t> (DEFAULT_STATISTICS_WINDOW); (this->m_stats)->Reset();
#if __cplusplus > 199711L
  this->m_recv_cb = NULL;
#else
  this->m_iface = static_cast<AsynchronousSubscriberIface*>(NULL);
#endif
  log_trace("AsynchronousSubscriberImpl::Initialize - Leaving method");

  return; 

}

/* Accessor methods */

const char* AsynchronousSubscriberIface::GetInterface (void) const { return (this->p_impl)->GetInterface(); }
const char* AsynchronousSubscriberIface::GetTopicName (void) const { return (this->p_impl)->GetTopicName(); }
uint_t AsynchronousSubscriberIface::GetTopicSize (void) const { return (this->p_impl)->GetTopicSize(); }
    
const void* AsynchronousSubscriberIface::GetTopicHeader (void) const { return (this->p_impl)->GetTopicHeader(); }
const void* AsynchronousSubscriberIface::GetTopicFooter (void) const { return (this->p_impl)->GetTopicFooter(); }
const void* AsynchronousSubscriberIface::GetTopicInstance (void) const { return (this->p_impl)->GetTopicInstance(); }
    
RET_STATUS AsynchronousSubscriberIface::SetAffinity (uint_t core) { return (this->p_impl)->SetAffinity(core); }
RET_STATUS AsynchronousSubscriberIface::SetPriority (uint_t policy, uint_t priority) { return (this->p_impl)->SetPriority(policy, priority); }
RET_STATUS AsynchronousSubscriberIface::SetTimeout (uint64_t timeout) { return (this->p_impl)->SetTimeout(timeout); }

RET_STATUS AsynchronousSubscriberIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb, static_cast<void*>(this)); }
RET_STATUS AsynchronousSubscriberIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); }

RET_STATUS AsynchronousSubscriberIface::SetInterface (const char* name) { return (this->p_impl)->SetInterface(name); }

/* Miscellaneous methods */

RET_STATUS AsynchronousSubscriberIface::Configure (void) 
{ 

  log_trace("AsynchronousSubscriberIface::Configure - Entering method");

  RET_STATUS status = STATUS_ERROR;

  if ((this->p_impl)->Configure() != STATUS_SUCCESS) 
    {
      log_error("AsynchronousSubscriberIface::Configure - Failed to configure participant"); 
      return status;
    }
#if __cplusplus > 199711L
  if ((this->p_impl)->RegisterHandler(std::bind(&AsynchronousSubscriberIface::HandleReceive, this)) != STATUS_SUCCESS) 
#else
  if ((this->p_impl)->RegisterHandler(this) != STATUS_SUCCESS) 
#endif
    {
      log_error("AsynchronousSubscriberIface::Configure - Failed to associate handler"); 
      return status;
    }

  if ((this->p_impl)->Launch() != STATUS_SUCCESS) 
    {
      log_error("AsynchronousSubscriberIface::Configure - Failed to launch asynchronous thread"); 
      return status;
    }

  status = STATUS_SUCCESS; 
  
  log_trace("AsynchronousSubscriberIface::Configure - Leaving method");

  return status; 

}

RET_STATUS AsynchronousSubscriberIface::CopyTopicInstance (void* instance) const { return (this->p_impl)->CopyTopicInstance(instance, this->GetTopicSize()); }
RET_STATUS AsynchronousSubscriberIface::CopyTopicInstance (void* instance, uint_t size) const { return (this->p_impl)->CopyTopicInstance(instance, size); }

RET_STATUS AsynchronousSubscriberImpl::Do (void)
{

  log_trace("AsynchronousSubscriberImpl::Do - Entering method");

  RET_STATUS status = STATUS_ERROR;

  /* ToDo - Spinlock mode when knowing the expected period */

  if (this->GetTopicInstance() == NULL)
    {
      log_debug("AsynchronousSubscriberImpl::Do - Skip execution - No topic instance defined");
      wait(DEFAULT_SUBSCRIBER_TIMEOUT);
    }
  else if (this->Receive(this->m_timeout) != STATUS_SUCCESS) /* Blocking receive with timeout */
    {
      if (this->m_timeout > 0ul) log_warning("AsynchronousSubscriberImpl::Do - Subscriber::Receive failed");
    }
  else 
    {
      log_debug("AsynchronousSubscriberImpl::Do - Subscriber::Receive successful at '%lu'", get_time());
#if __cplusplus > 199711L
      /* Callback */
      (this->m_recv_cb)();
#else
      if (static_cast<AsynchronousSubscriberIface*>(NULL) != this->m_iface) (this->m_iface)->HandleReceive();
#endif
      /* Compute statistics */
      Header_t* p_header = (Header_t*) this->GetTopicHeader();

      if (p_header->recv_time > p_header->send_time)
	{
	  uint64_t latency = p_header->recv_time - p_header->send_time;

	  if ((this->m_stats)->PushSample(latency) == true)
	    {
	      log_debug("AsynchronousSubscriberImpl::Do - Latency statistics over '%u' samples are '%lu %lu %lu %lu' [nsec]", (this->m_stats)->GetSize(), (this->m_stats)->GetAvg(), (this->m_stats)->GetStd(), (this->m_stats)->GetMin(), (this->m_stats)->GetMax());
	      (this->m_stats)->Reset();
	    }
	}

      status = STATUS_SUCCESS;
    }

  /* Do background activity ... Discovery housekeeping */
  this->DoBackgroundActivity();

  log_trace("AsynchronousSubscriberImpl::Do - Leaving method");

  return status; 

}

/* Constructor methods */

AsynchronousSubscriberIface::AsynchronousSubscriberIface (Metadata_t& mdata)
{

  log_trace("AsynchronousSubscriberIface::AsynchronousSubscriberIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new AsynchronousSubscriberImpl (mdata);

  log_trace("AsynchronousSubscriberIface::AsynchronousSubscriberIface - Leaving method"); 

  return;

}

AsynchronousSubscriberIface::AsynchronousSubscriberIface (Topic& topic)
{

  log_trace("AsynchronousSubscriberIface::AsynchronousSubscriberIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new AsynchronousSubscriberImpl (topic);

  log_trace("AsynchronousSubscriberIface::AsynchronousSubscriberIface - Leaving method"); 

  return;

}

/* Destructor method */

AsynchronousSubscriberIface::~AsynchronousSubscriberIface (void)
{
        
  log_trace("AsynchronousSubscriberIface::~AsynchronousSubscriberIface - Entering method");
        
  /* Release resources */
  if (this->p_impl != NULL) delete this->p_impl; this->p_impl= NULL;
        
  log_trace("AsynchronousSubscriberIface::~AsynchronousSubscriberIface - Leaving method");
        
  return;
        
}

} /* namespace sdn */

#undef LOG_ALTERN_SRC

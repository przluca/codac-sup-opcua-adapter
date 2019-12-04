/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/core-subscriber.cpp $
* $Id: core-subscriber.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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

/* 
   Done - Implement Receive with timeout
	- Implement packet and latency statistics computation after callback 
   ToDo - Implement SubscriberThread with spinlock in lieu of blocking reception 
 */

/* Global header files */

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */
#include "sdn-tools.h" /* Misc. helper functions, e.g. hash, etc. */

#include "sdn-base.h" /* Privately scoped base classes definition */
#include "sdn-mcast.h" /* SDN core library - API definition (sdn::mcast) */
#include "sdn-ucast.h" /* SDN core library - API definition (sdn::ucast) */
#include "sdn-core.h" /* SDN core library - API definition (sdn::core) */

#include "sdn-packet.h"
#include "sdn-header.h"
#include "sdn-topic.h"

#include "core-participant.h"
#include "core-subscriber.h" /* This class definition */

/* Constants */

#define DEFAULT_SUBSCRIBER_TIMEOUT 1000000000ul /* 1s */
#define DEFAULT_STATISTICS_WINDOW  1024

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::core"

/* Type definition */

namespace sdn {

namespace core {

/* Global variables */

/* Function declaration */

/* Function definition */

static void Subscriber_Asyn_Preamble (Subscriber_Asyn* self) { if (self != NULL) self->Preamble(); return; }
static void Subscriber_Asyn_CB (Subscriber_Asyn* self) { if (self != NULL) self->Do(); return; }

/* Initializer methods */

void SubscriberImpl::Initialize (void) 
{ 

  log_trace("SubscriberImpl::Initialize - Entering method");

  /* Initialize attributes */
  this->m_base = NULL; 
  this->m_counter = 0L; 

  log_trace("SubscriberImpl::Initialize - Leaving method");

  return; 

}

void Subscriber_Asyn::Initialize (void)
{

  log_trace("Subscriber_Asyn::Initialize - Entering method");

  /* Create thread instance */
  char thread_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  snprintf(thread_name, STRING_MAX_LENGTH, "Subscriber_Asyn_%s", this->GetTopicName());

  this->m_thread = new (std::nothrow) base::AnyThread ((char*) thread_name);

  if (this->m_thread != static_cast<base::AnyThread*>(NULL))
    {
      (this->m_thread)->SetPreamble((void (*) (void*)) &Subscriber_Asyn_Preamble, (void*) this); 
      (this->m_thread)->SetCallback((void (*) (void*)) &Subscriber_Asyn_CB, (void*) this);
      (this->m_thread)->SetPeriod(0ul); /* Blocking callback */
    }

  this->m_stats = new base::Statistics<uint64_t> (DEFAULT_STATISTICS_WINDOW); (this->m_stats)->Reset();

  this->m_cb = NULL; this->m_attr = NULL;

  log_trace("Subscriber_Asyn::Initialize - Leaving method");

  return; 

}

RET_STATUS Subscriber_Asyn::Launch (void) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->Launch(); return status; }
RET_STATUS Subscriber_Asyn::Terminate (void) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->Terminate(); return status; }

/* Accessor methods */

bool SubscriberIface::IsInitialized (void) { return (this->p_impl)->IsInitialized(); }
    
char* SubscriberIface::GetInterface (void) { return (this->p_impl)->GetInterface(); }
char* SubscriberIface::GetTopicName (void) { return (this->p_impl)->GetTopicName(); }
uint_t SubscriberIface::GetTopicSize (void) { return (this->p_impl)->GetTopicSize(); }
    
RET_STATUS SubscriberIface::SetInterface (char* name) { return (this->p_impl)->SetInterface(name); }
RET_STATUS SubscriberIface::SetMetadata (Metadata_t& mdata) { return (this->p_impl)->SetMetadata(mdata); }
    
RET_STATUS SubscriberIface::CopyTopicInstance (void* instance, uint_t size) { return (this->p_impl)->CopyTopicInstance(instance, size); }
void* SubscriberIface::GetTopicHeader (void) { return (this->p_impl)->GetTopicHeader(); }
void* SubscriberIface::GetTopicFooter (void) { return (this->p_impl)->GetTopicFooter(); }
void* SubscriberIface::GetTopicInstance (void) { return (this->p_impl)->GetTopicInstance(); }
    
RET_STATUS SubscriberIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb); }
RET_STATUS SubscriberIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); }

int SubscriberIface::GetSocket (void) const { return (this->p_impl)->GetSocket(); }

RET_STATUS Subscriber_Asyn::SetAffinity (uint_t core) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->SetAffinity(core); return status; }
RET_STATUS Subscriber_Asyn::SetPriority (uint_t policy, uint_t priority) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->SetPriority(policy, priority); return status; }

/* Miscellaneous methods */

RET_STATUS SubscriberIface::Configure (void)
{ 

  RET_STATUS status = STATUS_ERROR; 

  status = (this->p_impl)->Configure();

  /* For backward compatibility purposes with v1.0 */
  this->m_topic = (this->p_impl)->m_topic;
  this->m_packet = (this->p_impl)->m_packet; 
  if (this->m_packet != NULL) this->m_header = (this->m_packet)->GetHeader(); else log_warning("SubscriberIface::Configure - Packet instance is 'NULL'");
  if (this->m_packet != NULL) this->m_footer = (this->m_packet)->GetFooter(); else log_warning("SubscriberIface::Configure - Packet instance is 'NULL'");

  return status; 

}

RET_STATUS SubscriberIface::Receive (uint64_t timeout) { return (this->p_impl)->Receive(timeout); }
RET_STATUS SubscriberIface::Receive (void) { return (this->p_impl)->Receive(); }
    
RET_STATUS SubscriberImpl::Configure (void) 
{

  log_trace("SubscriberImpl::Configure - Entering method");

  RET_STATUS status = STATUS_ERROR;

  /* Configure base class */
  if (Participant_Configure((ParticipantImpl*) this) != STATUS_SUCCESS)
    {
      log_error("SubscriberImpl::Configure - Failed to configure participant"); 
      return status;
    }
  
  /* Bug 7919 - Return error in case interface is invalid */
  if (HelperTools::IsInterfaceValid(this->GetInterface()) != true) 
    { 
      log_error("SubscriberImpl::Configure - Currently defined interface '%s' is not valid", this->GetInterface()); 
      return status; 
    }

  /* WARNING - This code does not allow for changing between MCAST and UCAST participants at runtime */

  /* Create generic subscriber, if necessary */
  if ((this->m_base != NULL) && 
      ((strcmp((this->m_base)->GetAddress(), (this->m_topic)->GetMCastGroup()) != 0) ||
       ((this->m_base)->GetPort() != (this->m_topic)->GetMCastPort()) ||
       ((this->m_base)->GetSize() != (this->m_packet)->GetSize())))
    {
      log_debug("SubscriberImpl::Configure - Closing base::Subscriber instance");
      (this->m_base)->Close();
    }

  if (this->m_base == NULL) 
    { 
      if (sdn::HelperTools::IsMCastAddress((this->m_topic)->GetMCastGroup()) == true) { log_debug("Subscriber::Configure - Creating mcast::Subscriber instance"); this->m_base = (base::SubscriberIface*) new mcast::Subscriber (); } 
      if (sdn::HelperTools::IsUCastAddress((this->m_topic)->GetMCastGroup()) == true) { log_debug("Subscriber::Configure - Creating ucast::Subscriber instance"); this->m_base = (base::SubscriberIface*) new ucast::Subscriber (); } 
    }

  log_debug("SubscriberImpl::Configure - Configuring base::Subscriber instance");
  (this->m_base)->SetInterface(this->GetInterface()); 

  (this->m_base)->SetAddress((this->m_topic)->GetMCastGroup()); 
  (this->m_base)->SetPort((this->m_topic)->GetMCastPort()); 

  if (0u != this->m_depth) (this->m_base)->SetBufferDepth(this->m_depth * ((this->m_packet)->GetSize() + 68u));

  status = (this->m_base)->Open();

  log_debug("SubscriberImpl::Configure - Defining base::Subscriber buffer and size");
  (this->m_base)->SetBuffer((this->m_packet)->GetInstance(), (this->m_packet)->GetSize());
  //(this->m_base)->SetTimeout(DEFAULT_SUBSCRIBER_TIMEOUT);

  this->m_counter = 0ul;
  this->m_source = 0;

  log_trace("SubscriberImpl::Configure - Leaving method");

  return status; 

}

bool SubscriberImpl::IsValid (void) 
{ 

  log_trace("SubscriberImpl::IsValid - Entering method");

  bool valid = false; 

  if (((this->m_packet)->GetHeader())->IsValid() != true)
    {
      log_warning("SubscriberImpl::IsValid - Unexpected packet received - Invalid packet.header");
      return valid;
    }

  if (((this->m_packet)->GetHeader())->IsPayloadOrdered() != true)
    {
      log_debug("SubscriberImpl::IsValid - Payload byte order mismatch - Do nothing at library level");
    }
  else
    {
      Header_t* p_header = ((this->m_packet)->GetHeader())->GetInstance();

      /* Initialize counter, if necessary */
      if ((this->m_counter == 0L) || (p_header->topic_counter == 0L)) /* Newly started subscriber or publisher */
	{
	  this->m_counter = p_header->topic_counter;
	  log_info("SubscriberImpl::IsValid - Initializing (or resetting) packet counter at '%lu'", this->m_counter);
#ifdef USE_PACKET_FOOTER
	  if (((this->m_packet)->GetFooter())->IsValid() == true) this->m_source = ((this->m_packet)->GetFooter())->GetSourceUID();
	  log_info("SubscriberImpl::IsValid - Initializing (or resetting) topic source UID to '%u'", this->m_source);
#endif
	}
      
      /* INFO - This should be enough to detect size/type mismatch */
      
      /* Check topic definition - Topic UID (may be disabled with explicit 0) */
      if ((p_header->topic_uid != 0) && ((this->m_topic)->GetUID() != 0) && (p_header->topic_uid != (this->m_topic)->GetUID()))
	{
	  /* Unexpected topic */
	  log_warning("SubscriberImpl::IsValid - Unexpected topic received - Expected '%u' while packet.header.topic_uid is '%u'", (this->m_topic)->GetUID(), p_header->topic_uid);
	  return valid;
	}
      
      /* Check topic definition - Topic version (may be disabled with explicit 0) */
      if ((p_header->topic_version != 0) && ((this->m_topic)->GetVersion() != 0) && (p_header->topic_version != (this->m_topic)->GetVersion()))
	{
	  /* Unexpected topic version */
	  log_warning("SubscriberImpl::IsValid - Unexpected topic received - Expected '%u' while packet.header.topic_version is '%u'", (this->m_topic)->GetVersion(), p_header->topic_version);
	  return valid;
	}
      
      /* Check topic definition - Topic size */
      if ((p_header->topic_size != 0) && (p_header->topic_size != (this->m_topic)->GetSize()))
	{
	  /* Unexpected topic size */
	  log_warning("SubscriberImpl::IsValid - Unexpected topic received - Expected '%u' while packet.header.topic_size is '%u'", (this->m_topic)->GetSize(), p_header->topic_size);
	  return valid;
	}
#if 0
      /* Check latency */
      if (p_header->recv_time < p_header->send_time)
	{
	  log_warning("SubscriberImpl::IsValid - Mis-synchronized participants clocks");
	}
      else if ((p_header->recv_time - p_header->send_time) > 100000ul) /* Latency above 100us */
	{
	  log_warning("SubscriberImpl::IsValid - Late packet '%lu' with '%lu'", p_header->topic_counter, (p_header->recv_time - p_header->send_time));
	}
      else if ((p_header->recv_time - p_header->send_time) > 50000ul) /* Latency above 50us */
	{
	  log_notice("SubscriberImpl::IsValid - Late packet '%lu' with '%lu'", p_header->topic_counter, (p_header->recv_time - p_header->send_time));
	} 
#endif
      /* Check instance counter */
      if (p_header->topic_counter + 1ul == this->m_counter) /* Bug 9800 - Detect and drop replicated messages */
	{
	  log_warning("SubscriberImpl::IsValid - Packet replication detected");
	  return valid;
	} 
      else if (p_header->topic_counter < this->m_counter)
	{
	  log_warning("SubscriberImpl::IsValid - Probably having multiple publishers on the same topic");
	} 
      else if (this->m_counter != p_header->topic_counter)
	{
	  log_warning("SubscriberImpl::IsValid - Missing '%lu' topic instances", p_header->topic_counter - this->m_counter);
	}
      
      this->m_counter = p_header->topic_counter + 1ul;
#ifdef USE_PACKET_FOOTER
      if (((this->m_packet)->GetFooter())->IsValid() == true)
	{
	  SDNFooter_t* p_footer = ((this->m_packet)->GetFooter())->GetInstance();
	  
	  if ((p_footer->source_uid != 0) && (p_footer->source_uid != this->m_source)) 
	    {
	      log_warning("SubscriberImpl::IsValid - Unexpected topic received - Expected '%u' while packet.footer.source_uid is '%u'", this->m_source, p_footer->source_uid);
	      return valid;
	    }
#if 0 /* Do not trust the topic instance is not being changed externally to e.g. serialize past instances */
	  if ((p_footer->topic_crc != 0) && (p_footer->topic_crc != (this->m_topic)->GetInstanceChecksum())) /* Memory corruption */
#else
	    (this->m_topic)->SetInstance((this->m_packet)->GetPayload());
	  
	  if ((p_footer->topic_crc != 0) && (p_footer->topic_crc != (this->m_topic)->GetInstanceChecksum())) /* Memory corruption */
#endif
	    {
	      log_warning("SubscriberImpl::IsValid - Unexpected topic received - Expected '%u' while packet.footer.topic_crc is '%u'", (this->m_topic)->GetInstanceChecksum(), p_footer->topic_crc);
	      return valid;
	    }
	}
      else
	{
	  log_warning("SubscriberImpl::IsValid - Unexpected packet received - Invalid packet.footer");
	}
#endif
    }

  valid = true;

  log_trace("SubscriberImpl::IsValid - Leaving method");

  return valid; 

}

RET_STATUS SubscriberImpl::Receive (uint64_t timeout) 
{ 

  log_trace("SubscriberImpl::Receive - Entering method");

  RET_STATUS status = STATUS_ERROR; 

  if (this->m_topic == NULL)
    {
      log_error("PublisherImpl::Publish - Un-initialized topic");
      return status;
    }
  else if ((this->m_topic)->IsInitialized() != true) /* Incomplete metadata: name only must be supported by XML topic definition file, else metadata must be complete enough */
    {
      log_error("SubscriberImpl::Receive - Un-initialized topic '%s'", (this->m_topic)->GetName());
      return status;
    }

  /* Bug 7919 - mcast::Subscriber is not initialized in case of wrong interface */
  if ((this->m_base != NULL) && ((this->m_base)->Receive(timeout) == STATUS_SUCCESS))
    {
#if 0 /* Revert on Bug 10327 */
      /* Bug 10327 - Enfore network byte ordering of the header/footer */
      ((this->m_packet)->GetHeader())->FromNetworkByteOrder(); 
#ifdef USE_PACKET_FOOTER
      ((this->m_packet)->GetFooter())->FromNetworkByteOrder(); 
#endif
#endif
      /* Set receive timestamp */
      ((this->m_packet)->GetHeader())->SetReceiveTimestamp(); 

      /* Check received packet */
      if (this->IsValid() != true)
	{
	  /* Invalid packet */
	  log_warning("SubscriberImpl::Receive - Invalid packet received");
	  return status;
	}

      /* Invoke callback, if any */
      if (this->m_cb != NULL) { log_trace("SubscriberImpl::Receive - Invoke callback"); (*(this->m_cb))(this->m_attr); }

      status = STATUS_SUCCESS;
      
    }

  log_trace("SubscriberImpl::Receive - Leaving method");

  return status; 

}

RET_STATUS SubscriberImpl::Receive (void) 
{ 

  log_trace("SubscriberImpl::Receive - Entering method");

  RET_STATUS status = STATUS_ERROR; 

  if (this->m_topic == NULL)
    {
      log_error("PublisherImpl::Publish - Un-initialized topic");
      return status;
    }
  else if ((this->m_topic)->IsInitialized() != true) /* Incomplete metadata: name only must be supported by XML topic definition file, else metadata must be complete enough */
    {
      log_error("SubscriberImpl::Receive - Un-initialized topic '%s'", (this->m_topic)->GetName());
      return status;
    }

  /* Bug 7919 - mcast::Subscriber is not initialized in case of wrong interface */
  if ((this->m_base != NULL) && ((this->m_base)->Receive() == STATUS_SUCCESS))
    {
#if 0 /* Revert on Bug 10327 */
      /* Bug 10327 - Enfore network byte ordering of the header/footer */
      ((this->m_packet)->GetHeader())->FromNetworkByteOrder(); 
#ifdef USE_PACKET_FOOTER
      ((this->m_packet)->GetFooter())->FromNetworkByteOrder(); 
#endif
#endif
      /* Set receive timestamp */
      ((this->m_packet)->GetHeader())->SetReceiveTimestamp(); 

      /* Check received packet */
      if (this->IsValid() != true)
	{
	  /* Invalid packet */
	  log_warning("SubscriberImpl::Receive - Invalid packet received");
	  return status;
	}

      /* Invoke callback, if any */
      if (this->m_cb != NULL) { log_trace("SubscriberImpl::Receive - Invoke callback"); (*(this->m_cb))(this->m_attr); }

      status = STATUS_SUCCESS;
      
    }

  log_trace("SubscriberImpl::Receive - Leaving method");

  return status; 

}

RET_STATUS Subscriber_Asyn::Do (void)
{

  log_trace("Subscriber_Asyn::Do - Entering method");

  RET_STATUS status = STATUS_ERROR;

  /* ToDo - Spinlock mode when knowing the expected period */

  if (this->GetTopicInstance() == NULL)
    {
      log_debug("Subscriber_Asyn::Do - Skip execution - No topic instance defined");
      wait(DEFAULT_SUBSCRIBER_TIMEOUT);
    }
  else if (this->Receive(DEFAULT_SUBSCRIBER_TIMEOUT) != STATUS_SUCCESS) /* Blocking receive with timeout */
    {
      log_warning("Subscriber_Asyn::Do - Subscriber::Receive failed");
    }
  else 
    {
      log_debug("Subscriber_Asyn::Do - Subscriber::Receive successful at '%lu'", get_time());

      /* Callback, if any */
      if (this->m_cb != NULL) (*(this->m_cb))(this->m_attr);

      /* Compute statistics */
      Header_t* p_header = ((this->m_packet)->GetHeader())->GetInstance();

      if (p_header->recv_time > p_header->send_time)
	{
	  uint64_t latency = p_header->recv_time - p_header->send_time;

	  if ((this->m_stats)->PushSample(latency) == true)
	    {
	      log_debug("Subscriber_Asyn::Do - Latency statistics over '%u' samples are '%lu %lu %lu %lu' [nsec]", (this->m_stats)->GetSize(), (this->m_stats)->GetAvg(), (this->m_stats)->GetStd(), (this->m_stats)->GetMin(), (this->m_stats)->GetMax());
	      (this->m_stats)->Reset();
	    }
	}

      status = STATUS_SUCCESS;
    }

  log_trace("Subscriber_Asyn::Do - Leaving method");

  return status; 

}

/* Constructor methods */

SubscriberIface::SubscriberIface (void)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new SubscriberImpl ();

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}

SubscriberIface::SubscriberIface (Metadata_t& mdata)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new SubscriberImpl (mdata);

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}
#if 0
SubscriberIface::SubscriberIface (char* name)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new SubscriberImpl (name);

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}
#endif
SubscriberIface::SubscriberIface (Topic& topic)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new SubscriberImpl (topic);

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}

Subscriber_Base::Subscriber_Base (void) { this->SetInstanceType(OBJTYPE_CORE_SUBSCRIBER); /* Initialize resources */ this->Initialize(); return; }

Subscriber_Asyn::Subscriber_Asyn (Topic& topic) : SubscriberImpl(topic) 
{ 

  log_trace("Subscriber_Asyn::Subscriber_Asyn - Entering method"); 

  /* Initialize resources */ 
  this->Initialize(); 

  log_trace("Subscriber_Asyn::Subscriber_Asyn - Leaving method"); 

  return; 

}

/* Destructor method */

SubscriberIface::~SubscriberIface (void)
{
        
  log_trace("SubscriberIface::~SubscriberIface - Entering method");
        
  /* Release resources */
  if (this->p_impl != NULL) 
    {
      delete this->p_impl; this->p_impl = NULL;
    }
        
  log_trace("SubscriberIface::~SubscriberIface - Leaving method");
        
  return;
        
}

SubscriberImpl::~SubscriberImpl (void) 
{ 

  /* Release resources */
  if (this->m_base != NULL)
    {
      /* ToDo - Call appropriate destructor */
      delete this->m_base; this->m_base = NULL; 
    }

  return; 

}

Subscriber_Asyn::~Subscriber_Asyn (void) 
{ 

  /* Release resources */
  if (this->m_thread != NULL) 
    {
      delete this->m_thread; this->m_thread = NULL;
    }
 
  if (this->m_stats != NULL) 
    {
      delete this->m_stats; this->m_stats = NULL; 
    }

  return; 

}

} /* namespace core */

} /* namespace sdn */

#undef LOG_ALTERN_SRC

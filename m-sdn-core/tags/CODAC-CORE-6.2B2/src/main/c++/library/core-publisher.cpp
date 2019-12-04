/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/core-publisher.cpp $
* $Id: core-publisher.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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
#include "sdn-mcast.h" /* SDN core library - API definition (sdn::mcast) */
#include "sdn-ucast.h" /* SDN core library - API definition (sdn::ucast) */
#include "sdn-core.h" /* SDN core library - API definition (sdn::core) */

#include "sdn-packet.h"
#include "sdn-header.h"
#include "sdn-topic.h"

#include "core-participant.h"
#include "core-publisher.h" /* This class definition */

/* Constants */

#define DEFAULT_PUBLISHER_PERIOD 1000000000L /* 1Hz */
#define DEFAULT_PUBLISHER_WAIT_PERIOD 10000L

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::core"

/* Type definition */

namespace sdn {

namespace core {

/* Global variables */

/* Function declaration */

/* Function definition */

static void Publisher_Asyn_Preamble (Publisher_Asyn* self) { if (self != NULL) self->Preamble(); return; }
static void Publisher_Asyn_CB (Publisher_Asyn* self) { if (self != NULL) self->Do(); return; }

/* Initializer methods */

void PublisherImpl::Initialize (void) 
{ 

  /* Initialize attributes */
  this->m_base = static_cast<base::PublisherIface*>(NULL); 
  this->m_host_order = HelperTools::GetNativeByteOrder(); 
  this->m_comm_order = HelperTools::GetNativeByteOrder(); 
  this->m_source_port = 0u;

  return; 

}

void Publisher_Asyn::Initialize (void)
{

  log_trace("Publisher_Asyn::Initialize - Entering method");

  /* Create thread instance */
  char thread_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  snprintf(thread_name, STRING_MAX_LENGTH, "Publisher_Asyn_%s", this->GetTopicName());

  this->m_thread = new base::AnyThread ((char*) thread_name);
  (this->m_thread)->SetPreamble((void (*) (void*)) &Publisher_Asyn_Preamble, (void*) this); 
  (this->m_thread)->SetCallback((void (*) (void*)) &Publisher_Asyn_CB, (void*) this);
  (this->m_thread)->SetPeriod(DEFAULT_PUBLISHER_PERIOD);

  this->m_cb = NULL; this->m_attr = NULL;
  this->m_accuracy = DEFAULT_PUBLISHER_WAIT_PERIOD;
  this->m_synchronous = false; this->m_trigger = false;

  log_trace("Publisher_Asyn::Initialize - Leaving method");

  return; 

}

/* Initializer methods */

RET_STATUS Publisher_Asyn::Launch (void) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->Launch(); return status; }
RET_STATUS Publisher_Asyn::Terminate (void) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->Terminate(); return status; }

/* Accessor methods */

bool PublisherIface::IsInitialized (void) { return (this->p_impl)->IsInitialized(); }
    
char* PublisherIface::GetInterface (void) { return (this->p_impl)->GetInterface(); }
char* PublisherIface::GetTopicName (void) { return (this->p_impl)->GetTopicName(); }
uint_t PublisherIface::GetTopicSize (void) { return (this->p_impl)->GetTopicSize(); }
    
RET_STATUS PublisherIface::SetInterface (char* name) { return (this->p_impl)->ParticipantImpl::SetInterface(name); }
RET_STATUS PublisherIface::SetInterface (const char* name, uint_t port) { return (this->p_impl)->SetInterface(name, port); }
RET_STATUS PublisherIface::SetMetadata (Metadata_t& mdata) { return (this->p_impl)->SetMetadata(mdata); }

RET_STATUS PublisherIface::CopyTopicInstance (void* instance, uint_t size) { return (this->p_impl)->CopyTopicInstance(instance, size); }
void* PublisherIface::GetTopicHeader (void) { return (this->p_impl)->GetTopicHeader(); }
void* PublisherIface::GetTopicFooter (void) { return (this->p_impl)->GetTopicFooter(); }
void* PublisherIface::GetTopicInstance (void) { return (this->p_impl)->GetTopicInstance(); }
    
RET_STATUS PublisherIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb); }
RET_STATUS PublisherIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); }

int PublisherIface::GetSocket (void) const { return (this->p_impl)->GetSocket(); }

RET_STATUS Publisher_Asyn::SetAffinity (uint_t core) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->SetAffinity(core); return status; }

RET_STATUS Publisher_Asyn::SetPeriod (uint64_t period, int64_t phase, uint64_t accuracy) 
{ 

  RET_STATUS status = STATUS_ERROR; 

  if (period == 0L)
    {
      this->m_accuracy = accuracy;
      this->m_trigger = false;
      this->m_synchronous = true;
    }

  if (this->m_thread != NULL) 
    { 
      (this->m_thread)->SetAccuracy(accuracy); 
      (this->m_thread)->SetPhase(phase); 
      status = (this->m_thread)->SetPeriod(period); 
    } 

  return status; 

}

RET_STATUS Publisher_Asyn::SetPriority (uint_t policy, uint_t priority) { RET_STATUS status = STATUS_ERROR; if (this->m_thread != NULL) status = (this->m_thread)->SetPriority(policy, priority); return status; }

/* Miscellaneous methods */

RET_STATUS PublisherIface::Configure (void) { return (this->p_impl)->Configure(); }
RET_STATUS PublisherIface::Publish (void) { return (this->p_impl)->Publish(); }
    
RET_STATUS PublisherImpl::Configure (void) 
{ 

  log_trace("PublisherImpl::Configure - Entering method");

  RET_STATUS status = STATUS_ERROR;

  /* Configure base class */
  if (Participant_Configure((ParticipantImpl*) this) != STATUS_SUCCESS)
    {
      log_error("PublisherImpl::Configure - Failed to configure participant"); 
      return status;
    }
#ifdef USE_PACKET_FOOTER  
  /* Prepare footer fields */
  char host_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  char prog_name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  if (get_host_name(host_name) != STATUS_SUCCESS)
    {
      log_warning("get_host_name() failed");
    }
  else
    {
      log_debug("Host name is '%s'", host_name);
    }

  if (get_program_name(prog_name) != STATUS_SUCCESS)
    {
      log_warning("get_program_name() failed");
    }
  else
    {
      log_debug("Program name is '%s'", prog_name);
    }

  char source_id [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  snprintf((char*) source_id, STRING_MAX_LENGTH, "%s@%s", (char*) prog_name, (char*) host_name);

  uint_t source_uid = hash((char*) source_id);

  log_info("Set packet.footer.source_uid to '%u' (derived from '%s')", source_uid, (char*) source_id);

  ((this->m_packet)->GetFooter())->SetSourceUID(source_uid);
#endif
  /* Bug 7919 - Return error in case interface is invalid */
  if (HelperTools::IsInterfaceValid(this->GetInterface()) != true)
    { 
      log_error("PublisherImpl::Configure - Currently defined interface '%s' is not valid", this->GetInterface()); 
      return status; 
    }

  /* WARNING - This code does not allow for changing between MCAST and UCAST participants at runtime */

  /* Create generic publisher, if necessary */
  if ((this->m_base != NULL) && 
      ((strcmp((this->m_base)->GetAddress(), (this->m_topic)->GetMCastGroup()) != 0) ||
       ((this->m_base)->GetPort() != (this->m_topic)->GetMCastPort()) ||
       ((this->m_base)->GetSize() != (this->m_packet)->GetSize())))
    {
      log_debug("PublisherImpl::Configure - Closing base::Publisher instance");
      (this->m_base)->Close();
    }

  if (this->m_base == NULL) 
    { 
      if (sdn::HelperTools::IsMCastAddress((this->m_topic)->GetMCastGroup()) == true) { log_debug("Publisher::Configure - Creating mcast::Publisher instance"); this->m_base = (base::PublisherIface*) new mcast::Publisher (); }
      if (sdn::HelperTools::IsUCastAddress((this->m_topic)->GetMCastGroup()) == true) { log_debug("Publisher::Configure - Creating ucast::Publisher instance"); this->m_base = (base::PublisherIface*) new ucast::Publisher (); }
    }

  log_debug("PublisherImpl::Configure - Configuring base::Publisher instance");

  (this->m_base)->SetInterface(this->GetInterface());
  (this->m_base)->SetSourcePort(this->m_source_port); 

  (this->m_base)->SetAddress((this->m_topic)->GetMCastGroup()); 
  (this->m_base)->SetPort((this->m_topic)->GetMCastPort()); 

  if (0u != this->m_depth) (this->m_base)->SetBufferDepth(this->m_depth * ((this->m_packet)->GetSize() + 68u));

  status = (this->m_base)->Open();

  log_debug("PublisherImpl::Configure - Defining base::Publisher buffer and size");
  (this->m_base)->SetBuffer((this->m_packet)->GetInstance(), (this->m_packet)->GetSize());

  log_trace("PublisherImpl::Configure - Leaving method");

  return status; 

}

RET_STATUS PublisherImpl::Publish (void) 
{ 

  log_trace("PublisherImpl::Publish - Entering method");

  RET_STATUS status = STATUS_ERROR;

  if (this->m_topic == NULL) /* Instantiated in sdn::core::Publisher::Configure */
    {
      log_error("PublisherImpl::Publish - Un-initialized topic");
      return status;
    }
  else if ((this->m_topic)->IsInitialized() != true) /* Incomplete metadata: name only must be supported by XML topic definition file, else metadata must be complete enough */
    {
      log_error("PublisherImpl::Publish - Un-initialized topic '%s'", (this->m_topic)->GetName());
      return status;
    }

  /* Update packet header - Only the timestamp now - Do the counter after */
  //((this->m_packet)->GetHeader())->UpdateInstance(); 
  ((this->m_packet)->GetHeader())->SetTimestamp(); /* Update only the timestamp */
#ifdef USE_PACKET_FOOTER  
  /* Update packet footer */
  ((this->m_packet)->GetFooter())->SetTopicCRC((this->m_topic)->GetInstanceChecksum()); 
#endif
  /* Invoke callback, if any */
  if (this->m_cb != NULL) { log_trace("PublisherImpl::Publish - Invoke callback"); (*(this->m_cb))(this->m_attr); }

  /* Bug 7919 - mcast::Subscriber is not initialized in case of wrong interface */
  if (this->m_base != NULL) 
    {
      /* Bug 10327 - Deleted */
      /* Bug 10809 - Select byte order for messaging over the network */
      if (this->m_comm_order == sdn::types::BigEndian)
	{
	  ((this->m_packet)->GetHeader())->ToNetworkByteOrder();
#ifdef USE_PACKET_FOOTER  
	  ((this->m_packet)->GetFooter())->ToNetworkByteOrder();
#endif
	}
      
      status = (this->m_base)->Publish(); 

      /* Bug 10327 - Deleted */
      /* Bug 10809 - Select byte order for messaging over the network */
      if (this->m_comm_order == sdn::types::BigEndian)
	{
	  ((this->m_packet)->GetHeader())->FromNetworkByteOrder();
#ifdef USE_PACKET_FOOTER  
	  ((this->m_packet)->GetFooter())->FromNetworkByteOrder();
#endif
	}
    }

  /* Update packet header - Prepare for next iteration, if any */
  ((this->m_packet)->GetHeader())->IncrCounter();

  log_trace("PublisherImpl::Publish - Leaving method");

  return status;

}

RET_STATUS Publisher_Asyn::Do (void)
{

  log_trace("Publisher_Asyn::Do - Entering method");

  RET_STATUS status = STATUS_ERROR;

  while ((this->m_synchronous == true) && (__sync_val_compare_and_swap(&(this->m_trigger), true, false) == false)) wait_for(this->m_accuracy);

  if (this->GetTopicInstance() == NULL)
    {
      log_debug("Publisher_Asyn::Do - Skip execution - No topic instance defined");
      return status;
    }

  /* ToDo - Should be waiting for external trigger in case of synchronous operation */

  /* Callback, if any */
  if (this->m_cb != NULL) (*(this->m_cb))(this->m_attr);

  if (this->Publish() != STATUS_SUCCESS)
    {
      log_warning("Publisher_Asyn::Do - Publisher::Publish failed");
    }
  else 
    {
      log_debug("Publisher_Asyn::Do - Publisher::Publish successful at '%lu'", get_time());

      status = STATUS_SUCCESS;
    }

  log_trace("Publisher_Asyn::Do - Leaving method");

  return status; 

}

/* Constructor methods */

PublisherIface::PublisherIface (void)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new PublisherImpl ();

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

PublisherIface::PublisherIface (Metadata_t& mdata)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new PublisherImpl (mdata);

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}
#if 0
PublisherIface::PublisherIface (char* name)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new PublisherImpl (name);

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}
#endif
PublisherIface::PublisherIface (Topic& topic)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  /* Initialize attributes */
  this->p_impl = new PublisherImpl (topic);

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

Publisher_Base::Publisher_Base (void) 
{ 

  log_trace("Publisher_Base::Publisher_Base - Entering method"); 

  this->SetInstanceType(OBJTYPE_CORE_PUBLISHER); 

  /* Initialize resources */ 
  this->Initialize(); 

  log_trace("Publisher_Base::Publisher_Base - Leaving method"); 

  return; 

}

PublisherImpl::PublisherImpl (void) 
{ 

  log_trace("PublisherImpl::PublisherImpl - Entering method"); 

  /* Initialize resources */ 
  this->Initialize(); 

  log_trace("PublisherImpl::PublisherImpl - Leaving method"); 

  return; 

}

PublisherImpl::PublisherImpl (Topic& topic) 
{ 

  log_trace("PublisherImpl::PublisherImpl - Entering method"); 

  /* Initialize resources */ 
  this->Initialize(); 
  this->SetTopic(topic); 

  log_trace("PublisherImpl::PublisherImpl - Leaving method"); 

  return; 

}

/* Destructor method */

PublisherIface::~PublisherIface (void)
{
        
  log_trace("PublisherIface::~PublisherIface - Entering method");
        
  /* Release resources */
  if (this->p_impl != NULL) delete this->p_impl; this->p_impl= NULL;
        
  log_trace("PublisherIface::~PublisherIface - Leaving method");
        
  return;
        
}

PublisherImpl::~PublisherImpl (void) 
{ 

  if (this->m_base != NULL)
    {
      /* ToDo - Call appropriate destructor */
      delete this->m_base; this->m_base = NULL; 
    }

  return; 

}

Publisher_Asyn::~Publisher_Asyn (void) { if (this->m_thread != NULL) delete this->m_thread; this->m_thread = NULL; return; }

} /* namespace core */

} /* namespace sdn */

#undef LOG_ALTERN_SRC

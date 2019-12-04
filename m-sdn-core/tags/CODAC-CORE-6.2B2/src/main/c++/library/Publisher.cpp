/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/Publisher.cpp $
* $Id: Publisher.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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

// Global header files

#include <new> // std::nothrow

// Local header files

#include "constants.h" // Constants valid for this scope
#include "sdn-types.h" // Misc. type definition, e.g. RET_STATUS
#include "sdn-tools.h" // Misc. helper functions, e.g. hash, etc.

#include "sdn-base.h" // Privately scoped base classes definition

#include "sdn-header.h"
#include "sdn-footer.h"
#include "sdn-topic.h"
#include "sdn-packet.h"

#include "core-participant.h"
#include "core-publisher.h"

#include "disc-messenger.h"

#include "ParticipantImpl.h"

#include "Publisher.h" // Interface class definition
#include "PublisherImpl.h" // This class definition

// Constants

#define OBJTYPE_SDN_PUBLISHER (char*) "sdn::Publisher_Base"

#define DEFAULT_PUBLISHER_PERIOD 1000000000ul // 1Hz
#define DEFAULT_PUBLISHER_WAIT_PERIOD 10000ul

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::pub"

// Type definition

namespace sdn {

// Global variables

// Function declaration

// Function definition

// Initialiser methods

void PublisherImpl::Initialise (void)
{ 

  log_trace("PublisherImpl::Initialise - Entering method");

  // Initialise attributes
  this->m_base = static_cast<core::ParticipantImpl*>(NULL); 

  // Acquire resources
  log_debug("PublisherImpl::Initialise - Creating core::Publisher instance"); 
  this->m_base = dynamic_cast<core::ParticipantImpl*>(new (std::nothrow) core::PublisherImpl ());

  log_trace("PublisherImpl::Initialise - Leaving method");

  return; 

}

// Initialiser methods

// Accessor methods
    
bool PublisherIface::IsInitialized (void) const { return (this->p_impl)->IsInitialized(); }
bool PublisherIface::IsPayloadOrdered (void) const { bool ret = (this->GetPayloadOrder() == sdn::HelperTools::GetNativeByteOrder()); return ret; } // Bug 10327 - Same byte ordering as host

const char* PublisherIface::GetInterface (void) const { return (this->p_impl)->GetInterface(); }
const char* PublisherIface::GetTopicName (void) const { return (this->p_impl)->GetTopicName(); }
uint_t PublisherIface::GetTopicSize (void) const { return (this->p_impl)->GetTopicSize(); }
    
void* PublisherIface::GetTopicHeader (void) const { return (this->p_impl)->GetTopicHeader(); }
void* PublisherIface::GetTopicFooter (void) const { return (this->p_impl)->GetTopicFooter(); }
void* PublisherIface::GetTopicInstance (void) const { return (this->p_impl)->GetTopicInstance(); }

types::Endianness_t PublisherIface::GetPayloadOrder (void) const { return (this->p_impl)->GetPayloadOrder(); }
void PublisherIface::SetPayloadOrder (types::Endianness_t order) { (this->p_impl)->SetPayloadOrder(order); return; }

RET_STATUS PublisherIface::SetInterface (const char* name) { return (this->p_impl)->ParticipantImpl::SetInterface(name); }
RET_STATUS PublisherIface::SetInterface (const char* name, uint_t port) { return (this->p_impl)->SetInterface(name, port); }
RET_STATUS PublisherIface::SetBufferDepth (uint_t depth) { return (this->p_impl)->SetBufferDepth(depth); }

RET_STATUS PublisherIface::SetMetadata (Metadata_t& mdata) { return (this->p_impl)->SetMetadata(mdata); }
RET_STATUS PublisherIface::SetTopic (Topic& topic) { return (this->p_impl)->SetTopic(topic); }

RET_STATUS PublisherIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb); }
RET_STATUS PublisherIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); }

int PublisherIface::GetSocket (void) { return (this->p_impl)->GetSocket(); }

types::Endianness_t PublisherImpl::GetPayloadOrder (void) const { return dynamic_cast<core::PublisherImpl*>(this->m_base)->GetPayloadOrder(); }
types::Endianness_t PublisherImpl::SetPayloadOrder (types::Endianness_t order) { return dynamic_cast<core::PublisherImpl*>(this->m_base)->SetPayloadOrder(order); }

RET_STATUS PublisherImpl::SetInterface (const char* name, uint_t port) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (static_cast<core::ParticipantImpl*>(NULL) != this->m_base)
    { 
      status = dynamic_cast<core::PublisherImpl*>(this->m_base)->SetInterface(name, port);
    }

  return status; 

}

// Miscellaneous methods

RET_STATUS PublisherIface::Configure (void) 
{ 

  RET_STATUS status = STATUS_ERROR; 

  status = (this->p_impl)->Configure();

  // For backward compatibility purposes with v1.0
  this->m_topic = ((this->p_impl)->m_base)->m_topic;
  this->m_packet = ((this->p_impl)->m_base)->m_packet;
  if (this->m_packet != NULL) this->m_header = (this->m_packet)->GetHeader();
  if (this->m_packet != NULL) this->m_footer = (this->m_packet)->GetFooter();

  return status; 

}

RET_STATUS PublisherIface::CopyTopicInstance (void* instance, uint_t size) { return (this->p_impl)->CopyTopicInstance(instance, size); }
RET_STATUS PublisherIface::DoBackgroundActivity (void) { return (this->p_impl)->DoBackgroundActivity(); }
RET_STATUS PublisherIface::Publish (void) { return (this->p_impl)->Publish(); }
    
RET_STATUS PublisherImpl::Configure (void)
{ 

  log_trace("PublisherImpl::Configure - Entering method");

  RET_STATUS status = STATUS_ERROR;

  // Configure base class
  if (Participant_Configure(dynamic_cast<ParticipantImpl*>(this)) != STATUS_SUCCESS)
    {
      log_error("PublisherImpl::Configure - Failed to configure participant"); 
      return status;
    }

  log_debug("PublisherImpl::Configure - Configure core::Publisher instance");
  if ((this->m_base)->Configure() != STATUS_SUCCESS)
    {
      log_error("PublisherImpl::Configure - Failed to configure core::Publisher instance"); 
      return status;
    }

  log_debug("PublisherImpl::Configure - Notify disc::Messenger instance");
  (this->m_disc)->Register(this->m_base);

  status = STATUS_SUCCESS; 
  
  log_trace("PublisherImpl::Configure - Leaving method");

  return status; 

}

RET_STATUS PublisherImpl::Publish (void) 
{ 

  log_trace("PublisherImpl::Publish - Entering method");

  RET_STATUS status = STATUS_ERROR;

  status = dynamic_cast<core::PublisherImpl*>(this->m_base)->Publish();

  log_trace("PublisherImpl::Publish - Leaving method");

  return status;

}
#if 0
RET_STATUS MsgablePublisher::ProcessMessage (const char* msg)
{

  log_trace("MsgablePublisher::ProcessMessage - Entering method");

  RET_STATUS status = STATUS_ERROR;

  log_info("MsgablePublisher::ProcessMessage - Received message '%s'", msg);
#undef KEYWORD
#define KEYWORD "Configure"
  if (strncmp(msg, KEYWORD, strlen(KEYWORD)) == 0)
    {
      status = this->Configure();
    }
#undef KEYWORD
#define KEYWORD "Do"
  if (strncmp(msg, KEYWORD, strlen(KEYWORD)) == 0)
    {
      status = this->Do();
    }
#undef KEYWORD
#define KEYWORD "Publish"
  if (strncmp(msg, KEYWORD, strlen(KEYWORD)) == 0)
    {
      status = this->Publish();
    }
#undef KEYWORD
#define KEYWORD "SetInterface"
  if (strncmp(msg, KEYWORD, strlen(KEYWORD)) == 0)
    {
      char* p_buf = (char*) msg;
      char* p_attr = NULL;

      while ((*p_buf != 0) && (*p_buf != '(')) p_buf += 1;

      if (*p_buf != 0) p_buf += 1; p_attr = p_buf;

      while ((*p_buf != 0) && (*p_buf != ')')) p_buf += 1;

      if (*p_buf != 0) *p_buf = 0; p_buf += 1;

      status = this->SetInterface(p_attr);
    }
#undef KEYWORD
#define KEYWORD "SetTopicName"
  if (strncmp(msg, KEYWORD, strlen(KEYWORD)) == 0)
    {
      char* p_buf = (char*) msg;
      char* p_attr = NULL;

      while ((*p_buf != 0) && (*p_buf != '(')) p_buf += 1;

      if (*p_buf != 0) p_buf += 1; p_attr = p_buf;

      while ((*p_buf != 0) && (*p_buf != ')')) p_buf += 1;

      if (*p_buf != 0) *p_buf = 0; p_buf += 1;

      status = this->SetTopicName(p_attr);
    }

  log_trace("MsgablePublisher::ProcessMessage - Leaving method");

  return status; 

};
#endif
// Constructor methods

PublisherIface::PublisherIface (void)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  // Initialise attributes
  this->p_impl = new (std::nothrow) PublisherImpl ();

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

PublisherIface::PublisherIface (Metadata_t& mdata)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  // Initialise attributes
  this->p_impl = static_cast<PublisherImpl*>(NULL);

  bool ok = (static_cast<PublisherImpl*>(NULL) != 
	     (this->p_impl = new (std::nothrow) PublisherImpl ()));

  if (ok)
    {
      //ok = (this->SetMetadata(mdata) == STATUS_SUCCESS);
      this->SetMetadata(mdata);
    }

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

PublisherIface::PublisherIface (Topic& topic)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  // Initialise attributes
  this->p_impl = static_cast<PublisherImpl*>(NULL);

  bool ok = (static_cast<PublisherImpl*>(NULL) != 
	     (this->p_impl = new (std::nothrow) PublisherImpl ()));

  if (ok)
    {
      //ok = (this->SetTopic(topic) == STATUS_SUCCESS);
      this->SetTopic(topic);
    }

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

Publisher_Base::Publisher_Base (void) { this->SetInstanceType(OBJTYPE_SDN_PUBLISHER); return; }

PublisherImpl::PublisherImpl (void) 
{ 

  log_trace("PublisherImpl::PublisherImpl - Entering method"); 

  // Initialise resources 
  this->Initialise(); 

  log_trace("PublisherImpl::PublisherImpl - Leaving method"); 

  return; 

}

// Destructor method

PublisherIface::~PublisherIface (void)
{
        
  log_trace("PublisherIface::~PublisherIface - Entering method");
        
  // Release resources
  if (static_cast<PublisherImpl*>(NULL) != this->p_impl) 
    {
      delete this->p_impl; this->p_impl= static_cast<PublisherImpl*>(NULL);
    }
        
  log_trace("PublisherIface::~PublisherIface - Leaving method");
        
  return;
        
}

PublisherImpl::~PublisherImpl (void) 
{ 

  log_trace("PublisherImpl::~PublisherImpl - Entering method");
        
  // Release resources
  if (this->m_disc != NULL) (this->m_disc)->Remove(this->m_base); 

  if (this->m_base != NULL) delete dynamic_cast<core::PublisherImpl*>(this->m_base); this->m_base = NULL; 

  log_trace("PublisherImpl::~PublisherImpl - Leaving method");
        
  return; 

}

} // namespace sdn

#undef LOG_ALTERN_SRC

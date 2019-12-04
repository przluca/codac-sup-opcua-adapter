/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/mcast-publisher.cpp $
* $Id: mcast-publisher.cpp 100239 2019-06-23 15:40:27Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>

// Local header files

#include "sdn-types.h" // Misc. type definition, e.g. RET_STATUS
#include "sdn-tools.h" // Misc. helper functions, e.g. hash, etc.

#include "mcast-api.h" // API definition (ccs::mcast)
#include "mcast-constants.h" // Constants valid for this scope
#include "mcast-participant.h"
#include "mcast-publisher.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::mcast"

// Type definition

namespace ccs {

namespace mcast {

// Global variables

// Function declaration

// Function definition

// Initializer methods

// Accessor methods

const char* PublisherIface::GetInterfaceAddress (void) const { return (this->p_impl)->GetAddress(); }
const char* PublisherIface::GetInterface (void) const { return (this->p_impl)->GetInterface(); }
const char* PublisherIface::GetMCastAddr (void) const { return (this->p_impl)->GetMCastAddr(); }
uint_t PublisherIface::GetMCastPort (void) const { return (this->p_impl)->GetMCastPort(); }
    
void PublisherIface::SetInterface (const char* iface) { return (this->p_impl)->SetInterface(iface); }
void PublisherIface::SetMCastAddr (const char* addr) { return (this->p_impl)->SetMCastAddr(addr); }
void PublisherIface::SetMCastPort (const uint_t port) { (this->p_impl)->SetMCastPort(port); /* (this->p_impl)->SetSourcePort(port); */ return; }
void PublisherIface::SetSourcePort (const uint_t port) { (this->p_impl)->SetSourcePort(port); return; }
void PublisherIface::SetBufferDepth (const uint_t depth) { return (this->p_impl)->SetBufferDepth(depth); }

void* PublisherIface::GetBuffer (void) const { return (this->p_impl)->GetBuffer(); }
uint_t PublisherIface::GetSize (void) const { return (this->p_impl)->GetSize(); }
void PublisherIface::SetBuffer (void* buffer, const uint_t size) { return (this->p_impl)->SetBuffer(buffer, size); }

RET_STATUS PublisherIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb); } // Routine called before message publication
RET_STATUS PublisherIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); } // Routine called before message publication

int PublisherIface::GetSocket (void) const { return (this->p_impl)->GetSocket(); }

// Miscellaneous methods

RET_STATUS PublisherIface::Open (void) { return (this->p_impl)->Open(); }
RET_STATUS PublisherIface::Close (void) { return (this->p_impl)->Close(); }

RET_STATUS PublisherIface::Publish (const void* buffer, const uint_t size) { return (this->p_impl)->Publish(buffer, size); }
RET_STATUS PublisherIface::Publish (void) { return (this->p_impl)->Publish(); }
RET_STATUS PublisherImpl::Open (void)
{

  log_trace("PublisherImpl::Open - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  // Common base class implementation
  Participant_Open(dynamic_cast<ParticipantImpl*>(this));

  if (this->m_socket < 0)
    {
      log_error("PublisherImpl::Open - m_socket has not been initialized");
      return status;
    }

  // Set socket options - Set reuse flag to allow for multiple instances running on the same machine
  {
    int reuse = 1;
    
    if (setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse)) < 0)
      {
	log_error("PublisherImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("PublisherImpl::Open - setsockopt(...) successful");
      }
  }

  // Bind socket to physical interface and port
  {
    struct sockaddr_in mcast_addr;

    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(this->m_if_addr);
    mcast_addr.sin_port = htons(this->m_source_port);
    
    // Bind socket to multicast address
    if (bind(this->m_socket, (struct sockaddr*) &mcast_addr, sizeof(mcast_addr)) != 0)
      {
	log_error("PublisherImpl::Open - bind(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("PublisherImpl::Open - bind(...) successful");
	log_debug("PublisherImpl::Open - Interface is '%s %d'", this->m_if_addr, this->m_source_port);
      }
  }

  // Set socket buffer length
  {
    if (this->m_depth != MaxIPv4PacketSize)
      {
	log_info("PublisherImpl::Open - Overload default socket buffer depth '%u'", this->m_depth);
      }

    int length = this->m_depth;

    if (setsockopt(this->m_socket, SOL_SOCKET, SO_SNDBUF, (char*) &length, sizeof(length))  < 0)
      {
	log_error("PublisherImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("PublisherImpl::Open - setsockopt(...) successful");
      }
  }

  // Disable UDP checksum
  {
    int disable = 1;

    if (setsockopt(this->m_socket, SOL_SOCKET, SO_NO_CHECK, (char*) &disable, sizeof(disable))  < 0)
      {
	log_error("PublisherImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("PublisherImpl::Open - setsockopt(...) successful");
      }
  }

  status = STATUS_SUCCESS;

  log_trace("PublisherImpl::Open - Leaving method"); 

  return status;

}

RET_STATUS PublisherImpl::Close (void)
{

  log_trace("PublisherImpl::Close - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  // Common base class implementation
  Participant_Close(dynamic_cast<ParticipantImpl*>(this));

  status = STATUS_SUCCESS;

  log_trace("PublisherImpl::Close - Leaving method"); 

  return status;

}

RET_STATUS PublisherImpl::Publish (const void* buffer, const uint_t size)
{

  log_trace("PublisherImpl::Publish - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("PublisherImpl::Publish - m_socket has not been initialized");
      return status;
    }
#if 0
  if (size > MAX_PACKET_SIZE)
    {
      log_warning("PublisherImpl::Publish - Payload size '%u' above reasonable limit", size);
    }
#endif
  struct sockaddr_in mcast_addr;

  memset(&mcast_addr, 0, sizeof(mcast_addr));
  mcast_addr.sin_family = AF_INET;
  mcast_addr.sin_addr.s_addr = inet_addr(this->m_mcast_group);
  mcast_addr.sin_port = htons(this->m_mcast_port);
    
  if (sendto(this->m_socket, buffer, size, MSG_DONTWAIT, (struct sockaddr*) &mcast_addr, sizeof(mcast_addr)) < 0)
    {
      log_error("PublisherImpl::Publish - send(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }
  else
    {
      log_debug("PublisherImpl::Publish - send(...) successful with size '%d' ('%p')", size, buffer);
      status = STATUS_SUCCESS;
    }

  log_trace("PublisherImpl::Publish - Leaving method"); 

  return status;

}

RET_STATUS PublisherImpl::Publish (void) 
{ 

  log_trace("PublisherImpl::Publish - Entering method"); 

  // Invoke callback, if any
  if (this->m_cb != NULL) (*(this->m_cb))(this->m_attr);

  log_trace("PublisherImpl::Publish - Leaving method"); 

  return this->Publish(this->m_buffer, this->m_size); 

}

// Constructor methods

PublisherIface::PublisherIface (void)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new PublisherImpl ();

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

PublisherIface::PublisherIface (const char* iface, const char* addr, const uint_t port)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new PublisherImpl (iface, addr, port);

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

PublisherImpl::PublisherImpl (void) 
{ 

  log_trace("PublisherImpl::PublisherImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_MCAST_PUBLISHER);
  this->Initialize();

  log_trace("PublisherImpl::PublisherImpl - Leaving method"); 

  return; 

}

PublisherImpl::PublisherImpl (const char* iface, const char* addr, const uint_t port) 
{ 

  log_trace("PublisherImpl::PublisherImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_MCAST_PUBLISHER); 
  this->Initialize(); 

  this->SetInterface(iface); 
  this->SetMCastAddr(addr); 
  this->SetMCastPort(port); 
  //this->SetSourcePort(port); // Bug10809 - Default port

  // Open and configure socket 
  this->Open(); 

  log_trace("PublisherImpl::PublisherImpl - Leaving method"); 

  return; 

}

// Destructor method

PublisherIface::~PublisherIface (void)
{
        
  log_trace("PublisherIface::~PublisherIface - Entering method");
        
  // Release resources
  if (this->p_impl != NULL) delete this->p_impl; this->p_impl= NULL;
        
  log_trace("PublisherIface::~PublisherIface - Leaving method");
        
  return;
        
}

PublisherImpl::~PublisherImpl (void) 
{ 

  log_trace("PublisherImpl::~PublisherImpl - Entering method"); 

  // Close socket 
  this->Close(); 

  log_trace("PublisherImpl::~PublisherImpl - Leaving method"); 

  return; 

}

} // namespace mcast

} // namespace ccs

#undef LOG_ALTERN_SRC
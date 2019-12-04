/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/ucast-publisher.cpp $
* $Id: ucast-publisher.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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

#include "ucast-api.h" // API definition (ccs::ucast)
#include "ucast-constants.h" // Constants valid for this scope
#include "ucast-participant.h"
#include "ucast-publisher.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::ucast"

// Type definition

namespace ccs {

namespace ucast {

// Global variables

// Function declaration

// Function definition

// Initializer methods

// Accessor methods

const char* PublisherIface::GetInterfaceAddress (void) const { return (this->p_impl)->GetAddress(); }
const char* PublisherIface::GetInterface (void) const { return (this->p_impl)->GetInterface(); }
const char* PublisherIface::GetUCastAddr (void) const { return (this->p_impl)->GetUCastAddr(); }
uint_t PublisherIface::GetUCastPort (void) const { return (this->p_impl)->GetUCastPort(); }
    
void PublisherIface::SetInterface (const char* iface) { return (this->p_impl)->SetInterface(iface); }
void PublisherIface::SetUCastAddr (const char* addr) { return (this->p_impl)->SetUCastAddr(addr); }
void PublisherIface::SetUCastPort (uint_t port) { return (this->p_impl)->SetUCastPort(port); }
void PublisherIface::SetSourcePort (const uint_t port) { return (this->p_impl)->SetSourcePort(port); }
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

  // Bind socket to local interface address
  {
    struct sockaddr_in iface_addr;

    memset(&iface_addr, 0, sizeof(iface_addr));
    iface_addr.sin_family = AF_INET;
    iface_addr.sin_addr.s_addr = inet_addr(this->m_if_addr);
    iface_addr.sin_port = htons(this->m_source_port); 
    
    // Bind socket to multicast address
    if (bind(this->m_socket, (struct sockaddr*) &iface_addr, sizeof(iface_addr)) != 0)
      {
	log_error("PublisherImpl::Open - bind(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("PublisherImpl::Open - bind(...) successful");
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

  status = STATUS_SUCCESS;

  log_trace("PublisherImpl::Open - Leaving method"); 

  return status;

}

RET_STATUS PublisherImpl::Close (void)
{

  log_trace("PublisherImpl::Close - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("PublisherImpl::Close - m_socket has not been initialized");
      return status;
    }

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

  struct sockaddr_in ucast_addr; // The remote address to which to send
  int addr_size = sizeof(ucast_addr);

  memset(&ucast_addr, 0, sizeof(ucast_addr));
  ucast_addr.sin_family = AF_INET;
  ucast_addr.sin_addr.s_addr = inet_addr(this->m_ucast_addr);
  ucast_addr.sin_port = htons(this->m_ucast_port);
    
  if (sendto(this->m_socket, buffer, size, MSG_DONTWAIT, (struct sockaddr*) &ucast_addr, addr_size) < 0)
    {
      log_error("PublisherImpl::Publish - sendto(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }
  else
    {
      log_debug("PublisherImpl::Publish - sendto(...) successful with size '%d' ('%p')", size, buffer);
      status = STATUS_SUCCESS;
    }

  log_trace("PublisherImpl::Publish - Leaving method"); 

  return status;

}

RET_STATUS PublisherImpl::Publish (void) { return this->Publish(this->m_buffer, this->m_size); }

// Constructor methods

PublisherIface::PublisherIface (void)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new PublisherImpl ();

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

PublisherIface::PublisherIface (const char* iface, const char* addr, const uint_t size)
{

  log_trace("PublisherIface::PublisherIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new PublisherImpl (iface, addr, size);

  log_trace("PublisherIface::PublisherIface - Leaving method"); 

  return;

}

PublisherImpl::PublisherImpl (void) 
{ 

  log_trace("PublisherImpl::PublisherImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_UCAST_PUBLISHER);
  this->Initialize();

  log_trace("PublisherImpl::PublisherImpl - Leaving method"); 

  return; 

}

PublisherImpl::PublisherImpl (const char* iface, const char* addr, const uint_t port) 
{ 

  log_trace("PublisherImpl::PublisherImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_UCAST_PUBLISHER); 
  this->Initialize(); 

  this->SetInterface(iface); 
  this->SetUCastAddr(addr); 
  this->SetUCastPort(port); 

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

} // namespace ucast

} // namespace ccs

#undef LOG_ALTERN_SRC

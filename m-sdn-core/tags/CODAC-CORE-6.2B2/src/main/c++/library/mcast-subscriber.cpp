/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/mcast-subscriber.cpp $
* $Id: mcast-subscriber.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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
#include "mcast-subscriber.h" // This class definition

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

const char* SubscriberIface::GetInterfaceAddress (void) const { return (this->p_impl)->GetAddress(); }
const char* SubscriberIface::GetInterface (void) const { return (this->p_impl)->GetInterface(); }
const char* SubscriberIface::GetMCastAddr (void) const { return (this->p_impl)->GetMCastAddr(); }
uint_t SubscriberIface::GetMCastPort (void) const { return (this->p_impl)->GetMCastPort(); }
    
void SubscriberIface::SetInterface (const char* iface) { return (this->p_impl)->SetInterface(iface); }
void SubscriberIface::SetMCastAddr (const char* addr) { return (this->p_impl)->SetMCastAddr(addr); }
void SubscriberIface::SetMCastPort (uint_t port) { return (this->p_impl)->SetMCastPort(port); }
void SubscriberIface::SetBufferDepth (const uint_t depth) { return (this->p_impl)->SetBufferDepth(depth); }

void* SubscriberIface::GetBuffer (void) const { return (this->p_impl)->GetBuffer(); }
uint_t SubscriberIface::GetSize (void) const { return (this->p_impl)->GetSize(); }
void SubscriberIface::SetBuffer (void* buffer, const uint_t size) { return (this->p_impl)->SetBuffer(buffer, size); }
#if 0
uint64_t SubscriberIface::GetTimeout (void) const { return (this->p_impl)->GetTimeout(); }
void SubscriberIface::SetTimeout (const uint64_t timeout) { return (this->p_impl)->SetTimeout(timeout); }

bool SubscriberIface::IsUpdated (void) { return (this->p_impl)->IsUpdated(); } // Successful Receive
#endif
RET_STATUS SubscriberIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb); } // Routine called before message publication
RET_STATUS SubscriberIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); } // Routine called before message publication

int SubscriberIface::GetSocket (void) const { return (this->p_impl)->GetSocket(); }

// Miscellaneous methods

RET_STATUS SubscriberIface::Open (void) { return (this->p_impl)->Open(); }
RET_STATUS SubscriberIface::Close (void) { return (this->p_impl)->Close(); }

RET_STATUS SubscriberIface::Receive (void* buffer, uint_t& size, const uint64_t timeout) { return (this->p_impl)->Receive(buffer, size, timeout); }
RET_STATUS SubscriberIface::Receive (void* buffer, uint_t& size) { return (this->p_impl)->Receive(buffer, size); }
RET_STATUS SubscriberIface::Receive (const uint64_t timeout) { return (this->p_impl)->Receive(timeout); }
RET_STATUS SubscriberIface::Receive (void) { return (this->p_impl)->Receive(); }

RET_STATUS SubscriberImpl::Open (void)
{

  log_trace("SubscriberImpl::Open - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  // Common base class implementation
  Participant_Open(dynamic_cast<ParticipantImpl*>(this));

  if (this->m_socket < 0)
    {
      log_error("SubscriberImpl::Open - m_socket has not been initialized");
      return status;
    }

  // Set socket options - Set reuse flag to allow for multiple instances running on the same machine
  {
    int reuse = 1;
    
    if (setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse)) < 0)
      {
	log_error("SubscriberImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("SubscriberImpl::Open - setsockopt(...) successful");
      }
  }

  // Bind socket to multicast address
  {
    struct sockaddr_in mcast_addr;

    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(this->m_mcast_group);
    mcast_addr.sin_port = htons(this->m_mcast_port);
    
    // Bind socket to multicast address
    if (bind(this->m_socket, (struct sockaddr*) &mcast_addr, sizeof(mcast_addr)) != 0)
      {
	log_error("SubscriberImpl::Open - bind(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("SubscriberImpl::Open - bind(...) successful");
	log_debug("SubscriberImpl::Open - MCAST group is '%s %d'", this->m_mcast_group, this->m_mcast_port);
      }
  }

  // Set socket options - Add IGMP membership
  {
    struct ip_mreq ip_req;

    ip_req.imr_multiaddr.s_addr = inet_addr(this->m_mcast_group);
    ip_req.imr_interface.s_addr = inet_addr(this->m_if_addr);

    if (setsockopt(this->m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &ip_req, sizeof(ip_req)) < 0)
      {
	log_error("SubscriberImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("SubscriberImpl::Open - setsockopt(...) successful");
      }
  }

  // Set socket buffer length*/
  {
    if (this->m_depth != MaxIPv4PacketSize)
      {
	log_info("SubscriberImpl::Open - Overload default socket buffer depth '%u'", this->m_depth);
      }

    int length = this->m_depth;

    if (setsockopt(this->m_socket, SOL_SOCKET, SO_RCVBUF, (char*) &length, sizeof(length))  < 0)
      {
	log_error("SubscriberImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("SubscriberImpl::Open - setsockopt(...) successful");
      }
  }

  status = STATUS_SUCCESS;

  log_trace("SubscriberImpl::Open - Leaving method"); 

  return status;

}

RET_STATUS SubscriberImpl::Close (void)
{

  log_trace("SubscriberImpl::Close - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("SubscriberImpl::Close - m_socket has not been initialized");
      return status;
    }

  // Set socket options - Drop IGMP membership
  {
    struct ip_mreq ip_req;

    ip_req.imr_multiaddr.s_addr = inet_addr(this->m_mcast_group);
    ip_req.imr_interface.s_addr = inet_addr(this->m_if_addr);

    if (setsockopt(this->m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*) &ip_req, sizeof(ip_req)) < 0)
      {
	log_error("SubscriberImpl::Close - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("SubscriberImpl::Close - setsockopt(...) successful");
      }
  }

  // Common base class implementation
  Participant_Close(dynamic_cast<ParticipantImpl*>(this));

  status = STATUS_SUCCESS;

  log_trace("SubscriberImpl::Close - Leaving method"); 

  return status;

}

RET_STATUS SubscriberImpl::Receive (void* buffer, uint_t& size, const uint64_t timeout) // ToDo - Update timeout
{

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("SubscriberImpl::Receive - m_socket has not been initialized");
      return status;
    }
#if 0
  if (size > MAX_PACKET_SIZE)
    {
      log_warning("SubscriberImpl::Receive - Payload size '%u' above reasonable limit", size);
    }
#endif
  fd_set sel_fd;
  struct timespec sel_timeout; ns_to_timespec(timeout, sel_timeout);
      
  FD_ZERO(&sel_fd); FD_SET(this->m_socket, &sel_fd);

  if (pselect(this->m_socket + 1, &sel_fd, NULL, NULL, &sel_timeout, NULL) != 1)
    {
      //log_warning("Subscriber::Receive - pselect(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }

  return this->Receive(buffer, size);

}

RET_STATUS SubscriberImpl::Receive (void* buffer, uint_t& size)
{

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("SubscriberImpl::Receive - m_socket has not been initialized");
      return status;
    }
#if 0
  if (size > MAX_PACKET_SIZE)
    {
      log_warning("SubscriberImpl::Receive - Payload size '%u' above reasonable limit", size);
    }
#endif
  int recvsize = 0;

  if ((recvsize = recvfrom(this->m_socket, buffer, size, 0, 0, 0)) < 0)
    {
      log_error("SubscriberImpl::Receive - recvfrom(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }
  else
    {
      //log_debug("SubscriberImpl::Receive - recvfrom(...) successful with size '%d'", recvsize);
      size = recvsize;
      status = STATUS_SUCCESS;
    }

  return status;

}

RET_STATUS SubscriberImpl::Receive (const uint64_t timeout) // Blocking wait with configurable timeout - Buffer and size defined using mcast::Participant::SetBuffer()
{ 

  RET_STATUS status = STATUS_ERROR; 

  (void) __sync_val_compare_and_swap(&this->m_updated, true, false); // Clear flag
  uint_t size = this->m_size;

  if ((status = this->Receive(this->m_buffer, size, timeout)) == STATUS_SUCCESS) // WARNING - Size updated
    {
      (void) __sync_val_compare_and_swap(&this->m_updated, false, true); // Set flag

      // Invoke callback, if any
      if (this->m_cb != NULL) (*(this->m_cb))(this->m_attr);
    }

  return status;
 
}

RET_STATUS SubscriberImpl::Receive (void) // Blocking wait - Buffer and size defined using mcast::Participant::SetBuffer()
{ 

  RET_STATUS status = STATUS_ERROR; 

  (void) __sync_val_compare_and_swap(&this->m_updated, true, false); // Clear flag
  uint_t size = this->m_size;

  if ((status = this->Receive(this->m_buffer, size)) == STATUS_SUCCESS) // WARNING - Size updated
    {
      (void) __sync_val_compare_and_swap(&this->m_updated, false, true); // Set flag

      // Invoke callback, if any
      if (this->m_cb != NULL) (*(this->m_cb))(this->m_attr);
    }

  return status;
 
}

// Constructor methods

SubscriberIface::SubscriberIface (void)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new SubscriberImpl ();

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}

SubscriberIface::SubscriberIface (const char* iface, const char* addr, const uint_t size)
{

  log_trace("SubscriberIface::SubscriberIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new SubscriberImpl (iface, addr, size);

  log_trace("SubscriberIface::SubscriberIface - Leaving method"); 

  return;

}

SubscriberImpl::SubscriberImpl (void) 
{ 

  log_trace("SubscriberImpl::SubscriberImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_MCAST_SUBSCRIBER);
  this->Initialize();

  log_trace("SubscriberImpl::SubscriberImpl - Leaving method"); 

  return; 

}

SubscriberImpl::SubscriberImpl (const char* iface, const char* addr, const uint_t port) 
{ 

  log_trace("SubscriberImpl::SubscriberImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_MCAST_SUBSCRIBER); 
  this->Initialize(); 

  this->SetInterface(iface); 
  this->SetMCastAddr(addr); 
  this->SetMCastPort(port); 

  // Open and configure socket 
  this->Open(); 

  log_trace("SubscriberImpl::SubscriberImpl - Leaving method"); 

  return; 

}

// Destructor method

SubscriberIface::~SubscriberIface (void)
{
        
  log_trace("SubscriberIface::~SubscriberIface - Entering method");
        
  // Release resources
  if (this->p_impl != NULL) delete this->p_impl; this->p_impl= NULL;
        
  log_trace("SubscriberIface::~SubscriberIface - Leaving method");
        
  return;
        
}

SubscriberImpl::~SubscriberImpl (void) 
{ 

  log_trace("SubscriberImpl::~SubscriberImpl - Entering method"); 

  // Close socket 
  this->Close(); 

  log_trace("SubscriberImpl::~SubscriberImpl - Leaving method"); 

  return; 

}

} // namespace mcast

} // namespace ccs

#undef LOG_ALTERN_SRC

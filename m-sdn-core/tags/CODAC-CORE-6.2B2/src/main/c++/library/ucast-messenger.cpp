/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/ucast-messenger.cpp $
* $Id: ucast-messenger.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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
#include "ucast-messenger.h" // This class definition

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

const char* MessengerIface::GetInterfaceAddress (void) const { return (this->p_impl)->GetAddress(); }
const char* MessengerIface::GetInterface (void) const { return (this->p_impl)->GetInterface(); }
const char* MessengerIface::GetUCastAddr (void) const { return (this->p_impl)->GetUCastAddr(); }
uint_t MessengerIface::GetUCastPort (void) const { return (this->p_impl)->GetUCastPort(); }
    
void MessengerIface::SetInterface (const char* iface) { return (this->p_impl)->SetInterface(iface); }
void MessengerIface::SetUCastAddr (const char* addr) { return (this->p_impl)->SetUCastAddr(addr); }
void MessengerIface::SetUCastPort (uint_t port) { return (this->p_impl)->SetUCastPort(port); }
void MessengerIface::SetBufferDepth (const uint_t depth) { return (this->p_impl)->SetBufferDepth(depth); }

void* MessengerIface::GetBuffer (void) const { return (this->p_impl)->GetBuffer(); }
uint_t MessengerIface::GetSize (void) const { return (this->p_impl)->GetSize(); }
void MessengerIface::SetBuffer (void* buffer, const uint_t size) { return (this->p_impl)->SetBuffer(buffer, size); }
#if 0
uint64_t MessengerIface::GetTimeout (void) const { return (this->p_impl)->GetTimeout(); }
void MessengerIface::SetTimeout (const uint64_t timeout) { return (this->p_impl)->SetTimeout(timeout); }

bool MessengerIface::IsUpdated (void) { return (this->p_impl)->IsUpdated(); } // Successful Receive
#endif
RET_STATUS MessengerIface::SetCallback (void (* cb)(void*)) { return (this->p_impl)->SetCallback(cb); } // Routine called before message publication
RET_STATUS MessengerIface::SetCallback (void (* cb)(void*), void* attr) { return (this->p_impl)->SetCallback(cb, attr); } // Routine called before message publication

int MessengerIface::GetSocket (void) const { return (this->p_impl)->GetSocket(); }

// Miscellaneous methods

RET_STATUS MessengerIface::Open (void) { return (this->p_impl)->Open(); }
RET_STATUS MessengerIface::Close (void) { return (this->p_impl)->Close(); }

RET_STATUS MessengerIface::Publish (const void* buffer, const uint_t size) { return (this->p_impl)->Publish(buffer, size); }
RET_STATUS MessengerIface::Publish (void) { return (this->p_impl)->Publish(); }

RET_STATUS MessengerIface::Receive (void* buffer, uint_t& size, const uint64_t timeout) { return (this->p_impl)->Receive(buffer, size, timeout); }
RET_STATUS MessengerIface::Receive (void* buffer, uint_t& size) { return (this->p_impl)->Receive(buffer, size); }
RET_STATUS MessengerIface::Receive (const uint64_t timeout) { return (this->p_impl)->Receive(timeout); }
RET_STATUS MessengerIface::Receive (void) { return (this->p_impl)->Receive(); }

RET_STATUS MessengerIface::Reply (const void* buffer, const uint_t size) { return (this->p_impl)->Reply(buffer, size); }

RET_STATUS MessengerImpl::Open (void)
{

  log_trace("MessengerImpl::Open - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  // Common base class implementation
  Participant_Open(dynamic_cast<ParticipantImpl*>(this));

  if (this->m_socket < 0)
    {
      log_error("MessengerImpl::Open - m_socket has not been initialized");
      return status;
    }

  // Bind socket to local interface address
  {
    struct sockaddr_in iface_addr;

    memset(&iface_addr, 0, sizeof(iface_addr));
    iface_addr.sin_family = AF_INET;
    iface_addr.sin_addr.s_addr = inet_addr(this->m_if_addr);
    iface_addr.sin_port = htons(this->m_ucast_port);
    
    // Bind socket to multicast address
    if (bind(this->m_socket, (struct sockaddr*) &iface_addr, sizeof(iface_addr)) != 0)
      {
	log_error("MessengerImpl::Open - bind(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("MessengerImpl::Open - bind(...) successful");
      }
  }

  // Disable UDP checksum
  {
    int disable = 1;

    if (setsockopt(this->m_socket, SOL_SOCKET, SO_NO_CHECK, (char*) &disable, sizeof(disable))  < 0)
      {
	log_error("MessengerImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("MessengerImpl::Open - setsockopt(...) successful");
      }
  }

  // Set socket buffer length
  {
    int length = this->m_depth;

    if (setsockopt(this->m_socket, SOL_SOCKET, SO_SNDBUF, (char*) &length, sizeof(length))  < 0)
      {
	log_error("MessengerImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("MessengerImpl::Open - setsockopt(...) successful");
      }
  }

  // Set socket buffer length*/
  {
    int length = this->m_depth;

    if (setsockopt(this->m_socket, SOL_SOCKET, SO_RCVBUF, (char*) &length, sizeof(length))  < 0)
      {
	log_error("MessengerImpl::Open - setsockopt(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("MessengerImpl::Open - setsockopt(...) successful");
      }
  }

  status = STATUS_SUCCESS;

  log_trace("MessengerImpl::Open - Leaving method"); 

  return status;

}

RET_STATUS MessengerImpl::Close (void)
{

  log_trace("MessengerImpl::Close - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("MessengerImpl::Close - m_socket has not been initialized");
      return status;
    }

  // Common base class implementation
  Participant_Close(dynamic_cast<ParticipantImpl*>(this));

  status = STATUS_SUCCESS;

  log_trace("MessengerImpl::Close - Leaving method"); 

  return status;

}

RET_STATUS MessengerImpl::Publish (const void* buffer, const uint_t size)
{

  log_trace("MessengerImpl::Publish - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("MessengerImpl::Publish - m_socket has not been initialized");
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
      log_error("MessengerImpl::Publish - sendto(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }
  else
    {
      log_debug("MessengerImpl::Publish - sendto(...) successful with size '%d' ('%p')", size, buffer);
      status = STATUS_SUCCESS;
    }

  log_trace("MessengerImpl::Publish - Leaving method"); 

  return status;

}

RET_STATUS MessengerImpl::Publish (void) { return this->Publish(this->m_buffer, this->m_size); }

RET_STATUS MessengerImpl::Receive (void* buffer, uint_t& size, const uint64_t timeout) // ToDo - Update timeout
{

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("MessengerImpl::Receive - m_socket has not been initialized");
      return status;
    }

  fd_set sel_fd;
  struct timespec sel_timeout; ns_to_timespec(timeout, sel_timeout);
      
  FD_ZERO(&sel_fd); FD_SET(this->m_socket, &sel_fd);

  if (pselect(this->m_socket + 1, &sel_fd, NULL, NULL, &sel_timeout, NULL) != 1)
    {
      //log_warning("Messenger::Receive - pselect(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }

  return this->Receive(buffer, size);

}

RET_STATUS MessengerImpl::Receive (void* buffer, uint_t& size)
{

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("MessengerImpl::Receive - m_socket has not been initialized");
      return status;
    }

  struct sockaddr_in ucast_addr; // The remote address to which to reply
  int addr_size = sizeof(ucast_addr);

  memset(&ucast_addr, 0, addr_size);

  int recvsize = 0;

  if ((recvsize = recvfrom(this->m_socket, buffer, size, 0, (struct sockaddr*) &ucast_addr, (socklen_t*) &addr_size)) < 0)
    {
      log_error("MessengerImpl::Receive - recvfrom(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }
  else
    {
      log_debug("MessengerImpl::Receive - recvfrom(...) successful with size '%d'", recvsize);

      sstrncpy(this->m_reply_addr, inet_ntoa(ucast_addr.sin_addr) , MaxIPv4AddrLength);
      this->m_reply_port = ntohs(ucast_addr.sin_port);

      log_debug("MessengerImpl::Receive - Received message from '%s:%d'", this->m_reply_addr, this->m_reply_port);

      size = recvsize;
      status = STATUS_SUCCESS;
    }
  
  return status;

}

RET_STATUS MessengerImpl::Receive (const uint64_t timeout) // Blocking wait with configurable timeout - Buffer and size defined using ucast::Participant::SetBuffer()
{ 

  RET_STATUS status = STATUS_ERROR; 

  (void) __sync_val_compare_and_swap(&this->m_updated, true, false); // Clear flag
  this->m_recvsize = 0u;

  uint_t size = this->m_size;

  if ((status = this->Receive(this->m_buffer, size, timeout)) == STATUS_SUCCESS) // WARNING - Size updated
    {
      this->m_recvsize = size;
      (void) __sync_val_compare_and_swap(&this->m_updated, false, true); // Set flag

      // Invoke callback, if any
      if (this->m_cb != NULL) (*(this->m_cb))(this->m_attr);
    }

  return status;
 
}

RET_STATUS MessengerImpl::Receive (void) // Blocking wait - Buffer and size defined using ucast::Participant::SetBuffer()
{ 

  RET_STATUS status = STATUS_ERROR; 

  (void) __sync_val_compare_and_swap(&this->m_updated, true, false); // Clear flag
  this->m_recvsize = 0u;

  uint_t size = this->m_size;

  if ((status = this->Receive(this->m_buffer, size)) == STATUS_SUCCESS) // WARNING - Size updated
    {
      this->m_recvsize = size;
      (void) __sync_val_compare_and_swap(&this->m_updated, false, true); // Set flag

      // Invoke callback, if any
      if (this->m_cb != NULL) (*(this->m_cb))(this->m_attr);
    }

  return status;
 
}

RET_STATUS MessengerImpl::Reply (const void* buffer, const uint_t size)
{

  log_trace("MessengerImpl::Reply - Entering method"); 

  RET_STATUS status = STATUS_ERROR;

  if (this->m_socket < 0)
    {
      log_error("MessengerImpl::Reply - m_socket has not been initialized");
      return status;
    }

  struct sockaddr_in ucast_addr; // The remote address to which to send
  int addr_size = sizeof(ucast_addr);

  memset(&ucast_addr, 0, sizeof(ucast_addr));
  ucast_addr.sin_family = AF_INET;
  ucast_addr.sin_addr.s_addr = inet_addr(this->m_reply_addr);
  ucast_addr.sin_port = htons(this->m_reply_port);
    
  if (sendto(this->m_socket, buffer, size, MSG_DONTWAIT, (struct sockaddr*) &ucast_addr, addr_size) < 0)
    {
      log_error("MessengerImpl::Reply - sendto(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }
  else
    {
      log_debug("MessengerImpl::Reply - sendto(...) successful with size '%d' ('%p')", size, buffer);
      status = STATUS_SUCCESS;
    }

  log_trace("MessengerImpl::Reply - Leaving method"); 

  return status;

}

// Constructor methods

MessengerIface::MessengerIface (void)
{

  log_trace("MessengerIface::MessengerIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new MessengerImpl ();

  log_trace("MessengerIface::MessengerIface - Leaving method"); 

  return;

}

MessengerIface::MessengerIface (const char* iface, const char* addr, const uint_t size)
{

  log_trace("MessengerIface::MessengerIface - Entering method"); 

  // Initialize attributes
  this->p_impl = new MessengerImpl (iface, addr, size);

  log_trace("MessengerIface::MessengerIface - Leaving method"); 

  return;

}

MessengerImpl::MessengerImpl (void) 
{ 

  log_trace("MessengerImpl::MessengerImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_UCAST_MESSENGER);
  this->Initialize();

  log_trace("MessengerImpl::MessengerImpl - Leaving method"); 

  return; 

}

MessengerImpl::MessengerImpl (const char* iface, const char* addr, const uint_t port) 
{ 

  log_trace("MessengerImpl::MessengerImpl - Entering method"); 

  // Initialize attributes 
  this->SetInstanceType(OBJTYPE_UCAST_MESSENGER); 
  this->Initialize(); 

  this->SetInterface(iface); 
  this->SetUCastAddr(addr); 
  this->SetUCastPort(port); 

  // Open and configure socket 
  this->Open(); 

  log_trace("MessengerImpl::MessengerImpl - Leaving method"); 

  return; 

}

// Destructor method

MessengerIface::~MessengerIface (void)
{
        
  log_trace("MessengerIface::~MessengerIface - Entering method");
        
  // Release resources
  if (this->p_impl != NULL) delete this->p_impl; this->p_impl= NULL;
        
  log_trace("MessengerIface::~MessengerIface - Leaving method");
        
  return;
        
}

MessengerImpl::~MessengerImpl (void) 
{ 

  log_trace("MessengerImpl::~MessengerImpl - Entering method"); 

  // Close socket 
  this->Close(); 

  log_trace("MessengerImpl::~MessengerImpl - Leaving method"); 

  return; 

}

} // namespace ucast

} // namespace ccs

#undef LOG_ALTERN_SRC
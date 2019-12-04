/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/ucast-participant.cpp $
* $Id: ucast-participant.cpp 100239 2019-06-23 15:40:27Z bauvirb $
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
#include "ucast-participant.h" // This class definition

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

void ParticipantImpl::Initialize (void)
{

  log_trace("ParticipantImpl::Initialize - Entering method"); 

  // Initialize attributes
  this->SetBuffer(NULL, 0u);
  this->SetCallback(NULL, NULL);
  this->SetInterface(STRING_UNDEFINED);
  this->SetUCastAddr(STRING_UNDEFINED);
  this->SetUCastPort(0u);

  this->m_socket = -1;
  this->m_depth = MaxIPv4PacketSize;

  log_trace("ParticipantImpl::Initialize - Leaving method"); 

  return;

}

// Accessor methods

void ParticipantImpl::SetInterface (const char* iface) { if (HelperTools::IsInterfaceValid(iface) != true) log_warning("ParticipantImpl::SetInterface - Interface '%s' is invalid", iface); sstrncpy(this->m_if_name, iface, MaxIPv4AddrLength); return; }
void ParticipantImpl::SetUCastAddr (const char* addr) { if (IsAddrValid(addr) != true) log_warning("ParticipantImpl::SetUCastAddr - Address '%s' is invalid", addr); sstrncpy(this->m_ucast_addr, addr, MaxIPv4AddrLength); return; }
void ParticipantImpl::SetUCastPort (const uint_t port) { this->m_ucast_port = port; return; }

void ParticipantImpl::SetBuffer (void* buffer, const uint_t size) 
{ 

  log_trace("ParticipantImpl::SetBuffer - Entering method ('%p')", buffer); 

  // Bug 9249 - Natural payload size limit for UDP/IPv4
  if (size > MaxUCastPacketSize) log_warning("ParticipantImpl::SetBuffer - Buffer size '%u' is invalid - UCAST payload limited to '%u'", size, MaxUCastPacketSize); 

  this->m_buffer = buffer; this->m_size = size; 

  log_trace("ParticipantImpl::SetBuffer - Leaving method ('%p')", buffer); 

  return; 

}

// Miscellaneous methods

bool IsAddrValid (const char* addr) { return HelperTools::IsUCastAddress(addr); }

RET_STATUS Participant_Open (ParticipantImpl* self)
{

  log_trace("Participant_Open - Entering routine"); 

  RET_STATUS status = STATUS_ERROR;

  self->m_socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (self->m_socket < 0)
    {
      log_error("Participant_Open - socket(...) failed with '%d - %s'", errno, strerror(errno));
      return status;
    }
  else
    {
      log_debug("Participant_Open - socket(...) successful");
    }

  // Get address of selected local interface
  {
    struct ifreq if_req;

    strcpy(if_req.ifr_name, self->m_if_name);
    
    if (ioctl(self->m_socket, SIOCGIFADDR, &if_req) < 0)
      {
	log_error("Participant_Open - ioctl(...) failed with '%d - %s'", errno, strerror(errno));
	return status;
      }
    else
      {
	log_debug("Participant_Open - ioctl(...) successful");
	log_debug("Participant_Open - IP address is '%s'", inet_ntoa(((struct sockaddr_in*)&if_req.ifr_addr)->sin_addr));
      }

    strncpy(self->m_if_addr, inet_ntoa(((struct sockaddr_in*)&if_req.ifr_addr)->sin_addr), MaxIPv4AddrLength);
  }

  // Set socket options - Set reuse flag to allow for multiple instances running on the same machine
  {
    int reuse = 1;
    
    if (setsockopt(self->m_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse)) < 0)
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

  log_trace("Participant_Open - Leaving routine"); 

  return status;

}

RET_STATUS Participant_Close (ParticipantImpl* self)
{

  log_trace("Participant_Close - Entering routine"); 

  RET_STATUS status = STATUS_ERROR;

  if (self->m_socket < 0)
    {
      return status;
    }

  close(self->m_socket);
  self->m_socket = -1;

  status = STATUS_SUCCESS;

  log_trace("Participant_Close - Leaving routine"); 

  return status;

}

// Constructor methods

ParticipantImpl::ParticipantImpl (void)
{

  log_trace("ParticipantImpl::ParticipantImpl - Entering method"); 

  // Initialize attributes
  this->Initialize();

  log_trace("ParticipantImpl::ParticipantImpl - Leaving method"); 

  return;

}

// Destructor method

ParticipantImpl::~ParticipantImpl (void) {}

} // namespace ucast

} // namespace ccs

#undef LOG_ALTERN_SRC

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/core/Messenger.cpp $
* $Id: Messenger.cpp 100241 2019-06-24 07:00:39Z bauvirb $
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

#include <functional> // std::function, etc.
#include <chrono> // std::chrono, etc.
#include <new> // std::nothrow
#include <string> // std::string, etc.

#include <types.h> // Misc. type definition, e.g. RET_STATUS
#include <tools.h> // Misc. helper functions, e.g. hash, etc.

//#define LOG_TRACE_ENABLE
//#define LOG_DEBUG_ENABLE
#include <log-api.h> // Logging helper functions

#include <any-thread.h>

#include <mcast-api.h> // API definition (ccs::mcast)
#include <mcast-constants.h>
#include <mcast-participant.h>
#include <mcast-messenger.h>

// Local header files

#include "sup-messenger.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

class MessengerImpl
{

  private:

    Channel_t m_channel; 
    Role_t m_role;

    ccs::base::SynchronisedThreadWithCallback* p_thrd; 

  public:

    ccs::mcast::MessengerImpl* p_base;

    char m_buffer [ccs::mcast::MaxMCastPacketSize];
    bool m_defined;
    std::function<bool(const std::string&)> m_cb;

    MessengerImpl (const Channel_t channel, const Role_t role);
    virtual ~MessengerImpl (void);

    bool Send (const std::string& msg);
    bool RegisterHandler (const std::function<bool(const std::string&)>& cb);

};

// Global variables

// Function declaration

void MessengerImpl_CB (MessengerImpl* self);

// Function definition

void MessengerImpl_CB (MessengerImpl* self)
{

  log_trace("MessengerImpl::MessengerImpl_CB - Entering method");

  if (self->p_base->Receive(100000000ul) == STATUS_SUCCESS)
    {

      // WARNING - The implementation assumes null-terminated char arrays being transported
      log_debug("MessengerImpl::MessengerImpl_CB - Received message '%s'", self->m_buffer);
      std::string msg (static_cast<const char*>(self->m_buffer)) ;
      
      // Provide message as std::string to the application-specific handler
      if (self->m_defined) self->m_cb(msg);

    }

  log_trace("MessengerImpl::MessengerImpl_CB - Leaving method"); 

  return;

};

// Initialiser methods

// Accessor methods

bool MessengerImpl::RegisterHandler (const std::function<bool(const std::string&)>& cb)
{

  this->m_cb = cb;
  this->m_defined = true;

  return true;

};

// Miscellaneous methods

bool Messenger::Send (const std::string& msg)
{

  bool status = false;

  if (__impl != static_cast<MessengerImpl*>(NULL)) status = __impl->Send(msg);

  return status;

};

bool MessengerImpl::Send (const std::string& msg)
{

  bool status = false;

  // WARNING - The implementation assumes null-terminated char arrays being transported
  if (this->p_base != static_cast<ccs::mcast::MessengerImpl*>(NULL)) status = (((this->p_base)->Publish(msg.c_str(), msg.length()+1) == STATUS_SUCCESS) ? true : false);

  return status;

};

// Constructor methods

Messenger::Messenger (const Channel_t channel, const Role_t role)
{

  log_trace("Messenger::Messenger - Entering method"); 

  // Initialize attributes
  this->__impl = new (std::nothrow) MessengerImpl (channel, role);

  log_info("Register message handler"); 
  using namespace std::placeholders;
  __impl->RegisterHandler(std::bind(&Messenger::HandleReceive, this, _1));

  log_trace("Messenger::Messenger - Leaving method"); 

  return;

}

MessengerImpl::MessengerImpl (const Channel_t channel, const Role_t role)
{

  log_trace("MessengerImpl::MessengerImpl - Entering method"); 

  // Initialize attributes
  m_channel = channel;
  m_role = role;

  m_cb = NULL;
  m_defined = false;

  // Try and retrieve interface name from environment variable
  char iface [STRING_MAX_LENGTH] = STRING_UNDEFINED; sstrncpy(iface, ccs::mcast::DefaultIfaceName, STRING_MAX_LENGTH); 
  get_env_variable((char*) "SDN_INTERFACE_NAME", (char*) iface, STRING_MAX_LENGTH);
  if (net_is_interface_valid(iface) != true) log_notice("MessengerImpl::MessengerImpl - SDN_INTERFACE_NAME is either not defined or wrongly defined");

  // Define MCAST address and port from messenger channel/role attributes
  char mcast_addr [STRING_MAX_LENGTH]; sstrncpy(mcast_addr, sdn::DiscoveryMCastAddr, STRING_MAX_LENGTH);
  uint_t mcast_port = sdn::DiscoveryMCastPort;

  if (channel == TCN)
    {
      get_env_variable((char*) "PON_INTERFACE_NAME", (char*) iface, STRING_MAX_LENGTH);
      if (net_is_interface_valid(iface) != true) log_notice("MessengerImpl::MessengerImpl - PON_INTERFACE_NAME is either not defined or wrongly defined");
      sstrncpy((char*) mcast_addr, tcn::DiscoveryMCastAddr, STRING_MAX_LENGTH);
      mcast_port = tcn::DiscoveryMCastPort;
    }

  // Bug 10103 - Asynchronous messenger is instantiated before buffer is passed to it

  // Instantiate messenger class with asynchronous reception support
  log_info("Instantiate ccs::mcast::Messenger class with '%s, %s, %u", iface, mcast_addr, mcast_port);
  p_base = new (std::nothrow) ccs::mcast::MessengerImpl (iface, mcast_addr, mcast_port); p_base->SetBuffer(static_cast<void*>(this->m_buffer), ccs::mcast::MaxMCastPacketSize);
  p_thrd = new (std::nothrow) ccs::base::SynchronisedThreadWithCallback ("ccs::mcast::Messenger"); p_thrd->SetCallback((void (*)(void*)) &MessengerImpl_CB, static_cast<void*>(this)); p_thrd->SetPeriod(0ul); p_thrd->Launch();

  log_info("Associate buffer to ccs::mcast::Messenger for asynchronous message reception"); p_base->SetBuffer(m_buffer, ccs::mcast::MaxMCastPacketSize);

  log_trace("MessengerImpl::MessengerImpl - Leaving method"); 

  return;

}

// Destructor method

Messenger::~Messenger (void)
{
        
  log_trace("Messenger::~Messenger - Entering method");
        
  // Release resources
  if (__impl != static_cast<MessengerImpl*>(NULL)) delete __impl; __impl= static_cast<MessengerImpl*>(NULL);
        
  log_trace("Messenger::~Messenger - Leaving method");
        
  return;
        
}
MessengerImpl::~MessengerImpl (void)
{
        
  log_trace("MessengerImpl::~MessengerImpl - Entering method");
        
  // Release resources
  if (this->p_thrd != static_cast<ccs::base::SynchronisedThreadWithCallback*>(NULL)) delete this->p_thrd; this->p_thrd = static_cast<ccs::base::SynchronisedThreadWithCallback*>(NULL);
  if (this->p_base != static_cast<ccs::mcast::MessengerImpl*>(NULL)) delete this->p_base; this->p_base = static_cast<ccs::mcast::MessengerImpl*>(NULL);
        
  log_trace("MessengerImpl::~MessengerImpl - Leaving method");
        
  return;
        
}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC
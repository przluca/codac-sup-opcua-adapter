/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessMonitor.cpp $
* $Id: PVAccessMonitor.cpp 99790 2019-05-14 07:31:20Z bauvirb $
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

/* ToDo 
     - Manage default value from configuration
*/

// Global header files

#include <new> // std::nothrow
#include <functional> // std::function

#include <pv/pvData.h>
#include <pva/client.h>

#include <BasicTypes.h> // Global type definition
#include <SysTools.h> // Misc. helper functions

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE
#include <log-api.h> // Syslog wrapper routines

#include <AnyType.h> // Introspectable data type ..
#include <AnyTypeHelper.h> // .. associated helper routines
#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

#include "AnyTypeToPVA.h" // .. associated helper routines
#include "PVAToAnyType.h" // .. associated helper routines

#include "AnyValueToPVA.h" // .. associated helper routines
#include "PVAToAnyValue.h" // .. associated helper routines

#include "PVAccessMonitor.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "pva-if"

// Type definition

namespace ccs {

namespace base {

class PVAccessMonitor_Impl : public pvac::ClientChannel::MonitorCallback
{

  private:

    bool __initialised;
    bool __connected;
    ccs::types::string __name;
    std::function<void(const ccs::base::PVAccessMonitor::Event&)> __ecb;
    std::function<void(const ccs::types::AnyValue&)> __dcb;

    pvac::ClientProvider* __provider;
    pvac::ClientChannel* __channel;
    pvac::Monitor __monitor;

    std::shared_ptr<const ccs::types::CompoundType> __type;
    ccs::types::AnyValue* __value;

  protected:

  public:

    // Initializer methods
    bool Initialise (void);

    // Accessor methods
    bool IsConnected (void) const { return __connected; };

    const char* GetChannel (void) const { return __name; };
    bool SetChannel (const char* name) { ccs::HelperTools::SafeStringCopy(__name, name, ccs::types::MaxStringLength); return true; };
    bool RegisterEventHandler(std::function<void(const ccs::base::PVAccessMonitor::Event&)> cb) { __ecb = cb; return true; };
    bool RegisterDataHandler(std::function<void(const ccs::types::AnyValue&)> cb) { __dcb = cb; return true; };

    // Miscellaneous methods
    virtual void monitorEvent (const pvac::MonitorEvent& event); // Specialise virtual method

    // Constructor methods
    PVAccessMonitor_Impl (void) { __initialised = false; __connected = false; };

    // Destructor method
    virtual ~PVAccessMonitor_Impl (void); 

};

// Global variables

// Function declaration

// Function definition

// Initializer methods
 
bool PVAccessMonitor_Impl::Initialise (void) 
{ 

  __provider = new (std::nothrow) pvac::ClientProvider ("pva");
  __value = static_cast<ccs::types::AnyValue*>(NULL);

  bool status = (static_cast<pvac::ClientProvider*>(NULL) != __provider);

  if (status)
    {
      __channel = new (std::nothrow) pvac::ClientChannel (__provider->connect(__name));
      status = (static_cast<pvac::ClientChannel*>(NULL) != __channel);
    }

  if (status)
    {
      __monitor = __channel->monitor(this); // pvac::ClientChannel::MonitorCallback (this)
    }

  if (status)
    {
      __initialised = true;
    }

  return status;

}

// Accessor methods

bool PVAccessMonitor::IsConnected (void) const { return __impl->IsConnected(); }
const char* PVAccessMonitor::GetChannel (void) const { return __impl->GetChannel(); }

// Miscellaneous methods

void PVAccessMonitor::HandleEvent (const PVAccessMonitor::Event& event)
{

  if (PVAccessMonitor::Event::Connect == event)
    {
      log_notice("PVAccessMonitor::HandleEvent - Disconnect from '%s'", this->GetChannel());
    }

  if (PVAccessMonitor::Event::Disconnect == event)
    {
      log_notice("PVAccessMonitor::HandleEvent - Connect to '%s'", this->GetChannel());
    }

  return;

}

void PVAccessMonitor_Impl::monitorEvent (const pvac::MonitorEvent& event) // Specialise virtual method (pvac::ClientChannel::MonitorCallback)
{

  bool status = false;

  switch (event.event)
    {
      case pvac::MonitorEvent::Fail:
	log_warning("Monitor '%s' failed", this->GetChannel());
	break;
      case pvac::MonitorEvent::Cancel:
	log_warning("Monitor '%s' cancelled", this->GetChannel());
	break;
      case pvac::MonitorEvent::Disconnect:
	log_warning("Monitor '%s' disconnected", this->GetChannel());

	if (true == __connected)
	  {
	    // Call handler method
	    __ecb(ccs::base::PVAccessMonitor::Event::Disconnect);
	  }

	__connected = false;
	break;
      case pvac::MonitorEvent::Data:
	log_debug("Monitor '%s' ..", this->GetChannel());

	if (false == __connected)
	  {
	    // Call handler method
	    __ecb(ccs::base::PVAccessMonitor::Event::Connect);
	  }

	__connected = true;
	status = true;
	break;
    }

  if (status)
    {
      // Empty event queue
      while (__monitor.poll()) {}
    }

  if (status)
    {
      if (static_cast<ccs::types::AnyValue*>(NULL) == __value)
	{
#if 0
	  __value = new (std::nothrow) ccs::types::AnyValue (ccs::HelperTools::PVStructToAnyType((__monitor.root)->getStructure())); // Introspection interface
#else
	  __type = ccs::HelperTools::PVStructToAnyType<std::shared_ptr<const epics::pvData::PVStructure>>(__monitor.root); // In order to support arrays of structure
	  status = (__type ? true : false);

	  if (status)
	    {
	      __value = new (std::nothrow) ccs::types::AnyValue (__type); // Actual variable
	    }
	  else
	    {
	      log_error("Unable to extract type from server monitor");
	    }
#endif
	}

      status = (static_cast<ccs::types::AnyValue*>(NULL) != __value);
    }

  if (status)
    {
      // Update variable
      status = ccs::HelperTools::PVStructToAnyValue(__value, __monitor.root);
    }
#ifdef LOG_DEBUG_ENABLE
  if (status)
    {
      char buffer [1024];
      __value->SerialiseInstance(buffer, 1024u);
      log_debug(".. '%s'", buffer);
    }
#endif
  if (status)
    {
      // Call handler method
      __dcb(*__value);
    }

  return;

}

// Constructor methods

PVAccessMonitor::PVAccessMonitor (const char* channel)
{ 

  // Instantiate implementation class
  __impl = new (std::nothrow) PVAccessMonitor_Impl;

  bool status = (static_cast<PVAccessMonitor_Impl*>(NULL) != __impl);

  if (status)
    {
      using namespace std::placeholders;
      status = __impl->RegisterEventHandler(std::bind(&PVAccessMonitor::HandleEvent, this, _1));
    }

  if (status)
    {
      using namespace std::placeholders;
      status = __impl->RegisterDataHandler(std::bind(&PVAccessMonitor::HandleMonitor, this, _1));
    }

  if (status)
    {
      status = __impl->SetChannel(channel);
    }

  if (status)
    {
      status = __impl->Initialise();
    }

  return; 

}

// Destructor method

PVAccessMonitor::~PVAccessMonitor (void)
{ 

  if (static_cast<PVAccessMonitor_Impl*>(NULL) != __impl)
    { 
      delete __impl; 
    }

  __impl= static_cast<PVAccessMonitor_Impl*>(NULL);

  return;

}

PVAccessMonitor_Impl::~PVAccessMonitor_Impl (void) 
{ 

  if (true == __initialised)
    {
      __monitor.cancel(); 
    }

  if (static_cast<ccs::types::AnyValue*>(NULL) != __value)
    {
      delete __value;
      __value = static_cast<ccs::types::AnyValue*>(NULL);
    }

  if (static_cast<pvac::ClientChannel*>(NULL) != __channel)
    {
      delete __channel;
      __channel = static_cast<pvac::ClientChannel*>(NULL);
    }

  if (static_cast<pvac::ClientProvider*>(NULL) != __provider)
    {
      delete __provider;
      __provider = static_cast<pvac::ClientProvider*>(NULL);
    }

  return;

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

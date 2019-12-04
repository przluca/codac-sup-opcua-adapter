/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/cvvf/DelegatedFunctionHandler.cpp $
* $Id: DelegatedFunctionHandler.cpp 100159 2019-06-18 07:13:41Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <functional> // std::function<>
#include <map> // std::map
#include <new> // std::nothrow
#include <thread> // std::thread

#include <pthread.h> // pthread_cancel
#include <sys/prctl.h> // PR_SET_NAME

#include <SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.
#include <BasicTypes.h> // Misc. type definition
#include <log-api.h> // Logging helper functions

// Local header files

#include "DelegatedFunctionHandler.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

class DelegatedFunctionHandlerImpl
{

  private:

    ccs::types::uint32 __seed = 0u;

    struct DPC_t {
      bool running;
      bool status;
      std::function<bool(void*)> process;
      void* attribute;
    };

    // Warning - Deleting a joinable thread instance terminates the process. Aborting implies
    // and implementation with detached threads and storage of the native system handle.

    std::map<ccs::types::uint32,DPC_t> __processes;
    std::map<ccs::types::uint32,pthread_t> __handles; // See DelegatedFunctionHandler::Abort and DelegatedFunctionHandler::Launch

    ccs::types::uint32 GenerateKey (void) { __seed += 1u; return __seed; };

    DPC_t GetData (ccs::types::uint32 uid) const;
    bool SetData (ccs::types::uint32 uid, const DPC_t& data);

    bool Execute (ccs::types::uint32 uid);

  public:

    DelegatedFunctionHandlerImpl (void) {};
    virtual ~DelegatedFunctionHandlerImpl (void);

    bool Abort (ccs::types::uint32 uid);
    bool Discard (ccs::types::uint32 uid);

    void* GetAttribute (ccs::types::uint32 uid) const;
    bool GetStatus (ccs::types::uint32 uid) const;

    bool IsRunning (ccs::types::uint32 uid) const; 
    bool IsValid (ccs::types::uint32 uid) const;

    ccs::types::uint32 Launch (const std::function<bool(void*)>& cb, void* attr);

};

// Global variables

// Function declaration

// Function definition
  
DelegatedFunctionHandlerImpl::DPC_t DelegatedFunctionHandlerImpl::GetData (ccs::types::uint32 uid) const
{

  DelegatedFunctionHandlerImpl::DPC_t data;

  bool status = this->IsValid(uid);

  if (status)
    {
      data = __processes.find(uid)->second;
    }
  
  return data;
  
}

bool DelegatedFunctionHandlerImpl::SetData (ccs::types::uint32 uid, const DelegatedFunctionHandlerImpl::DPC_t& data)
{

  bool status = this->IsValid(uid);
  
  if (status)
    {
      __processes.find(uid)->second = data;
    }
  else
    {
      status = __processes.insert(std::pair<ccs::types::uint32,DPC_t>(uid,data)).second;
    }
  
  return status;
  
}
#if 1
bool DelegatedFunctionHandler::Abort (ccs::types::uint32 uid)
{

  bool status = false;

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      status = __impl->Abort(uid);
    }

  return status;

}

bool DelegatedFunctionHandlerImpl::Abort (ccs::types::uint32 uid)
{

  bool status = (this->IsValid(uid) && this->IsRunning(uid));

  if (status)
    {
      log_notice("DelegatedFunctionHandler::Abort - Execution '%u' is still running", uid);
      pthread_t thread = __handles.find(uid)->second;
      pthread_cancel(thread);
      // ToDo - Test that the thread disappeared
    }
  else // Invalid or not running .. ignore
    {
      status = true;
    }

  // No more reference to it
  this->Discard(uid);

  return status;

}
#endif
bool DelegatedFunctionHandler::Discard (ccs::types::uint32 uid)
{

  bool status = false;

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      status = __impl->Discard(uid);
    }

  return status;

}

bool DelegatedFunctionHandlerImpl::Discard (ccs::types::uint32 uid)
{

  bool status = this->IsValid(uid);

  if (status)
    {
      status = ((1 == __processes.erase(uid)) &&
		(1 == __handles.erase(uid)));      
    }

  return status;

}

bool DelegatedFunctionHandlerImpl::Execute (ccs::types::uint32 uid)
{

  log_trace("DelegatedFunctionHandler::Execute('%u') - Starting", uid);

  bool status = this->IsValid(uid);

  // Set thread name for test purposes
  char name [STRING_MAX_LENGTH]; snprintf(name, STRING_MAX_LENGTH, "Execute('%u')", uid);
  prctl(PR_SET_NAME, name, 0, 0, 0);

  if (status)
    {
      // See DelegatedFunctionHandler::Abort and DelegatedFunctionHandler::Launch 
      status = (0 == pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0));
    }

  if (status)
    {
      // Store native handle for future cancellation
      status = __handles.insert(std::pair<ccs::types::uint32,pthread_t>(uid,ccs::HelperTools::GetThreadId())).second;
    }

  if (status)
    {
      DelegatedFunctionHandlerImpl::DPC_t data = this->GetData(uid);

      log_debug("DelegatedFunctionHandler::Execute('%u') - Running", uid);

      data.running = true;

      // store for future use
      log_debug("DelegatedFunctionHandler::Execute('%u') - Replace", uid);
      this->SetData(uid, data);

      data.status = data.process(data.attribute);

      log_debug("DelegatedFunctionHandler::Execute('%u') - Finished", uid);

      data.running = false;

      // store for future use
      log_debug("DelegatedFunctionHandler::Execute('%u') - Replace", uid);
      this->SetData(uid, data);
     }

  return status;

}

void* DelegatedFunctionHandler::GetAttribute (ccs::types::uint32 uid) const
{

  void* attr = NULL;

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      attr = __impl->GetAttribute(uid);
    }

  return attr;

}

void* DelegatedFunctionHandlerImpl::GetAttribute (ccs::types::uint32 uid) const
{ 

  void* attribute = NULL; 

  bool status = this->IsValid(uid); 

  if (status) 
    { 
      DelegatedFunctionHandlerImpl::DPC_t data = this->GetData(uid); 
      attribute = data.attribute; 
    } 

  return attribute; 

}

bool DelegatedFunctionHandler::GetStatus (ccs::types::uint32 uid) const
{

  bool status = false;

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      status = __impl->GetStatus(uid);
    }

  return status;

}

bool DelegatedFunctionHandlerImpl::GetStatus (ccs::types::uint32 uid) const
{ 

  bool status = this->IsValid(uid); 

  if (status) 
    { 
      DelegatedFunctionHandlerImpl::DPC_t data = this->GetData(uid); 
      status = data.status; 
    } 

  return status; 

}

bool DelegatedFunctionHandler::IsRunning (ccs::types::uint32 uid) const
{

  bool status = false;

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      status = __impl->IsRunning(uid);
    }

  return status;

}

bool DelegatedFunctionHandlerImpl::IsRunning (ccs::types::uint32 uid) const
{ 

  bool running = false; 
  
  bool status = this->IsValid(uid); 
  
  if (status) 
    { 
      DelegatedFunctionHandlerImpl::DPC_t data = this->GetData(uid); 
      running = data.running; 
    }
  else
    {
      log_warning("DelegatedFunctionHandler::IsRunning('%u') - Invalid key", uid);
    }
#ifdef LOG_DEBUG_ENABLE
  if (running)
    {
      log_debug("DelegatedFunctionHandler::IsRunning('%u') - Still running", uid);
    }
  else
    {
      log_debug("DelegatedFunctionHandler::IsRunning('%u') - Stopped", uid);
    }
#endif
  return running; 
  
}
  
bool DelegatedFunctionHandler::IsValid (ccs::types::uint32 uid) const
{

  bool status = false;

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      status = __impl->IsValid(uid);
    }

  return status;

}

bool DelegatedFunctionHandlerImpl::IsValid (ccs::types::uint32 uid) const 
{ 

  return (__processes.end() != __processes.find(uid)); 

}

ccs::types::uint32 DelegatedFunctionHandler::Launch (const std::function<bool(void*)>& cb, void* attr)
{

  ccs::types::uint32 uid = 0u;

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      uid = __impl->Launch(cb, attr);
    }

  return uid;

}

ccs::types::uint32 DelegatedFunctionHandlerImpl::Launch (const std::function<bool(void*)>& cb, void* attr)
{

  ccs::types::uint32 uid = this->GenerateKey();
  
  DelegatedFunctionHandlerImpl::DPC_t data;
  
  data.running = false;
  data.status = false;
  data.process = cb;
  data.attribute = attr;
  
  bool status = (false == this->IsValid(uid));

  if (status)
    {
      // store for future use
      log_debug("DelegatedFunctionHandler::Launch - Store data for key '%u'", uid);
      status = this->SetData(uid, data);
    }

  if (status)
    {
      try // std::thread::detach may throw
	{
	  // Launch thread
	  using namespace std::placeholders;
	  std::thread thread (std::bind(&DelegatedFunctionHandlerImpl::Execute, this, _1), uid); thread.detach();
	} // std::thread goes out of scope here
      catch (std::exception& e)
	{
	  status = false;
	}
    }

  if (!status)
    {
      uid = 0u;
    }

  return uid;
  
}

DelegatedFunctionHandler::DelegatedFunctionHandler (void)
{

  __impl = new (std::nothrow) DelegatedFunctionHandlerImpl ();

  return;

}
  
DelegatedFunctionHandler::~DelegatedFunctionHandler (void)
{

  if (__builtin_expect((static_cast<DelegatedFunctionHandlerImpl*>(NULL) != __impl), 1)) // Likely
    {
      delete __impl;
      __impl = static_cast<DelegatedFunctionHandlerImpl*>(NULL);
    }

  return;

}
  
DelegatedFunctionHandlerImpl::~DelegatedFunctionHandlerImpl (void)
{
#if 0 // See DelegatedFunctionHandler::Abort .. Discard changes the element iterated upon 
  for (std::map<ccs::types::uint32,DelegatedFunctionHandlerImpl::DPC_t>::iterator it = __processes.begin(); it != __processes.end(); it++)
    {
      ccs::types::uint32 uid = it->first;
      this->Abort(uid); // Abort any running execution
    }
#else
  for (std::map<ccs::types::uint32,pthread_t>::iterator it = __handles.begin(); it != __handles.end(); ++it)
    {
      pthread_t thread = it->second;
      pthread_cancel(thread); // Abort any running execution
    }
#endif
  return;

}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC

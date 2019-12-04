/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/AnyThread.cpp $
* $Id: AnyThread.cpp 101628 2019-08-20 08:41:28Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <functional> // std::bind
#include <new> // std::nothrow
#include <thread> // std::thread

#include <sys/prctl.h> // PR_SET_NAME, etc.

// Local header files

#include "SysTools.h"
#include "TimeTools.h"

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE
#include "log-api.h" // Syslog wrapper routines

#include "AnyThread.h"
#include "AnyThreadImpl.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"

// Type definition

namespace ccs {

namespace base {

// Global variables

// Function declaration

// Function definition

// Initialiser methods

bool AnyThreadImpl::RegisterInterface (AnyThread_Base* base) { _base = base; return true; }

// Accessor methods

bool AnyThread_Base::IsRunning (void) const { return __impl->IsRunning(); }
bool AnyThread_Base::IsTerminating (void) const { return __impl->IsTerminating(); }

bool AnyThreadImpl::IsRunning (void) { return (true == __sync_val_compare_and_swap(&_running, true, true)); }
bool AnyThreadImpl::IsTerminating (void) { return (true == __sync_val_compare_and_swap(&_terminate, true, true)); }

bool AnyThread_Base::SetAffinity (ccs::types::uint32 core) { return __impl->SetAffinity(core); }
bool AnyThread_Base::SetPriority (ccs::types::uint32 priority, Policy policy) 
{ 

  ccs::types::uint32 __policy;

  bool status = true;

  if (policy == Default)
    {
      __policy = SCHED_OTHER;
    }
  else if (policy == FirstInFirstOut)
    {
      __policy = SCHED_FIFO;
    }
  else if (policy == RoundRobin)
    {
      __policy = SCHED_RR;
    }
  else
    {
      status = false;
    }

  if (status)
    {
      status = __impl->SetPriority(priority, __policy); 
    }

  return status; 

}

bool SynchronisedThreadWithCallback::SetPriority (ccs::types::uint32 policy, ccs::types::uint32 priority) 
{

  AnyThread_Base::Policy __policy;

  bool status = true;
  
  if (0u == policy)
    {
      __policy = AnyThread_Base::Policy::Default;
    }
  else if (1u == policy)
    {
      __policy = AnyThread_Base::Policy::FirstInFirstOut;
    }
  else if (2u == policy)
    {
      __policy = AnyThread_Base::Policy::RoundRobin;
    }
  else
    {
      status = false;
    }
  
  if (status)
    {
      status = this->AnyThread_Base::SetPriority(priority, __policy);
    }
  
  return status;
  
}

bool AnyThreadImpl::SetAffinity (ccs::types::uint32 core) 
{ 

  _affinity = core;

  bool status = ((NULL_PTR_CAST(std::thread*) != _thread) && 
                 (true == this->IsRunning()));

  if (status)
    {
      status = ((true == _thread->joinable()) && (0 != _thread->native_handle()));
    }

  if (status) 
    {
      status = ccs::HelperTools::SetAffinityToCore(_thread->native_handle(), _affinity);
    }

  return status; 

}

bool AnyThreadImpl::SetPriority (ccs::types::uint32 priority, ccs::types::uint32 policy)
{ 

  _policy = policy; 
  _priority = priority;

  bool status = ((NULL_PTR_CAST(std::thread*) != _thread) && 
                 (true == this->IsRunning()));

  if (status)
    {
      status = ((true == _thread->joinable()) && (0 != _thread->native_handle()));
    }

  if (status) 
    {
      status = ccs::HelperTools::SetPriority(_thread->native_handle(), _policy, _priority);
    }

  return status; 

}

bool SynchronisedThread::SetAccuracy (ccs::types::uint64 accuracy) { __accuracy = accuracy; return true; }
ccs::types::uint64 SynchronisedThread::GetAccuracy (void) const { return __accuracy; }
bool SynchronisedThread::SetPeriod (ccs::types::uint64 period) { __period = period; return true; }
ccs::types::uint64 SynchronisedThread::GetPeriod (void) const { return __period; }
bool SynchronisedThread::SetPhase (ccs::types::int64 phase) { __phase = phase; return true; }
bool SynchronisedThread::SetNextTime (ccs::types::uint64 time) { __till = time; return true; }

bool TriggeredThread::SetAccuracy (ccs::types::uint64 accuracy) { __accuracy = accuracy; return true; }

bool AnyThreadWithCallback::SetCallback (void (* cb)(void*), void * const attr)
{ 

  __exe.attr = attr; 
  __exe.cb = cb; 

  return true; 

}

bool AnyThreadWithCallback::SetPreamble (void (* cb)(void*), void * const attr)
{ 

  __pre.attr = attr; 
  __pre.cb = cb; 

  return true; 

}

bool AnyThreadWithCallback::SetPostface (void (* cb)(void*), void * const attr)
{ 

  __pos.attr = attr; 
  __pos.cb = cb; 

  return true; 

}

bool SynchronisedThreadWithCallback::SetSynchronous (bool flag) { (void)flag; return false; }

// Miscellaneous methods

void AnyThread_Base::Opening (void) { return; }
void AnyThread_Base::Sleep (void) { return; }
void AnyThread_Base::Closing (void) { return; }

void AnyThreadImpl::Execute (void)
{

  if (false == ccs::HelperTools::IsUndefinedString(_name))
    {
      log_debug("Starting '%s' thread", _name);
      log_debug("Run 'top -p %d', press <f>,<j>,<return> and <H> to show threads and CPU usage", ccs::HelperTools::GetProcessId());

      // Set thread name
      prctl(PR_SET_NAME, _name, 0, 0, 0);
    }

  // Call application-specific routine
  if (NULL_PTR_CAST(AnyThread_Base*) != _base)
    {
      _base->Opening();
    }

  _running = true; // AnyThread::Launch to return after preamble has returned

  // WARNING - Specialised classes must override the Execute method but may fail to call Terminate in their own destructor.
  //           As such, there is a risk thatthe thread sleeps while the specialised instance is destroyed and the Execute
  //           method reverts to being pure virtual.
  //           - Set interface reference to NULL before calling AnyThreadImpl::~AnyThreadImpl
  //           - Test before calling any interface method.

  while (false == __sync_val_compare_and_swap(&_terminate, false, false))
    {
      // Call application-specific routine
      if (NULL_PTR_CAST(AnyThread_Base*) != _base)
        {
          _base->Sleep();
        }

      if (NULL_PTR_CAST(AnyThread_Base*) != _base)
        {
          _base->Execute();
        }
    }

  // Call application-specific routine
  if (NULL_PTR_CAST(AnyThread_Base*) != _base)
    {
      _base->Closing();
    }

  _running = false;

  return;

}

void SynchronisedThread::Sleep (void) 
{

  ccs::HelperTools::WaitUntil(__till, DEFAULT_WAIT_UNTIL_SLEEP_LIMIT, __accuracy);

  __till += __period; 

  return;

}

void TriggeredThread::Sleep (void) 
{ 

  while ((false == AnyThread_Base::IsTerminating()) && (false == __sync_val_compare_and_swap(&__trigger, false, false))) 
    { 
      ccs::HelperTools::SleepFor(__accuracy); 
    } 

  // Reset
  (void)__sync_val_compare_and_swap(&__trigger, true, false); 

  return;

}

bool TriggeredThread::Trigger (void)
{ 

  bool status = AnyThread_Base::IsRunning(); 

  if (status) 
    { 
      status = (false == __sync_val_compare_and_swap(&__trigger, false, true));
    } 

  return status; 

}

bool AnyThread_Base::Launch (void) { return __impl->Launch(); }

bool SynchronisedThread::Launch (void)
{
  
  ccs::types::uint64 time = 0u;

  if (0u < __period)
    {
      time = static_cast<ccs::types::uint64>(ccs::HelperTools::GetCurrentTime()); 
      time = static_cast<ccs::types::uint64>(ccs::HelperTools::Ceil(time, __period)) + __period + __phase; 
    }

  (void)SetNextTime(time);

  bool status = AnyThread_Base::Launch(); 

  return status; 
}

bool AnyThreadImpl::Launch (void)
{

  bool status = ((NULL_PTR_CAST(std::thread*) == _thread) &&
                 (NULL_PTR_CAST(AnyThread_Base*) != _base));

  if (status)
    {
      _thread = new (std::nothrow) std::thread (std::bind(&AnyThreadImpl::Execute, this)); 
      status = (NULL_PTR_CAST(std::thread*) != _thread);
    }

  if (status)
    {
      // WARNING - Do not detach the thread .. SetAffinity, etc.
      while (true != this->IsRunning()) { ccs::HelperTools::SleepFor(10000000ul); };
    }

  return status;

}

bool AnyThread_Base::Terminate (void) { return __impl->Terminate(); }
bool AnyThreadImpl::Terminate (void)
{

  bool status = ((NULL_PTR_CAST(std::thread*) != _thread) && 
                 (true == this->IsRunning()));

  if (status)
    {
      // Notify termination request
      _terminate = true;

      if (true == _thread->joinable()) // Do not try and join a detached thread
        {
          log_debug("AnyThreadImpl::Terminate - Try and join thread ..");
          _thread->join(); 
        }
      else
        {
          while (true == this->IsRunning()) { ccs::HelperTools::SleepFor(10000000ul); };
        }

      status = (false == this->IsRunning());
    }

  if (status)
    {
      log_debug("AnyThreadImpl::Terminate - Try and delete thread ..");
      delete _thread; _thread = NULL_PTR_CAST(std::thread*);
      _terminate = false;
    }

  return status;

}

void AnyThreadWithCallback::CallPreamble (void) 
{ 

  if (NULL_PTR_CAST(void(*)(void*)) != __pre.cb) 
    {
      (*(__pre.cb))(__pre.attr);
    }

  return;

}

void AnyThreadWithCallback::CallExecute (void)
{ 

  if (NULL_PTR_CAST(void(*)(void*)) != __exe.cb)
    {
      (*(__exe.cb))(__exe.attr);
    }

  return;

}

void AnyThreadWithCallback::CallPostface (void)
{ 

  if (NULL_PTR_CAST(void(*)(void*)) != __pos.cb) 
    {
      (*(__pos.cb))(__pos.attr);
    }

  return;

}

void SynchronisedThreadWithCallback::Opening (void) { AnyThreadWithCallback::CallPreamble(); }
void SynchronisedThreadWithCallback::Execute (void) { AnyThreadWithCallback::CallExecute(); }
void SynchronisedThreadWithCallback::Closing (void) { AnyThreadWithCallback::CallPostface(); }

void TriggeredThreadWithCallback::Opening (void) { AnyThreadWithCallback::CallPreamble(); }
void TriggeredThreadWithCallback::Execute (void) { AnyThreadWithCallback::CallExecute(); }
void TriggeredThreadWithCallback::Closing (void) { AnyThreadWithCallback::CallPostface(); }

// Constructor methods

SynchronisedThread::SynchronisedThread (const ccs::types::char8 * const name) : AnyThread_Base(name)
{ 

  // Initialise attributes
  __till = 0ul; 
  __period = 1000000000ul; 
  __accuracy = 10000000ul; 
  __phase = 0l; 

  return;

}

TriggeredThread::TriggeredThread (const ccs::types::char8 * const name) : AnyThread_Base(name)
{ 

  // Initialise attributes
  __accuracy = 10000000ul; 
  __trigger = false; 

  return;

}

AnyThread_Base::AnyThread_Base (const ccs::types::char8 * const name)
{ 

  // Instantiate implementation class
  __impl = new (std::nothrow) AnyThreadImpl (name);

  bool status = (NULL_PTR_CAST(AnyThreadImpl*) != __impl);

  if (status)
    {
      __impl->RegisterInterface(this);
    }

  return; 

}

AnyThreadImpl::AnyThreadImpl (const ccs::types::char8 * const name)
{ 

  // Initialise attributes
  _affinity = 0u;
  _policy = 0u;
  _priority = 0u;

  _running = false; 
  _terminate = false;

  _base = NULL_PTR_CAST(AnyThread_Base*);
  _thread = NULL_PTR_CAST(std::thread*);

  ccs::HelperTools::SafeStringCopy(_name, name, ccs::types::MaxStringLength);

  return; 

}

AnyThreadWithCallback::AnyThreadWithCallback (void)
{

  __pre.cb = NULL_PTR_CAST(void (*)(void*)); 
  __pre.attr = NULL_PTR_CAST(void*); 

  __exe.cb = NULL_PTR_CAST(void (*)(void*)); 
  __exe.attr = NULL_PTR_CAST(void*); 

  __pos.cb = NULL_PTR_CAST(void (*)(void*)); 
  __pos.attr = NULL_PTR_CAST(void*); 

  return;

}

SynchronisedThreadWithCallback::SynchronisedThreadWithCallback (void) : SynchronisedThread(STRING_UNDEFINED), AnyThreadWithCallback() {}
SynchronisedThreadWithCallback::SynchronisedThreadWithCallback (const ccs::types::char8 * const name) : SynchronisedThread(name), AnyThreadWithCallback() {}
TriggeredThreadWithCallback::TriggeredThreadWithCallback (void) : TriggeredThread(STRING_UNDEFINED), AnyThreadWithCallback() {}
TriggeredThreadWithCallback::TriggeredThreadWithCallback (const ccs::types::char8 * const name) : TriggeredThread(name), AnyThreadWithCallback() {}

// Destructor method

SynchronisedThreadWithCallback::~SynchronisedThreadWithCallback (void) { (void)this->Terminate(); }
TriggeredThreadWithCallback::~TriggeredThreadWithCallback (void) { (void)this->Terminate(); }

AnyThread_Base::~AnyThread_Base (void)
{

  if (NULL_PTR_CAST(AnyThreadImpl*) != __impl)
    { 
      // Unregister interface .. do not risk calling pure virtual method in the thread
      __impl->RegisterInterface(NULL_PTR_CAST(AnyThread_Base*));

      delete __impl;
    }

  __impl = NULL_PTR_CAST(AnyThreadImpl*);

  return;

}

AnyThreadImpl::~AnyThreadImpl (void) 
{ 

  if (this->IsRunning())
    {
      (void)this->Terminate();
    }

  return;

}

} // namespace base

} // namespace ccs

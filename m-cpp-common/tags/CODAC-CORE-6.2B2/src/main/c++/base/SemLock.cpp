/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/SemLock.cpp $
* $Id: SemLock.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include <pthread.h>  // pthread_mutex_trylock, etc.

// Local header files

#include "BasicTypes.h"
#include "TimeTools.h"

#include "Lock.h"
#include "SemLock.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"

// Type definition

// Global variables

// Function declaration

// Function definition

namespace ccs {

namespace base {

bool SemLock::GetLock (void) const
{ 

  bool status = (0 != pthread_mutex_trylock(const_cast<pthread_mutex_t*>(&__mutex))); 

  if (!status) 
    {
      pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&__mutex)); // Release
    }

  return status; 

}

bool SemLock::AcquireLock (void) { return (0 == pthread_mutex_lock(&__mutex)); }
bool SemLock::AcquireLock (ccs::types::uint64 timeout)
{ 

  ccs::types::uint64 till_time = static_cast<ccs::types::uint64>(ccs::HelperTools::GetCurrentTime()) + timeout; 

  struct timespec at_most = ccs::HelperTools::ToTimespec(till_time);

  return (0 == pthread_mutex_timedlock(&__mutex, &at_most)); 

}

bool SemLock::ReleaseLock (void) { return (0 == pthread_mutex_unlock(&__mutex)); }
bool SemLock::TryLock (void) { return (0 == pthread_mutex_trylock(&__mutex)); }

SemLock& SemLock::operator= (const SemLock& lock)
{ 

  bool status = (this != &lock); 

  if (status) 
    { 
      if ((true == GetLock()) && (false == lock.GetLock()))
        {
          (void)ReleaseLock();
        }
      else if ((false == GetLock()) && (true == lock.GetLock()))
        { 
          (void)AcquireLock(); 
        }
      else
        {
          // Do nothing as locks are in identical states
        }
    } 

  return *this; 

}

SemLock::SemLock (void) : Lock()
{ 

  //pthread_mutex_init(&__mutex, NULL_PTR_CAST(pthread_mutexattr_t*)); 
  __mutex = PTHREAD_MUTEX_INITIALIZER;

  return; 

}

SemLock::SemLock (const SemLock& lock) : Lock()
{ 

  //pthread_mutex_init(&__mutex, NULL_PTR_CAST(pthread_mutexattr_t*)); 
  __mutex = PTHREAD_MUTEX_INITIALIZER;

  if (true == lock.GetLock())
    {
      (void)AcquireLock();
    }

  return; 

}

SemLock::~SemLock (void)
{ 

  // Take lock, or wait for lock to be released
  (void)AcquireLock();
  pthread_mutex_destroy(&__mutex);

  return; 

}

} // namespace base

} // namespace ccs

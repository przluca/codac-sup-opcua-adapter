/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/AtomicLock.cpp $
* $Id: AtomicLock.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

// Local header files

#include "BasicTypes.h"
#include "TimeTools.h"

#include "Lock.h"
#include "AtomicLock.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"

// Type definition

// Global variables

// Function declaration

// Function definition

namespace ccs {

namespace base {

bool AtomicLock::GetLock (void) const { return __lock; }
bool AtomicLock::AcquireLock (void)
{ 

  while (false == this->TryLock()) {} 

  return true; 

}

bool AtomicLock::AcquireLock (ccs::types::uint64 timeout)
{ 

  bool lock = false; 

  ccs::types::uint64 till_time = static_cast<ccs::types::uint64>(ccs::HelperTools::GetCurrentTime()) + timeout; 

  while (static_cast<ccs::types::uint64>(ccs::HelperTools::GetCurrentTime()) < till_time) 
    {
      lock = this->TryLock();

      if (true == lock) 
        { 
          break; 
        } 
    } 

  return lock; 

}

bool AtomicLock::ReleaseLock (void) { return __sync_val_compare_and_swap(&__lock, true, false); }
bool AtomicLock::TryLock (void) { return (false == __sync_val_compare_and_swap(&__lock, false, true)); }

AtomicLock& AtomicLock::operator= (const AtomicLock& lock)
{ 

  bool status = (this != &lock); 

  if (status) 
    { 
      __lock = lock.GetLock(); 
    } 

  return *this; 

}

AtomicLock::AtomicLock (void) : Lock() { __lock = false; return; }
AtomicLock::AtomicLock (const AtomicLock& lock) : Lock() { __lock = lock.GetLock(); return; }

AtomicLock::~AtomicLock (void)
{ 

  // Take lock, or wait for lock to be released
  (void)AcquireLock();
  __lock = false; 

  return; 

}

} // namespace base

} // namespace ccs
/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Lock-tests.cpp $
* $Id: Lock-tests.cpp 100896 2019-07-19 08:17:45Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Unit test code
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

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "TimeTools.h" // ccs::HelperTools::SleepFor, etc.

#include "log-api.h" // Syslog wrapper routines
#include "AnyThread.h" // Thread management class

#include "Lock.h"
#include "AtomicLock.h"
#include "SemLock.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition

void Thread_CB (ccs::base::Lock* self)
{

  if (true == self->AcquireLock(1000000000ul))
    {
      log_info("Thread_CB - Acquired lock at '%lu'", ccs::HelperTools::GetCurrentTime());
      self->ReleaseLock(); ccs::HelperTools::SleepFor(100000000ul);
    }
  else
    {
      log_warning("Thread_CB - Failed to acquire lock at '%lu'", ccs::HelperTools::GetCurrentTime());
    }

  return;

};

TEST(Lock_Test, AtomicLock_default)
{
  ccs::base::AtomicLock lock;

  bool ret = (false == lock.GetLock());

  if (ret)
    {
      ret = (true == lock.AcquireLock());
    }

  if (ret)
    {
      ret = (true == lock.GetLock());
    }

  if (ret)
    {
      ret = (false == lock.TryLock());
    }

  if (ret)
    {
      ret = (true == lock.ReleaseLock());
    }

  if (ret)
    {
      ret = (false == lock.GetLock());
    }

  if (ret)
    {
      ret = (true == lock.TryLock());
    }

  if (ret)
    {
      ret = (true == lock.ReleaseLock());
    }

  if (ret)
    {
      ret = (false == lock.GetLock());
    }

  ASSERT_EQ(true, ret);
}

TEST(Lock_Test, AtomicLock_copy)
{
  ccs::base::AtomicLock lock;

  bool ret = (false == lock.GetLock());

  if (ret)
    {
      log_info("TEST(Lock_Test, AtomicLock_copy) - Acquire lock ..");
      ret = (true == lock.AcquireLock());
    }

  if (ret)
    {
      ret = (true == lock.GetLock());
    }

  if (ret)
    {
      log_info("TEST(Lock_Test, AtomicLock_copy) - Copy constructor ..");
      ccs::base::AtomicLock copy (lock);
      ret = (true == copy.GetLock());

      if (ret)
	{
	  // WARNING - Release lock ahead of destruction
	  log_info("TEST(Lock_Test, AtomicLock_copy) - .. release ..");
	  copy.ReleaseLock();
	}

      log_info("TEST(Lock_Test, AtomicLock_copy) - .. going out of scope");
    }

  if (ret)
    {
      ccs::base::AtomicLock copy;
      ret = (false == copy.GetLock());

      if (ret)
	{
	  log_info("TEST(Lock_Test, AtomicLock_copy) - Copy assignment ..");
	  copy = lock;
	  ret = (true == copy.GetLock());
	}

      if (ret)
	{
	  // WARNING - Release lock ahead of destruction
	  log_info("TEST(Lock_Test, AtomicLock_copy) - .. release ..");
	  copy.ReleaseLock();
	}

      log_info("TEST(Lock_Test, AtomicLock_copy) - .. going out of scope");
    }

  if (ret)
    {
      ret = (true == lock.ReleaseLock());
    }

  ASSERT_EQ(true, ret);
}

TEST(Lock_Test, SemLock_default)
{
  ccs::base::SemLock lock;

  bool ret = (false == lock.GetLock());

  if (ret)
    {
      ret = (true == lock.AcquireLock());
    }

  if (ret)
    {
      ret = (true == lock.GetLock());
    }

  if (ret)
    {
      ret = (false == lock.TryLock());
    }

  if (ret)
    {
      ret = (true == lock.ReleaseLock());
    }

  if (ret)
    {
      ret = (false == lock.GetLock());
    }

  if (ret)
    {
      ret = (true == lock.TryLock());
    }

  if (ret)
    {
      ret = (true == lock.ReleaseLock());
    }

  if (ret)
    {
      ret = (false == lock.GetLock());
    }

  ASSERT_EQ(true, ret);
}

TEST(Lock_Test, SemLock_copy)
{
  ccs::base::SemLock lock;

  bool ret = (false == lock.GetLock());

  if (ret)
    {
      log_info("TEST(Lock_Test, SemLock_copy) - Acquire lock ..");
      ret = (true == lock.AcquireLock());
    }

  if (ret)
    {
      ret = (true == lock.GetLock());
    }

  if (ret)
    {
      log_info("TEST(Lock_Test, SemLock_copy) - Copy constructor ..");
      ccs::base::SemLock copy (lock);
      ret = (true == copy.GetLock());

      if (ret)
	{
	  // WARNING - Release lock ahead of destruction
	  log_info("TEST(Lock_Test, SemLock_copy) - .. release ..");
	  copy.ReleaseLock();
	}

      log_info("TEST(Lock_Test, SemLock_copy) - .. going out of scope");
    }

  if (ret)
    {
      ccs::base::SemLock copy;
      ret = (false == copy.GetLock());

      if (ret)
	{
	  log_info("TEST(Lock_Test, SemLock_copy) - Copy assignment ..");
	  copy = lock;
	  ret = (true == copy.GetLock());
	}

      if (ret)
	{
	  // WARNING - Release lock ahead of destruction
	  log_info("TEST(Lock_Test, SemLock_copy) - .. release ..");
	  copy.ReleaseLock();
	}

      log_info("TEST(Lock_Test, SemLock_copy) - .. going out of scope");
    }

  if (ret)
    {
      ret = (true == lock.ReleaseLock());
    }

  ASSERT_EQ(true, ret);
}

TEST(Lock_Test, AtomicLock_thread)
{
  ccs::base::AtomicLock lock;

  bool ret = (true == lock.AcquireLock());

  ccs::base::SynchronisedThreadWithCallback thread; 

  if (ret)
    {
      thread.SetPeriod(0ul); 
      thread.SetCallback((void (*) (void*)) &Thread_CB, (void*) &lock); 
      ret = thread.Launch();
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(10,0);
      ret = lock.ReleaseLock();
    }

  ccs::HelperTools::SleepFor(1,0); 

  if (ret)
    {
      ret = thread.Terminate();
    }

  ASSERT_EQ(true, ret);
}

TEST(Lock_Test, SemLock_thread)
{
  ccs::base::SemLock lock;

  bool ret = (true == lock.AcquireLock());

  ccs::base::SynchronisedThreadWithCallback thread; 

  if (ret)
    {
      thread.SetPeriod(0ul); 
      thread.SetCallback((void (*) (void*)) &Thread_CB, (void*) &lock); 
      ret = thread.Launch();
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(10,0);
      ret = lock.ReleaseLock();
    }

  ccs::HelperTools::SleepFor(1,0); 

  ASSERT_EQ(true, ret);
}


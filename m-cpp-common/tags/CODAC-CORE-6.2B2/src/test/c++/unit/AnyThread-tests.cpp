/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/AnyThread-tests.cpp $
* $Id: AnyThread-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include <stdio.h> // sscanf, printf, etc.
#include <string.h> // strncpy, etc.
#include <stdarg.h> // va_start, etc.
#include <signal.h> // sigset, etc.

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "types.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "AnyThread.h"

// Constants

// Type definition

class AnyThread_Test : public ccs::base::AnyThread_Base // Need to specialise abstract class
{

  private:

    bool _preamble;
    bool _execute;
    bool _postface;

  public:

    AnyThread_Test () : AnyThread_Base ("AnyThread_Test") { _preamble = false; _execute = false; _postface = false; };
    virtual ~AnyThread_Test () {};

    virtual void Opening (void) { _preamble = true; };
    virtual void Execute (void) { _execute = true; };
    virtual void Closing (void) { _postface = true; };
    virtual void Sleep (void) { ccs::HelperTools::SleepFor(10000000ul); }; // Avoid spinng thread in unit tests

    bool HasRun (void) { return _preamble && _execute && _postface; };

  protected:

};

class SynchronisedThread_Test : public ccs::base::SynchronisedThread // Need to specialise abstract class
{

  private:

    ccs::types::uint32 _count;

  public:

    SynchronisedThread_Test () : ccs::base::SynchronisedThread("SynchronisedThread_Test") { _count = 0u; };
    virtual ~SynchronisedThread_Test () { Terminate(); };

    virtual void Execute (void) { log_info("SynchronisedThread::Execute - Method called at '%lu'", ccs::HelperTools::GetCurrentTime()); _count += 1u; };

    bool HasRun (void) { return (0u < _count); };

  protected:

};

class TriggeredThread_Test : public ccs::base::TriggeredThread // Need to specialise abstract class
{

  private:

    bool _execute;

  public:

    TriggeredThread_Test () : ccs::base::TriggeredThread("TriggeredThread_Test") { _execute = false; };
    virtual ~TriggeredThread_Test () { Terminate(); };

    virtual void Execute (void) { log_info("TriggeredThread::Execute - Method called"); _execute = true; };

    bool HasRun (void) { return _execute; };

  protected:

};

// Global variables

// Function declaration

void AnyThread_CB (void* dummy);

// Function definition
  
void AnyThread_PRBL (void* dummy)
{
  log_info("AnyThread_PRBL - Function called at '%lu'", ccs::HelperTools::GetCurrentTime());
}

void AnyThread_CB (void* dummy)
{
  log_info("AnyThread_CB - Function called at '%lu'", ccs::HelperTools::GetCurrentTime());
}

void AnyThread_POST (void* dummy)
{
  log_info("AnyThread_POST - Function called at '%lu'", ccs::HelperTools::GetCurrentTime());
}

TEST(AnyThread_Test, Constructor_default)
{
  ccs::base::AnyThread_Base* thread = new (std::nothrow) AnyThread_Test;

  bool ret = (thread != static_cast<ccs::base::AnyThread_Base*>(NULL));

  if (ret)
    {
      delete thread;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyThread_Test, Launch)
{
  ccs::base::AnyThread_Base* thread = new (std::nothrow) AnyThread_Test;

  bool ret = (thread != static_cast<ccs::base::AnyThread_Base*>(NULL));

  if (ret)
    {
#if 0 // Exceptions if any are caught in the wrapper class
      try
	{
	  log_info("TEST(AnyThread_Test, Launch) - Launch ..");
	  ret = thread->Launch();
	}
      catch (const std::exception& e)
	{
	  log_error("TEST(AnyThread_Test, Launch) - .. '%s' exception", e.what());
	  ret = false;
	}
      catch (...)
	{
	  log_error("TEST(AnyThread_Test, Launch) - .. undefined exception");
	  ret = false;
	}
#else
      log_info("TEST(AnyThread_Test, Launch) - Launch ..");
      ret = thread->Launch();
#endif
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (ret)
    {
      ret = thread->IsRunning();
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, Launch) - Terminate ..");
      ret = thread->Terminate();
    }

  if (ret)
    {
      ret = !(thread->IsRunning());
    }

  if (ret)
    {
      ret = dynamic_cast<AnyThread_Test*>(thread)->HasRun();
    }

  if (ret)
    {
      delete thread;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyThread_Test, SetAffinity)
{
  ccs::base::AnyThread_Base* thread = new (std::nothrow) AnyThread_Test;

  bool ret = (thread != static_cast<ccs::base::AnyThread_Base*>(NULL));

  if (ret)
    {
      ret = thread->Launch();
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (ret)
    {
      ret = thread->IsRunning();
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, SetAffinity) - Check that the threads exist ..");
      // Compose system command
      std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
      
      ret = (std::system(command.c_str()) == 0);
      
      std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
      std::string result = sstr.str();
      std::cout << std::endl << result << std::endl;
    }
  
  if (ret)
    {
      log_info("TEST(AnyThread_Test, SetAffinity) - Set affinity ..");
      ret = thread->SetAffinity(1);
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, SetAffinity) - Terminate ..");
      ret = thread->Terminate();
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, SetAffinity) - Set affinity ..");
      ret = !thread->SetAffinity(1); // Expect failure
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, SetAffinity) - Delete ..");
      delete thread;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyThread_Test, SetPriority)
{
  ccs::base::AnyThread_Base* thread = new (std::nothrow) AnyThread_Test;

  bool ret = (thread != static_cast<ccs::base::AnyThread_Base*>(NULL));

  if (ret)
    {
      ret = thread->Launch();
    }

  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (ret)
    {
      ret = thread->IsRunning();
    }

  
  if (ret)
    {
      log_info("TEST(AnyThread_Test, SetPriority) - Set priority ..");
      ret = thread->SetPriority(80u, ccs::base::AnyThread_Base::Policy::RoundRobin);

      if (!ret)
	{
	  log_notice("TEST(AnyThread_Test, SetPriority) - .. failure");
	  ret = thread->SetPriority(0u); // Without RT policy
	}

      if (!ret)
	{
	  ret = true; // Ignore
	}
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, SetPriority) - Delete ..");
      delete thread;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyThread_Test, Synchronise)
{
  ccs::base::SynchronisedThread* thread = new (std::nothrow) SynchronisedThread_Test;

  bool ret = (thread != static_cast<ccs::base::SynchronisedThread*>(NULL));

  if (ret)
    {
      ret = (thread->SetAccuracy(10000ul) &&
	     thread->SetPeriod(1000000000ul) && // 1Hz
	     thread->SetPhase(-500000000l));
    }

  if (ret)
    {
      ret = ((10000ul == thread->GetAccuracy()) &&
	     (1000000000ul == thread->GetPeriod()));
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, Synchronise) - Launch ..");
      ret = thread->Launch();
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, Synchronise) - Check that the threads exist ..");
      // Compose system command
      std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
      
      ret = (std::system(command.c_str()) == 0);
      
      std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
      std::string result = sstr.str();
      std::cout << std::endl << result << std::endl;
    }
  
  if (ret)
    {
      ccs::HelperTools::SleepFor(50000000ul);
    }

  if (ret)
    {
      ret = (thread->IsRunning() && !dynamic_cast<SynchronisedThread_Test*>(thread)->HasRun());
    }
  
  if (ret)
    {
      log_info("TEST(AnyThread_Test, Synchronise) - Sleep ..");
      ccs::HelperTools::SleepFor(1000000000ul);
    }

  if (ret)
    {
      ret = (thread->IsRunning() && dynamic_cast<SynchronisedThread_Test*>(thread)->HasRun());
    }
  
  if (ret)
    {
      ret = (thread->SetAccuracy(1000000000ul) &&
	     thread->SetPeriod(1000000000ul) && // 1Hz
	     thread->SetPhase(-500000000l));
    }

  if (ret)
    {
      ret = ((1000000000ul == thread->GetAccuracy()) &&
	     (1000000000ul == thread->GetPeriod()));
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, Synchronise) - Sleep ..");
      ccs::HelperTools::SleepFor(1000000000ul);
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, Synchronise) - Check that the threads runs with zero CPU load ..");
      // Compose system command
      std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
      
      ret = (std::system(command.c_str()) == 0);
      
      std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
      std::string result = sstr.str();
      std::cout << std::endl << result << std::endl;
    }
  
  if (ret)
    {
      log_info("TEST(AnyThread_Test, Synchronise) - Delete ..");
      delete thread;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyThread_Test, Trigger)
{
  ccs::base::TriggeredThread* thread = new (std::nothrow) TriggeredThread_Test;

  bool ret = (thread != static_cast<ccs::base::TriggeredThread*>(NULL));

  if (ret)
    {
      ret = thread->SetAccuracy(1000000ul);
    }

  if (ret)
    {
      ret = thread->Launch();
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, Trigger) - Check that the threads exist ..");
      // Compose system command
      std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
      
      ret = (std::system(command.c_str()) == 0);
      
      std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
      std::string result = sstr.str();
      std::cout << std::endl << result << std::endl;
    }
  
  if (ret)
    {
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (ret)
    {
      ret = (thread->IsRunning() && !dynamic_cast<TriggeredThread_Test*>(thread)->HasRun());
    }
  
  if (ret)
    {
      log_info("TEST(AnyThread_Test, Trigger) - Trigger ..");
      ret = thread->Trigger();
    }
  
  if (ret)
    {
      log_info("TEST(AnyThread_Test, Trigger) - Sleep ..");
      ccs::HelperTools::SleepFor(100000000ul);
    }

  if (ret)
    {
      ret = (thread->IsRunning() && dynamic_cast<TriggeredThread_Test*>(thread)->HasRun());
    }
  
  if (ret)
    {
      log_info("TEST(AnyThread_Test, Trigger) - Delete ..");
      delete thread;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyThread_Test, Legacy)
{
  ccs::base::AnyThread* thread = new (std::nothrow) ccs::base::AnyThread ("LegacyThread");

  bool ret = (thread != static_cast<ccs::base::AnyThread*>(NULL));

  if (ret)
    {
      ret = !thread->SetSynchronous(false); // Expect failure .. deprecated API
    }

  if (ret)
    {
      ret = thread->SetAccuracy(1000000ul);
    }

  if (ret)
    {
      ret = thread->SetPriority(0u, 0u);

      if (!ret)
	{ // Ignore
	  ret = true;
	}
    }

  if (ret)
    {
      ret = thread->SetPriority(1u, 0u);

      if (!ret)
	{ // Ignore
	  ret = true;
	}
    }

  if (ret)
    {
      ret = thread->SetPriority(2u, 0u);

      if (!ret)
	{ // Ignore
	  ret = true;
	}
    }

  if (ret)
    {
      ret = !thread->SetPriority(5u, 0u); // Expect failure
    }

  if (ret)
    {
      ret = thread->SetCallback(&AnyThread_CB);
    }

  if (ret)
    {
      ret = thread->SetPreamble(&AnyThread_PRBL);
    }

  if (ret)
    {
      ret = thread->SetPostface(&AnyThread_POST);
    }

  if (ret)
    {
      ret = thread->Launch();
    }

  if (ret)
    {
      log_info("TEST(AnyThread_Test, Trigger) - Check that the threads exist ..");
      // Compose system command
      std::string command = std::string("/usr/bin/top -bH -n 1 -p `/usr/sbin/pidof unit-tests` > /tmp/top.out");
      
      ret = (std::system(command.c_str()) == 0);
      
      std::stringstream sstr; sstr << std::ifstream("/tmp/top.out").rdbuf();
      std::string result = sstr.str();
      std::cout << std::endl << result << std::endl;
    }
  
  if (ret)
    {
      delete thread;
    }

  ASSERT_EQ(true, ret);
}
#if 0 // Verify std::thread memory leak reported by valgrind
TEST(AnyThread_Test, Memory)
{
  bool ret = true;

  ccs::types::uint32 before = ccs::HelperTools::GetProcessMemory();

  log_info("TEST(AnyThread_Test, Memory) - Currently used memory '%u' ..", before);

  for (ccs::types::uint32 index = 0u; index < 1000; index += 1u)
    {
      ccs::types::uint32 memory = ccs::HelperTools::GetProcessMemory();
      log_info("TEST(AnyThread_Test, Memory) - .. memory '%u' at iteration '%u' ..", memory, index);
      ccs::base::AnyThread thread ("MemoryThread");
      thread.SetCallback(&AnyThread_CB);
      thread.SetAccuracy(10000000ul);
      thread.SetPeriod(200000000ul) && // 5Hz
      thread.Launch();
      ccs::HelperTools::SleepFor(1000000000ul);
      thread.Terminate();
    }

  ccs::types::uint32 final = ccs::HelperTools::GetProcessMemory();

  log_info("TEST(AnyThread_Test, Memory) - .. final '%u'", final);

  ASSERT_EQ(true, ret);
}
#endif
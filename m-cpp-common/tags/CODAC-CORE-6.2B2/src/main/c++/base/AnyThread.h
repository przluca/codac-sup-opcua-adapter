/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/AnyThread.h $
* $Id: AnyThread.h 100890 2019-07-19 07:14:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Thread root class definition
*
* Author        : Bertrand Bauvir (IO)
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

/**
 * @file AnyThread.h
 * @brief Header file for AnyThread class
 * @date 09/01/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyThread class.
 */

#ifndef _AnyThread_h_
#define _AnyThread_h_

// Global header files

// Local header files

#include "BasicTypes.h"

// Constants

// Type definition

namespace ccs {

namespace base {

class AnyThreadImpl; // Forward class declaration

/**
 * @brief The class provides thread management interface.
 * @todo Catch exceptions, if any, in callbacks or overloaded methods.
 * @todo Refactor with AnyThreadWithCallback class sharing commonality.
 */

class AnyThread_Base
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    AnyThreadImpl* __impl;

    AnyThread_Base (void); // Undefined
    AnyThread_Base (const AnyThread_Base& thread); // Undefined
    AnyThread_Base& operator= (const AnyThread_Base& thread); // Undefined

  protected:

    /**
     * @brief Constructor. Instantiates implementation class.
     * @post
     *   __impl != static_cast<AnyThreadImpl*>(NULL)
     */

    explicit AnyThread_Base (const ccs::types::char8 * const name);

  public:

    /**
     * @brief Destructor. Terminates thread, if necessary.
     * @post
     *   __impl = static_cast<AnyThreadImpl*>(NULL)
     * @note public destructor to allow for thread factories to delete from base class.
     */

    virtual ~AnyThread_Base (void);

    /**
     * @brief Accessor. Tests if thread is running.
     * @return TRUE if running.
     */

    bool IsRunning (void) const;

    /**
     * @brief Accessor. Tests if termination has been requested.
     * @return TRUE if termination ongoing.
     */

    bool IsTerminating (void) const;

    /**
     * @brief Launch method.
     * @detail The background thread calls the virtual AnyThread::Sleep and AnyThread::Execute
     * methods indefinitely, till AnyThread::Terminate is called or the instance is destroyed.
     * @return TRUE if running.
     * @code
       class UserThread : public ccs::base::AnyThread {
         public:
           UserThread (void) : ccs::base::AnyThread("UserThread") {
             // Start thread at instantiation
             this->ccs::base::AnyThread::Launch();
           };
          
           virtual ~UserThread (void) {
             // Terminate thread in destructor
             this->ccs::base::AnyThread::Terminate();
           };
          
           // Etc.
       };
       @endcode
     */

    virtual bool Launch (void);

    /**
     * @brief Terminate method.
     * @return TRUE if not running anymore.
     */

    virtual bool Terminate (void);

    enum Policy { // See /usr/include/bits/sched.h

      Default = 0,     // SCHED_OTHER 
      FirstInFirstOut, // SCHED_FIFO
      RoundRobin,      // SCHED_RR

    };

    /**
     * @brief Accessor. Set affinity to specified core.
     * @see ccs::HelperTools::SetAffinityToCore.
     * @return TRUE if successful.
     */

    bool SetAffinity (ccs::types::uint32 core);

    /**
     * @brief Accessor. Set priority and (optional) policy.
     * @see ccs::HelperTools::SetPriority.
     * @return TRUE if successful.
     * @warning May require process with elevated priviledges.
     */

    bool SetPriority (ccs::types::uint32 priority, Policy policy = Default);

    /**
     * @brief Virtual method. NOOP.
     * @detail Method may be overloaded to provide implementation-specific behaviour before entering
     * the indefinite loop.
     * @post
     *   IsRunning() = true
     */

    virtual void Opening (void); // Virtual method .. Can be overloaded in specialised implementations

    /**
     * @brief Virtual method. NOOP.
     * @detail Method may be overloaded to provide implementation-specific pacing of the background thread.
     */

    virtual void Sleep (void); // Virtual method .. Can be overloaded in specialised implementations

    /**
     * @brief Pure virtual method. NOOP.
     * @detail Method must be overloaded to provide implementation-specific behaviour in the body of the background thread.
     * @code
       class UserThread : public ccs::base::AnyThread {
         public:
           UserThread (void) : ccs::base::AnyThread("UserThread") {
             // Start thread at instantiation
             this->ccs::base::AnyThread::Launch();
           };
          
           virtual ~UserThread (void) {
             // Terminate thread in destructor
             this->ccs::base::AnyThread::Terminate();
           };
          
           // Etc.
           virtual void Sleep (void) {
             ccs::HelperTools::SleepFor(100000000ul);
           };

           virtual void Execute (void) {
             // Do Background activity
             log_info("UserThread::Execute - Running ..");
           };
       };
       @endcode
     */

    virtual void Execute (void) = 0; // Pure virtual method .. Must be overloaded in specialised implementations

    /**
     * @brief Virtual method. NOOP.
     * @detail Method may be overloaded to provide implementation-specific behaviour exiting
     * the indefinite loop.
     * @post
     *   IsRunning() = false
     */

    virtual void Closing (void); // Virtual method .. Can be overloaded in specialised implementations

};

/**
 * @brief The class provides time-synchronised thread management interface.
 * @detail The AnyThread::Execute method is called with the specified period
 * and synchronised to ITER time with the specified phase.
 * @see ccs::HelperTools::WaitUntil.
 */

class SynchronisedThread : public AnyThread_Base
{

  private:

    ccs::types::uint64 __till;
    ccs::types::uint64 __period;
    ccs::types::uint64 __accuracy; // Bug 11757 - TCNd consumes 10% CPU time .. change default to 10ms
    ccs::types::int64 __phase;

    SynchronisedThread (void); // Undefined
    SynchronisedThread (const SynchronisedThread& thread); // Undefined
    SynchronisedThread& operator= (const SynchronisedThread& thread); // Undefined

  protected:

    /**
     * @brief Constructor. Instantiates implementation class.
     */

    explicit SynchronisedThread (const ccs::types::char8 * const name);

  public:

    /**
     * @brief Destructor. Terminates thread, if necessary.
     * @note public destructor to allow for thread factories to delete from base class.
     */

    virtual ~SynchronisedThread (void) {};

    /**
     * @brief Accessor.
     * @detail Sets accuracy. See ccs::HelperTools::WaitUntil.
     * @return TRUE if successful.
     */

    bool SetAccuracy (ccs::types::uint64 accuracy);
    ccs::types::uint64 GetAccuracy (void) const;

    /**
     * @brief Accessor.
     * @detail Sets period. The thread will run at the specified period performing a call to ccs::HelperTools::WaitUntil.
     * The thread Execute method is being called with a specified period and phase aligned to ITER time.
     * @return TRUE if successful.
     */

    bool SetPeriod (ccs::types::uint64 period);
    ccs::types::uint64 GetPeriod (void) const;

    /**
     * @brief Accessor.
     * @detail Sets phase. The thread will run at the specified period performing a call to ccs::HelperTools::WaitUntil.
     * The thread Execute method is being called with a specified period and phase aligned to ITER time.
     * @return TRUE if successful.
     */

    bool SetPhase (ccs::types::int64 phase);

    /**
     * @brief Accessor.
     * @detail Sets next execution time. The thread will run till the specified time performing a call to ccs::HelperTools::WaitUntil.
     * @return TRUE if successful.
     */

    bool SetNextTime (ccs::types::uint64 time);

    /**
     * @brief Virtual method overload. See ccs::base::AnyThread::Launch.
     * @detail Sets the next execution time.
     * @post
     *   __till = ccs::HelperTools::Ceil(ccs::HelperTools::GetCurrentTime(), __period) + __phase
     */

    virtual bool Launch (void);

    /**
     * @brief Virtual method overload. See ccs::base::AnyThread::Sleep.
     * @detail Waits till the specified run time is elapsed.
     * @see ccs::HelperTools::WaitUntil.
     */

    virtual void Sleep (void); // Specialises virtual method

};

/**
 * @brief The class provides triggered thread management interface.
 * @detail The AnyThread::Execute method is called after the TriggeredThread::Trigger
 * method is called. The TriggeredThread::Sleep method is spinning with a configurable
 * sleep which can be changed using the TriggeredThread::SetAccuracy method.
 */

class TriggeredThread : public AnyThread_Base
{

  private:

    ccs::types::uint64 __accuracy; // Do not consume 100% CPU load by default
    volatile bool __trigger;

    TriggeredThread (void); // Undefined
    TriggeredThread (const TriggeredThread& thread); // Undefined
    TriggeredThread& operator= (const TriggeredThread& thread); // Undefined

  protected:

    /**
     * @brief Constructor. Instantiates implementation class.
     * @post
     *   __trigger = false
     */

    explicit TriggeredThread (const ccs::types::char8 * const name);

  public:

    /**
     * @brief Destructor. Terminates thread, if necessary.
     * @note public destructor to allow for thread factories to delete from base class.
     */

    virtual ~TriggeredThread (void) {};

    /**
     * @brief Accessor.
     * @detail Sets accuracy, i.e. allowed time to trigger Execute method. Set to 0 to
     * have the background thread spin-locking on the atomic trigger.
     * @return TRUE if successful.
     */

    bool SetAccuracy (ccs::types::uint64 accuracy);

    /**
     * @brief Trigger method.
     * @return TRUE if successful.
     */

    bool Trigger (void);

    /**
     * @brief Virtual method overload. See ccs::base::AnyThread::Sleep.
     * @detail Tests atomic trigger in a loop till Terminate or Trigger is called. The method performs
     * a sleep with the specified accuracy.
     */

    virtual void Sleep (void); // Specialises virtual method

};

/**
 * @brief The class provides callback support for thread management.
 * @detail The class provides synchronised thread support favouring composition over
 * specialisation.
 */

class AnyThreadWithCallback
{

  private:

    struct Callback {

      void (* cb) (void*); // Routine
      void* attr;          //      .. attribute

    };

    Callback __pre;
    Callback __exe;
    Callback __pos;

    AnyThreadWithCallback (const AnyThreadWithCallback& thread); // Undefined
    AnyThreadWithCallback& operator= (const AnyThreadWithCallback& thread); // Undefined

  public:

    /**
     * @brief Constructor. NOOP.
     */

    AnyThreadWithCallback (void);

    /**
     * @brief Destructor. NOOP
     */

    virtual ~AnyThreadWithCallback (void) {};

    /**
     * @brief Accessor. Provides user-defined callback.
     */

    bool SetCallback (void (* cb)(void*), void * const attr = 0);
    bool SetPreamble (void (* cb)(void*), void * const attr = 0);
    bool SetPostface (void (* cb)(void*), void * const attr = 0);

    /**
     * @brief Invokes the registered callbacks, if any.
     */

    void CallPreamble (void);
    void CallExecute (void);
    void CallPostface (void);

};

/**
 * @brief The class provides synchronised thread management interface and callbacks.
 * @detail The class provides synchronised thread support favouring composition over
 * specialisation.
 *
 * The class can be instantiated without specialisation where user-defined behaviour
 * is supplied through callback functions.
 *
 * The class aims at replacing the major use-case for the legacy ccs::base::AnyThread
 * implementation.
 */

typedef class SynchronisedThreadWithCallback : public SynchronisedThread, public AnyThreadWithCallback
{

  private:

    SynchronisedThreadWithCallback (const SynchronisedThreadWithCallback& thread); // Undefined
    SynchronisedThreadWithCallback& operator= (const SynchronisedThreadWithCallback& thread); // Undefined

  public:

    /**
     * @brief Constructor. Instantiates implementation class.
     */

    SynchronisedThreadWithCallback (void);
    explicit SynchronisedThreadWithCallback (const ccs::types::char8 * const name);

    /**
     * @brief Destructor. Terminates thread, if necessary.
     */

    virtual ~SynchronisedThreadWithCallback (void);

    /**
     * @brief Accessor. Obsolete API. 
     * @return FALSE in any case.
     */

    bool SetSynchronous (bool flag);

    /**
     * @brief Accessor. Obsolete.
     * @detail Conversion to AnyThread::SetPriority.
     * @return TRUE if successful.
     */

    bool SetPriority (ccs::types::uint32 policy = 0u, ccs::types::uint32 priority = 0u);

    /**
     * @brief Virtual method overload. See ccs::base::AnyThread::Opening.
     */

    virtual void Opening (void); // Specialises virtual method
    virtual void Execute (void); // Specialises virtual method
    virtual void Closing (void); // Specialises virtual method

} AnyThread __attribute__((deprecated)); // Backward compatibility

/**
 * @brief The class provides triggered thread management interface and callbacks.
 * @detail The class provides triggered thread support favouring composition over
 * specialisation.
 *
 * The class can be instantiated without specialisation where user-defined behaviour
 * is supplied through callback functions.
 */

class TriggeredThreadWithCallback : public TriggeredThread, public AnyThreadWithCallback
{

  private:

    TriggeredThreadWithCallback (const TriggeredThreadWithCallback& thread); // Undefined
    TriggeredThreadWithCallback& operator= (const TriggeredThreadWithCallback& thread); // Undefined

  public:

    /**
     * @brief Constructor. Instantiates implementation class.
     */

    TriggeredThreadWithCallback (void);
    explicit TriggeredThreadWithCallback (const ccs::types::char8 * const name);

    /**
     * @brief Destructor. Terminates thread, if necessary.
     */

    virtual ~TriggeredThreadWithCallback (void);

    /**
     * @brief Virtual method overload. See ccs::base::AnyThread::Opening.
     */

    virtual void Opening (void); // Specialises virtual method
    virtual void Execute (void); // Specialises virtual method
    virtual void Closing (void); // Specialises virtual method

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _AnyThread_h_


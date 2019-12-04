/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/SemLock.h $
* $Id: SemLock.h 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Lock class definition
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
 * @file SemLock.h
 * @brief Header file for SemLock class
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the SemLock class.
 */

#ifndef _SemLock_h_
#define _SemLock_h_

// Global header files

#include <pthread.h>  // pthread_mutex_trylock, etc.

// Local header files

#include "BasicTypes.h"

#include "Lock.h" // Interface class definition

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides mutex-based locking mechanism.
 */

class SemLock : public Lock
{ 

  private:

    pthread_mutex_t __mutex; // Mutex semaphore

  protected:

  public:

    /**
     * @brief Constructor.
     */

    SemLock (void);

    /**
     * @brief Copy constructor.
     */

    SemLock (const SemLock& lock);

    /**
     * @brief Destructor.
     * @detail The destructor waits till the lock is released.
     */

    virtual ~SemLock (void);

    /**
     * @brief Accessor. 
     * @return true if locked.
     */

    virtual bool GetLock (void) const; // Test lock state
        
    /**
     * @brief AcquireLock method.
     * @detail The method acquires the lock, it blocks if necessary till the lock is released.
     * @return true if locked.
     */

    virtual bool AcquireLock (void);
        
    /**
     * @brief AcquireLock method.
     * @detail The method acquires the lock and blocks at most <timeout> nsec.
     * @return true if locked within <timeout> nsec.
     */

    virtual bool AcquireLock (ccs::types::uint64 timeout);
        
    /**
     * @brief ReleaseLock method.
     * @detail The method releases the lock.
     * @return true if released.
     */

    virtual bool ReleaseLock (void);

    /**
     * @brief TryLock method.
     * @detail The method tests the lock and acquires it if available. The method corresponds to a 
     * non-blocking AcquireLock.
     * @return true if locked.
     */

    virtual bool TryLock (void);

    /**
     * @brief Copy assignment operator.
     */

    SemLock& operator= (const SemLock& lock);

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _SemLock_h_


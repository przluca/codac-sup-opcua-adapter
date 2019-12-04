/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/AtomicLock.h $
* $Id: AtomicLock.h 100703 2019-07-11 15:23:56Z bauvirb $
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
 * @file AtomicLock.h
 * @brief Header file for AtomicLock class
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AtomicLock class.
 */

#ifndef _AtomicLock_h_
#define _AtomicLock_h_

// Global header files

// Local header files

#include "BasicTypes.h"

#include "Lock.h"

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides atomic locking mechanism.
 */

class AtomicLock : public Lock
{ 

  private:

    /**
     * @brief Flag used for atomic operations.
     */

    volatile bool __lock;

    bool Try (void);
    bool Release (void);

  protected:

  public:

    /**
     * @brief Constructor.
     * @post lock = false
     */

    AtomicLock (void);

    /**
     * @brief Copy constructor.
     */

    AtomicLock (const AtomicLock& lock);

    /**
     * @brief Destructor.
     * @detail The destructor spinwaits till the lock is released.
     */

    virtual ~AtomicLock (void);

    /**
     * @brief Accessor. 
     * @return true if locked.
     */

    virtual bool GetLock (void) const;
        
    /**
     * @brief AcquireLock method.
     * @detail The method acquires the lock, it spinwaits if necessary till the lock is released.
     * @return true.
     */

    virtual bool AcquireLock (void);
        
    /**
     * @brief AcquireLock method.
     * @detail The method acquires the lock and spinwaits at most <timeout> nsec.
     * @return true if locked within <timeout> nsec.
     */

    virtual bool AcquireLock (ccs::types::uint64 timeout);
        
    /**
     * @brief ReleaseLock method.
     * @detail The method releases the lock.
     * @return true.
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

    AtomicLock& operator= (const AtomicLock& lock);

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _AtomicLock_h_


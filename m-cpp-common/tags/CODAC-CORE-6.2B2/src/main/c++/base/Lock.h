/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/Lock.h $
* $Id: Lock.h 100703 2019-07-11 15:23:56Z bauvirb $
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
 * @file Lock.h
 * @brief Header file for Lock class interface definition.
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the Lock interface class.
 */

#ifndef _Lock_h_
#define _Lock_h_

// Global header files

// Local header files

#include "BasicTypes.h"

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides Lock interface definition.
 */

class Lock
{ 

  private:

    /**
     * @brief Copy constructor.
     * @note Undefined and private to ensure the compiler does not generate a public one.
     */

    Lock (const Lock& lock);

    /**
     * @brief Copy assignment operator.
     * @note Undefined and private to ensure the compiler does not generate a public one.
     */

    Lock& operator= (const Lock& lock);

  protected:

    /**
     * @brief Constructor. NOOP.
     * @note Defined and protected to ensure the compiler does not generate a public one.
     */

    Lock (void) {};

    /**
     * @brief Destructor. NOOP.
     * @note Defined and protected to ensure the compiler does not generate a public one.
     */

    virtual ~Lock (void) {};

  public:

    /**
     * @brief Accessor. 
     * @return true if locked.
     */

    virtual bool GetLock (void) const = 0; // Pure virtual method
        
    /**
     * @brief AcquireLock method.
     * @detail The method acquires the lock.
     * @return true if locked.
     */

    virtual bool AcquireLock (void) = 0; // Pure virtual method
        
    /**
     * @brief AcquireLock method.
     * @detail The method acquires the lock and waits at most <timeout> nsec.
     * @return true if locked within <timeout> nsec.
     */

    virtual bool AcquireLock (ccs::types::uint64 timeout) = 0; // Pure virtual method
        
    /**
     * @brief ReleaseLock method.
     * @detail The method releases the lock.
     * @return true if released.
     */

    virtual bool ReleaseLock (void) = 0; // Pure virtual method
        
    /**
     * @brief TryLock method.
     * @detail The method tests the lock and acquires it if available. The method corresponds to a 
     * non-blocking AcquireLock.
     * @return true if locked.
     */

    virtual bool TryLock (void) = 0; // Pure virtual method

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _Lock_h_


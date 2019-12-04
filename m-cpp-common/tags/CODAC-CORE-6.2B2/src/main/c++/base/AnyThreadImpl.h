/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/AnyThreadImpl.h $
* $Id: AnyThreadImpl.h 100890 2019-07-19 07:14:30Z bauvirb $
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
 * @file AnyThreadImpl.h
 * @brief Header file for AnyThread imlpementation class
 * @date 09/01/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyThread implementation class.
 */

#ifndef _AnyThreadImpl_h_
#define _AnyThreadImpl_h_

// Global header files

#include <functional> // std::bind
#include <thread> // std::thread

// Local header files

#include "BasicTypes.h"

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides thread management implementation.
 */

class AnyThreadImpl
{

  private:

    ccs::types::uint32 _affinity;
    ccs::types::uint32 _priority;
    ccs::types::uint32 _policy;

    volatile bool _running;
    volatile bool _terminate;

    ccs::types::string _name;

    AnyThread_Base* _base;

    std::thread* _thread;

    // Miscellaneous methods
    void Execute (void);

    AnyThreadImpl (void); // Undefined
    AnyThreadImpl (const AnyThreadImpl& thread); // Undefined
    AnyThreadImpl& operator= (const AnyThreadImpl& thread); // Undefined

  protected:

  public:

    // Initialiser methods
    bool RegisterInterface (AnyThread_Base* base);

    // Accessor methods
    bool IsRunning (void);
    bool IsTerminating (void);

    bool SetAffinity (ccs::types::uint32 core);
    bool SetPriority (ccs::types::uint32 priority = 0u, ccs::types::uint32 policy = 0u); // SCHED_OTHER

    // Miscellaneous methods
    bool Launch (void);
    bool Terminate (void);

    // Constructor methods
    explicit AnyThreadImpl (const ccs::types::char8 * const name);

    // Destructor method
    virtual ~AnyThreadImpl (void); 

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _AnyThreadImpl_h_



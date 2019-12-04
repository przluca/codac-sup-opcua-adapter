/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/cvvf/DelegatedFunctionHandler.h $
* $Id: DelegatedFunctionHandler.h 100159 2019-06-18 07:13:41Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Generic type class definition
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

/**
 * @file DelegatedFunctionHandler.h
 * @brief Header file for DelegatedFunctionHandler class.
 * @date 11/11/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the DelegatedFunctionHandler class interface.
 * The DelegatedFunctionHandler class implementation is not exposed through this header file. A bridge
 * pattern is favoured to try and avoid exposing potentially changing implementation details
 * to calling applications. The DelegatedFunctionHandler is delegated the invocation of a function with
 * attributes passed by reference to a separate thread.
 */

#ifndef _DelegatedFunctionHandler_h_
#define _DelegatedFunctionHandler_h_

// Global header files

#include <functional> // std::function<>, etc.

// Local header files

#include <BasicTypes.h>

// Constants

// Type definition

namespace sup {

namespace core {

class DelegatedFunctionHandlerImpl; // Forward class declaration

/**
 * @brief Interface class providing support for delegated execution of functions.
 * @detail The DelegatedFunctionHandler class executes application functions in separate threads
 * and provides an interface to test status of delegated code.
 *
 * @note The design is based on a bridge pattern to avoid exposing implementation
 * specific details through the interface class.
 */

class DelegatedFunctionHandler
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    DelegatedFunctionHandlerImpl* __impl;

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    DelegatedFunctionHandler (void);

    /**
     * @brief Destructor.
     * @detail If necessary, all currently running delegated functions are 
     * terminated.
     */

    virtual ~DelegatedFunctionHandler (void);

    /**
     * @brief Launch method.
     * @detail The function execution is delegated to a separate thread. The calling
     * application has to take the necessary precaution for the attribute to be valid
     * during the scope of execution of the delegated function. Functions taking
     * multiple attributes, returning values, raising exceptions should be properly
     * wrapped.
     * @param cb Function to execute
     * @param attr Attribute (by reference)
     * @return Unique function identifier used in other methods to e.g. query status
     * of the request, etc.
     * @code
       OutputType_t AnyFunction (const InputType_t& input);

       typedef struct AnyFunctionWrapperData {
         const InputType_t input;
         OutputType_t output;
       } AnyFunctionWrapperData_t;
      
       bool AnyFunctionWrapper (void* attr)
       {

         bool status = (NULL != attr);

	 if (status) {

           AnyFunctionWrapperData_t* data = static_cast<AnyFunctionWrapperData_t*>(attr);

           try {
             // Call wrapped function
             data->output = AnyFunction(data->input);
           }
           catch (const std::excpetion& e) {
             // Handle excpetion
             status = false;
           }

         return status;

       }

       // Create execution context
       AnyFunctionWrapperData_t data;
       data.input = .. ;
       ccs::types::uint32 uid = DelegatedFunctionHandler::Launch(AnyFunctionWrapper, static_cast<void*>(&data));
       @endcode
     */

    ccs::types::uint32 Launch (const std::function<bool(void*)>& cb, void* attr);
#if 1
    /**
     * @brief Abort method.
     * @detail The function terminates execution if still running.
     * @param uid Unique function idenfier
     * @return TRUE if unique identifier is known to the DelegatedFunctionHandler instance and function
     * execution terminated.
     */

    bool Abort (ccs::types::uint32 uid);
#endif
    /**
     * @brief Discard method.
     * @detail The function discards data related to the specified delegated function. No more
     * anticipated use of the identifier.
     * @param uid Unique function idenfier
     * @pre
     *   uid = Launch(cb, attr)
     * @post
     *   false == IsValid(uid)
     * @return TRUE if unique identifier is known to the DelegatedFunctionHandler instance and function
     * related data discarded.
     */

    bool Discard (ccs::types::uint32 uid);

    /**
     * @brief Accessor method.
     * @param uid Unique function idenfier
     * @return TRUE if unique identifier is known to the DelegatedFunctionHandler instance.
     */

    bool IsValid (ccs::types::uint32 uid) const;

    /**
     * @brief Accessor method.
     * @param uid Unique function idenfier
     * @pre
     *   uid = Launch(cb, attr)
     * @return TRUE if delegated function is running.
     */

    bool IsRunning (ccs::types::uint32 uid) const;

    /**
     * @brief Accessor method.
     * @param uid Unique function idenfier
     * @pre
     *   uid = Launch(cb, attr)
     * @return Return status of the delegated function.
     */

    bool GetStatus (ccs::types::uint32 uid) const;

    /**
     * @brief Accessor method.
     * @param uid Unique function idenfier
     * @pre
     *   uid = Launch(cb, attr)
     * @return Attribute reference passed to the delegated function.
     */

    void* GetAttribute (ccs::types::uint32 uid) const;

};

// Global variables

// Function declaration

// Function definition

} // namespace core

} // namespace sup

#endif // _DelegatedFunctionHandler_h_ 

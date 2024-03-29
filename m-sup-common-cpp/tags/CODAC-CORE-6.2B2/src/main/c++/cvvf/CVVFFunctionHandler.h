/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/cvvf/CVVFFunctionHandler.h $
* $Id: CVVFFunctionHandler.h 100159 2019-06-18 07:13:41Z bauvirb $
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
 * @file CVVFFunctionHandler.h
 * @brief Header file for CVVFFunctionHandler class.
 * @date 11/11/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the CVVFFunctionHandler class interface.
 * The CVVFFunctionHandler class implementation is not exposed through this header file. A bridge
 * pattern is favoured to try and avoid exposing potentially changing implementation details
 * to calling applications. 
 * The CVVFFunctionHandler implements a Remote Procedure Call (RPC) mechanism over the network
 * for User-supplied registered code and data types (dynamically loaded libraries).
 * The API is that of a ccs::base::ConfigurableObject and ccs::base::MsgableObject to be managed
 * through the sup::core::ObjectFactoryEngine.
 */

#ifndef _CVVFFunctionHandler_h_
#define _CVVFFunctionHandler_h_

// Global header files

// Local header files

#include <BasicTypes.h>
#include <AnyObject.h>

// Constants

#define CVVF_FunctionDefinition_TypeName "sup::types::FunctionDefinition_t/v1.0"
#define CVVF_FunctionInstance_TypeName "sup::types::FunctionInstance_t/v1.0"

// Type definition

namespace sup {

namespace core {

class CVVFFunctionHandlerImpl; // Forward class declaration

/**
 * @brief Interface class providing support for remote execution of functions.
 * @detail ToDo.
 *
 * @note The design is based on a bridge pattern to avoid exposing implementation
 * specific details through the interface class.
 */

class CVVFFunctionHandler : public ccs::base::AnyObject, public ccs::base::CfgableObject//, public ccs::base::MsgableObject
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    CVVFFunctionHandlerImpl* __impl;

  protected:

  public:

    /**
     * @brief Constructor.
     */

    CVVFFunctionHandler (void);

    /**
     * @brief Destructor.
     * @detail If necessary, all currently running delegated functions are 
     * terminated. Underlying infrastructure is terminated.
     *
     * @todo Does not appear to terminate the infrastructure .. investigation required.
     */

    virtual ~CVVFFunctionHandler (void);

    /**
     * @brief Accessor. See ccs::base::CfgableObject::SetAttribute.
     * @detail Sets service name, etc.
     */

    virtual bool SetParameter (const char* name, const char* value); // Specialises virtual method
 
    /**
     * @brief Accessor. See ccs::base::MsgableObject::ProcessMessage.
     * @detail Messages correspond to orders to load library of User-supplied
     * code, etc.
     */

    //virtual bool ProcessMessage (const char* msg); // Specialises virtual method

};

} // namespace core

// Global variables

namespace cvvf {

namespace GlobalFunctionHandler { // In case the ObjectFactory is not used

using namespace ::sup::core;

static CVVFFunctionHandler* __p_fhdlr = static_cast<CVVFFunctionHandler*>(NULL); // Just instantiate the globally scoped handler

// Function declaration

// Function definition

static inline CVVFFunctionHandler* GetInstance (void)
{

  if (__builtin_expect((static_cast<CVVFFunctionHandler*>(NULL) == __p_fhdlr), 0)) // Unlikely
    {
      __p_fhdlr = new (std::nothrow) CVVFFunctionHandler ();
    }

  return __p_fhdlr; 

}

static inline bool LoadSharedLibrary (const char* name) { return GetInstance()->SetParameter("library", name); }
static inline bool StartService (const char* name) { return GetInstance()->SetParameter("service", name); }

} // namespace GlobalFunctionHandler

} // namespace cvvf

} // namespace sup

#endif // _CVVFFunctionHandler_h_ 


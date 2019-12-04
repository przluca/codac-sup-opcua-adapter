/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCClientImpl.h $
* $Id: RPCClientImpl.h 99922 2019-05-23 13:05:51Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*		  CS 90 046
*		  13067 St. Paul-lez-Durance Cedex
*		  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/**
 * @file RPCClientImpl.h
 * @brief Header file for RPCClientImpl class.
 * @date 18/09/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the RPCClientImpl class.
 */

#ifndef _RPCClientImpl_h_
#define _RPCClientImpl_h_

// Global header files

#include <AnyValue.h> // Variable with introspectable data type ..

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief Abstract base class providing support for RPC client implementations.
 *
 * @todo Mechanism to register and instantiate implementation classes.
 */

class RPCClientImpl
{

  private:

    /**
     * @brief Attribute. 
     * @detail Used to support the GetService() and SetService() default implementations.
     */

    ccs::types::string __service;

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    RPCClientImpl (void);

    /**
     * @brief Destructor.
     * @detail Terminates RPC client, if necessary.
     */

    virtual ~RPCClientImpl (void); 

    /**
     * @brief Accessor.
     * @return True if the client is currently connected.
     */

    virtual bool IsConnected (void) const = 0;

    /**
     * @brief Accessor. 
     * @detail Virtual method with default implementation which just returns the
     * recorded service name.
     * @return Associated service name (nil-terminated character array).
     */

    virtual const char* GetService (void) const;

    /**
     * @brief Accessor.
     * @detail Virtual method with default implementation which just stores  the
     * service name for future access. May be overloaded to provide implementation-specific
     * behaviour.
     * @param service Associated service name (nil-terminated character array).
     * @return True if successful, false otherwise.
     */

    virtual bool SetService (const char* service);

    /**
     * @brief Launch method.
     * @detail The client gets instantiated and connection to the service is
     * issued and verified.
     * @return True if successful.
     */

    virtual bool Launch (void) = 0;
    virtual bool Terminate (void);

    /**
     * @brief Virtual method.
     * @detail The request is sent to the server and reply is returned. Must be overloaded
     * to provide implementation-specific behaviour.
     * @return Reply from the server.
     */

    virtual ccs::types::AnyValue SendRequest (const ccs::types::AnyValue& request) const = 0;

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _RPCClientImpl_h_

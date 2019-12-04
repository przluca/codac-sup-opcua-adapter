/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCClient.h $
* $Id: RPCClient.h 99922 2019-05-23 13:05:51Z bauvirb $
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
 * @file RPCClient.h
 * @brief Header file for RPCClient interface class.
 * @date 18/09/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the RPCClient interface class.
 */

#ifndef _RPCClient_h_
#define _RPCClient_h_

// Global header files

#include <AnyValue.h> // Variable with introspectable data type ..

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

class RPCClientImpl; // Forward class declaration

/**
 * @brief Interface class providing support for RPC client.
 * @detail ToDo.
 *
 * @note The design is based on a bridge pattern to avoid exposing technology-specific
 * internals through the interface class.
 */

class RPCClient
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    RPCClientImpl* __impl;

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    RPCClient (void);

    /**
     * @brief Constructor.
     * @detail RPC client is instantiated and connection to the RPC service is
     * issued.
     * @param service RPC service name (nil-terminated character array).
     */

    RPCClient (const char* service);

    /**
     * @brief Destructor.
     */

    virtual ~RPCClient (void); 

    /**
     * @brief Accessor.
     * @return True if the service is currently connected.
     */

    bool IsConnected (void) const;

    /**
     * @brief Accessor.
     * @return Associated RPC service name (nil-terminated character array).
     */

    const char* GetService (void) const;

    /**
     * @brief Accessor.
     * @param service Associated RPC service name (nil-terminated character array).
     * @return True if successful.
     */

    bool SetService (const char* service);

    /**
     * @brief Launch method.
     * @detail The RPC client gets instantiated and connection to the RPC service is
     * issued and verified.
     * @return True if successful.
     */

    bool Launch (void);

    /**
     * @brief SendRequest method.
     * @detail The request is sent to the RPC server and reply is returned.
     * @return Reply from the RPC server.
     */

    ccs::types::AnyValue SendRequest (const ccs::types::AnyValue& request) const;

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _RPCClient_h_

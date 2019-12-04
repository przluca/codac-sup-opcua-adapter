/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessRPCClient.h $
* $Id: PVAccessRPCClient.h 99922 2019-05-23 13:05:51Z bauvirb $
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
 * @file PVAccessRPCClient.h
 * @brief Header file for PVAccessRPCClient class.
 * @date 18/09/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the PVAccessRPCClient class.
 */

#ifndef _PVAccessRPCClient_h_
#define _PVAccessRPCClient_h_

// Global header files

#include <memory> // std::shared_ptr

#include <pv/pvData.h>
#include <pv/rpcClient.h>
//#include <pv/rpcServer.h>
#include <pv/clientFactory.h>

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

#include "RPCClientImpl.h" 

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief Implementation class providing support for PVA RPC client.
 * @detail ToDo.
 */

class PVAccessRPCClient : public RPCClientImpl
{

  private:

    bool __initialised;
    bool __connected;

    std::shared_ptr<epics::pvAccess::RPCClient> __client;

    // Initialiser methods
    bool Initialise (void);

  protected:

  public:

    // Initialiser methods
    virtual bool Launch (void);
    virtual bool Terminate (void) { return true; };

    // Accessor methods
    virtual bool IsConnected (void) const;

    // Miscellaneous methods
    virtual ccs::types::AnyValue SendRequest (const ccs::types::AnyValue& request) const;

    // Constructor methods
    PVAccessRPCClient (void) { __initialised = false; __connected = false; RPCTypes::Initialise(); };

    // Destructor method
    virtual ~PVAccessRPCClient (void) {}; 

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _PVAccessRPCClient_h_


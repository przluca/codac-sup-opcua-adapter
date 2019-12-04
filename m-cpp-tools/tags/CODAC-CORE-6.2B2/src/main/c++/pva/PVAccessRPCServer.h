/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessRPCServer.h $
* $Id: PVAccessRPCServer.h 99922 2019-05-23 13:05:51Z bauvirb $
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
 * @file PVAccessRPCServer.h
 * @brief Header file for PVAccessRPCServer class.
 * @date 18/09/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the PVAccessRPCServer class.
 */

#ifndef _PVAccessRPCServer_h_
#define _PVAccessRPCServer_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

#include <pv/pvData.h>
#include <pv/rpcServer.h>
#include <pv/serverContext.h>

// Local header files

#include "RPCServerImpl.h"

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief Class providing implementation for PVA RPC server.
 */

class PVAccessRPCServer : public RPCServerImpl, public epics::pvAccess::RPCService
{

  private:

    bool __initialised;

    std::shared_ptr<epics::pvAccess::RPCServer> __server;
    std::shared_ptr<epics::pvAccess::ServerContext> __context;

    // Initialiser methods
    bool Initialise (void);

  protected:

  public:

    // Initialiser methods
    virtual bool Launch (void);
    virtual bool Terminate (void) { return true; };

    // Accessor methods

    // Overloaded method called upon client request 
    std::shared_ptr<epics::pvData::PVStructure> request (std::shared_ptr<epics::pvData::PVStructure> const & __request)
      throw (epics::pvAccess::RPCRequestException);

    // Constructor methods
    PVAccessRPCServer (void) : RPCServerImpl() { __initialised = false; };

    // Destructor method
    virtual ~PVAccessRPCServer (void) {}; 

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _PVAccessRPCServer_h_


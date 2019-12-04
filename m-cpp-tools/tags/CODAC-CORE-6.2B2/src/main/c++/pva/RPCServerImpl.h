/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCServerImpl.h $
* $Id: RPCServerImpl.h 99922 2019-05-23 13:05:51Z bauvirb $
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
 * @file RPCServerImpl.h
 * @brief Header file for RPCServerImpl abstract class.
 * @date 18/09/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the RPCServerImpl class.
 */

#ifndef _RPCServerImpl_h_
#define _RPCServerImpl_h_

// Global header files

#include <functional> // std::function

#include <AnyValue.h> // Variable with introspectable data type ..

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief Abstract base class defining interface for RPC server implementations.
 * @detail The base class is provided to be able to support a bridge (PIMPL) pattern
 * for RPC servers independent of the actual transport mechanism.
 *
 * Transport implementations inherit from this base class and provide any necessary
 * means to instantiate a named service implementation over the chosen communication
 * mechanism.
 *
 * The base class provides a mechanism to externally define a handler callback.
 *
 * @code
   class PVAccessRPCServer : public ccs::base::RPCServerImpl, public epics::pvAccess::RPCService
   {

     private:

       // PVAccess-specific constructs
       std::shared_ptr<epics::pvAccess::RPCServer> __server;
       std::shared_ptr<epics::pvAccess::ServerContext> __context;

     public:

       PVAccessRPCServer (void) : ccs::base::RPCServerImpl() {};
       virtual ~PVAccessRPCServer (void) {};

       virtual bool Launch (void) {

         bool status = !ccs::HelperTools::IsUndefinedString(GetService());

	 if (status)
	   {

             try
               {

                 // Perform implementation-specific service start operations ..
                 __context = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, true); 
                 __server = std::shared_ptr<epics::pvAccess::RPCServer>(new epics::pvAccess::RPCServer ()); 

                 __server->registerService(std::string(GetService()), std::shared_ptr<epics::pvAccess::RPCService>(this)); 
                 __server->runInNewThread(); 

	       }
	     catch (...)
               {
	         status = false;
	       }

	   }

         return status;

       };

       // PVAccess-specific methods
       std::shared_ptr<epics::pvData::PVStructure> request (std::shared_ptr<epics::pvData::PVStructure> const & __request)
         throw (epics::pvAccess::RPCRequestException);

   };

   @endcode
 *
 * @todo Mechanism to register and instantiate implementation classes.
 */

class RPCServerImpl
{

  private:

    /**
     * @brief Attribute. 
     * @detail Used to support the GetService() and SetService() default implementations.
     */

    ccs::types::string __service;

    /**
     * @brief Attribute. 
     * @detail RPC handler callback. The registered function is being called to provide
     * a structured reply associated to the structured variable input. It is the
     * responsibility of implementation classes to call the handler by means of the
     * CallHandler() method.
     */

    std::function<ccs::types::AnyValue(const ccs::types::AnyValue&)> __cb;

  protected:

  public:

    /**
     * @brief Constructor.
     * @detail RPC server is instantiated and service registered.
     */

    RPCServerImpl (void);

    /**
     * @brief Destructor.
     * @detail Terminates RPC server, if necessary.
     */

    virtual ~RPCServerImpl (void); 

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

    virtual bool Launch (void) = 0;
    virtual bool Terminate (void);

    /**
     * @brief Accessor.
     * @param cb Function callback providing service handling.
     * @return True in any condition.
     */

    bool RegisterHandler (std::function<ccs::types::AnyValue(const ccs::types::AnyValue&)> cb);

    /**
     * @brief Accessor.
     * @param request Request received as input for RPC.
     * @detail Calls the RPC handler callback, if any has been registered. The
     * registered function is being called to provide a structured reply associated
     * to the structured variable input. It is the responsibility of implementation
     * classes to call this method, as necessary.
     * @return Instance of ccs::types::AnyValue as reply.
     * @todo Assess need for exception specification.
     */

    ccs::types::AnyValue CallHandler (const ccs::types::AnyValue& request) const;

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _RPCServerImpl_h_


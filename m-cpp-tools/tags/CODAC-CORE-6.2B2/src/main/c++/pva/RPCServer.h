/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCServer.h $
* $Id: RPCServer.h 99922 2019-05-23 13:05:51Z bauvirb $
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
 * @file RPCServer.h
 * @brief Header file for RPCServer class.
 * @date 18/09/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the RPCServer class.
 */

#ifndef _RPCServer_h_
#define _RPCServer_h_

// Global header files

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

class RPCServerImpl; // Forward class declaration

/**
 * @brief Interface class providing support for RPC server.
 * @detail The base class is instantiated with the name of the RPC service.
 * The base class constructor and destructors take care of RPC server
 * life-cycle management.
 *
 * Implementation classes specialise the HandleRequest method to be notified
 * of RPC requests, perform application-specific processing, and provide a 
 * reply to the RPC client.
 *
 * @code
   class SpecialisedRPCHandler : public ccs::base::RPCServer
   {

     private:

       std::shared_ptr<ccs::types::CompoundType> __reply_base; // Introspectable type definition

     public:

       struct __attribute__((packed)) Reply {
         ccs::types::uint64 timestamp;
	 ccs::types::string qualifier = "reply";
	 ccs::types::boolean status = true;
	 ccs::types::string reason = "";
       };

       template <typename Type> struct __attribute__((packed)) ReplyWithValue : public Reply {
         Type value;
       };

       SpecialisedRPCHandler (const char* name) : ccs::base::RPCServer(name) {
         __reply_base = std::shared_ptr<ccs::types::CompoundType>((new (std::nothrow) ccs::types::CompoundType ("app::RPCReply_t/v1.0"))
	   ->AddAttribute<ccs::types::uint64>("timestamp")
	   ->AddAttribute<ccs::types::string>("qualifier")
	   ->AddAttribute<ccs::types::boolean>("status")
	   ->AddAttribute<ccs::types::string>("reason"));
       };

       virtual ~SpecialisedRPCHandler (void) {};

       virtual ccs::types::AnyValue HandleRequest (const ccs::types::AnyValue& request) {

         // Reply as C-like struct
         ReplyWithValue<ccs::types::string> reply;
	 reply.timestamp = ccs::HelperTools::GetCurrentTime();
	 ccs::HelperTools::SafeStringCopy(reply.value, "Hello world!");

         // Reply as introspectable object
	 ccs::types::CompoundType reply_type (*__reply_base); // Copy base reply type
	 reply_type.AddAttribute<ccs::types::string>("value"); // Extend with value field

         ccs::types::AnyValue reply_ret (reply_type);
	 reply_ret = reply; // Assign with C-like struct

	 return reply_ret;

       };

   };

   @endcode
 *
 * @note The design is based on a bridge pattern to avoid exposing server-specific
 * internals through the interface class, e.g. transport technology, etc.
 *
 * @todo Mechanism to register and instantiate various transport implementation classes.
 */

class RPCServer
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    RPCServerImpl* __impl;

  protected:

  public:

    /**
     * @brief Constructor.
     * @detail PVA server is instantiated and service registered.
     * @param service RPC service name (nil-terminated character array).
     */

    RPCServer (const char* service);

    /**
     * @brief Destructor.
     * @detail Terminates RPC server.
     */

    virtual ~RPCServer (void); 

    /**
     * @brief Accessor.
     * @return Associated PVA service name (nil-terminated character array).
     */

    const char* GetService (void) const;

    /**
     * @brief Virtual handler method.
     * @param request Received structure associated to an introspectable type definition.
     * @return Structure to use as RPC reply.
     */

    virtual ccs::types::AnyValue HandleRequest (const ccs::types::AnyValue& request) = 0; // Pure virtual method

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _RPCServer_h_


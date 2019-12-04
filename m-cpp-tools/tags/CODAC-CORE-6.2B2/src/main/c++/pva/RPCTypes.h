/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCTypes.h $
* $Id: RPCTypes.h 99922 2019-05-23 13:05:51Z bauvirb $
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
 * @file RPCTypes.h
 * @brief Header file for RPC-specific types.
 * @date 11/11/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the RPC-specific types.
 */

#ifndef _RPCTypes_h_
#define _RPCTypes_h_

// Global header files

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

// Constants

#define RPCReply_TypeName "ccs::RPCReply_t/v1.0"
#define RPCRequest_TypeName "ccs::RPCRequest_t/v1.0"

// Type definition

namespace ccs {

namespace base {

namespace RPCTypes {

typedef struct __attribute__((packed)) Request {
  ccs::types::uint64 timestamp;
  ccs::types::string qualifier = "";
} Request_t;

template <typename Type> struct __attribute__((packed)) RequestWithValue : public Request {
  Type value;
};

typedef struct __attribute__((packed)) Reply {
  ccs::types::uint64 timestamp;
  ccs::types::string qualifier = "reply";
  ccs::types::boolean status = true;
  ccs::types::string reason = ""; // Optional substantiating information related to status
} Reply_t;

template <typename Type> struct __attribute__((packed)) ReplyWithValue : public Reply {
  Type value;
};

// Global variables

extern std::shared_ptr<const ccs::types::CompoundType> Request_int; // Introspectable type definition
extern std::shared_ptr<const ccs::types::CompoundType> Reply_int; // Introspectable type definition

// Function declaration

// WARNING - ScalarType are statically initialised but there is no way to guarantee static
// initialisation order. The above types use ScalarType as attributes and must therefore be
// dynamically initialised .. called by PVAccessRPC classes upon initialisation.
bool Initialise (void);

// Function definition

} // namespace RPCTypes

} // namespace base

} // namespace ccs

#endif // _RPCTypes_h_

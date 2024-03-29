/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/RPCClientImpl.cpp $
* $Id: RPCClientImpl.cpp 99922 2019-05-23 13:05:51Z bauvirb $
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

// Global header files

#include <new> // std::nothrow

#include <BasicTypes.h> // Global type definition
#include <SysTools.h> // Misc. helper functions

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE
#include <log-api.h> // Syslog wrapper routines

#include <AnyType.h> // Introspectable data type ..
#include <AnyTypeHelper.h> // .. associated helper routines

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

#include "RPCClientImpl.h" // This class definition
#include "RPCTypes.h" // .. associated types

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "rpc-if"

// Type definition

namespace ccs {

namespace base {

// Global variables

// Function declaration

// Function definition

const char * RPCClientImpl::GetService (void) const { return __service; }
bool RPCClientImpl::SetService (const char* service) { ccs::HelperTools::SafeStringCopy(__service, service, STRING_MAX_LENGTH); return true; }

bool RPCClientImpl::Terminate (void) { return true; }

RPCClientImpl::RPCClientImpl (void)
{ 
  // Initialise attributes
  ccs::HelperTools::SafeStringCopy(__service, STRING_UNDEFINED, STRING_MAX_LENGTH); 

  // Register types in GlobalTypeDatabase
  RPCTypes::Initialise(); 

  return; 

}

RPCClientImpl::~RPCClientImpl (void)
{ 

  Terminate();

  return;

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

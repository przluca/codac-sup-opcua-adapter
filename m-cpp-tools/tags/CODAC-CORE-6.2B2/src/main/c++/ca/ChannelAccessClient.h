/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/ca/ChannelAccessClient.h $
* $Id: ChannelAccessClient.h 96477 2019-01-09 14:53:55Z sivecm $
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
 * @file ChannelAccessClient.h
 * @brief Header file for ChannelAccessClient class.
 * @date 11/11/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the ChannelAccessClient class.
 */

#ifndef _ChannelAccessClient_h_
#define _ChannelAccessClient_h_

// Global header files

#include <functional> // std::function
#include <new> // std::nothrow

//#include <cadef.h> // Channel Access API definition, etc.

#include <BasicTypes.h> // Global type definition

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

// Constants

#undef DBR_NOACCESS
#define DBR_NOACCESS DBR_LONG // SDD bug in CCSv5.0
#undef DBR_ULONG
#define DBR_ULONG DBR_DOUBLE // SDD bug in CCSv5.1
#undef DBR_UNDEF
#define DBR_UNDEF 1024 

// Type definition

namespace ccs {

namespace base {

class ChannelAccessClient_Impl; // Forward class declaration

/**
 * @brief Interface class providing support for CA client with variable cache.
 * @detail The class provides access to a variable cache and asynchronous CA
 * update to ensure non-blocking calls on the application side. The variable
 * cache is implemented by means of ccs::types::AnyValue instances which allow
 * assignment to/from C-like structures.
 *
 * The class also offers a callback mechanism for input or bi-directional
 * variables to allow for application-specific synchronous handling of CA
 * monitor events.
 *
 * @note The design is based on a bridge pattern to avoid exposing CA specific
 * internals through the interface class.
 */

class ChannelAccessClient
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    ChannelAccessClient_Impl* __impl;

  protected:

  public:

    ChannelAccessClient (void);

    virtual ~ChannelAccessClient (void);

    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const std::shared_ptr<const ccs::types::AnyType>& type);
    bool AddVariable (const char* name, bool isInput, ccs::types::uint32 type, ccs::types::uint32 mult = 1); // From SDD-generated PV list */

    bool Launch (void); // Should be called after the variable table is populated

    bool IsValid (const char* name) const;

    template <typename Type> bool GetVariable (const char* name, Type& value) const;
    template <typename Type> bool SetVariable (const char* name, Type& value);

    ccs::types::AnyValue* GetVariable (const char* name) const;

    bool UpdateVariable (const char* name);

    /**
     * @brief Accessor. SetCallback method.
     * @detail The method installs an application callback to be called synchronously when
     * an input or bi-directional variable has been externally updated. The application can
     * use other methods to access the variable cache asynchronously.
     * @param name Variable identifier.
     * @param cb Callback method.
     * @return True if successful.
     *
     * @code
       // Synchronous callback on record change
       void HandleUpdate (const char* name, const ccs::types::AnyValue& value)
       {
         // Handle update ..
         log_info("CA record '%s' updated", name);
	 return;
       }

       bool status = ccs::base::ChannelAccessInterface::GetInstance<ccs::base::ChannelAccessClient>()
                       ->SetCallback("MyVariable", &HandleUpdate);
       @endcode
     *
     * @todo Mechanism for callback to identify the corresponding variable
     */ 

    bool SetCallback (const char* name, std::function<void(const char*, const ccs::types::AnyValue&)> cb);

};

// Global variables

namespace ChannelAccessInterface {

extern ChannelAccessClient* __ca_if;

// Function declaration

template <typename Type> Type* GetInstance (void);

// Function definition

template <> inline ChannelAccessClient* GetInstance<ChannelAccessClient> (void) 
{ 

  if (static_cast<ChannelAccessClient*>(NULL) == __ca_if) 
    __ca_if = new (std::nothrow) ccs::base::ChannelAccessClient (); 

  return __ca_if;

} 

} // namespace ChannelAccessInterface

template <typename Type> bool ChannelAccessClient::GetVariable (const char* name, Type& value) const 
{ 

  const ccs::types::AnyValue* var = static_cast<ccs::types::AnyValue*>(NULL); 

  bool status = this->IsValid(name); 

  if (status) 
    { 
      var = this->GetVariable(name); 
      value = static_cast<Type>(*var); 
    } 
  
  return status; 
  
}

template <typename Type> bool ChannelAccessClient::SetVariable (const char* name, Type& value) 
{ 

  ccs::types::AnyValue* var = static_cast<ccs::types::AnyValue*>(NULL); 

  bool status = this->IsValid(name); 

  if (status) 
    { 
      var = this->GetVariable(name); 
      *var = value; 
      this->UpdateVariable(name); 
    } 

  return status; 

}

} // namespace base

} // namespace ccs

#endif // _ChannelAccessClient_h_


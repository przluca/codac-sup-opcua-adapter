/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessClient.h $
* $Id: PVAccessClient.h 96477 2019-01-09 14:53:55Z sivecm $
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
 * @file PVAccessClient.h
 * @brief Header file for PVAccessClient class.
 * @date 15/10/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the PVAccessClient class.
 */

#ifndef _PVAccessClient_h_
#define _PVAccessClient_h_

// Global header files

#include <new> // std::nothrow

#include <BasicTypes.h> // Global type definition

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

class PVAccessClient_Impl; // Forward class declaration

/**
 * @brief Interface class providing support for PVA client with variable cache.
 * @detail The class provides access to a variable cache and asynchronous PVA
 * update to ensure non-blocking calls on the application side. The variable
 * cache is implemented by means of ccs::types::AnyValue instances which allow
 * assignment to/from C-like structures.
 *
 * @note The design is based on a bridge pattern to avoid exposing PVA specific
 * internals through the interface class.
 */

class PVAccessClient
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    PVAccessClient_Impl* __impl;

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    PVAccessClient (void);

    /**
     * @brief Destructor. 
     * @detail The PVA client and thread are terminated, if necessary.
     */

    virtual ~PVAccessClient (void);

    /**
     * @brief AddVariable method. 
     * @detail The method adds a variable to the cache. The variable is associated to a PVA
     * record. The direction instructs the asynchronous thread on the intended use of the variable
     * by the application, either as input variable, output variable or bi-directional. This version
     * does not perform any synchronous update; rather, the asynchronous handling thread is using the
     * direction attribute to perform either a get, put, or put/get operation at each cycle.
     * @param name Variable name to be used as PVA record name.
     * @param direction Variable direction, see ccs::types::DirIdentifier.
     * @param type Introspectable type definition, see ccs::types::AnyType.
     * @return True on success. 
     *
     * @code
       // PVA Client interface instance
       ccs::base::PVAccessClient* pvac_if = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessClient>();

       bool status = (static_cast<ccs::base::PVAccessClient*>(NULL) != pvac_if);

       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
	 ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
	 ->AddAttribute("value", ccs::types::Float64);

       if (status) {
         // Add variable to the PVA variable cache
         status = pvac_if->AddVariable("MyVariable", ccs::types::OutputVariable, type);
       }
       @endcode
     *
     * @note This version does not perform any synchronous update; rather, the asynchronous
     */

    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyType* type);

    /**
     * @brief Accessor. SetPeriod method. 
     * @detail The method changes the period with which the variable cache asynchronous
     * handling thread runs.
     * @param period Thread update period [ns].
     * @return True on success.
     */

    bool SetPeriod (ccs::types::uint64 period);

    /**
     * @brief Launch method. 
     * @detail The method creates PVA records for each variable, starts the server
     * and the variable cache asynchronous handling thread. The method should be
     * after the variable cache is populated by the application.
     * @return True on success. 
     *
     * @code
       // PVA Client interface instance
       ccs::base::PVAccessClient* pvac_if = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessClient>();

       bool status = (static_cast<ccs::base::PVAccessClient*>(NULL) != pvac_if);

       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
	 ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
	 ->AddAttribute("value", ccs::types::Float64);

       if (status) {
         // Add variable to the PVA variable cache
         status = pvac_if->AddVariable("MyVariable", ccs::types::OutputVariable, type);
       }

       if (status) {
         // Start client and cache asynchronous handling thread
         status = pvac_if->Launch();
       }
       @endcode
     */

    bool Launch (void);

    /**
     * @brief Test if variable is part of the cache.
     * @return True if variable exists. 
     */ 

    bool IsValid (const char* name) const;

    /**
     * @brief Accessor. GetVariable method.
     * @detail The variable held in the cache is statically cast to the <value> parameter.
     * @param name Variable identifier in the cache.
     * @param value Application-side variable updated with value in the cache.
     * @return True if variable exists. 
     *
     * @code
       // The application-side variable
       struct {
         ccs::types::uint64 time;
	 ccs::types::float64 value;
       } var;

       // Read from the PVA variable cache
       bool status = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessClient>()
         ->GetVariable("MyVariable", var);
       @endcode
     */ 

    template <typename Type> bool GetVariable (const char* name, Type& value) const;

    /**
     * @brief Accessor. SetVariable method.
     * @detail The variable held in the cache is assigned with the <value> parameter.
     * If successful, the variable is also marked for asynchronous update of the PVA
     * record at the next thread cycle.
     * @param name Variable identifier in the cache.
     * @param value Application-side variable to update the variable in the cache.
     * @return True if variable exists. 
     *
     * @code
       // The application-side variable
       struct {
         ccs::types::uint64 time;
	 ccs::types::float64 value;
       } var;

       // Value to be reflected on the PVA record
       var.time = ccs::HelperTools::GetCurrentTime();
       var.value = .. ;

       // Update the PVA variable cache
       bool status = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessClient>()
         ->SetVariable("MyVariable", var);
       @endcode
     */ 

    template <typename Type> bool SetVariable (const char* name, Type& value);

    /**
     * @brief Accessor. GetVariable method.
     * @detail The method allows to manipulate the variable cache from the application-side,
     * e.g. attribute level access for structured variables, etc. The UpdateVariable method
     * has to be called by the application to trigger PVA record update, when necessary.
     * @param name Variable identifier.
     * @return Reference to the ccs::types::AnyValue instance in the cache if variable exists, 
     * NULL otherwise. 
     *
     * @code
       // Get variable reference in the cache
       ccs::types::AnyValue* var = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessClient>()
         ->GetVariable("MyVariable");

       bool status = (static_cast<ccs::types::AnyValue*>(NULL) != var);

       if (status) {
         status = ccs::HelperTools::HasAttribute(var, "timestamp");
       }

       if (status) {
         status = (ccs::types::UnsignedInteger64 == ccs::HelperTools::GetAttributeType(var, "timestamp"));
       }

       if (status) {
         ccs::types::uint64 time = ccs::HelperTools::GetCurrentTime();
         status = ccs::HelperTools::SetAttributeValue(var, "timestamp", time);
       }
       @endcode
     */ 

    ccs::types::AnyValue* GetVariable (const char* name) const;

    /**
     * @brief Accessor. UpdateVariable method.
     * @detail The method allows to trigger PVA record update, after variable cache is modified
     * by the application. For output or bi-directional variable.
     * @param name Variable identifier.
     *
     * @code
       // Get variable reference in the cache
       ccs::types::AnyValue* var = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessClient>()
         ->GetVariable("MyVariable");

       bool status = (static_cast<ccs::types::AnyValue*>(NULL) != var);

       if (status) {
         status = ccs::HelperTools::HasAttribute(var, "timestamp");
       }

       if (status) {
         status = (ccs::types::UnsignedInteger64 == ccs::HelperTools::GetAttributeType(var, "timestamp"));
       }

       if (status) {
         ccs::types::uint64 time = ccs::HelperTools::GetCurrentTime();
         status = ccs::HelperTools::SetAttributeValue(var, "timestamp", time);
       }

       if (status) {
         ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessClient>()->UpdateVariable("MyVariable");
       }
       @endcode
     */ 

    void UpdateVariable (const char* name);

};

// Global variables

namespace PVAccessInterface {

extern PVAccessClient* __pvac_if;

// Function declaration

// Function definition

template <typename Type> Type* GetInstance (void);

template <> inline PVAccessClient* GetInstance<PVAccessClient> (void) 
{ 

  if (static_cast<PVAccessClient*>(NULL) == __pvac_if)
    __pvac_if = new (std::nothrow) ccs::base::PVAccessClient ();

  return __pvac_if;

} 

} // namespace PVAInterface

template <typename Type> bool PVAccessClient::GetVariable (const char* name, Type& value) const 
{ 

  ccs::types::AnyValue* var = static_cast<ccs::types::AnyValue*>(NULL); 

  bool status = this->IsValid(name); 

  if (status) 
    { 
      var = this->GetVariable(name); 
      value = static_cast<Type>(*var); 
    } 

  return status; 

}

template <typename Type> bool PVAccessClient::SetVariable (const char* name, Type& value) 
{ 

  const ccs::types::AnyValue* var = static_cast<ccs::types::AnyValue*>(NULL); 

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

#endif // _PVAccessClient_h_

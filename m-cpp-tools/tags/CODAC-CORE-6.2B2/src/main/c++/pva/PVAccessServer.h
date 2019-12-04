/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessServer.h $
* $Id: PVAccessServer.h 96477 2019-01-09 14:53:55Z sivecm $
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
 * @file PVAccessServer.h
 * @brief Header file for PVAccessServer class.
 * @date 15/10/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the PVAccessServer class.
 */

#ifndef _PVAccessServer_h_
#define _PVAccessServer_h_

// Global header files

#include <functional> // std::function
#include <new> // std::nothrow

#include <BasicTypes.h> // Global type definition

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

class PVAccessServer_Impl; // Forward class declaration

/**
 * @brief Interface class providing support for PVA server with variable cache.
 * @detail The class provides access to a variable cache and asynchronous PVA
 * update to ensure non-blocking calls on the application side. The variable
 * cache is implemented by means of ccs::types::AnyValue instances which allow
 * assignment to/from C-like structures.
 *
 * @note The design is based on a bridge pattern to avoid exposing PVA specific
 * internals through the interface class.
 */

class PVAccessServer
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    PVAccessServer_Impl* __impl;

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    PVAccessServer (void);

    /**
     * @brief Destructor. 
     * @detail The PVA server and thread are terminated, if necessary.
     */

    virtual ~PVAccessServer (void);

    /**
     * @brief AddVariable method. 
     * @detail The method adds a variable to the cache. The variable is associated to a PVA
     * record. The direction instructs the server on the intended use of the variable by the
     * application, either as input variable, output variable or bi-directional. If specified
     * as input or bi-directional, the application may attach a callback to the variable which
     * will be called upon external write to the PVA record.
     * @param name Variable name to be used as PVA record name.
     * @param direction Variable direction, see ccs::types::DirIdentifier.
     * @param type Introspectable type definition, see ccs::types::AnyType.
     * @return True on success. 
     *
     * @code
       // PVA Server interface instance
       ccs::base::PVAccessServer* pvas_if = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>();

       bool status = (static_cast<ccs::base::PVAccessServer*>(NULL) != pvas_if);

       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
	 ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
	 ->AddAttribute("value", ccs::types::Float64);

       if (status) {
         // Add variable to the PVA variable cache
         status = pvas_if->AddVariable("MyVariable", ccs::types::OutputVariable, type);
       }
       @endcode
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
       // PVA Server interface instance
       ccs::base::PVAccessServer* pvas_if = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>();

       bool status = (static_cast<ccs::base::PVAccessServer*>(NULL) != pvas_if);

       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
	 ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
	 ->AddAttribute("value", ccs::types::Float64);

       if (status) {
         // Add variable to the PVA variable cache
         status = pvas_if->AddVariable("MyVariable", ccs::types::OutputVariable, type);
       }

       if (status) {
         // Start server and cache asynchronous handling thread
         status = pvas_if->Launch();
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
       bool status = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()
         ->GetVariable("MyVariable", var);
       @endcode
     */ 

    template <typename Type> bool GetVariable (const char* name, Type& value) const;

    /**
     * @brief Accessor. SetVariable method.
     * @detail The variable held in the cache is assigned with the <value> parameter.
     * If successful, the variable is also marked for asynchronous update of the PVA
     * record.
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
       bool status = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()
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
       ccs::types::AnyValue* var = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()
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
     * by the application.
     * @param name Variable identifier.
     *
     * @code
       // Get variable reference in the cache
       ccs::types::AnyValue* var = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()
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
         ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()->UpdateVariable("MyVariable");
       }
       @endcode
     */ 

    void UpdateVariable (const char* name);

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
       void HandleUpdate (const ccs::types::AnyValue& value)
       {
         // Handle update ..
         log_info("PVA record updated");
	 return;
       }

       bool status = ccs::base::PVAccessInterface::GetInstance<ccs::base::PVAccessServer>()
                       ->SetCallback("MyVariable", &HandleUpdate);
       @endcode
     */ 

    bool SetCallback (const char* name, std::function<void(const ccs::types::AnyValue&)> cb);

};

// Global variables

namespace PVAccessInterface {

extern PVAccessServer* __pvas_if;

// Function declaration

template <typename Type> Type* GetInstance (void);

// Function definition

template <> inline PVAccessServer* GetInstance<PVAccessServer> (void) 
{ 

  if (static_cast<PVAccessServer*>(NULL) == __pvas_if)
    __pvas_if = new (std::nothrow) ccs::base::PVAccessServer ();

  return __pvas_if;

} 

} // namespace PVAccessInterface

template <typename Type> bool PVAccessServer::GetVariable (const char* name, Type& value) const 
{ 

  const ccs::types::AnyValue* var = static_cast<const ccs::types::AnyValue*>(NULL); 

  bool status = this->IsValid(name); 

  if (status) 
    { 
      var = this->GetVariable(name); 
      value = static_cast<Type>(*var); 
    } 

  return status; 

}

template <typename Type> bool PVAccessServer::SetVariable (const char* name, Type& value) 
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

#endif // _PVAccessServer_h_


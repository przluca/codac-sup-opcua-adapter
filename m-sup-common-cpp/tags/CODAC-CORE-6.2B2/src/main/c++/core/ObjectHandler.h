/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/core/ObjectHandler.h $
* $Id: ObjectHandler.h 100486 2019-07-05 09:14:14Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Generic type class definition
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/**
 * @file ObjectHandler.h
 * @brief Header file for ObjectHandler class.
 * @date 19/06/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the ObjectHandler class.
 */

#ifndef _ObjectHandler_h_
#define _ObjectHandler_h_

// Global header files

#include <BasicTypes.h>

#include <AnyObject.h>
#include <ObjectDatabase.h>

// Local header files

// Constants

// Type definition

#ifdef __cplusplus

namespace sup {

namespace core {

/**
 * @brief Class providing support for handling instances of ccs::base::AnyObject.
 * @detail The class allows for instantiating classes inheriting from ccs::base::AnyObject
 * or associated/derived interface classes ccs::base::CfgableObject and
 * ccs::base::MsgableObject.
 *
 * The implementation relies on ccs::base::GlobalObjectFactory for instance creation and
 * deletion, and on ccs::base::GlobalObjectDatabase for storing instance references by name.
 *
 * @note The class also inherits from ccs::base::ObjectDatabase to keep its own list of
 * handled references to avoid terminating instances which might otherwise be registered in
 * the ccs::base::GlobalObjectDatabase.
 */

typedef class ObjectHandler : public ::ccs::base::ObjectDatabase
{

  private:

    /**
     * @brief Copy constructor.
     * @note Undefined as invalid operation to copy instances of this class.
     */

    ObjectHandler (const ObjectHandler& handler);

    /**
     * @brief Copy assignment operator.
     * @note Undefined as invalid operation to copy instances of this class.
     */

    ObjectHandler& operator= (const ObjectHandler& handler);

  protected:

  public:

    /**
     * @brief Constructor.
     */

    ObjectHandler (void);

    /**
     * @brief Destructor.
     */

    virtual ~ObjectHandler (void);

    /**
     * @brief Loads shared library. 
     * @param name Library name.
     * @return True if successful, false otherwise.
     */

    bool LoadLibrary (const char * const name) const;

    /**
     * @brief Tests if class is registered. 
     * @detail Proxy to ccs::base::GlobalObjectFactory::IsValid().
     * @param name Class name.
     * @return True if successful, false otherwise.
     */

    bool IsTypeValid (const char * const name) const;

    /**
     * @brief Instantiates registered class.
     * @detail Creates an instance of a registered class and stores the instance by name
     * for future reference.
     * @param name Instance key.
     * @return True if successful, false otherwise.
     */

    bool Instantiate (const char * const type, const char * const name);

    /**
     * @brief Terminates all registered instances.
     * @return True if successful, false otherwise.
     */

    bool Terminate (void);

    /**
     * @brief Terminates registered instances.
     * @param name Registered instance name.
     * @return True if successful, false otherwise.
     */

    bool Terminate (const char * const name);

    /**
     * @brief Tests if instance is registered and has the appropriate type. 
     * @param name Instance name.
     * @return True if successful, false otherwise.
     */

    template <typename Type> bool IsInstanceOf (const char * const name) const;

    /**
     * @brief Accessor.
     * @detail Retreves named instance and perform RTTI type casting.
     * @param name Registered instance name.
     * @return Instance reference or NULL.
     */

    template <typename Type> Type* GetInstance (const char * const name) const;

    /**
     * @detail ToDo
     */

    bool SetParameter (const char * const name, const char * const param, const char * const value) const;

    /**
     * @detail ToDo
     */

    bool ProcessMessage (const char * const name, const char * const msg) const;

} ObjectEngine __attribute__((deprecated)); // Backward compatibility

// Global variables

// Function declaration

// Function definition

template <typename Type> bool ObjectHandler::IsInstanceOf (const char * const name) const
{

  bool status = ::ccs::base::ObjectDatabase::IsValid(name);

  if (status)
    {
      status = (static_cast<Type*>(NULL) != dynamic_cast<Type*>(::ccs::base::ObjectDatabase::GetInstance(name)));
    }

  return status;

}

template <typename Type> Type* ObjectHandler::GetInstance (const char * const name) const
{

  bool status = ::ccs::base::ObjectDatabase::IsValid(name);

  Type* ref = static_cast<Type*>(NULL);
#if 0
  if (status)
    {
      status = IsInstanceOf<Type>(name);
    }
#endif
  if (status)
    {
      ref = dynamic_cast<Type*>(::ccs::base::ObjectDatabase::GetInstance(name));
    }

  return ref;

}

} // namespace core

} // namespace sup

extern "C" { // Python wrappings

#endif // __cplusplus

// ObjectHandler methods
sup::core::ObjectHandler* new_ObjectFactoryEngine (void);
void delete_ObjectFactoryEngine (sup::core::ObjectHandler* engine);

bool ObjectFactoryEngine_Load (sup::core::ObjectHandler* engine, const char * const library);

ccs::base::AnyObject* ObjectFactoryEngine_Instantiate (sup::core::ObjectHandler* engine, const char * const type, const char * const name);

bool ObjectFactoryEngine_Terminate_All (sup::core::ObjectHandler* engine);
bool ObjectFactoryEngine_Terminate_Ref (sup::core::ObjectHandler* engine, ccs::base::AnyObject* ref);
    
// AnyObject methods
bool AnyObject_SetParameter (ccs::base::AnyObject* instance, const char * const name, const char * const value);
bool AnyObject_ProcessMessage (ccs::base::AnyObject* instance, const char * const msg);

#ifdef __cplusplus
} // extern C
#endif // __cplusplus

#endif // _ObjectHandler_h_ 


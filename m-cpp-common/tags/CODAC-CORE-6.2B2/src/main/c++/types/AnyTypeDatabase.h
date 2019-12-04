/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyTypeDatabase.h $
* $Id: AnyTypeDatabase.h 100890 2019-07-19 07:14:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Generic type class definition
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*                                 CS 90 046
*                                 13067 St. Paul-lez-Durance Cedex
*                                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/**
 * @file AnyTypeDatabase.h
 * @brief Header file for AnyTypeDatabase class.
 * @date 11/11/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the singleton AnyTypeDatabase interface.
 * The AnyTypeDatabase class definition is not exposed through this header file. The interface
 * to a singleton instance is provided. Using this interface in an application causes an implicit
 * instantiation of the AnyTypeDatabase class and registration of all built-in ScalarType. Types
 * are stored using smart pointers. This ensures persistence of AnyType definitions throughout the
 * lifetime of an application without requiring the application to manage instances and/or
 * references.
 */

#ifndef _AnyTypeDatabase_h_
#define _AnyTypeDatabase_h_

// Global header files

#include <memory> // std::shared_ptr

// Local header files

#include "log-api.h" // Syslog wrapper routines

#include "LookUpTable.h"

#include "AnyType.h"
#include "ArrayType.h"
#include "CompoundType.h"
#include "ScalarType.h"

// Constants

// Type definition

namespace ccs {

namespace types {

typedef ::ccs::base::LookUpTable<::std::shared_ptr<const AnyType>> AnyTypeDatabase;

// Global variables

namespace GlobalTypeDatabase {

extern AnyTypeDatabase* __p_tdb;

// Function declaration

/**
 * @brief Test if named type is registered to the GlobalTypeDatabase.
 * @return True if type is registered.
 */

bool IsValid (const char8 * const name);

/**
 * @brief Register a type definition to the GlobalTypeDatabase.
 * @detail The type must be defined with a unique name.
 * @param type The type definition to register.
 * @pre
 *   ccs::types::GlobalTypeDatabase::IsValid(type->GetName()) == false
 * @return True if successful.
 *
 * @code
   // Introspectable type definition
   ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyValueType_t/v1.0"))
                                      ->AddAttribute("valid", "boolean")
                                      ->AddAttribute("setpoint", "float64");

   bool status = (static_cast<ccs::types::CompoundType*>(NULL) != type);

   if (status)
     {
       // Register type for future reference
       status = ccs::types::GlobalTypeDatabase::Register(type);
     }

   if (status)
     {
       // Extend type definition .. e.g. with timestamp for archiving purposes
       char type [] = "{\"type\":\"MyOtherType_t/v1.0\","
                       "\"attributes\":["
                                       "{\"timestamp\":{\"type\":\"uint64\"}},"
                                       "{\"value\":{\"type\":\"MyValueType_t/v1.0\"}}"
                                      "]"
                      "}";

       // Register type for future reference
       status = ccs::types::GlobalTypeDatabase::Register(type);
     }
   @endcode
 */

bool Register (const AnyType * const type);
bool Register (const ::std::shared_ptr<const AnyType>& type);
bool Register (const char8 * const type);

/**
 * @brief Retrieve type definition from the GlobalTypeDatabase.
 * @param name The name of the type definition to retrieve.
 * @pre
 *   ccs::types::GlobalTypeDatabase::IsValid(name) == true
 * @return Valid shared pointer if successful.
 */

const std::shared_ptr<const AnyType> GetType (const char8 * const name);

/**
 * @brief Remove a type definition from the GlobalTypeDatabase.
 * @param name The name of the type definition to remove.
 * @pre
 *   ccs::types::GlobalTypeDatabase::IsValid(name) == true
 * @return True if successful.
 */

bool Remove (const char8 * const name);

/**
 * @brief Retrieve type definition from the GlobalTypeDatabase.
 * @detail The returned smart pointer is dynamically cast to the 
 * template Type defined by the application.
 * @param name The name of the type definition to retrieve.
 * @pre
 *   ccs::types::GlobalTypeDatabase::IsValid(name) == true
 * @return Valid shared pointer if successful.
 */

template <typename Type> inline const std::shared_ptr<const Type> GetAsType (const char8 * const name);

// Function definition

/**
 * @return The GlobalTypeDatabase singleton instance.
 * @note Expose built-in type in order to get rid of the AnyTypeDatabase class wrapping
 * the lookup table without much added value.
 * @todo Re-consider hiding the implementation details in case the LookUpTable interface
 * changes in the future.
 */

static inline AnyTypeDatabase* GetInstance (void)
{

  log_trace("GlobalTypeDatabase::GetInstance - Entering method"); 

  if (__builtin_expect((NULL_PTR_CAST(AnyTypeDatabase*) == __p_tdb), 0)) // Unlikely
    {
      __p_tdb = new (std::nothrow) AnyTypeDatabase ();

      if (NULL_PTR_CAST(AnyTypeDatabase*) != __p_tdb)
        {
          // Register scalar types
          (void)Register(::ccs::types::Boolean);
          (void)Register(::ccs::types::Character8);
          (void)Register(::ccs::types::SignedInteger8);
          (void)Register(::ccs::types::UnsignedInteger8);
          (void)Register(::ccs::types::SignedInteger16);
          (void)Register(::ccs::types::UnsignedInteger16);
          (void)Register(::ccs::types::SignedInteger32);
          (void)Register(::ccs::types::UnsignedInteger32);
          (void)Register(::ccs::types::SignedInteger64);
          (void)Register(::ccs::types::UnsignedInteger64);
          (void)Register(::ccs::types::Float32);
          (void)Register(::ccs::types::Float64);
          (void)Register(::ccs::types::String);
        }
    }
  
  log_trace("GlobalTypeDatabase::GetInstance - Leaving method"); 

  return __p_tdb; 

}

template <typename Type> inline const std::shared_ptr<const Type> GetAsType (const char8 * const name)
{

  std::shared_ptr<const Type> type;

  bool status = (true == IsValid(name));

  if (status)
    {
      type = std::dynamic_pointer_cast<const Type>(GetType(name));
      status = static_cast<bool>(type);
    }

  if (!status)
    {
      log_warning("GetAsType<CompoundType> - Invalid type '%s'", name);
    }

  return type;

}

} // namespace GlobalTypeDatabase

} // namespace types

} // namespace ccs

#endif // _AnyTypeDatabase_h_ 


/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyValueHelper.h $
* $Id: AnyValueHelper.h 101447 2019-08-08 07:36:13Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Generic type class definition
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/**
 * @file AnyValueHelper.h
 * @brief Header file for AnyValue helper methods.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyValue helper methods.
 */

#ifndef _AnyValueHelper_h_
#define _AnyValueHelper_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "AnyValue.h"
#include "AnyTypeHelper.h"

// Constants

// Type definition

// Global variables

namespace ccs {

namespace HelperTools {

// Function declaration

/**
 * @todo Documentation.
 * @todo Rename functions for better understanding of scope .. GetAttributeOffset .. GetOffsetAtPath or anything that explicits
 * that the methods would work for any type and not be restricted to CompoundTypes
 */

template <typename Type> inline bool Is (const ccs::types::AnyValue * const value);

static inline bool HasAttribute (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name);
static inline ccs::types::uint32 GetAttributeNumber (const ccs::types::AnyValue * const value);
static inline ccs::types::uint32 GetAttributeOffset (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name);
static inline void* GetAttributeReference (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name);
static inline std::shared_ptr<const ccs::types::AnyType> GetAttributeType (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name);

/**
 * @brief Accessor helper routine.
 * @detail The helper routine will check the validity of the path within type specification and copy to the provided
 * argument in the types match. The path is not restricted to leaf of the type specification.
 *
 * As to type matching, the default implementation provides only verification of the type size and will work for
 * any packed structure or array of the corresponding size. The template is further specialised for all built-in
 * scalar types and verify signedness, etc.
 *
 * Specialisation for:
 *   ccs::types::AnyValue
 *   ccs::types::boolean
 *   ccs::types::char8
 *   ccs::types::int8, ccs::types::uint8
 *   ccs::types::int16, ccs::types::uint16
 *   ccs::types::int32, ccs::types::uint32
 *   ccs::types::int64, ccs::types::uint64
 *   ccs::types::float32, ccs::types::float64
 *
 * @param value Instance of variable with introspectable data type definition.
 * @param name Path within type definition.
 * @param attr Reference to external variable.
 * @return TRUE in case of success, FALSE otherwise.
 * @code
   // ToDo - Example code with scalar and path to leaf ..
   @endcode
 * @code
   // ToDo - Example code with packed structure and intermediate path ..
   @endcode
 * @code
   // ToDo - Example code with AnyValue ..
   @endcode
 */

template <typename Type> inline bool GetAttributeValue (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name, Type& attr);

/**
 * @brief Accessor helper routine.
 * @detail The helper routine will check the validity of the path within type specification and return the
 * appropriate value in case types match. An exception is thrown in case of mismatch.
 * @param value Instance of variable with introspectable data type definition.
 * @param name Path within type definition.
 * @return Value with Type in case of success, std::runtime_error exception thrown in case of error.
 */

template <typename Type> inline Type GetAttributeValue (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name);

/**
 * @brief Accessor helper routine.
 * @detail The helper routine will check the validity of the path within type specification and serialise
 * the value as JSON string into the provided buffer.
 * @param value Instance of variable with introspectable data type definition.
 * @param name Path within type definition.
 * @param attr Buffer provided as placeholder for serialised value.
 * @param size Size of buffer.
 * @return TRUE in case of success, FALSE otherwise.
 */

static inline bool GetAttributeValue (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name, ccs::types::char8 * const attr, const ccs::types::uint32 size);

/**
 * @brief Accessor helper routine.
 * @detail The helper routine will check the validity of the path within type specification and copy from the provided
 * argument in the types match. The path is not restricted to leaf of the type specification.
 *
 * As to type matching, the default implementation provides only verification of the type size and will work for
 * any packed structure or array of the corresponding size. The template is further specialised for all built-in
 * scalar types and verify signedness, etc.
 *
 * Specialisation for:
 *   ccs::types::AnyValue
 *   ccs::types::boolean
 *   ccs::types::char8
 *   ccs::types::int8, ccs::types::uint8
 *   ccs::types::int16, ccs::types::uint16
 *   ccs::types::int32, ccs::types::uint32
 *   ccs::types::int64, ccs::types::uint64
 *   ccs::types::float32, ccs::types::float64
 *
 * @param value Instance of variable with introspectable data type definition.
 * @param name Path within type definition.
 * @param attr Reference to external variable.
 * @return TRUE in case of success, FALSE otherwise.
 * @code
   // ToDo - Example code with scalar and path to leaf ..
   @endcode
 * @code
   // ToDo - Example code with packed structure and intermediate path ..
   @endcode
 * @code
   // ToDo - Example code with AnyValue ..
   @endcode
 */

template <typename Type> inline bool SetAttributeValue (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const Type& attr);

/**
 * @brief Accessor helper routine.
 * @detail The helper routine will check the validity of the path within type specification and parse
 * the value from the JSON string provided.
 * @param value Instance of variable with introspectable data type definition.
 * @param name Path within type definition.
 * @param attr Buffer containing serialised value as JSON string.
 * @return TRUE in case of success, FALSE otherwise.
 */

static inline bool SetAttributeValue (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const ccs::types::char8 * const attr);

static inline bool SetAttributeByReference (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const void * const attr);

static inline bool HasElement (const ccs::types::AnyValue * const value, const ccs::types::uint32 index);

static inline ccs::types::uint32 GetElementNumber (const ccs::types::AnyValue * const value);
static inline void* GetElementReference (const ccs::types::AnyValue * const value, const ccs::types::uint32 index);
static inline std::shared_ptr<const ccs::types::AnyType> GetElementType (const ccs::types::AnyValue * const value);

template <typename Type> inline bool GetElementValue (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, Type& elem);
template <typename Type> inline Type GetElementValue (const ccs::types::AnyValue * const value, const ccs::types::uint32 index);

template <typename Type> inline bool SetElementValue (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, const Type& elem);

// Function definition

template <typename Type> inline bool Is (const ccs::types::AnyValue * const value) { return Is<Type>(value->GetType()); }

static inline bool HasAttribute (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name) 
{ 
  // Recursion on attribute name provided by AnyType Helper routines
  return HasAttribute(value->GetType(), name); 

}

static inline ccs::types::uint32 GetAttributeNumber (const ccs::types::AnyValue * const value)
{

  ccs::types::uint32 number = 0u;

  bool status = Is<ccs::types::CompoundType>(value);

  if (status)
    {
      number = std::dynamic_pointer_cast<const ccs::types::CompoundType>(value->GetType())->GetAttributeNumber();
    }

  return number;

}

static inline ccs::types::uint32 GetAttributeOffset (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name) 
{ 

  return GetAttributeOffset(value->GetType(), name);

}

static inline void* GetAttributeReference (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name) 
{ 

  ccs::types::uint8* ref = NULL_PTR_CAST(ccs::types::uint8*);

  bool status = HasAttribute(value, name);

  if (status)
    {
      // Recursion on attribute name provided by AnyType Helper routines
      ccs::types::uint32 offset = GetAttributeOffset(value->GetType(), name);

      ref = static_cast<ccs::types::uint8*>(value->GetInstance()) + offset;
    }

  return static_cast<void*>(ref);

}

static inline std::shared_ptr<const ccs::types::AnyType> GetAttributeType (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name) 
{ 
  // Recursion on attribute name provided by AnyType Helper routines
  return GetAttributeType(value->GetType(), name);

}

// Default template implementation
template <typename Type> inline bool GetAttributeValue (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name, Type& attr)
{

  bool status = HasAttribute(value, name);

  if (status)
    { // Should work for any packed structure or array of appropriate type
      std::shared_ptr<const ccs::types::AnyType> type = GetAttributeType(value, name);
      status = (static_cast<ccs::types::uint32>(sizeof(Type)) == type->GetSize());
    }

  if (status)
    {
      status = (NULL_PTR_CAST(void*) != GetAttributeReference(value, name));
    }

  if (status)
    {
      (void)memcpy(&attr, GetAttributeReference(value, name), sizeof(Type));
    }

  return status;

}

template <> inline bool GetAttributeValue<ccs::types::AnyValue> (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name, ccs::types::AnyValue& attr)
{

  bool status = HasAttribute(value, name);

  if (status)
    {
      void* attr_inst = GetAttributeReference(value, name);
      std::shared_ptr<const ccs::types::AnyType> attr_type = GetAttributeType(value, name);

      ccs::types::AnyValue __attr (attr_type, attr_inst);

      attr = __attr; // Copy assignment
    }
  
  return status;

}

template <> inline ccs::types::boolean GetAttributeValue<ccs::types::boolean> (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name, ccs::types::boolean& attr)
{

  bool status = HasAttribute(value, name);

  if (status)
    {
      status = Is<ccs::types::ScalarType>(GetAttributeType(value, name));
    }

  if (status)
    {
      status = (NULL_PTR_CAST(void*) != GetAttributeReference(value, name));
    }

  if (status)
    {
      if (ccs::types::Boolean == GetAttributeType(value, name))
        {
          attr = *static_cast<ccs::types::boolean*>(GetAttributeReference(value, name));
        }
      else if (ccs::types::Character8 == GetAttributeType(value, name))
        {
          attr = !(*static_cast<ccs::types::char8*>(GetAttributeReference(value, name)) == static_cast<ccs::types::char8>(0));
        }
      else if (ccs::types::UnsignedInteger8 == GetAttributeType(value, name))
        {
          attr = !(*static_cast<ccs::types::uint8*>(GetAttributeReference(value, name)) == static_cast<ccs::types::uint8>(0));
        }
      else if (ccs::types::UnsignedInteger16 == GetAttributeType(value, name))
        {
          attr = !(*static_cast<ccs::types::uint16*>(GetAttributeReference(value, name)) == static_cast<ccs::types::uint16>(0));
        }
      else if (ccs::types::UnsignedInteger32 == GetAttributeType(value, name))
        {
          attr = !(*static_cast<ccs::types::uint32*>(GetAttributeReference(value, name)) == static_cast<ccs::types::uint32>(0));
        }
      else if (ccs::types::UnsignedInteger64 == GetAttributeType(value, name))
        {
          attr = !(*static_cast<ccs::types::uint64*>(GetAttributeReference(value, name)) == static_cast<ccs::types::uint64>(0));
        }
      else
        {
          status = false;
        }
    }

  return status;

}

#define GETATTRIBUTEVALUEBYREFERENCE(BASIC_TYPE,INTRO_TYPE) \
template <> inline bool GetAttributeValue<BASIC_TYPE> (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name, BASIC_TYPE& attr) \
{ \
\
  bool status = HasAttribute(value, name); \
\
  if (status) \
    { \
      status = (INTRO_TYPE == GetAttributeType(value, name)); \
    } \
\
  if (status) \
    { \
      status = (NULL_PTR_CAST(void*) != GetAttributeReference(value, name)); \
    } \
\
  if (status) \
    { \
      attr = *static_cast<BASIC_TYPE*>(GetAttributeReference(value, name)); \
    } \
\
  return status; \
\
}

GETATTRIBUTEVALUEBYREFERENCE(ccs::types::char8, ccs::types::Character8)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::int8, ccs::types::SignedInteger8)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::uint8, ccs::types::UnsignedInteger8)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::int16, ccs::types::SignedInteger16)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::uint16, ccs::types::UnsignedInteger16)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::int32, ccs::types::SignedInteger32)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::uint32, ccs::types::UnsignedInteger32)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::int64, ccs::types::SignedInteger64)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::uint64, ccs::types::UnsignedInteger64)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::float32, ccs::types::Float32)
GETATTRIBUTEVALUEBYREFERENCE(ccs::types::float64, ccs::types::Float64)

template <typename Type> inline Type GetAttributeValue (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name)
{

  Type attr;

  bool status = GetAttributeValue<Type>(value, name, attr);

  if (!status)
    {
      log_error("GetAttributeValue<Type> - Invalid attribute path '%s' or type mismatch", name);
      throw std::runtime_error("Invalid attribute path or type mismatch");
    }

  return attr;

}

static inline bool GetAttributeValue (const ccs::types::AnyValue * const value, const ccs::types::char8 * const name, ccs::types::char8 * const attr, const ccs::types::uint32 size)
{

  bool status = HasAttribute(value, name);

  if (status)
    {
      status = Is<ccs::types::ScalarType>(GetAttributeType(value, name));
    }

  if (status)
    {
      status = (NULL_PTR_CAST(void*) != GetAttributeReference(value, name));
    }

  if (status)
    {
      if (ccs::types::String == GetAttributeType(value, name))
        {
          (void)ccs::HelperTools::SafeStringCopy(attr, static_cast<ccs::types::char8*>(GetAttributeReference(value, name)), size);
        }
      else
        {
          status = SerialiseInstance(GetAttributeType(value, name), GetAttributeReference(value, name), attr, size);
        }
    }

  return status;

}

template <typename Type> inline bool SetAttributeValue (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const Type& attr)
{

  bool status = HasAttribute(value, name);

  if (status)
    { // Should work for any packed structure or array of appropriate type
      std::shared_ptr<const ccs::types::AnyType> type = GetAttributeType(value, name);
      status = (static_cast<ccs::types::uint32>(sizeof(Type)) == type->GetSize());
    }

  if (status)
    {
      status = (NULL_PTR_CAST(void*) != GetAttributeReference(value, name));
    }

  if (status)
    {
      (void)memcpy(GetAttributeReference(value, name), &attr, sizeof(Type));
    }

  return status;

}

template <> inline bool SetAttributeValue<ccs::types::AnyValue> (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const ccs::types::AnyValue& attr)
{

  bool status = HasAttribute(value, name);

  ccs::types::uint32 size = 0u;
  
  if (status)
    {
      std::shared_ptr<const ccs::types::AnyType> type = GetAttributeType(value, name);
      size = type->GetSize();
    }

  if (status)
    {
      std::shared_ptr<const ccs::types::AnyType> type = attr.GetType();
      status = (type->GetSize() == size);
    }

  void* ref = NULL_PTR_CAST(void*);

  if (status)
    {
      ref = GetAttributeReference(value, name);
      status = ((NULL_PTR_CAST(void*) != attr.GetInstance()) &&
                (NULL_PTR_CAST(void*) != ref));
    }

  if (status)
    {
      (void)memcpy(ref, attr.GetInstance(), size);
    }

  return status;

}

#define SETATTRIBUTEVALUE(BASIC_TYPE,INTRO_TYPE) \
template <> inline bool SetAttributeValue<BASIC_TYPE> (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const BASIC_TYPE& attr) \
{ \
\
  bool status = HasAttribute(value, name); \
\
  if (status) \
    { \
      status = (INTRO_TYPE == GetAttributeType(value, name)); \
    } \
\
  if (status) \
    { \
      status = (NULL_PTR_CAST(void*) != GetAttributeReference(value, name)); \
    } \
\
  if (status) \
    { \
      *static_cast<BASIC_TYPE*>(GetAttributeReference(value, name)) = attr; \
    } \
\
  return status; \
\
}

SETATTRIBUTEVALUE(ccs::types::boolean, ccs::types::Boolean)
SETATTRIBUTEVALUE(ccs::types::char8, ccs::types::Character8)
SETATTRIBUTEVALUE(ccs::types::int8, ccs::types::SignedInteger8)
SETATTRIBUTEVALUE(ccs::types::uint8, ccs::types::UnsignedInteger8)
SETATTRIBUTEVALUE(ccs::types::int16, ccs::types::SignedInteger16)
SETATTRIBUTEVALUE(ccs::types::uint16, ccs::types::UnsignedInteger16)
SETATTRIBUTEVALUE(ccs::types::int32, ccs::types::SignedInteger32)
SETATTRIBUTEVALUE(ccs::types::uint32, ccs::types::UnsignedInteger32)
SETATTRIBUTEVALUE(ccs::types::int64, ccs::types::SignedInteger64)
SETATTRIBUTEVALUE(ccs::types::uint64, ccs::types::UnsignedInteger64)
SETATTRIBUTEVALUE(ccs::types::float32, ccs::types::Float32)
SETATTRIBUTEVALUE(ccs::types::float64, ccs::types::Float64)

static inline bool SetAttributeValue (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const ccs::types::char8 * const attr)
{

  bool status = HasAttribute(value, name);

  if (status)
    {
      status = Is<ccs::types::ScalarType>(GetAttributeType(value, name));
    }

  if (status)
    {
      status = (NULL_PTR_CAST(void*) != GetAttributeReference(value, name));
    }

  if (status)
    {
      if (ccs::types::String == GetAttributeType(value, name))
        {
          (void)ccs::HelperTools::SafeStringCopy(static_cast<ccs::types::char8*>(GetAttributeReference(value, name)), attr, STRING_MAX_LENGTH);
        }
      else
        {
          status = (0u < ParseInstance(GetAttributeType(value, name), GetAttributeReference(value, name), attr));
        }
    }

  return status;

}

static inline bool SetAttributeByReference (ccs::types::AnyValue * const value, const ccs::types::char8 * const name, const void * const attr)
{

  bool status = HasAttribute(value, name);

  ccs::types::uint32 size = 0u;
  
  if (status)
    {
      std::shared_ptr<const ccs::types::AnyType> type = GetAttributeType(value, name);
      size = type->GetSize();
    }

  void* ref = NULL_PTR_CAST(void*);

  if (status)
    {
      ref = GetAttributeReference(value, name);
      status = ((NULL_PTR_CAST(void*) != ref) &&
                (NULL_PTR_CAST(void*) != attr));
    }

  if (status)
    {
      (void)memcpy(ref, attr, size);
    }

  return status;

}

static inline bool HasElement (const ccs::types::AnyValue * const value, const ccs::types::uint32 index) 
{ 

  bool status = (index < GetElementNumber(value));

  return status; 

}

static inline ccs::types::uint32 GetElementNumber (const ccs::types::AnyValue * const value)
{

  ccs::types::uint32 number = 0u;

  bool status = Is<ccs::types::ArrayType>(value);

  if (status)
    {
      number = std::dynamic_pointer_cast<const ccs::types::ArrayType>(value->GetType())->GetElementNumber();
    }

  return number;

}

static inline void* GetElementReference (const ccs::types::AnyValue * const value, const ccs::types::uint32 index) 
{ 

  ccs::types::uint8* ref = NULL_PTR_CAST(ccs::types::uint8*);

  bool status = HasElement(value, index);

  if (status)
    {
      ccs::types::uint32 offset = std::dynamic_pointer_cast<const ccs::types::ArrayType>(value->GetType())->GetElementOffset(index);
      ref = static_cast<ccs::types::uint8*>(value->GetInstance()) + offset;
    }

  return static_cast<void*>(ref);

}

static inline std::shared_ptr<const ccs::types::AnyType> GetElementType (const ccs::types::AnyValue * const value) 
{ 

  std::shared_ptr<const ccs::types::AnyType> type;

  bool status = Is<ccs::types::ArrayType>(value->GetType());

  if (status)
    {
      type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(value->GetType())->GetElementType();
    }

  return type;

}

template <typename Type> inline bool GetElementValue (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, Type& elem)
{

  bool status = HasElement(value, index);

  if (status)
    { // Should work for any packed structure or array of appropriate type
      std::shared_ptr<const ccs::types::AnyType> type = GetElementType(value);
      status = (static_cast<ccs::types::uint32>(sizeof(Type)) == type->GetSize());
    }

  if (status)
    {
      status = (NULL_PTR_CAST(void*) != GetElementReference(value, index));
    }

  if (status)
    {
      (void)memcpy(&elem, GetElementReference(value, index), sizeof(Type));
    }

  return status;

}

template <> inline bool GetElementValue<ccs::types::AnyValue> (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, ccs::types::AnyValue& elem)
{

  bool status = HasElement(value, index);

  if (status)
    {
      void* elem_inst = GetElementReference(value, index);
      std::shared_ptr<const ccs::types::AnyType> elem_type = GetElementType(value);

      ccs::types::AnyValue __elem (elem_type, elem_inst);

      elem = __elem; // Copy assignment
    }

  return status;

}

#define GETELEMENTVALUEBYREFERENCE(BASIC_TYPE,INTRO_TYPE) \
template <> inline bool GetElementValue<BASIC_TYPE> (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, BASIC_TYPE& elem) \
{ \
\
  bool status = HasElement(value, index); \
\
  if (status) \
    { \
      status = (INTRO_TYPE == GetElementType(value)); \
    } \
\
  if (status) \
    { \
      status = (NULL_PTR_CAST(void*) != GetElementReference(value, index)); \
    } \
\
  if (status) \
    { \
      elem = *static_cast<BASIC_TYPE*>(GetElementReference(value, index)); \
    } \
\
  return status; \
\
}

GETELEMENTVALUEBYREFERENCE(ccs::types::boolean, ccs::types::Boolean)
GETELEMENTVALUEBYREFERENCE(ccs::types::char8, ccs::types::Character8)
GETELEMENTVALUEBYREFERENCE(ccs::types::int8, ccs::types::SignedInteger8)
GETELEMENTVALUEBYREFERENCE(ccs::types::uint8, ccs::types::UnsignedInteger8)
GETELEMENTVALUEBYREFERENCE(ccs::types::int16, ccs::types::SignedInteger16)
GETELEMENTVALUEBYREFERENCE(ccs::types::uint16, ccs::types::UnsignedInteger16)
GETELEMENTVALUEBYREFERENCE(ccs::types::int32, ccs::types::SignedInteger32)
GETELEMENTVALUEBYREFERENCE(ccs::types::uint32, ccs::types::UnsignedInteger32)
GETELEMENTVALUEBYREFERENCE(ccs::types::int64, ccs::types::SignedInteger64)
GETELEMENTVALUEBYREFERENCE(ccs::types::uint64, ccs::types::UnsignedInteger64)
GETELEMENTVALUEBYREFERENCE(ccs::types::float32, ccs::types::Float32)
GETELEMENTVALUEBYREFERENCE(ccs::types::float64, ccs::types::Float64)

template <typename Type> inline Type GetElementValue (const ccs::types::AnyValue * const value, const ccs::types::uint32 index)
{

  Type elem;

  bool status = GetElementValue<Type>(value, index, elem);

  if (!status)
    {
      log_error("GetElementValue<Type> - Invalid element '%u' or type mismatch", index);
      throw std::runtime_error("Invalid element number or type mismatch");
    }

  return elem;

}

template <typename Type> inline bool SetElementValue (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, const Type& elem)
{

  bool status = HasElement(value, index);

  if (status)
    { // Should work for any packed structure or array of appropriate type
      std::shared_ptr<const ccs::types::AnyType> type = GetElementType(value);
      status = (static_cast<ccs::types::uint32>(sizeof(Type)) == type->GetSize());
    }

  if (status)
    {
      status = (NULL_PTR_CAST(void*) != GetElementReference(value, index));
    }

  if (status)
    {
      (void)memcpy(GetElementReference(value, index), &elem, sizeof(Type));
    }

  return status;

}

template <> inline bool SetElementValue<ccs::types::AnyValue> (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, const ccs::types::AnyValue& elem)
{

  bool status = HasElement(value, index);

  ccs::types::uint32 size = 0u;
  
  if (status)
    {
      std::shared_ptr<const ccs::types::AnyType> type = GetElementType(value);
      size = type->GetSize();
    }

  if (status)
    {
      std::shared_ptr<const ccs::types::AnyType> type = elem.GetType();
      status = (type->GetSize() == size);
    }

  void* ref = NULL_PTR_CAST(void*);

  if (status)
    {
      ref = GetElementReference(value, index);
      status = ((NULL_PTR_CAST(void*) != elem.GetInstance()) &&
                (NULL_PTR_CAST(void*) != ref));
    }

  if (status)
    {
      (void)memcpy(ref, elem.GetInstance(), size);
    }

  return status;

}

#define SETELEMENTVALUE(BASIC_TYPE,INTRO_TYPE) \
template <> inline bool SetElementValue<BASIC_TYPE> (const ccs::types::AnyValue * const value, const ccs::types::uint32 index, const BASIC_TYPE& elem) \
{ \
\
  bool status = HasElement(value, index); \
\
  if (status) \
    { \
      status = (INTRO_TYPE == GetElementType(value)); \
    } \
\
  if (status) \
    { \
      status = (NULL_PTR_CAST(void*) != GetElementReference(value, index)); \
    } \
\
  if (status) \
    { \
      *static_cast<BASIC_TYPE*>(GetElementReference(value, index)) = elem; \
    } \
\
  return status; \
\
}

SETELEMENTVALUE(ccs::types::boolean, ccs::types::Boolean)
SETELEMENTVALUE(ccs::types::char8, ccs::types::Character8)
SETELEMENTVALUE(ccs::types::int8, ccs::types::SignedInteger8)
SETELEMENTVALUE(ccs::types::uint8, ccs::types::UnsignedInteger8)
SETELEMENTVALUE(ccs::types::int16, ccs::types::SignedInteger16)
SETELEMENTVALUE(ccs::types::uint16, ccs::types::UnsignedInteger16)
SETELEMENTVALUE(ccs::types::int32, ccs::types::SignedInteger32)
SETELEMENTVALUE(ccs::types::uint32, ccs::types::UnsignedInteger32)
SETELEMENTVALUE(ccs::types::int64, ccs::types::SignedInteger64)
SETELEMENTVALUE(ccs::types::uint64, ccs::types::UnsignedInteger64)
SETELEMENTVALUE(ccs::types::float32, ccs::types::Float32)
SETELEMENTVALUE(ccs::types::float64, ccs::types::Float64)

static bool CopyOver (ccs::types::AnyValue * const dst, const ccs::types::AnyValue * const src)
{

  bool status = ((dst->GetSize() == src->GetSize()) && 
                 (GetAttributeNumber(dst) == GetAttributeNumber(src)));

  if (status && (0u == GetAttributeNumber(dst))) // Not a structure anymore
    {
      // Copy operation
      *dst = *src;
    }

  for (ccs::types::uint32 index = 0u; (index < GetAttributeNumber(dst)) && status; index++)
    {
      ccs::types::string attr_name; 
      (void)ccs::HelperTools::SafeStringCopy(attr_name, std::dynamic_pointer_cast<const ccs::types::CompoundType>(dst->GetType())->GetAttributeName(index), ccs::types::MaxStringLength);

      // Check source has an attribute named
      status = HasAttribute(src, attr_name);

      if (status)
        {
          ccs::types::AnyValue dst_attr (GetAttributeType(dst, attr_name), GetAttributeReference(dst, attr_name));
          ccs::types::AnyValue src_attr (GetAttributeType(src, attr_name), GetAttributeReference(src, attr_name));

          status = CopyOver(&dst_attr, &src_attr);
        }
    }

  return status;

}

static inline void LogSerialisedType (const ccs::types::AnyValue * const value)
{

  bool status = (NULL_PTR_CAST(const ccs::types::AnyValue *) != value);

  if (status)
    {
      ccs::types::char8 buffer [1024];

      (void)Serialise(value->GetType(), buffer, 1024u);

      log_info("Serialised type ..");
      log_info(".. '%s'", buffer);
    }

  return;

}

static inline void LogSerialisedInstance (const ccs::types::AnyValue * const value)
{

  bool status = (NULL_PTR_CAST(const ccs::types::AnyValue *) != value);

  if (status)
    {
      ccs::types::char8 buffer [1024];

      (void)SerialiseInstance(value->GetType(), value->GetInstance(), buffer, 1024u);

      log_info("Serialised instance ..");
      log_info(".. '%s'", buffer);
    }

  return;

}

} // namespace HelperTools

} // namespace ccs

#endif // _AnyValueHelper_h_ 


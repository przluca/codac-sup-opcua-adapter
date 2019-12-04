/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyTypeHelper.h $
* $Id: AnyTypeHelper.h 102953 2019-09-30 08:52:09Z bauvirb $
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
 * @file AnyTypeHelper.h
 * @brief Header file for AnyType helper methods.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyType helper methods.
 */

#ifndef _AnyTypeHelper_h_
#define _AnyTypeHelper_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

// Local header files

#include "log-api.h" // Syslog wrapper routines
#include "SysTools.h" // ccs::HelperTools::IsIntegerString, etc.

#include "AnyType.h"
#include "ArrayType.h"
#include "CompoundType.h"
#include "ScalarType.h"

#include "AnyTypeDatabase.h"

// Constants

// Type definition

namespace ccs {

namespace HelperTools {

// Global variables

// Function declaration

/**
 * @brief Test AnyType against specialised type.
 * @param type Pointer to AnyType instance
 * @return true if instance is of specialised type.
 */

template <typename Type> inline bool Is (const std::shared_ptr<const ccs::types::AnyType>& type);
template <typename Type> inline bool Is (const ccs::types::AnyType * const type);
template <typename Type> inline bool Is (const ccs::types::char8 * const buffer); // JSON type description

static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const ccs::types::char8 * const base, const ccs::types::uint32 multiplicity = 0u); // Registered name
static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const std::shared_ptr<const ccs::types::AnyType>& base, const ccs::types::uint32 multiplicity = 0u); // Specialised class depends on base type
static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const ccs::types::AnyType * const base, const ccs::types::uint32 multiplicity = 0u); // Specialised class depends on base type
static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const ccs::types::AnyType& base, const ccs::types::uint32 multiplicity = 0u); // Specialised class depends on base type

/**
 * @brief Parse AnyType routine.
 * @detail The routine tests if the path is valid within the defined type.
 * @param type Pointer to AnyType instance
 * @param name Attribute fully-qualified name, i.e. path within AnyType instance.
 * @return true if attribute exists within the defined type.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static bool HasAttribute (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name);

/**
 * @brief Parse AnyType routine.
 * @detail The routine provides the offset in memory corresponding to the defined
 * type.
 * @param type Pointer to AnyType instance
 * @param name Attribute fully-qualified name, i.e. path within AnyType instance.
 * @return Offset within memory if attribute exists in the defined type, 0u otherwise.
 * @note The routine returns 0u in case of invalid attribute name. The calling application
 * should itself perform test that the name is valid to discriminate between first
 * attribute and error condition.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static ccs::types::uint32 GetAttributeOffset (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name);

/**
 * @brief Parse AnyType routine.
 * @detail The routine returns the size (memory footprint) of the AnyType instance
 * corresponding to the attribute.
 * @param type Pointer to AnyType instance
 * @param name Attribute fully-qualified name, i.e. path within AnyType instance.
 * @return Size of the AnyType instance if attribute exists in the defined type, 0u otherwise.
 * @note The routine returns 0u in case of invalid attribute name. The calling application
 * should itself perform test that the name is valid to discriminate between first
 * attribute and error condition.
 */

static inline ccs::types::uint32 GetAttributeSize (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name);

/**
 * @brief Parse AnyType routine.
 * @detail The routine returns the AnyType instance corresponding to the attribute.
 * @param type Pointer to AnyType instance
 * @param name Attribute fully-qualified name, i.e. path within AnyType instance.
 * @return AnyType instance if attribute exists in the defined type, invalid pointer otherwise.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static std::shared_ptr<const ccs::types::AnyType> GetAttributeType (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name);

/**
 * @brief Parse JSON stream and fill AnyType instance.
 * @detail The routine parses a JSON type description string and instantiates the appropriate AnyType
 * corresponding to the description.
 * @param type Pointer where the  AnyType instance will be stored.
 * @param buffer A JSON type description string.
 * @pre
 *   false == static_cast<bool>(type) &&
 *   NULL != buffer &&
 *   JSON string represents a type description * @return Number of consumed characters, 0 in case of error.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::AnyType>& type, const ccs::types::char8 * const buffer);
static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::ArrayType>& type, const ccs::types::char8 * const buffer);
static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::CompoundType>& type, const ccs::types::char8 * const buffer);
static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::ScalarType>& type, const ccs::types::char8 * const buffer);

/**
 * @brief Serialisation routine.
 * @detail The routine parses a human-readable JSON string into a memory buffer.
 * @param type Pointer to AnyType instance
 * @param ref Pointer to memory location
 * @param buffer Input string
 * @pre
 *   true == static_cast<bool>(type) &&
 *   NULL != ref && NULL != buffer &&
 *   JSON string represents the specified AnyType
 * @return Number of consumed characters, 0u otherwise.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::AnyType>& type, void * const ref, const ccs::types::char8 * const buffer);
static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::ArrayType>& type, void * const ref, const ccs::types::char8 * const buffer);
static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::CompoundType>& type, void * const ref, const ccs::types::char8 * const buffer);
static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::ScalarType>& type, void * const ref, const ccs::types::char8 * const buffer);

/**
 * @brief Serialisation routine.
 * @detail The routine provides a human-readable JSON string representing an AnyType instance.
 * @param type Pointer to AnyType instance
 * @param buffer Output string
 * @param size Output string maximum size
 * @pre
 *   true == static_cast<bool>(type) &&
 *   NULL != buffer
 * @return true if pre-conditions are met, false otherwise.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static bool Serialise (const std::shared_ptr<const ccs::types::AnyType>& type, ccs::types::char8 * const buffer, const ccs::types::uint32 size);
static bool Serialise (const std::shared_ptr<const ccs::types::ArrayType>& type, ccs::types::char8 * const buffer, ccs::types::uint32 size);
static bool Serialise (const std::shared_ptr<const ccs::types::CompoundType>& type, ccs::types::char8 * const buffer, ccs::types::uint32 size);
static bool Serialise (const std::shared_ptr<const ccs::types::ScalarType>& type, ccs::types::char8 * const buffer, ccs::types::uint32 size);

/**
 * @brief Serialisation routine.
 * @detail The routine provides a human-readable JSON string representing the content of a memory buffer.
 * @param type Pointer to AnyType instance
 * @param ref Pointer to memory location
 * @param buffer Output string
 * @param size Output string maximum size
 * @pre
 *   true == static_cast<bool>(type) &&
 *   NULL != ref && NULL != buffer
 * @return true if pre-conditions are met, false otherwise.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static bool SerialiseInstance (const std::shared_ptr<const ccs::types::AnyType>& type, const void * const ref, ccs::types::char8 * const buffer, const ccs::types::uint32 size);
static bool SerialiseInstance (const std::shared_ptr<const ccs::types::ArrayType>& type, const void * const ref, ccs::types::char8 * const buffer, ccs::types::uint32 size);
static bool SerialiseInstance (const std::shared_ptr<const ccs::types::CompoundType>& type, const void * const ref, ccs::types::char8 * const buffer, ccs::types::uint32 size);
static bool SerialiseInstance (const std::shared_ptr<const ccs::types::ScalarType>& type, const void * const ref, ccs::types::char8 * const buffer, ccs::types::uint32 size);

/**
 * @brief Type conversion routine.
 * @detail The routine converts an AnyType instance to a flattened structure.
 * @param type Pointer to AnyType instance
 * @pre
 *   true == static_cast<bool>(type)
 * @return CompoundType instance upon success, invalid pointer otherwise.
 * @note The routine follows a recursion pattern and can therefore not be inline.
 */

static std::shared_ptr<const ccs::types::CompoundType> Flatten (const std::shared_ptr<const ccs::types::AnyType>& type);
static std::shared_ptr<const ccs::types::CompoundType> Flatten (const std::shared_ptr<const ccs::types::ArrayType>& type);
static std::shared_ptr<const ccs::types::CompoundType> Flatten (const std::shared_ptr<const ccs::types::CompoundType>& type);

/**
 * @brief Logs contents on the GlobalTypeDatabase for investigation purposes.
 */

static inline void DumpGTDBToLog (void);

// Function definition

template <typename Type> inline bool Is (const std::shared_ptr<const ccs::types::AnyType>& type) 
{ 

  return static_cast<bool>(std::dynamic_pointer_cast<const Type>(type)); 

}

template <typename Type> inline bool Is (const ccs::types::AnyType * const type) 
{ 

  return (NULL_PTR_CAST(const Type * const) != dynamic_cast<const Type * const>(type)); 

}

static inline bool GetAttributeFromJSONContent (const ccs::types::char8 * const buffer, const ccs::types::char8 * const name, ccs::types::char8 * const attr, const ccs::types::uint32 size)
{

  bool status = ((NULL_PTR_CAST(const ccs::types::char8*) != buffer) &&
                 (NULL_PTR_CAST(const ccs::types::char8*) != name));

  ccs::types::uint32 name_size = static_cast<ccs::types::uint32>(strlen(name));

  if (status)
    {
      log_debug("GetAttributeFromJSONContent - Try and parse '%s'", buffer);
    }

  if (status)
    {
      status = (0u < ccs::HelperTools::Count(buffer, "{"));
    }

  const ccs::types::char8 * p_buf = buffer;

  if (status)
    {
      while ((*p_buf != 0) && (*p_buf != '{')) { p_buf++; }
      status = (*p_buf == '{');
    }

  ccs::types::int32 outer = -1;

  if (status)
    {
      outer = ccs::HelperTools::FindMatchingBrace(p_buf);
      status = (-1 != outer);
    }

  if (status) // p_buf points to '{', outer points to the matching '}'
    {
      p_buf++; outer--;
    }

  if (status) // p_buf points after '{'
    {
      // Contains 'attr' at first level
      while ((*p_buf != 0) && (*p_buf != '{') && (*p_buf != '[') && (*p_buf != ']') && (*p_buf != '}') && !ccs::HelperTools::StringCompare(p_buf, name, name_size)) { p_buf++; outer--; }
      status = (*p_buf != 0);
    }

  if (status) // p_buf points to 'attr' .. or a brace
    {
      if (!ccs::HelperTools::StringCompare(p_buf, name, name_size)) // p_buf points to a brace
        { // Skip till matching closing inner brace
          ccs::types::int32 inner = ccs::HelperTools::FindMatchingBrace(p_buf);
          status = ((-1 != inner) && (outer > inner));

          if (status)
            {
              p_buf += inner; outer -= inner;
              while ((*p_buf != 0) && !ccs::HelperTools::StringCompare(p_buf, name, name_size)) { p_buf++; outer--; }
            }
        }

      status = (ccs::HelperTools::StringCompare(p_buf, name, name_size) && (*(p_buf + name_size) == '"')); // Risk of prefix
   }

  if (status) // p_buf points to 'attr' .. for sure
    {
      while ((*p_buf != 0) && (*p_buf != '{') && (*p_buf != '[') && (*p_buf != ']') && (*p_buf != '}') && (*p_buf != ':')) { p_buf++; outer--; }
      status = (*p_buf == ':');
    }

  if (status) // p_buf points to ':'
    {
      // Extract attribute value .. may be numeric literal, string literal, array or object
      while ((*p_buf != 0) && (*p_buf != '{') && (*p_buf != '[') && (*p_buf != '"') && (ccs::HelperTools::IsAlphaNumeric(*p_buf) != true)) { p_buf++; outer--; }
      status = (*p_buf != 0);
    }

  if (status) // p_buf points to 'value'
    {
      ccs::types::int32 close = -1;

      if (*p_buf == '"') // String literal ..
        {
          close = ccs::HelperTools::Find(p_buf, "\"", 1u);
          status = ((close != -1) && (close < outer)); // There is a '"' before the closing outer brace
        }
      else if (*p_buf == '{') // Object ..
        {
          close = ccs::HelperTools::FindMatchingBrace(p_buf);
          status = ((close != -1) && (close < outer)); // There is a '}' before the closing outer brace
        }
      else if (*p_buf == '[') // Array ..
        {
          close = ccs::HelperTools::FindMatchingBrace(p_buf);
          status = ((close != -1) && (close < outer)); // There is a ']' before the closing outer brace
        }
      else if (true == ccs::HelperTools::IsAlphaNumeric(*p_buf)) // Numeric literal, incl. boolean
        {
          close = 0;
          while ((*(p_buf + close) != 0) && (*(p_buf + close) != '}') && (*(p_buf + close) != ']') && (*(p_buf + close) != ',')) { close++; }
          status = (*(p_buf + close) != 0);

          if (status)
            {
              close--;
            }
        }

      if (status)
        {
          (void)ccs::HelperTools::SafeStringCopy(attr, p_buf, size);

          if (close < static_cast<ccs::types::int32>(size))
            {
              *(attr + close + 1) = 0;
            }
        }
    }

  if (status)
    {
      log_debug("GetAttributeFromJSONContent - Found attribute '%s':'%s'", name, attr);
    }

  return status;

}

static inline bool GetTypeNameFromJSONContent (const ccs::types::char8 * const buffer, ccs::types::char8 * const name, const ccs::types::uint32 size)
{

  bool status = (NULL_PTR_CAST(ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("GetTypeNameFromJSONContent - Try and parse '%s'", buffer);
    }

  if (status)
    {
      ccs::types::string type;

      status = GetAttributeFromJSONContent(buffer, "type", type, ccs::types::MaxStringLength);

      if (status) // Type name includes '"'
        {
          status = ccs::HelperTools::Strip(type, "\"");
          (void)SafeStringCopy(name, type, size);
        }
    }

  if (status)
    {
      log_debug("GetTypeNameFromJSONContent - Found type name '%s'", name);
    }

  return status;

}

template <> inline bool Is<ccs::types::ArrayType> (const ccs::types::char8 * const buffer) 
{ 

  bool status = (NULL_PTR_CAST(const ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("Is<ccs::types::ArrayType> - Try and parse '%s'", buffer);
    }

  // Should be supporting the following cases:
  //   Named types '{"type":"MyArrayType","multiplicity":4,"element":{"type":..}}'
  //   Named types with out-of-order type name '{"multiplicity":4,"element":{"type":..},"type":"MyArrayType"}' .. need to count braces.
  //   Unnamed types '{"multiplicity":4,"element":{"type":..}}'
  //   Already registered types '{"type":"MyArrayType"}'
  //   Out-of-order declaration '{"type":"MyArrayType","element":{"type":..},"multiplicity":4}'
  //   Indented declaration '{"type":"MyArrayType",\n
  //                          "element":{"type":..},\n
  //                          "multiplicity":4\n
  //                         }'
  //   First of a list of types ':{"type":"MyArrayType","multiplicity":4,"element":{"type":..}},"attribute":{"type":..},..'
  //
  // Let's not consider the cases:
  //
  // May or may not contain a '"type":"name"' attribute.
  // The type 'name' may or may not be registered in the GlobalTypeDatabase.

  if (status)
    { // Extract type name
      ccs::types::string name = STRING_UNDEFINED; 

      status = GetTypeNameFromJSONContent(buffer, name, ccs::types::MaxStringLength);

      if (status)
        {
          status = ccs::types::GlobalTypeDatabase::IsValid(name);
        }

      if (status)
        {
          log_debug("Is<ccs::types::ArrayType> - Name '%s' is registered .. return", name);
          return Is<ccs::types::ArrayType>(ccs::types::GlobalTypeDatabase::GetType(name));
        }
      else
        {
          log_debug("Is<ccs::types::ArrayType> - Name '%s' is not registered .. continue", name);
          status = true;
        }
    }

  // Reaching here, we may have or not a 'type' name but we are sure the type is not registered ..
  // .. test the rest of the type specification 

  const ccs::types::char8 * p_buf = buffer;

  if (status)
    {
      while ((*p_buf != 0) && (*p_buf != '{')) { p_buf++; }
      status = (*p_buf == '{');
    }

  if (status) // p_buf points to '{'
    { // Must have 'element' and 'multiplicity'
      status = ((true == ccs::HelperTools::Contain(p_buf, "elem")) &&
                (true == ccs::HelperTools::Contain(p_buf, "mult")) &&
                (ccs::HelperTools::Find(p_buf, "elem") < ccs::HelperTools::Locate(p_buf, "{", 2u)));
    }
  
  return status;

}

template <> inline bool Is<ccs::types::CompoundType> (const ccs::types::char8 * const buffer) 
{ 

  bool status = (NULL_PTR_CAST(const ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("Is<ccs::types::CompoundType> - Try and parse '%s'", buffer);
    }

  if (status)
    { // Extract type name
      ccs::types::string name = STRING_UNDEFINED; 

      status = GetTypeNameFromJSONContent(buffer, name, ccs::types::MaxStringLength);

      if (status)
        {
          status = ccs::types::GlobalTypeDatabase::IsValid(name);
        }

      if (status)
        {
          log_debug("Is<ccs::types::CompoundType> - Name '%s' is registered .. return", name);
          return Is<ccs::types::CompoundType>(ccs::types::GlobalTypeDatabase::GetType(name));
        }
      else
        {
          log_debug("Is<ccs::types::CompoundType> - Name '%s' is not registered .. continue", name);
          status = true;
        }
    }

  // Reaching here, we may have or not a 'type' name but we are sure the type is not registered ..
  // .. test the rest of the type specification 

  const ccs::types::char8 * p_buf = buffer;

  if (status)
    {
      while ((*p_buf != 0) && (*p_buf != '{')) { p_buf++; }
      status = (*p_buf == '{');
    }

  if (status) // p_buf points to '{'
    { // Must have 'attributes'
      status = ((true == ccs::HelperTools::Contain(p_buf, "attr")) &&
                (ccs::HelperTools::Find(p_buf, "attr") < ccs::HelperTools::Find(p_buf, "[")));
    }

  return status;

}

template <> inline bool Is<ccs::types::ScalarType> (const ccs::types::char8 * const buffer) 
{ 

  bool status = (NULL_PTR_CAST(const ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("Is<ccs::types::ScalarType> - Try and parse '%s'", buffer);
    }

  if (status)
    { // Extract type name
      ccs::types::string name = STRING_UNDEFINED; 

      status = GetTypeNameFromJSONContent(buffer, name, ccs::types::MaxStringLength);

      if (status)
        {
          status = ccs::types::GlobalTypeDatabase::IsValid(name);
        }

      if (status)
        {
          log_debug("Is<ccs::types::ScalarType> - Name '%s' is registered .. return", name);
          return Is<ccs::types::ScalarType>(ccs::types::GlobalTypeDatabase::GetType(name));
        }
      else
        {
          log_debug("Is<ccs::types::ScalarType> - Name '%s' is not registered .. continue", name);
          status = true;
        }
    }

  // Reaching here, we may have or not a 'type' name but we are sure the type is not registered ..
  // .. test the rest of the type specification 

  const ccs::types::char8 * p_buf = buffer;

  if (status)
    {
      while ((*p_buf != 0) && (*p_buf != '{')) { p_buf++; }
      status = (*p_buf == '{');
    }

  if (status) // p_buf points to '{'
    { // Must have 'size'
      status = ((true == ccs::HelperTools::Contain(p_buf, "size")) &&
                (ccs::HelperTools::Find(p_buf, "size") < ccs::HelperTools::Find(p_buf, "}")));
    }

  return status;

}

static inline bool IsArrayType (const std::shared_ptr<const ccs::types::AnyType>& type) { return Is<ccs::types::ArrayType>(type); }
static inline bool IsCompoundType (const std::shared_ptr<const ccs::types::AnyType>& type) { return Is<ccs::types::CompoundType>(type); }
static inline bool IsScalarType (const std::shared_ptr<const ccs::types::AnyType>& type) { return Is<ccs::types::ScalarType>(type); }

static inline bool IsCompoundArray (const std::shared_ptr<const ccs::types::AnyType>& type) 
{ 

  bool status = Is<ccs::types::ArrayType>(type); 

  if (status)
    {
      status = Is<ccs::types::CompoundType>(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType());
    }

  return status; 

}

static inline bool IsScalarArray (const std::shared_ptr<const ccs::types::AnyType>& type)
{ 

  bool status = Is<ccs::types::ArrayType>(type); 

  if (status)
    {
      status = Is<ccs::types::ScalarType>(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType());
    }

  return status; 

}

static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const ccs::types::char8 * const base, const ccs::types::uint32 multiplicity) // Registered name
{

  ccs::types::ArrayType* ref = NULL_PTR_CAST(ccs::types::ArrayType*);

  bool status = ((NULL_PTR_CAST(const ccs::types::char8*) != base) &&
                 (true == ccs::types::GlobalTypeDatabase::IsValid(base)));

  if (status)
    {
      ref = NewArrayType(type, ccs::types::GlobalTypeDatabase::GetType(base), multiplicity);
    }

  return ref;

}

static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const std::shared_ptr<const ccs::types::AnyType>& base, const ccs::types::uint32 multiplicity)
{

  ccs::types::ArrayType* ref = NULL_PTR_CAST(ccs::types::ArrayType*);

  bool status = static_cast<bool>(base);

  if (status)
    {
      ref = new (std::nothrow) ccs::types::ArrayType (type, base, multiplicity);
    }

  return ref;

}

static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const ccs::types::AnyType * const base, const ccs::types::uint32 multiplicity) { return NewArrayType(type, std::shared_ptr<const ccs::types::AnyType>(base), multiplicity); }
static inline ccs::types::ArrayType* NewArrayType (const ccs::types::char8 * const type, const ccs::types::AnyType& base, const ccs::types::uint32 multiplicity) { return NewArrayType(type, &base, multiplicity); }

static bool HasAttribute (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name)
{ 

  log_trace("HasAttribute('%s') - Entering routine", name);

  bool status = static_cast<bool>(type);

  if (status && (false == ccs::HelperTools::IsUndefinedString(name)))
    {

      // The method should eventually work with nested attributes
      // It should reply true with attribute name 'a.b.c' or 'a[0].b' or even 'a[0]' if any such path is valid, 
      // e.g. HasAttribute('a') == true && Is<ArrayType>(GetAtributeType('a')) == true
      
      ccs::types::string buffer = STRING_UNDEFINED;
      
      ccs::types::char8* p_next = NULL_PTR_CAST(ccs::types::char8*);
      ccs::types::char8* p_curr = ccs::HelperTools::SafeStringCopy(buffer, name, ccs::types::MaxStringLength);
      ccs::types::char8* p_first = strtok_r(p_curr, ".[]", &p_next);
      
      if ((p_next == NULL_PTR_CAST(ccs::types::char8*)) || (*p_next == 0)) // Token without separator .. End of recursion
        {
          log_debug("HasAttribute('%s') - No separator found ..", name);
          
          if (true == ccs::HelperTools::IsIntegerString(p_first)) // An integer token .. Test element within array
            {
              status = (Is<ccs::types::ArrayType>(type) && std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->HasElement(ccs::HelperTools::ToInteger(p_first)));

              if (status)
                {
                  log_debug("HasAttribute('%s') - Success", name);
                }
              else
                {
                  log_debug("HasAttribute('%s') - Error", name);
                }
            }
          else // A non-integer token
            {
              status = (Is<ccs::types::CompoundType>(type) && std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->HasAttribute(p_first));

              if (status)
                {
                  log_debug("HasAttribute('%s') - Success", name);
                }
              else
                {
                  log_debug("HasAttribute('%s') - Error", name);
                }
            }
        }
      else // Token with a remainder .. Further recursion
        {
          log_debug("HasAttribute('%s') - Token '%s' found (rest '%s') ..", name, p_first, p_next);
          
          if (true == ccs::HelperTools::IsIntegerString(p_first)) // An integer token .. Test element within array
            {
              status = (Is<ccs::types::ArrayType>(type) && std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->HasElement(ccs::HelperTools::ToInteger(p_first)));
              
              if (status)
                {
                  log_debug("HasAttribute('%s') - Recursion on ArrayType ..", name);
                  
                  // Recursion
                  status = HasAttribute(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType(), p_next); 
                }
            }
          else // A non-integer token
            {
              status = (Is<ccs::types::CompoundType>(type) && std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->HasAttribute(p_first));
              
              if (status)
                {
                  log_debug("HasAttribute('%s') - Recursion on CompoundType ..", name);
                  
                  // Recursion
                  status = HasAttribute(std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->GetAttributeType(p_first), p_next); 
                }
            }
        }
    }      

  log_trace("HasAttribute('%s') - Leaving routine", name);

  return status; 

}

static ccs::types::uint32 GetAttributeOffset (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name)
{ 

  log_trace("GetAttributeOffset('%s') - Entering routine", name);

  ccs::types::uint32 offset = 0u;

  bool status = HasAttribute(type, name); // Tests both for type and name validity

  if (status && (false == ccs::HelperTools::IsUndefinedString(name)))
    {
      ccs::types::string buffer = STRING_UNDEFINED; 
  
      ccs::types::char8* p_next = NULL_PTR_CAST(ccs::types::char8*);
      ccs::types::char8* p_curr = ccs::HelperTools::SafeStringCopy(buffer, name, ccs::types::MaxStringLength);
      ccs::types::char8* p_first = strtok_r(p_curr, ".[]", &p_next);

      if ((p_next == NULL_PTR_CAST(ccs::types::char8*)) || (*p_next == 0)) // Token without separator .. End of recursion
        {
          log_debug("GetAttributeOffset('%s') - No separator found ..", name);
          
          if (true == ccs::HelperTools::IsIntegerString(p_first)) // An integer token .. Locate element within array
            {
              offset = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementOffset(ccs::HelperTools::ToInteger(p_first));
            }
          else // A non-integer token
            {
              offset = std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->GetAttributeOffset(p_first);
            }
        }
      else // Token with a remainder .. Further recursion
        {
          log_debug("GetAttributeOffset('%s') - Token '%s' found (rest '%s') ..", name, p_first, p_next);
          
          if (true == ccs::HelperTools::IsIntegerString(p_first)) // An integer token .. Locate element within array
            {
              log_debug("GetAttributeOffset('%s') - Recursion on ArrayType ..", name);
                  
              offset = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementOffset(ccs::HelperTools::ToInteger(p_first)) 
                + GetAttributeOffset(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType(), p_next); 
            }
          else // A non-integer token
            {
              log_debug("GetAttributeOffset('%s') - Recursion on CompoundType ..", name);
                  
              offset = std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->GetAttributeOffset(p_first) 
                + GetAttributeOffset(std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->GetAttributeType(p_first), p_next); 
            }
        }
    }

  log_trace("GetAttributeOffset('%s') - Leaving routine", name);

  return offset;

}

static inline ccs::types::uint32 GetAttributeSize (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name) 
{ 

  ccs::types::uint32 size = 0u;

  bool status = HasAttribute(type, name);

  if (status)
    {
      std::shared_ptr<const ccs::types::AnyType> attr_type = GetAttributeType(type, name);
      size = attr_type->GetSize(); 
    }

  return size;

}

static std::shared_ptr<const ccs::types::AnyType> GetAttributeType (const std::shared_ptr<const ccs::types::AnyType>& type, const ccs::types::char8 * const name) 
{ 

  log_trace("GetAttributeType('%s') - Entering routine", name);

  std::shared_ptr<const ccs::types::AnyType> ret;

  bool status = HasAttribute(type, name); // Tests both for type and name validity

  if (status && (true == ccs::HelperTools::IsUndefinedString(name))) // '' is a valid name
    {
      ret = type; // Return this type
    }

  if (status && (false == ccs::HelperTools::IsUndefinedString(name))) // Called with a valid non-empty name
    {
      ccs::types::string buffer = STRING_UNDEFINED; 
  
      ccs::types::char8* p_next = NULL_PTR_CAST(ccs::types::char8*);
      ccs::types::char8* p_curr = ccs::HelperTools::SafeStringCopy(buffer, name, ccs::types::MaxStringLength);
      ccs::types::char8* p_first = strtok_r(p_curr, ".[]", &p_next);

      if ((p_next == NULL_PTR_CAST(ccs::types::char8*)) || (*p_next == 0)) // Token without separator .. End of recursion
        {
          log_debug("GetAttributeType('%s') - No separator found ..", name);
          
          if (true == ccs::HelperTools::IsIntegerString(p_first)) // An integer token .. Return element type
            {
              ret = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType();
            }
          else // A non-integer token
            {
              ret = std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->GetAttributeType(p_first);
            }
        }
      else // Token with a remainder .. Further recursion
        {
          log_debug("GetAttributeType('%s') - Token '%s' found (rest '%s') ..", name, p_first, p_next);
          
          if (true == ccs::HelperTools::IsIntegerString(p_first)) // An integer token .. Proceed with element type
            {
              log_debug("GetAttributeType('%s') - Recursion on ArrayType ..", name);
                  
              ret = GetAttributeType(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType(), p_next); 
            }
          else // A non-integer token
            {
              log_debug("GetAttributeType('%s') - Recursion on CompoundType ..", name);
                  
              ret = GetAttributeType(std::dynamic_pointer_cast<const ccs::types::CompoundType>(type)->GetAttributeType(p_first), p_next); 
            }
        }
    }

  log_trace("GetAttributeType('%s') - Leaving routine", name);

  return ret;

}

static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::AnyType>& type, const ccs::types::char8 * const buffer)
{

  bool status = (NULL_PTR_CAST(const ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("Parse<AnyType> - Try and parse '%s' ..", buffer);
    }

  if (status && static_cast<bool>(type))
    {
      type.reset();
      status = !static_cast<bool>(type);
    }

  ccs::types::uint32 ret = 0u;

  if (status)
    {
      if (ccs::HelperTools::Is<ccs::types::ArrayType>(buffer))
        {
          log_debug("Parse<AnyType> - .. Is<ArrayType> ..");
          std::shared_ptr<ccs::types::ArrayType> tmp = std::dynamic_pointer_cast<ccs::types::ArrayType>(type);
          ret = Parse(tmp, buffer);

          if (ret > 0u)
            {
              log_debug("Parse<AnyType> - .. consumed '%u' characters", ret);
              type = std::dynamic_pointer_cast<ccs::types::AnyType>(tmp);
            }
        }
      else if (ccs::HelperTools::Is<ccs::types::CompoundType>(buffer))
        {
          log_debug("Parse<AnyType> - .. Is<CompoundType> ..");
          std::shared_ptr<ccs::types::CompoundType> tmp = std::dynamic_pointer_cast<ccs::types::CompoundType>(type);
          ret = Parse(tmp, buffer);

          if (ret > 0u)
            {
              log_debug("Parse<AnyType> - .. consumed '%u' characters", ret);
              type = std::dynamic_pointer_cast<ccs::types::AnyType>(tmp);
            }
        }
      else if (ccs::HelperTools::Is<ccs::types::ScalarType>(buffer))
        {
          log_debug("Parse<AnyType> - .. Is<ScalarType> ..");
          std::shared_ptr<ccs::types::ScalarType> tmp = std::dynamic_pointer_cast<ccs::types::ScalarType>(type);
          ret = Parse(tmp, buffer);

          if (ret > 0u)
            {
              log_debug("Parse<AnyType> - .. consumed '%u' characters", ret);
              type = std::dynamic_pointer_cast<ccs::types::AnyType>(tmp);
            }
        }
      else
        {
          log_error("Parse<AnyType> - Unable to parse '%s'", buffer);
        }

      status = (ret > 0u); // Unused
    }

  if (!status)
    {
      log_error("Parse<AnyType> - .. failure");
    }

  log_debug("Parse<AnyType> - Returning '%u'", ret);

  return ret;

}
  
static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::ArrayType>& type, const ccs::types::char8 * const buffer)
{

  bool status = (NULL_PTR_CAST(const ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("Parse<ArrayType> - Try and parse '%s'", buffer);
    }

  if (status && !static_cast<bool>(type))
    {
      type = std::shared_ptr<ccs::types::ArrayType>(new (std::nothrow) ccs::types::ArrayType ());
      status = static_cast<bool>(type);
    }

  ccs::types::uint32 ret = 0u;

  if (status)
    {
      // Assumptions and contraints
      //   - Function called with a 'valid' JSON format, i.e. true == Is<ArrayType>(buffer)
      //   implies that the type 'name' is registered as an ArrayType or the type specification
      //   includes element 'type' and multiplicity.

      ret = static_cast<ccs::types::uint32>(ccs::HelperTools::FindMatchingBrace(buffer));
      ret++;
    }

  std::string line;

  if (status)
    {
      // Copy type specification till matching brace
      line = std::string(buffer, ret);
      log_debug("Parse<ArrayType> - Try and parse '%s'", line.c_str());
    }

  if (status)
    {
      ccs::types::string name = STRING_UNDEFINED;
      status = GetTypeNameFromJSONContent(line.c_str(), name, ccs::types::MaxStringLength);

      if (status)
        {
          if (true == ccs::types::GlobalTypeDatabase::IsValid(name))
            {
              log_debug("Parse<ArrayType> - Registered type '%s' ..", name);
              type = std::const_pointer_cast<ccs::types::ArrayType>(std::dynamic_pointer_cast<const ccs::types::ArrayType>(ccs::types::GlobalTypeDatabase::GetType(name)));
            }
          else
            {
              log_debug("Parse<ArrayType> - Not registered type '%s' ..", name);

              (void)type->SetName(name);

              ccs::types::char8 elem_type [1024] = STRING_UNDEFINED;
              ccs::types::string mult = STRING_UNDEFINED;

              status = (GetAttributeFromJSONContent(line.c_str(), "element", elem_type, 1024u) &&
                        GetAttributeFromJSONContent(line.c_str(), "multiplicity", mult, ccs::types::MaxStringLength));

              if (status)
                {
                  std::shared_ptr<ccs::types::AnyType> elem;
          
                  log_debug("Parse<ArrayType> - Recursion with '%s'", elem_type);
                  
                  ccs::types::uint32 jmp = Parse(elem, elem_type);
                  status = (jmp > 0u);

                  if (status)
                    {
                      log_debug("Parse<ArrayType> - SetElementType('%s') ..", elem->GetName());
                      (void)type->SetElementType(elem);
                    }
                }

              if (status)
                {
                  status = ccs::HelperTools::IsIntegerString(mult);
              
                  if (status)
                    {
                      log_debug("Parse<ArrayType> - SetMultiplicity('%u') ..", static_cast<ccs::types::uint32>(ccs::HelperTools::ToInteger(mult)));
                      (void)type->SetMultiplicity(static_cast<ccs::types::uint32>(ccs::HelperTools::ToInteger(mult)));
                    }
                }
            }
        }
    }

  if (!status)
    {
      ret = 0u;
    }

  log_info("Parse<ArrayType> - Returning '%u'", ret);

  return ret;

}
  
static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::CompoundType>& type, const ccs::types::char8 * const buffer)
{

  bool status = (NULL_PTR_CAST(const ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("Parse<CompoundType> - Try and parse '%s'", buffer);
    }

  if (status && !static_cast<bool>(type))
    {
      type = std::shared_ptr<ccs::types::CompoundType>(new (std::nothrow) ccs::types::CompoundType ());
      status = static_cast<bool>(type);
    }

  ccs::types::uint32 ret = 0u;

  if (status)
    {
      // Assumptions and contraints
      //   - Function called with a 'valid' JSON format, i.e. true == Is<CompoundType>(buffer)
      //   implies that the type 'name' is registered as a CompoundType or the type specification
      //   includes attributes list.

      ret = static_cast<ccs::types::uint32>(ccs::HelperTools::FindMatchingBrace(buffer));
      ret++;
    }

  std::string line;

  if (status)
    {
      // Copy type specification till matching brace
      line = std::string(buffer, ret);
      log_debug("Parse<CompoundType> - Try and parse '%s'", line.c_str());
    }

  if (status)
    {
      ccs::types::string name = STRING_UNDEFINED;
      status = GetTypeNameFromJSONContent(line.c_str(), name, ccs::types::MaxStringLength);

      if (status)
        {
          if (true == ccs::types::GlobalTypeDatabase::IsValid(name))
            {
              log_debug("Parse<CompoundType> - Registered type '%s' ..", name);
              type = std::const_pointer_cast<ccs::types::CompoundType>(std::dynamic_pointer_cast<const ccs::types::CompoundType>(ccs::types::GlobalTypeDatabase::GetType(name)));
            }
          else
            {
              log_debug("Parse<CompoundType> - Not registered type '%s' ..", name);

              (void)type->SetName(name);

              // Copy type specification till matching brace
              line = line.substr(line.find("["), std::string::npos);

              // Should point to '[{"attr":{<type>}},..]' or '[{"attr":{<type>}}]'

              log_debug("Parse<CompoundType> - Iterating through '%s'", line.c_str());

              while (status)
                {
                  std::size_t marker = line.find("\"");
                  status = (std::string::npos != marker);
                      
                  if (status) 
                    {
                      // Re-align line to after marker
                      line = line.substr(marker+1u, std::string::npos);
                    }
                  
                  std::string keyword;
                  
                  if (status) 
                    {
                      std::size_t marker = line.find("\"");
                      status = (std::string::npos != marker);
                      
                      if (status) 
                        {
                          // Extract keyword
                          keyword = line.substr(0u, marker);
                          log_debug("Parse<CompoundType> - Keyword '%s'", keyword.c_str());
                        }
                      
                      if (status) 
                        {
                          // Re-align line to after marker
                          line = line.substr(marker+1u, std::string::npos);
                        }
                    }
                  
                  if (status) 
                    {
                      std::size_t marker = line.find(":");
                      status = (std::string::npos != marker);
                      
                      if (status) 
                        {
                          // Re-align line to after marker
                          line = line.substr(marker+1u, std::string::npos);
                        }
                    }

                  if (status) 
                    {
                      log_debug("Parse<CompoundType> - Recursion with '%s'", line.c_str());
                      std::shared_ptr<ccs::types::AnyType> attr;
                      ccs::types::uint32 jmp = Parse(attr, line.c_str());

                      status = (jmp > 0u);

                      if (status)
                        {
                          log_debug("Parse<CompoundType> - AddAttribute('%s') ..", keyword.c_str());
                          (void)type->AddAttribute(keyword.c_str(), attr);
                          // Re-align line to after parsed section
                          line = line.substr(jmp, std::string::npos);
                        }
                    }

                  // Continue on ',' and break on ']'
                  while (status && (line[0] != ',') && (line[0] != ']'))
                    {
                      line = line.substr(1u, std::string::npos);
                    }

                  if (status)
                    {
                      status = (line[0] == ',');

                      if (!status)
                        {
                          // WARNING - Skip to make sure the outside recursion does not end with the 'inner' closing bracket
                          line = line.substr(1u, std::string::npos);
                          status = true;
                          break;
                        }
                    }
                }
            }
        }
    }

  if (!status)
    {
      ret = 0u;
    }

  log_debug("Parse<CompoundType> - Returning '%u'", ret);

  return ret;

}

static ccs::types::uint32 Parse (std::shared_ptr<ccs::types::ScalarType>& type, const ccs::types::char8 * const buffer)
{

  bool status = (NULL_PTR_CAST(const ccs::types::char8*) != buffer);

  if (status)
    {
      log_debug("Parse<ScalarType> - Try and parse '%s'", buffer);
    }

  ccs::types::uint32 ret = 0u;

  if (status)
    {
      // Assumptions and contraints
      //   - Function called with a 'valid' JSON format, i.e. true == Is<ScalarType>(buffer)
      //   implies that the type 'name' is registered as an ArrayType or the type specification
      //   includes element 'type' and multiplicity.

      ret = static_cast<ccs::types::uint32>(ccs::HelperTools::FindMatchingBrace(buffer));
      ret++;
    }

  std::string line;

  if (status)
    {
      // Copy type specification till matching brace
      line = std::string(buffer, ret);
      log_debug("Parse<ScalarType> - Try and parse '%s'", line.c_str());
    }

  if (status)
    {
      ccs::types::string name = STRING_UNDEFINED;
      status = GetTypeNameFromJSONContent(line.c_str(), name, ccs::types::MaxStringLength);

      if (status)
        {
          if (true == ccs::types::GlobalTypeDatabase::IsValid(name))
            {
              log_debug("Parse<ScalarType> - Registered type '%s' ..", name);
              type = std::const_pointer_cast<ccs::types::ScalarType>(std::dynamic_pointer_cast<const ccs::types::ScalarType>(ccs::types::GlobalTypeDatabase::GetType(name)));
            }
          else
            {
              status = false;
            }
        }
    }

  if (!status)
    {
      ret = 0u;
    }

  log_debug("Parse<ScalarType> - Returning '%u'", ret);

  return ret;

}
  
static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::AnyType>& type, void * const ref, const ccs::types::char8 * const buffer)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  ccs::types::uint32 ret = 0u;

  if (status)
    {
      if (Is<ccs::types::ArrayType>(type))
        {
          ret = ParseInstance(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type), ref, buffer);
        }
      else if (Is<ccs::types::CompoundType>(type))
        {
          ret = ParseInstance(std::dynamic_pointer_cast<const ccs::types::CompoundType>(type), ref, buffer);
        }
      else if (Is<ccs::types::ScalarType>(type))
        {
          ret = ParseInstance(std::dynamic_pointer_cast<const ccs::types::ScalarType>(type), ref, buffer);
        }
    }

  return ret;

}

static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::ArrayType>& type, void * const ref, const ccs::types::char8 * const buffer)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  ccs::types::uint32 ret = 0u;

  const ccs::types::char8 * p_buf = buffer;

  if (status)
    {
      log_debug("ParseInstance('%s') - Try and parse '%s'", type->GetName(), p_buf);
    }

  if (status && (ccs::types::Character8 == type->GetElementType()))
    {
      // Treat as string and return
      const ccs::types::char8 * p_buf = buffer;

      while ((*p_buf != 0) && (*p_buf != '"')) { p_buf++; ret++; }
      status = (*p_buf != 0);

      ccs::types::char8* copy = static_cast<ccs::types::char8*>(malloc(type->GetSize()));

      if (status) // p_buf points to '"'
        {
          p_buf++; ret++; 
          ccs::HelperTools::SafeStringCopy(copy, p_buf, type->GetSize());
        }

      if (status)
        {
          ccs::types::char8* p_buf = copy;
          while ((*p_buf != 0) && (*p_buf != '"')) { p_buf++; ret++; } // Search closing quote
          status = (*p_buf != 0);
          
          if (status)
            {
              *p_buf = 0; ret++; // Forcefully terminate string
            }
        }

      if (status)
        {
          log_debug("ParseInstance('%s') - Copying string '%s'", type->GetName(), copy);
          (void)ccs::HelperTools::SafeStringCopy(static_cast<ccs::types::char8*>(ref), copy, type->GetSize());
        }

      free(copy);

      return ret;
    }

  if (status)
    {
      while ((*p_buf != 0) && (*p_buf != '[')) { p_buf++; ret++; }
      status = (*p_buf == '[');
    }

  if (status) // p_buf points to '[' 
    {
      p_buf++; ret++; 
    }
  
  for (ccs::types::uint32 index = 0u; ((index < type->GetElementNumber()) && status); index++)
    {
      void* attr = static_cast<void*>(static_cast<ccs::types::uint8*>(ref) + type->GetElementOffset(index));

      //if (status) // Search for appropriate entry
        {
          while ((*p_buf != 0) && (*p_buf == ' ')) { p_buf++; ret++; }
          status = (*p_buf != 0);
        }

      if (!status)
        {
          log_error("ParseInstance('%s') - Failed to find element '%u'", type->GetName(), index);
        }

      if (status) // p_buf points to 'attr_value,' 
        {
          ccs::types::uint32 jmp = ParseInstance(type->GetElementType(), attr, p_buf);
          status = (jmp > 0u);

          if (status)
            {
              p_buf += jmp; ret += jmp;
            }
        }

      if (status) // p_buf points to ',' 
        {
          while ((*p_buf != 0) && (*p_buf != ',') && (*p_buf != ']')) { p_buf++; ret++; } // End of array may not have trailing ','
          status = (*p_buf != 0);
        }

      if (status) // p_buf points to ',' or ']' 
        {
          p_buf++; ret++; 
        }
    }

  if (!status)
    {
      ret = 0u;
    }

  log_debug("ParseInstance('%s') - Returning '%u'", type->GetName(), ret);

  return ret;

}

static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::CompoundType>& type, void * const ref, const ccs::types::char8 * const buffer)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  ccs::types::uint32 ret = 0u;

  const ccs::types::char8 * p_buf = buffer;

  if (status)
    {
      log_debug("ParseInstance('%s') - Try and parse '%s'", type->GetName(), p_buf);
    }

  for (ccs::types::uint32 index = 0u; ((index < type->GetAttributeNumber()) && status); index++)
    {
      void* attr = static_cast<void*>(static_cast<ccs::types::uint8*>(ref) + type->GetAttributeOffset(index));
      const ccs::types::char8 * attr_name = type->GetAttributeName(index);

      //if (status) // Search for appropriate entry
        {
          while ((*p_buf != 0) && (strncmp(p_buf, attr_name, strlen(attr_name)) != 0)) { p_buf++; ret++; }
          status = (*p_buf != 0);
        }

      if (!status)
        {
          log_error("ParseInstance('%s') - Failed to find attribute '%s' at index '%u'", type->GetName(), attr_name, index);
        }

      if (status) // p_buf points to 'attr_name":attr_value,'
        {
          while ((*p_buf != 0) && (*p_buf != ':')) { p_buf++; ret++; }
          status = (*p_buf == ':');
        }

      if (status) // p_buf points to ':attr_value,'
        {
          p_buf++; ret++;
        }

      if (status) // p_buf points to 'attr_value,'
        {
          while ((*p_buf != 0) && (*p_buf == ' ')) { p_buf++; ret++; } // Skip any additional ' ' character
          status = (*p_buf != 0);
        }

      if (status) // p_buf points to 'attr_value,'
        {
          ccs::types::uint32 jmp = ParseInstance(type->GetAttributeType(index), attr, p_buf);
          status = (jmp > 0u);

          if (status)
            {
              p_buf += jmp; ret += jmp;
            }
        }

      if (status) // p_buf points to ','
        {
          while ((*p_buf != 0) && (*p_buf != ',') && (*p_buf != '}')) { p_buf++; ret++; } // End of struct may not have trailing ','
          status = (*p_buf != 0);
        }

      if (status) // p_buf points to ','
        {
          p_buf++; ret++;
        }
    }

  if (!status)
    {
      ret = 0u;
    }

  log_debug("ParseInstance('%s') - Returning '%u'", type->GetName(), ret);

  return ret;

}

static ccs::types::uint32 ParseInstance (const std::shared_ptr<const ccs::types::ScalarType>& type, void * const ref, const ccs::types::char8 * const buffer)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  ccs::types::uint32 ret = 0u;

  if (status)
    {
      log_debug("ParseInstance('%s') - Try and parse '%s'", type->GetName(), buffer);
    }

  if (status)
    {
      if (ccs::types::Boolean == type)
        {
          if (strncmp(buffer, "false", 5) == 0)
            {
              *(static_cast<ccs::types::boolean*>(ref)) = false;
              ret = 5u;
            }
          else if (strncmp(buffer, "0", 1) == 0)
            {
              *(static_cast<ccs::types::boolean*>(ref)) = false;
              ret = 1u;
            }
          else if (strncmp(buffer, "true", 4) == 0)
            {
              *(static_cast<ccs::types::boolean*>(ref)) = true;
              ret = 4u;
            }
          else if (strncmp(buffer, "1", 1) == 0)
            {
              *(static_cast<ccs::types::boolean*>(ref)) = true;
              ret = 1u;
            }
          else
            {
              status = false;
            }
        }
      else if (ccs::types::Character8 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%hhx%n", static_cast<ccs::types::uint8*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "\"%c\"%n", static_cast<ccs::types::char8*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::SignedInteger8 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%hhx%n", static_cast<ccs::types::uint8*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%hhd%n", static_cast<ccs::types::int8*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::UnsignedInteger8 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%hhx%n", static_cast<ccs::types::uint8*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%hhu%n", static_cast<ccs::types::uint8*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::SignedInteger16 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%hx%n", static_cast<ccs::types::uint16*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%hd%n", static_cast<ccs::types::int16*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::UnsignedInteger16 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%hx%n", static_cast<ccs::types::uint16*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%hu%n", static_cast<ccs::types::uint16*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::SignedInteger32 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%x%n", static_cast<ccs::types::uint32*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%d%n", static_cast<ccs::types::int32*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::UnsignedInteger32 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%x%n", static_cast<ccs::types::uint32*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%u%n", static_cast<ccs::types::uint32*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::SignedInteger64 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%lx%n", static_cast<ccs::types::uint64*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%ld%n", static_cast<ccs::types::int64*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::UnsignedInteger64 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%lx%n", static_cast<ccs::types::uint64*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%lu%n", static_cast<ccs::types::uint64*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::Float32 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%x%n", static_cast<ccs::types::uint32*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%g%n", static_cast<ccs::types::float32*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::Float64 == type)
        {
          int num = 0; // '%n' requires int*

          if (strncmp(buffer, "0x", 2) == 0)
            {
              status = (sscanf(buffer, "0x%lx%n", static_cast<ccs::types::uint64*>(ref), &num) > 0);
            }
          else
            {
              status = (sscanf(buffer, "%lg%n", static_cast<ccs::types::float64*>(ref), &num) > 0);
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
      else if (ccs::types::String == type)
        {
          ccs::types::string copy;

          if (status)
            {
              const ccs::types::char8 * p_buf = buffer;

              while ((*p_buf != 0) /* && (*p_buf != '\'') */ && (*p_buf != '"') /* && (*p_buf != '>') */ && !ccs::HelperTools::IsAlphaNumeric(*p_buf)) { p_buf++; ret++; } // Search opening quote
              status = (*p_buf != 0);

              if (status)
                {
                  if (!ccs::HelperTools::IsAlphaNumeric(*p_buf)) { p_buf++; ret++; }
                  (void)ccs::HelperTools::SafeStringCopy(copy, p_buf, type->GetSize());
                }
            }

          if (status)
            {
              ccs::types::char8* p_buf = copy;
              while ((*p_buf != 0) /* && (*p_buf != '\'') */ && (*p_buf != '"') /* && (*p_buf != '<') */ && (*p_buf != ',')) { p_buf++; ret++; } // Search closing quote
              status = (*p_buf != 0);

              if (status)
                {
                  *p_buf = 0; // Forcefully terminate string
                }
            }
          
          if (status)
            {
              log_debug("ParseInstance('%s') - Copying string '%s'", type->GetName(), copy);
              (void)ccs::HelperTools::SafeStringCopy(static_cast<ccs::types::char8*>(ref), copy, type->GetSize());
            }
        }
      else
        {
          int num = 0; // '%n' requires int*
          ccs::types::uint32 size = type->GetSize();

          if (size == 1u)
            {
              status = (sscanf(buffer, "0x%hhx%n", static_cast<ccs::types::uint8*>(ref), &num) > 0);
            }
          else if (size == 2u)
            {
              status = (sscanf(buffer, "0x%hx%n", static_cast<ccs::types::uint16*>(ref), &num) > 0);
            }
          else if (size == 4u)
            {
              status = (sscanf(buffer, "0x%x%n", static_cast<ccs::types::uint32*>(ref), &num) > 0);
            }
          else if (size == 8u)
            {
              status = (sscanf(buffer, "0x%lx%n", static_cast<ccs::types::uint64*>(ref), &num) > 0);
            }
          else
            {
              status = false;
            }

          if (status)
            {
              ret = static_cast<ccs::types::uint32>(num);
            }
        }
    }

  if (!status)
    {
      ret = 0u;
    }

  log_debug("ParseInstance('%s') - Returning '%u'", type->GetName(), ret);

  return ret;

}

static bool Serialise (const std::shared_ptr<const ccs::types::AnyType>& type, ccs::types::char8 * const buffer, const ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  if (status)
    {
      if (Is<ccs::types::ArrayType>(type))
        {
          status = Serialise(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type), buffer, size);
        }
      else if (Is<ccs::types::CompoundType>(type))
        {
          status = Serialise(std::dynamic_pointer_cast<const ccs::types::CompoundType>(type), buffer, size);
        }
      else if (Is<ccs::types::ScalarType>(type))
        {
          status = Serialise(std::dynamic_pointer_cast<const ccs::types::ScalarType>(type), buffer, size);
        }
      else
        {
          status = false;
        }
    }

  return status;

}

static bool Serialise (const std::shared_ptr<const ccs::types::ArrayType>& type, ccs::types::char8 * const buffer, ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  ccs::types::char8* p_buf = buffer;

  if (status)
    {
      (void)snprintf(p_buf, size, "{\"type\":\"%s\",\"multiplicity\":%u,\"size\":%u,\"element\":", type->GetName(), type->GetElementNumber(), type->GetSize()); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 
    }

  if (status)
    {
      status = Serialise(type->GetElementType(), p_buf, size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 
    }

  if (status)
    {
      (void)ccs::HelperTools::SafeStringCopy(p_buf, "}", size); 
    }

  return status;

}

static bool Serialise (const std::shared_ptr<const ccs::types::CompoundType>& type, ccs::types::char8 * const buffer, ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  ccs::types::char8* p_buf = buffer;

  if (status)
    {
      snprintf(p_buf, size, "{\"type\":\"%s\",\"size\":%u,\"attributes\":[", type->GetName(), type->GetSize()); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 
    }

  for (ccs::types::uint32 index = 0u; ((index < type->GetAttributeNumber()) && status); index++)
    {
      const ccs::types::char8 * attr_name = type->GetAttributeName(index);

      (void)snprintf(p_buf, size, "{\"%s\":", attr_name); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer

      status = Serialise(type->GetAttributeType(index), p_buf, size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer

      (void)ccs::HelperTools::SafeStringCopy(p_buf, "},", size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 
    }

  // Terminate buffer - remove final comma character 
  if (static_cast<ccs::types::uint32>(strlen(buffer)) >= 1u) 
    {
      *(buffer + strlen(buffer) - 1u) = 0; p_buf = buffer + strlen(buffer); // Re-align pointer 
    }

  (void)ccs::HelperTools::SafeStringCopy(p_buf, "]}", size); 

  return status;

}

static bool Serialise (const std::shared_ptr<const ccs::types::ScalarType>& type, ccs::types::char8 * const buffer, ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  if (status)
    {
      (void)snprintf(buffer, size, "{\"type\":\"%s\",\"size\":%u}", type->GetName(), type->GetSize()); 
    }

  return status;

}

static bool SerialiseInstance (const std::shared_ptr<const ccs::types::AnyType>& type, const void * const ref, ccs::types::char8 * const buffer, const ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  if (status)
    {
      if (Is<ccs::types::ArrayType>(type))
        {
          status = SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type), ref, buffer, size);
        }
      else if (Is<ccs::types::CompoundType>(type))
        {
          status = SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::CompoundType>(type), ref, buffer, size);
        }
      else if (Is<ccs::types::ScalarType>(type))
        {
          status = SerialiseInstance(std::dynamic_pointer_cast<const ccs::types::ScalarType>(type), ref, buffer, size);
        }
      else
        {
          status = false;
        }
    }

  return status;

}

static bool SerialiseInstance (const std::shared_ptr<const ccs::types::ArrayType>& type, const void * const ref, ccs::types::char8 * const buffer, ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  if (status && (ccs::types::Character8 == type->GetElementType()))
    {
      // Treat as string and return
      ccs::HelperTools::SafeStringCopy(buffer, "\"", size);
      ccs::HelperTools::SafeStringAppend(buffer, static_cast<const ccs::types::char8*>(ref), size);
      ccs::HelperTools::SafeStringAppend(buffer, "\"", size);
      return status;
    }

  ccs::types::char8* p_buf = buffer;

  (void)ccs::HelperTools::SafeStringCopy(p_buf, "[", size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 

  for (ccs::types::uint32 index = 0u; ((index < type->GetElementNumber()) && status); index++)
    {
      const void* attr = static_cast<const void*>(static_cast<const ccs::types::uint8*>(ref) + type->GetElementOffset(index));

      status = SerialiseInstance(type->GetElementType(), attr, p_buf, size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 

      (void)ccs::HelperTools::SafeStringCopy(p_buf, ",", size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 
    }

  // Terminate buffer - remove final comma character 
  if (static_cast<ccs::types::uint32>(strlen(buffer)) >= 1u) 
    {
      *(buffer + strlen(buffer) - 1u) = 0; p_buf = buffer + strlen(buffer); // Re-align pointer 
    }

  (void)ccs::HelperTools::SafeStringCopy(p_buf, "]", size); 

  return status;

}

static bool SerialiseInstance (const std::shared_ptr<const ccs::types::CompoundType>& type, const void * const ref, ccs::types::char8 * const buffer, ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  ccs::types::char8* p_buf = buffer;

  (void)ccs::HelperTools::SafeStringCopy(p_buf, "{", size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 

  for (ccs::types::uint32 index = 0u; ((index < type->GetAttributeNumber()) && status); index++)
    {
      const void* attr = static_cast<const void*>(static_cast<const ccs::types::uint8*>(ref) + type->GetAttributeOffset(index));
      const ccs::types::char8* attr_name = type->GetAttributeName(index);

      snprintf(p_buf, size, "\"%s\":", attr_name); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer

      status = SerialiseInstance(type->GetAttributeType(index), attr, p_buf, size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer

      (void)ccs::HelperTools::SafeStringCopy(p_buf, ",", size); size -= strlen(p_buf); p_buf += strlen(p_buf); // Re-align pointer 
    }

  // Terminate buffer - remove final comma character 
  if (static_cast<ccs::types::uint32>(strlen(buffer)) >= 1u) 
    {
      *(buffer + strlen(buffer) - 1u) = 0; p_buf = buffer + strlen(buffer); // Re-align pointer 
    }

  (void)ccs::HelperTools::SafeStringCopy(p_buf, "}", size); 

  return status;

}

static bool SerialiseInstance (const std::shared_ptr<const ccs::types::ScalarType>& type, const void * const ref, ccs::types::char8 * const buffer, ccs::types::uint32 size)
{

  bool status = (static_cast<bool>(type) &&
                 (NULL_PTR_CAST(const void*) != ref) && 
                 (NULL_PTR_CAST(const ccs::types::char8*) != buffer));

  if (status)
    {
      if (ccs::types::Boolean == type)
        {
          if (true == *(static_cast<const ccs::types::boolean*>(ref)))
            {
              (void)ccs::HelperTools::SafeStringCopy(buffer, "true", size);
            }
          else
            {
              (void)ccs::HelperTools::SafeStringCopy(buffer, "false", size);
            }
        }
      else if (ccs::types::Character8 == type)
        {
          status = (snprintf(buffer, size, "\"%c\"", *(static_cast<const ccs::types::char8*>(ref))) > 0);
        }
      else if (ccs::types::SignedInteger8 == type)
        {
          status = (snprintf(buffer, size, "%hhd", *(static_cast<const ccs::types::int8*>(ref))) > 0);
        }
      else if (ccs::types::UnsignedInteger8 == type)
        {
          status = (snprintf(buffer, size, "%hhu", *(static_cast<const ccs::types::uint8*>(ref))) > 0);
        }
      else if (ccs::types::SignedInteger16 == type)
        {
          status = (snprintf(buffer, size, "%hd", *(static_cast<const ccs::types::int16*>(ref))) > 0);
        }
      else if (ccs::types::UnsignedInteger16 == type)
        {
          status = (snprintf(buffer, size, "%hu", *(static_cast<const ccs::types::uint16*>(ref))) > 0);
        }
      else if (ccs::types::SignedInteger32 == type)
        {
          status = (snprintf(buffer, size, "%d", *(static_cast<const ccs::types::int32*>(ref))) > 0);
        }
      else if (ccs::types::UnsignedInteger32 == type)
        {
          status = (snprintf(buffer, size, "%u", *(static_cast<const ccs::types::uint32*>(ref))) > 0);
        }
      else if (ccs::types::SignedInteger64 == type)
        {
          status = (snprintf(buffer, size, "%ld", *(static_cast<const ccs::types::int64*>(ref))) > 0);
        }
      else if (ccs::types::UnsignedInteger64 == type)
        {
          status = (snprintf(buffer, size, "%lu", *(static_cast<const ccs::types::uint64*>(ref))) > 0);
        }
      else if (ccs::types::Float32 == type)
        {
          status = (snprintf(buffer, size, "%g", *(static_cast<const ccs::types::float32*>(ref))) > 0);
        }
      else if (ccs::types::Float64 == type)
        {
          status = (snprintf(buffer, size, "%lg", *(static_cast<const ccs::types::float64*>(ref))) > 0);
        }
      else if (ccs::types::String == type)
        {
          status = (snprintf(buffer, size, "\"%s\"", static_cast<const ccs::types::char8*>(ref)) > 0);
        }
      else
        {
          ccs::types::uint32 size = type->GetSize();

          if (size == 1u)
            {
              status = (snprintf(buffer, size, "0x%2.2hhx", *(static_cast<const ccs::types::uint8*>(ref))) > 0);
            }
          else if (size == 2u)
            {
              status = (snprintf(buffer, size, "0x%4.4hx", *(static_cast<const ccs::types::uint16*>(ref))) > 0);
            }
          else if (size == 4u)
            {
              status = (snprintf(buffer, size, "0x%8.8x", *(static_cast<const ccs::types::uint32*>(ref))) > 0);
            }
          else if (size == 8u)
            {
              status = (snprintf(buffer, size, "0x%16.16lx", *(static_cast<const ccs::types::uint64*>(ref))) > 0);
            }
          else
            {
              status = false;
            }
        }
    }

  return status;

}

static std::shared_ptr<const ccs::types::CompoundType> Flatten (const std::shared_ptr<const ccs::types::AnyType>& type)
{

  log_trace("Flatten('%s') - Entering method", type->GetName());

  bool status = static_cast<bool>(type);

  std::shared_ptr<const ccs::types::CompoundType> flattened; // ToDo - Name derived from this->GetType()

  if (status)
    {
      if (Is<ccs::types::ArrayType>(type))
        {
          flattened = Flatten(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type));
        } 
      else if (Is<ccs::types::CompoundType>(type))
        {
          flattened = Flatten(std::dynamic_pointer_cast<const ccs::types::CompoundType>(type));
        }
      else
        {
          log_warning("Flatten('%s') - Is<ScalarType> .. unsupported");
        }
    }

  log_trace("Flatten('%s') - Leaving method", type->GetName());

  return flattened;

}

static std::shared_ptr<const ccs::types::CompoundType> Flatten (const std::shared_ptr<const ccs::types::ArrayType>& type)
{

  log_trace("Flatten('%s') - Entering method", type->GetName());

  bool status = static_cast<bool>(type);

  std::shared_ptr<ccs::types::CompoundType> flattened;

  if (status)
    {
      flattened = std::shared_ptr<ccs::types::CompoundType>(new (std::nothrow) ccs::types::CompoundType); // ToDo - Name derived from this->GetType()
      status = static_cast<bool>(flattened);
    }

  std::shared_ptr<const ccs::types::AnyType> elem_type;

  if (status)
    {
      elem_type = type->GetElementType();
      status = static_cast<bool>(elem_type);
    }

  if (status)
    {
      if (Is<ccs::types::ScalarType>(elem_type))
        {
          for (ccs::types::uint32 index = 0u; (status && (index < type->GetMultiplicity())); index++)
            {
              ccs::types::string attr_name = STRING_UNDEFINED;
              snprintf(attr_name, ccs::types::MaxStringLength, "%u", index);
              
              flattened->AddAttribute(attr_name, elem_type);
            }
        }
      else
        {
          const std::shared_ptr<const ccs::types::CompoundType> nested = Flatten(elem_type);
          status = static_cast<bool>(nested); // Then it is a flattened type .. no more recursion
          
          for (ccs::types::uint32 outer = 0u; (status && (outer < type->GetMultiplicity())); outer++)
            {
              for (ccs::types::uint32 inner = 0u; (status && (inner < nested->GetAttributeNumber())); inner++)
                {
                  ccs::types::string attr_name = STRING_UNDEFINED;
                  snprintf(attr_name, ccs::types::MaxStringLength, "%u/%s", outer, nested->GetAttributeName(inner));
                  
                  flattened->AddAttribute(attr_name, nested->GetAttributeType(inner));
                }
            }
        }
    }

  log_debug("Flatten('%s') - Flattened '%u' attributes", type->GetName(), flattened->GetAttributeNumber());

  log_trace("Flatten('%s') - Leaving method", type->GetName());

  return std::const_pointer_cast<const ccs::types::CompoundType>(flattened);

}

static std::shared_ptr<const ccs::types::CompoundType> Flatten (const std::shared_ptr<const ccs::types::CompoundType>& type)
{

  log_trace("Flatten('%s') - Entering method", type->GetName());

  bool status = static_cast<bool>(type);

  std::shared_ptr<ccs::types::CompoundType> flattened;

  if (status)
    {
      flattened = std::shared_ptr<ccs::types::CompoundType>(new (std::nothrow) ccs::types::CompoundType); // ToDo - Name derived from this->GetType()
      status = static_cast<bool>(flattened);
    }

  for (ccs::types::uint32 outer = 0u; (status && (outer < type->GetAttributeNumber())); outer++)
    {
      std::shared_ptr<const ccs::types::AnyType> attr_type = type->GetAttributeType(outer);
      status = static_cast<bool>(attr_type);

      if (status)
        {
          if (Is<ccs::types::ScalarType>(attr_type))
            {
              flattened->AddAttribute(type->GetAttributeName(outer), attr_type);
            }
          else
            {
              std::shared_ptr<const ccs::types::CompoundType> nested = Flatten(type->GetAttributeType(outer));
              status = static_cast<bool>(nested); // Then it is a flattened type .. no more recursion
              
              for (ccs::types::uint32 inner = 0u; (status && (inner < nested->GetAttributeNumber())); inner++)
                {
                  ccs::types::string attr_name = STRING_UNDEFINED;
                  snprintf(attr_name, ccs::types::MaxStringLength, "%s/%s", type->GetAttributeName(outer), nested->GetAttributeName(inner));
                  
                  flattened->AddAttribute(attr_name, nested->GetAttributeType(inner));
                }
            }
        }
    }

  log_debug("Flatten('%s') - Flattened '%u' attributes", type->GetName(), flattened->GetAttributeNumber());
  
  log_trace("Flatten('%s') - Leaving method", type->GetName());

  return std::const_pointer_cast<const ccs::types::CompoundType>(flattened);

}

static inline void DumpGTDBToLog (void)
{

  for (::ccs::types::uint32 index = 0ul; index < ::ccs::types::GlobalTypeDatabase::GetInstance()->GetSize(); index++)
    {
      log_info("[%u] - '%s' ..", index, ::ccs::types::GlobalTypeDatabase::GetInstance()->GetName(index));
      ::ccs::types::char8 buffer [1024];
      (void)::ccs::HelperTools::Serialise(::ccs::types::GlobalTypeDatabase::GetInstance()->GetElement(index), buffer, 1024u);
      log_info(".. '%s'", buffer);
    }

  return;

}

} // namespace HelperTools

} // namespace ccs

#endif // _AnyTypeHelper_h_ 


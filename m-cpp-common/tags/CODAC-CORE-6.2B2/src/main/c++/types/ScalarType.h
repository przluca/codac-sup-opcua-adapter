/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/ScalarType.h $
* $Id: ScalarType.h 100890 2019-07-19 07:14:30Z bauvirb $
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
 * @file ScalarType.h
 * @brief Header file for introspectable ScalarType class.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the ScalarType class.
 */

#ifndef _ScalarType_h_
#define _ScalarType_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. helper functions

#include "NetTools.h" 

#include "AnyType.h" // Base class definition

// Constants

// Type definition

namespace ccs {

namespace types {

/**
 * @brief Introspectable type definition for scalar types.
 */

class ScalarType : public AnyType
{

  private:

    /**
     * @brief Copy assignment operator.
     * @note Undefined since the class is used to wrap built-in types and should not be copied.
     */

    ScalarType& operator= (const ScalarType& type); // Undefined

  protected:

  public:

    /**
     * @brief Constructor.
     * @post
     *   __size = 0u && __type = "undefined"
     */

    ScalarType (void);

    /**
     * @brief Copy constructor.
     */

    ScalarType (const ScalarType& type);

    /**
     * @brief Constructor.
     * @param type Name of type definition.
     * @param size Byte size of type definition.
     * @post
     *   __size = size && __type = type
     */

    ScalarType (const char8 * const type, const uint32 size);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~ScalarType (void) {}; 

    /**
     * @brief Virtual method. See AnyType::ToNetworkByteOrder.
     */

    virtual bool ToNetworkByteOrder (void * const ref) const; // In-place conversion

    /**
     * @brief Virtual method. See AnyType::FromNetworkByteOrder.
     */

    virtual bool FromNetworkByteOrder (void * const ref) const; // In-place conversion

    /**
     * @brief Comparison operator.
     * @detail Compares against other type definition for equivalence.
     * @return True if successful.
     */

    virtual bool operator== (const AnyType& type) const; // Specialises virtual method
    bool operator== (const ScalarType& type) const;

};

/**
 * @brief Introspectable type definition for scalar types.
 */

template <typename Type> class ScalarTypeT : public ScalarType
{

  private:

    /**
     * @brief Copy constructor.
     * @note Undefined since the class is used to wrap built-in types and should not be copied.
     */

    ScalarTypeT (const ScalarTypeT& type); // Undefined

    /**
     * @brief Copy assignment operator.
     * @note Undefined since the class is used to wrap built-in types and should not be copied.
     */

    ScalarTypeT& operator= (const ScalarTypeT& type); // Undefined

  protected:

  public:

    /**
     * @brief Constructor.
     */

    ScalarTypeT (void);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~ScalarTypeT (void) {}; 

    /**
     * @brief Virtual method. See AnyType::ToNetworkByteOrder.
     */

    virtual bool ToNetworkByteOrder (void * const ref) const;

    /**
     * @brief Virtual method. See AnyType::FromNetworkByteOrder.
     */

    virtual bool FromNetworkByteOrder (void * const ref) const;

};

// Global variables

extern const std::shared_ptr<const ScalarTypeT<boolean>> Boolean;
extern const std::shared_ptr<const ScalarTypeT<char8>> Character8;
extern const std::shared_ptr<const ScalarTypeT<int8>> SignedInteger8;
extern const std::shared_ptr<const ScalarTypeT<uint8>> UnsignedInteger8;
extern const std::shared_ptr<const ScalarTypeT<int16>> SignedInteger16;
extern const std::shared_ptr<const ScalarTypeT<uint16>> UnsignedInteger16;
extern const std::shared_ptr<const ScalarTypeT<int32>> SignedInteger32;
extern const std::shared_ptr<const ScalarTypeT<uint32>> UnsignedInteger32;
extern const std::shared_ptr<const ScalarTypeT<int64>> SignedInteger64;
extern const std::shared_ptr<const ScalarTypeT<uint64>> UnsignedInteger64;
extern const std::shared_ptr<const ScalarTypeT<float32>> Float32;
extern const std::shared_ptr<const ScalarTypeT<float64>> Float64;
extern const std::shared_ptr<const ScalarTypeT<string>> String;

// ToDo - Time with specialised parse/serialise methods

// Function declaration

// Function definition

template <> inline ScalarTypeT<boolean>::ScalarTypeT (void) : ScalarType("bool", sizeof(boolean)) {}
template <> inline ScalarTypeT<char8>::ScalarTypeT (void) : ScalarType("char8", sizeof(char8)) {}
template <> inline ScalarTypeT<int8>::ScalarTypeT (void) : ScalarType("int8", sizeof(int8)) {}
template <> inline ScalarTypeT<uint8>::ScalarTypeT (void) : ScalarType("uint8", sizeof(uint8)) {}
template <> inline ScalarTypeT<int16>::ScalarTypeT (void) : ScalarType("int16", sizeof(int16)) {}
template <> inline ScalarTypeT<uint16>::ScalarTypeT (void) : ScalarType("uint16", sizeof(uint16)) {}
template <> inline ScalarTypeT<int32>::ScalarTypeT (void) : ScalarType("int32", sizeof(int32)) {}
template <> inline ScalarTypeT<uint32>::ScalarTypeT (void) : ScalarType("uint32", sizeof(uint32)) {}
template <> inline ScalarTypeT<int64>::ScalarTypeT (void) : ScalarType("int64", sizeof(int64)) {}
template <> inline ScalarTypeT<uint64>::ScalarTypeT (void) : ScalarType("uint64", sizeof(uint64)) {}
template <> inline ScalarTypeT<float32>::ScalarTypeT (void) : ScalarType("float32", sizeof(float32)) {}
template <> inline ScalarTypeT<float64>::ScalarTypeT (void) : ScalarType("float64", sizeof(float64)) {}
template <> inline ScalarTypeT<string>::ScalarTypeT (void) : ScalarType("string", sizeof(string)) {}

template <> inline bool ScalarTypeT<string>::ToNetworkByteOrder (void * const ref) const { (void) ref; return true; }
template <> inline bool ScalarTypeT<string>::FromNetworkByteOrder (void * const ref) const { (void) ref; return true; }

template <typename Type> bool ScalarTypeT<Type>::ToNetworkByteOrder (void * const ref) const
{ 

  Type value = ccs::HelperTools::ToNetworkByteOrder(*(static_cast<Type*>(ref))); 
  *(static_cast<Type*>(ref)) = value; 

  return true; 

}

template <typename Type> bool ScalarTypeT<Type>::FromNetworkByteOrder (void * const ref) const
{ 

  Type value = ccs::HelperTools::FromNetworkByteOrder(*(static_cast<Type*>(ref))); 
  *(static_cast<Type*>(ref)) = value; 

  return true; 

}

} // namespace types

} // namespace ccs

#endif // _ScalarType_h_


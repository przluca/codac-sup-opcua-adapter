/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/ArrayType.h $
* $Id: ArrayType.h 100890 2019-07-19 07:14:30Z bauvirb $
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
 * @file ArrayType.h
 * @brief Header file for introspectable ArrayType class.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the ArrayType class.
 */

#ifndef _ArrayType_h_
#define _ArrayType_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#include "AnyType.h" // Base class definition

// Constants

// Type definition

namespace ccs {

namespace types {

class CompoundType; // Forward class declaration
class ScalarType; // Forward class declaration

/**
 * @brief Introspectable type definition for array types.
 * @details The interface considers both multiplicity (maximum number of elements) and current number of elements
 * to account for future modification to support less than maximum number of elements in any particular instant.
 */

class ArrayType : public AnyType
{

  private:

    std::shared_ptr<const AnyType> __base;
    uint32 __multiplicity;

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    ArrayType (void);

    /**
     * @brief Copy constructor.
     */

    ArrayType (const ArrayType& type);

    /**
     * @brief Constructor.
     * @param type Name of type definition.
     * @param base Base type definition for element.
     * @param multiplicity Maximum number of elements.
     * @post
     *   __base = base && __multiplicity = multiplicity
     */

    ArrayType (const char8 * const type, const std::shared_ptr<const AnyType>& base, const uint32 multiplicity = 0u);

    /**
     * @brief Constructor.
     * @param type Name of type definition.
     * @param base Base type definition for element.
     * @param multiplicity Maximum number of elements.
     * @post
     *   __base = base && __multiplicity = multiplicity
     */

    ArrayType (const char8 * const type, const AnyType * const base, const uint32 multiplicity = 0u);

    /**
     * @brief Constructor.
     * @param type Name of type definition.
     * @param base Base type definition for element.
     * @param multiplicity Maximum number of elements.
     * @post
     *   __base = base && __multiplicity = multiplicity
     *
     * @notice Makes a copy of the AnyType instance before storing it as smart pointer 
     * so as to avoid deletion before the actual instance goes out of scope.
     */

    ArrayType (const char8 * const type, const ArrayType& base, const uint32 multiplicity = 0u);
    ArrayType (const char8 * const type, const CompoundType& base, const uint32 multiplicity = 0u);
    ArrayType (const char8 * const type, const ScalarType& base, const uint32 multiplicity = 0u);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~ArrayType (void); 

    /**
     * @brief Test method.
     * @detail Tests if type has element at index.
     * @param index Index of element.
     * @return True if successful.
     */

    bool HasElement (const uint32 index) const;

    /**
     * @brief Accessor.
     * @return Element type definition.
     */

    std::shared_ptr<const AnyType> GetElementType (void) const;

    /**
     * @brief Accessor.
     * @param type Element type definition.
     * @return Class instance.
     */

    ArrayType* SetElementType (const std::shared_ptr<const AnyType>& type);

    /**
     * @brief Accessor.
     * @param type Element type definition.
     * @return Class instance.
     */

    ArrayType* SetElementType (const AnyType* type);

    /**
     * @brief Accessor.
     * @return Number of elements in array.
     */

    uint32 GetElementNumber (void) const;

    /**
     * @brief Accessor.
     * @param index Element index in array.
     * @return Memory offset of element at index.
     */

    uint32 GetElementOffset (const uint32 index) const;

    /**
     * @brief Accessor.
     * @param ref Buffer holding the type instance.
     * @param index Element index in array.
     * @return Memory location of element at index.
     */

    void* GetElementReference (const void * const ref, const uint32 index) const;

    /**
     * @brief Accessor.
     * @return Maximum number of elements in array.
     */

    uint32 GetMultiplicity (void) const;

    /**
     * @brief Accessor.
     * @param multiplicity Maximum number of elements.
     * @return Class instance.
     * @post
     *   GetSize() == GetMultiplicity() * __base.GetSize()
     */

    ArrayType* SetMultiplicity (uint32 multiplicity);

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
     * @detail Compares against other type definition for equivalence. The two type definition
     * are construed to be equivalent if their size, number of elements are identical and the
     * element types are equivalent.
     * @return True if successful.
     */

    virtual bool operator== (const AnyType& type) const; // Specialises virtual methos
    bool operator== (const ArrayType& type) const;

    /**
     * @brief Copy assignment operator.
     */

    ArrayType& operator= (const ArrayType& type); 

};

// Global variables

// Function declaration

// Function definition
  
} // namespace types

} // namespace ccs

#endif // _ArrayType_h_


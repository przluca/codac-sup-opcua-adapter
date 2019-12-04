/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/CompoundType.h $
* $Id: CompoundType.h 100890 2019-07-19 07:14:30Z bauvirb $
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
 * @file CompoundType.h
 * @brief Header file for introspectable CompoundType class.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the CompoundType class.
 */

#ifndef _CompoundType_h_
#define _CompoundType_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. helper functions

//#define USE_LUTABLE

#include "LookUpTable.h"

#include "AnyType.h" // Base class definition

#include "ScalarType.h"

// Constants

// Type definition

namespace ccs {

namespace types {

class ArrayType; // Forward class declaration
//class ScalarType; // Forward class declaration

/**
 * @brief Introspectable type definition for compound types.
 */

class CompoundType : public AnyType
{

  private:

    ccs::base::LookUpTable<std::shared_ptr<const AnyType>> __attributes;
    ccs::base::LookUpTable<uint32> __offsets;

  protected:

  public:

    /**
     * @brief Constructor.
     */

    CompoundType (void);

    /**
     * @brief Copy constructor.
     */

    CompoundType (const CompoundType& type);

    /**
     * @brief Constructor.
     * @param type Name of type definition.
     */

    explicit CompoundType (const char8 * const type);

    /**
     * @brief Destructor.
     * @detail Frees allocated memory for attributes/offsets vectors.
     */

    virtual ~CompoundType (void);

    /**
     * @brief Add attribute to type definition.
     * @detail The method consults the ccs::types::GlobalTypeDatabase and
     * creates the attribute if any such type name can be found.
     * @param name Name of attribute.
     * @param type Type of attribute.
     * @return Class instance.
     *
     * @code
       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
         ->AddAttribute("timestamp", "uint64")
         ->AddAttribute("value", "MyValueType_t"); // Previously registered type
       @endcode
     */

    CompoundType* AddAttribute (const char8 * const name, const char8 * const type);

    /**
     * @brief Add attribute to type definition.
     * @param name Name of attribute.
     * @param type Type of attribute.
     * @return Class instance.
     *
     * @code
       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
         ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
         ->AddAttribute("value", ccs::types::Float64);
       @endcode
     */

    CompoundType* AddAttribute (const char8 * const name, const std::shared_ptr<const AnyType>& type);

    /**
     * @brief Add attribute to type definition.
     * @param name Name of attribute.
     * @param type Type of attribute.
     * @return Class instance.
     *
     * @code
       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
         ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
         ->AddAttribute("value", ccs::types::Float64);
       @endcode
     */

    CompoundType* AddAttribute (const char8 * const name, const AnyType* type);

    /**
     * @brief Add attribute to type definition.
     * @param name Name of attribute.
     * @param type Type of attribute.
     * @return Class instance.
     *
     * @code
       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
         ->AddAttribute("timestamp", ccs::types::UnsignedInteger64)
         ->AddAttribute("value", ccs::types::Float64);
       @endcode
     *
     * @notice Makes a copy of the AnyType instance before storing it as smart pointer 
     * so as to avoid deletion before the actual instance goes out of scope.
     */

    CompoundType* AddAttribute (const char8 * const name, const ArrayType& type);
    CompoundType* AddAttribute (const char8 * const name, const CompoundType& type);
    CompoundType* AddAttribute (const char8 * const name, const ScalarType& type);

    /**
     * @brief Add attribute to type definition.
     * @param name Name of attribute.
     * @return Class instance.
     *
     * @code
       // Introspectable type definition
       ccs::types::CompoundType* type = (new (std::nothrow) ccs::types::CompoundType ("MyType_t"))
         ->AddAttribute<ccs::types::uint64>("timestamp")
         ->AddAttribute<ccs::types::float64>("value");
       @endcode
     */

    template <typename Type> CompoundType* AddAttribute (const char8 * const name); // For native scalar types

    /**
     * @brief Test method.
     * @detail Tests if type has attribute at index.
     * @param index Index of attribute.
     * @return True if successful.
     */

    bool HasAttribute (const uint32 index) const;

    /**
     * @brief Test method.
     * @detail Tests if type has attribute with name.
     * @param name Name of attribute.
     * @return True if successful.
     */

    bool HasAttribute (const char8 * const name) const;

    /**
     * @brief Accessor.
     * @param name Name of attribute.
     * @return Index of attribute for name, 0 if invalid attribute.
     */

    uint32 GetAttributeIndex (const char8 * const name) const;

    /**
     * @brief Accessor.
     * @param index Index of attribute.
     * @return Name of attribute at index, NULL if invalid attribute.
     */

    const char8* GetAttributeName (const uint32 index) const;

    /**
     * @brief Accessor.
     * @return Number of attributes.
     */

    uint32 GetAttributeNumber (void) const;

    /**
     * @brief Accessor.
     * @param index Index of attribute.
     * @return Memory offset of attribute at index, 0 if invalid attribute.
     */

    uint32 GetAttributeOffset (const uint32 index) const;

    /**
     * @brief Accessor.
     * @param name Name of attribute.
     * @return Memory offset of attribute for name, 0 if invalid attribute.
     */

    uint32 GetAttributeOffset (const char8 * const name) const;

    /**
     * @brief Accessor.
     * @param ref Buffer holding the type instance.
     * @param index Index of attribute.
     * @return Memory location of attribute at index, NULL if invalid attribute.
     */

    void* GetAttributeReference (const void* ref, const uint32 index) const;

    /**
     * @brief Accessor.
     * @param ref Buffer holding the type instance.
     * @param name Name of attribute.
     * @return Memory location of attribute for name, NULL if invalid attribute.
     */

    void* GetAttributeReference (const void* ref, const char8 * const name) const;

    /**
     * @brief Accessor.
     * @param index Index of attribute.
     * @return Byte size of attribute type definition at index, 0 if invalid attribute.
     */

    uint32 GetAttributeSize (const uint32 index) const ;

    /**
     * @brief Accessor.
     * @param name Name of attribute.
     * @return Byte size of attribute type definition for name, 0 if invalid attribute.
     */

    uint32 GetAttributeSize (const char8 * const name) const;

    /**
     * @brief Accessor.
     * @param index Index of attribute.
     * @return Attribute type definition at index, NULL if invalid attribute.
     */

    std::shared_ptr<const AnyType> GetAttributeType (const uint32 index) const;

    /**
     * @brief Accessor.
     * @param name Name of attribute.
     * @return Attribute type definition for name, NULL if invalid attribute.
     */

    std::shared_ptr<const AnyType> GetAttributeType (const char8 * const name) const;

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
     * are construed to be equivalent if their size, number of attributes are identical and the
     * attributes types are equivalent.
     * @return True if successful.
     */

    virtual bool operator== (const AnyType& type) const; // Specialises virtual method
    bool operator== (const CompoundType& type) const;

    /**
     * @brief Copy assignment operator.
     * @note Necessary due to dynamic allocation of attributes/offsets vectors.
     */

    CompoundType& operator= (const CompoundType& type); 

};

// Global variables

// Function declaration

// Function definition
  
//template <typename Type> CompoundType* CompoundType::AddAttribute (const char8 * const name) { return this; }

template <> inline CompoundType* CompoundType::AddAttribute<boolean> (const char8 * const name) { return this->AddAttribute(name, Boolean); }
template <> inline CompoundType* CompoundType::AddAttribute<int8> (const char8 * const name) { return this->AddAttribute(name, SignedInteger8); }
template <> inline CompoundType* CompoundType::AddAttribute<uint8> (const char8 * const name) { return this->AddAttribute(name, UnsignedInteger8); }
template <> inline CompoundType* CompoundType::AddAttribute<int16> (const char8 * const name) { return this->AddAttribute(name, SignedInteger16); }
template <> inline CompoundType* CompoundType::AddAttribute<uint16> (const char8 * const name) { return this->AddAttribute(name, UnsignedInteger16); }
template <> inline CompoundType* CompoundType::AddAttribute<int32> (const char8 * const name) { return this->AddAttribute(name, SignedInteger32); }
template <> inline CompoundType* CompoundType::AddAttribute<uint32> (const char8 * const name) { return this->AddAttribute(name, UnsignedInteger32); }
template <> inline CompoundType* CompoundType::AddAttribute<int64> (const char8 * const name) { return this->AddAttribute(name, SignedInteger64); }
template <> inline CompoundType* CompoundType::AddAttribute<uint64> (const char8 * const name) { return this->AddAttribute(name, UnsignedInteger64); }
template <> inline CompoundType* CompoundType::AddAttribute<float32> (const char8 * const name) { return this->AddAttribute(name, Float32); }
template <> inline CompoundType* CompoundType::AddAttribute<float64> (const char8 * const name) { return this->AddAttribute(name, Float64); }
template <> inline CompoundType* CompoundType::AddAttribute<string> (const char8 * const name) { return this->AddAttribute(name, String); }

} // namespace types

} // namespace ccs

#endif // _CompoundType_h_


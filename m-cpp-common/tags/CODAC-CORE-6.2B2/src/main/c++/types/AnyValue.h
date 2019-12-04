/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyValue.h $
* $Id: AnyValue.h 100890 2019-07-19 07:14:30Z bauvirb $
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
 * @file AnyValue.h
 * @brief Header file for AnyValue class.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyValue class.
 */

#ifndef _AnyValue_h_
#define _AnyValue_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "log-api.h" // Syslog wrapper routines

#include "AnyType.h"
#include "ArrayType.h"
#include "CompoundType.h"
#include "ScalarType.h"

#include "AnyTypeHelper.h"

// Constants

// Type definition

namespace ccs {

namespace types {

/**
 * @brief AnyValue associated to introspectable type definition.
 * @detail The class associates a memory buffer to an introspectable
 * type definition. The memory area is allocated upon instantiation.
 * The class also provides assignment, comparison, and static cast
 * operator overload to basic types.
 *
 * @note The class is associated with static helper routines to
 * simplify access to attributes, etc. without replicating the
 * AnyType interface. See AnyValueHelper.h.
 *
 * @todo Ownership externally managed .. type should not necessarilly 
 * be deleted upon deletion of the AnyValue instance.
 */

class AnyValue
{

  private:

    std::shared_ptr<const AnyType> __type;

    uint8* __instance;
    bool __allocated;

    ccs::types::Endianness __order;

    void* CreateInstance (void);
    void DeleteInstance (void);

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    AnyValue (void);

    /**
     * @brief Constructor.
     * @detail Instantiates memory for type instance.
     * @param type Introspectable type definition as JSON stream.
     */

    explicit AnyValue (const char8 * const type);

    /**
     * @brief Constructor.
     * @detail Instantiates memory for type instance.
     * @param type Introspectable type definition.
     */

    AnyValue (const std::shared_ptr<const AnyType>& type);

    /**
     * @brief Constructor.
     * @detail Instantiates memory for type instance.
     * @param type Introspectable type definition.
     */

    explicit AnyValue (const AnyType * const type);

    /**
     * @brief Constructor.
     * @detail Instantiates memory for type instance.
     * @param type Introspectable type definition.
     */

    AnyValue (const ArrayType& type);
    AnyValue (const CompoundType& type);
    AnyValue (const ScalarType& type);

    /**
     * @brief Constructor.
     * @detail Memory is not allocated and assumed to be
     * managed by the calling application, e.g. a C-like
     * struct mapped to an introspectable type definition.
     * @param type Introspectable type definition.
     * @param instance Externally managed type instance.
     */

    AnyValue (const std::shared_ptr<const AnyType>& type, void * const instance);

    /**
     * @brief Constructor.
     * @detail Memory is not allocated and assumed to be
     * managed by the calling application, e.g. a C-like
     * struct mapped to an introspectable type definition.
     * @param type Introspectable type definition.
     * @param instance Externally managed type instance.
     */

    AnyValue (const AnyType * const type, void * const instance);

    /**
     * @brief Constructor.
     * @detail Memory is not allocated and assumed to be
     * managed by the calling application, e.g. a C-like
     * struct mapped to an introspectable type definition.
     * @param type Introspectable type definition.
     * @param instance Externally managed type instance.
     */

    AnyValue (const ArrayType& type, void * const instance);
    AnyValue (const CompoundType& type, void * const instance);
    AnyValue (const ScalarType& type, void * const instance);

    /**
     * @brief Copy constructor.
     */

    AnyValue (const AnyValue& value); // Copy constructor

    /**
     * @brief Destructor.
     * @detail Frees allocated memory for type instance.
     */

    virtual ~AnyValue (void);

    /**
     * @brief Accessor.
     * @return Byte size of type definition.
     */

    uint32 GetSize (void) const;

    /**
     * @brief Accessor.
     * @return Introspectable type definition.
     */

    std::shared_ptr<const AnyType> GetType (void) const;

    /**
     * @brief Accessor.
     * @return Type instance memory location.
     */

    void* GetInstance (void) const;

    /**
     * @brief Byte ordering conversion.
     * @detail In-place conversion from host platform endianness to network byte order (big-endianness).
     * @return True if successful.
     */

    bool ToNetworkByteOrder (void);

    /**
     * @brief Byte ordering conversion.
     * @detail In-place conversion from network byte order (big-endianness).
     * @return True if successful.
     */

    bool FromNetworkByteOrder (void);

    /**
     * @brief Serialisation method.
     * @detail Parses serialised buffer and provides binary equivalent.
     * @param buffer Serialised instance value.
     * @return True if successful.
     */

    bool ParseInstance (const char8 * const buffer);

    /**
     * @brief Serialisation method.
     * @detail Serialises type instance to string buffer.
     * @param buffer Placeholder to receive serialised instance value.
     * @param size Size of string buffer.
     * @return True if successful.
     */

    bool SerialiseInstance (char8 * const buffer, uint32 size) const;

    /**
     * @brief Serialisation method.
     * @detail Serialises type definition to string buffer.
     * @param buffer Placeholder to receive serialised type.
     * @param size Size of string buffer.
     * @return True if successful.
     */

    bool SerialiseType (char8 * const buffer, uint32 size) const;

    /**
     * @brief Comparison operator.
     * @detail Compares instance with externally provided value. The operator allows to
     * compare to basic types, C-like struct, etc.
     * @return True if successful.
     *
     * @code
       ccs::types::AnyValue value (ccs::types::UnsignedInteger32);
        
       if (value == 0u)
         // Do something
       else
         // Do something else
       @endcode
     *
     * @todo Extend to other types of operators, e.g. increment.
     */

    template <typename Type> bool operator== (const Type& value) const;

    /**
     * @brief Assignment operator.
     * @detail Set instance to externally provided value. The operator allows to
     * assign to basic types, from C-like struct, etc.
     *
     * @code
       // C-like structure definition
       typedef struct MyType {
         ccs::types::uint64 timestamp;
         ccs::types::float64 value;
       } MyType_t;

       // Equivalent introspectable type definition
       ccs::types::CompoundType type ("MyType_t"); 
       type.AddAttribute("timestamp", ccs::types::UnsignedInteger64);
       type.AddAttribute("value", ccs::types::Float64);

       MyType_t data; 
       data.timestamp = ccs::HelperTools::GetCurrentTime();
       data.value = 0.0;

       // Assignment
       ccs::types::AnyValue value (type) = data;
       @endcode
     */

    template <typename Type> AnyValue& operator= (const Type& value); // Assign from externally defined variable

    /**
     * @brief Copy assignment operator.
     */

    AnyValue& operator= (const AnyValue& value); // Copy assignment operator

    /**
     * @brief Static cast operator.
     * @detail Use instance to set externally provided value. The operator allows to
     * cast to to basic types, C-like struct, etc.
     *
     * @code
       // C-like structure definition
       typedef struct MyType {
         ccs::types::uint64 timestamp;
         ccs::types::float64 value;
       } MyType_t;

       // Equivalent introspectable type definition
       ccs::types::CompoundType type ("MyType_t"); 
       type.AddAttribute("timestamp", ccs::types::UnsignedInteger64);
       type.AddAttribute("value", ccs::types::Float64);

       ccs::types::AnyValue value (type);

       // Cast introspectable instance to C-like structure
       MyType_t data = static_cast<MyType_t>(value); 
       @endcode
     */

    template <typename Type> operator Type() const; // Cast to externally defined variable

};

// Global variables

// Function declaration

// Function definition

template <typename Type> bool AnyValue::operator== (const Type& value) const
{

  bool status = (this->GetSize() == sizeof(Type));

  if (status)
    {
      for (uint32 index = 0u; index < this->GetSize(); index += 1u)
        {
          // Byte per byte comparison
          status = (*(static_cast<const uint8*>(this->GetInstance()) + index) == 
                    *(reinterpret_cast<const uint8*>(&value) + index));
        }
    }

  return status;

}

template <typename Type> AnyValue& AnyValue::operator= (const Type& value) 
{ 

  bool status = (this->GetSize() == sizeof(Type));

  if (status)
    {
      (void)memcpy(this->GetInstance(), &value, this->GetSize());
    }
  else
    {
      log_warning("template <typename Type> AnyValue& AnyValue::operator= - Size error '%u' vs '%u'", this->GetSize(), sizeof(Type));
      (void)memcpy(this->GetInstance(), &value, this->GetSize());
    }

  return *this;

}

template <typename Type> AnyValue::operator Type() const
{

  Type value;

  (void)memset(&value, 0, sizeof(Type));

  bool status = (this->GetSize() == sizeof(Type));

  if (status)
    {
      (void)memcpy(&value, this->GetInstance(), sizeof(Type));
    }
  else
    {
      log_warning("template <typename Type> AnyValue::operator Type() - Size error '%u' vs '%u'", this->GetSize(), sizeof(Type));
      (void)memcpy(&value, this->GetInstance(), sizeof(Type));
    }

  return value;

}

} // namespace types

} // namespace ccs

#endif // _AnyValue_h_ 

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyType.h $
* $Id: AnyType.h 100890 2019-07-19 07:14:30Z bauvirb $
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
 * @file AnyType.h
 * @brief Header file for AnyType class.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyType class.
 */

#ifndef _AnyType_h_
#define _AnyType_h_

// Global header files

// Local header files

#include "BasicTypes.h" // Misc. type definition

// Constants

// Type definition

namespace ccs {

namespace types {

//typedef enum AnyTypeId : uint32 {
typedef enum AnyTypeId {

  Undefined = 0,
  Array,
  Compound,  
  Scalar,
  Union

} AnyTypeId_t;

/**
 * @brief Base class for introspectable type definition.
 * @note Copy constructor and cop assignment operators removed as class is still abstract.
 */

class AnyType
{

  private:

    uint32 __size;
    char8 __type [STRING_MAX_LENGTH];

    /**
     * @brief Copy constructor.
     * @note Undefined as invalid operation to copy instances of this class.
     */

    AnyType (const AnyType& type); // Undefined

    /**
     * @brief Copy assignment operator.
     * @note Undefined as invalid operation to copy instances of this class.
     */

    AnyType& operator= (const AnyType& type); // Undefined

  protected:

  public:

    /**
     * @brief Constructor.
     * @post
     *   __size = 0u && __type = "undefined"
     */

    AnyType (void);

    /**
     * @brief Constructor.
     * @param type Name of type definition.
     * @post
     *   __size = 0u && __type = type
     */

    explicit AnyType (const char8 * const type);

    /**
     * @brief Constructor.
     * @param type Name of type definition.
     * @param size Byte size of type definition.
     * @post
     *   __size = size && __type = type
     */

    AnyType (const char8 * const type, const uint32 size);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~AnyType (void);

    /**
     * @brief Accessor.
     * @return Byte size of type definition.
     */

    uint32 GetSize (void) const;

    /**
     * @brief Accessor.
     * @param size Byte size of type definition.
     * @return Class instance.
     */

    AnyType* SetSize (const uint32 size);
 
    /**
     * @brief Accessor.
     * @return Name of type definition.
     */

    const char8 * GetName (void) const;

    /**
     * @brief Accessor.
     * @param type Name of type definition.
     * @return Class instance.
     */

    AnyType* SetName (const char8 * const type);

    /**
     * @brief Virtual method.
     * @detail Instantiates and resets heap memory to host one instance of the type.
     * @return Memory pointer or NULL in case of error.
     */

    virtual void* CreateInstance (void) const;

    /**
     * @brief Virtual method.
     * @detail Deletes previously instantiated memory area.
     */

    virtual void DeleteInstance (const void * const ref) const;

    /**
     * @brief Test method.
     * @detail Tests name of type definition.
     * @param type Name of type definition.
     * @return True if successful.
     */

    bool IsType (const char8 * const type) const;

    /**
     * @brief Pure virtual method.
     * @detail In-place conversion from host platform endianness to network byte order (big-endianness).
     * @param ref Buffer holding the instance to be converted.
     * @return True if successful.
     */

    virtual bool ToNetworkByteOrder (void * const ref) const = 0; // Pure virtual method

    /**
     * @brief Pure virtual method.
     * @detail In-place conversion from network byte order (big-endianness).
     * @param ref Buffer holding the instance to be converted.
     * @return True if successful.
     */

    virtual bool FromNetworkByteOrder (void * const ref) const = 0; // Pure virtual method

    /**
     * @brief Comparison operator.
     * @detail Compares against other type definition for equivalence.
     * @return True if successful.
     */

    virtual bool operator== (const AnyType& type) const = 0; // Pure virtual method

};

// Global variables

// Function declaration

// Function definition

} // namespace types

} // namespace ccs

#endif // _AnyType_h_ 


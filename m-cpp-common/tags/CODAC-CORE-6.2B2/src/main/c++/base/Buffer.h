/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/Buffer.h $
* $Id: Buffer.h 100814 2019-07-17 13:23:31Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Templated array management class
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
 * @file Buffer.h
 * @brief Header file for Buffer<Type> class.
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the Buffer<Type> class.
 */

#ifndef _Buffer_h_
#define _Buffer_h_

// Global header files

#include <new> // std::nothrow

// Local header files

#include "BasicTypes.h"

#include "AtomicLock.h" // AtomicLock class definition

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides templated memory buffer management to store and
 * retrieve data at index.
 * @warning The accessors do not use the class locking mechanism which must be
 * used by the application-specific code, if necessary. See ccs::base::AtomicLock.
 */

template <typename Type> class Buffer : public AtomicLock
{

  private:

    ccs::types::uint32 m_size;
    Type* m_buffer;

    Buffer ();
    Buffer (const Buffer& buffer);
    Buffer& operator= (const Buffer& buffer);

  public:

    /**
     * @brief Constructor. Allocates memory from the heap.
     * @post
     *   m_buffer = new Type [size]
     */

    explicit Buffer (const ccs::types::uint32 size);

    /**
     * @brief Destructor. Frees allocated memory.
     */

    virtual ~Buffer (void);

    /**
     * @brief Initialiser. Preloads templated array with default value.
     * @return TRUE if the memory was properly allocated. 
     */

    bool Initialise (const Type& data);

    /**
     * @brief Accessor. Returns size of templated array.
     * @return size of array.
     */

    ccs::types::uint32 GetSize (void) const { return m_size; };

    /**
     * @brief Accessor. Rerieves data at index.
     * @return TRUE if valid index.
     */

    bool GetData (Type& data, const ccs::types::uint32 index) const;

    /**
     * @brief Accessor. Inserts data at index.
     * @return TRUE if valid index.
     */

    bool PutData (const Type& data, const ccs::types::uint32 index);

};

// Global variables

// Function declaration

// Function definition

template <typename Type> Buffer<Type>::Buffer (const ccs::types::uint32 size) : AtomicLock()
{ 

  m_size = size;
  m_buffer = NULL_PTR_CAST(Type *);
  
  // Allocate memory
  m_buffer = new (std::nothrow) Type [m_size];
  
}

template <typename Type> Buffer<Type>::~Buffer (void) 
{ 

  // Ensure the lock is free prior to releasing memory
  AcquireLock();

  if (m_buffer != NULL_PTR_CAST(Type *)) 
    {
      delete [] m_buffer;
    }

  m_buffer = NULL_PTR_CAST(Type *);

  ReleaseLock();

}

template <typename Type> bool Buffer<Type>::Initialise (const Type& data) 
{ 

  bool status = (m_buffer != NULL_PTR_CAST(Type *));

  for (ccs::types::uint32 index = 0u; ((index < m_size) && status); index++) 
    {
      status = PutData(data, index);
    }

  return status; 

}

template <typename Type> bool Buffer<Type>::GetData (Type& data, const ccs::types::uint32 index) const 
{ 

  bool status = ((m_buffer != NULL_PTR_CAST(Type *)) && (index < m_size));

  if (status) 
    {
      data = m_buffer[index]; 
    }
  
  return status; 

}

template <typename Type> bool Buffer<Type>::PutData (const Type& data, const ccs::types::uint32 index) 
{ 

  bool status = ((m_buffer != NULL_PTR_CAST(Type *)) && (index < m_size));

  if (status) 
    {
      m_buffer[index] = data; 
    }
  
  return status; 

}

} // namespace base

} // namespace ccs

#endif // _Buffer_h_


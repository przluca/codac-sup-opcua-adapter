/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/FIFOBuffer.h $
* $Id: FIFOBuffer.h 100920 2019-07-20 12:45:19Z bauvirb $
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
 * @file FIFOBuffer.h
 * @brief Header file for FIFOBuffer<Type> class.
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the FIFOBuffer<Type> class.
 */

#ifndef _FIFOBuffer_h_
#define _FIFOBuffer_h_

// Global header files

// Local header files

#include "BasicTypes.h"
#include "Buffer.h"

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides templated memory buffer management to store data
 * in a First-In-First-Out way. 
 * @warning The accessors do not use any locking mechanism. See Buffer<Type>
 * base class for a description of the locking mechanism.
 */

template <typename Type> class FIFOBuffer : public Buffer<Type> 
{

  private:

    volatile ccs::types::uint32 m_count;
    volatile ccs::types::uint32 m_head;
    volatile ccs::types::uint32 m_tail;

    FIFOBuffer ();
    FIFOBuffer (const FIFOBuffer& buffer);
    FIFOBuffer& operator= (const FIFOBuffer& buffer);

  public:

    /**
     * @brief Constructor. Allocates memory from the standard heap.
     * @post
     *   m_count = m_head = m_tail = 0u
     */

    explicit FIFOBuffer (const ccs::types::uint32 size);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~FIFOBuffer (void) {}; 

    /**
     * @brief Tests if FIFO is empty.
     * @return TRUE if FIFO is empty.
     */

    bool IsEmpty (void) const;

    /**
     * @brief Tests if FIFO is full.
     * @return TRUE if FIFO is full.
     */

    bool IsFull (void) const;

    /**
     * @brief Accessor. Removes data at the head of the FIFO.
     * @return TRUES if the FIFO is not empty.
     */

    bool PullData (Type& data);

    /**
     * @brief Accessor. Inserts data at the tail of the FIFO.
     * @return TRUE if the FIFO is not yet full.
     */

    bool PushData (Type& data);

};

// Global variables

// Function declaration

// Function definition

template <typename Type> FIFOBuffer<Type>::FIFOBuffer (const ccs::types::uint32 size) : Buffer<Type> (size) 
{ 

  this->m_count = 0u;
  this->m_head = 0u;
  this->m_tail = 0u; 

  return;

}

template <typename Type> bool FIFOBuffer<Type>::IsEmpty (void) const
{ 
  return (m_count == 0u); 
}

template <typename Type> bool FIFOBuffer<Type>::IsFull (void) const
{ 
  return (m_count == this->Buffer<Type>::GetSize()); 
}

template <typename Type> bool FIFOBuffer<Type>::PullData (Type& data) 
{ 

  bool status = !IsEmpty();

  if (status)
    {
      ccs::types::uint32 size = this->Buffer<Type>::GetSize(); 

      (void)this->Buffer<Type>::GetData(data, this->m_head);
      (void)__sync_fetch_and_sub(&m_count, 1u);
      (void)__sync_fetch_and_add(&m_head, 1u); 
      (void)__sync_bool_compare_and_swap(&m_head, size, 0u);
    }
      
  return status;

}

template <typename Type> bool FIFOBuffer<Type>::PushData (Type& data) 
{ 
  
  bool status = !IsFull();

  if (status)
    {
      ccs::types::uint32 size = this->Buffer<Type>::GetSize(); 

      (void)this->Buffer<Type>::PutData(data, this->m_tail);
      (void)__sync_fetch_and_add(&m_count, 1u);
      (void)__sync_fetch_and_add(&m_tail, 1u); 
      (void)__sync_bool_compare_and_swap(&m_tail, size, 0u);
    }
      
  return status;
  
}

} // namespace base

} // namespace ccs

#endif // _FIFOBuffer_h_


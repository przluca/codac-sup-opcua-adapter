/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/HIFOBuffer.h $
* $Id: HIFOBuffer.h 100814 2019-07-17 13:23:31Z bauvirb $
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
 * @file HIFOBuffer.h
 * @brief Header file for HIFOBuffer<Type> class.
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the HIFOBuffer<Type> class.
 */

#ifndef _HIFOBuffer_h_
#define _HIFOBuffer_h_

// Global header files

// Local header files

#include "BasicTypes.h"
#include "Buffer.h"

// Constants

// Type definition

namespace ccs {

namespace base {

template <typename Type> struct RankedDataType
{

  ccs::types::uint64 rank; // uint64 chosen to be able to use this for time-based sorting of samples
  Type value;

};

/**
 * @brief The class provides templated memory buffer management to store data
 * in a First-In-First-Out way, where decision to discard data when full is
 * taken from a rank attribute associated to each entry. 
 * @warning The accessors do not use any locking mechanism. See Buffer<Type>
 * base class for a description of the locking mechanism.
 */

template <typename Type> class HIFOBuffer : public Buffer< RankedDataType<Type> > 
{ 

  private:

    bool m_invert;
    ccs::types::uint32 m_count;
    ccs::types::uint32 m_head;
    ccs::types::uint32 m_tail;

    // Accessor methods
    ccs::types::uint64 GetRank (const ccs::types::uint32 index) const;

    HIFOBuffer ();
    HIFOBuffer (const HIFOBuffer& buffer);
    HIFOBuffer& operator= (const HIFOBuffer& buffer);

  protected:

  public:

    /**
     * @brief Constructor. Allocates memory from the standard heap.
     * @post
     *   m_count = m_head = m_tail = 0u
     */

    explicit HIFOBuffer (const ccs::types::uint32 size);
    HIFOBuffer (const ccs::types::uint32 size, const bool invert);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~HIFOBuffer (void) {}; 

    /**
     * @brief Initialiser. Preloads templated array with default value.
     * @return TRUE if the memory was properly allocated. 
     */

    bool Initialise (const Type& data);

    /**
     * @brief Tests if empty.
     * @return TRUE if empty.
     */

    bool IsEmpty (void) const;

    /**
     * @brief Tests if full.
     * @return TRUE if full.
     */

    bool IsFull (void) const;

    /**
     * @brief Accessor. Removes data at the head of the HIFO.
     * @return TRUE if the HIFO is not empty.
     */

    bool PullData (Type& data);

    /**
     * @brief Accessor. Inserts data at the tail of the HIFO.
     * @return TRUE if the HIFO is not yet full.
     */

    bool PushData (const Type& data);

    /**
     * @brief Accessor. Inserts data at the appropriate ranked
     * location in the HIFO.
     * @return TRUE upon insertion. 
     * @post
     *   An entry of lower rank has been dropped if full.
     */

    bool PushData (const Type& data, const ccs::types::uint64 rank);

};

// Global variables

// Function declaration

// Function definition

template <typename Type> ccs::types::uint64 HIFOBuffer<Type>::GetRank (const ccs::types::uint32 index) const
{ 

  struct RankedDataType<Type> temp;
 
  (void)this->GetData(temp, index); 

  return temp.rank; 

}

template <typename Type> HIFOBuffer<Type>::HIFOBuffer (const ccs::types::uint32 size) : Buffer< RankedDataType<Type> > (size)
{ 

  this->m_invert = false; 

  this->m_count = 0u;
  this->m_head = 0u;
  this->m_tail = 0u; 

  return;

}

template <typename Type> HIFOBuffer<Type>::HIFOBuffer (const ccs::types::uint32 size, const bool invert) : Buffer< RankedDataType<Type> > (size)
{ 

  this->m_invert = invert; 

  this->m_count = 0u;
  this->m_head = 0u;
  this->m_tail = 0u; 

  return;

}

template <typename Type> bool HIFOBuffer<Type>::Initialise (const Type& data) 
{ 

  struct RankedDataType<Type> temp;
 
  temp.rank = 0ul; 
  temp.value = data; 

  bool status = this->Buffer< RankedDataType<Type> >::Initialise(temp);

  return status; 

}

template <typename Type> bool HIFOBuffer<Type>::IsEmpty (void) const
{ 
  return (m_count == 0u); 
}

template <typename Type> bool HIFOBuffer<Type>::IsFull (void) const
{ 
  return (m_count == this->Buffer< RankedDataType<Type> >::GetSize());
}

template <typename Type> bool HIFOBuffer<Type>::PullData (Type& data)
{ 

  bool status = !IsEmpty();

  if (status)
    {
      struct RankedDataType<Type> temp; 

      (void)this->GetData(temp, m_head); 

      data = temp.value; 

      this->m_count--; 
      this->m_head++;
 
      if (this->m_head == this->GetSize()) 
        {
          this->m_head = 0u; 
        }
    }

  return status; 

}

template <typename Type> bool HIFOBuffer<Type>::PushData (const Type& data) 
{ 

  bool status = !IsFull();

  if (status)
    {
      struct RankedDataType<Type> temp; 

      temp.rank = 0ul; 
      temp.value = data; 

      (void)this->PutData(temp, this->m_tail); 

      this->m_count++; 
      this->m_tail++; 

      if (this->m_tail == this->GetSize()) 
        {
          this->m_tail = 0u; 
        }
    }

  return status; 

}

template <typename Type> bool HIFOBuffer<Type>::PushData (const Type& data, const ccs::types::uint64 rank) 
{

  bool status = false;

  if (IsEmpty()) 
    { 
      struct RankedDataType<Type> temp; 

      temp.rank = rank; 
      temp.value = data; 

      (void)this->PutData(temp, this->m_tail); 

      this->m_count++; 
      this->m_tail++; 

      if (this->m_tail == this->GetSize()) 
        {
          this->m_tail = 0u; 
        }

      status = true;
      return status; 
    }
  
  // Index identifies the tail of the queue
  ccs::types::uint32 index = this->m_tail;
  
  if (IsFull()) // Head and tail markers are identical
    {
      // Index identifies most recent data of smallest rank

      if (index == 0u) { index = this->GetSize(); }
      index--;

      if (((this->m_invert == false) && (rank <= this->GetRank(index))) ||
          ((this->m_invert == true) && (rank >= this->GetRank(index))))
        {
          // Do nothing
          return status;
        }
    }
  else // Insert data wherever appropriate
    {
      // Index identifies the tail of the queue

      this->m_count++; 
      this->m_tail++; 

      if (this->m_tail == this->GetSize()) 
        {
          this->m_tail = 0u; 
        }
    }
  
  struct RankedDataType<Type> temp; 

  temp.rank = rank; 
  temp.value = data;
  
  // Insert newest data at the tail of the queue
  (void)this->PutData(temp, index); // Insert at the place of the most recent data of smallest rank
  
  // Swap until data of same or higher rank is found
  ccs::types::uint32 swap = index;

  if (index == 0u) { swap = this->GetSize(); }
  swap--;

  while ((index != this->m_head) &&
         (((this->m_invert == false) && (rank > this->GetRank(swap))) ||
          ((this->m_invert == true) && (rank < this->GetRank(swap)))))
    { 
      struct RankedDataType<Type> copy; 

      // Copy data

      (void)this->GetData(copy, swap); 
      
      // Swap data

      (void)this->PutData(temp, swap); 
      (void)this->PutData(copy, index); 
      
      // Update indexes

      if (index == 0u) { index = this->GetSize(); }      
      index--;

      if (index == 0u) { swap = this->GetSize(); } else { swap = index; }      
      swap--;
    }
  
  status = true; 
  
  return status;

}
 
} // namespace base

} // namespace ccs

#endif // _HIFOBuffer_h_

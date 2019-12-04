/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/LIFOBuffer.h $
* $Id: LIFOBuffer.h 100814 2019-07-17 13:23:31Z bauvirb $
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
 * @file LIFOBuffer.h
 * @brief Header file for LIFOBuffer<Type> class
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the LIFOBuffer<Type> class.
 */

#ifndef _LIFOBuffer_h_
#define _LIFOBuffer_h_

/* Global header files */

/* Local header files */

#include "BasicTypes.h"
#include "Buffer.h"

/* Constants */

/* Type definition */

namespace ccs {

namespace base {

/**
 * @brief The class provides templated memory buffer management to store data
 * in a Last-In-First-Out way (stack). 
 * @warning The accessors do not use any locking mechanism. See Buffer<Type>
 * base class for a description of the locking mechanism.
 */

template <typename Type> class LIFOBuffer : public Buffer<Type> 
{

  private:

    volatile ccs::types::uint32 m_top;

    LIFOBuffer ();
    LIFOBuffer (const LIFOBuffer& buffer);
    LIFOBuffer& operator= (const LIFOBuffer& buffer);

  public:

    /**
     * @brief Constructor. Allocates memory from the standard heap.
     * @post
     *   m_top = 0u
     */

    explicit LIFOBuffer (const ccs::types::uint32 size);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~LIFOBuffer (void) {}; 

    /**
     * @brief Tests if LIFO is empty.
     * @return true if LIFO is empty.
     */

    bool IsEmpty (void) const;

    /**
     * @brief Tests if LIFO is full.
     * @return true if LIFO is full.
     */

    bool IsFull (void) const;

    /**
     * @brief Accessor. Removes data at the top of the LIFO.
     * @return TRUE if the LIFO is not empty.
     */

    bool PullData (Type& data);

    /**
     * @brief Accessor. Inserts data at the top of the LIFO.
     * @return TRUE if the LIFO is not yet full.
     */

    bool PushData (Type& data);

};

/* Global variables */

/* Function declaration */

/* Function definition */

template <typename Type> LIFOBuffer<Type>::LIFOBuffer (const ccs::types::uint32 size) : Buffer<Type> (size) 
{ 

  this->m_top = 0u; 

  return;

}

template <typename Type> bool LIFOBuffer<Type>::IsEmpty (void) const
{ 
  return (0u == m_top); 
}

template <typename Type> bool LIFOBuffer<Type>::IsFull (void) const
{ 
  return (m_top == this->Buffer<Type>::GetSize()); 
}

template <typename Type> bool LIFOBuffer<Type>::PullData (Type& data) 
{ 

  bool status = !IsEmpty();

  if (status)
    {
      (void)this->Buffer<Type>::GetData(data, __sync_sub_and_fetch(&m_top, 1u));
    }
      
  return status;

}

template <typename Type> bool LIFOBuffer<Type>::PushData (Type& data) 
{ 
  
  bool status = !IsFull();

  if (status)
    {
      (void)this->Buffer<Type>::PutData(data, __sync_fetch_and_add(&m_top, 1u));
    }
      
  return status;
  
}

} /* namespace base */

} /* namespace ccs */

#endif /* _LIFOBuffer_h_ */


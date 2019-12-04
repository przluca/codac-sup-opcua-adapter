/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/CircularBuffer.h $
* $Id: CircularBuffer.h 100920 2019-07-20 12:45:19Z bauvirb $
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
 * @file CircularBuffer.h
 * @brief Header file for CircularBuffer<Type> class.
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the Buffer<Type> class.
 */

#ifndef _CircularBuffer_h_
#define _CircularBuffer_h_

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
 * in a circular way.
 */

template <typename Type> class CircularBuffer : public Buffer<Type> 
{

  private:

    ccs::types::uint32 m_index;

    CircularBuffer ();
    CircularBuffer (const CircularBuffer& buffer);
    CircularBuffer& operator= (const CircularBuffer& buffer);

  public:

    /**
     * @brief Constructor. Allocates memory from the standard heap.
     * @post
     *   m_index = 0u
     */

    explicit CircularBuffer (const ccs::types::uint32 size);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~CircularBuffer (void) {}; 

    /**
     * @brief Accessor. Inserts data in buffer.
     * @return TRUE if the memory was properly allocated.
     */

    bool PushData (const Type& data);

    /**
     * @brief Accessor. Inserts data in buffer and return removed element.
     * @return TRUE if the memory was properly allocated.
     */

    bool PushData (const Type& data_in, Type& data_out);

};

// Global variables

// Function declaration

// Function definition

template <typename Type> CircularBuffer<Type>::CircularBuffer (const ccs::types::uint32 size) : Buffer<Type> (size) 
{ 

  m_index = 0u;

  return;

}

template <typename Type> bool CircularBuffer<Type>::PushData (const Type& data) 
{ 

  bool status = Buffer<Type>::PutData(data, m_index);
  
  if (status) 
    {
      m_index++;
  
      if (m_index == Buffer<Type>::GetSize()) 
        {
          m_index = 0u;
        }
    }
  
  return status;
  
}

template <typename Type> bool CircularBuffer<Type>::PushData (const Type& data_in, Type& data_out) 
{ 
  
  bool status = Buffer<Type>::GetData(data_out, m_index);
  
  if (status) 
    {
      status = PushData(data_in);
    }
  
  return status;
  
}

} // namespace base

} // namespace ccs

#endif // _CircularBuffer_h_


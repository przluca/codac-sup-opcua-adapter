/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/BlockMemoryBuffer.h $
* $Id: BlockMemoryBuffer.h 100814 2019-07-17 13:23:31Z bauvirb $
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
 * @brief Header file for BlockMemoryBuffer class.
 * @date 21/05/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2019 ITER Organization
 * @detail This header file contains the definition of the BlockMemoryBuffer class.
 */

#ifndef _BlockMemoryBuffer_h_
#define _BlockMemoryBuffer_h_

// Global header files

// Local header files

#include "BasicTypes.h"

#include "AtomicLock.h" // AtomicLock class definition

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides memory buffer management to store and retrieve data at index.
 * @detail The class manipulated data block memory. Each data block has the same size, the
 * memory is managed as an array where data blocks are accessed by index.
 * @warning The accessors do not use the class locking mechanism which must be
 * used by the application-specific code, if necessary. See ccs::base::AtomicLock.
 */

class BlockMemoryBuffer : public AtomicLock
{

  private:

    ccs::types::uint32 _size;
    ccs::types::uint32 _width;
    ccs::types::uint8* _buffer;

    void Allocate (void);
    void Reset (void);

    BlockMemoryBuffer ();
    BlockMemoryBuffer (const BlockMemoryBuffer& fifo);
    BlockMemoryBuffer& operator= (const BlockMemoryBuffer& fifo);

  public:

    /**
     * @brief Constructor. Allocates memory from the heap.
     * @post
     *   _buffer = new uint8 [size*width]
     */

    BlockMemoryBuffer (const ccs::types::uint32 size, const ccs::types::uint32 width);

    /**
     * @brief Destructor. Frees allocated memory.
     */

    virtual ~BlockMemoryBuffer (void);

    /**
     * @brief Accessor. Returns size of memory array.
     * @return size of array.
     */

    ccs::types::uint32 GetSize (void) const;

    /**
     * @brief Accessor. Returns width of memory array.
     * @return width of array.
     */

    ccs::types::uint32 GetWidth (void) const;

    /**
     * @brief Accessor. Returns reference to data block at index.
     * @return Reference if valid index, NULL otherwise.
     */

    void* GetDataBlockReference (const ccs::types::uint32 index) const;

    /**
     * @brief Accessor. Rerieves data at index.
     * @return TRUE if valid index.
     */

    bool GetDataBlock (void * const data, const ccs::types::uint32 index) const;

    /**
     * @brief Accessor. Inserts data at index.
     * @return TRUE if valid index.
     */

    bool PutDataBlock (const void * const data, const ccs::types::uint32 index);

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _BlockMemoryBuffer_h_


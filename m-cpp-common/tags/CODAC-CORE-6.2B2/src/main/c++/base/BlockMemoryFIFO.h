/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/BlockMemoryFIFO.h $
* $Id: BlockMemoryFIFO.h 100814 2019-07-17 13:23:31Z bauvirb $
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
 * @brief Header file for BlockMemoryFIFO class.
 * @date 21/05/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2019 ITER Organization
 * @detail This header file contains the definition of the BlockMemoryFIFO class.
 */

#ifndef _BlockMemoryFIFO_h_
#define _BlockMemoryFIFO_h_

// Global header files

// Local header files

#include "BasicTypes.h"

#include "BlockMemoryBuffer.h" // Base class definition

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides First-In-First-Out (FIFO) memory buffer management.
 * @detail The class manipulated data block memory. Each data block has the same size, the
 * memory is managed as a FIFO.
 * @warning The accessors do not use the class locking mechanism which must be
 * used by the application-specific code, if necessary. See ccs::base::AtomicLock.
 */

class BlockMemoryFIFO : public BlockMemoryBuffer
{

  private:

    ccs::types::uint32 _count;
    ccs::types::uint32 _head;
    ccs::types::uint32 _tail;

    void Initialise (void);

    BlockMemoryFIFO ();
    BlockMemoryFIFO (const BlockMemoryFIFO& fifo);
    BlockMemoryFIFO& operator= (const BlockMemoryFIFO& fifo);

  public:

    /**
     * @brief Constructor. Allocates memory from the heap.
     * @post
     *   _count = _head = _tail = 0u;
     */

    BlockMemoryFIFO (const ccs::types::uint32 size, const ccs::types::uint32 width);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~BlockMemoryFIFO (void);

    /**
     * @brief Accessor. Returns current content of buffer.
     * @return usage of array.
     */

    ccs::types::uint32 GetCount (void) const;

    /**
     * @brief Accessor. Tests if buffer is empty.
     * @return TRUE if empty.
     */

    bool IsEmpty (void) const;

    /**
     * @brief Accessor. Tests if buffer is full.
     * @return TRUE if full.
     */

    bool IsFull (void) const;

    /**
     * @brief Accessor. Returns reference to data block at FIFO input.
     * @return Reference if not full, NULL otherwise.
     * @warning The accessors do not use the class locking mechanism which must be
     * used by the application-specific code, if necessary. See ccs::base::AtomicLock.
     */

    void* GetInDataBlockReference (void) const;
    void ReleaseInDataBlockReference (void);

    /**
     * @brief Accessor. Returns reference to data block at FIFO output.
     * @return Reference if not empty, NULL otherwise.
     * @warning The accessors do not use the class locking mechanism which must be
     * used by the application-specific code, if necessary. See ccs::base::AtomicLock.
     */

    void* GetOutDataBlockReference (void) const;
    void ReleaseOutDataBlockReference (void);

    /**
     * @brief Accessor. Rerieves data at the head of the FIFO.
     * @return TRUE if valid index.
     */

    bool PullDataBlock (void * const data);

    /**
     * @brief Accessor. Inserts data at tail of the FIFO.
     * @return TRUE if valid index.
     */

    bool PushDataBlock (const void * const data);

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _BlockMemoryFIFO_h_


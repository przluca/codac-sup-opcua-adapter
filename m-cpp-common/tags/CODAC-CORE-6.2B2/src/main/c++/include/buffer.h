#ifndef BUFFER_H
#define BUFFER_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/include/buffer.h $
* $Id: buffer.h 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Memory management classes
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*		  CS 90 046
*		  13067 St. Paul-lez-Durance Cedex
*		  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

// Local header files

#include "Buffer.h" // Templated classes
#include "CircularBuffer.h"
#include "FIFOBuffer.h"
#include "HIFOBuffer.h"
#include "LIFOBuffer.h"

#include "BlockMemoryBuffer.h"
#include "BlockMemoryFIFO.h"

// Constants

// Type definition

typedef ccs::base::BlockMemoryBuffer BlockMemory; // Backward compatibility
typedef ccs::base::BlockMemoryFIFO BlockMemory_FIFO; // Backward compatibility

#endif // BUFFER_H


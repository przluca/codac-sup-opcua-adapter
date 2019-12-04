/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/BlockMemoryBuffer.cpp $
* $Id: BlockMemoryBuffer.cpp 100814 2019-07-17 13:23:31Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
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

// Global header files

#include <new> // std::nothrow

#include <string.h> // memset

// Local header files

#include "BasicTypes.h"

#include "BlockMemoryBuffer.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"

// Type definition

// Global variables

// Function declaration

// Function definition

namespace ccs {

namespace base {

void BlockMemoryBuffer::Allocate (void) 
{ 

  ccs::types::uint32 size = _width * _size; 
  _buffer = new (std::nothrow) ccs::types::uint8 [size]; 

  return; 

}

void BlockMemoryBuffer::Reset (void) 
{ 

  ccs::types::uint32 size = _width * _size; 
  (void)memset(_buffer, 0, size); 

  return; 

}

ccs::types::uint32 BlockMemoryBuffer::GetSize (void) const { return _size; }
ccs::types::uint32 BlockMemoryBuffer::GetWidth (void) const { return _width; }

void* BlockMemoryBuffer::GetDataBlockReference (const ccs::types::uint32 index) const 
{ 

  void* ref = NULL_PTR_CAST(void*); 

  if (index < this->GetWidth())
    {
      ref = static_cast<void*>(&(_buffer[index * _size]));
    }

  return ref; 

}

bool BlockMemoryBuffer::GetDataBlock (void * const data, const ccs::types::uint32 index) const 
{ 

  bool status = (index < this->GetWidth()); 

  if (status)
    {
      (void)memcpy(data, this->GetDataBlockReference(index), this->GetSize());
    }

  return status; 

}

bool BlockMemoryBuffer::PutDataBlock (const void * const data, const ccs::types::uint32 index) 
{ 

  bool status = (index < this->GetWidth()); 

  if (status)
    {
      (void)memcpy(this->GetDataBlockReference(index), data, this->GetSize());
    }

  return status; 

}

BlockMemoryBuffer::BlockMemoryBuffer (const ccs::types::uint32 size, const ccs::types::uint32 width) 
{ 

  // Initialise attributes
  _size = size; 
  _width = width; 
  _buffer = NULL_PTR_CAST(ccs::types::uint8*);

  this->Allocate(); 
  this->Reset(); 

  return; 

}

BlockMemoryBuffer::~BlockMemoryBuffer (void) 
{ 

  if (NULL_PTR_CAST(ccs::types::uint8*) != _buffer)
    {
      delete [] _buffer;
    }

  _buffer = NULL_PTR_CAST(ccs::types::uint8*);

  return; 

}

} // namespace base

} // namespace ccs

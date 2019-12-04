/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/BlockMemoryFIFO.cpp $
* $Id: BlockMemoryFIFO.cpp 100814 2019-07-17 13:23:31Z bauvirb $
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

// Local header files

#include "BasicTypes.h"

#include "BlockMemoryBuffer.h"
#include "BlockMemoryFIFO.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"

// Type definition

// Global variables

// Function declaration

// Function definition

namespace ccs {

namespace base {

void BlockMemoryFIFO::Initialise (void) 
{ 

  _count = 0u; 
  _head = 0u; 
  _tail = 0u; 

  return; 

}

ccs::types::uint32 BlockMemoryFIFO::GetCount (void) const { return _count; }

bool BlockMemoryFIFO::IsEmpty (void) const { return (0u == this->GetCount()); }
bool BlockMemoryFIFO::IsFull (void) const { return (this->BlockMemoryBuffer::GetWidth() == this->GetCount()); }

void* BlockMemoryFIFO::GetInDataBlockReference (void) const 
{ 

  void* ref = NULL_PTR_CAST(void*); 

  if (!this->IsFull())
    {
      ref = this->BlockMemoryBuffer::GetDataBlockReference(_tail);
    }
  
  return ref; 

}

void BlockMemoryFIFO::ReleaseInDataBlockReference (void) 
{

  _count++; _tail++;

  if (_tail == this->GetWidth())
    {
      _tail = 0u;
    }

  return; 

}

void* BlockMemoryFIFO::GetOutDataBlockReference (void) const 
{ 

  void* ref = NULL_PTR_CAST(void*); 

  if (!this->IsEmpty())
    {
      ref = this->BlockMemoryBuffer::GetDataBlockReference(_head);
    }

  return ref;

}

void BlockMemoryFIFO::ReleaseOutDataBlockReference (void) 
{ 

  _count--; _head++; 

  if (_head == this->GetWidth())
    {
      _head = 0u;
    }

  return; 

}

bool BlockMemoryFIFO::PullDataBlock (void * const data) 
{ 

  bool status = (!this->IsEmpty()); 

  if (status) 
    { 
      status = this->BlockMemoryBuffer::GetDataBlock(data, _head); 
    } 

  if (status) 
    {
      _count--; _head++; 
      
      if (_head == this->GetWidth()) 
        {
          _head = 0u; 
        }
    }

  return status; 

}

bool BlockMemoryFIFO::PushDataBlock (const void * const data) 
{
  
  bool status = (!this->IsFull()); 

  if (status) 
    { 
      status = this->BlockMemoryBuffer::PutDataBlock(data, _tail); 
    } 

  if (status) 
    {
      _count++; _tail++;
      
      if (_tail == this->GetWidth())
        {
          _tail = 0u; 
        } 
    }

  return status; 

}

BlockMemoryFIFO::BlockMemoryFIFO (const ccs::types::uint32 size, const ccs::types::uint32 width) : BlockMemoryBuffer(size, width) { this->Initialise(); return; }

BlockMemoryFIFO::~BlockMemoryFIFO (void) {}

} // namespace base

} // namespace ccs

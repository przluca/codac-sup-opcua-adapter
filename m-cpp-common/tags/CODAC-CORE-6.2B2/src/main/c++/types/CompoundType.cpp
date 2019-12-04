/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/CompoundType.cpp $
* $Id: CompoundType.cpp 100890 2019-07-19 07:14:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*                                 CS 90 046
*                                 13067 St. Paul-lez-Durance Cedex
*                                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <memory> // std::shared_ptr, etc.

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. helper functions

#include "NetTools.h" 

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE 
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE 
#include "log-api.h" // Syslog wrapper routines

#include "LookUpTable.h"

#include "AnyType.h" // Introspectable type definition (base class) ..
#include "AnyTypeHelper.h" // .. associated helper routines
#include "AnyTypeDatabase.h" // .. associated helper class

#include "ArrayType.h"
#include "CompoundType.h"
#include "ScalarType.h"
//#include "CompoundArray.h"
//#include "ScalarArray.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::types"

// Type definition

namespace ccs {

namespace types {

// Global variables

// Function declaration

// Function definition
  
CompoundType* CompoundType::AddAttribute (const char8 * const name, const char8 * const type)
{ 

  log_trace("CompoundType::AddAttribute('%s') - Entering method", name);

  bool status = ccs::types::GlobalTypeDatabase::IsValid(type);

  if (!status)
    {
      log_warning("CompoundType::AddAttribute('%s') - Unknown '%s' type", name, type);
    }

  std::shared_ptr<const AnyType> __type;

  if (status)
    {
      __type = ccs::types::GlobalTypeDatabase::GetType(type);
      status = static_cast<bool>(__type);
    }

  if (status)
    {
      (void)this->AddAttribute(name, __type);
    }

  log_trace("CompoundType::AddAttribute('%s') - Leaving method", name);

  return this; 

}

CompoundType* CompoundType::AddAttribute (const char8 * const name, const std::shared_ptr<const AnyType>& type)
{ 

  log_trace("CompoundType::AddAttribute('%s') - Entering method", name);

  bool status = (false == this->HasAttribute(name)); // Unknown attribute ..

  if (status)
    {
      status = static_cast<bool>(type); // .. and valid type

      if (!status)
        {
          log_warning("CompoundType::AddAttribute('%s') - Invalid type", name);
        }
    }

  if (status)
    {
      log_debug("CompoundType::AddAttribute('%s') - Add attribute type '%s'", name, type->GetName());
      status = __attributes.Register(name, type);
    }

  if (status)
    {
      // Update attributes offsets
      uint32 size = this->GetSize();
      log_debug("CompoundType::AddAttribute('%s') - Add attribute offset '%u'", name, size);
      status = __offsets.Register(name, size);
    }

  if (status)
    {
      // Update datatype size
      this->SetSize(this->GetSize() + type->GetSize());
      log_debug("CompoundType::AddAttribute('%s') - Update data type size '%u'", name, this->GetSize());
    }

  log_trace("CompoundType::AddAttribute('%s') - Leaving method", name);

  return this; 

}

CompoundType* CompoundType::AddAttribute (const char8 * const name, const AnyType* type) { return this->AddAttribute(name, std::shared_ptr<const AnyType>(type)); }
// v1.3.3 - Smart pointers impose to make a copy so as to avoid deleting the type when this goes out of scope
CompoundType* CompoundType::AddAttribute (const char8 * const name, const ArrayType& type) { return this->AddAttribute(name, std::dynamic_pointer_cast<const AnyType>(std::make_shared<const ArrayType>(type))); }
CompoundType* CompoundType::AddAttribute (const char8 * const name, const CompoundType& type) { return this->AddAttribute(name, std::dynamic_pointer_cast<const AnyType>(std::make_shared<const CompoundType>(type))); }
CompoundType* CompoundType::AddAttribute (const char8 * const name, const ScalarType& type) { return this->AddAttribute(name, std::dynamic_pointer_cast<const AnyType>(std::make_shared<const ScalarType>(type))); }

bool CompoundType::HasAttribute (const uint32 index) const 
{ 

  bool status = (index < this->GetAttributeNumber());

  return status; 

}

bool CompoundType::HasAttribute (const char8 * const name) const 
{

  log_trace("CompoundType::HasAttribute('%s') - Entering method", name);

  bool status = !ccs::HelperTools::IsUndefinedString(name);

  if (status)
    {
      status = __attributes.IsValid(name);
    }

  log_trace("CompoundType::HasAttribute('%s') - Leaving method", name);

  return status; 

}

uint32 CompoundType::GetAttributeIndex (const char8 * const name) const 
{ 

  uint32 index = 0u; 

  bool status = (true == this->HasAttribute(name));

  if (status)
    {
      index = __attributes.GetIndex(name);
    }

  return index; 

}

const char8* CompoundType::GetAttributeName (const uint32 index) const 
{ 

  const char8* ref = NULL_PTR_CAST(const char8*); 

  bool status = (true == this->HasAttribute(index));

  if (status)
    {
      ref = __attributes.GetName(index);
    }

  return ref; 

}

uint32 CompoundType::GetAttributeNumber (void) const
{ 

  return __attributes.GetSize(); 

}

uint32 CompoundType::GetAttributeOffset (const uint32 index) const 
{ 

  uint32 offset = 0u; 

  bool status = (true == this->HasAttribute(index));

  if (status)
    {
      offset = __offsets.GetElement(index);
    }

  return offset; 

}

uint32 CompoundType::GetAttributeOffset (const char8 * const name) const 
{ 

  uint32 offset = 0u; 

  bool status = (true == this->HasAttribute(name));

  if (status)
    {
      offset = __offsets.GetElement(name);
    }

  return offset; 

}

void* CompoundType::GetAttributeReference (const void * const ref, const uint32 index) const 
{ 

  void* attr = NULL_PTR_CAST(void*); 

  if ((NULL_PTR_CAST(void*) != ref) && this->HasAttribute(index)) 
    {
      attr = const_cast<void*>(static_cast<const void*>(static_cast<const uint8_t*>(ref) + this->GetAttributeOffset(index)));
    }

  return attr; 

}

void* CompoundType::GetAttributeReference (const void * const ref, const char8 * const name) const 
{ 

  void* attr = NULL_PTR_CAST(void*); 

  if ((NULL_PTR_CAST(void*) != ref) && this->HasAttribute(name)) 
    {
      attr = const_cast<void*>(static_cast<const void*>(static_cast<const uint8_t*>(ref) + this->GetAttributeOffset(name)));
    }

  return attr; 

}

uint32 CompoundType::GetAttributeSize (const uint32 index) const 
{ 

  uint32 size = 0u; 

  bool status = (true == this->HasAttribute(index));

  if (status) 
    {
      size = this->GetAttributeType(index)->GetSize(); 
    }

  return size; 

}

uint32 CompoundType::GetAttributeSize (const char8 * const name) const 
{ 

  uint32 size = 0u; 

  bool status = (true == this->HasAttribute(name));

  if (status) 
    {
      size = this->GetAttributeType(name)->GetSize(); 
    }

  return size; 

}

std::shared_ptr<const AnyType> CompoundType::GetAttributeType (const uint32 index) const 
{ 

  std::shared_ptr<const AnyType> ref; 

  bool status = (true == this->HasAttribute(index));

  if (status)
    {
      ref = __attributes.GetElement(index);
    }

  return ref; 

}

std::shared_ptr<const AnyType> CompoundType::GetAttributeType (const char8 * const name) const 
{ 

  std::shared_ptr<const AnyType> ref; 

  bool status = (true == this->HasAttribute(name));

  if (status)
    {
      ref = __attributes.GetElement(name);
    }

  return ref; 

}

bool CompoundType::ToNetworkByteOrder (void * const ref) const // In-place conversion
{

  log_trace("CompoundType::ToNetworkByteOrder('%p') - Entering method", ref);

  bool status = (NULL_PTR_CAST(void*) != ref);

  for (uint32 index = 0u; ((index < this->GetAttributeNumber()) && status); index++)
    {
      void* attr = static_cast<void*>(static_cast<uint8_t*>(ref) + this->GetAttributeOffset(index));
      status = this->GetAttributeType(index)->ToNetworkByteOrder(attr);
    }

  log_trace("CompoundType::ToNetworkByteOrder('%p') - Leaving method", ref);

  return status;

}

bool CompoundType::FromNetworkByteOrder (void * const ref) const // In-place conversion
{

  log_trace("CompoundType::FromNetworkByteOrder - Entering method");

  bool status = (NULL_PTR_CAST(void*) != ref);

  for (uint32 index = 0u; ((index < this->GetAttributeNumber()) && status); index++)
    {
      void* attr = static_cast<void*>(static_cast<uint8_t*>(ref) + this->GetAttributeOffset(index));
      status = this->GetAttributeType(index)->FromNetworkByteOrder(attr);
    }

  log_trace("CompoundType::FromNetworkByteOrder - Leaving method");

  return status;

}

bool CompoundType::operator== (const CompoundType& type) const
{

  bool status = ((this->GetSize() == type.GetSize()) &&
                 (this->GetAttributeNumber() == type.GetAttributeNumber()));

  for (uint32 index = 0u; (status && (index < this->GetAttributeNumber())); index++)
    {
      status = (*(this->GetAttributeType(index)) == *(type.GetAttributeType(index)));
    }

  return status;

}

bool CompoundType::operator== (const AnyType& type) const
{

  bool status = ccs::HelperTools::Is<CompoundType>(&type);

  if (status)
    {
      const CompoundType* ref = dynamic_cast<const CompoundType*>(&type);

      status = ((this->GetSize() == ref->GetSize()) &&
                (this->GetAttributeNumber() == ref->GetAttributeNumber()));

      for (uint32 index = 0u; (status && (index < this->GetAttributeNumber())); index++)
        {
          status = (*(this->GetAttributeType(index)) == *(ref->GetAttributeType(index)));
        }
    }

  return status;

}

CompoundType& CompoundType::operator= (const CompoundType& type)
{

  log_trace("CompoundType::operator= - Entering method");

  if (this != &type)
    {
      // WARNING - Do not call base class assignment operator as size is computed for each time
      (void)this->SetName(type.GetName());
      (void)this->SetSize(0u);

      // Ensure attributes are instantiated in the copy operation
      for (uint32 index = 0u; index < type.GetAttributeNumber(); index++)
        {
          (void)this->AddAttribute(type.GetAttributeName(index), type.GetAttributeType(index));
        }
    }

  log_trace("CompoundType::operator= - Leaving method");

  return *this;

}

CompoundType::CompoundType (void) {}
CompoundType::CompoundType (const char8 * const type) : AnyType (type) {}
CompoundType::CompoundType (const CompoundType& type) : AnyType (type.GetName()) // Copy constructor
{ 

  log_trace("CompoundType::CopyConstructor('%s') - Entering method", type.GetName());

  log_debug("CompoundType::CopyConstructor('%s') - Will declare '%u' attributes ..", type.GetName(), type.GetAttributeNumber());

  // Ensure attributes are instantiated in the copy operation
  for (uint32 index = 0u; index < type.GetAttributeNumber(); index++)
    {
      log_debug("CompoundType::CopyConstructor('%s') - .. '%s' with type '%s'", type.GetName(), type.GetAttributeName(index), type.GetAttributeType(index)->GetName());
      (void)this->AddAttribute(type.GetAttributeName(index), type.GetAttributeType(index));
    }

  log_trace("CompoundType::CopyConstructor('%s') - Leaving method", type.GetName());

  return; 

}

CompoundType::~CompoundType (void) {}

} // namespace types

} // namespace ccs

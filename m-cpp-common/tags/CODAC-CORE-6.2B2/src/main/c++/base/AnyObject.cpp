/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/AnyObject.cpp $
* $Id: AnyObject.cpp 100889 2019-07-19 06:46:30Z bauvirb $
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

#include "BasicTypes.h" // Global type definition
#include "SysTools.h" // Misc. helper functions

#include "Hash.h"

#include "AnyObject.h" // This class definition
#include "ObjectFactory.h"

// Constants

// Type definition

namespace ccs {

namespace base {

// Global variables

REGISTER_CLASS_WITH_OBJECT_FACTORY(AnyObject) // Auto-register constructor with the ObjectFactory

// Function declaration

// Function definition

// Initialiser methods

// Accessor methods

const ccs::types::char8* AnyObject::GetInstanceName (void) const { return this->m_instance_name; }
void AnyObject::SetInstanceName (const ccs::types::char8 * const name) { ccs::HelperTools::SafeStringCopy(this->m_instance_name, name, STRING_MAX_LENGTH); return; }

ObjectUId_t AnyObject::GetInstanceUId (void) const { return this->m_instance_uid; }

void AnyObject::SetInstanceUId (ObjectUId_t id) { this->m_instance_uid = id; return; }
void AnyObject::SetInstanceUId (void)
{ 

  ObjectTypeId_t id = static_cast<ObjectUId_t>(ccs::HelperTools::GetCurrentTime());

  this->SetInstanceUId(id); 

  return;

}

ObjectTypeId_t AnyObject::GetInstanceType (void) const { return this->m_instance_type; }
void AnyObject::SetInstanceType (ObjectTypeId_t type) { this->m_instance_type = type; return; }
void AnyObject::SetInstanceType (const ccs::types::char8 * const type) { this->SetInstanceType(GetInstanceTypeByName(type)); return; }

// Miscellaneous methods

bool AnyObject::IsType (void) const { return (this->m_instance_root == GetInstanceTypeByName(OBJTYPE_ROOT)); }
bool AnyObject::IsType (ObjectTypeId_t type) const { return (this->m_instance_type == type); }
bool AnyObject::IsType (const ccs::types::char8 * const type) const { return (this->m_instance_type == GetInstanceTypeByName(type)); }

ObjectTypeId_t GetInstanceTypeByName (const ccs::types::char8 * const type)
{ 

  ObjectTypeId_t id = static_cast<ObjectTypeId_t>(ccs::HelperTools::Hash<uint16_t>(type));

  return id; 

}

AnyObject& AnyObject::operator= (const AnyObject& obj)
{

  if (this != &obj)
    {
      this->SetInstanceName(obj.GetInstanceName());
      this->SetInstanceUId(obj.GetInstanceUId());
      this->m_instance_type = obj.GetInstanceType();
    }

  return *this;

}

// Constructor methods

AnyObject::AnyObject (void) 
{ 

  this->SetInstanceName(STRING_UNDEFINED);
  this->SetInstanceUId();
  this->m_instance_root = GetInstanceTypeByName(OBJTYPE_ROOT);
  this->m_instance_type = GetInstanceTypeByName(OBJTYPE_ROOT);

  return;

}

AnyObject::AnyObject (const AnyObject& obj)
{

  this->SetInstanceName(obj.GetInstanceName());
  this->SetInstanceUId(obj.GetInstanceUId());
  this->m_instance_root = GetInstanceTypeByName(OBJTYPE_ROOT);
  this->m_instance_type = obj.GetInstanceType();

  return;

}

// Destructor method

AnyObject::~AnyObject (void) 
{ 

  // Clear memory footprint
  this->SetInstanceName(STRING_UNDEFINED);
  this->m_instance_root = OBJTYPE_UNDEFINED;
  this->m_instance_type = OBJTYPE_UNDEFINED;

  return;

}

} // namespace base

} // namespace ccs

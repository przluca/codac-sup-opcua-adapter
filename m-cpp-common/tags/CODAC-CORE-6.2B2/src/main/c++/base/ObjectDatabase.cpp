/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/ObjectDatabase.cpp $
* $Id: ObjectDatabase.cpp 101457 2019-08-08 12:51:16Z bauvirb $
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

#include <new> // std::nothrow, etc.

// Local header files

#include "BasicTypes.h" // Global type definition
#include "SysTools.h" // Misc. helper functions, e.g. hash, etc.

#define LOG_TRACE_ENABLE
#undef LOG_TRACE_ENABLE 
#define LOG_DEBUG_ENABLE
#undef LOG_DEBUG_ENABLE 
#include "log-api.h" // Syslog wrapper routines (ccs::log)

#include "AnyObject.h" 
#include "ObjectDatabase.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"

// Type definition

namespace ccs {

namespace base {

// Global variables

namespace GlobalObjectDatabase {

static ObjectDatabase* __p_odb = NULL_PTR_CAST(ObjectDatabase*); // Just instantiate the globally scoped object database

ObjectDatabase* GetInstance (void) 
{ 

  if (__p_odb == NULL_PTR_CAST(ObjectDatabase*))
    {
      __p_odb = new (std::nothrow) ObjectDatabase ();
    }

  return __p_odb; 

}

} // namespace GlobalObjectDatabase

// Function declaration

// Function definition

AnyObject* ObjectDatabase::GetInstance (ccs::types::uint32 id) const
{ 

  bool status = LookUpTable<AnyObject*>::IsValid(id);

  AnyObject* ref = NULL_PTR_CAST(AnyObject*);

  if (status)
    {
      ref = LookUpTable<AnyObject*>::GetElement(id); 
    }

  return ref;

}

AnyObject* ObjectDatabase::GetInstance (const ccs::types::char8 * const name) const
{ 

  bool status = LookUpTable<AnyObject*>::IsValid(name);

  AnyObject* ref = NULL_PTR_CAST(AnyObject*);

  if (status)
    {
      ref = LookUpTable<AnyObject*>::GetElement(name); 
    }

  return ref;

}

// Initializer methods

namespace GlobalObjectDatabase {

bool Register (const ccs::types::char8 * const name, AnyObject* const& ref) 
{ 

  // WARNING - Statically casting references from classes that do not
  //           inherit from AnyObject may have the virtual table associating
  //           AnyObject::IsType to the OtherClass::~OtherClass.

  if ((ref != NULL_PTR_CAST(AnyObject*)) && (ref->IsType() != true))
    {
      log_warning("'%s' instance has unclear parentage", name);
    }

  return GetInstance()->Register(name, ref); 

}

bool Remove (const ccs::types::char8 * const name) { return GetInstance()->Remove(name); }
bool Remove (const ccs::types::uint32 index) { return GetInstance()->Remove(index); }
bool Remove (const AnyObject* ref)
{ 

  bool status = ((ref != NULL_PTR_CAST(AnyObject*)) && (ref->IsType() == true)); 

  if (status) 
    { 
      status = Remove(ref->GetInstanceName()); 
    } 

  return status; 

}

// Accessor methods

bool IsValid (const ccs::types::char8 * const name) { return GetInstance()->IsValid(name); }

ccs::types::uint32 GetSize (void) { return GetInstance()->GetSize(); }

AnyObject* GetInstance (const ccs::types::char8 * const name) { return GetInstance()->GetInstance(name); }
AnyObject* GetInstance (const ccs::types::uint32 index) { return GetInstance()->GetInstance(index); }

} // namespace GlobalObjectDatabase

// Miscellaneous methods

// Constructor methods

// Destructor method

} // namespace base

} // namespace ccs

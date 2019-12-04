/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/ObjectFactory.cpp $
* $Id: ObjectFactory.cpp 100890 2019-07-19 07:14:30Z bauvirb $
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
#include "tools.h" // Misc. helper functions, e.g. hash, etc.
//#include "lu-table.h" // Look-up table class definition, etc.
#include "LookUpTable.h" // Look-up table class definition, etc.

#define LOG_TRACE_ENABLE
#undef LOG_TRACE_ENABLE 
#define LOG_DEBUG_ENABLE
#undef LOG_DEBUG_ENABLE 
#include "log-api.h" // Syslog wrapper routines (ccs::log)

#include "AnyObject.h"
#include "ObjectDatabase.h"
#include "ObjectFactory.h" // This class definition

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"

// Type definition

namespace ccs {

namespace base {

class ObjectFactory
{

  private:

    struct ObjectTypeInfo_t {

      ObjectTypeId_t type;
      AnyObject_Constructor_t constr;
      AnyObject_Destructor_t destr;
    
    };

    LookUpTable<ObjectTypeInfo_t> __type_list;

    // Accessor methods
    AnyObject_Constructor_t GetConstructor (const ccs::types::char8 * const type) const;
    AnyObject_Destructor_t GetDestructor (ObjectTypeId_t type) const;

    ObjectFactory (const ObjectFactory& ofy);
    ObjectFactory& operator= (const ObjectFactory& ofy);

  protected:

  public:

    // Initializer methods
    AnyObject* Instantiate (const ccs::types::char8 * const type) const;
    bool Register (const ccs::types::char8 * const type, AnyObject_Constructor_t constr, AnyObject_Destructor_t destr = 0);
    bool Remove (const ccs::types::char8 * const type) { return __type_list.Remove(type); };
    bool Terminate (AnyObject* ref) const;

    // Accessor methods
    bool IsValid (const ccs::types::char8 * const type) const;

    // Miscellaneous methods

    // Constructor methods
    ObjectFactory (void) {};

    // Destructor method
    virtual ~ObjectFactory (void) {};

};

// Global variables

namespace GlobalObjectFactory {

static ObjectFactory* __p_ofy = NULL_PTR_CAST(ObjectFactory*); // Just instantiate the globally scoped class database

ObjectFactory* GetInstance (void) 
{ 

  if (__p_ofy == NULL_PTR_CAST(ObjectFactory*)) 
    {
      __p_ofy = new (std::nothrow) ObjectFactory (); 
    }

  return __p_ofy; 

}

} // namespace GlobalObjectFactory

// Function declaration

// Function definition

// Initializer methods

AnyObject* ObjectFactory::Instantiate (const ccs::types::char8 * const type) const
{ 

  AnyObject* ref = NULL_PTR_CAST(AnyObject*);
  AnyObject_Constructor_t construct = NULL_PTR_CAST(AnyObject_Constructor_t); 

  bool status = this->IsValid(type);

  if (status)
    {
      construct = this->GetConstructor(type); 
      status = (construct != NULL_PTR_CAST(AnyObject_Constructor_t));
    }

  if (status) 
    {
      log_info("Try and instantiate class '%s (%d)'", type, GetInstanceTypeByName(type));
      ref = (*construct)(); 
    }

  if ((ref != NULL_PTR_CAST(AnyObject*)) && (GetInstanceTypeByName(type) != ref->GetInstanceType()))
    {
      log_warning("There might be difficulties to terminate instance of class '%s (%d %d)'", type, GetInstanceTypeByName(type), ref->GetInstanceType());
    }

  if ((ref != NULL_PTR_CAST(AnyObject*)) && (ref->IsType() == true)) 
    {
      ref->SetInstanceType(type); 
    }

  return ref; 

}

bool ObjectFactory::Register (const ccs::types::char8 * const type, AnyObject_Constructor_t constr, AnyObject_Destructor_t destr)
{ 

  bool status = (constr != NULL_PTR_CAST(AnyObject_Constructor_t)); // Optional destructor

  if (status)
    {
      status = (false == this->IsValid(type));
    }

  if (status)
    {
      ObjectTypeInfo_t info; 

      info.type = GetInstanceTypeByName(type); 
      info.constr = constr; 
      info.destr = destr; 

      status = __type_list.Register(type, info);
    }

  return status; 

}

bool ObjectFactory::Terminate (AnyObject* ref) const
{ 

  log_info("Try and terminate instance '%s (%d)'", ref->GetInstanceName(), ref->GetInstanceType());

  AnyObject_Destructor_t destruct = this->GetDestructor(ref->GetInstanceType()); 

  bool status = (destruct != NULL_PTR_CAST(AnyObject_Destructor_t));

  if (status) 
    {
      (*destruct)(ref); 
    }
  else // Optional destructor
    {
      delete ref;
    }

  return status; 

}

namespace GlobalObjectFactory {

bool IsValid (const ccs::types::char8 * const type) { return GetInstance()->IsValid(type); }
bool Register (const ccs::types::char8 * const type, AnyObject_Constructor_t construct, AnyObject_Destructor_t destruct) { return GetInstance()->Register(type, construct, destruct); }

AnyObject* Instantiate (const ccs::types::char8 * const type)
{ 

  log_info("Try and instantiate '%s' class", type); 

  AnyObject* ref = GetInstance()->Instantiate(type); 

  return ref; 

}
// WARNING - The constructor being called by the ObjectFactory must specifically cast to the abstract base class else this intantiate and auto-register in the ObjectDatabase may return mis-aligned references
AnyObject* Instantiate (const ccs::types::char8 * const type, const ccs::types::char8 * const name)
{ 

  AnyObject* ref = Instantiate(type);
  GlobalObjectDatabase::Register(name, ref); 

  return ref; 

}

bool Terminate (AnyObject* ref)
{ 

  bool status = ((ref != NULL_PTR_CAST(AnyObject*)) && (ref->IsType() == true)); 

  if (status) 
    { 
      status = GetInstance()->Terminate(ref); 
    } 

  return status; 

}

} // namespace GlobalObjectFactory

// Accessor methods

bool ObjectFactory::IsValid (const ccs::types::char8 * const type) const { return __type_list.IsValid(type); }

AnyObject_Constructor_t ObjectFactory::GetConstructor (const ccs::types::char8 * const type) const
{ 

  AnyObject_Constructor_t construct = NULL_PTR_CAST(AnyObject_Constructor_t);

  bool status = this->IsValid(type);

  if (status)
    {
      ObjectTypeInfo_t info; 
      info.constr = NULL_PTR_CAST(AnyObject_Constructor_t); 

      status = __type_list.GetElement(type, info);

      if (status)
        {
          construct = info.constr;
        }
    } 

  return construct; 

}

AnyObject_Destructor_t ObjectFactory::GetDestructor (ObjectTypeId_t type) const
{ 

  AnyObject_Destructor_t destruct = NULL_PTR_CAST(AnyObject_Destructor_t);

  for (ccs::types::uint32 index = 0u; index < __type_list.GetSize(); index += 1u)
    {
      ObjectTypeInfo_t info;
      info.destr = NULL_PTR_CAST(AnyObject_Destructor_t); 

      (void)__type_list.GetElement(index, info);

      if (info.type == type) 
        {
          destruct = info.destr;
          break;
        }
    }

  return destruct; 

}

// Miscellaneous methods

// Constructor methods

// Destructor method

} // namespace base

} // namespace ccs

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/core/ObjectHandler.cpp $
* $Id: ObjectHandler.cpp 100486 2019-07-05 09:14:14Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <new> // std::nothrow

#include <BasicTypes.h> // Misc. type definition
#include <SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.

#include <AnyObject.h>
#include <ObjectDatabase.h>
#include <ObjectFactory.h>

#include <log-api.h> // Logging helper functions

// Local header files

#include "ObjectHandler.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

// Type definition

namespace sup {

namespace core {

// Global variables

// Function declaration

// Function definition
  
bool ObjectHandler::LoadLibrary (const char * const name) const
{

  bool status = ::ccs::HelperTools::LoadSharedLibrary(name);

  return status;

}

bool ObjectHandler::IsTypeValid (const char * const name) const
{

  bool status = ::ccs::base::GlobalObjectFactory::IsValid(name);

  return status;

}

bool ObjectHandler::Instantiate (const char * const type, const char * const name)
{

  bool status = (IsTypeValid(type) && 
		 !::ccs::HelperTools::IsUndefinedString(name) && 
		 !::ccs::base::ObjectDatabase::IsValid(name));

  ccs::base::AnyObject* ref = static_cast<ccs::base::AnyObject*>(NULL);

  if (status)
    {
      ref = ::ccs::base::GlobalObjectFactory::Instantiate(type);
      status = (static_cast<ccs::base::AnyObject*>(NULL) != ref);

      if (status && ref->IsType()) // Check we have truly an instance of ccs::base::AnyObject ..
	{                          // .. before using its interface
	  ref->SetInstanceName(name);
	}
    }

  if (status)
    {
      status = (::ccs::base::GlobalObjectDatabase::Register(name, ref) && ::ccs::base::ObjectDatabase::Register(name, ref));
    }

  return status;

}

bool ObjectHandler::Terminate (void)
{

  log_trace("ObjectHandler::Terminate - Entering method");

  bool status = true;

  ccs::base::AnyObject* ref = static_cast<ccs::base::AnyObject*>(NULL);

  log_debug("ObjectHandler::Terminate - Looping through LODB of size '%u' ..", ::ccs::base::ObjectDatabase::GetSize());

  for (ccs::types::uint32 index = 0u; index < ::ccs::base::ObjectDatabase::GetSize(); index += 1u)
    {
      log_debug("ObjectHandler::Terminate - .. '%u' ..", index);

      ref = ::ccs::base::ObjectDatabase::GetInstance(index);
      status = (static_cast<ccs::base::AnyObject*>(NULL) != ref);

      if (status)
	{
	  log_debug("ObjectHandler::Terminate - .. GODB::Remove('%s') ..", ref->GetInstanceName());
	  status = ::ccs::base::GlobalObjectDatabase::Remove(ref->GetInstanceName());
	}

      if (status)
	{
	  log_debug("ObjectHandler::Terminate - .. GOFY::Terminate('%s') ..", ref->GetInstanceName());
	  (void)::ccs::base::GlobalObjectFactory::Terminate(ref);
	}
    }

  // Empty LookUpTable
  (void)::ccs::base::ObjectDatabase::Remove();

  log_trace("ObjectHandler::Terminate - Leaving method");

  return status;

}

bool ObjectHandler::Terminate (const char * const name)
{

  bool status = ::ccs::base::ObjectDatabase::IsValid(name);

  ccs::base::AnyObject* ref = static_cast<ccs::base::AnyObject*>(NULL);

  if (status)
    {
      ref = ::ccs::base::ObjectDatabase::GetInstance(name);
      status = (static_cast<ccs::base::AnyObject*>(NULL) != ref);
    }

  if (status)
    {
      status = (::ccs::base::GlobalObjectDatabase::Remove(name) && 
		::ccs::base::ObjectDatabase::Remove(name));
    }

  if (status)
    {
      (void)::ccs::base::GlobalObjectFactory::Terminate(ref);
    }

  return status;

}

bool ObjectHandler::SetParameter (const char * const name, const char * const param, const char * const value) const
{

  bool status = IsInstanceOf<::ccs::base::CfgableObject>(name);

  ccs::base::CfgableObject* ref = static_cast<ccs::base::CfgableObject*>(NULL);

  if (status)
    {
      ref = GetInstance<::ccs::base::CfgableObject>(name);
      status = (static_cast<ccs::base::CfgableObject*>(NULL) != ref);
    }

  if (status)
    {
      status = ref->SetParameter(param, value);
    }

  return status;

}

bool ObjectHandler::ProcessMessage (const char * const name, const char * const msg) const
{

  bool status = IsInstanceOf<::ccs::base::MsgableObject>(name);

  ccs::base::MsgableObject* ref = static_cast<ccs::base::MsgableObject*>(NULL);

  if (status)
    {
      ref = GetInstance<::ccs::base::MsgableObject>(name);
      status = (static_cast<ccs::base::MsgableObject*>(NULL) != ref);
    }

  if (status)
    {
      status = ref->ProcessMessage(msg);
    }

  return status;

}

ObjectHandler::ObjectHandler (void) {}

ObjectHandler::~ObjectHandler (void) { Terminate(); return; }

} // namespace core

} // namespace sup

extern "C" { // Python wrappings

// ObjectHandler Methods
sup::core::ObjectHandler* new_ObjectFactoryEngine (void)
{
  log_info("Create ObjectFactoryEngine");
  return new (std::nothrow) sup::core::ObjectHandler (); 
}

void delete_ObjectFactoryEngine (sup::core::ObjectHandler* engine)
{
  log_info("Delete ObjectFactoryEngine"); 
  if (static_cast<sup::core::ObjectHandler*>(NULL) != engine) delete engine;
  return;
}

bool ObjectFactoryEngine_Load (sup::core::ObjectHandler* engine, const char * const library)
{

  bool status = (static_cast<sup::core::ObjectHandler*>(NULL) != engine);

  if (status)
    {
      status = engine->LoadLibrary(library);
    }

  return status;

}

ccs::base::AnyObject* ObjectFactoryEngine_Instantiate (sup::core::ObjectHandler* engine, const char * const type, const char * const name)
{
 
  bool status = (static_cast<sup::core::ObjectHandler*>(NULL) != engine);

  ccs::base::AnyObject* ref = static_cast<ccs::base::AnyObject*>(NULL);

  if (status)
    {
      status = engine->Instantiate(type, name);
    }

  if (status)
    {
      ref = engine->GetInstance<ccs::base::AnyObject>(name);
    }

  return ref;

}

bool ObjectFactoryEngine_Terminate_All (sup::core::ObjectHandler* engine)
{

  bool status = (static_cast<sup::core::ObjectHandler*>(NULL) != engine);

  if (status)
    {
      status = engine->Terminate();
    }

  return status;

}

bool ObjectFactoryEngine_Terminate_Ref (sup::core::ObjectHandler* engine, ccs::base::AnyObject* ref)
{

  bool status = ((static_cast<sup::core::ObjectHandler*>(NULL) != engine) && (static_cast<ccs::base::AnyObject*>(NULL) != ref));

  if (status)
    {
      status = engine->Terminate(ref->GetInstanceName());
    }

  return status;

}

// AnyObject methods
bool AnyObject_SetParameter (ccs::base::AnyObject* instance, const char * const name, const char * const value)
{

  bool status = (static_cast<ccs::base::AnyObject*>(NULL) != instance);

  if (status)
    {
      status = (static_cast<ccs::base::CfgableObject*>(NULL) != dynamic_cast<ccs::base::CfgableObject*>(instance));
    }

  if (status)
    {
      status = dynamic_cast<ccs::base::CfgableObject*>(instance)->SetParameter(name, value);
    }

  return status;

}
    
bool AnyObject_ProcessMessage (ccs::base::AnyObject* instance, const char * const msg) 
{

  bool status = (static_cast<ccs::base::AnyObject*>(NULL) != instance);

  if (status)
    {
      status = (static_cast<ccs::base::MsgableObject*>(NULL) != dynamic_cast<ccs::base::MsgableObject*>(instance));
    }

  if (status)
    {
      status = dynamic_cast<ccs::base::MsgableObject*>(instance)->ProcessMessage(msg);
    }

  return status;

}

} // extern C

#undef LOG_ALTERN_SRC

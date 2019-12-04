/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyValue.cpp $
* $Id: AnyValue.cpp 100890 2019-07-19 07:14:30Z bauvirb $
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

#include "types.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#include "NetTools.h" 

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE 
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE 
#include "log-api.h" // Syslog wrapper routines

#include "AnyType.h" // Introspectable type definition class ..
#include "AnyTypeHelper.h" // .. helper routines

#include "AnyValue.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::types"

// Type definition

namespace ccs {

namespace types {

// Global variables

// Function declaration

// Function definition
  
void* AnyValue::CreateInstance (void) 
{ 

  log_trace("AnyValue::CreateInstance - Entering method");

  bool status = ((this->GetSize() > 0u) && (NULL_PTR_CAST(uint8*) == __instance));

  if (status) 
    {
      log_debug("AnyValue::CreateInstance - Create instance of size '%u'", this->GetSize());
      __instance = new (std::nothrow) uint8 [this->GetSize()];
      status = (NULL_PTR_CAST(uint8*) != __instance); 
    }

  if (status) 
    {
      __allocated = true;
      (void)memset(__instance, 0, this->GetSize());
    }

  log_trace("AnyValue::CreateInstance - Leaving method");

  return static_cast<void*>(__instance); 

}

void AnyValue::DeleteInstance (void) 
{ 

  log_trace("AnyValue::DeleteInstance - Entering method");

  if (__allocated && (NULL_PTR_CAST(uint8*) != __instance))
    {
      log_debug("AnyValue::DeleteInstance - Delete instance of size '%u'", this->GetSize());
      delete [] __instance;
    }

  __instance = NULL_PTR_CAST(uint8*); 

  log_debug("AnyValue::DeleteInstance - Leaving method");

  return; 

}

uint32 AnyValue::GetSize (void) const 
{ 

  bool status = (true == static_cast<bool>(__type)); 

  uint32 size = 0u; 

  if (status) 
    {
      size = __type->GetSize(); 
    }

  return size; 

}

std::shared_ptr<const AnyType> AnyValue::GetType (void) const { return __type; }

void* AnyValue::GetInstance (void) const { return static_cast<void*>(__instance); }

bool AnyValue::ToNetworkByteOrder (void)
{ 

  bool status = (true == static_cast<bool>(__type));

  if (status)
    {
      status = (__order == ccs::HelperTools::GetNativeByteOrder());
    }

  if (status)
    {
      status = this->GetType()->ToNetworkByteOrder(this->GetInstance());
    }

  if (status)
    {
      __order = ccs::types::NetworkByteOrder;
    }

  return status; 

}

bool AnyValue::FromNetworkByteOrder (void)
{ 

  bool status = (true == static_cast<bool>(__type));

  if (status)
    {
      status = (__order == ccs::types::NetworkByteOrder); 
    }

  if (status)
    {
      status = this->GetType()->FromNetworkByteOrder(this->GetInstance()); 
    }

  if (status)
    {
      __order = ccs::HelperTools::GetNativeByteOrder();
    }

  return status; 

}

bool AnyValue::ParseInstance (const char8 * const buffer) 
{ 

  bool status = (true == static_cast<bool>(__type));

  if (status)
    {
      status = ccs::HelperTools::ParseInstance(this->GetType(), this->GetInstance(), buffer); 
    }

  return status;

}

bool AnyValue::SerialiseInstance (char8 * const buffer, uint32 size) const 
{ 

  bool status = (true == static_cast<bool>(__type));

  if (status)
    {
      status = (0u < ccs::HelperTools::SerialiseInstance(this->GetType(), this->GetInstance(), buffer, size)); 
    }

  return status;

}

bool AnyValue::SerialiseType (char8 * const buffer, uint32 size) const 
{ 

  bool status = (true == static_cast<bool>(__type));

  if (status)
    {
      status = (0u < ccs::HelperTools::Serialise(this->GetType(), buffer, size)); 
    }

  return status;

}

AnyValue& AnyValue::operator= (const AnyValue& value) // Copy assignment operator
{ 

  log_trace("AnyValue:::operator= - Entering method");

  if (this != &value)
    {
      bool status = (true == static_cast<bool>(__type));

      if (!status) // Uninitialised instance
        {
          // Initialise attributes
          __type = value.GetType(); 
          
          // Create value buffer
          status = (NULL_PTR_CAST(void*) != this->CreateInstance());
        }
      else
        {
          status = (NULL_PTR_CAST(void*) != this->GetInstance());
        }

      if (status)
        {
          status = (NULL_PTR_CAST(void*) != value.GetInstance());
        }

      if (status)
        {
          status = (this->GetSize() == value.GetSize());
        }

      if (status)
        {
          (void)memcpy(this->GetInstance(), value.GetInstance(), this->GetSize());
        }
    }

  log_trace("AnyValue:::operator= - Leaving method");

  return *this;

}

AnyValue::AnyValue (void) 
{ 

  // Initialise attributes
  __type = std::shared_ptr<AnyType>(); 
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  return; 

}

AnyValue::AnyValue (const char8 * const type) 
{ 

  // Initialise attributes
  __type = std::shared_ptr<AnyType>(); 
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  std::shared_ptr<AnyType> tmp;
  bool status = (0u < ccs::HelperTools::Parse(tmp, type));

  if (status)
    {
      __type = tmp;
    }

  // Create value buffer
  (void)this->CreateInstance(); 

  return; 

}

AnyValue::AnyValue (const std::shared_ptr<const AnyType>& type) 
{ 

  // Initialise attributes
  __type = type; 
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  // Create value buffer
  (void)this->CreateInstance(); 

  return; 

}

AnyValue::AnyValue (const AnyType * const type) 
{

  // Initialise attributes
  __type = std::shared_ptr<const AnyType>(type); 
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  // Create value buffer
  (void)this->CreateInstance(); 

  return; 

}

AnyValue::AnyValue (const ArrayType& type) 
{ 

  // Initialise attributes
  __type = std::dynamic_pointer_cast<const AnyType>(std::make_shared<const ArrayType>(type)); // shared_ptr from using copy constructor
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  // Create value buffer
  (void)this->CreateInstance(); 

  return; 

}

AnyValue::AnyValue (const CompoundType& type) 
{ 

  // Initialise attributes
  __type = std::dynamic_pointer_cast<const AnyType>(std::make_shared<const CompoundType>(type)); // shared_ptr from using copy constructor
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  // Create value buffer
  (void)this->CreateInstance(); 

  return; 

}

AnyValue::AnyValue (const ScalarType& type) 
{ 

  // Initialise attributes
  __type = std::dynamic_pointer_cast<const AnyType>(std::make_shared<const ScalarType>(type)); // shared_ptr from using copy constructor
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  // Create value buffer
  (void)this->CreateInstance(); 

  return; 

}

AnyValue::AnyValue (const std::shared_ptr<const AnyType>& type, void * const instance)
{ 

  // Initialise attributes
  __type = type;
  __instance = static_cast<uint8*>(instance); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  return; 

}

AnyValue::AnyValue (const AnyType * const type, void * const instance)
{ 

  // Initialise attributes
  __type = std::shared_ptr<const AnyType>(type); 
  __instance = static_cast<uint8*>(instance); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  return; 

}

AnyValue::AnyValue (const ArrayType& type, void * const instance) 
{ 

  // Initialise attributes
  __type = std::dynamic_pointer_cast<const AnyType>(std::make_shared<const ArrayType>(type)); // shared_ptr from using copy constructor
  __instance = static_cast<uint8*>(instance); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  return; 

}

AnyValue::AnyValue (const CompoundType& type, void * const instance) 
{ 

  // Initialise attributes
  __type = std::dynamic_pointer_cast<const AnyType>(std::make_shared<const CompoundType>(type)); // shared_ptr from using copy constructor
  __instance = static_cast<uint8*>(instance); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  return; 

}

AnyValue::AnyValue (const ScalarType& type, void * const instance) 
{ 

  // Initialise attributes
  __type = std::dynamic_pointer_cast<const AnyType>(std::make_shared<const ScalarType>(type)); // shared_ptr from using copy constructor
  __instance = static_cast<uint8*>(instance); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  return; 

}

AnyValue::AnyValue (const AnyValue& value) // Copy constructor
{ 

  // Initialise attributes
  __type = value.GetType(); 
  __instance = NULL_PTR_CAST(uint8*); 
  __allocated = false;
  __order = ccs::HelperTools::GetNativeByteOrder();

  // Create value buffer
  (void)this->CreateInstance(); 

  // Copy value
  (void)memcpy(this->GetInstance(), value.GetInstance(), this->GetSize());

  return; 

}

AnyValue::~AnyValue (void)
{ 

  (void)this->DeleteInstance(); 

  return; 

}

} // namespace types

} // namespace ccs

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/AnyTypeDatabase.cpp $
* $Id: AnyTypeDatabase.cpp 100890 2019-07-19 07:14:30Z bauvirb $
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

#include <map> // std::map
#include <new> // std::nothrow
#include <utility> // std::pair

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "LookUpTable.h"

#include "AnyType.h" // Introspectable type definition (base class) ..
#include "AnyTypeHelper.h" // .. associated helper routines

#include "CompoundType.h"
#include "ScalarType.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::types"

// Type definition

namespace ccs {

namespace types {

// Global variables

namespace GlobalTypeDatabase {

AnyTypeDatabase* __p_tdb; // Just instantiate the globally scoped type database

// Function declaration

// Function definition
  
bool IsValid (const char8 * const name) { return GetInstance()->IsValid(name); }

bool Register (const std::shared_ptr<const AnyType>& type)
{

  bool status = static_cast<bool>(type);

  if (status)
    {
      status = GetInstance()->Register(type->GetName(), type);
    }
  
  return status;

}

bool Register (const AnyType * const type) { return Register(std::shared_ptr<const AnyType>(type)); }

bool Register (const char8 * const buffer) 
{ 

  std::shared_ptr<AnyType> type;

  bool status = (0u < ::ccs::HelperTools::Parse(type, buffer));

  if (status)
    {
      status = Register(type);
    }

  return status;

}

const std::shared_ptr<const AnyType> GetType (const char8 * const name)
{ 

  std::shared_ptr<const AnyType> type;

  bool status = (true == IsValid(name));

  if (status)
    {
      type = GetInstance()->GetElement(name);
    }

  return type;

}

bool Remove (const char8 * const name) { return GetInstance()->Remove(name); }

} // namespace GlobalTypeDatabase

} // namespace types

} // namespace ccs

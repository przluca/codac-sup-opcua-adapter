/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/BasicTypes.cpp $
* $Id: BasicTypes.cpp 101291 2019-08-02 07:00:16Z bauvirb $
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

// Local header files

#include "LookUpTable.h"

#include "BasicTypes.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::types"

// Type definition

namespace ccs {

namespace types {

typedef struct {

  Identifier id; // Type identifier enumeration
  uint32 size; // Type memory footprint

} TypeDef;

// Global variables

static ccs::base::NameValuePair<TypeDef> __types [] = 
{

  { "bool", { BOOLEAN, sizeof(boolean) } },
  { "char", { CHAR8, sizeof(char8) } }, // Bug 12047 - XML configuration files
  { "char8", { CHAR8, sizeof(char8) } },
  { "string", { STRING, STRING_MAX_LENGTH * sizeof(char8) } }, // WARNING
  { "int8",    { SINT8, sizeof(int8) } },
  { "int8_t",  { SINT8, sizeof(int8) } }, 
  { "uint8",   { UINT8, sizeof(uint8) } },
  { "uint8_t", { UINT8, sizeof(uint8) } },
  { "int16",    { SINT16, sizeof(int16) } }, 
  { "int16_t",  { SINT16, sizeof(int16) } }, 
  { "uint16",   { UINT16, sizeof(uint16) } }, 
  { "uint16_t", { UINT16, sizeof(uint16) } }, 
  { "int32",    { SINT32, sizeof(int32) } }, 
  { "int32_t",  { SINT32, sizeof(int32) } },
  { "int",      { SINT32, sizeof(int32) } }, 
  { "int_t",    { SINT32, sizeof(int32) } },
  { "uint32",   { UINT32, sizeof(uint32) } }, 
  { "uint32_t", { UINT32, sizeof(uint32) } },
  { "uint",     { UINT32, sizeof(uint32) } }, 
  { "uint_t",   { UINT32, sizeof(uint32) } },
  { "int64",    { SINT64, sizeof(int64) } },
  { "int64_t",  { SINT64, sizeof(int64) } },
  { "uint64",   { UINT64, sizeof(uint64) } },
  { "uint64_t", { UINT64, sizeof(uint64) } },
  { "float",   { FLOAT32, sizeof(float32) } },
  { "float32", { FLOAT32, sizeof(float32) } }, // MARTe2 Integration
  { "double",  { FLOAT64, sizeof(float64) } },
  { "float64", { FLOAT64, sizeof(float64) } }, // MARTe2 Integration

  { EOT_KEYWORD, { UNDEF, 0u } }

};

static ccs::base::LookUpTable<TypeDef> __type_list (__types);

// Function declaration

// Function definition
  
const char8 * GetName (const Identifier type) 
{ 

  const char8 * name = NULL_PTR_CAST(const char8 *);

  for (uint32 index = 0u; index < __type_list.GetSize(); index += 1u) 
    { 
      TypeDef info; 

      (void)__type_list.GetElement(index, info); 

      if (info.id == type)
        {
          name = __type_list.GetName(index); 
          break; 
        }
    } 

  return name;

}

// cppcheck-suppress unusedFunction // Used by logging library for configuration structure
Identifier GetIdentifier (const char8 * const type) 
{

  TypeDef info; info.id = UNDEF;

  (void)__type_list.GetElement(type, info); 

  return info.id; 

}

uint32 GetSize (const char8 * const type)
{ 

  TypeDef info; info.size = 0u; 

  (void)__type_list.GetElement(type, info); 

  return info.size; 

}

uint32 GetSize (const Identifier type) { return GetSize(GetName(type)); }

} // namespace types

} // namespace ccs

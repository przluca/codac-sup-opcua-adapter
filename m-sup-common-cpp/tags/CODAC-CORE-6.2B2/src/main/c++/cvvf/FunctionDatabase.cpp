/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/cvvf/FunctionDatabase.cpp $
* $Id: FunctionDatabase.cpp 99275 2019-04-25 06:34:12Z bauvirb $
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

#include <map> // std::map
#include <new> // std::nothrow
#include <utility> // std::pair

// Local header files

#include <SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.
#include <BasicTypes.h> // Misc. type definition
#include <log-api.h> // Logging helper functions
#include <AnyTypeDatabase.h>

#include "FunctionDatabase.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::types"

// Type definition

namespace ccs {

namespace types {

class FunctionDatabase
{

  private:

    std::map<std::string, FunctionDefinition_t> __lutable;

  protected:

  public:

    FunctionDatabase (void) {};

    virtual ~FunctionDatabase (void) {};

    uint32 GetSize (void) const { return __lutable.size(); };

    bool IsValid (const char* name) const { return (__lutable.end() != __lutable.find(std::string(name))); };

    const FunctionDefinition_t GetFunction (const char* name) const;

    bool Register (const FunctionDefinition_t& func);
    bool Remove (const char* name);

};

// Global variables

namespace GlobalFunctionDatabase {

static FunctionDatabase* __p_fdb = static_cast<FunctionDatabase*>(NULL); // Just instantiate the globally scoped function database

// Function declaration

// Function definition
  
static inline FunctionDatabase* GetInstance (void) 
{ 

  if (__builtin_expect((static_cast<FunctionDatabase*>(NULL) == __p_fdb), 0)) // Unlikely
    {
      __p_fdb = new (std::nothrow) FunctionDatabase ();
    }

  return __p_fdb; 

}

bool IsValid (const char* name) { return GetInstance()->IsValid(name); }
bool Register (const ccs::types::FunctionDefinition_t& func) { return GetInstance()->Register(func); }
const FunctionDefinition_t GetFunction (const char* name) { return GetInstance()->GetFunction(name); }

} // namespace GlobalFunctionDatabase

const FunctionDefinition_t FunctionDatabase::GetFunction (const char* name) const
{

  FunctionDefinition_t func;

  bool status = IsValid(name);

  if (status)
    {
      func = __lutable.find(std::string(name))->second;
    }

  return func;

}

bool FunctionDatabase::Register (const FunctionDefinition_t& func)
{

  bool status = !IsValid(func.name);

  if (status)
    {
      status = __lutable.insert(std::pair<std::string,const FunctionDefinition_t>(std::string(func.name),func)).second;
    }

  return status;

}

bool FunctionDatabase::Remove (const char* name)
{

  bool status = IsValid(name);

  if (status)
    {
      status = (1 == __lutable.erase(std::string(name)));
    }

  return status;

}

} // namespace types

} // namespace ccs

#undef LOG_ALTERN_SRC
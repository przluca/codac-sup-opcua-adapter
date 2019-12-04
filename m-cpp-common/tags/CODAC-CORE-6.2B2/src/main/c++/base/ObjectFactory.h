/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/ObjectFactory.h $
* $Id: ObjectFactory.h 100890 2019-07-19 07:14:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Object root class definition
*
* Author        : Bertrand Bauvir (IO)
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

/**
 * @file ObjectFactory.h
 * @brief Header file for ObjectFactory class
 * @date 25/04/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the ObjectFactory class and
 * GlobalObjectFactory methods.
 */

#ifndef _ObjectFactory_h_
#define _ObjectFactory_h_

// Global header files

// Local header files

#include "BasicTypes.h"

#include "AnyObject.h" // Root object class definition

// Constants

// WARNING - ISO C++ does not specify the evaluation order of # or ##
#define JOIN(X,Y) X##Y
#define STRINGIFY(X) #X

// WARNING - The constructor being called by the ObjectFactory must specifically cast to the abstract base class
// WARNING - Dynamic cast due to virtual inheritance to be able to assemble Cfgable and Msgable classes 
#define REGISTER_CLASS_WITH_OBJECT_FACTORY(ClassName) \
ccs::base::AnyObject* JOIN(ClassName,_Constructor) (void) { \
\
  ClassName* ref = new (std::nothrow) ClassName (); \
\
  return dynamic_cast<ccs::base::AnyObject*>(ref); \
\
} \
void JOIN(ClassName,_Destructor) (ccs::base::AnyObject* ref) { \
\
  if (ref != NULL_PTR_CAST(ccs::base::AnyObject*)) \
    { \
      delete dynamic_cast<ClassName*>(ref); \
    } \
\
} \
bool JOIN(ClassName,_IsRegistered) = \
  ccs::base::GlobalObjectFactory::Register(STRINGIFY(ClassName),        \
                                           static_cast<ccs::base::AnyObject_Constructor_t>( & JOIN(ClassName,_Constructor)), \
                                           static_cast<ccs::base::AnyObject_Destructor_t>( & JOIN(ClassName,_Destructor))); \

// Type definition

namespace ccs {

namespace base {

class ObjectFactory; // Forward class declaration

// Global variables

// Function declaration

namespace GlobalObjectFactory {

bool IsValid (const ccs::types::char8 * const type); // Test if valid type name
AnyObject* Instantiate (const ccs::types::char8 * const type); // Instantiate object with class name
AnyObject* Instantiate (const ccs::types::char8 * const type, const ccs::types::char8 * const name); // Instantiate object with class name and register in the object database
bool Register (const ccs::types::char8 * const type, AnyObject_Constructor_t construct, AnyObject_Destructor_t destruct = 0); // Register constructor with class name
bool Terminate (AnyObject* ref); // Destroy object by reference

} // namespace GlobalObjectFactory

} // namespace base

} // namespace ccs

#endif // _ObjectFactory_h_



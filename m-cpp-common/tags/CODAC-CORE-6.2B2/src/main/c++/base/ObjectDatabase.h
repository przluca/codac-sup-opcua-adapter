/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/ObjectDatabase.h $
* $Id: ObjectDatabase.h 100890 2019-07-19 07:14:30Z bauvirb $
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
 * @file ObjectDatabase.h
 * @brief Header file for ObjectDatabase class
 * @date 25/04/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the ObjectDatabase class and
 * GlobalObjectDatabase methods.
 */

#ifndef _ObjectDatabase_h_
#define _ObjectDatabase_h_

// Global header files

// Local header files

#include "BasicTypes.h"
#include "LookUpTable.h" // LookUpTable<Type> class definition
#include "AnyObject.h" // Root object class definition

// Constants

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides AnyObject storage and access by instance name.
 */

class ObjectDatabase : public LookUpTable<AnyObject*>
{

  private:

    ObjectDatabase (const ObjectDatabase& odb);
    ObjectDatabase& operator= (const ObjectDatabase& odb);

  protected:

  public:

    // Accessor methods
    ccs::types::uint32 GetInstanceId (const ccs::types::char8 * const name) const { return LookUpTable<AnyObject*>::GetIndex(name); };

    AnyObject* GetInstance (ccs::types::uint32 id) const;
    AnyObject* GetInstance (const ccs::types::char8 * const name) const;

    // Constructor methods
    ObjectDatabase (void) {};

    // Destructor method
    virtual ~ObjectDatabase (void) {}; // Nothing further than base class destructor 

};

// Global variables

// Function declaration

namespace GlobalObjectDatabase {

bool IsValid (const ccs::types::char8 * const name);
bool Register (const ccs::types::char8 * const name, AnyObject* const& reference);
//bool Register (AnyObject* reference); // The object will be given a unique id/name
bool Remove (const ccs::types::char8 * const name);
bool Remove (const ccs::types::uint32 index);
bool Remove (const AnyObject* reference);

ccs::types::uint32 GetSize (void);

AnyObject* GetInstance (const ccs::types::char8 * const name);
AnyObject* GetInstance (const ccs::types::uint32 index);

} // namespace GlobalObjectDatabase

} // namespace base

} // namespace ccs

#endif // _ObjectDatabase_h_



/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/AnyObject.h $
* $Id: AnyObject.h 100889 2019-07-19 06:46:30Z bauvirb $
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
 * @file AnyObject.h
 * @brief Header file for AnyObject class
 * @date 25/04/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyObject class.
 */

#ifndef _AnyObject_h_
#define _AnyObject_h_

// Global header files

// Local header files

#include "BasicTypes.h"

// Constants

#define OBJTYPE_UNDEFINED static_cast<ObjectTypeId_t>(0)
#define OBJTYPE_ROOT      "AnyObject"

// Type definition

namespace ccs {

namespace base {

typedef ::ccs::types::uint32 ObjectTypeId_t;
typedef ::ccs::types::uint64 ObjectUId_t;

/**
 * @brief The class provides root capabilities for objects managed through factories.
 */

class AnyObject
{

  private:

    // Attributes
    ::ccs::types::string m_instance_name;
    ObjectUId_t m_instance_uid;

    ObjectTypeId_t m_instance_root; // Immutable attribute - Used to detect inheritance from this base class
    ObjectTypeId_t m_instance_type;

  protected:

  public:

    /**
     * @brief Constructor.
     */

    AnyObject (void);

    /**
     * @brief Copy constructor.
     */

    AnyObject (const AnyObject& obj);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~AnyObject (void); 

    // Accessor methods
    const ccs::types::char8* GetInstanceName (void) const;
    void SetInstanceName (const ccs::types::char8 * const name);

    ObjectUId_t GetInstanceUId (void) const;
    void SetInstanceUId (ObjectUId_t id);
    void SetInstanceUId (void); // Allows to register unnamed objects

    ObjectTypeId_t GetInstanceType (void) const;
    void SetInstanceType (ObjectTypeId_t type);
    void SetInstanceType (const ccs::types::char8 * const type);

    // Miscellaneous methods
    virtual bool IsType (void) const;
    bool IsType (ObjectTypeId_t type) const;
    bool IsType (const ccs::types::char8 * const type) const;

    /**
     * @brief Copy assignment operator.
     */

    AnyObject& operator= (const AnyObject& obj); 

};

/**
 * @brief The interface class providing methods to get/set parameters by name.
 */

typedef class CfgableObject_Iface
{

  private:

  protected:

    // Constructor methods
    CfgableObject_Iface (void) {};

    // Destructor method
    virtual ~CfgableObject_Iface (void) {}; 

  public:

    // Initialiser methods

    // Accessor methods
    virtual RET_STATUS SetParameter (const ccs::types::char8 * name, const ccs::types::char8 * value) = 0;

    // Miscellaneous methods

} CfgableObject;

/**
 * @brief The interface class providing a messaging methods.
 */

typedef class MsgableObject_Iface
{

  private:

  protected:

    // Constructor methods
    MsgableObject_Iface (void) {};

    // Destructor method
    virtual ~MsgableObject_Iface (void) {};

  public:

    // Initialiser methods

    // Accessor methods

    // Miscellaneous methods
    virtual RET_STATUS ProcessMessage (const ccs::types::char8 * msg) = 0;

} MsgableObject;

typedef AnyObject* (*AnyObject_Constructor_t) (void);
typedef void (*AnyObject_Destructor_t) (AnyObject*);

// Global variables

// Function declaration

ObjectTypeId_t GetInstanceTypeByName (const ccs::types::char8 * const type);

} // namespace base

} // namespace ccs

#endif // _AnyObject_h_


/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/core/MsgableTypeDatabase.h $
* $Id: MsgableTypeDatabase.h 101405 2019-08-06 14:39:31Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Generic type class definition
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

/**
 * @file MsgableTypeDatabase.h
 * @brief Header file for MsgableTypeDatabase class.
 * @date 13/05/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the MsgableTypeDatabase class interface.
 * The MsgableTypeDatabase class provides a MsgableObject interface atop the
 * ccs::types::GlobalTypeDatabase singleton.
 */

#ifndef _MsgableTypeDatabase_h_
#define _MsgableTypeDatabase_h_

// Global header files

// Local header files

#include <BasicTypes.h>
#include <AnyObject.h>

// Constants

// Type definition

namespace sup {

namespace core {

/**
 * @brief Interface class providing support for registering types to the global AnyType database.
 */

class MsgableTypeDatabase : public ccs::base::AnyObject, public ccs::base::CfgableObject, public ccs::base::MsgableObject
{

  private:

  protected:

  public:

    /**
     * @brief Constructor.
     * @param service Named RPC service.
     */

    MsgableTypeDatabase (void);

    /**
     * @brief Destructor.
     */

    virtual ~MsgableTypeDatabase (void);

    /**
     * @brief Accessor. See ccs::base::CfgableObject::SetParameter.
     * @detail Parameter sets logging verbosity.
     */

    virtual bool SetParameter (const char* name, const char* value); // Specialises virtual method

    /**
     * @brief Accessor. See ccs::base::MsgableObject::ProcessMessage.
     * @detail Messages correspond to orders to load library of User-supplied
     * types, register type provided as JSON description, etc.
     */

    virtual bool ProcessMessage (const char* msg); // Specialises virtual method

};

// Global variables

} // namespace core

} // namespace sup

#endif // _MsgableTypeDatabase_h_ 


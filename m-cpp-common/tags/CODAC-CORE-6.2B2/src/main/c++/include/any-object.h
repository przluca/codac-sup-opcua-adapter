#ifndef ANY_OBJECT_H
#define ANY_OBJECT_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/include/any-object.h $
* $Id: any-object.h 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*		  CS 90 046
*		  13067 St. Paul-lez-Durance Cedex
*		  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/* Global header files */

/* Local header files */

#include "AnyObject.h" /* Maintained for backward compatibility reason */
#include "ObjectDatabase.h"
#include "ObjectFactory.h"

/* Constants */

/* Maintained for backward compatibility reason */
#define ObjectDatabase_Register ccs::base::GlobalObjectDatabase::Register 
#define ObjectDatabase_Remove ccs::base::GlobalObjectDatabase::Remove
#define ObjectDatabase_GetInstance ccs::base::GlobalObjectDatabase::GetInstance

/* Maintained for backward compatibility reason */
#define ObjectFactory_Load ccs::base::GlobalObjectFactory::Load 
#define ObjectFactory_Instantiate ccs::base::GlobalObjectFactory::Instantiate
#define ObjectFactory_Register ccs::base::GlobalObjectFactory::Register
#define ObjectFactory_Terminate ccs::base::GlobalObjectFactory::Terminate

/* Type definition */

/* Global variables */

/* Function declaration */

using namespace ccs::base; /* For backward compatibility purposes with v1.0 */

#endif /* ANY_OBJECT_H */


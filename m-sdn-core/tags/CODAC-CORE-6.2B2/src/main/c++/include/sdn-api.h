#ifndef SDN_CORE_API_H
#define SDN_CORE_API_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/sdn-api.h $
* $Id: sdn-api.h 96477 2019-01-09 14:53:55Z sivecm $
*
* Project	: CODAC Core System
*
* Description	: SDN - Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
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

/* Global header files */

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */

//#include "sdn-mcast.h" /* SDN core library - API definition (sdn::mcast) */
//#include "sdn-core.h"  /* SDN core library - API definition (sdn::core) */
//#include "sdn-disc.h"  /* SDN core library - API definition (sdn::disc) */

#include "sdn-header.h"
//#include "sdn-footer.h" /* Favour forward class declaration*/
#include "sdn-topic.h" 
//#include "sdn-packet.h" /* Favour forward class declaration*/ /* For backward compatibility purposes with v1.0 - Deprecated */

#include "Participant.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "AsynchronousSubscriber.h"

/* Constants */

#ifdef __cplusplus

/* Type definition */

/* Global variables */

/* Function declaration */

/* Function definition */

extern "C" {
    
#endif /* __cplusplus */
    
/* ToDo - Insert C API declaration */
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SDN_CORE_API_H */

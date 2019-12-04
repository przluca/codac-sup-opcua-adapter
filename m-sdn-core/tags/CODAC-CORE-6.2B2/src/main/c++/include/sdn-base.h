#ifndef SDN_BASE_H
#define SDN_BASE_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/sdn-base.h $
* $Id: sdn-base.h 99201 2019-04-19 11:02:48Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: SDN - Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
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

/* Documentation - This file is used to simplify the declaration of foundation classes for compiling the 
   SDN core library. It is not intended to be used by client applications, hence can not be included in
   public header files.
*/

/* Global header files */

#include <lu-table.h> /* LUTable template class definition */
#include <any-thread.h> /* Thread management class definition */
//#include <any-type.h> /* Runtime datatype definition */
#include <Buffer.h> /* Memory management template classes definition */
#include <CircularBuffer.h> /* Memory management template classes definition */
#include <Statistics.h> /* Statistics template class definition */

#include <AnyObject.h> /* Root object class definition */

//#include <base-api.h> /* Interface definition common for ccs::mcast and ccs::ucast participants */
//#include <mcast-api.h> /* Interface definition ccs::mcast participants */
//#include <ucast-api.h> /* Interface definition ccs::ucast participants */

/* Local header files */

//#include "lu-table.h" /* LUTable template class definition */
//#include "any-thread.h" /* Thread management class definition */
#include "any-type.h" /* Runtime datatype definition */
//#include "Buffer.h" /* Memory management template classes definition */
//#include "CircularBuffer.h" /* Memory management template classes definition */
//#include "Statistics.h" /* Statistics template class definition */

#include "sdn-common.h" /* Common header file */

//#include "AnyObject.h" /* Root object class definition */
#include "any-type-ext.h" /* Extended type definition, description, units, etc. */

#include "base-api.h" /* Interface definition common for ccs::mcast and ccs::ucast participants */
#include "mcast-api.h" /* Interface definition ccs::mcast participants */
#include "ucast-api.h" /* Interface definition ccs::ucast participants */

#include "base-participant.h" /* Interface definition common for sdn::mcast and sdn::ucast participants */
#include "base-publisher.h"   /* Interface definition common for sdn::mcast and sdn::ucast participants */
#include "base-subscriber.h"  /* Interface definition common for sdn::mcast and sdn::ucast participants */

/* Constants */

/* Type definition */

namespace sdn {

namespace base {

using ccs::base::AnyObject;
using ccs::base::AnyThread;
using ccs::base::Statistics;

/* Global variables */

/* Function declaration */

/* Function definition */

} /* namespace base */

} /* namespace sdn */

#endif /* SDN_BASE_H */

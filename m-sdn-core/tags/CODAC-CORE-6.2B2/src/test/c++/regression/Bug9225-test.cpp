/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug9225-test.cpp $
* $Id: Bug9225-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
*
* Project	: CODAC Core System
*
* Description	: Unit test code
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

/* Global header files */

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include <gtest/gtest.h> /* Google test framework */

/* Local header files */

#include "sdn-types.h" /* Misc. type definition */
#include "sdn-tools.h" /* Misc. helper functions */

#include "sdn-base.h" /* Misc. class definition */
#include "sdn-header.h"
#include "sdn-packet.h"

#include "sdn-api.h" /* SDN core library - API definition */

/* Constants */

/* Type definition */

class Bug9225_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug9225_Test, Success)
{

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "RTDN_SDN_FELIX", 64u);

  bool ret = (sdn::TopicHelper::IsMetadataValid(mdata) == true);

  if (ret)
    {
      ret = (mdata.mcast_port > 1024);
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug9225_Test, Failure)
{

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "RTDN_SDN_FELIX", 64u, "239.0.92.25", 0u);

  bool ret = (sdn::TopicHelper::IsMetadataValid(mdata) == false);

  ASSERT_EQ(ret, true);
}

TEST(Bug9225_Test, Restricted)
{

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "RTDN_SDN_FELIX", 64u, "239.0.92.25", 1023u);

  bool ret = (sdn::TopicHelper::IsMetadataValid(mdata) == true); // There should be a warning

  ASSERT_EQ(ret, true);
}


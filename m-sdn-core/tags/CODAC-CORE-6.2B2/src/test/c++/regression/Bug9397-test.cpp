/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug9397-test.cpp $
* $Id: Bug9397-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-api.h" /* SDN core library - API definition */

/* Constants */

/* Type definition */

class Bug9397_Test
{

  public:

    sdn::Metadata_t mdata;
    sdn::Publisher* sdn_pub;
    sdn::Subscriber* sdn_sub;

    bool Initialise (void);

    virtual ~Bug9397_Test (void);

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

bool Bug9397_Test::Initialise (void)
{
  // Initialise attributes
  sdn::TopicHelper::InitializeMetadata(mdata, "Bug9397", 1024, "127.0.0.1", 9397);

  sdn_pub = static_cast<sdn::Publisher*>(NULL);
  sdn_sub = static_cast<sdn::Subscriber*>(NULL);

  bool ret = true;

  // Instantiate SDN participants
  if (ret)
    {
      ret = ((sdn_pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL));
    }

  if (ret)
    {
      ret = (sdn_pub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sdn_pub->Configure() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((sdn_sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL));
    }

  if (ret)
    {
      ret = (sdn_sub->SetInterface("lo") == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sdn_sub->Configure() == STATUS_SUCCESS);
    }

  return ret;

};

Bug9397_Test::~Bug9397_Test (void)
{

  // Release resources
  if (static_cast<sdn::Publisher*>(NULL) != sdn_pub)
    {
      delete sdn_pub;
      sdn_pub = static_cast<sdn::Publisher*>(NULL);
    }

  if (static_cast<sdn::Subscriber*>(NULL) != sdn_sub)
    {
      delete sdn_sub;
      sdn_sub = static_cast<sdn::Subscriber*>(NULL);
    }

  return;

};

TEST(Bug9397_Test, Default)
{

  Bug9397_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = ((test.sdn_pub)->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = ((test.sdn_sub)->Receive(0ul) == STATUS_SUCCESS);
    }

  ASSERT_EQ(ret, true);
}


/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug7919-test.cpp $
* $Id: Bug7919-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-api.h" /* SDN core library - API definition */

/* Constants */

/* Type definition */

class Bug7919_Test
{

  public:

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

TEST(Bug7919_Test, HelperTools_success)
{

  bool ret = (sdn::HelperTools::IsInterfaceValid(DEFAULT_IFACE_NAME) == true);;

  ASSERT_EQ(ret, true);
}

TEST(Bug7919_Test, HelperTools_error)
{

  bool ret = (sdn::HelperTools::IsInterfaceValid("invalid") == false);

  ASSERT_EQ(ret, true);
}

TEST(Bug7919_Test, SetInterface_success)
{

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MyTopic", 64u);

  /* Create Participants with metadata */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = ((pub->SetInterface(DEFAULT_IFACE_NAME) == STATUS_SUCCESS) && (sub->SetInterface(DEFAULT_IFACE_NAME) == STATUS_SUCCESS));
    }

  if (ret)
    {
      delete pub;
      delete sub;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug7919_Test, SetInterface_error)
{

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MyTopic", 64u);

  /* Create Participants with metadata */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = ((pub->SetInterface("invalid") != STATUS_SUCCESS) && (sub->SetInterface("invalid") != STATUS_SUCCESS));
    }

  if (ret)
    {
      delete pub;
      delete sub;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug7919_Test, Configure_success)
{

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MyTopic", 64u);
#if 1 // Unsure about environment variables on Jenkins
  if (ret)
    {
      log_info("TEST(Bug7919_Test, Configure_success) - Set environment variable");
      ret = (setenv(SDN_INTERFACE_ENVVAR, DEFAULT_IFACE_NAME, 1) == 0); // Valid environment variable
    }
#endif
  /* Create Participants with metadata */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = ((pub->Configure() == STATUS_SUCCESS) && (sub->Configure() == STATUS_SUCCESS)); // Environment variable
    }

  if (ret)
    {
      delete pub;
      delete sub;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug7919_Test, Configure_error)
{

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MyTopic", 64u);

  if (ret)
    {
      log_info("TEST(Bug7919_Test, Configure_error) - Set environment variable");
      ret = (setenv(SDN_INTERFACE_ENVVAR, "invalid", 1) == 0); // Invalid environment variable
    }

  /* Create Participants with metadata */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = ((pub->Configure() != STATUS_SUCCESS) && (sub->Configure() != STATUS_SUCCESS)); // Environment variable
    }

  if (ret)
    {
      log_info("TEST(Bug7919_Test, Configure_error) - Reset environment variable");
      ret = (setenv(SDN_INTERFACE_ENVVAR, DEFAULT_IFACE_NAME, 1) == 0); // Valid environment variable
    }

  if (ret)
    {
      delete pub;
      delete sub;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug7919_Test, Communicate_success)
{

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MyTopic", 64u);

  /* Create Participants with metadata */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL)); 
    }

  if (ret)
    {
      ret = ((pub->Configure() == STATUS_SUCCESS) && (sub->Configure() == STATUS_SUCCESS)); // Environment variable
    }

  if (ret)
    {
      ret = (pub->Publish() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sub->Receive(100000000ul) == STATUS_SUCCESS);
    }

  if (ret)
    {
      delete pub;
      delete sub;
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug7919_Test, Communicate_error)
{

  bool ret = true;

  sdn::Metadata_t mdata; sdn::TopicHelper::InitializeMetadata(mdata, "MyTopic", 64u);
#if 1 // sdn::core::Participant::SetInterface does not alter default in case of failure
  if (ret)
    {
      log_info("TEST(Bug7919_Test, Communicate_error) - Set environment variable");
      ret = (setenv(SDN_INTERFACE_ENVVAR, "invalid", 1) == 0); // Invalid environment variable
    }
#endif
  /* Create Participants with metadata */
  sdn::Publisher* pub = static_cast<sdn::Publisher*>(NULL);

  if (ret)
    {
      ret = ((pub = new (std::nothrow) sdn::Publisher (mdata)) != static_cast<sdn::Publisher*>(NULL)); 
    }

  sdn::Subscriber* sub = static_cast<sdn::Subscriber*>(NULL);

  if (ret)
    {
      ret = ((sub = new (std::nothrow) sdn::Subscriber (mdata)) != static_cast<sdn::Subscriber*>(NULL)); 
    }
#if 0 // sdn::core::Participant::SetInterface does not alter default in case of failure
  if (ret)
    {
      ret = ((pub->SetInterface("invalid") != STATUS_SUCCESS) && (sub->SetInterface("invalid") != STATUS_SUCCESS));
    }
#endif
  if (ret)
    {
      ret = ((pub->Configure() != STATUS_SUCCESS) && (sub->Configure() != STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = (pub->Publish() != STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (sub->Receive(100000000ul) != STATUS_SUCCESS);
    }
#if 1 // sdn::core::Participant::SetInterface does not alter default in case of failure
  if (ret)
    {
      log_info("TEST(Bug7919_Test, Communicate_error) - Reset environment variable");
      ret = (setenv(SDN_INTERFACE_ENVVAR, DEFAULT_IFACE_NAME, 1) == 0); // Valid environment variable
    }
#endif
  if (ret)
    {
      delete pub;
      delete sub;
    }

  ASSERT_EQ(ret, true);
}

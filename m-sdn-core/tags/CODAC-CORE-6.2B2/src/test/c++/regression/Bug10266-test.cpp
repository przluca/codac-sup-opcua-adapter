/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Bug10266-test.cpp $
* $Id: Bug10266-test.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-msg.h"

/* Constants */

/* Type definition */

class Bug10266_Test
{

  public:

    sdn::message::Discovery* msg;

    bool Initialise (void);

    virtual ~Bug10266_Test (void);

  protected:

};

/* Global variables */

/* Internal function declaration */

/* Internal function definition */

bool Bug10266_Test::Initialise (void)
{
  // Initialise attributes
  msg = static_cast<sdn::message::Discovery*>(NULL);

  bool ret = ((msg = new sdn::message::Discovery) != static_cast<sdn::message::Discovery*>(NULL));

  if (ret)
    {
      msg->SetSize(1024u); 
      ret = (msg->CreateInstance() == STATUS_SUCCESS);
    }

  return ret;

};

Bug10266_Test::~Bug10266_Test (void)
{

  // Release resources
  if (msg != static_cast<sdn::message::Discovery*>(NULL))
    {
      delete msg;
      msg = static_cast<sdn::message::Discovery*>(NULL);
    }

  return;

};

TEST(Bug10266_Test, Query_all)
{

  Bug10266_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
		      "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"/>", 
		      (test.msg)->GetSize()) != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = ((test.msg)->Parse() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (((test.msg)->IsValid() == true) && ((test.msg)->IsQuery() == true));
    }

  if (ret)
    {
      ret = (((test.msg)->IsJoin() == false) && ((test.msg)->IsLeave() == false) && ((test.msg)->IsResponse() == false));
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10266_Test, Query_host)
{

  Bug10266_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
		      "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"><participant host=\"4501AS-SRV-0001\"/></message>", 
		      (test.msg)->GetSize()) != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = ((test.msg)->Parse() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (((test.msg)->IsValid() == true) && ((test.msg)->IsQuery() == true));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetHostName(), "4501AS-SRV-0001") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->IsJoin() == false) && ((test.msg)->IsLeave() == false) && ((test.msg)->IsResponse() == false));
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10266_Test, Query_topic)
{

  Bug10266_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
		      "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"><participant><topic name=\"Bug10266Topic\"/></participant></message>", 
		      (test.msg)->GetSize()) != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = ((test.msg)->Parse() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (((test.msg)->IsValid() == true) && ((test.msg)->IsQuery() == true));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetTopicName(), "Bug10266Topic") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->IsJoin() == false) && ((test.msg)->IsLeave() == false) && ((test.msg)->IsResponse() == false));
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10266_Test, Join)
{

  Bug10266_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
		      "<message group=\"status\" qualifier=\"startup\" schemaVersion=\"1.0\"><participant hostName=\"trunk.codac.iter.org\" applicationName=\"TEST-CPS-PCF0SDN\" pid=\"23180\" interfaceName=\"lo\" ipAddress=\"127.0.0.1\" role=\"publisher\"><topic name=\"TEST_CPS_PF1_QVAL\" version=\"9\" size=\"20\" mapping=\"239.0.48.194:53053\"><attributes><attribute name=\"undefined\" dataType=\"uint8\" multiplicity=\"20\"/></attributes></topic></participant></message>", 
		      (test.msg)->GetSize()) != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = ((test.msg)->Parse() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (((test.msg)->IsValid() == true) && ((test.msg)->IsJoin() == true));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetHostName(), "trunk.codac.iter.org") == 0);
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetParticipant(), "TEST-CPS-PCF0SDN") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->IsPublisher() == true) && ((test.msg)->IsSubscriber() == false));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetTopicName(), "TEST_CPS_PF1_QVAL") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->GetTopicSize() == 20u) && ((test.msg)->GetTopicVersion() == 9u));
    }

  if (ret)
    {
      ret = (((test.msg)->IsResponse() == false) && ((test.msg)->IsLeave() == false) && ((test.msg)->IsQuery() == false));
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10266_Test, Response)
{

  Bug10266_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
		      "<message group=\"discovery\" qualifier=\"response\" schemaVersion=\"1.0\"><participant hostName=\"trunk.codac.iter.org\" applicationName=\"TEST-CPS-PCF0SDN\" pid=\"23180\" interfaceName=\"lo\" ipAddress=\"127.0.0.1\" role=\"publisher\"><topic name=\"TEST_CPS_PF1_QVAL\" version=\"9\" size=\"20\" mapping=\"239.0.48.194:53053\"><attributes><attribute name=\"undefined\" dataType=\"uint8\" multiplicity=\"20\"/></attributes></topic></participant></message>", 
		      (test.msg)->GetSize()) != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = ((test.msg)->Parse() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (((test.msg)->IsValid() == true) && ((test.msg)->IsResponse() == true));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetHostName(), "trunk.codac.iter.org") == 0);
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetParticipant(), "TEST-CPS-PCF0SDN") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->IsPublisher() == true) && ((test.msg)->IsSubscriber() == false));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetTopicName(), "TEST_CPS_PF1_QVAL") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->GetTopicSize() == 20u) && ((test.msg)->GetTopicVersion() == 9u));
    }

  if (ret)
    {
      ret = (((test.msg)->IsJoin() == false) && ((test.msg)->IsLeave() == false) && ((test.msg)->IsQuery() == false));
    }

  ASSERT_EQ(ret, true);
}

TEST(Bug10266_Test, Leave)
{

  Bug10266_Test test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
		      "<message group=\"status\" qualifier=\"shutdown\" schemaVersion=\"1.0\"><participant hostName=\"trunk.codac.iter.org\" applicationName=\"TEST-CPS-PCF0SDN\" pid=\"23180\" interfaceName=\"lo\" ipAddress=\"127.0.0.1\" role=\"publisher\"><topic name=\"TEST_CPS_PF1_QVAL\" version=\"9\" size=\"20\" mapping=\"239.0.48.194:53053\"><attributes><attribute name=\"undefined\" dataType=\"uint8\" multiplicity=\"20\"/></attributes></topic></participant></message>", 
		      (test.msg)->GetSize()) != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = ((test.msg)->Parse() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (((test.msg)->IsValid() == true) && ((test.msg)->IsLeave() == true));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetHostName(), "trunk.codac.iter.org") == 0);
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetParticipant(), "TEST-CPS-PCF0SDN") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->IsPublisher() == true) && ((test.msg)->IsSubscriber() == false));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetTopicName(), "TEST_CPS_PF1_QVAL") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->GetTopicSize() == 20u) && ((test.msg)->GetTopicVersion() == 9u));
    }

  if (ret)
    {
      ret = (((test.msg)->IsJoin() == false) && ((test.msg)->IsQuery() == false) && ((test.msg)->IsResponse() == false));
    }

  ASSERT_EQ(ret, true);
}


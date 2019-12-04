#include <gtest/gtest.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include "sdn-api.h"
#include "sdn-msg.h"

class SDN_msg
{
	public:
		
    sdn::message::Discovery* msg;

	bool Initialise (void) {
		// Initialise attributes
		msg = static_cast<sdn::message::Discovery*>(NULL);

		bool ret = ((msg = new sdn::message::Discovery) != static_cast<sdn::message::Discovery*>(NULL));

		if (ret)
		{
			msg->SetSize(1024u); 
			ret = (msg->CreateInstance() == STATUS_SUCCESS);
		}

		return ret;
	}

	virtual ~SDN_msg (void);

	protected:

};

SDN_msg::~SDN_msg (void)
{
	// Release resources
	if (msg != static_cast<sdn::message::Discovery*>(NULL))	{
		delete msg;
		msg = static_cast<sdn::message::Discovery*>(NULL);
	}

	return;
};

class SDN_qry// : public testing::Test
{
	public:

    sdn::message::Query* qry;

	bool Initialise (void) {
		// Initialise attributes
		qry = static_cast<sdn::message::Query*>(NULL);

		bool ret = ((qry = new sdn::message::Query) != static_cast<sdn::message::Query*>(NULL));

		if (ret) {
			qry->SetSize(1024u); 
			ret = (qry->CreateInstance() == STATUS_SUCCESS);
		}

		return ret;
	}

	virtual ~SDN_qry (void);

	protected:

};

SDN_qry::~SDN_qry (void)
{
	// Release resources
	if (qry != static_cast<sdn::message::Query*>(NULL)) {
		delete qry;
		qry = static_cast<sdn::message::Query*>(NULL);
	}

	return;
};

TEST(SDN_msg, Query_all)
{
	SDN_msg test;

	bool ret = test.Initialise();

	if (ret) {
		ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
			  "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"/>", 
			  (test.msg)->GetSize()) != static_cast<void*>(NULL));
	}

	if (ret) {
		ret = ((test.msg)->Parse() == STATUS_SUCCESS);
	}

	if (ret) {
		ret = (((test.msg)->IsValid() == STATUS_SUCCESS) && ((test.msg)->IsQuery() == STATUS_SUCCESS));
	}

	if (ret) {
		ret = (((test.msg)->IsJoin() == STATUS_ERROR) && ((test.msg)->IsLeave() == STATUS_ERROR) && ((test.msg)->IsResponse() == STATUS_ERROR));
	}

	if (ret) {
      ret = (strcmp((test.msg)->GetMsgGroup(), "discovery") == 0);
    }

	ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST(SDN_msg, Query_host)
{

  SDN_msg test;

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
      ret = (((test.msg)->IsValid() == STATUS_SUCCESS) && ((test.msg)->IsQuery() == STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetHostName(), "4501AS-SRV-0001") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->IsJoin() == STATUS_ERROR) && ((test.msg)->IsLeave() == STATUS_ERROR) && ((test.msg)->IsResponse() == STATUS_ERROR));
    }

	ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST(SDN_msg, Query_topic)
{

  SDN_msg test;

  bool ret = test.Initialise();

  if (ret)
    {
      ret = (sstrncpy(static_cast<char*>((test.msg)->GetInstance()), 
		      "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"><participant><topic name=\"TestTopic\"/></participant></message>", 
		      (test.msg)->GetSize()) != static_cast<void*>(NULL));
    }

  if (ret)
    {
      ret = ((test.msg)->Parse() == STATUS_SUCCESS);
    }

  if (ret)
    {
      ret = (((test.msg)->IsValid() == STATUS_SUCCESS) && ((test.msg)->IsQuery() == STATUS_SUCCESS));
    }

  if (ret)
    {
      ret = (strcmp((test.msg)->GetTopicName(), "TestTopic") == 0);
    }

  if (ret)
    {
      ret = (((test.msg)->IsJoin() == STATUS_ERROR) && ((test.msg)->IsLeave() == STATUS_ERROR) && ((test.msg)->IsResponse() == STATUS_ERROR));
    }

  ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST(SDN_msg, Join)
{

  SDN_msg test;

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
      ret = (((test.msg)->IsValid() == STATUS_SUCCESS) && ((test.msg)->IsJoin() == STATUS_SUCCESS));
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
      ret = (((test.msg)->IsPublisher() == STATUS_SUCCESS) && ((test.msg)->IsSubscriber() == STATUS_ERROR));
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
      ret = (((test.msg)->IsResponse() == STATUS_ERROR) && ((test.msg)->IsLeave() == STATUS_ERROR) && ((test.msg)->IsQuery() == STATUS_ERROR));
    }

  ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST(SDN_msg, Response)
{

  SDN_msg test;

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
      ret = (((test.msg)->IsValid() == STATUS_SUCCESS) && ((test.msg)->IsResponse() == STATUS_SUCCESS));
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
      ret = (((test.msg)->IsPublisher() == STATUS_SUCCESS) && ((test.msg)->IsSubscriber() == STATUS_ERROR));
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
      ret = (((test.msg)->IsJoin() == STATUS_ERROR) && ((test.msg)->IsLeave() == STATUS_ERROR) && ((test.msg)->IsQuery() == STATUS_ERROR) && ((test.msg)->IsUpdate() == STATUS_ERROR));
    }

  ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST(SDN_msg, Leave)
{

  SDN_msg test;

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
      ret = (((test.msg)->IsValid() == STATUS_SUCCESS) && ((test.msg)->IsLeave() == STATUS_SUCCESS));
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
      ret = (((test.msg)->IsPublisher() == STATUS_SUCCESS) && ((test.msg)->IsSubscriber() == STATUS_ERROR));
    }

	if (ret) {
	ret = (strcmp((test.msg)->GetTopicName(), "TEST_CPS_PF1_QVAL") == 0);
    }

	if (ret) {
		ret = (((test.msg)->GetTopicSize() == 20u) && ((test.msg)->GetTopicVersion() == 9u));
    }

	if (ret){
		ret = (strcmp((test.msg)->GetMCastGroup(), "239.0.48.194") == 0);
	}

	if (ret) {
		ret = ((test.msg)->GetMCastPort() == 53053u);
	}

	if (ret) {
		ret = (((test.msg)->IsJoin() == STATUS_ERROR) && ((test.msg)->IsQuery() == STATUS_ERROR) && ((test.msg)->IsResponse() == STATUS_ERROR));
    }

	(test.msg)->SetInstance(static_cast<char*>((test.msg)->GetInstance()), (test.msg)->GetSize());
	(test.msg)->ClearInstance();

	ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST(SDN_qry, Query)
{
	SDN_qry test;

	bool ret = test.Initialise();

	if (ret){
		ret = ((test.qry)->SetHostName("trunk.codac.iter.org") == STATUS_SUCCESS);
	}

	if (ret){
		ret = ((test.qry)->SetTopicName("TEST_CPS_PF1_QVAL") == STATUS_SUCCESS);
	}

	ASSERT_EQ(ret, STATUS_SUCCESS);
}



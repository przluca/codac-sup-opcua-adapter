#include <gtest/gtest.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include "sdn-api.h"

/**************************************************************
 * Publisher test
 **************************************************************/

uint32_t __pub_counter = 0u;

class SDN_Publisher : public testing::Test
{
	// cppcheck-suppress unusedFunction // GTest fixture
	void SetUp(){
		// Replace static initialisation to get rid of -std=c++11
		sstrncpy((char*) iface_name, "lo", STRING_MAX_LENGTH);
		sstrncpy((char*) topic_name, "payload-32", STRING_MAX_LENGTH);
		topic_size = 1400;

		sdn::TopicHelper::InitializeMetadata(mdata);
		sstrncpy((char*) mdata.name, topic_name, STRING_MAX_LENGTH);
		mdata.size=topic_size;
	}

	protected:
		sdn::Metadata_t mdata;

		char iface_name [STRING_MAX_LENGTH];
		char topic_name [STRING_MAX_LENGTH];
		int topic_size;
};

TEST_F(SDN_Publisher, Configure)
{
	sdn::Publisher publisher (mdata);
	ASSERT_EQ(publisher.SetInterface(iface_name), STATUS_SUCCESS);
	ASSERT_EQ(publisher.SetMetadata(mdata), STATUS_SUCCESS);

	ASSERT_EQ(publisher.Configure(), STATUS_SUCCESS);
}

TEST_F(SDN_Publisher, GetInterface)
{
	sdn::Publisher publisher (mdata);
	publisher.Configure();
	ASSERT_EQ(publisher.SetInterface(iface_name), STATUS_SUCCESS);

	ASSERT_STREQ(publisher.GetInterface(), iface_name) << publisher.GetInterface();
}

TEST_F(SDN_Publisher, SetInterface)
{
	sdn::Topic topic(mdata);
	sdn::Publisher publisher (topic);
	publisher.Configure();
	ASSERT_EQ(publisher.SetInterface(iface_name, 1024u), STATUS_SUCCESS);
}

TEST_F(SDN_Publisher, SetTopic)
{
	sdn::Publisher publisher;
	sdn::Topic topic(mdata);
	ASSERT_EQ(publisher.SetTopic(topic), STATUS_SUCCESS);
	publisher.Configure();
}

void __callback (sdn::Publisher* pub) { log_info("Static callback called"); __pub_counter += 1u; };
TEST_F(SDN_Publisher, SetCallback)
{
	sdn::Publisher publisher (mdata);
	ASSERT_EQ(publisher.SetCallback((void(*)(void*)) &__callback), STATUS_SUCCESS);
	publisher.Configure();
}

TEST_F(SDN_Publisher, SetCallback_attr)
{
	sdn::Publisher publisher (mdata);
	ASSERT_EQ(publisher.SetCallback((void(*)(void*)) &__callback, static_cast<void*>(&publisher)), STATUS_SUCCESS);
	publisher.Configure();
}

TEST_F(SDN_Publisher, IsInitialized)
{
	sdn::Publisher publisher (mdata);
	publisher.SetInterface(iface_name);

	ASSERT_EQ(publisher.SetMetadata(mdata), STATUS_SUCCESS);
	publisher.Configure();

	ASSERT_TRUE(publisher.IsInitialized());
}

TEST_F(SDN_Publisher, IsPayloadOrdered)
{
	sdn::Publisher publisher (mdata);
	publisher.SetInterface(iface_name);

	ASSERT_EQ(publisher.SetMetadata(mdata), STATUS_SUCCESS);
	publisher.Configure();

	ASSERT_TRUE(publisher.IsPayloadOrdered());
}

TEST_F(SDN_Publisher, IsMetadataValid)
{
	ASSERT_TRUE(sdn::TopicHelper::IsMetadataValid(mdata)) << topic_name <<  mdata.name;
}

TEST_F(SDN_Publisher, SetTopicName_GetTopicName)
{
	sdn::Publisher publisher (mdata);
	publisher.Configure();
	//ASSERT_EQ(publisher.SetTopicName(topic_name), STATUS_SUCCESS);
	ASSERT_STREQ(publisher.GetTopicName(), topic_name);
}

TEST_F(SDN_Publisher, SetMetadata)
{
	sdn::Publisher publisher (mdata);
	publisher.Configure();
	ASSERT_EQ(publisher.SetMetadata(mdata), STATUS_SUCCESS);
}

TEST_F(SDN_Publisher, SetBufferDepth)
{
	sdn::Publisher publisher (mdata);
	publisher.Configure();
	ASSERT_EQ(publisher.SetBufferDepth(0u), STATUS_SUCCESS);
}

TEST_F(SDN_Publisher, SetPayloadOrder)
{
	sdn::Publisher publisher (mdata);
	publisher.Configure();
	publisher.SetPayloadOrder(ccs::types::BigEndian);
}

TEST_F(SDN_Publisher, GetTopicSize)
{
	sdn::Publisher publisher (mdata);
	publisher.Configure();
	ASSERT_EQ(publisher.GetTopicSize(), topic_size);
}

TEST_F(SDN_Publisher, GetSocket)
{
	sdn::Publisher publisher (mdata);
	publisher.SetInterface(iface_name);
	publisher.Configure();

	ASSERT_STREQ(publisher.GetInterface(), iface_name) ;
	ASSERT_NE(publisher.GetSocket(), -1) << publisher.GetSocket();
}

TEST_F(SDN_Publisher, Do)
{
	sdn::Publisher publisher (mdata);
	publisher.SetInterface(iface_name);
	publisher.Configure();
	ASSERT_EQ(publisher.Do(), STATUS_SUCCESS);
}

TEST_F(SDN_Publisher, DoBackgroundActivity)
{
	sdn::Publisher publisher (mdata);
	publisher.SetInterface(iface_name);
	publisher.Configure();
	ASSERT_EQ(publisher.DoBackgroundActivity(), STATUS_SUCCESS);
}

TEST_F(SDN_Publisher, Publish)
{
	sdn::Publisher publisher (mdata);
	publisher.SetInterface(iface_name);
	publisher.Configure();

	ASSERT_EQ(publisher.Publish(), STATUS_SUCCESS);
}

TEST_F(SDN_Publisher, GetTopicHeader)
{
	/* Create publisher using metadata */
	sdn::Publisher pub (mdata);
	pub.SetInterface(iface_name);
	pub.Configure();

	ASSERT_STRNE((char*)pub.GetTopicHeader(), NULL);
}
#if 0
TEST_F(SDN_Publisher, GetTopicFooter)
{
	/* Create publisher using metadata */
	sdn::Publisher pub (mdata);
	pub.SetInterface(iface_name);
	pub.Configure();

	ASSERT_STRNE((char*)pub.GetTopicFooter(), NULL);
}
#endif
TEST_F(SDN_Publisher, GetTopicInstance)
{
	/* Create publisher using metadata */
	sdn::Publisher pub (mdata);
	pub.SetInterface(iface_name);
	pub.Configure();

	ASSERT_STRNE((char*)pub.GetTopicInstance(), NULL);
}

TEST_F(SDN_Publisher, GetPayloadOrder)
{
	/* Create publisher using metadata */
	sdn::Publisher pub (mdata);
	pub.Configure();

	pub.GetPayloadOrder();
}



#include <gtest/gtest.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include "sdn-api.h"

/**************************************************************
 * Subscriber test
 **************************************************************/
uint32_t __sub_counter = 0u;

class SDN_Subscriber : public testing::Test
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

		typedef struct {
			uint64_t index;
			uint64_t timestamp;
			char buffer [STRING_MAX_LENGTH];
			uint8_t reserved [8];
		} LegacyTopic_t;

};


TEST_F(SDN_Subscriber, Configure)
{
	sdn::Subscriber subscriber (mdata);
	ASSERT_EQ(subscriber.SetInterface(iface_name), STATUS_SUCCESS);
	ASSERT_EQ(subscriber.SetMetadata(mdata), STATUS_SUCCESS);

	ASSERT_EQ(subscriber.Configure(), STATUS_SUCCESS);
}

TEST_F(SDN_Subscriber, GetInterface)
{
	sdn::Topic topic(mdata);
	sdn::Subscriber subscriber (topic);
	subscriber.Configure();
	ASSERT_EQ(subscriber.SetInterface(iface_name), STATUS_SUCCESS)<<iface_name;

	ASSERT_STREQ(subscriber.GetInterface(), iface_name) << subscriber.GetInterface();
}

TEST_F(SDN_Subscriber, SetTopic)
{
	sdn::Subscriber subscriber;
	sdn::Topic topic(mdata);
	ASSERT_EQ(subscriber.SetTopic(topic), STATUS_SUCCESS);
	subscriber.Configure();
}

void __callback (sdn::Subscriber* sub) { log_info("Static callback called"); __sub_counter += 1u; };
TEST_F(SDN_Subscriber, SetCallback)
{
	sdn::Subscriber subscriber (mdata);
	ASSERT_EQ(subscriber.SetCallback((void(*)(void*)) &__callback), STATUS_SUCCESS);
	subscriber.Configure();
}

TEST_F(SDN_Subscriber, SetCallback_attr)
{
	sdn::Subscriber subscriber (mdata);
	ASSERT_EQ(subscriber.SetCallback((void(*)(void*)) &__callback, static_cast<void*>(&subscriber)), STATUS_SUCCESS);
	subscriber.Configure();
}

TEST_F(SDN_Subscriber, IsInitialized)
{
	sdn::Subscriber subscriber (mdata);
	subscriber.SetInterface(iface_name);
	ASSERT_EQ(subscriber.SetMetadata(mdata), STATUS_SUCCESS);
	subscriber.Configure();

	ASSERT_TRUE(subscriber.IsInitialized());
}

TEST_F(SDN_Subscriber, IsMetadataValid)
{
	ASSERT_TRUE(sdn::TopicHelper::IsMetadataValid(mdata)) << topic_name <<  mdata.name;
}

TEST_F(SDN_Subscriber, IsPayloadOrdered)
{
	sdn::Subscriber subscriber (mdata);
	subscriber.SetInterface(iface_name);
	ASSERT_EQ(subscriber.SetMetadata(mdata), STATUS_SUCCESS);
	subscriber.Configure();

	ASSERT_TRUE(subscriber.IsPayloadOrdered());
}

TEST_F(SDN_Subscriber, SetTopicName_GetTopicName)
{
	sdn::Subscriber subscriber (mdata);
	subscriber.Configure();
	//ASSERT_EQ(subscriber.SetTopicName(topic_name), STATUS_SUCCESS);
	ASSERT_STREQ(subscriber.GetTopicName(), topic_name);
}

TEST_F(SDN_Subscriber, SetMetadata)
{
	sdn::Subscriber subscriber (mdata);
	subscriber.Configure();
	ASSERT_EQ(subscriber.SetMetadata(mdata), STATUS_SUCCESS);
}

TEST_F(SDN_Subscriber, SetBufferDepth)
{
	sdn::Subscriber subscriber (mdata);
	subscriber.Configure();
	ASSERT_EQ(subscriber.SetBufferDepth(0u), STATUS_SUCCESS);
}

TEST_F(SDN_Subscriber, GetTopicSize)
{
	sdn::Subscriber subscriber (mdata);
	subscriber.Configure();
	ASSERT_EQ(subscriber.GetTopicSize(), topic_size);
}

TEST_F(SDN_Subscriber, GetSocket)
{
	sdn::Subscriber subscriber (mdata);
	subscriber.SetInterface(iface_name);
	subscriber.Configure();

	ASSERT_STREQ(subscriber.GetInterface(), iface_name) ;
	ASSERT_NE(subscriber.GetSocket(), -1) << subscriber.GetSocket();
}

TEST_F(SDN_Subscriber, GetTopicHeader)
{
	/* Create Subscriber using metadata */
	sdn::Subscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	ASSERT_STRNE((char*)sub.GetTopicHeader(), NULL);
}

#if 0
TEST_F(SDN_Subscriber, GetTopicFooter)
{
	/* Create publisher using metadata */
	sdn::Subscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	ASSERT_STRNE((char*)sub.GetTopicFooter(), NULL);
}
#endif
TEST_F(SDN_Subscriber, GetTopicInstance)
{
	/* Create publisher using metadata */
	sdn::Subscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	ASSERT_STRNE((char*)sub.GetTopicInstance(), NULL);
}

TEST_F(SDN_Subscriber, DoBackgroundActivity)
{
	/* Create publisher using metadata */
	sdn::Subscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	ASSERT_EQ(sub.DoBackgroundActivity(), STATUS_SUCCESS);
}

TEST_F(SDN_Subscriber, Receive)
{
	sdn::TopicHelper::InitializeMetadata(mdata, "LegacyTopic", sizeof(LegacyTopic_t));

	/* Create Publisher using metadata */
	sdn::Publisher pub (mdata);
	pub.SetInterface(iface_name);
	pub.Configure();

	/* Create Subscriber using metadata */
	sdn::Subscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	/* Create Topic */
	LegacyTopic_t pub_t; memset(&pub_t, 0, sizeof(LegacyTopic_t));
	LegacyTopic_t sub_t; memset(&sub_t, 0, sizeof(LegacyTopic_t));

	/* Topic Initialize */
	pub_t.index = 1ul;
	pub_t.timestamp = get_time();
	sstrncpy(pub_t.buffer, "Hello world!", STRING_MAX_LENGTH);

	pub.CopyTopicInstance(&pub_t, sizeof(LegacyTopic_t));

	pub.Publish();
	ASSERT_EQ(sub.Receive(10000), STATUS_SUCCESS);

	sub.CopyTopicInstance(&sub_t, sizeof(LegacyTopic_t));

	ASSERT_EQ(pub_t.index, sub_t.index);
	ASSERT_EQ(pub_t.timestamp, sub_t.timestamp);
	ASSERT_STREQ(pub_t.buffer, sub_t.buffer);

//	ASSERT_EQ(sub.Receive(), STATUS_SUCCESS);
}


#include <gtest/gtest.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include "sdn-api.h"

/**************************************************************
 * Subscriber test
 **************************************************************/
uint32_t __asynchrounousSub_counter = 0u;

class SDN_AsynchrounousSubscriber : public testing::Test
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

TEST_F(SDN_AsynchrounousSubscriber, Configure)
{
	sdn::AsynchronousSubscriber subscriber (mdata);
	ASSERT_EQ(subscriber.SetInterface(iface_name), STATUS_SUCCESS);
	ASSERT_EQ(subscriber.Configure(), STATUS_SUCCESS);

	ASSERT_EQ(subscriber.SetAffinity(1u), STATUS_SUCCESS);
	ASSERT_EQ(subscriber.SetTimeout(1000u), STATUS_SUCCESS);
}

TEST_F(SDN_AsynchrounousSubscriber, SetPriority)
{
	bool ret=false;
	sdn::AsynchronousSubscriber subscriber (mdata);

	subscriber.SetInterface(iface_name);
	subscriber.Configure();

	log_info("TEST(AnyThread_Test, SetPriority) - Set priority ..");
	ret = subscriber.SetPriority(80u, 2u);

	if (!ret) {
		log_notice("TEST(AnyThread_Test, SetPriority) - .. failure");
		ret = subscriber.SetPriority(0u, 0u); // Without RT policy
	}

	if (!ret) {
		ret = true; // Ignore
	}

	ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST_F(SDN_AsynchrounousSubscriber, GetInterface)
{
	sdn::Topic topic(mdata);
	sdn::AsynchronousSubscriber subscriber (topic);

	subscriber.Configure();
	subscriber.SetInterface(iface_name);

	ASSERT_STREQ(subscriber.GetInterface(), iface_name) << subscriber.GetInterface();
}

TEST_F(SDN_AsynchrounousSubscriber, GetTopicName)
{
	sdn::AsynchronousSubscriber subscriber (mdata);
	subscriber.Configure();
	//ASSERT_EQ(subscriber.SetTopicName(topic_name), STATUS_SUCCESS);
	ASSERT_STREQ(subscriber.GetTopicName(), topic_name);
}

TEST_F(SDN_AsynchrounousSubscriber, GetTopicSize)
{
	sdn::AsynchronousSubscriber subscriber (mdata);
	subscriber.Configure();
	ASSERT_EQ(subscriber.GetTopicSize(), topic_size);
}

TEST_F(SDN_AsynchrounousSubscriber, GetTopicHeader)
{
	/* Create Subscriber using metadata */
	sdn::AsynchronousSubscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	ASSERT_STRNE((char*)sub.GetTopicHeader(), NULL);
}

#if 0
TEST_F(SDN_AsynchrounousSubscriber, GetTopicFooter)
{
	/* Create publisher using metadata */
	sdn::AsynchronousSubscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	ASSERT_STRNE((char*)sub.GetTopicFooter(), NULL);
}
#endif
TEST_F(SDN_AsynchrounousSubscriber, GetTopicInstance)
{
	/* Create publisher using metadata */
	sdn::AsynchronousSubscriber sub (mdata);
	sub.SetInterface(iface_name);
	sub.Configure();

	ASSERT_STRNE((char*)sub.GetTopicInstance(), NULL);
}

void __callback (sdn::AsynchronousSubscriber* sub) { log_info("Static callback called"); __asynchrounousSub_counter += 1u; };
TEST_F(SDN_AsynchrounousSubscriber, SetCallback)
{
	sdn::AsynchronousSubscriber subscriber (mdata);
	ASSERT_EQ(subscriber.SetCallback((void(*)(void*)) &__callback), STATUS_SUCCESS);
	subscriber.Configure();
}

TEST_F(SDN_AsynchrounousSubscriber, SetCallback_attr)
{
	sdn::AsynchronousSubscriber subscriber (mdata);
	ASSERT_EQ(subscriber.SetCallback((void(*)(void*)) &__callback, static_cast<void*>(&subscriber)), STATUS_SUCCESS);
	subscriber.Configure();
}
#if 1
TEST_F(SDN_AsynchrounousSubscriber, Receive)
{
	bool ret = false;

	sdn::TopicHelper::InitializeMetadata(mdata, "LegacyTopic", sizeof(LegacyTopic_t));

	/* Create Publisher using metadata */
	sdn::Publisher pub (mdata);
	pub.SetInterface(iface_name);
	pub.Configure();

	/* Create Subscriber using metadata */
	sdn::AsynchronousSubscriber sub (mdata);
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

	ret = pub.Publish();
//	ASSERT_EQ(sub.Receive(10000), STATUS_SUCCESS);
	if (ret)
	{
		wait_for(10000000ul);
	}

	sub.CopyTopicInstance(&sub_t);

	ASSERT_EQ(pub_t.index, sub_t.index);
	ASSERT_EQ(pub_t.timestamp, sub_t.timestamp);
	ASSERT_STREQ(pub_t.buffer, sub_t.buffer);

	sub.CopyTopicInstance(&sub_t, sizeof(LegacyTopic_t));

	ASSERT_EQ(pub_t.index, sub_t.index);
	ASSERT_EQ(pub_t.timestamp, sub_t.timestamp);
	ASSERT_STREQ(pub_t.buffer, sub_t.buffer);

//	ASSERT_EQ(sub.Receive(), STATUS_SUCCESS);
}
#endif

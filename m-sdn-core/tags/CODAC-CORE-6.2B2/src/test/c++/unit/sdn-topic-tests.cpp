#include <gtest/gtest.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include "sdn-api.h"
#include "sdn-topic.h"

/**************************************************************
 * Publisher test
 **************************************************************/

class SDN_topic
{
	// cppcheck-suppress unusedFunction // GTest fixture
	public:
		
	// cppcheck-suppress unusedFunction // GTest fixture
	sdn::Topic* topic;
	bool Initialise(void) {
		topic = static_cast<sdn::Topic*>(NULL);

		bool ret = ((topic = new sdn::Topic) != static_cast<sdn::Topic*>(NULL));

		if (ret) {
			topic->SetSize(1024u);
			ret = (topic->CreateInstance() == STATUS_SUCCESS);
		}

		return ret;
	}
	virtual ~SDN_topic (void);

	protected:

};

SDN_topic::~SDN_topic (void)
{
	// Release resources
	if (topic != static_cast<sdn::Topic*>(NULL)) {
		delete topic;
		topic = static_cast<sdn::Topic*>(NULL);
	}

	return;
};


TEST(SDN_topic, Topic_Init)
{
	SDN_topic test;

	bool ret = test.Initialise();

	sdn::Metadata_t mdata;
	sdn::TopicHelper::InitializeMetadata(mdata, "sdn://239.0.103.27:10327/topic_all");
	sdn::TopicHelper::InitializeMetadata(mdata, "Bug10327", 64u, "239.0.103.27", 10327);
	sdn::TopicHelper::InitializeMetadata(mdata, "Bug10327", 64u, "239.0.103.27:10327", 10327);
	sdn::TopicHelper::InitializeMetadata(mdata, "Bug10327", 64u, 1u);
	
	ASSERT_EQ(ret, STATUS_SUCCESS);
}

TEST(SDN_topic, Topic_All)
{
	SDN_topic test;

	bool ret = test.Initialise();

	sdn::Metadata_t mdata;
	sdn::TopicHelper::InitializeMetadata(mdata, "sdn://239.0.103.27:10327/topic_all");

	if (ret) {
		(test.topic)->SetSize("1024");
		(test.topic)->SetMetadata(mdata);
		(test.topic)->GetMetadata(mdata);
		(test.topic)->SetDescription("topic-test");
		(test.topic)->SetUID("0x01234567u");
		(test.topic)->SetVersion("0x89abcdefu");
		(test.topic)->SetMCastPort("10327");
	}

	if (ret)
	{
		ret = (sstrncpy(static_cast<char*>((test.topic)->GetInstance()), 
		      "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"/>", 
		      (test.topic)->GetSize()) != static_cast<void*>(NULL));
	}

	if (ret) {
		ret = ((test.topic)->HasDescription() == STATUS_SUCCESS);
	}

	if (ret) {
		const char* desc = NULL;
		desc = (test.topic)->GetDescription();
		ret = ((NULL != desc) && (strcmp(desc, "topic-test") == 0));
	}

	if (ret) {
		char desc[32];
		ret = (test.topic)->GetDescription(desc, 32u);
		ret = (ret && (NULL != desc) && (strcmp(desc, "topic-test") == 0));
	}

	if (ret) {
		ret = (((test.topic)->AddAttribute<uint64_t>(2u, "packet-counter") == STATUS_SUCCESS) && 
	     ((test.topic)->AddAttribute<uint64_t>(3u, "packet-timestamp") == STATUS_SUCCESS));
	}

	sdn::base::AnyType* type_def = static_cast<sdn::base::AnyType*>(NULL);

	if (ret)
	{
		type_def = (test.topic)->GetTypeDefinition();
		ret = (static_cast<sdn::base::AnyType*>(NULL) != type_def);
	}

	if (ret)
	{
		ret = ((type_def->AddExtAttribute("2", "packet-counter", NULL, "samplenb", NULL, NULL) == STATUS_SUCCESS) && 
		 (type_def->AddExtAttribute("3", "packet-timestamp", NULL, "timestamp", NULL, NULL) == STATUS_SUCCESS));
	}

	if (ret)
	{
		ret = ((test.topic)->Configure() == STATUS_SUCCESS);
	}

	if (ret)
	{
		ret = ((test.topic)->IsInitialized() == true);
	}

	const char* attr_name = NULL;
	if (ret) {
		attr_name = (test.topic)->HasInstanceIndex();
		ret = ((NULL != attr_name) && (strcmp(attr_name, "packet-counter") == 0));
	}

	if (ret)
	{
		attr_name = (test.topic)->HasInstanceTimestamp();
		ret = ((NULL != attr_name) && (strcmp(attr_name, "packet-timestamp") == 0));
	}

/*	if (ret) {
		char buffer [1024];
		ret = (test.topic)->SerializeInstance(buffer, 1024);
	}
*/
	ASSERT_EQ(ret, STATUS_SUCCESS);
}



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

class SDN_header
{
	// cppcheck-suppress unusedFunction // GTest fixture
	public:
		
	// cppcheck-suppress unusedFunction // GTest fixture
	sdn::Header* header;
	bool Initialise(void) {
		header = static_cast<sdn::Header*>(NULL);

		bool ret = ((header = new sdn::Header) != static_cast<sdn::Header*>(NULL));

		if (ret) {
			header->Initialize();
		}

		return ret;
	}
	virtual ~SDN_header (void);

	protected:

};

SDN_header::~SDN_header (void)
{
	// Release resources
	if (header != static_cast<sdn::Header*>(NULL)) {
		delete header;
		header = static_cast<sdn::Header*>(NULL);
	}

	return;
};


TEST(SDN_header, Header_Init)
{
	SDN_header test;

	bool ret = test.Initialise();

	ASSERT_EQ(ret, STATUS_SUCCESS);
}



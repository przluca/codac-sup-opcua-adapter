#include <gtest/gtest.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include "sdn-api.h"
#include "disc-messenger.h"
#include "core-participant.h"

class SDN_disc_messenger
{
	public:
		
    sdn::disc::MessengerImpl* msg;

	bool Initialise (void) {
		// Initialise attributes
		msg = static_cast<sdn::disc::MessengerImpl*>(NULL);

		bool ret = ((msg = new sdn::disc::MessengerImpl) != static_cast<sdn::disc::MessengerImpl*>(NULL));

		return ret;
	}

	bool Initialise_iface (void) {
		// Initialise attributes
		msg = static_cast<sdn::disc::MessengerImpl*>(NULL);

		bool ret = ((msg = new sdn::disc::MessengerImpl("lo")) != static_cast<sdn::disc::MessengerImpl*>(NULL));

		return ret;
	}

	virtual ~SDN_disc_messenger (void);

	protected:

};

SDN_disc_messenger::~SDN_disc_messenger (void)
{
	// Release resources
	if (msg != static_cast<sdn::disc::MessengerImpl*>(NULL)) {
		delete msg;
		msg = static_cast<sdn::disc::MessengerImpl*>(NULL);
	}

	return;
};


TEST(SDN_disc_messenger, Configure)
{
	SDN_disc_messenger test;

	bool ret = test.Initialise();

	if (ret) {
		ret = ((test.msg)->Configure() == STATUS_SUCCESS);
	}

	ASSERT_EQ(ret, STATUS_SUCCESS);
}
/*
TEST(SDN_disc_messenger, Receive)
{
	SDN_disc_messenger test;
	sdn::core::ParticipantImpl* m_base = static_cast<sdn::core::ParticipantImpl*>(NULL);
		
	
	bool ret = test.Initialise_iface();

	if (ret) {
		ret = ((test.msg)->Configure() == STATUS_SUCCESS);
	}

	if (ret) {
		ret = ((test.msg)->Register(m_base) == STATUS_SUCCESS);
	}

	if (ret) {
		ret = ((test.msg)->Receive(1u) == STATUS_SUCCESS);
	}

	ASSERT_EQ(ret, STATUS_SUCCESS);
}
*/


#include <gtest/gtest.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */
#include <stdarg.h> /* va_start, etc. */
#include <signal.h> /* sigset, etc. */

#include "sdn-api.h"
#include "sdn-packet.h"

class SDN_packet
{
	// cppcheck-suppress unusedFunction // GTest fixture
	public:
		
	sdn::Packet* packet;
	bool Initialise(void) {
		packet = static_cast<sdn::Packet*>(NULL);

		bool ret = ((packet = new sdn::Packet) != static_cast<sdn::Packet*>(NULL));

		if (ret) {
			ret = (packet->CreateInstance() == STATUS_SUCCESS);
		}

		return ret;
	}
	virtual ~SDN_packet (void);

	protected:

};

SDN_packet::~SDN_packet (void)
{
	// Release resources
	if (packet != static_cast<sdn::Packet*>(NULL)) {
		delete packet;
		packet = static_cast<sdn::Packet*>(NULL);
	}

	return;
};


TEST(SDN_packet, Packet_all)
{
    char buffer [256]; 
	SDN_packet test;

	bool ret = test.Initialise();

    (test.packet)->SerializeInstance((char*) buffer, 256);

	ASSERT_EQ(ret, STATUS_SUCCESS);
}


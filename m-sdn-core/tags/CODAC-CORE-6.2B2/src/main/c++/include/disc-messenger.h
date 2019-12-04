#ifndef DISC_MESSENGER_H
#define DISC_MESSENGER_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/disc-messenger.h $
* $Id: disc-messenger.h 100239 2019-06-23 15:40:27Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: SDN - Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*		  CS 90 046
*		  13067 St. Paul-lez-Durance Cedex
*		  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/* Global header files */

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */

#include "mcast-api.h"

#include "core-participant.h"
#include "disc-participant.h"

#ifdef __cplusplus

/* Constants */

#define OBJTYPE_DISC_MESSENGER (char*) "sdn::disc::MessengerImpl"

/* Type definition */

namespace sdn {

namespace disc {

typedef class MessengerImpl : public Participant
{

  private:

    ccs::mcast::Messenger* m_base; /* Publish/receive on discovery channel */
    sdn::core::ParticipantImpl* m_participant;

    /* Initializer methods */
    void Initialize (void);

  public:

    /* Initializer methods */

    /* Accessor methods */
    sdn::core::ParticipantImpl* GetParticipant (void) { return this->m_participant; };

    /* Miscellaneous methods */
    RET_STATUS Configure (void); /* Specializes virtual method */
    RET_STATUS Do (void) { return this->Receive(0ul); }; /* Specializes virtual method */
    RET_STATUS Register (sdn::core::ParticipantImpl* ref);
    RET_STATUS Remove (sdn::core::ParticipantImpl* ref);

    RET_STATUS Publish (void);
    RET_STATUS Receive (void);
    RET_STATUS Receive (const uint64_t timeout);

    bool IsQuery(const char* p_msg);

    /* Constructor methods */
    MessengerImpl (void);
    MessengerImpl (char* iface);

    /* Destructor method */
    virtual ~MessengerImpl (void);

} Messenger;

/* Global variables */

/* Function declaration */

/* Function definition */

} /* namespace disc */

} /* namespace sdn */

extern "C" {

#endif /* __cplusplus */

/* ToDo - Insert C API declaration */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DISC_MESSENGER_H */

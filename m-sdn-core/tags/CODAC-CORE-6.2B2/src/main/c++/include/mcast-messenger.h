#ifndef MCAST_MESSENGER_H
#define MCAST_MESSENGER_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/mcast-messenger.h $
* $Id: mcast-messenger.h 100239 2019-06-23 15:40:27Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
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

#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */

#include "mcast-participant.h"

/* Constants */

#define OBJTYPE_MCAST_MESSENGER "ccs::mcast::MessengerImpl"

#ifdef __cplusplus

/* Type definition */

namespace ccs {

namespace mcast {

class MessengerImpl : public ParticipantImpl /* Implementation class */
{

  private:

    char   m_reply_addr [MaxIPv4AddrLength];
    uint_t m_reply_port;

    volatile bool m_blocking;
    uint64_t m_timeout;
    uint32_t m_uid;

    volatile bool m_updated; /* Buffer has been updated */

    /* Initializer methods */
    void Initialize (void) { this->m_blocking = false; this->m_timeout = 0lu; this->m_uid = 0u; this->m_updated = false; return; };

  public:

    /* Initializer methods */

    /* Accessor methods */
    uint64_t GetTimeout (void) const { return this->m_timeout; };
    void SetTimeout (const uint64_t timeout) { this->m_blocking = true; this->m_timeout = timeout; return; };

    bool IsUpdated (void) { return __sync_val_compare_and_swap(&this->m_updated, true, true); }; /* Successful Receive */

    /* Miscellaneous methods */
    virtual RET_STATUS Do (void) { return this->Receive(); };

    RET_STATUS Open (void);
    RET_STATUS Close (void);

    RET_STATUS Publish (const void* buffer, const uint_t size);
    RET_STATUS Publish (void);

    RET_STATUS Receive (void* buffer, uint_t& size, const uint64_t timeout); /* Blocking wait with configurable timeout - Size updated with actual number of bytes received */
    RET_STATUS Receive (void* buffer, uint_t& size);                         /* Blocking wait - Size updated with actual number of bytes received */

    RET_STATUS Receive (const uint64_t timeout); /* Blocking wait with configurable timeout - Buffer and size defined using sdn::mcast::Participant::SetBuffer() */
    RET_STATUS Receive (void);                   /* Blocking wait - Buffer and size defined using sdn::mcast::Participant::SetBuffer() */

    RET_STATUS Reply (const void* buffer, const uint_t size);

    /* Constructor methods */
    MessengerImpl (void);
    MessengerImpl (const char* iface, const char* group, const uint_t port);

    /* Destructor method */
    virtual ~MessengerImpl (void);

};

/* Global variables */

/* Function declaration */

/* Function definition */

} /* namespace mcast */

} /* namespace ccs */

extern "C" {

#endif /* __cplusplus */

/* ToDo - Insert C API declaration */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MCAST_MESSENGER_H */
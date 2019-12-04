#ifndef MCAST_PUBLISHER_H
#define MCAST_PUBLISHER_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/mcast-publisher.h $
* $Id: mcast-publisher.h 100239 2019-06-23 15:40:27Z bauvirb $
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

#define OBJTYPE_MCAST_PUBLISHER "ccs::mcast::PublisherImpl"

#ifdef __cplusplus

/* Type definition */

namespace ccs {

namespace mcast {

class PublisherImpl : public ParticipantImpl /* Implementation class */
{

  private:

    uint_t m_source_port;

    /* Initializer methods */
    void Initialize (void) { this->m_source_port = 0u; };

  public:

    /* Initializer methods */

    /* Accessor methods */
    void SetSourcePort (const uint_t port) { this->m_source_port = port; }; /* Bug 10809 - Publisher-side port */

    /* Miscellaneous methods */
    virtual RET_STATUS Do (void) { return this->Publish(); };

    RET_STATUS Open (void);  
    RET_STATUS Close (void); 

    RET_STATUS Publish (const void* buffer, const uint_t size);
    RET_STATUS Publish (void);

    /* Constructor methods */
    PublisherImpl (void);
    PublisherImpl (const char* iface, const char* group, const uint_t port);

    /* Destructor method */
    virtual ~PublisherImpl (void); /* Note - Virtual destructor */

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

#endif /* MCAST_PUBLISHER_H */

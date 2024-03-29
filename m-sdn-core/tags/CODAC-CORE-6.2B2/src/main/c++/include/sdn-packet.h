#ifndef SDN_PACKET_H
#define SDN_PACKET_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/sdn-packet.h $
* $Id: sdn-packet.h 96477 2019-01-09 14:53:55Z sivecm $
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

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */

#include "sdn-msg.h"
#include "sdn-header.h"
#include "sdn-footer.h"

/* Constants */

#define DEFAULT_PAYLOAD_SIZE 1024
#if 0 // Scoped enums put requirements on compiler used to include this header file
typedef enum SDNPacket_AttributeId : uint_t {
#else
typedef enum SDNPacket_AttributeId {
#endif
  SDN_PACKET_HEADER = 0,
  SDN_PACKET_CORPUS,
  SDN_PACKET_FOOTER,

  SDN_PACKET_UNDEFINED

} SDNPacket_AttributeId_t;

#define SDNPacket sdn::Packet /* For backward compatibility purposes with v1.0 */
#define sdn_packet sdn::Packet /* For backward compatibility purposes with v1.0 */

/* Type definition */

namespace sdn {

class Packet : public Message
{

  private:

    ccs::base::AnyType* m_buffer; /* Raw allocated memory buffer which is split in regions: */
    Header*  m_header;            /*   - Header,                                            */
    ccs::base::AnyType* m_corpus; /*   - Body, payload, topic                               */
    Footer*  m_footer;            /*   - Footer, if any                                     */

    /* Initializer methods */
    void Initialize (uint_t size); /* Define payload by size */

  public:

    /* Initializer methods */
    void ClearInstance (void);
    RET_STATUS CreateInstance (void);

    /* Accessor methods */
    void SetInstance (void* instance);

    Header* GetHeader (void) { return this->m_header; };
    Footer* GetFooter (void) { return this->m_footer; };

    void* GetPayload (void) { return (void*) (this->m_corpus)->GetInstance(); };
    void GetPayload (void* ref) { memcpy(ref, (this->m_corpus)->GetInstance(), (this->m_corpus)->GetSize()); };
    void SetPayload (void* ref) { memcpy((this->m_corpus)->GetInstance(), ref, (this->m_corpus)->GetSize()); };

    uint_t GetHeaderSize (void)  { uint_t size = 0; if (this->m_header != NULL) size = (this->m_header)->GetSize(); return size; };
    uint_t GetPayloadSize (void) { uint_t size = 0; if (this->m_corpus != NULL) size = (this->m_corpus)->GetSize(); return size; };
    uint_t GetFooterSize (void)  { uint_t size = 0; if (this->m_footer != NULL) size = (this->m_footer)->GetSize(); return size; };

    /* Miscellaneous methods */

    /* Constructor methods */
    Packet (void);
    Packet (uint_t size); /* Define payload by size */

    /* Destructor method */
   ~Packet (void);

    /* Display methods */
    uint16_t SerializeType (char* buffer, int max_size = STRING_MAX_LENGTH);
    void SerializeInstance (char* buffer, int max_size = STRING_MAX_LENGTH);

};

/* Global variables */

/* Function declaration */

/* Function definition */

} /* namespace sdn */

#endif /* SDN_PACKET_H */

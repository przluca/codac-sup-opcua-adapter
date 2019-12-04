#ifndef SDN_HEADER_H
#define SDN_HEADER_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/sdn-header.h $
* $Id: sdn-header.h 96477 2019-01-09 14:53:55Z sivecm $
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

//#include "sdn-base.h" /* Privately scoped case classes definition */
#include "any-type-ext.h" /* Extended type definition, description, units, etc. */

/* Constants */

#define DEFAULT_SDN_HEADER_UID (char*) "SDN"

typedef enum SDNHeader_AttributeId {

  SDN_HEADER_HEADER_UID = 0,
  SDN_HEADER_HEADER_VERSION,
  SDN_HEADER_HEADER_SIZE,
#if 0 /* Revert on Bug 10327 */
  SDN_HEADER_HEADER_RESERVED,
#endif
  SDN_HEADER_TOPIC_UID,
  SDN_HEADER_TOPIC_VERSION,
  SDN_HEADER_TOPIC_SIZE,
#if 0 /* Revert on Bug 10327 */
  SDN_HEADER_TOPIC_RESERVED,
#endif
  SDN_HEADER_COUNTER,
  SDN_HEADER_SEND_TIME,
  SDN_HEADER_RECV_TIME

} SDNHeader_AttributeId_t;

/* Type definition */

namespace sdn {

typedef struct {

  char     header_uid [4];
  char     header_version [4];
#if 0 /* Revert on Bug 10327 */
  uint16_t header_size;
  uint16_t _h_reserved__;
#else
  uint32_t header_size;
#endif
  uint32_t topic_uid;
  uint32_t topic_version;
#if 0 /* Revert on Bug 10327 */
  uint16_t topic_size;
  uint16_t _t_reserved__;
#else
  uint32_t topic_size;
#endif
  uint64_t topic_counter;
  uint64_t send_time;
  uint64_t recv_time;

} Header_t;

class Header : public sdn::base::AnyType
{

  private:

  public:

    /* Initializer methods */
    void Initialize (void);

    /* Accessor methods */
    const char* HasInstanceIndex (void) const;
    const char* HasInstanceTimestamp (void) const;

    bool IsValid (const void* buffer);
    bool IsValid (void) { return this->IsValid(this->AnyType::GetInstance()); };

    Header_t* GetInstance (void) { return (Header_t*) this->AnyType::GetInstance(); };
    void SetInstance (void* instance) { this->AnyType::SetInstance(instance); this->Initialize(); return; };

    uint64_t GetCounter (void) { uint64_t* attr = (uint64_t*) this->AnyType::GetAttributeReference(SDN_HEADER_COUNTER); return *attr; };
    void SetCounter (uint64_t counter) { uint64_t* attr = (uint64_t*) this->AnyType::GetAttributeReference(SDN_HEADER_COUNTER); *attr = counter; return; };
    void IncrCounter (void) { uint64_t* attr = (uint64_t*) this->AnyType::GetAttributeReference(SDN_HEADER_COUNTER); *attr += 1; return; };

    uint64_t GetTimestamp (void) { uint64_t* attr = (uint64_t*) this->AnyType::GetAttributeReference(SDN_HEADER_SEND_TIME); return *attr; };
    uint64_t GetReceiveTimestamp (void) { uint64_t* attr = (uint64_t*) this->AnyType::GetAttributeReference(SDN_HEADER_RECV_TIME); return *attr; };
    void SetTimestamp (uint64_t time) { uint64_t* attr = (uint64_t*) this->AnyType::GetAttributeReference(SDN_HEADER_SEND_TIME); *attr = time; return; };
    void SetReceiveTimestamp (uint64_t time) { uint64_t* attr = (uint64_t*) this->AnyType::GetAttributeReference(SDN_HEADER_RECV_TIME); *attr = time; return; };
    void SetTimestamp (void) { return this->SetTimestamp(get_time()); };
    void SetReceiveTimestamp (void) { return this->SetReceiveTimestamp(get_time()); };
#if 0 /* Revert on Bug 10327 */
    uint16_t GetTopicSize (void) { uint16_t* attr = (uint16_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_SIZE); return *attr; };
    void SetTopicSize (uint16_t size) { uint16_t* attr = (uint16_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_SIZE); *attr = size; return; };
#else
    uint32_t GetTopicSize (void) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_SIZE); return *attr; };
    void SetTopicSize (uint32_t size) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_SIZE); *attr = size; return; };
#endif

    uint32_t GetTopicUID (void) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_UID); return *attr; };
    void SetTopicUID (uint32_t uid) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_UID); *attr = (uint32_t) uid; return; };

    //char* GetTopicVersion (void) { return (char*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_VERSION); };
    uint32_t GetTopicVersion (void) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_VERSION); return *attr; };

    void SetTopicVersion (char* version);
    void SetTopicVersion (uint32_t version);

    /* Miscellaneous methods */
    void ClearInstance (void);
    void UpdateInstance (void) { this->SetTimestamp(); this->IncrCounter(); return; };

    void FromNetworkByteOrder (void); 
    void ToNetworkByteOrder (void);

    bool IsPayloadOrdered (void) { Header_t* ref = this->GetInstance(); return ((ref->header_size == this->AnyType::GetSize()) ? true : false); }; // Bug 10327 - Same ordering as host

    /* Constructor methods */
    Header (void);

    /* Destructor method */
    virtual ~Header (void) { /* Nothing further */ };

    /* Display methods */

};

/* Global variables */

/* Function declaration */

/* Function definition */

} /* namespace sdn */

#endif /* SDN_HEADER_H */

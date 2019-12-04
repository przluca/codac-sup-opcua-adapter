#ifndef SDN_FOOTER_H
#define SDN_FOOTER_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/sdn-footer.h $
* $Id: sdn-footer.h 96477 2019-01-09 14:53:55Z sivecm $
*
* Project	: CODAC Core System
*
* Description	: SDN - Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
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

#define DEFAULT_SDN_FOOTER_UID (char*) "SDN"
#if 0 // Scoped enums put requirements on compiler used to include this header file
typedef enum SDNFooter_AttributeId : uint_t {
#else
typedef enum SDNFooter_AttributeId  {
#endif
  SDN_FOOTER_FOOTER_UID = 0,
  SDN_FOOTER_FOOTER_SIZE,
  SDN_FOOTER_SOURCE_UID,
  SDN_FOOTER_TOPIC_CRC

} SDNFooter_AttributeId_t;

#define sdn_footer sdn::Footer /* For backward compatibility purposes with v1.0 */
#define SDNFooter_t sdn::Footer_t /* For backward compatibility purposes with v1.0 */

/* Type definition */

namespace sdn {

typedef struct {

  char     footer_uid [8];
  uint32_t footer_size;
  uint32_t source_uid;
  uint32_t topic_crc;
  //char     reserved [8];

} Footer_t;

class Footer : public ccs::base::AnyType
{

  private:

  public:

    /* Initializer methods */
    void Initialize (void);

    /* Accessor methods */
    bool IsValid (void* buffer) { uint_t offset = this->AnyType::GetAttributeOffset(SDN_FOOTER_FOOTER_UID); return ((strncmp((((char*) buffer) + offset), DEFAULT_SDN_FOOTER_UID, strlen(DEFAULT_SDN_FOOTER_UID)) == 0) ? true : false); };
    bool IsValid (void) { return this->IsValid(this->AnyType::GetInstance()); };

    SDNFooter_t* GetInstance (void) { return (SDNFooter_t*) this->AnyType::GetInstance(); };
    void SetInstance (void* instance) { this->AnyType::SetInstance(instance); this->Initialize(); return; };

    uint_t GetSourceUID (void) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_FOOTER_SOURCE_UID); return *attr; };
    void SetSourceUID (uint_t uid) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_FOOTER_SOURCE_UID); *attr = (uint32_t) uid; return; };

    uint_t GetTopicCRC (void) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_FOOTER_TOPIC_CRC); return *attr; };
    void SetTopicCRC (uint_t crc) { uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_FOOTER_TOPIC_CRC); *attr = (uint32_t) crc; return; };

    /* Miscellaneous methods */

    /* Constructor methods */
    Footer (void);

    /* Destructor method */
   ~Footer (void) { /* Nothing further */ };

    /* Display methods */

};

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initializer methods */

/* Accessor methods */

/* Miscellaneous methods */

/* Constructor methods */

/* Destructor method */

} /* namespace sdn */

#endif
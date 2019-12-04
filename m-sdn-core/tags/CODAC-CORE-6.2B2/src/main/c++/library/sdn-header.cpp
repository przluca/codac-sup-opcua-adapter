/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/sdn-header.cpp $
* $Id: sdn-header.cpp 96477 2019-01-09 14:53:55Z sivecm $
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
#include "sdn-tools.h" /* Misc. helper functions, e.g. hash, etc. */

#include "sdn-base.h" /* Privately scoped base classes definition */

#include "sdn-header.h" /* This class definition */

/* Constants */

/* Type definition */

namespace sdn {

/* Global variables */

static ccs::base::AttrInfo_t Header_attributes_list [] = {

  /* Attributes */
  /* Header UID      */ { "header_uid",     "char",     "4", "SDN" }, 
  /* Header version  */ { "header_version", "char",     "4", "2.x" }, 
#if 0 /* Revert on Bug 10327 */
  /* Header size     */ { "header_size",    "uint16_t", "1", "48" }, 
                        { "_h_reserved__",  "uint16_t", "1", "0" }, 
#else
  /* Header size     */ { "header_size",    "uint32_t", "1", "48" }, 
#endif
  /* Topic UID       */ { "topic_uid",      "uint32_t", "1", "0" }, 
  /* Topic version   */ { "topic_version",  "uint32_t", "1", "0" }, 
#if 0 /* Revert on Bug 10327 */
  /* Topic size      */ { "topic_size",     "uint16_t", "1", "0" }, 
                        { "_t_reserved__",  "uint16_t", "1", "0" }, 
#else
  /* Topic size      */ { "topic_size",     "uint32_t", "1", "0" }, 
#endif
  /* Topic counter   */ { "topic_counter",  "uint64_t", "1", "0" }, 
  /* Topic timestamp */ { "send_time",      "uint64_t", "1", "0" }, 
  /* Topic timestamp */ { "recv_time",      "uint64_t", "1", "0" }, 

  { STRING_UNDEFINED, STRING_UNDEFINED, STRING_UNDEFINED, STRING_UNDEFINED }

};

/* Function declaration */

/* Function definition */

/* Initializer methods */

void Header::Initialize (void) 
{ 

  if (this->AnyType::GetInstance() == NULL) return;

  void* attr = NULL;

  /*
   * Bug 10327 - Static header attribute stored in native byte ordering in order to be able
   *             to detect publisher endianness.
   */
#if 0 /* Bug 10419 - Forward compatibility issue - Legacy subscribers expect a 4bytes UID */
  /* Header UID      */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_UID);      sstrncpy((char*) attr, (char*) "SDN", 4);
#else
  /* Header UID      */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_UID);      *((uint32_t*) attr) = 0x764E4453; /* SDNv string */
#endif
  /* Header version  */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_VERSION);  sstrncpy((char*) attr, (char*) "2.x", 4);
#if 0 /* Revert on Bug 10327 */
  /* Header size     */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_SIZE);     *((uint16_t*) attr) = static_cast<uint16_t>(this->AnyType::GetSize()); 
                        attr = this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_RESERVED); *((uint16_t*) attr) = 0u; 
#else
  /* Header size     */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_SIZE);     *((uint32_t*) attr) = this->AnyType::GetSize(); 
#endif
  /* Topic UID       */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_UID);       *((uint32_t*) attr) = 0u;
  /* Topic version   */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_VERSION);   *((uint32_t*) attr) = 0u;
#if 0 /* Revert on Bug 10327 */
  /* Topic size      */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_SIZE);      *((uint16_t*) attr) = 0u;
                        attr = this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_RESERVED);  *((uint16_t*) attr) = 0u;
#else
  /* Topic size      */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_SIZE);      *((uint32_t*) attr) = 0u;
#endif
  /* Topic counter   */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_COUNTER);         *((uint64_t*) attr) = 0ul;
  /* Topic timestamp */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_SEND_TIME);       *((uint64_t*) attr) = 0ul;
  /* Topic timestamp */ attr = this->AnyType::GetAttributeReference(SDN_HEADER_RECV_TIME);       *((uint64_t*) attr) = 0ul;

  return; 

}

/* Accessor methods */

const char* Header::HasInstanceIndex (void) const
{ 

  const char* p_attr = NULL;

  p_attr = this->AnyType::GetAttributeName((uint_t) SDN_HEADER_COUNTER);

  return p_attr; 

}

const char* Header::HasInstanceTimestamp (void) const
{ 

  const char* p_attr = NULL;

  p_attr = this->AnyType::GetAttributeName((uint_t) SDN_HEADER_SEND_TIME);

  return p_attr; 

}

bool Header::IsValid (const void* buffer) 
{ 

  uint_t offset = this->AnyType::GetAttributeOffset(SDN_HEADER_HEADER_UID); 
  return ((strncmp((((char*) buffer) + offset), DEFAULT_SDN_HEADER_UID, strlen(DEFAULT_SDN_HEADER_UID)) == 0) ? true : false); 

}

void Header::SetTopicVersion (char* version) 
{ 

  uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_VERSION);
  uint32_t vers = 0;

  sscanf(version, "%u", &vers); /* Parse version string as integer */

  *attr = vers;

  return; 

}

void Header::SetTopicVersion (uint32_t version) 
{ 
  uint32_t* attr = (uint32_t*) this->AnyType::GetAttributeReference(SDN_HEADER_TOPIC_VERSION);

  *attr = version;

  return; 

}

/* Miscellaneous methods */

void Header::ClearInstance (void) 
{ 

  log_debug("Header::ClearInstance - Invoke AnyType::ClearInstance method");
  this->AnyType::ClearInstance(); 

  /* Initialize header instance */ 
  this->Initialize(); 

  return; 

}

void Header::FromNetworkByteOrder (void)
{ 

  uint32_t* attr = static_cast<uint32_t*>(this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_SIZE));
  *attr = HelperTools::FromNetworkByteOrder(*attr);

  this->SetTopicUID(HelperTools::FromNetworkByteOrder(this->GetTopicUID()));
  this->SetTopicVersion(HelperTools::FromNetworkByteOrder(this->GetTopicVersion()));
  this->SetTopicSize(HelperTools::FromNetworkByteOrder(this->GetTopicSize()));
  this->SetCounter(HelperTools::FromNetworkByteOrder(this->GetCounter()));
  this->SetTimestamp(HelperTools::FromNetworkByteOrder(this->GetTimestamp()));

  return; 

}

void Header::ToNetworkByteOrder (void)
{

  uint32_t* attr = static_cast<uint32_t*>(this->AnyType::GetAttributeReference(SDN_HEADER_HEADER_SIZE));
  *attr = HelperTools::ToNetworkByteOrder(*attr);

  this->SetTopicUID(HelperTools::ToNetworkByteOrder(this->GetTopicUID()));
  this->SetTopicVersion(HelperTools::ToNetworkByteOrder(this->GetTopicVersion()));
  this->SetTopicSize(HelperTools::ToNetworkByteOrder(this->GetTopicSize()));
  this->SetCounter(HelperTools::ToNetworkByteOrder(this->GetCounter()));
  this->SetTimestamp(HelperTools::ToNetworkByteOrder(this->GetTimestamp()));

  return;

}

/* Constructor methods */
 
Header::Header (void)
{ 

  /* Declare attributes */
  uint_t index = 0u; 

  while (!IsUndefined(Header_attributes_list[index].name))
    {
      this->ccs::base::AnyType::AddAttribute(STRING_UNDEFINED, Header_attributes_list[index].name, Header_attributes_list[index].type, Header_attributes_list[index].mult, Header_attributes_list[index].deflt); 
      this->sdn::base::AnyType::AddExtAttribute(NULL, Header_attributes_list[index].name, NULL, NULL, NULL, Header_attributes_list[index].mult); 
      index += 1u;
    }

  return; 

}

/* Destructor method */

} /* namespace sdn */

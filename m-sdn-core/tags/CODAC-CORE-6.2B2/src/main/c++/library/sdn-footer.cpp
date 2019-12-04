/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/sdn-footer.cpp $
* $Id: sdn-footer.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-footer.h" /* This class definition */

/* Constants */

/* Type definition */

namespace sdn {

/* Global variables */

static ccs::base::AttrInfo_t Footer_attributes_list [] = {

  /* Attributes */
  /* Footer UID      */ { "footer_uid",    "char",     "8", "SDNv2.x" }, 
  /* Footer size     */ { "footer_size",   "uint32_t", "1", "20" }, 
  /* Source UID      */ { "source_uid",    "uint32_t", "1", "0" }, 
  /* Topic CRC       */ { "topic_crc",     "uint32_t", "1", "0" }, 

  { STRING_UNDEFINED, STRING_UNDEFINED, STRING_UNDEFINED, STRING_UNDEFINED }

};

/* Function declaration */

/* Function definition */

/* Initializer methods */

void Footer::Initialize (void) 
{ 

  if (this->AnyType::GetInstance() == NULL) return;

  void* attr = NULL;

  /* Footer UID      */ attr = this->AnyType::GetAttributeReference(SDN_FOOTER_FOOTER_UID);  sstrncpy((char*) attr, (char*) "SDNv2.x", 8);
  /* Footer size     */ attr = this->AnyType::GetAttributeReference(SDN_FOOTER_FOOTER_SIZE); *((uint32_t*) attr) = this->AnyType::GetSize();
  /* Source UID      */ attr = this->AnyType::GetAttributeReference(SDN_FOOTER_SOURCE_UID);  *((uint32_t*) attr) = 0;
  /* Topic CRC       */ attr = this->AnyType::GetAttributeReference(SDN_FOOTER_TOPIC_CRC);   *((uint32_t*) attr) = 0;

  return; 

}

/* Accessor methods */

/* Miscellaneous methods */

/* Constructor methods */

Footer::Footer (void) : AnyType ((ccs::base::AttrInfo_t*) Footer_attributes_list) { /* Nothing further */ return; }

/* Destructor method */

/* Display methods */

} /* namespace sdn */

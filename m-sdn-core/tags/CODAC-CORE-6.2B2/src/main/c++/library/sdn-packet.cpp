/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/sdn-packet.cpp $
* $Id: sdn-packet.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

//#include "sdn-base.h" /* Privately scoped base classes definition */

#include "sdn-header.h"
#include "sdn-footer.h"

#include "sdn-packet.h" /* This class definition */

/* Constants */

#ifndef UNIT_VERSION
#define UNIT_VERSION UNIT_VERSION_UID(1,0,0)
#endif

#define USE_PACKET_FOOTER
#if UNIT_VERSION < UNIT_VERSION_UID(2,0,0)
#undef USE_PACKET_FOOTER
#endif

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::packet"

/* Type definition */

namespace sdn {

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initializer methods */

void Packet::ClearInstance (void) 
{ 

  log_trace("Packet::ClearInstance - Entering method");

  log_debug("Packet::ClearInstance - Clear buffer"); (this->m_buffer)->ClearInstance();
  log_debug("Packet::ClearInstance - Clear header"); (this->m_header)->ClearInstance();
  log_debug("Packet::ClearInstance - Clear corpus"); (this->m_corpus)->ClearInstance();
#ifdef USE_PACKET_FOOTER
  log_debug("Packet::ClearInstance - Clear footer"); (this->m_footer)->ClearInstance();
#endif
  log_trace("Packet::ClearInstance - Leaving method");

  return; 

}

RET_STATUS Packet::CreateInstance (void) 
{

  log_trace("Packet::CreateInstance - Entering method");

  RET_STATUS status = STATUS_ERROR;

  /* Allocate instance */
  log_debug("Packet::CreateInstance - Instantiate sdn::Message buffer");
  status = this->Message::CreateInstance();

  /* Pass references down */
  log_debug("Packet::CreateInstance - Assign references to various packet sections");
  this->SetInstance(this->Message::GetInstance());

  /* Clear instance */
  log_debug("Packet::CreateInstance - Clear buffer");
  this->ClearInstance();

  log_trace("Packet::CreateInstance - Leaving method");

  return status;

}

void Packet::Initialize (uint_t size)
{

  log_trace("Packet::Initialize - Entering method");

  /* Initialize attributes */
  this->m_header = NULL; this->m_corpus = NULL; this->m_footer = NULL;
  this->m_buffer = new ccs::base::AnyType (); /* Assemble packet type definition */

  /* A complete message is composed of header, body and footer sections */

  /* Load header type definition */
  this->m_header = new Header; (this->m_buffer)->AddAttribute(SDN_PACKET_HEADER, (char*) "header", (this->m_header)->GetSize());

  /* Load payload type definition */
  log_info("Try and define payload by size '%d'", size);
  this->m_corpus = new ccs::base::AnyType (size); (this->m_buffer)->AddAttribute(SDN_PACKET_CORPUS, (char*) "corpus", (this->m_corpus)->GetSize());

  if (((this->m_corpus)->IsInitialized() != true) || ((this->m_corpus)->GetSize() == 0))
    {
      log_warning("Undefined payload datatype definition");
      log_warning("Packet will have no payload to be found");
    }
#ifdef USE_PACKET_FOOTER
  /* Load footer type definition */
  this->m_footer = new Footer; (this->m_buffer)->AddAttribute(SDN_PACKET_FOOTER, (char*) "footer", (this->m_footer)->GetSize());
#endif
  this->SetSize((this->m_buffer)->GetSize()); 

  log_info("Initialized packet of size '%d'", this->GetSize()); 

  log_trace("Packet::Initialize - Leaving method");

  return;

}

/* Accessor methods */

void Packet::SetInstance (void* instance) /* Externally provided instance */
{ 
  
  this->Message::SetInstance(instance); 

  /* Pass reference and compute offset */
  log_debug("Packet::SetInstance - Buffer has reference '%p'", instance);
  (this->m_buffer)->SetInstance(instance); 

  /* Pass references down */
  log_debug("Packet::SetInstance - Header has reference '%p'", (this->m_buffer)->GetAttributeReference(SDN_PACKET_HEADER));
  (this->m_header)->SetInstance((this->m_buffer)->GetAttributeReference(SDN_PACKET_HEADER));

  log_debug("Packet::SetInstance - Corpus has reference '%p'", (this->m_buffer)->GetAttributeReference(SDN_PACKET_CORPUS));
  (this->m_corpus)->SetInstance((this->m_buffer)->GetAttributeReference(SDN_PACKET_CORPUS));
#ifdef USE_PACKET_FOOTER
  log_debug("Packet::SetInstance - Footer has reference '%p'", (this->m_buffer)->GetAttributeReference(SDN_PACKET_FOOTER));
  (this->m_footer)->SetInstance((this->m_buffer)->GetAttributeReference(SDN_PACKET_FOOTER));
#endif
  return;

}

/* Miscellaneous methods */

/* Constructor methods */

Packet::Packet (void)
{

  /* Initialize attributes */

  /* Initialize topic instance */
  this->Initialize(DEFAULT_PAYLOAD_SIZE);

  return;

}

Packet::Packet (uint_t size) /* Payload size */
{

  /* Initialize attributes */

  /* Initialize topic instance */
  this->Initialize(size);

  return;

}

/* Destructor method */

Packet::~Packet (void)
{

  /* Delete topic instance, if necessary */
  this->DeleteInstance();

  return;

}

/* Display methods */

uint16_t Packet::SerializeType (char* buffer, int max_size) { return (this->m_buffer)->SerializeType(buffer, max_size); }
void Packet::SerializeInstance (char* buffer, int max_size) 
{

  char* p_buf = buffer;

  /* WARNING - Check if size could be negative - yes in case snprintf truncates ... does not append zero  ... and the buffer is not cleared */

  /* Zero the buffer */
  memset(buffer, 0, max_size); max_size -= 1; /* Make sure the buffer is zero'ed (and the trailing zero won't get overwrittent) so as to avoid strlen failure in case snprintf truncates */

  (this->m_header)->SerializeInstance(p_buf, max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

  snprintf(p_buf, max_size, ", uint8 payload [%d]", (this->m_corpus)->GetSize()); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
#ifdef USE_PACKET_FOOTER
  if ((this->m_footer)->IsValid() == true)
    {
      sstrncpy(p_buf, (char*) ", ", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      (this->m_footer)->SerializeInstance(p_buf, max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }
#endif
  return;

}

} /* namespace sdn */

#undef LOG_ALTERN_SRC

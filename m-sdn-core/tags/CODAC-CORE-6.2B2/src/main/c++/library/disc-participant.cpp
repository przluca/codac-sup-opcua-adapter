/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/disc-participant.cpp $
* $Id: disc-participant.cpp 96477 2019-01-09 14:53:55Z sivecm $
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
#include "sdn-tools.h" /* Misc. helper functions, e.g. hash, etc. */

#include "sdn-base.h" /* Privately scoped base classes definition */

#include "disc-participant.h" /* This class definition */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::disc"

/* Type definition */

namespace sdn {

namespace disc {

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initializer methods */

void Participant::Initialize (void)
{

  log_trace("Participant::Initialize - Entering method"); 

  /* Initialize attributes */
  sstrncpy(this->m_if_name, (char*) DEFAULT_IFACE_NAME, MAX_IP_ADDR_LENGTH);
  sstrncpy(this->m_mcast_group, (char*) DEFAULT_DISCOVERY_MCAST_GROUP, MAX_IP_ADDR_LENGTH);
  this->m_mcast_port = DEFAULT_DISCOVERY_MCAST_PORT;

  sstrncpy(this->m_host_name, (char*) STRING_UNDEFINED, STRING_MAX_LENGTH);
  sstrncpy(this->m_prog_name, (char*) STRING_UNDEFINED, STRING_MAX_LENGTH);

  this->m_prog_id = 0;

  this->m_buffer = NULL; this->m_size = DEFAULT_DISCOVERY_MSG_LENGTH;

  /* Try and retrieve interface name from environment variable */
  get_env_variable((char*) "SDN_INTERFACE_NAME", (char*) this->m_if_name, MAX_IP_ADDR_LENGTH); 

  log_trace("Participant::Initialize - Leaving method"); 

  return;

}

/* Accessor methods */

void Participant::SetInterface (char* iface) { sstrncpy((char*) this->m_if_name, iface, MAX_IP_ADDR_LENGTH); return; }
void Participant::SetMCastGroup (char* group) { sstrncpy( (char*) this->m_mcast_group, group, MAX_IP_ADDR_LENGTH); return; }
void Participant::SetMCastPort (uint_t port) { this->m_mcast_port = port; return; }

/* Miscellaneous methods */

RET_STATUS Participant_Configure (Participant* self)
{

  log_trace("Participant_Configure - Entering method");

  RET_STATUS status = STATUS_ERROR;

  if (get_host_name(self->m_host_name) != STATUS_SUCCESS)
    {
      log_warning("Participant_Configure - get_host_name() failed");
    }
  else
    {
      log_debug("Participant_Configure - Host name is '%s'", self->m_host_name);
    }

  if (get_program_name(self->m_prog_name) != STATUS_SUCCESS)
    {
      log_warning("Participant_Configure - get_program_name() failed");
    }
  else
    {
      //strlower(self->m_prog_name); // Bug 10266 - Do not alter program name
      log_debug("Participant_Configure - Program name is '%s'", self->m_prog_name);
    }

  self->m_prog_id = (uint_t) get_process_id();
  log_debug("Participant_Configure - Program pid is '%u'", self->m_prog_id);

  status = STATUS_SUCCESS;

  log_trace("Participant_Configure - Leaving method");

  return status;

}

bool Participant_IsQuery (const char* p_msg)
{

  bool status = false;

  char* p_buf = (char*) p_msg;

#define DISC_QUERY_KEYWORD "qualifier=\"query\""

  while (*p_buf != 0)
    {
      if (strncmp(p_buf, DISC_QUERY_KEYWORD, strlen(DISC_QUERY_KEYWORD)) == 0)
	{
	  status = true;
	  break;
	}

      p_buf += 1;
    }

  return status;

}

/* Constructor methods */

Participant::Participant (void)
{

  log_trace("Participant::Participant - Entering method"); 

  /* Initialize attributes */
  this->Initialize();

  /* Acquire resources */
  this->m_buffer = new uint8_t [this->m_size];

  log_trace("Participant::Participant - Leaving method"); 

  return;

}

/* Destructor method */

Participant::~Participant (void) 
{ 

  log_trace("Participant::~Participant - Entering method"); 

  /* Release resources */
  if (this->m_buffer != NULL) delete [] this->m_buffer; this->m_buffer = NULL; 

  log_trace("Participant::~Participant - Leaving method"); 

  return; 

} 

} /* namespace disc */

} /* namespace sdn */

#undef LOG_ALTERN_SRC

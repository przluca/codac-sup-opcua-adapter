/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/sdn-msg.cpp $
* $Id: sdn-msg.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include "sdn-msg.h" /* This class definition */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::msg"

/* Type definition */

namespace sdn {

namespace message {

/* Global variables */

/* Function declaration */

/* Function definition */

#undef MSG_HEADER
#define MSG_HEADER "<message"
static inline bool IsValid (const char* buffer) { return ((strncmp(buffer, MSG_HEADER, strlen(MSG_HEADER)) == 0) ? true : false); }

#undef MSG_HEADER
#define MSG_HEADER "<message group=\"discovery\""
static inline bool IsDiscoveryMsg (const char* buffer) { return ((strncmp(buffer, MSG_HEADER, strlen(MSG_HEADER)) == 0) ? true : false); }

#undef MSG_HEADER
#define MSG_HEADER "<message group=\"status\""
static inline bool IsStatusMsg (const char* buffer) { return ((strncmp(buffer, MSG_HEADER, strlen(MSG_HEADER)) == 0) ? true : false); }

/* Initializer methods */

RET_STATUS Query::CreateInstance (void) 
{ 

  char msg [1024] = STRING_UNDEFINED;

  if ((IsUndefined(this->m_host_name) || (strcmp(this->m_host_name, "*") == 0)) && (IsUndefined(this->m_topic_name) || (strcmp(this->m_topic_name, "*") == 0)))
    {
#undef QUERY_MSG
#define QUERY_MSG "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"/>"
      sstrncpy((char*) msg, QUERY_MSG, 1024);
    }
  else if (IsUndefined(this->m_host_name) || (strcmp(this->m_host_name, "*") == 0))
    {
#undef QUERY_MSG_FMT
#define QUERY_MSG_FMT "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"><participant><topic name=\"%s\"/></participant></message>"
      snprintf((char*) msg, 1024, QUERY_MSG_FMT, (char*) this->m_topic_name); 
    }
  else
    {
#undef QUERY_MSG_FMT
#define QUERY_MSG_FMT "<message group=\"discovery\" qualifier=\"query\" schemaVersion=\"1.0\"><participant hostName=\"%s\"/></message>"
      snprintf((char*) msg, 1024, QUERY_MSG_FMT, (char*) this->m_host_name); 
    }

  return this->sdn::Message::CreateInstance((void*) msg, strlen(msg)+1); /* Warning - Assume NULL terminated char array */

}

/* Accessor methods */

char* Discovery::GetMsgGroup (void) { return this->m_group; }
char* Discovery::GetQualifier (void) { return this->m_qual; }
char* Discovery::GetHostName (void) { return this->m_host; }
char* Discovery::GetParticipant (void) { return this->m_part; }
char* Discovery::GetRole (void) { return this->m_role; }
char* Discovery::GetTopicName (void) { return this->m_topic; }
uint_t Discovery::GetTopicVersion (void) { uint_t vers = 0; sscanf(this->m_version, "%u", &vers); return vers; }
uint_t Discovery::GetTopicSize (void) { uint_t size = 0; sscanf(this->m_size, "%u", &size); return size; }
char* Discovery::GetMCastGroup (void) { return this->m_mcast_group; }
uint_t Discovery::GetMCastPort (void) { uint_t port = 0; sscanf(this->m_mcast_port, "%u", &port); return port; }

RET_STATUS Query::SetHostName (const char* host) { sstrncpy(m_host_name, host, STRING_MAX_LENGTH); return STATUS_SUCCESS; }
RET_STATUS Query::SetTopicName (const char* topic) { sstrncpy(m_topic_name, topic, STRING_MAX_LENGTH); return STATUS_SUCCESS; }

/* Miscellaneous methods */

bool Discovery::IsValid (void) { return ((this->Parse() != STATUS_SUCCESS) ? false : true); }

bool Discovery::IsJoin (void) { return ((strcmp(this->GetQualifier(), "startup") == 0) ? true : false); }
bool Discovery::IsLeave (void) { return ((strcmp(this->GetQualifier(), "shutdown") == 0) ? true : false); }
bool Discovery::IsUpdate (void) { return ((strcmp(this->GetQualifier(), "update") == 0) ? true : false); }
bool Discovery::IsQuery (void) { return ((strcmp(this->GetQualifier(), "query") == 0) ? true : false); }
bool Discovery::IsResponse (void) { return ((strcmp(this->GetQualifier(), "response") == 0) ? true : false); }
bool Discovery::IsPublisher (void) { return ((strcmp(this->GetRole(), "publisher") == 0) ? true : false); }
bool Discovery::IsSubscriber (void) { return ((strcmp(this->GetRole(), "subscriber") == 0) ? true : false); }

RET_STATUS Discovery::Parse (const char* buffer)
{

  RET_STATUS status = STATUS_ERROR;

  if ((sdn::message::IsValid(buffer) != true) || ((sdn::message::IsDiscoveryMsg(buffer) != true) && (sdn::message::IsStatusMsg(buffer) != true))) 
    {
      log_error("Discovery::Parse - Invalid buffer '%s' received", buffer);
      return status;
    }
  else
    {
      log_debug("Discovery::Parse - Received buffer '%s'", buffer);
    }

  /* Copy message */
  sstrncpy(this->m_copy, buffer, DISCOVERY_MAX_LENGTH);
  char* p_buf = this->m_copy;

  this->m_group = NULL;
  this->m_qual = NULL;
  this->m_host = NULL; /* Host name */
  this->m_part = NULL; /* Process name */
  this->m_role = NULL;
  this->m_topic = NULL;
  this->m_size = NULL;
  this->m_version = NULL;
  this->m_mcast_group = NULL;
  this->m_mcast_port = NULL;

  while (*p_buf != 0)
    {
#undef KEYWORD
#define KEYWORD "group=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_group = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_group); }
#undef KEYWORD
#define KEYWORD "qualifier=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_qual = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_qual); }
#undef KEYWORD
#define KEYWORD "host=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_info("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_host = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_host); }
#undef KEYWORD
#define KEYWORD "hostName=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_info("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_host = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_host); }
#undef KEYWORD
#define KEYWORD "application=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_part = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_part); }
#undef KEYWORD
#define KEYWORD "applicationName=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_part = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_part); }
#undef KEYWORD
#define KEYWORD "role=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_role = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_role); }
#undef KEYWORD
#define KEYWORD "topic name=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_topic = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_topic); }
#undef KEYWORD
#define KEYWORD "size=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_size = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_size); }
#undef KEYWORD
#define KEYWORD "version=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) { log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); p_buf += strlen(KEYWORD); m_version = p_buf; while (*p_buf != '"') p_buf += 1; *p_buf = 0; log_debug("Discovery::Parse - Set value '%s'", m_version); }
#undef KEYWORD
#define KEYWORD "mapping=\""
      if (strncmp(p_buf, KEYWORD, strlen(KEYWORD)) == 0) 
	{ 
	  log_debug("Discovery::Parse - Found keyword '%s'", KEYWORD); 

	  p_buf += strlen(KEYWORD); 
	  m_mcast_group = p_buf; 

	  while (*p_buf != ':') p_buf += 1; *p_buf = 0; 

	  p_buf += 1; 
	  m_mcast_port = p_buf;

	  while (*p_buf != '"') p_buf += 1; *p_buf = 0; 

	  log_debug("Discovery::Parse - Set value '%s:%s'", m_mcast_group, m_mcast_port); 
	}

      p_buf += 1; 
    }

  status = STATUS_SUCCESS;

  return status;

}

/* Constructor methods */

Discovery::Discovery (void) { this->SetSize(DISCOVERY_MAX_LENGTH); memset(this->m_copy, 0, DISCOVERY_MAX_LENGTH); this->m_group = this->m_qual = this->m_host = this->m_part = this->m_role = this->m_topic = this->m_size = NULL; return; }

Query::Query (void) { sstrncpy(this->m_host_name, "*", STRING_MAX_LENGTH); sstrncpy(this->m_topic_name, "*", STRING_MAX_LENGTH); return; }

/* Destructor method */

} /* namespace message */

} /* namespace sdn */

#undef LOG_ALTERN_SRC
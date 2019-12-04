#ifndef CA_WRAP_H
#define CA_WRAP_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/ca/ca-wrap.h $
* $Id: ca-wrap.h 99248 2019-04-23 10:41:47Z bauvirb $
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

/* WARNING - These wrappers require that all those routines be called by the same thread. */

/* Global header files */

#include <cadef.h> /* Channel Access API definition, etc. */

/* Local header files */

#include "types.h" /* Condensed integer type definition, RET_STATUS, etc. */
#include "tools.h" /* Misc. helper functions, e.g. hash, etc. */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ca-wrap"

/* Type definition */

/* Global variables */

/* Public function declaration */

static inline RET_STATUS CAInterface_CreateContext (void) /* CA wrappers */
{

  RET_STATUS status = STATUS_ERROR;

  log_trace("%s - Entering method", __FUNCTION__);
  
  /* Create CA context */
  log_info("Create CA context");
  ca_context_create(ca_disable_preemptive_callback);

  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

};

static inline RET_STATUS CAInterface_ClearContext (void) /* CA wrappers */
{

  RET_STATUS status = STATUS_ERROR;

  log_trace("%s - Entering method", __FUNCTION__);
  
  /* Destroy CA context */
  log_info("Destroy CA context");
  ca_context_destroy();

  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

};

static inline RET_STATUS CAInterface_ConnectVariable (char* name, chid& channel) /* CA wrappers */
{

  RET_STATUS status = STATUS_ERROR;

  log_trace("%s - Entering method", __FUNCTION__);
  
  log_info("Trying and connect to '%s' channel", name);

  /* Connect to channels */
  if (ca_create_channel(name, NULL, NULL, 10, &channel) != ECA_NORMAL)
    {
      log_error("%s - ca_create_channel failed", __FUNCTION__);
      return status;
    }
    
  /* Wait for connections */
  if (ca_pend_io(1) != ECA_NORMAL)
    {
      log_error("%s - ca_pend_io failed", __FUNCTION__);
      return status;
    }

  /* Verify channel */
  if (ca_state(channel) != cs_conn) 
    {
      log_warning("Connection to channel '%s' has not been successful", name);
    }
  else
    {
      log_info("Connection to channel '%s' has been successfully verified", name);
    }

  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

};
 
static inline RET_STATUS CAInterface_DetachVariable (chid channel) /* CA wrappers */
{

  RET_STATUS status = STATUS_ERROR;

  log_trace("%s - Entering method", __FUNCTION__);
  
  /* Connect to channels */
  if (ca_clear_channel(channel) != ECA_NORMAL)
    {
      log_error("%s - ca_clear_channel failed", __FUNCTION__);
      return status;
    }
    
  /* Wait for connections */
  if (ca_pend_io(1) != ECA_NORMAL)
    {
      log_error("%s - ca_pend_io failed", __FUNCTION__);
      return status;
    }

  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

};
 
static inline bool CAInterface_IsConnected (chid channel) { return ((ca_state(channel) != cs_conn) ? false : true); }; /* CA wrappers */

static inline RET_STATUS CAInterface_ReadVariable (chid channel, chtype type, void* p_value) /* CA wrappers */
{

  RET_STATUS status = STATUS_ERROR;

  log_trace("%s - Entering method", __FUNCTION__);

  if (ca_state(channel) != cs_conn) 
    {
      log_error("%s - Connection to channel has been lost", __FUNCTION__);
      return status;
    }

  if (ca_get(type, channel, p_value) != ECA_NORMAL)
    {
      log_error("%s - ca_get failed", __FUNCTION__);
      return status;
    }
  
  /* Flush the requests */
  if (ca_pend_io(1) != ECA_NORMAL)
    {
      log_error("%s - ca_pend_io failed", __FUNCTION__);
      return status;
    }
  
  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

};

static inline RET_STATUS CAInterface_WriteVariable (chid channel, chtype type, void* p_value) /* CA wrappers */
{

  RET_STATUS status = STATUS_ERROR;

  log_trace("%s - Entering method", __FUNCTION__);

  if (ca_state(channel) != cs_conn) 
    {
      log_error("%s - Connection to channel has been lost", __FUNCTION__);
      return status;
    }

  if (ca_put(type, channel, p_value) != ECA_NORMAL)
    {
      log_warning("%s - ca_put failed", __FUNCTION__);
    }

  /* Flush the requests */
  if (ca_pend_io(1) != ECA_NORMAL)
    {
      log_error("%s - ca_pend_io failed", __FUNCTION__);
      return status;
    }
  
  status = STATUS_SUCCESS;

  log_trace("%s - Leaving method", __FUNCTION__);

  return status;

};

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC ""

#endif /* CA_WRAP_H */


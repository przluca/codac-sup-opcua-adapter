#ifndef LOG_WRAP_H
#define LOG_WRAP_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/common/log-wrap.h $
* $Id: log-wrap.h 100718 2019-07-12 11:24:24Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Some useful POSIX wrappers or routines
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/* Global header files */

#include <syslog.h> /* syslog, etc. */

/* Local header files */

/* Constants */

#ifndef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC ""
#endif

#ifndef log_trace
#ifdef LOG_TRACE_ENABLE
#define log_trace(arg_msg, ...) syslog(LOG_DEBUG, "log-lib:%d][%s line %d][trace] " arg_msg, getpid(), __FILE__, __LINE__, ## __VA_ARGS__)
#else
#define log_trace(arg_msg...) {}
#endif
#endif

#ifndef log_debug
#ifdef LOG_DEBUG_ENABLE
#define log_debug(arg_msg, ...) syslog(LOG_DEBUG, "log-lib:%d][%s line %d][debug] " arg_msg, getpid(), __FILE__, __LINE__, ## __VA_ARGS__)
#else
#define log_debug(arg_msg...) {}
#endif
#endif

#ifndef log_info
#define log_info(arg_msg, ...) syslog(LOG_INFO, "log-lib:%d][%s][info] " arg_msg, getpid(), LOG_ALTERN_SRC, ## __VA_ARGS__)
#endif

#ifndef log_notice
#define log_notice(arg_msg, ...) syslog(LOG_NOTICE, "log-lib:%d][%s][notice] " arg_msg, getpid(), LOG_ALTERN_SRC, ## __VA_ARGS__)
#endif

#ifndef log_warning
#define log_warning(arg_msg, ...) syslog(LOG_WARNING, "log-lib:%d][%s][warning] " arg_msg, getpid(), LOG_ALTERN_SRC, ## __VA_ARGS__)
#endif

#ifndef log_error
#define log_error(arg_msg, ...) syslog(LOG_ERR, "log-lib:%d][%s line %d][error] " arg_msg, getpid(), __FILE__, __LINE__, ## __VA_ARGS__)
#endif

#ifndef log_critical
#define log_critical(arg_msg, ...) syslog(LOG_CRIT, "log-lib:%d][%s][critical] " arg_msg, getpid(), LOG_ALTERN_SRC, ## __VA_ARGS__)
#endif

#ifndef log_emergency
#define log_emergency(arg_msg, ...) syslog(LOG_EMERG, "log-lib:%d][%s][emergency] " arg_msg, getpid(), LOG_ALTERN_SRC, ## __VA_ARGS__)
#endif

/* Type definition */

/* Global variables */

/* Function declaration */

/* Function definition */

#endif /* LOG_WRAP_H */


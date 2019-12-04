#ifndef LOG_API_H
#define LOG_API_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/common/log-api.h $
* $Id: log-api.h 100871 2019-07-18 08:40:41Z bauvirb $
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

// Global header files

#include <stdarg.h> // va_list
#include <syslog.h> // Log message severity

// Local header files

#include "BasicTypes.h" // Misc. type definition, e.g. RET_STATUS

// Constants

#ifndef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC ""
#endif

#ifndef LOG_TRACE
#define LOG_TRACE 8 // LOG_DEBUG+1
#endif

#ifndef LOG_ERROR
#define LOG_ERROR LOG_ERR
#endif

#ifndef LOG_EMERGENCY
#define LOG_EMERGENCY LOG_EMERG
#endif

#ifndef LOG_CRITICAL
#define LOG_CRITICAL LOG_CRIT
#endif

#ifndef LOG_CASUAL
#define LOG_CASUAL LOG_INFO
#endif

#ifdef __cplusplus
// Bug 10833 - SWIL-1 components warnings with -Wall -Wpedantic - Issue with __FILE__, etc.
#define __LOG_MSG_0_PARAM(severity, source, message) ccs::log::Message(severity, source, message)
#define __LOG_MSG_1_PARAM(severity, source, message, ...) ccs::log::Message(severity, source, message, ## __VA_ARGS__)
#else
// Bug 11846 - Duplicated symbols with logging library
#define __LOG_MSG_0_PARAM(severity, source, message) ccs_log_msg(severity, source, message)
#define __LOG_MSG_1_PARAM(severity, source, message, ...) ccs_log_msg(severity, source, message, ## __VA_ARGS__)
#endif

#define __LOG_MSG_MACRO_CHOOSER(_sev, _src, _msg, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, MACRO_NAME, ...) MACRO_NAME
#define __LOG_MSG(...) __LOG_MSG_MACRO_CHOOSER(__VA_ARGS__, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_1_PARAM, __LOG_MSG_0_PARAM, )(__VA_ARGS__)

#ifndef log_trace
#ifdef LOG_TRACE_ENABLE
#define log_trace(arg_msg...) { char source [STRING_MAX_LENGTH] = STRING_UNDEFINED; snprintf(source, STRING_MAX_LENGTH, "%s line %d", __FILE__, __LINE__); __LOG_MSG(LOG_TRACE, source, arg_msg); }
#else
#define log_trace(arg_msg...) {}
#endif
#endif

#ifndef log_debug
#ifdef LOG_DEBUG_ENABLE
#define log_debug(arg_msg...) { char source [STRING_MAX_LENGTH] = STRING_UNDEFINED; snprintf(source, STRING_MAX_LENGTH, "%s line %d", __FILE__, __LINE__); __LOG_MSG(LOG_DEBUG, source, arg_msg); }
#else
#define log_debug(arg_msg...) {}
#endif
#endif

#ifndef log_info
#define log_info(arg_msg...) __LOG_MSG(LOG_INFO, LOG_ALTERN_SRC, arg_msg)
#endif

#ifndef log_notice
#define log_notice(arg_msg...) __LOG_MSG(LOG_NOTICE, LOG_ALTERN_SRC, arg_msg)
#endif

#ifndef log_warning
#define log_warning(arg_msg...) __LOG_MSG(LOG_WARNING, LOG_ALTERN_SRC, arg_msg)
#endif

#ifndef log_error
#define log_error(arg_msg...) __LOG_MSG(LOG_ERROR, LOG_ALTERN_SRC, arg_msg)
#endif

#ifndef log_critical
#define log_critical(arg_msg...) __LOG_MSG(LOG_CRITICAL, LOG_ALTERN_SRC, arg_msg)
#endif

#ifndef log_alert
#define log_alert(arg_msg...) __LOG_MSG(LOG_ALERT, LOG_ALTERN_SRC, arg_msg)
#endif

#ifndef log_emergency
#define log_emergency(arg_msg...) __LOG_MSG(LOG_EMERGENCY, LOG_ALTERN_SRC, arg_msg)
#endif

#ifdef __cplusplus

// Type definition

namespace ccs {

namespace log {

typedef ccs::types::int32 Severity_t;
typedef void (*Func_t) (Severity_t, const ccs::types::char8 * const, const ccs::types::char8 * const, va_list);

// Global variables

// Function declaration

void vMessage2Stdout (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, va_list args);
void vMessage2Syslog (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, va_list args);

// WARNING - Confusion in case of log_level(<format>, '0') argument which can be interpreted as '(va_list) NULL'.
//           The methods are named differently to ensure that the log_level macros are all pointing to the same
//           method in any condition.


// cppcheck-suppress unusedFunction // Used by logging library which replaces the backend method
void vMessage (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, va_list args);
void Message (Severity_t severity, const ccs::types::char8 * const source, const ccs::types::char8 * const message, ...);

Func_t SetCallback (Func_t cb); 
Severity_t SetFilter (Severity_t level);

// Function definition

static inline Func_t SetStdout (void) { return SetCallback(&vMessage2Stdout); }
static inline Func_t SetSyslog (void) { return SetCallback(&vMessage2Syslog); }

} // namespace log

} // namespace ccs

extern "C" {

#endif // __cplusplus

// Bug 11846 - Duplicated symbols with logging library
void ccs_log_msg (int severity, const char * const source, const char * const message, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LOG_API_H


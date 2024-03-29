#ifndef TOOLS_H
#define TOOLS_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/include/tools.h $
* $Id: tools.h 100889 2019-07-19 06:46:30Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Some useful POSIX wrappers or routines
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

// Global header files
#ifndef LINT // MARTe2 Integration
#include <ctype.h> // toupper, etc.
#include <dlfcn.h> // dlopen, dlsym, etc.
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h> // fopen, etc.
#include <stdlib.h> // getenv, etc.
#include <string.h> // strerror, etc.
#include <time.h> // timespec, etc.
#include <unistd.h> // setuid, getuid, etc.

#include <sys/mman.h> // mlockall, etc.
#include <sys/stat.h> // mkdir, etc.
#include <sys/types.h>
#endif
// Local header files

#include "types.h" // Condensed integer type definition, RET_STATUS, etc.

#include "NetTools.h"
#include "SysTools.h"
#include "TimeTools.h"

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition

#ifdef __cplusplus // Only allow for overloading for C++ code base

static inline void ns_to_timespec (ccs::types::AbsoluteTime nsec, struct timespec& time) { time = ccs::HelperTools::ToTimespec(nsec); return; }
static inline ccs::types::AbsoluteTime timespec_to_ns (ccs::types::AbsoluteTime sec, ccs::types::AbsoluteTime nsec) { return ccs::HelperTools::FromFractionalToAbsoluteTime(sec, nsec); }
static inline ccs::types::AbsoluteTime timespec_to_ns (struct timespec& time) { return timespec_to_ns(time.tv_sec, time.tv_nsec); }
static inline ccs::types::AbsoluteTime get_time (void) { return ccs::HelperTools::GetCurrentTime(); }
static inline ccs::types::IntervalTime floor_time (ccs::types::IntervalTime time, ccs::types::IntervalTime resolution = 1000000000ul) { return ccs::HelperTools::Floor(time, resolution); }
static inline ccs::types::IntervalTime ceil_time (ccs::types::IntervalTime time, ccs::types::IntervalTime resolution = 1000000000ul) { return ccs::HelperTools::Ceil(time, resolution); }

static inline ccs::types::AbsoluteTime wait(ccs::types::IntervalTime sec, ccs::types::IntervalTime nsec) { return ccs::HelperTools::SleepFor(sec, nsec); }
static inline ccs::types::AbsoluteTime wait(ccs::types::IntervalTime nsec) { return ccs::HelperTools::SleepFor(0ul, nsec); }
static inline ccs::types::AbsoluteTime wait_for (ccs::types::IntervalTime sec, ccs::types::IntervalTime nsec) { return ccs::HelperTools::SleepFor(sec, nsec); }
static inline ccs::types::AbsoluteTime wait_for (ccs::types::IntervalTime nsec) { return ccs::HelperTools::SleepFor(0ul, nsec); }
static inline ccs::types::AbsoluteTime wait_until (ccs::types::AbsoluteTime till, ccs::types::IntervalTime sleep = DEFAULT_WAIT_UNTIL_SLEEP_PERIOD) { return ccs::HelperTools::WaitUntil(till, DEFAULT_WAIT_UNTIL_SLEEP_LIMIT, sleep); }

static inline void format_time (ccs::types::AbsoluteTime time, char * const buffer, int size = STRING_MAX_LENGTH) { ccs::HelperTools::ToISO8601(time, buffer, size); return; }

static inline uint8_t bcd_to_decimal (uint8_t bcd) { return 10 * ((bcd & 0xf0) >> 4) + (bcd & 0x0f); }

static inline char* strupper (char * const s) { return ccs::HelperTools::ToUppercase(s); }
static inline char* strlower (char * const s) { return ccs::HelperTools::ToLowercase(s); }
static inline char* sstrncpy (char * const dst, const char * const src, int size) { return ccs::HelperTools::SafeStringCopy(dst, src, size); }

static inline bool create_path (char * const path, mode_t mode = 0777) { return ccs::HelperTools::CreatePath(path, mode); }

static inline bool exist (const char * const file) { return ccs::HelperTools::Exist(file); }

static inline bool is_variable_string (const char * const str) { return ccs::HelperTools::IsVariableString(str); }

static inline RET_STATUS resolve_variable_string (const char * const in_str, char * const out_str, int size) { return (ccs::HelperTools::ResolveVariableString(in_str, out_str, size) ? STATUS_SUCCESS : STATUS_ERROR); }

static inline RET_STATUS get_host_name (char * const host_name, int size = STRING_MAX_LENGTH) { return (ccs::HelperTools::GetHostName(host_name, size) ? STATUS_SUCCESS : STATUS_ERROR); }
static inline RET_STATUS get_program_name (char * const prog_name, int size = STRING_MAX_LENGTH) { return (ccs::HelperTools::GetProcessName(prog_name, size) ? STATUS_SUCCESS : STATUS_ERROR); }
static inline RET_STATUS get_env_variable (const char * const env_name, char * const env_var, int size = STRING_MAX_LENGTH) { return (ccs::HelperTools::GetEnvironmentVariable(env_name, env_var, size) ? STATUS_SUCCESS : STATUS_ERROR); }
static inline RET_STATUS set_env_variable (const char * const env_name, char * const env_var) { return (ccs::HelperTools::SetEnvironmentVariable(env_name, env_var) ? STATUS_SUCCESS : STATUS_ERROR); }
static inline int get_current_core (void) { return ccs::HelperTools::GetCurrentCore(); }
static inline pthread_t get_thread_id (void) { return ccs::HelperTools::GetThreadId(); }
static inline int get_process_id (void) { return ccs::HelperTools::GetProcessId(); }
static inline RET_STATUS get_ccs_version (char * const version) { return (ccs::HelperTools::GetEnvironmentVariable("CODAC_VERSION", version) ? STATUS_SUCCESS : STATUS_ERROR); }

static inline RET_STATUS set_thread_priority (pthread_t tid, int policy, int priority) { return (ccs::HelperTools::SetPriority(tid, policy, priority) ? STATUS_SUCCESS : STATUS_ERROR); }
static inline RET_STATUS set_thread_affinity_to_core (pthread_t tid, int cid) { return (ccs::HelperTools::SetAffinityToCore(tid, cid) ? STATUS_SUCCESS : STATUS_ERROR); }
static inline RET_STATUS set_thread_priority (int policy, int priority) { return (ccs::HelperTools::SetPriority(ccs::HelperTools::GetThreadId(), policy, priority) ? STATUS_SUCCESS : STATUS_ERROR); }
static inline RET_STATUS set_thread_affinity_to_core (int cid) { return (ccs::HelperTools::SetAffinityToCore(ccs::HelperTools::GetThreadId(), cid) ? STATUS_SUCCESS : STATUS_ERROR); }

#else // __cplusplus

static inline void ns_to_timespec (uint64_t nsec, struct timespec* time) { time->tv_sec = nsec / 1000000000ul; time->tv_nsec = nsec - (1000000000ul * time->tv_sec); return; }
static inline uint64_t timespec_to_ns (struct timespec* time) { return (1000000000ul * (uint64_t) time->tv_sec) + (uint64_t) time->tv_nsec; };
static inline uint64_t get_time (void) { struct timespec time; clock_gettime(CLOCK_REALTIME, &time); return timespec_to_ns(&time); }

static inline uint64_t ceil_time (uint64_t time) { return 1000000000ul * (1ul + (time / 1000000000ul)); };
static inline uint64_t floor_time (uint64_t time) { return 1000000000ul * (time / 1000000000ul); };

static inline uint64_t wait_for (uint32_t sec, uint64_t nsec) 
{ 

  if ((sec != 0u) || (nsec != 0ul)) 
    { 
      struct timespec time; 
      time.tv_sec = sec; 

      while (nsec >= 1000000000ul) 
        { 
          time.tv_sec += 1; 
          nsec -= 1000000000ul; 
        } 

      time.tv_nsec = nsec; 
      nanosleep(&time, NULL); 
    } 

  return get_time(); 

}

static inline uint64_t wait_until (uint64_t till, uint64_t accuracy)
{

   uint64_t time = get_time();

   if (time > till) 
     return time;

   // Sleep till some time before target
   while ((time < till) && ((till - time) > DEFAULT_WAIT_UNTIL_SLEEP_LIMIT))
     {
       time = wait_for(0,accuracy); // nanosleep
     }

   // Spin till target has been reached
   while (time < till)
     {
       time = get_time();
     }

   return time;

}

static inline uint64_t wait (uint32_t sec, uint64_t nsec) { return wait_for(sec, nsec); }

static inline char* strupper (char * const s) { char* p = s; for (; *p; ++p) *p = toupper( *p ); return s; }
static inline char* strlower (char * const s) { char* p = s; for (; *p; ++p) *p = tolower( *p ); return s; }
static inline char* sstrncpy (char * const dst, const char * const src, int size) { if ((dst != NULL) && (src != NULL) && (size > 0)) { strncpy(dst, src, size); dst[size-1] = 0; } return dst; }

static inline RET_STATUS create_path (char * const path, mode_t mode)
{

  bool status = false;

  // Take a copy of the parameter - strtok replaces '/' by '/0'
  char cpy_path [PATH_MAX_LENGTH] = STRING_UNDEFINED; sstrncpy(cpy_path, path, PATH_MAX_LENGTH);
  char tmp_path [PATH_MAX_LENGTH] = STRING_UNDEFINED; char* p_path = tmp_path;

  if (cpy_path[0] == '/') // There is no starting '/' character
    {
      tmp_path[0] = '/'; p_path += 1;
    }

  if (cpy_path[strlen(cpy_path)-1] != '/') // There is no trailing '/' character
    {
      cpy_path[strlen(cpy_path)] = '/';
      cpy_path[strlen(cpy_path)+1] = 0;
    }

  char* p_first = NULL;
  char* p_curr = cpy_path;
  char* p_next = NULL;

  // Clear process umask
  mode_t mask = umask(0);

  while ((p_first = strtok_r(p_curr, "/", &p_next)) != NULL)
    {
      p_curr = NULL;

      snprintf(p_path, STRING_MAX_LENGTH, "%s/", p_first); p_path = tmp_path + strlen(tmp_path);

      if ((mkdir(tmp_path, mode) != 0) && (errno != EEXIST))
        {
          status = false;
          break;
        }
      else
        {
          status = true;
        }
    }

  // Restore process umask
  umask(mask);

  return status;

}

static inline bool exist (const char * const file) { FILE* file_handle = fopen(file, "r"); bool status = (file_handle != NULL); if (status) fclose(file_handle); return status; }

static inline RET_STATUS get_host_name (char * const host_name) { return ((gethostname(host_name, STRING_MAX_LENGTH) == 0) ? STATUS_SUCCESS : STATUS_ERROR); }

static inline RET_STATUS get_env_variable (const char * const envvar, char * const value, int size) 
{ 

  bool status = (getenv(envvar) != NULL);

  if (status) 
    {
      status = (strlen(getenv(envvar)) < size); // strlen does not count trailing '\0'
    }

  if (status)
    {
      sstrncpy(value, getenv(envvar), size);
    }

  return (status ? STATUS_SUCCESS : STATUS_ERROR); 

}

static inline RET_STATUS set_env_variable (const char * const envvar, char * const value) 
{ 
  return ((setenv(envvar, value, 1) == 0) ? STATUS_SUCCESS : STATUS_ERROR); 
}

static inline int get_current_core (void) { return sched_getcpu(); }
static inline pthread_t get_thread_id (void) { return pthread_self(); }
static inline int get_process_id (void) { return getpid(); }
static inline RET_STATUS get_ccs_version (char * const version) { return get_env_variable("CODAC_VERSION", version, STRING_MAX_LENGTH); }

static inline RET_STATUS set_thread_priority (pthread_t tid, int policy, int priority) 
{

  struct sched_param sp; 

  // Assign scheduling policy and priority
  sp.sched_priority = priority;
  
  bool status = (pthread_setschedparam(tid, policy, &sp) == 0);
  
  return (status ? STATUS_SUCCESS : STATUS_ERROR); 
  
}

static inline RET_STATUS set_thread_affinity_to_core (pthread_t tid, int cid) 
{

  cpu_set_t cpuset; 
  CPU_ZERO(&cpuset); 

  bool status = (cid >= 0 && cid < sysconf(_SC_NPROCESSORS_ONLN));

  if (status)
    {
      CPU_SET(cid, &cpuset);
      status = (pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0);
    }
   
  return (status ? STATUS_SUCCESS : STATUS_ERROR); 

}

#endif // __cplusplus

#endif // TOOLS_H


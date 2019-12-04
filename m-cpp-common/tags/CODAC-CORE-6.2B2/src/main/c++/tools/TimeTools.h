/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/tools/TimeTools.h $
* $Id: TimeTools.h 100889 2019-07-19 06:46:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Misc. system-level helper routines
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*                                 CS 90 046
*                                 13067 St. Paul-lez-Durance Cedex
*                                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

#ifndef _TimeTools_h
#define _TimeTools_h

/**
 * @file TimeTools.h
 * @brief Header file for various time-related helper routines
 * @date 24/06/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of time-related helper routines.
 */

// Global header files

#include <stdio.h> // snprintf, etc.
#include <string.h> // strlen, etc.
#include <time.h> // timespec, etc.

// Local header files

#include "BasicTypes.h" // ccs::types::uint64, etc.

// Constants

#define DEFAULT_WAIT_UNTIL_SLEEP_LIMIT 100000ul
#define DEFAULT_WAIT_UNTIL_SLEEP_PERIOD 10000ul

// Type definition

#ifdef __cplusplus

namespace ccs {

namespace types {

typedef uint64 AbsoluteTime; // nsec precision
typedef uint64 IntervalTime; // nsec precision

} // namespace types

namespace HelperTools {

// Global variables

// Function declaration

// Function definition

/**
 * @brief Conversion between two time representation.
 */

static inline ccs::types::AbsoluteTime FromFractionalToAbsoluteTime (ccs::types::AbsoluteTime sec, ccs::types::AbsoluteTime nsec)
{

  return ((1000000000ul * sec) + nsec);

}

/**
 * @brief Rounds downward with specified resolution.
 * @return highest value of specified resolution which is not higher than ..
 * @code
   #include "TimeTools.h"

   ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
   ccs::types::AbsoluteTime past_second = ccs::HelperTools::Floor(curr_time); // Past exact second

   // Do something
   @endcode
 */

static inline ccs::types::IntervalTime Floor (ccs::types::IntervalTime time, ccs::types::IntervalTime resolution = 1000000000ul) 
{ 

  return (resolution * (time / resolution)); // Lower integral resolution

}

/**
 * @brief Rounds upward with specified resolution.
 * @return Smallest value of specified resolution which is not less than ..
 *
 * @code
   #include "TimeTools.h"

   ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
   ccs::types::AbsoluteTime next_second = ccs::HelperTools::Ceil(curr_time); // Next exact second

   // Do something
   @endcode
 */

static inline ccs::types::IntervalTime Ceil (ccs::types::IntervalTime time, ccs::types::IntervalTime resolution = 1000000000ul) 
{ 

  return (resolution + Floor(time, resolution)); // Higher integral resolution

}

/**
 * @brief Retrieves the current time.
 * @return Current absolute time.
 *
 * @code
   #include "TimeTools.h"

   ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
   ccs::types::AbsoluteTime till_time = ccs::HelperTools::Ceil(curr_time, 1000000ul); // Next exact millisec in the future

   // Run at 1kHz aligned on absolute time
   while (true) {
     // Wake-up at future time
     curr_time = ccs::HelperTools::WaitUntil(till_time);
     ccs::types::IntervalTime jitter = curr_time - till_time;

     // Do something

     // Compute next wake-up time in the future
     till_time += 1000000ul; 
   }
   @endcode
 */

static inline ccs::types::AbsoluteTime GetCurrentTime (void)
{

  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time); 

  ccs::types::AbsoluteTime sec = static_cast<ccs::types::AbsoluteTime>(time.tv_sec);
  ccs::types::AbsoluteTime nsec = static_cast<ccs::types::AbsoluteTime>(time.tv_nsec);

  return FromFractionalToAbsoluteTime(sec, nsec);

}

/**
 * @brief Sleeps for specified interval time.
 * @detail The implementation suspends the thread for a configurable interval. 
 * @param[1] interval [sec]
 * @param[2] interval [nsec]
 * @return Absolute time after sleep.
 */

static inline ccs::types::AbsoluteTime SleepFor (ccs::types::IntervalTime sec, ccs::types::IntervalTime nsec)
{ 

  if ((sec != 0ul) || (nsec != 0ul)) 
    { 
      struct timespec time; 
      time.tv_sec = static_cast<types::uint32>(sec); 
      while (nsec >= 1000000000ul) { time.tv_sec += 1u; nsec -= 1000000000ul; } 
      time.tv_nsec = static_cast<types::uint32>(nsec); 
      nanosleep(&time, NULL_PTR_CAST(struct timespec *)); 
    } 

  return GetCurrentTime(); 

}

/**
 * @brief Sleeps for specified interval time.
 * @detail The implementation suspends the thread for a configurable interval. 
 * @param[1] interval [nsec]
 * @return Absolute time after sleep.
 */

static inline ccs::types::AbsoluteTime SleepFor (ccs::types::IntervalTime interval) { return SleepFor(0ul, interval); }

/**
 * @brief Sleeps till some future time.
 * @detail The implementation performs configurable micro-sleeps till till some absolute
 * time in the future. 
 *
 * The method returns immediately in case the defined target wake-up time is in the past.
 *
 * @param[1] wake-up time [nsec]
 * @param[2] sleep period [nsec], defaults to 10000ul (10 microsec)
 * @return Current absolute time at the end of the sleep period.
 */

static inline types::AbsoluteTime SleepUntil (ccs::types::AbsoluteTime till, ccs::types::IntervalTime sleep = DEFAULT_WAIT_UNTIL_SLEEP_PERIOD)
{

  ccs::types::AbsoluteTime time = GetCurrentTime();

  while (time < till)
    {
      time = SleepFor(0ul, sleep);
    }

  return time;

}

/**
 * @brief Waits till some future time.
 * @detail The implementation provides means to wait till some absolute time in the future
 * with configurable accuracy. The method sleeps for configurable intervals till current
 * time is within a configurable range from the target wake-up time, upon which it switches
 * to spin-waiting.
 *
 * The method returns immediately in case the defined target wake-up time is in the past.
 *
 * The method has been used to perform time-based thread scheduling and, e.g. time-sharing
 * CPU among multiple phase-shifted threads. The method also allows to keep core idle time
 * to service kernel interrupts.
 *
 * @param[1] wake-up time [nsec]
 * @param[2] spin limit [nsec], defaults to 100000ul (100 microsec)
 * @param[3] sleep period [nsec], defaults to 10000ul (10 microsec)
 * @return Current absolute time at the end of the wait period.
 *
 * @notice Used with default parameters, the method may be spin-waiting for 100microsec and
 * therefore use 10% of CPU time when employed to implement a high-accuracy 1kHz timed loop.
 *
 * @notice On an isolated core and tickless kernel, the accuracy is determined by the time
 * to read the system clock (CLOCK_REALTIME) and can be below 100nsec for platforms that
 * map the system clock to the CPU Timestamp Counter (TSC).
 *
 * @code
   #include "TimeTools.h"

   ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
   ccs::types::AbsoluteTime till_time = ccs::HelperTools::Ceil(curr_time, 1000000ul); // Next exact millisec in the future

   // Run at 1kHz aligned on absolute time
   while (true) {
     // Wake-up at future time
     curr_time = ccs::HelperTools::WaitUntil(till_time);
     ccs::types::IntervalTime jitter = curr_time - till_time;

     // Do something

     // Compute next wake-up time in the future
     till_time += 1000000ul; 
   }
   @endcode
 */

static inline types::AbsoluteTime WaitUntil (ccs::types::AbsoluteTime till, ccs::types::IntervalTime spin = DEFAULT_WAIT_UNTIL_SLEEP_LIMIT, ccs::types::IntervalTime sleep = DEFAULT_WAIT_UNTIL_SLEEP_PERIOD)
{

  ccs::types::AbsoluteTime time = GetCurrentTime();

  // Sleep till some time before target
  while ((time < till) && ((till - time) > spin)) // Larger than spin limit
    {
      time = SleepFor(0ul, sleep); // Sleep for period
    }

  // Spin till target has been reached
  while (time < till)
    {
      time = GetCurrentTime();
    }

  return time;

}

/**
 * @brief Converts absolute time to system timespec.
 */

static inline struct timespec ToTimespec (ccs::types::AbsoluteTime time)
{

  struct timespec time_s;

  time_s.tv_sec  = time / 1000000000ul;
  time_s.tv_nsec = time - (1000000000ul * time_s.tv_sec);

  return time_s;
 
}

/**
 * @brief Converts absolute time to ISO8601 human readable time representation.
 */

 static inline void ToISO8601 (ccs::types::AbsoluteTime time, ccs::types::char8 * const buffer, ccs::types::int32 size = STRING_MAX_LENGTH)
{

  ccs::types::char8* p_buf = buffer;

  // Format timestamp  
  struct timespec time_s; 

  time_s.tv_sec  = time / 1000000000ul;
  time_s.tv_nsec = time - (1000000000ul * time_s.tv_sec);
 
  // Bug 8104 - Re-entrancy
  struct tm time_r;

  (void)localtime_r(&(time_s.tv_sec), &time_r);

  (void)strftime(p_buf, size, "%FT%T", &time_r); size -= strlen(p_buf); p_buf = buffer + strlen(buffer); // Re-align

  if (time_s.tv_nsec != 0ul)
    {
      (void)snprintf(p_buf, size, ".%.9ld", time_s.tv_nsec); size -= strlen(p_buf); p_buf = buffer + strlen(buffer); // Re-align
    }

  return;

}

} // namespace HelperTools

} // namespace ccs

#endif // __cplusplus

#endif // _TimeTools_h

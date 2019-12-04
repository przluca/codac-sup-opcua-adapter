/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/TimeTools-tests.cpp $
* $Id: TimeTools-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Unit test code
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*				  CS 90 046
*				  13067 St. Paul-lez-Durance Cedex
*				  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

// Global header files

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "types.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "TimeTools.h"

#include "FIFOBuffer.h"
#include "Statistics.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition

TEST(TimeTools_Test, GetCurrentTime)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::AbsoluteTime ceil_time = ccs::HelperTools::Ceil(curr_time);
  ccs::types::AbsoluteTime floo_time = ccs::HelperTools::Floor(curr_time);

  bool ret = ((curr_time <= ceil_time) && (curr_time >= floo_time));

  if (ret)
    {
      log_info("TEST(TimeTools_Test, GetCurrentTime) - Result '%lu'", curr_time);
    }

  if (ret)
    {
      ret = ((ceil_time - floo_time) == 1000000000ul); // 1 second
    }

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, SleepFor)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::IntervalTime interval  = 100000000ul;
  ccs::types::AbsoluteTime post_time = ccs::HelperTools::SleepFor(interval);

  bool ret = ((post_time - curr_time) >= interval);

  if (ret)
    {
      log_info("TEST(TimeTools_Test, SleepFor) - Result '%lu %lu %lu'", curr_time, post_time, interval);
    }

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, SleepFor_nsec)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::IntervalTime interval  = 100000000ul;
  ccs::types::AbsoluteTime post_time = ccs::HelperTools::SleepFor(0ul, interval);

  bool ret = ((post_time - curr_time) >= interval);

  if (ret)
    {
      log_info("TEST(TimeTools_Test, SleepFor_nsec) - Result '%lu %lu %lu'", curr_time, post_time, interval);
    }

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, SleepFor_sec)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::IntervalTime interval  = 1ul;
  ccs::types::AbsoluteTime post_time = ccs::HelperTools::SleepFor(interval, 0ul);

  bool ret = ((post_time - curr_time) >= interval);

  if (ret)
    {
      log_info("TEST(TimeTools_Test, SleepFor_sec) - Result '%lu %lu %lu'", curr_time, post_time, interval);
    }

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, SleepUntil_default)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::AbsoluteTime till_time = ccs::HelperTools::Ceil(curr_time, 2000000000ul);
  ccs::types::AbsoluteTime post_time = ccs::HelperTools::SleepUntil(till_time);

  bool ret = (post_time >= till_time);

  if (ret)
    {
      log_info("TEST(TimeTools_Test, SleepUntil_default) - Result '%lu %lu %lu'", curr_time, till_time, post_time);
    }

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, SleepUntil_spin)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::AbsoluteTime till_time = ccs::HelperTools::Ceil(curr_time, 100000000ul);
  ccs::types::AbsoluteTime post_time = ccs::HelperTools::SleepUntil(till_time, 0ul);

  bool ret = (post_time >= till_time);

  if (ret)
    {
      log_info("TEST(TimeTools_Test, SleepUntil_spin) - Result '%lu %lu %lu'", curr_time, till_time, post_time);
    }

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, ToISO8601)
{
  ccs::types::AbsoluteTime zero_time = 0x0000000000000000ul;
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::AbsoluteTime ceil_time = ccs::HelperTools::Ceil(curr_time);
  ccs::types::AbsoluteTime floo_time = ccs::HelperTools::Floor(curr_time);
  ccs::types::AbsoluteTime endl_time = 0xFFFFFFFFFFFFFFFFul;

  bool ret = true;

  char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED;

  ccs::HelperTools::ToISO8601(zero_time, buffer, STRING_MAX_LENGTH);
  log_info("TEST(TimeTools_Test, ToISO8601) - '%s' for '0x%.16X'", buffer, zero_time);

  ccs::HelperTools::ToISO8601(curr_time, buffer, STRING_MAX_LENGTH);
  log_info("TEST(TimeTools_Test, ToISO8601) - '%s' for '%lu'", buffer, curr_time);

  ccs::HelperTools::ToISO8601(ceil_time, buffer, STRING_MAX_LENGTH);
  log_info("TEST(TimeTools_Test, ToISO8601) - '%s' for '%lu'", buffer, ceil_time);

  ccs::HelperTools::ToISO8601(floo_time, buffer, STRING_MAX_LENGTH);
  log_info("TEST(TimeTools_Test, ToISO8601) - '%s' for '%lu'", buffer, floo_time);

  ccs::HelperTools::ToISO8601(endl_time, buffer, STRING_MAX_LENGTH);
  log_info("TEST(TimeTools_Test, ToISO8601) - '%s' for '0x%.16X'", buffer, endl_time);

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, WaitUntil_default)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::AbsoluteTime till_time = ccs::HelperTools::Ceil(curr_time, 100000000ul);
  ccs::types::AbsoluteTime post_time = ccs::HelperTools::WaitUntil(till_time);

  bool ret = (post_time >= till_time);

  if (ret)
    {
      log_info("TEST(TimeTools_Test, WaitUntil_default) - Result '%lu %lu %lu'", curr_time, till_time, post_time);
    }

  ASSERT_EQ(true, ret);
}

TEST(TimeTools_Test, WaitUntil_loop)
{
  ccs::types::AbsoluteTime curr_time = ccs::HelperTools::GetCurrentTime();
  ccs::types::AbsoluteTime till_time = ccs::HelperTools::Ceil(curr_time, 100000000ul);
  ccs::types::IntervalTime latency = 0ul;

  ccs::types::uint64 count = 10000ul;

  ccs::base::FIFOBuffer<ccs::types::uint64>* fifo = new ccs::base::FIFOBuffer<ccs::types::uint64> (count); 
  ccs::base::Statistics<ccs::types::uint64>* stat = new ccs::base::Statistics<ccs::types::uint64> (count);

  bool ret = ((static_cast<ccs::base::FIFOBuffer<ccs::types::uint64>*>(NULL) != fifo) &&
	      (static_cast<ccs::base::Statistics<ccs::types::uint64>*>(NULL) != stat));

  while (ret && (count > 0ul))
    {
      curr_time = ccs::HelperTools::WaitUntil(till_time);
      latency = curr_time - till_time;

      ret = fifo->PushData(latency);

      till_time += 1000000ul; // 1kHz
      count -= 1u;
    }

  if (ret)
    {
      log_info("TEST(TimeTools_Test, WaitUntil_loop) - Computing statistics ..");
    }

  while (ret && (fifo->PullData(latency) == true))
    {
      ret = stat->PushSample(latency);
    }

  if (ret)
    {
      log_info("TEST(TimeTools_Test, WaitUntil_loop) - .. '%lu %lu %lu %lu [ns]'", stat->GetAvg(), stat->GetStd(), stat->GetMin(), stat->GetMax());
    }

  ASSERT_EQ(true, ret);
}

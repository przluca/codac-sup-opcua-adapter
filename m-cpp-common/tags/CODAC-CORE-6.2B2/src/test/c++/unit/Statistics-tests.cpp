/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Statistics-tests.cpp $
* $Id: Statistics-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include "Statistics.h"

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition
#if 0
static inline void DisplayStatistics(ccs::base::Statistics<double>* p_ref) 
{ 
  log_info("Statistics avg='%g', std='%g', min/max=['%g' '%g'], rms='%g'", p_ref->GetAvg(), p_ref->GetStd(), p_ref->GetMin(), p_ref->GetMax(), p_ref->GetRms()); wait_for(100000);
}
static inline void DisplayStatistics(ccs::base::Statistics<uint32_t>* p_ref) 
{ 
  log_info("Statistics avg='%u', std='%u', min/max=['%u' '%u'], rms='%u'", p_ref->GetAvg(), p_ref->GetStd(), p_ref->GetMin(), p_ref->GetMax(), p_ref->GetRms()); wait_for(100000);
}
static inline void DisplayStatistics(ccs::base::Statistics<int>* p_ref)
{ 
  log_info("Statistics sum='%d', avg='%d', std='%d', min/max=['%d' '%d'], rms='%d'", p_ref->GetSum(), p_ref->GetAvg(), p_ref->GetStd(), p_ref->GetMin(), p_ref->GetMax(), p_ref->GetRms()); wait_for(100000);
}
#endif
TEST(Statistics_Test, SignedInteger64)
{

  using namespace ccs::base;
  using namespace ccs::types;

  Statistics<int64>* p_stats = new (std::nothrow) Statistics<int64> (1024u);

  bool ret = (static_cast<Statistics<int64>*>(NULL) != p_stats);

  if (ret)
    {
      int64 spl = 0l;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      int64 avg = p_stats->GetAvg();
      int64 max = p_stats->GetMax();
      int64 min = p_stats->GetMin();
      int64 rms = p_stats->GetRms();
      int64 std = p_stats->GetStd();

      ret = ((avg == 0l) &&
	     (min == 0l) &&
	     (max == 0l) &&
	     (std == 0l) &&
	     (rms == 0l));
    }

  if (ret)
    {
      int64 spl = 1000000l;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      int64 avg = p_stats->GetAvg();
      int64 max = p_stats->GetMax();
      int64 min = p_stats->GetMin();
      int64 rms = p_stats->GetRms();
      int64 std = p_stats->GetStd();

      ret = (((avg > 0l) && (avg < 1000l)) && // 1000000 / 1024
	     (min == 0l) &&
	     (max == 1000000l) &&
	     (std > 0l) &&
	     (rms > 0l));
    }

  if (ret)
    {
      int64 spl = -1000000l;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      int64 avg = p_stats->GetAvg();
      int64 max = p_stats->GetMax();
      int64 min = p_stats->GetMin();
      int64 rms = p_stats->GetRms();
      int64 std = p_stats->GetStd();

      ret = ((avg == 0l) &&
	     (min == -1000000l) &&
	     (max == +1000000l) &&
	     (std > 0l) &&
	     (rms > 0l));
    }

  // Clear content of window
  for (uint32 index = 0u; index < 1024u; index += 1u)
    {
      int64 spl = 0l;
      p_stats->PushSample(spl);
    }

  if (ret)
    {
      int64 avg = p_stats->GetAvg();
      int64 max = p_stats->GetMax();
      int64 min = p_stats->GetMin();
      int64 rms = p_stats->GetRms();
      int64 std = p_stats->GetStd();

      ret = ((avg == 0l) &&
	     (min == 0l) &&
	     (max == 0l) &&
	     (std == 0l) &&
	     (rms == 0l));
    }

  if (ret)
    {
      delete p_stats;
    }

  ASSERT_EQ(true, ret);
}

TEST(Statistics_Test, Float32)
{

  using namespace ccs::base;
  using namespace ccs::types;

  Statistics<float32>* p_stats = new (std::nothrow) Statistics<float32> (1024u);

  bool ret = (static_cast<Statistics<float32>*>(NULL) != p_stats);

  if (ret)
    {
      float32 spl = 0.0;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      float32 avg = p_stats->GetAvg();
      float32 max = p_stats->GetMax();
      float32 min = p_stats->GetMin();
      float32 rms = p_stats->GetRms();
      float32 std = p_stats->GetStd();

      ret = ((avg == 0.0) &&
	     (min == 0.0) &&
	     (max == 0.0) &&
	     (std == 0.0) &&
	     (rms == 0.0));
    }

  if (ret)
    {
      float32 spl = +1.0;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      float32 avg = p_stats->GetAvg();
      float32 max = p_stats->GetMax();
      float32 min = p_stats->GetMin();
      float32 rms = p_stats->GetRms();
      float32 std = p_stats->GetStd();

      ret = (((avg > 0.0) && (avg < 0.001)) && // 1.0 / 1024
	     (min == 0.0) &&
	     (max == +1.0) &&
	     (std > 0.0) &&
	     (rms > 0.0));
    }

  if (ret)
    {
      float32 spl = -1.0;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      float32 avg = p_stats->GetAvg();
      float32 max = p_stats->GetMax();
      float32 min = p_stats->GetMin();
      float32 rms = p_stats->GetRms();
      float32 std = p_stats->GetStd();

      ret = ((avg == 0.0) &&
	     (min == -1.0) &&
	     (max == +1.0) &&
	     (std > 0.0) &&
	     (rms > 0.0));
    }

  // Clear content of window
  for (uint32 index = 0u; index < 1024u; index += 1u)
    {
      float32 spl = 0.0;
      p_stats->PushSample(spl);
    }

  if (ret)
    {
      float32 avg = p_stats->GetAvg();
      float32 max = p_stats->GetMax();
      float32 min = p_stats->GetMin();
      float32 rms = p_stats->GetRms();
      float32 std = p_stats->GetStd();

      ret = ((avg == 0.0) &&
	     (min == 0.0) &&
	     (max == 0.0) &&
	     (std == 0.0) &&
	     (rms == 0.0));
    }

  if (ret)
    {
      delete p_stats;
    }

  ASSERT_EQ(true, ret);
}

TEST(Statistics_Test, Float64)
{

  using namespace ccs::base;
  using namespace ccs::types;

  Statistics<float64>* p_stats = new (std::nothrow) Statistics<float64> (1024u);

  bool ret = (static_cast<Statistics<float64>*>(NULL) != p_stats);

  if (ret)
    {
      float64 spl = 0.0;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      float64 avg = p_stats->GetAvg();
      float64 max = p_stats->GetMax();
      float64 min = p_stats->GetMin();
      float64 rms = p_stats->GetRms();
      float64 std = p_stats->GetStd();

      ret = ((avg == 0.0) &&
	     (min == 0.0) &&
	     (max == 0.0) &&
	     (std == 0.0) &&
	     (rms == 0.0));
    }

  if (ret)
    {
      float64 spl = +1.0;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      float64 avg = p_stats->GetAvg();
      float64 max = p_stats->GetMax();
      float64 min = p_stats->GetMin();
      float64 rms = p_stats->GetRms();
      float64 std = p_stats->GetStd();

      ret = (((avg > 0.0) && (avg < 0.001)) && // 1.0 / 1024
	     (min == 0.0) &&
	     (max == +1.0) &&
	     (std > 0.0) &&
	     (rms > 0.0));
    }

  if (ret)
    {
      float64 spl = -1.0;
      ret = (p_stats->PushSample(spl) == STATUS_SUCCESS);
    }

  if (ret)
    {
      float64 avg = p_stats->GetAvg();
      float64 max = p_stats->GetMax();
      float64 min = p_stats->GetMin();
      float64 rms = p_stats->GetRms();
      float64 std = p_stats->GetStd();

      ret = ((avg == 0.0) &&
	     (min == -1.0) &&
	     (max == +1.0) &&
	     (std > 0.0) &&
	     (rms > 0.0));
    }

  // Clear content of window
  for (uint32 index = 0u; index < 1024u; index += 1u)
    {
      float64 spl = 0.0;
      p_stats->PushSample(spl);
    }

  if (ret)
    {
      float64 avg = p_stats->GetAvg();
      float64 max = p_stats->GetMax();
      float64 min = p_stats->GetMin();
      float64 rms = p_stats->GetRms();
      float64 std = p_stats->GetStd();

      ret = ((avg == 0.0) &&
	     (min == 0.0) &&
	     (max == 0.0) &&
	     (std == 0.0) &&
	     (rms == 0.0));
    }

  if (ret)
    {
      delete p_stats;
    }

  ASSERT_EQ(true, ret);
}

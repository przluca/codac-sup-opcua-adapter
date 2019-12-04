/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Buffer-tests.cpp $
* $Id: Buffer-tests.cpp 100814 2019-07-17 13:23:31Z bauvirb $
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

#include "Hash.h"

#include "FIFOBuffer.h"
#include "HIFOBuffer.h"
#include "LIFOBuffer.h"

#include "BlockMemoryFIFO.h"

// Constants

// Type definition

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Global variables

// Function declaration

// Function definition

TEST(Buffer_Test, FIFO)
{
  using namespace ccs::base;
  using namespace ccs::types;

  FIFOBuffer<uint32>* buffer = new (std::nothrow) FIFOBuffer<uint32> (8u);

  bool ret = (static_cast<FIFOBuffer<uint32>*>(NULL) != buffer);

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      uint32 data = index;
      ret = buffer->PushData(data);
      log_info("TEST(Buffer_Test, FIFO) - PushData('%u')", data);
    }

  if (ret)
    {
      ret = (!buffer->IsEmpty() && buffer->IsFull());
    }

  if (ret)
    {
      uint32 data = 0u;
      ret = !buffer->PushData(data); // Expect failure
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      uint32 data;
      buffer->GetData(data, index);
      log_info("TEST(Buffer_Test, FIFO) - GetData('%u, %u')", data, index);
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      uint32 data;
      ret = (buffer->PullData(data) &&
	     (data == index)); // First-In-First-Out
      log_info("TEST(Buffer_Test, FIFO) - PullData('%u')", data);
    }

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  if (ret)
    {
      delete buffer;
    }

  ASSERT_EQ(true, ret);
}

TEST(Buffer_Test, HIFO)
{
  using namespace ccs::base;
  using namespace ccs::types;

  HIFOBuffer<uint32>* buffer = new (std::nothrow) HIFOBuffer<uint32> (8u);

  bool ret = (static_cast<HIFOBuffer<uint32>*>(NULL) != buffer);

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      ret = buffer->PushData(index);
    }

  if (ret)
    {
      ret = (!buffer->IsEmpty() && buffer->IsFull());
    }

  if (ret)
    {
      uint32 data = 0u;
      ret = !buffer->PushData(data); // Expect failure
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      uint32 data;
      ret = (buffer->PullData(data) &&
	     (data == index));
    }

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  for (uint32 index = 0u; ret && (index < 10u); index += 1u)
    {
      uint64 time = ccs::HelperTools::SleepFor(10000000ul);
      ret = buffer->PushData(index, time);
    }

  if (ret)
    {
      ret = (!buffer->IsEmpty() && buffer->IsFull());
    }

  if (ret)
    {
      uint32 data = 10u;
      ret = (buffer->PullData(data) &&
	     (data == 9u)); // Data with highest rank gets out first

      if (!ret)
	{
	  log_error("TEST(Buffer_Test, HIFO) - Unforseen '%u' out", data);
	}
    }

  if (ret)
    {
      ret = (!buffer->IsEmpty() && !buffer->IsFull());
    }

  if (ret)
    {
      delete buffer;
    }

  ASSERT_EQ(true, ret);
}

TEST(Buffer_Test, LIFO)
{
  using namespace ccs::base;
  using namespace ccs::types;

  LIFOBuffer<uint32>* buffer = new (std::nothrow) LIFOBuffer<uint32> (8u);

  bool ret = (static_cast<LIFOBuffer<uint32>*>(NULL) != buffer);

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      uint32 data = index;
      ret = buffer->PushData(data);
      log_info("TEST(Buffer_Test, LIFO) - PushData('%u')", data);
    }

  if (ret)
    {
      ret = (!buffer->IsEmpty() && buffer->IsFull());
    }

  if (ret)
    {
      uint32 data = 0u;
      ret = !buffer->PushData(data); // Expect failure
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      uint32 data;
      buffer->GetData(data, index);
      log_info("TEST(Buffer_Test, LIFO) - GetData('%u, %u')", data, index);
    }

  for (uint32 index = 0u; ret && (index < buffer->GetSize()); index += 1u)
    {
      uint32 data;
      ret = (buffer->PullData(data) &&
	     (data == (buffer->GetSize() - index - 1u))); // Last-In-First-Out
      log_info("TEST(Buffer_Test, LIFO) - PullData('%u')", data);
    }

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  if (ret)
    {
      delete buffer;
    }

  ASSERT_EQ(true, ret);
}

TEST(Buffer_Test, BlockMemoryFIFO)
{
  using namespace ccs::base;
  using namespace ccs::types;

  BlockMemoryFIFO* buffer = new (std::nothrow) BlockMemoryFIFO (8u, sizeof(uint32));

  bool ret = (static_cast<BlockMemoryFIFO*>(NULL) != buffer);

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  for (uint32 index = 0u; ret && (index < buffer->GetWidth()); index += 1u)
    {
      uint32 data = index;
      ret = buffer->PushDataBlock(reinterpret_cast<void*>(&data));
      log_info("TEST(Buffer_Test, BlockMemoryFIFO) - PushData('%u')", data);
    }

  if (ret)
    {
      ret = (!buffer->IsEmpty() && buffer->IsFull());
    }

  if (ret)
    {
      uint32 data = 0u;
      ret = !buffer->PushDataBlock(reinterpret_cast<void*>(&data)); // Expect failure
    }

  for (uint32 index = 0u; ret && (index < buffer->GetWidth()); index += 1u)
    {
      uint32 data;
      buffer->GetDataBlock(reinterpret_cast<void*>(&data), index);
      log_info("TEST(Buffer_Test, BlockMemoryFIFO) - GetData('%u, %u')", data, index);
    }

  for (uint32 index = 0u; ret && (index < buffer->GetWidth()); index += 1u)
    {
      uint32 data;
      ret = (buffer->PullDataBlock(reinterpret_cast<void*>(&data)) &&
	     (data == index)); // First-In-First-Out
      log_info("TEST(Buffer_Test, BlockMemoryFIFO) - PullData('%u')", data);
    }

  if (ret)
    {
      ret = (buffer->IsEmpty() && !buffer->IsFull());
    }

  // References .. to avoid copies

  uint32* inp = static_cast<uint32*>(NULL);
  uint32* out = static_cast<uint32*>(NULL);

  while (ret && !buffer->IsFull())
    {
      inp = reinterpret_cast<uint32*>(buffer->GetInDataBlockReference());
      ret = (static_cast<uint32*>(NULL) != inp);

      if (ret)
	{
	  // Do some data manipulation
	  *inp = ccs::HelperTools::Hash<uint32>("TEST(Buffer_Test, BlockMemoryFIFO)");
	}

      buffer->ReleaseInDataBlockReference();
    }

  while (ret && !buffer->IsEmpty())
    {
      out = reinterpret_cast<uint32*>(buffer->GetOutDataBlockReference());
      ret = (static_cast<uint32*>(NULL) != out);

      if (ret)
	{
	  // Do some data manipulation
	  ret = (*out == ccs::HelperTools::Hash<uint32>("TEST(Buffer_Test, BlockMemoryFIFO)"));
	}

      buffer->ReleaseOutDataBlockReference();
    }

  if (ret)
    {
      delete buffer;
    }

  ASSERT_EQ(true, ret);
}

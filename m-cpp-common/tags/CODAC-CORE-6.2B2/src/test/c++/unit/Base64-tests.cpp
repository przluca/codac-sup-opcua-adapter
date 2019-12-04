/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Base64-tests.cpp $
* $Id: Base64-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h"

#include "log-api.h" // Syslog wrapper routines

#include "Base64.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition

TEST(Base64_Test, Encode_default)
{
  bool ret = (ccs::HelperTools::Base64Encode(static_cast<const ccs::types::uint8*>(NULL), 0u, static_cast<char*>(NULL), 0u) == 0u);

  ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Decode_default)
{
  bool ret = (ccs::HelperTools::Base64Decode(static_cast<ccs::types::uint8*>(NULL), 0u, static_cast<const char*>(NULL)) == 0u);

  ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Encode_0_1)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 0, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Encode_0_1) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(1u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 1u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Encode_0_1) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(1u));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_0_1) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 1u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_0_1) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Encode_0_1) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

  ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Encode_1_1)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 1, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Encode_1_1) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(1u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 1u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Encode_1_1) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(1u));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_1_1) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 1u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_1_1) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Encode_1_1) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

   ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Encode_0_2)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 0, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Encode_0_2) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(2u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 2u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Encode_0_2) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(2u));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_0_2) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 2u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_0_2) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Encode_0_2) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

   ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Encode_1_2)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 1, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Encode_1_2) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(2u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 2u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Encode_1_2) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(2u));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_1_2) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 2u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_1_2) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Encode_1_2) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

   ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Encode_0_3)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 0, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Encode_0_3) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(3u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 3u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Encode_0_3) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(3u));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_0_3) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 3u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_0_3) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Encode_0_3) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

   ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Encode_1_3)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 1, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Encode_1_3) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(3u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 3u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Encode_1_3) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(3u));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_1_3) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 3u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Encode_1_3) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Encode_1_3) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

   ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Buffer_zero)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 0, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Buffer_zero) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(64u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 64u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Buffer_zero) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(64u));

  log_info("TEST(Base64_Test, Buffer_zero) - ccs::HelperTools::Base64Encode is '%s' length '%u'", encoded_buffer, strlen(encoded_buffer));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Buffer_zero) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 64u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Buffer_zero) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Buffer_zero) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

  ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Buffer_one)
{
  ccs::types::uint8 binary_buffer [64]; memset(binary_buffer, 1, 64u);
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Buffer_one) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(64u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 64u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Buffer_one) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(64u));

  log_info("TEST(Base64_Test, Buffer_one) - ccs::HelperTools::Base64Encode is '%s' length '%u'", encoded_buffer, strlen(encoded_buffer));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Buffer_one) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 64u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Buffer_one) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Buffer_one) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

  ASSERT_EQ(true, ret);
}

TEST(Base64_Test, Buffer_some)
{
  ccs::types::uint8 binary_buffer [64]; *((ccs::types::uint64*) binary_buffer) = ccs::HelperTools::GetCurrentTime();
  char encoded_buffer [1024u] = STRING_UNDEFINED;

  log_info("TEST(Base64_Test, Buffer_some) - ccs::HelperTools::ComputeEncodedBufferSize is '%u'", ccs::HelperTools::ComputeEncodedBufferSize(8u));

  ccs::types::uint32 encoded_size = ccs::HelperTools::Base64Encode(binary_buffer, 8u, encoded_buffer, 1024u);

  log_info("TEST(Base64_Test, Buffer_some) - ccs::HelperTools::Base64Encode is '%s' size '%u'", encoded_buffer, encoded_size);

  bool ret = (encoded_size == ccs::HelperTools::ComputeEncodedBufferSize(8u));

  log_info("TEST(Base64_Test, Buffer_some) - ccs::HelperTools::Base64Encode is '%s' length '%u'", encoded_buffer, strlen(encoded_buffer));

  ccs::types::uint8 result_buffer [64];
  ccs::types::uint32 result_size = ccs::HelperTools::ComputeBinaryBufferSize(encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Buffer_some) - ccs::HelperTools::ComputeBinaryBufferSize '%s' is '%u'", encoded_buffer, result_size);
      ret = (result_size == 8u);
    }

  ccs::types::uint32 decoded_size = ccs::HelperTools::Base64Decode(result_buffer, 64u, encoded_buffer);

  if (ret)
    {
      log_info("TEST(Base64_Test, Buffer_some) - ccs::HelperTools::Base64Decode '%s' size '%u'", encoded_buffer, decoded_size);
      ret = (decoded_size == result_size);
    }

  for (ccs::types::uint32 index = 0u; ret && (index < decoded_size); index += 1u)
    {
      log_info("TEST(Base64_Test, Buffer_some) - Buffer['%u'] = '%hhu'", index, result_buffer[index]);
      ret = (result_buffer[index] == binary_buffer[index]);
    }

  ASSERT_EQ(true, ret);
}

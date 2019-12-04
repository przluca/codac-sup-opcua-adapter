/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/NetTools-tests.cpp $
* $Id: NetTools-tests.cpp 100703 2019-07-11 15:23:56Z bauvirb $
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

#include <stdio.h> // sscanf, printf, etc.
#include <string.h> // strncpy, etc.
#include <stdarg.h> // va_start, etc.
#include <signal.h> // sigset, etc.

#include <gtest/gtest.h> // Google test framework

// Local header files

#include "types.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

#include "log-api.h" // Syslog wrapper routines

#include "NetTools.h"

// Constants

// Type definition

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

// Function declaration

// Function definition

TEST(NetTools_Test, IsAddressValid_success)
{
  bool ret = (ccs::HelperTools::IsAddressValid("127.0.0.1") == true);

  if (ret)
    {
      ret = (ccs::HelperTools::IsAddressValid("0.0.0.0") == true);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsAddressValid("10.136.144.22:6161") == true);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsAddressValid_error)
{
  bool ret = (ccs::HelperTools::IsAddressValid("undefined") == false);

  if (ret)
    {
      ret = (ccs::HelperTools::IsAddressValid("0") == false);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsAddressValid("123.456.789.0") == false);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsAddressValid("255.255.255.255") == false);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsAddressValid("a.b.c.d") == false);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsAddressValid_null)
{
  bool ret = (ccs::HelperTools::IsAddressValid(NULL) == false);
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsAddressValid_corrupt)
{
  char* buffer = static_cast<char*>(malloc(STRING_MAX_LENGTH));
  bool ret = (static_cast<char*>(NULL) != buffer);

  if (ret)
    {
      ret = (ccs::HelperTools::IsAddressValid(buffer) == false);
      free(buffer);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsMCastAddress_success)
{
  bool ret = ((ccs::HelperTools::IsMCastAddress("224.0.0.1") == true) &&
	      (ccs::HelperTools::IsMCastAddress("239.0.0.1") == true) &&
	      (ccs::HelperTools::IsMCastAddress("239.0.0.1:20001") == true));
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsMCastAddress_error)
{
  bool ret = ((ccs::HelperTools::IsMCastAddress("10.136.144.22:6161") == false) &&
	      (ccs::HelperTools::IsMCastAddress("0") == false) && 
	      (ccs::HelperTools::IsMCastAddress(NULL) == false));
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsUCastAddress_success)
{
  bool ret = ((ccs::HelperTools::IsUCastAddress("127.0.0.1") == true) &&
	      (ccs::HelperTools::IsUCastAddress("0.0.0.0") == true) &&
	      (ccs::HelperTools::IsUCastAddress("10.136.144.22:6161") == true));
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsUCastAddress_error)
{
  bool ret = ((ccs::HelperTools::IsUCastAddress("239.0.0.1") == false) &&
	      (ccs::HelperTools::IsUCastAddress("0") == false) && 
	      (ccs::HelperTools::IsUCastAddress(NULL) == false));
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsInterfaceValid_success)
{
  bool ret = (ccs::HelperTools::IsInterfaceValid("lo") == true);
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsInterfaceValid_error)
{
  bool ret = (ccs::HelperTools::IsInterfaceValid("undefined") == false);
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsInterfaceValid_null)
{
  bool ret = (ccs::HelperTools::IsInterfaceValid(NULL) == false);
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, IsInterfaceValid_corrupt)
{
  char* buffer = static_cast<char*>(malloc(STRING_MAX_LENGTH));
  bool ret = (ccs::HelperTools::IsInterfaceValid(buffer) == false);
  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetMACAddress_success)
{
  char address [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::GetMACAddress("lo", address) == true);

  if (ret)
    {
      ret = (strncmp(address, "00:00:00:00:00:00", 18) == 0);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::GetMACAddress("lo", address, 10) == true);
    }

  if (ret)
    {
      ret = (strncmp(address, "00:00:00:", 10) == 0);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetMACAddress_error)
{
  char address [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::GetMACAddress("undefined", address) == false);

  if (ret)
    {
      ret = (IsUndefined(address) == true); // Unmodified buffer
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetMACAddress_null)
{
  char address [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::GetMACAddress(NULL, address) == false);

  if (ret)
    {
      ret = (ccs::HelperTools::GetMACAddress("lo", NULL) == false);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetIPAddress_success)
{
  char address [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::GetIPAddress("lo", address) == true);

  if (ret)
    {
      ret = (strncmp(address, "127.0.0.1", 9) == 0);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::GetIPAddress("lo", address, 5) == true);
    }

  if (ret)
    {
      ret = (strncmp(address, "127.", 5) == 0);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetIPAddress_error)
{
  char address [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::GetIPAddress("undefined", address) == false);

  if (ret)
    {
      ret = (IsUndefined(address) == true); // Unmodified buffer
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetIPAddress_null)
{
  char address [STRING_MAX_LENGTH] = STRING_UNDEFINED;
  bool ret = (ccs::HelperTools::GetIPAddress(NULL, address) == false);

  if (ret)
    {
      ret = (ccs::HelperTools::GetIPAddress("lo", NULL) == false);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, FromNetworkByteOrder_16bits)
{

  union { ccs::types::uint16 value; ccs::types::uint8 array [2]; } value = { 0x2301 };

  log_info("TEST(NetTools_Test, FromNetworkByteOrder_16bits) - Value '0x%4.4x' is stored as '[ 0x%2.2x, 0x%2.2x ]'", value.value, value.array[0], value.array[1]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::FromNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      //ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::FromNetworkByteOrder(value.value)));
      ret = (ccs::HelperTools::IsLittleEndian() && (ccs::HelperTools::FromNetworkByteOrder(value.value) == 0x0123));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, FromNetworkByteOrder_32bits)
{

  union { ccs::types::uint32 value; ccs::types::uint8 array [4]; } value = { 0x67452301u };

  log_info("TEST(NetTools_Test, FromNetworkByteOrder_32bits) - Value '0x%8.8x' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::FromNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      //ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::FromNetworkByteOrder(value.value)));
      ret = (ccs::HelperTools::IsLittleEndian() && (ccs::HelperTools::FromNetworkByteOrder(value.value) == 0x01234567));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, FromNetworkByteOrder_float32)
{

  union { ccs::types::float32 value; ccs::types::uint8 array [4]; } value = { 0.1f };

  log_info("TEST(NetTools_Test, FromNetworkByteOrder_float32) - Value '0x%8.8x' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::FromNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::FromNetworkByteOrder(value.value)));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, FromNetworkByteOrder_64bits)
{

  union { ccs::types::uint64 value; ccs::types::uint8 array [8]; } value = { 0xefcdab8967452301ul };

  log_info("TEST(NetTools_Test, FromNetworkByteOrder_64bits) - Value '0x%16.16lx' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3], value.array[4], value.array[5], value.array[6], value.array[7]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::FromNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      //ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::FromNetworkByteOrder(value.value)));
      ret = (ccs::HelperTools::IsLittleEndian() && (ccs::HelperTools::FromNetworkByteOrder(value.value) == 0x0123456789abcdef));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, FromNetworkByteOrder_float64)
{

  union { ccs::types::float64 value; ccs::types::uint8 array [8]; } value = { 0.1 };

  log_info("TEST(NetTools_Test, FromNetworkByteOrder_float) - Value '0x%16.16lx' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3], value.array[4], value.array[5], value.array[6], value.array[7]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::FromNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::FromNetworkByteOrder(value.value)));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, ToNetworkByteOrder_16bits)
{

  union { ccs::types::uint16 value; ccs::types::uint8 array [2]; } value = { 0x0123 };

  log_info("TEST(NetTools_Test, ToNetworkByteOrder_16bits) - Value '0x%4.4x' is stored as '[ 0x%2.2x, 0x%2.2x ]'", value.value, value.array[0], value.array[1]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::ToNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      //ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::ToNetworkByteOrder(value.value)));
      ret = (ccs::HelperTools::IsLittleEndian() && (ccs::HelperTools::ToNetworkByteOrder(value.value) == 0x2301));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, ToNetworkByteOrder_32bits)
{

  union { ccs::types::uint32 value; ccs::types::uint8 array [4]; } value = { 0x01234567u };

  log_info("TEST(NetTools_Test, ToNetworkByteOrder_32bits) - Value '0x%8.8x' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::ToNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      //ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::ToNetworkByteOrder(value.value)));
      ret = (ccs::HelperTools::IsLittleEndian() && (ccs::HelperTools::ToNetworkByteOrder(value.value) == 0x67452301));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, ToNetworkByteOrder_float32)
{

  union { ccs::types::float32 value; ccs::types::uint8 array [4]; } value = { 0.1f };

  log_info("TEST(NetTools_Test, ToNetworkByteOrder_float32) - Value '0x%8.8x' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::ToNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::ToNetworkByteOrder(value.value)));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, ToNetworkByteOrder_64bits)
{

  union { ccs::types::uint64 value; ccs::types::uint8 array [8]; } value = { 0x0123456789abcdeful };

  log_info("TEST(NetTools_Test, ToNetworkByteOrder_64bits) - Value '0x%16.16lx' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3], value.array[4], value.array[5], value.array[6], value.array[7]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::ToNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      //ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::ToNetworkByteOrder(value.value)));
      ret = (ccs::HelperTools::IsLittleEndian() && (ccs::HelperTools::ToNetworkByteOrder(value.value) == 0xefcdab8967452301));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, ToNetworkByteOrder_float64)
{

  union { ccs::types::float64 value; ccs::types::uint8 array [8]; } value = { 0.1 };

  log_info("TEST(NetTools_Test, ToNetworkByteOrder_float64) - Value '0x%16.16lx' is stored as '[ 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x, 0x%2.2x ]'", 
	   value.value, value.array[0], value.array[1], value.array[2], value.array[3], value.array[4], value.array[5], value.array[6], value.array[7]);

  bool ret = (ccs::HelperTools::IsBigEndian() && (value.value == ccs::HelperTools::ToNetworkByteOrder(value.value)));

  if (!ret) // Perhaps a little-endian platform
    {
      ret = (ccs::HelperTools::IsLittleEndian() && (value.value != ccs::HelperTools::ToNetworkByteOrder(value.value)));
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetHostname)
{

  char hostname [STRING_MAX_LENGTH] = STRING_UNDEFINED;
#if 0
  bool ret = ((getenv("HOSTNAME") != static_cast<char*>(NULL)) && (strlen(getenv("HOSTNAME")) > 0));

  if (ret)
    {
      ret = (get_env_variable("HOSTNAME", hostname, STRING_MAX_LENGTH) == STATUS_SUCCESS);
      log_info("TEST(NetTools_Test, GetHostname) - Hostname is '%s'", hostname);
    }

  if (ret)
    {
      ret = (get_host_name(hostname) == STATUS_SUCCESS);
      log_info("TEST(NetTools_Test, GetHostname) - Hostname is '%s'", hostname);
    }
#else
  bool ret = (gethostname(hostname, STRING_MAX_LENGTH) == 0);
#endif
  if (ret)
    {
      ret = (get_host_name(hostname) == STATUS_SUCCESS);
      log_info("TEST(NetTools_Test, GetHostname) - Hostname is '%s'", hostname);
    }

  ASSERT_EQ(true, ret);
}

TEST(NetTools_Test, GetInterfaceStatistics)
{

  ccs::types::PortStatistics stats;

  bool ret = ccs::HelperTools::GetInterfaceStatistics("lo", stats);

  if (ret)
    {
      log_info("TEST(NetTools_Test, GetInterfaceStatistics) - Statistics ..");
      log_info(".. Received bytes '%lu'", stats.rx[ccs::types::PortStatistics::Bytes]);
      log_info(".. Received packets '%lu'", stats.rx[ccs::types::PortStatistics::Packets]);
      log_info(".. Transmitted bytes '%lu'", stats.tx[ccs::types::PortStatistics::Bytes]);
      log_info(".. Transmitted packets '%lu'", stats.tx[ccs::types::PortStatistics::Packets]);

      ret = (stats.rx[0] == stats.tx[0]); // Etc.
    }
  
  ASSERT_EQ(true, ret);
}


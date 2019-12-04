/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/tools/NetTools.h $
* $Id: NetTools.h 100897 2019-07-19 08:29:09Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Misc. system-level helper routines
*
* Author        : Bertrand Bauvir (IO)
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

#ifndef _NetTools_h
#define _NetTools_h

/**
 * @file NetTools.h
 * @brief Header file for various network helper routines.
 * @date 14/09/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of misc. network helper routines.
 */

// Global header files

#ifdef __cplusplus
#include <string>
#endif
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <endian.h>
#ifndef LINT // MARTe2 Integration
#include <stdio.h> // fopen, getline, etc.
#endif
// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "SysTools.h" // Misc. system-related helper functions

//#include "log-api.h" // Syslog wrapper routines

// Constants

// Type definition

#ifdef __cplusplus

namespace ccs {

namespace types {

typedef struct {

  enum Id {
    Bytes = 0,
    Packets,
    Errors,
    Dropped,
    Overrun,
    Frame,
    Compressed,
    Multicast
  };

  uint64 rx [8];
  uint64 tx [8];

} PortStatistics;

} // namespace types

// Function declaration

// Function definition

namespace HelperTools {

/**
 * @brief Retrieves the IPv4 address of a named interface.
 * @detail This method retrieves the IPv4 address of a named interface. 
 * The char array is left unmodified in case the interface is not found.
 * @return TRUE if interface exists, FLASE otherwise.
 */

static inline bool GetIPAddress (const ccs::types::char8 * const iface_name, ccs::types::char8 * const iface_addr, ccs::types::int32 size = STRING_MAX_LENGTH)
{

  struct ifreq ifr; 
  (void)memset(&ifr, 0, sizeof(ifr));

  (void)ccs::HelperTools::SafeStringCopy(ifr.ifr_name, iface_name, IFNAMSIZ); 

  int fd_socket = socket(AF_INET, SOCK_DGRAM, 0);

  bool status = (fd_socket != -1);

  if (status) 
    {
      // Get address of selected local interface
      status = (ioctl(fd_socket, SIOCGIFADDR, &ifr) != -1);
      (void)close(fd_socket);
    }

  if (status)
    {
      status = (iface_addr != NULL_PTR_CAST(ccs::types::char8*));
    }

  if (status)
    {
      (void)ccs::HelperTools::SafeStringCopy(iface_addr, inet_ntoa(reinterpret_cast<struct sockaddr_in*>(&ifr.ifr_addr)->sin_addr), size);
    }

  return status;
  
}

/**
 * @brief Retrieves the MAC address of a named interface.
 * @detail This method retrieves the MAC address of a named interface. 
 * The char array is left unmodified in case the interface is not found.
 * @return TRUE if interface exists, FALSE otherwise.
 */

static inline RET_STATUS GetMACAddress (const ccs::types::char8 * const iface_name, ccs::types::char8 * const iface_addr, ccs::types::int32 size = STRING_MAX_LENGTH) // Named interface
{

  struct ifreq ifr; 
  (void)memset(&ifr, 0, sizeof(ifr));

  struct ethtool_perm_addr* info = static_cast<struct ethtool_perm_addr *>(malloc(sizeof(struct ethtool_perm_addr) + 6));

  bool status = (NULL_PTR_CAST(struct ethtool_perm_addr *) != info);

  if (status)
    {
      (void)memset(info, 0, sizeof(struct ethtool_perm_addr) + 6u);

      info->cmd = ETHTOOL_GPERMADDR;
      info->size = 6;
        
      (void)ccs::HelperTools::SafeStringCopy(ifr.ifr_name, iface_name, IFNAMSIZ); 

      ifr.ifr_data = reinterpret_cast<ccs::types::char8*>(info);
    }

  int fd_socket;

  if (status)
    {
      fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
      status = (fd_socket != -1);
    }

  if (status) 
    {
      status = (ioctl(fd_socket, SIOCETHTOOL, &ifr) != -1);
      close(fd_socket);
    }

  if (status)
    {
      status = (iface_addr != NULL_PTR_CAST(ccs::types::char8*));
    }

  if (status)
    {
      (void)snprintf(iface_addr, size, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", info->data[0], info->data[1], info->data[2], info->data[3], info->data[4], info->data[5]);
    }

  if (NULL_PTR_CAST(struct ethtool_perm_addr *) != info)
    {
      free(info);
    }

  return status;

}

/**
 * @brief Tests if the named interface exists.
 * @return TRUE is the interface exists, FALSE otherwise.
 */

static inline bool IsInterfaceValid (const ccs::types::char8 * const iface_name) // Bug 8722 - Test validity of network interface
{

  ccs::types::string iface_addr = STRING_UNDEFINED;

  return GetMACAddress(iface_name, iface_addr, ccs::types::MaxStringLength);

}

/**
 * @brief Tests if the link is up on the named interface.
 * @return TRUE is the interface exists, FALSE otherwise.
 */

static inline bool IsLinkDetected (const ccs::types::char8 * const iface_name)
{

  struct ifreq ifr;
  struct ethtool_value info;

  (void)memset(&ifr, 0, sizeof(ifr));
  (void)memset(&info, 0, sizeof(info));

  info.cmd = ETHTOOL_GLINK;
        
  (void)ccs::HelperTools::SafeStringCopy(ifr.ifr_name, iface_name, IFNAMSIZ);

  ifr.ifr_data = reinterpret_cast<ccs::types::char8*>(&info);

  int fd_socket = socket(AF_INET, SOCK_DGRAM, 0);

  bool status = (fd_socket != -1);

  if (status)
    {
      status = (ioctl(fd_socket, SIOCETHTOOL, &ifr) != -1);
      close(fd_socket);
    }

  if (status) 
    {
      status = (info.data == 1);
    }

  return status;
  
}

/**
 * @brief Tests if the char array can be parsed as an IPv4 address.
 * @detail This method parsed the char array and expects conformance
 * to an equivalent of '%u.%u.%u.%u(:%u)'. Furthermore, the first byte
 * of the address is restricted to be <= 239.
 * @return TRUE is the address is valid, FALSE otherwise.
 */

static inline bool IsAddressValid (const ccs::types::char8 * const addr)
{

  ccs::types::string buffer = STRING_UNDEFINED; 
  (void)ccs::HelperTools::SafeStringCopy(buffer, addr, ccs::types::MaxStringLength);

  // IP address has the form A.B.C.D
  ccs::types::char8* a_str = NULL_PTR_CAST(ccs::types::char8*);
  ccs::types::char8* b_str = NULL_PTR_CAST(ccs::types::char8*); 
  ccs::types::char8* c_str = NULL_PTR_CAST(ccs::types::char8*);
  ccs::types::char8* d_str = NULL_PTR_CAST(ccs::types::char8*);
  ccs::types::char8* p_buf = NULL_PTR_CAST(ccs::types::char8*);

  a_str = buffer;
  p_buf = buffer;

  while (*p_buf != 0) 
    {
      if (*p_buf == '.') 
        {       
          *p_buf = 0;

          if (b_str == NULL_PTR_CAST(ccs::types::char8*))
            {
              b_str = p_buf + 1;
            }
          else if (c_str == NULL_PTR_CAST(ccs::types::char8*))
            {
              c_str = p_buf + 1;
            }
          else if (d_str == NULL_PTR_CAST(ccs::types::char8*))
            {
              d_str = p_buf + 1;
            }
          else
            {
              // Ignore
            }
        }

      // Three '.' tokens found prior to meeting a ':'
      if ((*p_buf == ':') && (b_str != NULL_PTR_CAST(ccs::types::char8*)) && (c_str != NULL_PTR_CAST(ccs::types::char8*)) && (d_str != NULL_PTR_CAST(ccs::types::char8*)))
        {
          *p_buf = 0;
        }

      p_buf += 1;
    }

  bool status = ((b_str != NULL_PTR_CAST(ccs::types::char8*)) && (c_str != NULL_PTR_CAST(ccs::types::char8*)) && (d_str != NULL_PTR_CAST(ccs::types::char8*)));

  if (status)
    {
      ccs::types::uint32 a = 256u;
      ccs::types::uint32 b = 256u;
      ccs::types::uint32 c = 256u;
      ccs::types::uint32 d = 256u;

      (void)sscanf(a_str, "%u", &a);
      (void)sscanf(b_str, "%u", &b);
      (void)sscanf(c_str, "%u", &c);
      (void)sscanf(d_str, "%u", &d);

      status = ((a <= 239u) && (b <= 255u) && (c <= 255u) && (d <= 255u));
    }

  return status;

}

static inline ccs::types::uint8 GetFirstAddressByte (const ccs::types::char8 * const addr)
{

  ccs::types::uint8 ret = 0u;

  bool status = (true == IsAddressValid(addr));

  if (status)
    {
      ccs::types::string buffer = STRING_UNDEFINED; 
      (void)ccs::HelperTools::SafeStringCopy(buffer, addr, ccs::types::MaxStringLength);

      ccs::types::char8 * a_str = buffer;
      ccs::types::char8 * p_buf = buffer;

      while (*p_buf != 0) 
        {
          if (*p_buf == '.') 
            {
              *p_buf = 0; break;
            }
          
          p_buf += 1;
        }

       (void)sscanf(a_str, "%hhu", &ret);
    }

  return ret;

}

/**
 * @brief Tests if the char array can be parsed as an IPv4 multicast address.
 * @return TRUE is the address is a valid multicast address, FALSE otherwise.
 */

static inline bool IsMCastAddress (const ccs::types::char8 * const addr)
{

  bool status = (true == IsAddressValid(addr));

  if (status)
    {
      ccs::types::uint8 a = GetFirstAddressByte(addr); 
      status = ((a >= 224u) && (a <= 239u));
    }

  return status;

}

/**
 * @brief Tests if the char array can be parsed as an IPv4 unicast address.
 * @return TRUE is the address is a valid unicast address, FALSE otherwise.
 */

static inline bool IsUCastAddress (const ccs::types::char8 * const addr)
{

  bool status = (true == IsAddressValid(addr));

  if (status)
    {
      ccs::types::uint8 a = GetFirstAddressByte(addr); 
      status = (a < 224u);
    }

  return status;

}

/**
 * @brief Retrieves RX/TX statistics for the named interface.
 * @return TRUE in case of success, FALSE otherwise.
 * @code
   ccs::types::PortStatistics statistics;
   bool status = ccs::HelperTools::GetInterfaceStatistics("lo", statistics);

   if (status)
     {
       // Test for dropped packets
       status = ((0u == statictics.rx[ccs::types::PortStatistics::Dropped]) &&
                 (0u == statictics.tx[ccs::types::PortStatistics::Dropped]));
     }
   @endcode
 */

static inline bool GetInterfaceStatistics (const ccs::types::char8 * const iface_name, ccs::types::PortStatistics& stats)
{

  bool status = IsInterfaceValid(iface_name);

  std::string result;

  if (status)
    {
      std::stringstream sstr; 
      sstr << std::ifstream("/proc/net/dev").rdbuf();
      result = sstr.str();
      status = (std::string::npos != result.find(iface_name));
    }

  if (status)
    {
      result = result.substr(result.find(iface_name), std::string::npos); // From interface name to the end of the buffer ..
      result = result.substr(0u, result.find("\n")); // .. to the next end of line
    }

  std::istringstream istr (result);
  std::string value;

  ccs::types::uint64* array = reinterpret_cast<ccs::types::uint64*>(&stats);

  while (static_cast<bool>(std::getline(istr, value, ' ')))
    {
      if (true != value.empty()) 
        {
          if (sscanf(value.c_str(), "%lu", array) > 0)
            {
              array += 1; // First token is the interface name
            }
        }
    }

  return status;

}

/**
 * @brief Conversion from network byte order (big-endianness) to host platform.
 * @todo In-place conversion.
 */

template <typename Type> inline Type FromNetworkByteOrder (Type value) { return value; }

template <> inline ccs::types::uint8 FromNetworkByteOrder<ccs::types::uint8> (ccs::types::uint8 value) { return value; }
template <> inline ccs::types::int8 FromNetworkByteOrder<ccs::types::int8> (ccs::types::int8 value) { return value; }

template <> inline ccs::types::uint16 FromNetworkByteOrder<ccs::types::uint16> (ccs::types::uint16 value) { return be16toh(value); }
template <> inline ccs::types::int16 FromNetworkByteOrder<ccs::types::int16> (ccs::types::int16 value)
{
  ccs::types::uint16 tmp = static_cast<ccs::types::uint16>(value);
  ccs::types::uint16 ret = FromNetworkByteOrder<ccs::types::uint16>(tmp);

  return static_cast<ccs::types::int16>(ret); 
}

template <> inline ccs::types::uint32 FromNetworkByteOrder<ccs::types::uint32> (ccs::types::uint32 value) { return be32toh(value); }
template <> inline ccs::types::int32 FromNetworkByteOrder<ccs::types::int32> (ccs::types::int32 value)
{
  ccs::types::uint32 tmp = static_cast<ccs::types::uint32>(value);
  ccs::types::uint32 ret = FromNetworkByteOrder<ccs::types::uint32>(tmp);

  return static_cast<ccs::types::int32>(ret); 
}

template <> inline ccs::types::uint64 FromNetworkByteOrder<ccs::types::uint64> (ccs::types::uint64 value) { return be64toh(value); }
template <> inline ccs::types::int64 FromNetworkByteOrder<ccs::types::int64> (ccs::types::int64 value)
{
  ccs::types::uint64 tmp = static_cast<ccs::types::uint64>(value);
  ccs::types::uint64 ret = FromNetworkByteOrder<ccs::types::uint64>(tmp);

  return static_cast<ccs::types::int64>(ret); 
}

template <> inline ccs::types::float32 FromNetworkByteOrder (ccs::types::float32 value) { return be32toh(value); }
template <> inline ccs::types::float64 FromNetworkByteOrder (ccs::types::float64 value) { return be64toh(value); }

/**
 * @brief Conversion from host platform to network byte order (big-endianness).
 * @todo In-place conversion.
 */

template <typename Type> inline Type ToNetworkByteOrder (Type value) { return value; }

template <> inline ccs::types::uint8 ToNetworkByteOrder<ccs::types::uint8> (ccs::types::uint8 value) { return value; }
template <> inline ccs::types::int8 ToNetworkByteOrder<ccs::types::int8> (ccs::types::int8 value) { return value; }

template <> inline ccs::types::uint16 ToNetworkByteOrder<ccs::types::uint16> (ccs::types::uint16 value) { return htobe16(value); }
template <> inline ccs::types::int16 ToNetworkByteOrder<ccs::types::int16> (ccs::types::int16 value)
{
  ccs::types::uint16 tmp = static_cast<ccs::types::uint16>(value);
  ccs::types::uint16 ret = ToNetworkByteOrder<ccs::types::uint16>(tmp);

  return static_cast<ccs::types::int16>(ret); 
}

template <> inline ccs::types::uint32 ToNetworkByteOrder<ccs::types::uint32> (ccs::types::uint32 value) { return htobe32(value); }
template <> inline ccs::types::int32 ToNetworkByteOrder<ccs::types::int32> (ccs::types::int32 value)
{
  ccs::types::uint32 tmp = static_cast<ccs::types::uint32>(value);
  ccs::types::uint32 ret = ToNetworkByteOrder<ccs::types::uint32>(tmp);

  return static_cast<ccs::types::int32>(ret); 
}

template <> inline ccs::types::uint64 ToNetworkByteOrder<ccs::types::uint64> (ccs::types::uint64 value) { return htobe64(value); }
template <> inline ccs::types::int64 ToNetworkByteOrder<ccs::types::int64> (ccs::types::int64 value)
{
  ccs::types::uint64 tmp = static_cast<ccs::types::uint64>(value);
  ccs::types::uint64 ret = ToNetworkByteOrder<ccs::types::uint64>(tmp);

  return static_cast<ccs::types::int64>(ret); 
}

template <> inline ccs::types::float32 ToNetworkByteOrder<ccs::types::float32> (ccs::types::float32 value) { return htobe32(value); }
template <> inline ccs::types::float64 ToNetworkByteOrder<ccs::types::float64> (ccs::types::float64 value) { return htobe64(value); }

} // namespace HelperTools

namespace base {

using ccs::HelperTools::IsInterfaceValid; // Backward compatibility

} // namespace base

} // namespace ccs

#endif // __cplusplus

#endif // _NetTools_h

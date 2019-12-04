#ifndef NET_TOOLS_H
#define NET_TOOLS_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/include/net-tools.h $
* $Id: net-tools.h 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: SDN - Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
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

/* Global header files */

/* Local header files */

#include "NetTools.h"

/* Constants */
#if 0
typedef enum PortStatistics_Id {

  PORT_STATISTICS_BYTES = 0,
  PORT_STATISTICS_PACKETS,
  PORT_STATISTICS_ERRORS,
  PORT_STATISTICS_DROPPED,
  PORT_STATISTICS_OVERRUN,
  PORT_STATISTICS_FRAME,      /* RX - TX is documented as collisions */
  PORT_STATISTICS_COMPRESSED, /* RX - TX is documented as carrier */
  PORT_STATISTICS_MCAST,      /* RX - TX is documented as compressed */

  PORT_STATISTICS_ARRAY_SIZE,

} PortStatistics_Id;

/* Type definition */

typedef struct PortStatistics {

  uint64_t rx [PORT_STATISTICS_ARRAY_SIZE];
  uint64_t tx [PORT_STATISTICS_ARRAY_SIZE];

} PortStatistics_t;

/* Global variables */

/* Function declaration */

/* Function definition */

static inline RET_STATUS net_get_statistics (const char* iface_name, PortStatistics_t& iface_stats)
{

  RET_STATUS status = STATUS_ERROR;
#if 0 /* This call is driver/device specific */
  struct ifreq ifr;
  struct ethtool_stats* info = (struct ethtool_stats *) malloc(sizeof(struct ethtool_stats) + 1000);

  memset(&ifr, 0, sizeof(ifr));
  memset(info, 0, sizeof(struct ethtool_stats) + 1000);

  info->cmd = ETHTOOL_GSTATS;
  info->n_stats = 8;
	
  sstrncpy(ifr.ifr_name, (char*) iface_name, IFNAMSIZ); 

  ifr.ifr_data = (char *) info;

  int fd_socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd_socket < 0) 
    {
      //log_error("net_get_statistics - socket failed with '%m'");
      return status;
    }

  if (ioctl(fd_socket, SIOCETHTOOL, &ifr) < 0) 
    {
      //log_error("net_get_statistics - ioctl SIOCETHTOOL failed with '%m'");
    }
  else 
    {
      //log_info("net_get_statistics - ioctl SIOCETHTOOL successful with '%u'", info->n_stats);

      for (uint_t index = 0; index < info->n_stats; index += 1)
	{
	  //log_info("net_get_statistics - ioctl SIOCETHTOOL successful with '%u %lu'", index, info->data[index]);
	}

      status = STATUS_SUCCESS;
    }
      
  free(info);
  close(fd_socket);
#else
  char file_path [STRING_MAX_LENGTH] = "/proc/net/dev";
  FILE* file_handle = fopen((char*) file_path, "r"); /* open( .. , O_RDONLY) */

  if (NULL == file_handle)
    {
      //log_error("net_get_statistics - open failed with '%m'");
      return status;
    }

  char* line;
  size_t length;

  while (getline((char**) &line, &length, file_handle) != -1)
    {
      char* p_buf = line; while (*p_buf == ' ') p_buf += 1; /* Skip characters */

      if (strncmp(p_buf, iface_name, strlen(iface_name)) == 0)
	{

	  p_buf += strlen(iface_name); /* Skip characters */
	  while ((*p_buf < '0') || (*p_buf > '9'))  p_buf += 1; /* Skip characters */

	  /* Fill statistics buffer - We should now have space-separated numbers */
	  {

	    char* temp_ptr = NULL;
	    char* next_ptr = NULL;

	    uint64_t* p_array = (uint64_t*) &iface_stats;

	    while ((next_ptr = strtok_r(p_buf, (const char*) " ", &temp_ptr)) != NULL)
	      {
		p_buf = NULL;
		sscanf(next_ptr, "%lu", p_array); p_array += 1;
	      }

	  }

	  status = STATUS_SUCCESS;

	  break; /* Terminate */

	}

    }

  if (line != NULL) free(line);
 #endif
  return status;
  
}

static inline RET_STATUS net_get_ip_address (const char* iface_name, char* iface_addr, uint_t max_size = STRING_MAX_LENGTH) { return ccs::HelperTools::GetIPAddress(iface_name, iface_addr, max_size); }
static inline RET_STATUS net_get_mac_address (const char* iface_name, char* iface_addr, uint_t max_size = STRING_MAX_LENGTH) { return ccs::HelperTools::GetMACAddress(iface_name, iface_addr, max_size); }
static inline bool net_is_link_detected (const char* iface_name) { return ccs::HelperTools::IsLinkDetected(iface_name); }
/* Bug 8722 - Test validity of network interface */
static inline bool net_is_interface_valid (const char* iface_name) { return ccs::HelperTools::IsInterfaceValid(iface_name); }
static inline bool net_is_address_valid (const char* addr) { return ccs::HelperTools::IsAddressValid(addr); }
static inline bool net_is_mcast_address (const char* addr) { return ccs::HelperTools::IsMCastAddress(addr); }
static inline bool net_is_ucast_address (const char* addr) { return ccs::HelperTools::IsUCastAddress(addr); }
#endif
#endif /* NET_TOOLS_H */
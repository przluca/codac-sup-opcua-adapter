/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/tools/CyclicRedundancyCheck.h $
* $Id: CyclicRedundancyCheck.h 100814 2019-07-17 13:23:31Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : CyclicRedundancyCheck computation
*
* Author        : Bertrand Bauvir (IO)
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

#ifndef _CyclicRedundancyCheck_h
#define _CyclicRedundancyCheck_h

/**
 * @file CyclicRedundancyCheck.h
 * @brief Header file for CRC computation
 * @date 18/04/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the CyclicRedundancyCheck<> methods.
 */

// Global header files

// Local header files

#include "BasicTypes.h"

// Constants

// Type definition

namespace ccs {

namespace HelperTools {

// Global variables

static ccs::types::uint32 __table [256];
static bool __table_init = false;

// Function declaration

/**
 * @brief CRC operation .. Initialise table from CRC polynomial
 * @return True.
 */

static inline bool InitialiseCyclicRedundancyCheck (const ccs::types::uint32 poly = 0xEDB88320u);

/**
 * @brief CRC operation
 * @detail This method implements a cyclic redundancy check on a sized byte array with an optional
 * seed in case the same binary buffer needs to produce different checksum for distinct uses.
 * @return The CRC over the sized array.
 */

template <typename Type> inline Type CyclicRedundancyCheck (const ccs::types::uint8 * const buffer, const ccs::types::uint32 size, const ccs::types::uint32 seed = 0u);

// Function definition

static inline bool InitialiseCyclicRedundancyCheck (const ccs::types::uint32 poly)
{

  bool status = (__table_init == false);

  if (status)
    {
      for (ccs::types::uint32 table_index = 0u; table_index < 256u; table_index++) 
        {
          ccs::types::uint32 seed  = table_index;
          
          for (ccs::types::uint32 bit_index = 0u; bit_index < 8u; bit_index++) 
            {
              if (0x01u == (seed & 0x01u))
                {
                  seed = poly ^ (seed >> 1);
                }
              else 
                {
                  seed = seed >> 1;
                }
            }
          
          __table[table_index] = seed;
        }
      
      __table_init = true;
      status = true;
    }

  return status;

}

template <> inline ccs::types::uint32 CyclicRedundancyCheck (const ccs::types::uint8 * const buffer, const ccs::types::uint32 size, const ccs::types::uint32 seed)
{

  if (__builtin_expect((__table_init == false), 0)) // Unlikely
    {
      __table_init = InitialiseCyclicRedundancyCheck();
    }

  ccs::types::uint32 chksum = 0xFFFFFFFFul; // Initial value

  if (0u != seed)
    {
      chksum = __table[(chksum ^ reinterpret_cast<const ccs::types::uint8*>(&seed)[0]) & 0xFFu] ^ (chksum >> 8);
      chksum = __table[(chksum ^ reinterpret_cast<const ccs::types::uint8*>(&seed)[1]) & 0xFFu] ^ (chksum >> 8);
      chksum = __table[(chksum ^ reinterpret_cast<const ccs::types::uint8*>(&seed)[2]) & 0xFFu] ^ (chksum >> 8);
      chksum = __table[(chksum ^ reinterpret_cast<const ccs::types::uint8*>(&seed)[3]) & 0xFFu] ^ (chksum >> 8);
    }

  for (ccs::types::uint32 index = 0u; index < size; index++) 
    {
      chksum = __table[(chksum ^ buffer[index]) & 0xFFu] ^ (chksum >> 8);
    }

  chksum = chksum ^ 0xFFFFFFFFul;
     
  return chksum;

}

} // namespace HelperTools

} // namespace ccs

#endif // _CyclicRedundancyCheck_h


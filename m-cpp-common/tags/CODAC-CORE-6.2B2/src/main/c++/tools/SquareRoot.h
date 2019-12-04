/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/tools/SquareRoot.h $
* $Id: SquareRoot.h 100814 2019-07-17 13:23:31Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Square root computation extended for integer types
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

#ifndef _SquareRoot_h
#define _SquareRoot_h

/**
 * @file SquareRoot.h
 * @brief Header file for SquareRoot computation
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the SquareRoot<> methods.
 */

// Global header files

#include <math.h> // Floating-point sqrt methods

// Local header files

#include "BasicTypes.h" // Misc. type definition

// Constants

// Type definition

// Global variables

// Function declaration

/**
 * @brief SquareRoot computation
 * @detail This method computes the square root of scalar numbers. It uses 
 * sqrt and sqrtf functions for floating-point numbers and a digit-by-digit
 * calculation method for integer types. In case of negative numbers, the
 * approach taken is to reverse the sign in lieu of throwing an exception.
 * @return sqrt(x) or sqrt(-x) for all supported types
 */

template <typename Type> inline Type SquareRoot (const Type x);

// Function definition

template <> inline ccs::types::float32 SquareRoot (const ccs::types::float32 x) 
{ 

  ccs::types::float32 tmp = x;

  if (tmp < 0.0F) // Warning
    {
      tmp *= -1.0F;
    }
  
  return sqrtf(tmp); 

}

template <> inline ccs::types::float64 SquareRoot (const ccs::types::float64 x) 
{ 

  ccs::types::float64 tmp = x;

  if (tmp < 0.0) // Warning
    {
      tmp *= -1.0;
    }
  
  return sqrt(tmp); 

}

template <> inline ccs::types::uint8 SquareRoot<ccs::types::uint8> (const ccs::types::uint8 x) // From http://en.wikipedia.org/wiki/Methods_of_computing_square_roots
{

  ccs::types::uint8 tmp = x;
  ccs::types::uint8 res = 0u;
  ccs::types::uint8 bit = static_cast<ccs::types::uint8>(0x40u); // The second-to-top bit is set
  
  // "bit" starts at the highest power of four <= the argument.
  while (bit > tmp) 
    {
      bit >>= 2;
    }
  
  while (bit != 0u) 
    {
      if (tmp >= (res + bit)) 
        { 
          tmp -= res + bit; 
          res = (res >> 1) + bit; 
        } 
      else 
        {
          res >>= 1;
        }
      
      bit >>= 2;
    }
  
  return res;

}

template <> inline ccs::types::uint16 SquareRoot<ccs::types::uint16> (const ccs::types::uint16 x) // From http://en.wikipedia.org/wiki/Methods_of_computing_square_roots
{

  ccs::types::uint16 tmp = x;
  ccs::types::uint16 res = 0u;
  ccs::types::uint16 bit = static_cast<ccs::types::uint16>(0x4000u); // The second-to-top bit is set
  
  // "bit" starts at the highest power of four <= the argument.
  while (bit > tmp) 
    {
      bit >>= 2;
    }
  
  while (bit != 0u) 
    {
      if (tmp >= (res + bit)) 
        { 
          tmp -= res + bit; 
          res = (res >> 1) + bit; 
        }
      else 
        {
          res >>= 1;
        }
      
      bit >>= 2;
    }
  
  return res;
  
}

template <> inline ccs::types::uint32 SquareRoot<ccs::types::uint32> (const ccs::types::uint32 x) // From http://en.wikipedia.org/wiki/Methods_of_computing_square_roots
{ 

  ccs::types::uint32 tmp = x;
  ccs::types::uint32 res = 0u;
  ccs::types::uint32 bit = 0x40000000u; // The second-to-top bit is set
  
  // "bit" starts at the highest power of four <= the argument.
  while (bit > tmp) 
    {
      bit >>= 2;
    }
  
  while (bit != 0u) 
    {
      if (tmp >= (res + bit)) 
        {
          tmp -= res + bit; 
          res = (res >> 1) + bit; 
        } 
      else 
        {
          res >>= 1;
        }
      
      bit >>= 2;
    }
  
  return res;
  
}

template <> inline ccs::types::uint64 SquareRoot<ccs::types::uint64> (const ccs::types::uint64 x) // From http://en.wikipedia.org/wiki/Methods_of_computing_square_roots
{ 

  ccs::types::uint64 tmp = x;
  ccs::types::uint64 res = 0u;
  ccs::types::uint64 bit = 0x4000000000000000ul; // The second-to-top bit is set
  
  // "bit" starts at the highest power of four <= the argument.
  while (bit > tmp) 
    {
      bit >>= 2;
    }
  
  while (bit != 0u) 
    {
      if (tmp >= (res + bit)) 
        { 
          tmp -= res + bit; 
          res = (res >> 1) + bit; 
        } 
      else 
        {
          res >>= 1;
        }
      
      bit >>= 2;
    }
  
  return res;
  
}

template <> inline ccs::types::int8 SquareRoot<ccs::types::int8> (const ccs::types::int8 x) 
{

  ccs::types::int8 tmp = x;

  if (tmp < 0) // Warning
    {
      tmp *= -1;
    }

  ccs::types::int8 res = static_cast<ccs::types::int8>(SquareRoot<ccs::types::uint8>(static_cast<ccs::types::uint8>(tmp)));
  
  return res; 

}

template <> inline ccs::types::int16 SquareRoot<ccs::types::int16> (const ccs::types::int16 x) 
{

  ccs::types::int16 tmp = x;

  if (tmp < 0) // Warning
    {
      tmp *= -1;
    }

  ccs::types::int16 res = static_cast<ccs::types::int16>(SquareRoot<ccs::types::uint16>(static_cast<ccs::types::uint16>(tmp)));
  
  return res; 
  
}

template <> inline ccs::types::int32 SquareRoot<ccs::types::int32> (const ccs::types::int32 x) 
{

  ccs::types::int32 tmp = x;

  if (tmp < 0) // Warning
    {
      tmp *= -1;
    }

  ccs::types::int32 res = static_cast<ccs::types::int32>(SquareRoot<ccs::types::uint32>(static_cast<ccs::types::uint32>(tmp)));
  
  return res; 
  
}

template <> inline ccs::types::int64 SquareRoot<ccs::types::int64> (const ccs::types::int64 x) 
{

  ccs::types::int64 tmp = x;

  if (tmp < 0) // Warning
    {
      tmp *= -1;
    }

  ccs::types::int64 res = static_cast<ccs::types::int64>(SquareRoot<ccs::types::uint64>(static_cast<ccs::types::uint64>(tmp)));
  
  return res; 

}

#endif // _SquareRoot_h_



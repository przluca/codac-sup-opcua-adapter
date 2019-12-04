/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/tools/Hash.h $
* $Id: Hash.h 100889 2019-07-19 06:46:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : String hashing computation
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

#ifndef _Hash_h
#define _Hash_h

/**
 * @file Hash.h
 * @brief Header file for hashing computation
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the Hash<> methods.
 */

// Global header files

// Local header files

#include "BasicTypes.h" // Condensed integer type definition, RET_STATUS, etc.

// Constants

// Type definition

namespace ccs {

namespace HelperTools {

// Global variables

// Random uint16_t numbers associated to uint8_t
// Generated using MATLAB randi([0 2^16-1],256,1)
static const ccs::types::uint16 __table_uint16 [] = {
  53393, 59361,  8322, 59859, 41442,  6392, 18251, 35840, 62751, 63234, 51918, 62881, 42957, 11218, 62273,  2257,
  10329, 63608, 62728, 31809, 52447,  9298, 27640, 60013, 42974,  2340, 55648, 61210, 44481, 49659, 48701, 25704,
  46271,  2086, 18148,  3025,  6365, 53966, 45536, 20781, 28753, 25005, 50168, 52114, 12246, 32097, 29201, 42356,
  46488, 49459, 38356, 14667,  9784, 16876, 40373, 31017, 18089, 44544, 42932, 10656,  7798, 32660, 62897, 22307,
  49235, 16717, 33158, 45814, 58386, 62868, 35862,  9084, 55097, 16664, 53364, 15959, 60900, 22936, 12884, 16455,
  23046, 54449, 38355, 36026, 60109, 18732, 49623, 49396, 24932, 37212,   780, 22093, 45168, 49030,  5123, 29011,
   4971,  3535, 34786, 51063, 61211,  8513, 37278, 30761, 10628, 52054, 20395, 34637, 10855, 39451, 17234, 42865,
  29526,  5493, 15006, 59856,  9986, 54120, 35280, 65282,  6989, 63038,   303, 50784, 53562, 56930,  5533, 26200,
  28273, 59680, 11917, 17288,  9538,  8917, 56969, 37991, 55904, 40767, 23000, 33636, 26332,  4978, 15723,  8081,
  27346,  3254, 59160, 61917, 32169, 32063, 22132, 58985, 24198,  7287, 51134, 25541, 15839, 26470,  6321, 8649,
  61738, 62661, 37696,  3917, 15386, 23144, 53817,  1009,  2819, 11074, 42540, 47954, 42450, 29551, 35848, 19419,
  48804, 12383, 45008, 12026, 24149, 41000, 51132,  5316, 60908, 50837, 31902, 28564, 29280, 20076, 33325, 33473,
  53584, 52090, 42226, 24812, 53187, 34919, 22985, 61538, 57405, 36055, 40794, 38472, 13614, 19742, 30862, 15105,
  55332, 12764, 14806, 11187, 14920, 28553, 20388, 60514, 28194, 12112, 59302, 64208, 28761,  7282, 16912,  1914,
  26785, 38987, 17184, 39507, 46610, 14532,  7695, 19442, 20891, 27798, 33283,  5604, 17202, 52495, 60873, 47862,
  32021, 37914, 15550, 30071, 63116, 35835, 34153, 15177, 32040, 40898, 17030, 52433, 36035,  9499, 12052, 15725,
};

// Random uint32_t numbers associated to uint8_t
// Generated using MATLAB randi([0 2^32-1],256,1,'uint32')
static const ccs::types::uint32 __table_uint32 [] = {

  2335972299, 3096871654, 2244100250, 4267928862,  939208984,  454400123,  471147022,  273122858,
  1737657850, 1925746993, 1571168515, 3279227462, 2696794415, 3315630509, 4006575576, 4177890155,
   824755480,  596460159, 2990441147,  402953946, 2256594731, 2277811073, 3698567327, 2082429210,
  1689882203, 2883774786, 3183678625, 2233608339, 1493414551,  644233299, 2517246261, 1125905566,
   190928872, 3242413693, 1042755171, 1900103466, 2954061691, 1542873415, 3162556537, 1695255697,
  2935248798, 3023860688, 1899687285,   84085252, 1421023775, 1822395412, 1160802629,  846339618,
  3529265615, 1846498393, 3812947214, 1680118172, 3303321140, 1704206588, 3472541600, 3243031446,
  1620901522,  927794179, 3394773151, 4077229255, 1406882826, 2883058518, 1883965854, 3579857799,
  3302203869,  718348508, 3702177965, 4251468527, 2209431921, 3797958074, 2525552676,  664656276,
   858404287, 1747857716, 3215666574, 3545855488, 3392865378, 1368051216, 2293787961,  386335223,
   479772518,  585372040, 2914789454, 2126769102,  814799989, 2126033829,  633972486,  236112163,
  3653783114, 2407584837, 3992639680, 2992162842, 2503068135, 3502104356, 3775335972, 4247343049,
     2243585, 3717030445, 2630952953, 4251803758, 2266368640, 2059537257, 3441761758,  978577957,
  2139298691, 3869131976, 2468151142, 3630012664, 3172435897, 2516795154, 1059716719, 2862235858,
   358555954, 2688476805, 2838735260, 3134260352, 3825751208, 4218960217, 3302954771, 2497293649,
  3987074243, 2491469149,   72941164,  519087905, 3705314322, 2080037677, 3628627492,  899387987,
  2372073249, 2705328539,  137400366, 2640174031, 1556545378,  212740807, 2102687092,  826825855,
   528640670,  882590743,  629276749,  812058805,  183190709, 2728154279, 1210608927, 2313255117,
  2985702519, 2143686954, 2301248011, 1912047135,  532285079, 2106068538, 3663599180, 3753489627,
  1160905317,  895334718, 2426568779, 2750118348, 1791125708,  884658102, 4071341754,  352493150,
   454018530,  610061973,  714942149, 2666997067, 2464064677,  223672835, 3999479492, 3129578092,
  3169005772,  272320256, 3695564078, 4013239427, 4227958557, 3689114126, 3373950167, 2204939223,
   762796759, 1711928853,  575230342,  132669601, 4033582913, 1294099693, 1269308153, 1429950442,
  2006042588, 2783990957,  108354214, 3617249856, 2401026498, 3668331349, 1494129762, 1915669866,
   232956811,  760671065, 2846738949, 1420899714, 3858968577,  507472713, 4245222678, 2319205455,
  3036187196, 4292783820, 1236303522, 1780360748, 1996472347, 3281170667, 3514159588,  430448237,
   765006491, 1544620191,  243544785, 2241481900, 1442460362,  754492737,  897419131, 3887604933,
  2900783018, 2012055597, 3917579146,  446726312, 3202096004, 3162244642, 2413176446,  791107624,
  2565003218, 1288219563,  576053610,  913116632, 3843745227,  306887494, 1041471840,  230873356,
  1897181789,   57050911, 3853407510,  844640500,  401023315, 1320130781, 1958752764,  436666720,
  4275166327, 1426327858, 1277094849,  266482196, 1280948105,  199077166, 2170797342, 3270299281,
  2710425008,  386081700,  347301462, 3338222685, 3887524124, 2292533076,  468813770, 3546822037

};

// Function declaration

/**
 * @brief Bitwise circular shift operation
 * @detail This method implements a left bit rotation by a number of positions.
 * @return Rotated bitmap for all supported types
 */

 template <typename Type> inline Type Rotate (const Type bitmap, const ccs::types::uint8 nbits);

/**
 * @brief String hashing operation
 * @detail This method generates a hash for a string. It uses a static table of uniformly
 * distributed pseudorandom integers, bitwise XOR and rotation operations to ensure the
 * non-commutativity of the operation on the character array,
 * @return Hash for all supported types
 */

template <typename Type> inline Type Hash (const ccs::types::char8 * const key); // Has to be a null terminated array

// Function definition

template <> inline ccs::types::uint8 Rotate (const ccs::types::uint8 bitmap, const ccs::types::uint8 nbits)
{

  ccs::types::uint16 lsb_mask = 0x00FFu;
  ccs::types::uint16 msb_mask = 0x0000u;
  ccs::types::uint8 bitnum = nbits; 

  while (bitnum > 7u) 
    { 
      bitnum -= 8u; 
    }

  while (bitnum > 0u)
    {
      msb_mask = msb_mask << 1; msb_mask += 1u; 
      bitnum -= 1u;
    }

  msb_mask = msb_mask << 8;

  ccs::types::uint16 shift  = (static_cast<ccs::types::uint16>(bitmap) & lsb_mask) << nbits; // Shift bitmap by number of bits ...
  ccs::types::uint16 rotate = (static_cast<ccs::types::uint16>( shift) & msb_mask) >> 8;    // ... and rotate MSB ...

  return static_cast<ccs::types::uint8>((shift + rotate) & lsb_mask);

}

template <> inline ccs::types::uint16 Rotate (const ccs::types::uint16 bitmap, const ccs::types::uint8 nbits)
{

  ccs::types::uint32 lsb_mask = 0x0000FFFFu;
  ccs::types::uint32 msb_mask = 0x00000000u;
  ccs::types::uint8 bitnum = nbits; 

  while (bitnum > 15u) 
    { 
      bitnum -= 16u; 
    }

  while (bitnum > 0u)
    {
      msb_mask = msb_mask << 1; msb_mask += 1u; 
      bitnum -= 1u;
    }

  msb_mask = msb_mask << 16;

  ccs::types::uint32 shift  = (static_cast<ccs::types::uint32>(bitmap) & lsb_mask) << nbits; // Shift bitmap by number of bits ...
  ccs::types::uint32 rotate = (static_cast<ccs::types::uint32>( shift) & msb_mask) >> 16;    // ... and rotate MSB ...

  return static_cast<ccs::types::uint16>((shift + rotate) & lsb_mask);

}

template <> inline ccs::types::uint32 Rotate (const ccs::types::uint32 bitmap, const ccs::types::uint8 nbits)
{

  ccs::types::uint64 lsb_mask = 0x00000000FFFFFFFFul;
  ccs::types::uint64 msb_mask = 0x0000000000000000ul;
  ccs::types::uint8 bitnum = nbits; 

  while (bitnum > 31u) 
    { 
      bitnum -= 32u; 
    }

  while (bitnum > 0u)
    {
      msb_mask = msb_mask << 1; msb_mask += 1u; 
      bitnum -= 1u;
    }

  msb_mask = msb_mask << 32;

  ccs::types::uint64 shift  = (static_cast<ccs::types::uint64>(bitmap) & lsb_mask) << nbits; // Shift bitmap by number of bits ...
  ccs::types::uint64 rotate = (static_cast<ccs::types::uint64>( shift) & msb_mask) >> 32;    // ... and rotate MSB ...

  return static_cast<ccs::types::uint32>((shift + rotate) & lsb_mask);

}

template <> inline ccs::types::uint16 Hash (const ccs::types::char8 * const key) // Has to be a nil terminated array
{

  ccs::types::uint16 hash = 0u;

  const ccs::types::char8 * buffer = key;

  while (*buffer != '\0')
    {
      // This is a commutative operation, changing the order of the characters in the key does not change the result
      hash = Rotate<ccs::types::uint16>(hash, 3u);
      hash ^= __table_uint16[static_cast<int>(*buffer)];
      buffer++;
    }

  return hash;

}

template <> inline ccs::types::uint32 Hash (const ccs::types::char8 * const key) // Has to be a null terminated array
{

  ccs::types::uint32 hash = 0u;

  const ccs::types::char8 * buffer = key;

  while (*buffer != '\0')
    {

      hash = Rotate<ccs::types::uint32>(hash, 7u);
      hash ^= __table_uint32[static_cast<int>(*buffer)];
      buffer++;

    }

  return hash;

}

} // namespace HelperTools

} // namespace ccs

#endif // _Hash_h


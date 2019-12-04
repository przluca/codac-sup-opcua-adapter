/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types/BasicTypes.h $
* $Id: BasicTypes.h 100890 2019-07-19 07:14:30Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Generic type class definition
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

/**
 * @file BasicTypes.h
 * @brief Header file for basic type definition
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the basic C++ types.
 */

#ifndef _BasicTypes_h_
#define _BasicTypes_h_

// Global header files

#include <stdbool.h> // TRUE and FALSE definition
#include <stddef.h> // NULL

// Local header files

// Constants

#ifndef RET_STATUS
#define RET_STATUS     bool
#define STATUS_SUCCESS true
#define STATUS_ERROR   false
#endif // ifndef RET_STATUS

#ifndef STRING_MAX_LENGTH
#define STRING_MAX_LENGTH 64
#define STRING_UNDEFINED ""
#endif // ifndef STRING_MAX_LENGTH

#ifndef PATH_MAX_LENGTH
#define PATH_MAX_LENGTH 256
#endif // ifndef PATH_MAX_LENGTH

#define NULL_PTR_CAST(type) static_cast<type>(0)

// Type definition

typedef unsigned int uint_t;

#ifdef __cplusplus

namespace ccs {

namespace types {

/**
 * @brief XXX.
 */

typedef bool boolean;

typedef char char8;
typedef char8 string [STRING_MAX_LENGTH]; // Fixed-size character array

typedef signed char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long int64;
typedef unsigned long uint64;

typedef float float32;
typedef double float64;
//typedef float32 float32_t; // Backward compatibility
//typedef float64 float64_t; // Backward compatibility

typedef enum {

  BOOLEAN = 0,
  CHAR8,
  STRING,
  SINT8,
  UINT8,
  SINT16,
  UINT16,
  SINT32,
  UINT32,
  SINT64,
  UINT64,
  FLOAT32,
  FLOAT64,
  UNDEF

} Identifier;

typedef uint32 DirIdentifier;

// Global variables

static const boolean True = true;
static const boolean False = false;
static const uint32 MaxStringLength = 64u;
static const uint32 MaxPathLength = 256u;

static const uint32 UndefVariable = 0u;
static const uint32 InputVariable = 1u;
static const uint32 OutputVariable = 2u;
static const uint32 AnyputVariable = 3u;

// Function declaration

const char8 * GetName (const Identifier type); // Enumeration
Identifier GetIdentifier (const char8 * const type);
uint32 GetSize (const char8 * const type);
uint32 GetSize (const Identifier type);

// Function definition

} // namespace types

} // namespace ccs

#endif // __cplusplus

#endif // _BasicTypes_h_ 

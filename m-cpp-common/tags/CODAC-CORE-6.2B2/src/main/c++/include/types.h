#ifndef TYPES_H
#define TYPES_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/include/types.h $
* $Id: types.h 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Some useful common type definition
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*		  CS 90 046
*		  13067 St. Paul-lez-Durance Cedex
*		  France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/* Global header files */

#include <stdbool.h> /* TRUE and FALSE definition */
#include <stdint.h>  /* Condensed integer type definition */

/* Local header files */

#include "BasicTypes.h" // C++ types (ccs::types namespace)

/* Constants */

#define PATH_MAX_LENGTH  256
#ifndef STRING_MAX_LENGTH
#define STRING_MAX_LENGTH 64
#endif // ifndef STRING_MAX_LENGTH
#define STRING_UNDEFINED  ""

#ifndef IsUndefined
#define IsUndefined(string) (((string == NULL) || (strlen(string) == 0)) ? true : false)
#endif

// Backward compatibility ...
typedef bool RetStatus_t;
static const RetStatus_t RetStatusSuccess = true;
static const RetStatus_t RetStatusError = false;

#ifndef RET_STATUS
#define RET_STATUS     RetStatus_t
#define STATUS_SUCCESS RetStatusSuccess
#define STATUS_ERROR   RetStatusError
#endif

/* Type definition */

typedef char charray [STRING_MAX_LENGTH]; /* For backward compatibility purposes with SDN library */

/* Global variables */

/* Function declaration */

/* Function definition */

#endif /* TYPES_H */


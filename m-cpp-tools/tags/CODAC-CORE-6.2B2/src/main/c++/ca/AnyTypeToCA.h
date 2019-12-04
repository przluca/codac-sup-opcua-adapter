/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/ca/AnyTypeToCA.h $
* $Id: AnyTypeToCA.h 99155 2019-04-18 13:18:39Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Generic type class definition
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

/**
 * @file AnyTypeToCA.h
 * @brief Header file for AnyTypeToCA helper routines.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyTypeToCA helper routines.
 */

#ifndef _AnyTypeToCA_h_
#define _AnyTypeToCA_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

#include <cadef.h>

#include <types.h> // Misc. type definition
#include <tools.h> // Misc. helper functions

#include <log-api.h> // Logging functions

#include <AnyType.h>
#include <ArrayType.h>
#include <CompoundType.h>
#include <ScalarType.h>

#include <AnyTypeHelper.h>

// Local header files

// Constants

// Type definition

namespace ccs {

namespace HelperTools {

typedef chtype CATypeIdentifier_t;

// Global variables

// Function declaration

static inline CATypeIdentifier_t AnyTypeToCAScalar (const std::shared_ptr<const ccs::types::AnyType>& type);

// Function definition

static inline CATypeIdentifier_t AnyTypeToCAScalar (const std::shared_ptr<const ccs::types::AnyType>& type)
{

  bool status = (ccs::HelperTools::Is<ccs::types::ScalarType>(type) || 
		 (ccs::HelperTools::Is<ccs::types::ArrayType>(type) && 
		  ccs::HelperTools::Is<ccs::types::ScalarType>(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType())));

  std::shared_ptr<const ccs::types::ScalarType> inp_type;

  if (status)
    {
      inp_type = (ccs::HelperTools::Is<ccs::types::ScalarType>(type) ? 
		  std::dynamic_pointer_cast<const ccs::types::ScalarType>(type) :
		  std::dynamic_pointer_cast<const ccs::types::ScalarType>((std::dynamic_pointer_cast<const ccs::types::ArrayType>(type))->GetElementType()));
      status = (inp_type ? true : false);
    }

  CATypeIdentifier_t out_type = -1;

  if (status)
    {
      // Etc.
      if (ccs::types::Boolean == inp_type) out_type = DBR_CHAR;
      if (ccs::types::SignedInteger8 == inp_type) out_type = DBR_CHAR;
      if (ccs::types::UnsignedInteger8 == inp_type) out_type = DBR_CHAR;
      if (ccs::types::SignedInteger16 == inp_type) out_type = DBR_SHORT;
      if (ccs::types::UnsignedInteger16 == inp_type) out_type = DBR_SHORT;
      if (ccs::types::SignedInteger32 == inp_type) out_type = DBR_LONG;
      if (ccs::types::UnsignedInteger32 == inp_type) out_type = DBR_LONG;
      //if (ccs::types::SignedInteger64 == inp_type) out_type = DBR_LONG;
      //if (ccs::types::UnsignedInteger64 == inp_type) out_type = DBR_LONG;
      if (ccs::types::Float32 == inp_type) out_type = DBR_FLOAT;
      if (ccs::types::Float64 == inp_type) out_type = DBR_DOUBLE;
      if (ccs::types::String == inp_type) out_type = DBR_STRING;
    }

  return out_type;

}

static inline std::shared_ptr<const ccs::types::AnyType> CAScalarToAnyType (CATypeIdentifier_t inp_type, ccs::types::uint32 mult)
{

  std::shared_ptr<const ccs::types::AnyType> out_type;

  switch (inp_type)
    {
      case DBR_CHAR:
      case DBR_ENUM:
	out_type = ((mult <= 1u) ? 
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger8) :
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(std::shared_ptr<const ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("int8Array", ccs::types::SignedInteger8, mult))));
	break;
      case DBR_SHORT:
	out_type = ((mult <= 1u) ? 
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger16) :
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(std::shared_ptr<const ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("int16Array", ccs::types::SignedInteger16, mult))));
	break;
      case DBR_LONG:
	out_type = ((mult <= 1u) ? 
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::SignedInteger32) :
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(std::shared_ptr<const ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("int32Array", ccs::types::SignedInteger32, mult))));
	break;
      case DBR_FLOAT:
	out_type = ((mult <= 1u) ? 
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::Float32) :
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(std::shared_ptr<const ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("float32Array", ccs::types::Float32, mult))));
	break;
      case DBR_DOUBLE:
	out_type = ((mult <= 1u) ? 
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::Float64) :
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(std::shared_ptr<const ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("float64Array", ccs::types::Float64, mult))));
	break;
      case DBR_STRING:
	out_type = ((mult <= 1u) ? 
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(ccs::types::String) :
		    std::dynamic_pointer_cast<const ccs::types::AnyType>(std::shared_ptr<const ccs::types::ArrayType>(ccs::HelperTools::NewArrayType("stringArray", ccs::types::String, mult))));
	break;
      default:
	break;
    }

  return out_type;

}

} // namespace HelperTools

} // namespace ccs

#endif // _AnyTypeToCA_h_


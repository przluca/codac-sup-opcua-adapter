/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/AnyTypeToPVA.h $
* $Id: AnyTypeToPVA.h 99932 2019-05-24 06:21:23Z bauvirb $
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
 * @file AnyTypeToPVA.h
 * @brief Header file for AnyTypeToPVA helper routines.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyTypeToPVA helper routines.
 */

#ifndef _AnyTypeToPVA_h_
#define _AnyTypeToPVA_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

#include <pv/pvData.h>

#include <types.h> // Misc. type definition
#include <tools.h> // Misc. helper functions

#include <log-api.h> // Logging functions

#include <AnyType.h>
#include <ArrayType.h>
#include <CompoundType.h>
#include <ScalarType.h>
//#include <CompoundArray.h>
//#include <ScalarArray.h>

#include <AnyTypeHelper.h>

// Local header files

// Constants

// Type definition

// Global variables

// Function declaration

namespace ccs {

namespace HelperTools {

static inline epics::pvData::ScalarType AnyTypeToPVScalar (const std::shared_ptr<const ccs::types::AnyType>& type);
static inline std::shared_ptr<const epics::pvData::Structure> AnyTypeToPVStruct (const std::shared_ptr<const ccs::types::AnyType>& type);

// Function definition

static inline epics::pvData::ScalarType AnyTypeToPVScalar (const std::shared_ptr<const ccs::types::AnyType>& type)
{

  std::shared_ptr<const ccs::types::ScalarType> inp_type;
#if 1
  bool status = ccs::HelperTools::Is<ccs::types::ScalarType>(type);

  if (status)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::ScalarType>(type);
      status = (inp_type ? true : false);
    }
#else // Map ScalarArray<int8> to epics::pvData::pvString
  bool status = (ccs::HelperTools::Is<ccs::types::ScalarType>(type) ||
		 (ccs::HelperTools::Is<ccs::types::ScalarArray>(type) && 
		  (ccs::types::SignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(std::dynamic_pointer_cast<const ccs::types::ScalarType>(type)->GetElementType()))));

  if (status)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::ScalarType>(type);
      status = (inp_type ? true : false);

      if (!status)
	{
	  inp_type = ccs::types::String;
	  status = true;
	}
    }
#endif
  epics::pvData::ScalarType out_type = epics::pvData::MAX_SCALAR_TYPE;

  if (status)
    {
      // Etc.
      if (ccs::types::Boolean == inp_type) out_type = epics::pvData::pvBoolean;
      if (ccs::types::SignedInteger8 == inp_type) out_type = epics::pvData::pvByte;
      if (ccs::types::UnsignedInteger8 == inp_type) out_type = epics::pvData::pvUByte;
      if (ccs::types::SignedInteger16 == inp_type) out_type = epics::pvData::pvShort;
      if (ccs::types::UnsignedInteger16 == inp_type) out_type = epics::pvData::pvUShort;
      if (ccs::types::SignedInteger32 == inp_type) out_type = epics::pvData::pvInt;
      if (ccs::types::UnsignedInteger32 == inp_type) out_type = epics::pvData::pvUInt;
      if (ccs::types::SignedInteger64 == inp_type) out_type = epics::pvData::pvLong;
      if (ccs::types::UnsignedInteger64 == inp_type) out_type = epics::pvData::pvULong;
      if (ccs::types::Float32 == inp_type) out_type = epics::pvData::pvFloat;
      if (ccs::types::Float64 == inp_type) out_type = epics::pvData::pvDouble;
      if (ccs::types::String == inp_type) out_type = epics::pvData::pvString;
    }

  return out_type;

}

static inline std::shared_ptr<const epics::pvData::Structure> AnyTypeToPVStruct (const std::shared_ptr<const ccs::types::AnyType>& type)
{

  std::shared_ptr<const ccs::types::CompoundType> inp_type;

  bool status = ccs::HelperTools::Is<ccs::types::CompoundType>(type);

  if (status)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::CompoundType>(type);
      status = (inp_type ? true : false);
    }

  log_debug("Create field builder");
  epics::pvData::FieldBuilderPtr builder = epics::pvData::getFieldCreate()->createFieldBuilder();

  for (uint_t index = 0u; (status && (index < inp_type->GetAttributeNumber())); index += 1u)
    {
      const char* attr_name = inp_type->GetAttributeName(index);
      std::shared_ptr<const ccs::types::AnyType> attr_type = inp_type->GetAttributeType(index);

      if (ccs::HelperTools::Is<ccs::types::ArrayType>(attr_type))
	{
	  std::shared_ptr<const ccs::types::ArrayType> array_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(attr_type);
	  std::shared_ptr<const ccs::types::AnyType> elem_type = array_type->GetElementType();

	  log_debug("AnyTypeToPVStruct - Add array field with name '%s' and size '%u' ..", attr_name, array_type->GetElementNumber());
	  if (ccs::HelperTools::Is<ccs::types::ScalarType>(elem_type))
	    {
	      log_debug("AnyTypeToPVStruct - .. which is scalar");
	      builder->addBoundedArray(std::string(attr_name), AnyTypeToPVScalar(elem_type), array_type->GetElementNumber());
	      //builder->addFixedArray(std::string(attr_name), AnyTypeToPVScalar(array_type->GetElementType()), array_type->GetElementNumber());
	      //builder->addArray(std::string(attr_name), AnyTypeToPVScalar(array_type->GetElementType()));
	    }
	  else if (ccs::HelperTools::Is<ccs::types::CompoundType>(elem_type))
	    {
	      log_debug("AnyTypeToPVStruct - .. which is struct");
	      builder->addArray(std::string(attr_name), AnyTypeToPVStruct(elem_type));
	    }
	  else
	    log_error("AnyTypeToPVStruct - .. which is of unknown type");
	}
      else
	{
	  log_debug("Add field with name '%s'", attr_name);
	  if (ccs::HelperTools::Is<ccs::types::ScalarType>(attr_type))
	    builder->add(std::string(attr_name), AnyTypeToPVScalar(attr_type));
	  else
	    builder->add(std::string(attr_name), AnyTypeToPVStruct(attr_type));
	}
    }

  // Create PVA structure
  log_debug("Create structure with type name '%s'", inp_type->GetName());
  std::shared_ptr<const epics::pvData::Structure> out_type = builder->setId(std::string(inp_type->GetName()))->createStructure();

  return out_type;

}
  
} // namespace HelperTools

} // namespace ccs

#endif // _AnyTypeToPVA_h_


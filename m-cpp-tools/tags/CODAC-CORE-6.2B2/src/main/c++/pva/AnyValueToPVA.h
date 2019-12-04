/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/AnyValueToPVA.h $
* $Id: AnyValueToPVA.h 98970 2019-04-10 12:37:49Z bauvirb $
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
 * @file AnyValueToPVA.h
 * @brief Header file for AnyValueToPVA helper routines.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the AnyValueToPVA helper routines.
 */

#ifndef _AnyValueToPVA_h_
#define _AnyValueToPVA_h_

// Global header files

#include <memory> // std::shared_ptr, etc.

#include <pv/pvData.h>

#include <types.h> // Misc. type definition
#include <tools.h> // Misc. helper functions

#include <log-api.h> // Logging functions

#include <AnyValue.h>
#include <AnyValueHelper.h>

// Local header files

#include "AnyTypeToPVA.h"
#include "PVAToAnyType.h"

// Constants

// Type definition

// Global variables

// Function declaration

namespace ccs {

namespace HelperTools {

template <typename Type> inline bool AnyValueToPVScalar (const ccs::types::AnyValue* inp_value,
							 Type out_value);
template <typename Type> inline bool AnyValueToPVArray (const ccs::types::AnyValue* inp_value,
							Type out_value);
static inline bool AnyValueToPVStruct (const ccs::types::AnyValue* inp_value,
				       std::shared_ptr<epics::pvData::PVStructure> out_value);

// Function definition

#define ANYSCALARTOPVSCALAR(INP_BASIC_TYPE,INP_INTRO_TYPE,OUT_TYPE) \
template <> inline bool AnyValueToPVScalar<std::shared_ptr<OUT_TYPE>> (const ccs::types::AnyValue* inp_value, \
								       std::shared_ptr<OUT_TYPE> out_value) \
{ \
\
  log_trace("AnyValueToPVScalar<" #INP_BASIC_TYPE "> - Entering method"); \
\
  std::shared_ptr<const ccs::types::AnyType> type = inp_value->GetType(); \
  std::shared_ptr<const ccs::types::ScalarType> inp_type; \
\
  bool status = ccs::HelperTools::Is<ccs::types::ScalarType>(type); \
\
  if (status) \
    { \
      inp_type = std::dynamic_pointer_cast<const ccs::types::ScalarType>(type); \
      status = (inp_type ? true : false); \
    } \
\
  if (!status) \
    { \
      log_error("AnyValueToPVScalar<" #INP_BASIC_TYPE "> - Not a scalar"); \
    } \
\
  if (status) \
    { \
      status = (INP_INTRO_TYPE == inp_type); \
    } \
\
  if (!status) \
    { \
      log_error("AnyValueToPVScalar<" #INP_BASIC_TYPE "> - Wrong basic type"); \
    } \
\
  if (status) \
    { \
      out_value->put(*static_cast<const INP_BASIC_TYPE*>(inp_value->GetInstance())); \
    } \
\
  log_trace("AnyValueToPVScalar<" #INP_BASIC_TYPE "> - Leaving method"); \
\
  return status; \
\
}

ANYSCALARTOPVSCALAR(ccs::types::boolean, ccs::types::Boolean, epics::pvData::PVBoolean)
ANYSCALARTOPVSCALAR(ccs::types::int8, ccs::types::SignedInteger8, epics::pvData::PVByte)
ANYSCALARTOPVSCALAR(ccs::types::uint8, ccs::types::UnsignedInteger8, epics::pvData::PVUByte)
ANYSCALARTOPVSCALAR(ccs::types::int16, ccs::types::SignedInteger16, epics::pvData::PVShort)
ANYSCALARTOPVSCALAR(ccs::types::uint16, ccs::types::UnsignedInteger16, epics::pvData::PVUShort)
ANYSCALARTOPVSCALAR(ccs::types::int32, ccs::types::SignedInteger32, epics::pvData::PVInt)
ANYSCALARTOPVSCALAR(ccs::types::uint32, ccs::types::UnsignedInteger32, epics::pvData::PVUInt)
ANYSCALARTOPVSCALAR(ccs::types::int64, ccs::types::SignedInteger64, epics::pvData::PVLong)
ANYSCALARTOPVSCALAR(ccs::types::uint64, ccs::types::UnsignedInteger64, epics::pvData::PVULong)
ANYSCALARTOPVSCALAR(ccs::types::float32, ccs::types::Float32, epics::pvData::PVFloat)
ANYSCALARTOPVSCALAR(ccs::types::float64, ccs::types::Float64, epics::pvData::PVDouble)

template <> inline bool AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVString>> (const ccs::types::AnyValue* inp_value,
										      std::shared_ptr<epics::pvData::PVString> out_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = inp_value->GetType();
  std::shared_ptr<const ccs::types::ScalarType> inp_type;

  bool status = ccs::HelperTools::Is<ccs::types::ScalarType>(type);

  if (status)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::ScalarType>(type);
      status = (inp_type ? true : false);
    }

  if (status)
    {
      status = (ccs::types::String == inp_type);
    }

  if (status)
    {
      out_value->put(std::string(static_cast<const char*>(inp_value->GetInstance())));
    }
  
  return status;

}

#define ANYSCALARARRAYTOPVARRAY(INP_BASIC_TYPE,INP_INTRO_TYPE,OUT_TYPE) \
template <> inline bool AnyValueToPVArray<std::shared_ptr<OUT_TYPE>> (const ccs::types::AnyValue* inp_value, \
								      std::shared_ptr<OUT_TYPE> out_value) \
{ \
\
  log_trace("AnyValueToPVArray<" #INP_BASIC_TYPE "> - Entering method"); \
\
  std::shared_ptr<const ccs::types::AnyType> type = inp_value->GetType(); \
  std::shared_ptr<const ccs::types::ArrayType> inp_type; \
\
  bool status = ccs::HelperTools::Is<ccs::types::ArrayType>(type); \
\
  if (status) \
    { \
      inp_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type); \
      status = (inp_type ? true : false); \
    } \
\
  std::shared_ptr<const ccs::types::AnyType> elem_type; \
\
  if (status) \
    { \
      elem_type = ccs::HelperTools::GetElementType(inp_value); \
      status = (elem_type ? true : false); \
    } \
\
  /* ToDo - Multi-dimension arrays */ \
\
  if (status) \
    { \
      status = (ccs::HelperTools::Is<ccs::types::ScalarType>(elem_type) && (INP_INTRO_TYPE == elem_type)); \
    } \
\
  if (status) \
    { \
\
      OUT_TYPE::svector out_array (inp_type->GetElementNumber()); \
\
      for (ccs::types::uint32 index = 0u; (status && (index < inp_type->GetElementNumber())); index += 1u) \
	{ \
	  out_array[index] = ccs::HelperTools::GetElementValue<INP_BASIC_TYPE>(inp_value, index); \
	} \
\
      out_value->replace(freeze(out_array)); \
\
    } \
\
  log_trace("AnyValueToPVArray<" #INP_BASIC_TYPE "> - Leaving method"); \
\
  return status; \
\
}

ANYSCALARARRAYTOPVARRAY(ccs::types::boolean, ccs::types::Boolean, epics::pvData::PVBooleanArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::int8, ccs::types::SignedInteger8, epics::pvData::PVByteArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::uint8, ccs::types::UnsignedInteger8, epics::pvData::PVUByteArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::int16, ccs::types::SignedInteger16, epics::pvData::PVShortArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::uint16, ccs::types::UnsignedInteger16, epics::pvData::PVUShortArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::int32, ccs::types::SignedInteger32, epics::pvData::PVIntArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::uint32, ccs::types::UnsignedInteger32, epics::pvData::PVUIntArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::int64, ccs::types::SignedInteger64, epics::pvData::PVLongArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::uint64, ccs::types::UnsignedInteger64, epics::pvData::PVULongArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::float32, ccs::types::Float32, epics::pvData::PVFloatArray)
ANYSCALARARRAYTOPVARRAY(ccs::types::float64, ccs::types::Float64, epics::pvData::PVDoubleArray)

template <> inline bool AnyValueToPVArray<std::shared_ptr<epics::pvData::PVStringArray>> (const ccs::types::AnyValue* inp_value,
											  std::shared_ptr<epics::pvData::PVStringArray> out_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = inp_value->GetType();
  std::shared_ptr<const ccs::types::ArrayType> inp_type;

  bool status = ccs::HelperTools::Is<ccs::types::ArrayType>(type);

  if (status)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type);
      status = (inp_type ? true : false);
    }

  std::shared_ptr<const ccs::types::AnyType> elem_type;

  if (status)
    {
      elem_type = ccs::HelperTools::GetElementType(inp_value);
      status = (elem_type ? true : false);
    }

  // ToDo - Multi-dimension arrays

  if (status)
    {
      status = (ccs::HelperTools::Is<ccs::types::ScalarType>(elem_type) && (ccs::types::String == elem_type));
    }

  if (status)
    {
      
      epics::pvData::PVStringArray::svector out_array (inp_type->GetElementNumber());
      
      for (ccs::types::uint32 index = 0u; (status && (index < inp_type->GetElementNumber())); index += 1u)
	{
	  out_array[index] = std::string(static_cast<const char*>(ccs::HelperTools::GetElementReference(inp_value, index)));
	}
      
      out_value->replace(freeze(out_array));
      
    }
  
  return status;

}

template <> inline bool AnyValueToPVArray<std::shared_ptr<epics::pvData::PVStructureArray>> (const ccs::types::AnyValue* inp_value,
											     std::shared_ptr<epics::pvData::PVStructureArray> out_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = inp_value->GetType();
  std::shared_ptr<const ccs::types::ArrayType> inp_type;

  bool status = ccs::HelperTools::Is<ccs::types::ArrayType>(type);

  if (status)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type);
      status = (inp_type ? true : false);
    }

  std::shared_ptr<const ccs::types::AnyType> elem_type;

  if (status)
    {
      elem_type = ccs::HelperTools::GetElementType(inp_value);
      status = (elem_type ? true : false);
    }

  // ToDo - Multi-dimension arrays

  if (status)
    {
      status = ccs::HelperTools::Is<ccs::types::CompoundType>(elem_type);
    }

  if (status)
    {

      // ToDo - Explicit type
      epics::pvData::PVStructureArray::svector out_array (inp_type->GetElementNumber());

      for (ccs::types::uint32 index = 0u; (status && (index < inp_type->GetElementNumber())); index += 1u)
	{
	  ccs::types::AnyValue attr_value = ccs::HelperTools::GetElementValue<ccs::types::AnyValue>(inp_value, index);
	  const ccs::types::AnyValue* nst_value = &attr_value;

	  out_array[index] = epics::pvData::getPVDataCreate()->createPVStructure(ccs::HelperTools::AnyTypeToPVStruct(elem_type));

	  status = AnyValueToPVStruct(nst_value, out_array[index]);
	}

      out_value->replace(freeze(out_array));

    }

  return status;

}

static inline bool AnyValueToPVStruct (const ccs::types::AnyValue* inp_value,
				       std::shared_ptr<epics::pvData::PVStructure> out_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = inp_value->GetType();
  std::shared_ptr<const ccs::types::CompoundType> inp_type;

  bool status = ccs::HelperTools::Is<ccs::types::CompoundType>(type);

  if (status)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::CompoundType>(type);
      status = (inp_type ? true : false);
    }

  for (ccs::types::uint32 index = 0u; (status && (index < inp_type->GetAttributeNumber())); index += 1u)
    {
      const char* attr_name = inp_type->GetAttributeName(index);
      std::shared_ptr<const ccs::types::AnyType> attr_type = ccs::HelperTools::GetAttributeType(inp_value, attr_name);

      log_debug("AnyValueToPVStruct - Attribute '%s' with type '%s' ..", attr_name, attr_type->GetName());
      ccs::types::AnyValue attr_value = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(inp_value, attr_name);
      const ccs::types::AnyValue* nst_value = &attr_value;

      if (ccs::HelperTools::Is<ccs::types::ScalarType>(attr_type))
	{
	  log_debug("AnyValueToPVStruct - .. is scalar");
	  if (ccs::types::Boolean == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVBoolean>>(nst_value, out_value->getSubField<epics::pvData::PVBoolean>(std::string(attr_name)));
	  if (ccs::types::SignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVByte>>(nst_value, out_value->getSubField<epics::pvData::PVByte>(std::string(attr_name)));
	  if (ccs::types::UnsignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVUByte>>(nst_value, out_value->getSubField<epics::pvData::PVUByte>(std::string(attr_name)));
	  if (ccs::types::SignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVShort>>(nst_value, out_value->getSubField<epics::pvData::PVShort>(std::string(attr_name)));
	  if (ccs::types::UnsignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVUShort>>(nst_value, out_value->getSubField<epics::pvData::PVUShort>(std::string(attr_name)));
	  if (ccs::types::SignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVInt>>(nst_value, out_value->getSubField<epics::pvData::PVInt>(std::string(attr_name)));
	  if (ccs::types::UnsignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVUInt>>(nst_value, out_value->getSubField<epics::pvData::PVUInt>(std::string(attr_name)));
	  if (ccs::types::SignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVLong>>(nst_value, out_value->getSubField<epics::pvData::PVLong>(std::string(attr_name)));
	  if (ccs::types::UnsignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVULong>>(nst_value, out_value->getSubField<epics::pvData::PVULong>(std::string(attr_name)));
	  if (ccs::types::Float32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVFloat>>(nst_value, out_value->getSubField<epics::pvData::PVFloat>(std::string(attr_name)));
	  if (ccs::types::Float64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVDouble>>(nst_value, out_value->getSubField<epics::pvData::PVDouble>(std::string(attr_name)));
	  if (ccs::types::String == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = AnyValueToPVScalar<std::shared_ptr<epics::pvData::PVString>>(nst_value, out_value->getSubField<epics::pvData::PVString>(std::string(attr_name)));
	}
#if 0
      else if (ccs::HelperTools::Is<ccs::types::ScalarArray>(attr_type))
	{
	  log_debug("AnyValueToPVStruct - .. is array of scalar");
	  if (ccs::types::Boolean == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVBooleanArray>>(nst_value, out_value->getSubField<epics::pvData::PVBooleanArray>(attr_name));
	  if (ccs::types::SignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVByteArray>>(nst_value, out_value->getSubField<epics::pvData::PVByteArray>(attr_name));
	  if (ccs::types::UnsignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVUByteArray>>(nst_value, out_value->getSubField<epics::pvData::PVUByteArray>(attr_name));
	  if (ccs::types::SignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVShortArray>>(nst_value, out_value->getSubField<epics::pvData::PVShortArray>(attr_name));
	  if (ccs::types::UnsignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVUShortArray>>(nst_value, out_value->getSubField<epics::pvData::PVUShortArray>(attr_name));
	  if (ccs::types::SignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVIntArray>>(nst_value, out_value->getSubField<epics::pvData::PVIntArray>(attr_name));
	  if (ccs::types::UnsignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVUIntArray>>(nst_value, out_value->getSubField<epics::pvData::PVUIntArray>(attr_name));
	  if (ccs::types::SignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVLongArray>>(nst_value, out_value->getSubField<epics::pvData::PVLongArray>(attr_name));
	  if (ccs::types::UnsignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVULongArray>>(nst_value, out_value->getSubField<epics::pvData::PVULongArray>(attr_name));
	  if (ccs::types::Float32 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVFloatArray>>(nst_value, out_value->getSubField<epics::pvData::PVFloatArray>(attr_name));
	  if (ccs::types::Float64 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVDoubleArray>>(nst_value, out_value->getSubField<epics::pvData::PVDoubleArray>(attr_name));
	  if (ccs::types::String == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVStringArray>>(nst_value, out_value->getSubField<epics::pvData::PVStringArray>(attr_name));
	}
      else if (ccs::HelperTools::Is<ccs::types::CompoundArray>(attr_type))
	{
	  log_debug("AnyValueToPVStruct - .. is array of struct");
	  status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVStructureArray>>(nst_value, out_value->getSubField<epics::pvData::PVStructureArray>(attr_name));
	}
#else
      else if (ccs::HelperTools::Is<ccs::types::ArrayType>(attr_type))
	{
	  std::shared_ptr<const ccs::types::AnyType> elem_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(attr_type)->GetElementType();

	  if (ccs::HelperTools::Is<ccs::types::CompoundType>(elem_type))
	    {
	      log_debug("AnyValueToPVStruct - .. is array of struct");
	      status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVStructureArray>>(nst_value, out_value->getSubField<epics::pvData::PVStructureArray>(attr_name));
	    }
	  else
	    {
	      log_debug("AnyValueToPVStruct - .. is array of scalar");
	      if (ccs::types::Boolean == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVBooleanArray>>(nst_value, out_value->getSubField<epics::pvData::PVBooleanArray>(attr_name));
	      else if (ccs::types::SignedInteger8 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVByteArray>>(nst_value, out_value->getSubField<epics::pvData::PVByteArray>(attr_name));
	      else if (ccs::types::UnsignedInteger8 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVUByteArray>>(nst_value, out_value->getSubField<epics::pvData::PVUByteArray>(attr_name));
	      else if (ccs::types::SignedInteger16 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVShortArray>>(nst_value, out_value->getSubField<epics::pvData::PVShortArray>(attr_name));
	      else if (ccs::types::UnsignedInteger16 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVUShortArray>>(nst_value, out_value->getSubField<epics::pvData::PVUShortArray>(attr_name));
	      else if (ccs::types::SignedInteger32 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVIntArray>>(nst_value, out_value->getSubField<epics::pvData::PVIntArray>(attr_name));
	      else if (ccs::types::UnsignedInteger32 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVUIntArray>>(nst_value, out_value->getSubField<epics::pvData::PVUIntArray>(attr_name));
	      else if (ccs::types::SignedInteger64 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVLongArray>>(nst_value, out_value->getSubField<epics::pvData::PVLongArray>(attr_name));
	      else if (ccs::types::UnsignedInteger64 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVULongArray>>(nst_value, out_value->getSubField<epics::pvData::PVULongArray>(attr_name));
	      else if (ccs::types::Float32 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVFloatArray>>(nst_value, out_value->getSubField<epics::pvData::PVFloatArray>(attr_name));
	      else if (ccs::types::Float64 == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVDoubleArray>>(nst_value, out_value->getSubField<epics::pvData::PVDoubleArray>(attr_name));
	      else if (ccs::types::String == elem_type) 
		status = AnyValueToPVArray<std::shared_ptr<epics::pvData::PVStringArray>>(nst_value, out_value->getSubField<epics::pvData::PVStringArray>(attr_name));
	      else
		status = false;
	    }
	}
#endif
      else if (ccs::HelperTools::Is<ccs::types::CompoundType>(attr_type))
	{
	  log_debug("AnyValueToPVStruct - .. is struct");
	  status = AnyValueToPVStruct(nst_value, out_value->getSubField<epics::pvData::PVStructure>(attr_name));
	}
      else
	{
	  log_error("AnyValueToPVStruct - .. is of unknown type");
	  status = false;
	}
    }

  return status;

}
  
} // namespace HelperTools

} // namespace ccs

#endif // _AnyValueToPVA_h_


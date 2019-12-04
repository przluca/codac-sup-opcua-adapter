/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAToAnyValue.h $
* $Id: PVAToAnyValue.h 99248 2019-04-23 10:41:47Z bauvirb $
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
 * @file PVAToAnyValue.h
 * @brief Header file for PVAToAnyValue helper routines.
 * @date 01/11/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the PVAToAnyValue helper routines.
 */

#ifndef _PVAToAnyValue_h_
#define _PVAToAnyValue_h_

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

/**
 * @brief PVAccess to AnyValue conversion method.
 * @detail The conversion method must be provided two variables of compatible types.
 * @param out_value ccs::types::AnyValue instance
 * @param inp_value epics::pvData::PV<Type>(*) instance
 * @pre
 *   ccs::HelperTools::IsScalarType(out_value) == true &&
 *   out_value->GetType() is compatible with template <Type>.
 * @return True if conversion successful.
 */

template <typename Type> inline bool PVScalarToAnyValue (ccs::types::AnyValue* out_value,
							 const Type& inp_value);

/**
 * @brief PVAccess to AnyValue conversion method.
 * @detail The conversion method must be provided two variables of compatible types.
 * @param out_value ccs::types::AnyValue instance
 * @param inp_value epics::pvData::PV<Type>Array(*) instance
 * @pre
 *   ccs::HelperTools::IsArrayType(out_value) == true &&
 *   out_value->GetElementType() is compatible with template <Type> array.
 * @return True if conversion successful.
 */

template <typename Type> inline bool PVArrayToAnyValue (ccs::types::AnyValue* out_value,
							const Type& inp_value);

static inline bool PVStructToAnyValue (ccs::types::AnyValue* out_value,
				       const std::shared_ptr<const epics::pvData::PVStructure>& inp_value);

// Function definition

#define PVSCALARTOANYSCALAR(OUT_BASIC_TYPE,OUT_INTRO_TYPE,INP_TYPE) \
template <> inline bool PVScalarToAnyValue<std::shared_ptr<const INP_TYPE>> (ccs::types::AnyValue* out_value, \
									     const std::shared_ptr<const INP_TYPE>& inp_value) \
{ \
\
  log_trace("PVScalarToAnyValue<" #OUT_BASIC_TYPE "> - Entering method"); \
\
  std::shared_ptr<const ccs::types::AnyType> type = out_value->GetType(); \
  std::shared_ptr<const ccs::types::ScalarType> out_type; \
\
  bool status = (static_cast<bool>(inp_value) && (static_cast<ccs::types::AnyValue*>(NULL) != out_value)); \
\
  if (status) \
    { \
      status = ccs::HelperTools::Is<ccs::types::ScalarType>(type); \
    } \
\
  if (status) \
    { \
      out_type = std::dynamic_pointer_cast<const ccs::types::ScalarType>(type); \
      status = (out_type ? true : false); \
    } \
\
  if (!status) \
    { \
      log_error("PVScalarToAnyValue<" #OUT_BASIC_TYPE "> - Not a scalar"); \
    } \
\
  if (status) \
    { \
      status = (OUT_INTRO_TYPE == out_type); \
    } \
\
  if (!status) \
    { \
      log_error("PVScalarToAnyValue<" #OUT_BASIC_TYPE "> - Wrong basic type"); \
    } \
\
  if (status) \
    { \
      *out_value = inp_value->get(); /* Copy assignment operator */ \
    } \
\
  log_trace("PVScalarToAnyValue<" #OUT_BASIC_TYPE "> - Leaving method"); \
\
  return status; \
\
}

PVSCALARTOANYSCALAR(ccs::types::boolean, ccs::types::Boolean, epics::pvData::PVBoolean)
PVSCALARTOANYSCALAR(ccs::types::int8, ccs::types::SignedInteger8, epics::pvData::PVByte)
PVSCALARTOANYSCALAR(ccs::types::uint8, ccs::types::UnsignedInteger8, epics::pvData::PVUByte)
PVSCALARTOANYSCALAR(ccs::types::int16, ccs::types::SignedInteger16, epics::pvData::PVShort)
PVSCALARTOANYSCALAR(ccs::types::uint16, ccs::types::UnsignedInteger16, epics::pvData::PVUShort)
PVSCALARTOANYSCALAR(ccs::types::int32, ccs::types::SignedInteger32, epics::pvData::PVInt)
PVSCALARTOANYSCALAR(ccs::types::uint32, ccs::types::UnsignedInteger32, epics::pvData::PVUInt)
PVSCALARTOANYSCALAR(ccs::types::int64, ccs::types::SignedInteger64, epics::pvData::PVLong)
PVSCALARTOANYSCALAR(ccs::types::uint64, ccs::types::UnsignedInteger64, epics::pvData::PVULong)
PVSCALARTOANYSCALAR(ccs::types::float32, ccs::types::Float32, epics::pvData::PVFloat)
PVSCALARTOANYSCALAR(ccs::types::float64, ccs::types::Float64, epics::pvData::PVDouble)

template <> inline bool PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVString>> (ccs::types::AnyValue* out_value,
											    const std::shared_ptr<const epics::pvData::PVString>& inp_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = out_value->GetType();
  std::shared_ptr<const ccs::types::ScalarType> out_type;

  bool status = ccs::HelperTools::Is<ccs::types::ScalarType>(type);

  if (status)
    {
      out_type = std::dynamic_pointer_cast<const ccs::types::ScalarType>(type);
      status = (out_type ? true : false);
    }

  if (status)
    {
      status = (ccs::types::String == out_type);
    }

  if (status)
    {
      ccs::HelperTools::SafeStringCopy(static_cast<char*>(out_value->GetInstance()),
				       inp_value->get().c_str(),
				       STRING_MAX_LENGTH);
    }
  
  return status;

}

#define PVARRAYTOANYSCALAR(OUT_BASIC_TYPE,OUT_INTRO_TYPE,INP_TYPE) \
template <> inline bool PVArrayToAnyValue<std::shared_ptr<const INP_TYPE>> (ccs::types::AnyValue* out_value, \
									    const std::shared_ptr<const INP_TYPE>& inp_value) \
{ \
\
  std::shared_ptr<const ccs::types::AnyType> type = out_value->GetType(); \
  std::shared_ptr<const ccs::types::ArrayType> out_type; \
\
  bool status = (static_cast<bool>(inp_value) && (static_cast<ccs::types::AnyValue*>(NULL) != out_value)); \
\
  if (status) \
    { \
      status = ccs::HelperTools::Is<ccs::types::ArrayType>(type); \
    } \
\
  if (status) \
    { \
      out_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type); \
      status = (out_type ? true : false); \
    } \
\
  std::shared_ptr<const ccs::types::AnyType> elem_type; \
\
  if (status) \
    { \
      elem_type = ccs::HelperTools::GetElementType(out_value); \
      status = (elem_type ? true : false); \
    } \
\
  /* ToDo - Multi-dimension arrays */ \
\
  if (status) \
    { \
      status = (ccs::HelperTools::Is<ccs::types::ScalarType>(elem_type) && (OUT_INTRO_TYPE == elem_type)); \
    } \
\
  if (status) \
    { \
\
      INP_TYPE::const_svector inp_array = inp_value->view(); \
\
      for (ccs::types::uint32 index = 0u; (status && (index < inp_array.size())); index += 1u) \
	{ \
	  status = ccs::HelperTools::SetElementValue<OUT_BASIC_TYPE>(out_value, index, inp_array[index]); \
	} \
\
    } \
\
  return status; \
\
}

PVARRAYTOANYSCALAR(ccs::types::boolean, ccs::types::Boolean, epics::pvData::PVBooleanArray)
PVARRAYTOANYSCALAR(ccs::types::int8, ccs::types::SignedInteger8, epics::pvData::PVByteArray)
PVARRAYTOANYSCALAR(ccs::types::uint8, ccs::types::UnsignedInteger8, epics::pvData::PVUByteArray)
PVARRAYTOANYSCALAR(ccs::types::int16, ccs::types::SignedInteger16, epics::pvData::PVShortArray)
PVARRAYTOANYSCALAR(ccs::types::uint16, ccs::types::UnsignedInteger16, epics::pvData::PVUShortArray)
PVARRAYTOANYSCALAR(ccs::types::int32, ccs::types::SignedInteger32, epics::pvData::PVIntArray)
PVARRAYTOANYSCALAR(ccs::types::uint32, ccs::types::UnsignedInteger32, epics::pvData::PVUIntArray)
PVARRAYTOANYSCALAR(ccs::types::int64, ccs::types::SignedInteger64, epics::pvData::PVLongArray)
PVARRAYTOANYSCALAR(ccs::types::uint64, ccs::types::UnsignedInteger64, epics::pvData::PVULongArray)
PVARRAYTOANYSCALAR(ccs::types::float32, ccs::types::Float32, epics::pvData::PVFloatArray)
PVARRAYTOANYSCALAR(ccs::types::float64, ccs::types::Float64, epics::pvData::PVDoubleArray)

template <> inline bool PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVStringArray>> (ccs::types::AnyValue* out_value,
												const std::shared_ptr<const epics::pvData::PVStringArray>& inp_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = out_value->GetType();
  std::shared_ptr<const ccs::types::ArrayType> out_type;

  bool status = ccs::HelperTools::Is<ccs::types::ArrayType>(type);

  if (status)
    {
      out_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type);
      status = (out_type ? true : false);
    }

  std::shared_ptr<const ccs::types::AnyType> elem_type;

  if (status)
    {
      elem_type = ccs::HelperTools::GetElementType(out_value);
      status = (elem_type ? true : false);
    }

  // ToDo - Multi-dimension arrays

  if (status)
    {
      status = (ccs::HelperTools::Is<ccs::types::ScalarType>(elem_type) && (ccs::types::String == elem_type));
    }

  if (status)
    {

      epics::pvData::PVStringArray::const_svector inp_array = inp_value->view();
      
      for (ccs::types::uint32 index = 0u; (status && (index < inp_array.size())); index += 1u)
	{
	  ccs::HelperTools::SafeStringCopy(static_cast<char*>(ccs::HelperTools::GetElementReference(out_value, index)),
					   inp_array[index].c_str(),
					   STRING_MAX_LENGTH);
	}
      
    }

  return status;

}

template <> inline bool PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVStructureArray>> (ccs::types::AnyValue* out_value,
												   const std::shared_ptr<const epics::pvData::PVStructureArray>& inp_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = out_value->GetType();
  std::shared_ptr<const ccs::types::ArrayType> out_type;

  bool status = ccs::HelperTools::Is<ccs::types::ArrayType>(type);

  if (status)
    {
      out_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(type);
      status = (out_type ? true : false);
    }

  std::shared_ptr<const ccs::types::AnyType> elem_type;

  if (status)
    {
      elem_type = ccs::HelperTools::GetElementType(out_value);
      status = (elem_type ? true : false);
    }

  // ToDo - Multi-dimension arrays

  if (status)
    {
      status = ccs::HelperTools::Is<ccs::types::CompoundType>(elem_type);
    }

  if (status)
    {

      epics::pvData::PVStructureArray::const_svector inp_array = inp_value->view();
#if 0 // Reference is the AnyType
      for (ccs::types::uint32 index = 0u; (status && (index < inp_array.size())); index += 1u)
#else
      ccs::types::uint32 array_size = ((inp_array.size() > ccs::HelperTools::GetElementNumber(out_value)) ?
				       ccs::HelperTools::GetElementNumber(out_value) :
				       inp_array.size());

      for (ccs::types::uint32 index = 0u; (status && (index < array_size)); index += 1u)
#endif
	{
	  void* elem_inst = ccs::HelperTools::GetElementReference(out_value, index);

	  ccs::types::AnyValue nst_value (elem_type, elem_inst); // Sub-structure reference

	  status = PVStructToAnyValue(&nst_value, inp_array[index]);
	}

    }

  return status;

}

static inline bool PVStructToAnyValue (ccs::types::AnyValue* out_value,
				       const std::shared_ptr<const epics::pvData::PVStructure>& inp_value)
{

  std::shared_ptr<const ccs::types::AnyType> type = out_value->GetType();
  std::shared_ptr<const ccs::types::CompoundType> out_type;

  bool status = ccs::HelperTools::Is<ccs::types::CompoundType>(type);

  if (status)
    {
      out_type = std::dynamic_pointer_cast<const ccs::types::CompoundType>(type);
      status = (out_type ? true : false);
    }

  for (ccs::types::uint32 index = 0u; (status && (index < out_type->GetAttributeNumber())); index += 1u)
    {
      const char* attr_name = out_type->GetAttributeName(index);
      void* attr_inst = ccs::HelperTools::GetAttributeReference(out_value, attr_name);
      std::shared_ptr<const ccs::types::AnyType> attr_type = ccs::HelperTools::GetAttributeType(out_value, attr_name);

      log_debug("PVStructToAnyValue - Attribute '%s' with type '%s' ..", attr_name, attr_type->GetName());

      ccs::types::AnyValue attr_value (attr_type, attr_inst); // Sub-structure reference

      if (ccs::HelperTools::Is<ccs::types::ScalarType>(attr_type))
	{
	  log_debug("PVStructToAnyValue - .. is scalar");
	  if (ccs::types::Boolean == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVBoolean>>(&attr_value, inp_value->getSubField<epics::pvData::PVBoolean>(std::string(attr_name)));
	  else if (ccs::types::SignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVByte>>(&attr_value, inp_value->getSubField<epics::pvData::PVByte>(std::string(attr_name)));
	  else if (ccs::types::UnsignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVUByte>>(&attr_value, inp_value->getSubField<epics::pvData::PVUByte>(std::string(attr_name)));
	  else if (ccs::types::SignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVShort>>(&attr_value, inp_value->getSubField<epics::pvData::PVShort>(std::string(attr_name)));
	  else if (ccs::types::UnsignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVUShort>>(&attr_value, inp_value->getSubField<epics::pvData::PVUShort>(std::string(attr_name)));
	  else if (ccs::types::SignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVInt>>(&attr_value, inp_value->getSubField<epics::pvData::PVInt>(std::string(attr_name)));
	  else if (ccs::types::UnsignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVUInt>>(&attr_value, inp_value->getSubField<epics::pvData::PVUInt>(std::string(attr_name)));
	  else if (ccs::types::SignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVLong>>(&attr_value, inp_value->getSubField<epics::pvData::PVLong>(std::string(attr_name)));
	  else if (ccs::types::UnsignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVULong>>(&attr_value, inp_value->getSubField<epics::pvData::PVULong>(std::string(attr_name)));
	  else if (ccs::types::Float32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVFloat>>(&attr_value, inp_value->getSubField<epics::pvData::PVFloat>(std::string(attr_name)));
	  else if (ccs::types::Float64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVDouble>>(&attr_value, inp_value->getSubField<epics::pvData::PVDouble>(std::string(attr_name)));
	  else if (ccs::types::String == std::dynamic_pointer_cast<const ccs::types::ScalarType>(attr_type)) 
	    status = PVScalarToAnyValue<std::shared_ptr<const epics::pvData::PVString>>(&attr_value, inp_value->getSubField<epics::pvData::PVString>(std::string(attr_name)));
	}
#if 0
      else if (ccs::HelperTools::Is<ccs::types::ScalarArray>(attr_type))
	{
	  log_debug("PVStructToAnyValue - .. is array of scalar");
	  if (ccs::types::Boolean == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVBooleanArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVBooleanArray>(attr_name));
	  else if (ccs::types::SignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVByteArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVByteArray>(attr_name));
	  else if (ccs::types::UnsignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVUByteArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVUByteArray>(attr_name));
	  else if (ccs::types::SignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVShortArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVShortArray>(attr_name));
	  else if (ccs::types::UnsignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVUShortArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVUShortArray>(attr_name));
	  else if (ccs::types::SignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVIntArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVIntArray>(attr_name));
	  else if (ccs::types::UnsignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVUIntArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVUIntArray>(attr_name));
	  else if (ccs::types::SignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVLongArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVLongArray>(attr_name));
	  else if (ccs::types::UnsignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVULongArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVULongArray>(attr_name));
	  else if (ccs::types::Float32 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVFloatArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVFloatArray>(attr_name));
	  else if (ccs::types::Float64 == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVDoubleArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVDoubleArray>(attr_name));
	  else if (ccs::types::String == std::dynamic_pointer_cast<const ccs::types::ScalarArray>(attr_type)->GetElementType()) 
	    status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVStringArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVStringArray>(attr_name));
	}
      else if (ccs::HelperTools::Is<ccs::types::CompoundArray>(attr_type))
	{
	  log_debug("PVStructToAnyValue - .. is array of struct");
	  status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVStructureArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVStructureArray>(attr_name));
	}
#else
      else if (ccs::HelperTools::Is<ccs::types::ArrayType>(attr_type))
	{
	  std::shared_ptr<const ccs::types::AnyType> elem_type = std::dynamic_pointer_cast<const ccs::types::ArrayType>(attr_type)->GetElementType();

	  if (ccs::HelperTools::Is<ccs::types::CompoundType>(elem_type))
	    {
	      log_debug("PVStructToAnyValue - .. is array of struct");
	      status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVStructureArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVStructureArray>(attr_name));
	    }
	  else 
	    {
	      log_debug("PVStructToAnyValue - .. is array of scalar");
	      if (ccs::types::Boolean == elem_type)
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVBooleanArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVBooleanArray>(attr_name));
	      else if (ccs::types::SignedInteger8 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVByteArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVByteArray>(attr_name));
	      else if (ccs::types::UnsignedInteger8 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVUByteArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVUByteArray>(attr_name));
	      else if (ccs::types::SignedInteger16 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVShortArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVShortArray>(attr_name));
	      else if (ccs::types::UnsignedInteger16 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVUShortArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVUShortArray>(attr_name));
	      else if (ccs::types::SignedInteger32 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVIntArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVIntArray>(attr_name));
	      else if (ccs::types::UnsignedInteger32 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVUIntArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVUIntArray>(attr_name));
	      else if (ccs::types::SignedInteger64 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVLongArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVLongArray>(attr_name));
	      else if (ccs::types::UnsignedInteger64 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVULongArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVULongArray>(attr_name));
	      else if (ccs::types::Float32 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVFloatArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVFloatArray>(attr_name));
	      else if (ccs::types::Float64 == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVDoubleArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVDoubleArray>(attr_name));
	      else if (ccs::types::String == elem_type) 
		status = PVArrayToAnyValue<std::shared_ptr<const epics::pvData::PVStringArray>>(&attr_value, inp_value->getSubField<epics::pvData::PVStringArray>(attr_name));
	      else
		status = false;
	    }
	}
#endif
      else if (ccs::HelperTools::Is<ccs::types::CompoundType>(attr_type))
	{
	  log_debug("PVStructToAnyValue - .. is struct");
	  status = PVStructToAnyValue(&attr_value, inp_value->getSubField<epics::pvData::PVStructure>(attr_name));
	}
      else
	{
	  log_error("PVStructToAnyValue - .. is of unknown type");
	  status = false;
	}
    }

  return status;

}
  
} // namespace HelperTools

} // namespace ccs

#endif // _PVAToAnyValue_h_


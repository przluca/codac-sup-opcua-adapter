/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/any-type-ext.cpp $
* $Id: any-type-ext.cpp 96477 2019-01-09 14:53:55Z sivecm $
*
* Project	: CODAC Core System
*
* Description	: SDN - Infrastructure tools - Prototype
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

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */

/* Local header files */

#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */
#include "sdn-tools.h" /* Misc. helper functions, e.g. Hash<uint16_t>, etc. */

#include "any-type-ext.h" /* This class definition */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sdn::base"

/* Type definition */

namespace sdn {

namespace base {

/* Global variables */

/* Function declaration */

/* Function definition */

/* Initializer methods */

RET_STATUS AnyType::AddExtAttribute (const char* rank, const char* name, const char* desc, const char* qual, const char* unit, const char* mult)
{

  log_debug("AnyType::AddExtAttribute('%s %s %s %s %s %s') - Entering method", rank, name, desc, qual, unit, mult);

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(name) == false) 
    {
      log_error("AnyType::AddExtAttribute - Undefined attribute '%s'", name);
      this->m_defined = false; 
      return status;
    }
  else
    {
      log_debug("AnyType::AddExtAttribute - Attribute '%s' exists", name);
    }

  uint_t rank_ = (this->m_type_ext)->GetSize(); if (IsUndefined(rank) != true) sscanf(rank, "%u", &rank_);

  if (rank_ > MAXIMUM_ATTRIBUTE_NUM) /* Scalability issue */
    {
      log_error("AnyType::AddExtAttribute - Out-of-bound attribute '%s' rank '%d'", name, rank_);
      this->m_defined = false;
      return status;
    }

  /* Bug 8733 - Attributes defined with identical rank specification overwrite each other */

  if ((this->m_type_ext)->IsValid(rank_) == true) /* An attribute already exists at this location */
    {
      log_error("AnyType::AddExtAttribute - An attribute '%s' already exists at rank '%d'", (this->m_type_ext)->GetKeyword(rank_), rank_);
      this->m_defined = false;
      return status; 
    }

  ExtTypeInfo_t info; 

  info.rank = rank_; 

  sstrncpy((char*) info.desc, (IsUndefined(desc) ? (char*) STRING_UNDEFINED : desc), STRING_MAX_LENGTH);
  sstrncpy((char*) info.qual, (IsUndefined(qual) ? (char*) STRING_UNDEFINED : qual), STRING_MAX_LENGTH);
  sstrncpy((char*) info.unit, (IsUndefined(unit) ? (char*) STRING_UNDEFINED : unit), STRING_MAX_LENGTH);
  sstrncpy((char*) info.mult, (IsUndefined(mult) ? (char*) STRING_UNDEFINED : mult), STRING_MAX_LENGTH);

  if ((this->m_type_ext)->AddPair(name, info, rank_) != STATUS_SUCCESS) /* Create attribute at LUTable index corresponding to rank and not in the order they are declared */
    { 
      log_error("AnyType::AddExtAttribute - Unable to add attribute '%s' at rank '%d'", name, rank_);
      this->m_defined = false;
      return status; 
    }
  else
    {
      log_debug("AnyType::AddExtAttribute - LUTable<>::AddPair('%s %s %s %s %s %u') successful", name, info.desc, info.qual, info.unit, info.mult, rank_);
    }

  status = STATUS_SUCCESS;

  log_trace("AnyType::AddExtAttribute('%s %s %s %s %s %s') - Leaving method", rank, name, desc, qual, unit, mult);

  return status;
 
}

void AnyType::Initialize (void)
{

  log_trace("AnyType::Initialize - Entering method");

  /* Initialize attributes */
  this->SetName(DEFAULT_TOPIC_NAME);

  this->m_type_ext = new LUTable<ExtTypeInfo_t> (MAXIMUM_ATTRIBUTE_NUM); 
  
  ExtTypeInfo_t info; info.rank = 0; 

  sstrncpy((char*) info.desc, (char*) STRING_UNDEFINED, STRING_MAX_LENGTH);
  sstrncpy((char*) info.qual, (char*) STRING_UNDEFINED, STRING_MAX_LENGTH);
  sstrncpy((char*) info.unit, (char*) STRING_UNDEFINED, STRING_MAX_LENGTH);
  sstrncpy((char*) info.mult, (char*) STRING_UNDEFINED, STRING_MAX_LENGTH);
  
  (this->m_type_ext)->SetDefault(info); 
  (this->m_type_ext)->Clear();

  log_trace("AnyType::Initialize - Leaving method");

  return;

}

/* Accessor methods */

const char* AnyType::GetAttributeDescription (uint_t rank) const
{

  const char* ret = NULL;

  bool ok = (static_cast<LUTable<ExtTypeInfo_t>*>(NULL) != this->m_type_ext);

  if (ok)
    {
      ok = ((this->m_type_ext)->IsValid(rank) == true);
    }

  ExtTypeInfo_t* p_info = static_cast<ExtTypeInfo_t*>(NULL);

  if (ok)
    {
      p_info = (this->m_type_ext)->GetReference(rank);
      ok = (IsUndefined(p_info->desc) == false);
    }

  if (ok)
    {
      ret = p_info->desc;
    }

  return const_cast<char*>(ret);

}

const char* AnyType::GetAttributeQualifier (uint_t rank) const
{

  const char* ret = NULL;

  bool ok = (static_cast<LUTable<ExtTypeInfo_t>*>(NULL) != this->m_type_ext);

  if (ok)
    {
      ok = ((this->m_type_ext)->IsValid(rank) == true);
    }

  ExtTypeInfo_t* p_info = static_cast<ExtTypeInfo_t*>(NULL);

  if (ok)
    {
      p_info = (this->m_type_ext)->GetReference(rank);
      ok = (IsUndefined(p_info->qual) == false);
    }

  if (ok)
    {
      ret = p_info->qual;
    }

  return const_cast<char*>(ret);

}

const char* AnyType::GetAttributeUnit (uint_t rank) const
{

  const char* ret = NULL;

  bool ok = (static_cast<LUTable<ExtTypeInfo_t>*>(NULL) != this->m_type_ext);

  if (ok)
    {
      ok = ((this->m_type_ext)->IsValid(rank) == true);
    }

  ExtTypeInfo_t* p_info = static_cast<ExtTypeInfo_t*>(NULL);

  if (ok)
    {
      p_info = (this->m_type_ext)->GetReference(rank);
      ok = (IsUndefined(p_info->unit) == false);
    }

  if (ok)
    {
      ret = p_info->unit;
    }

  return const_cast<char*>(ret);

}

const char* AnyType::GetAttributeMultiplicity (uint_t rank) const
{

  const char* ret = NULL;

  bool ok = (static_cast<LUTable<ExtTypeInfo_t>*>(NULL) != this->m_type_ext);

  if (ok)
    {
      ok = ((this->m_type_ext)->IsValid(rank) == true);
    }

  ExtTypeInfo_t* p_info = static_cast<ExtTypeInfo_t*>(NULL);

  if (ok)
    {
      p_info = (this->m_type_ext)->GetReference(rank);
      ok = (IsUndefined(p_info->mult) == false);
    }

  if (ok)
    {
      ret = p_info->mult;
    }

  return const_cast<char*>(ret);

 }

/* Miscellaneous methods */

RET_STATUS AnyType::Compress (void) 
{ 

  RET_STATUS status = STATUS_ERROR;

  /* Bug 9290 - ccs::base::AnyType::Compress() returns when the type is contiguous
              - LUtable<>::Compress() performs at most one-step compression 
  */

  if ((status = this->ccs::base::AnyType::Compress()) != STATUS_SUCCESS)
    {
      log_error("ccs::base::AnyType::Compress() failed");
      return status;
    }

  while ((this->m_type_ext)->Compress() != STATUS_ERROR) { log_info("AnyType::Compress - Compression iteration"); }

  status = STATUS_SUCCESS;

  return status; 

}

/* Constructor methods */

AnyType::AnyType (void)
{

  log_trace("AnyType::AnyType - Entering method");

  /* Initialize attributes */
  this->Initialize();

  log_trace("AnyType::AnyType - Leaving method");

  return;

}

/* Destructor method */

/* Display methods */

uint_t AnyType::SerializeType (char* buffer, int max_size) const
{

  log_trace("AnyType::SerializeType - Entering method");

  /* WARNING - Unsafe implementation */

  if (buffer == NULL) /* Only interested in hash key */
    {
      char tmp [2048] = STRING_UNDEFINED;
      buffer = (char*) tmp; max_size = 2048;
    }

  /* WARNING - Check if size could be negative - yes in case snprintf truncates ... does not append zero  ... and the buffer is not cleared */

  /* Zero the buffer */
  memset(buffer, 0, max_size); max_size -= 1; /* Make sure the buffer is zero'ed (and the trailing zero won't get overwrittent) so as to avoid strlen failure in case snprintf truncates */

  char* p_buf = buffer;

  if (this->IsDefined() != true)
    {
      sstrncpy(p_buf, (char*) "</attributes>", max_size);
      return 0;
    }

  snprintf(p_buf, max_size, "<attributes>"); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

  for (uint_t index = 0; index < this->GetRank(); index += 1)
    {
      ccs::base::TypeInfo_t info; (this->m_type)->GetValue(info, index);

      if (ccs::base::AnyType_IsAttrValid(info) != true) continue;

      char name [STRING_MAX_LENGTH]; (this->m_type)->GetKeyword(name, index); /* Get attribute name */
      char* type = ccs::base::AnyType_GetTypeName((TypeIdentifier_t) info.type); /* Get type identifier */

      snprintf(p_buf, max_size, "<attribute name=\"%s\" dataType=\"%s\"", name, type); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      if (info.mult != 1)
	{ 
	  snprintf(p_buf, max_size, " multiplicity=\"%u\"/>", info.mult); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}
      else
	{
	  sstrncpy(p_buf, (char*) "/>", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}
    }

  /* Terminate buffer */
  sstrncpy(p_buf, (char*) "</attributes>", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

  log_debug("Serialized type '%s'", buffer);

  log_trace("AnyType::SerializeType - Leaving method");

  /* Hask key from datatype definition */
  return HelperTools::Hash<uint16_t>(buffer);

}

} /* namespace base */

} /* namespace sdn */

#undef LOG_ALTERN_SRC


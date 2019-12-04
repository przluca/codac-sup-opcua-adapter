/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/library/any-type.cpp $
* $Id: any-type.cpp 100239 2019-06-23 15:40:27Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
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

#include <types.h> /* Condensed integer type definition, RET_STATUS, etc. */
#include <tools.h> /* Misc. helper functions */
#include <Hash.h>
#include <lu-table.h> /* LUTable class definition, etc. */

#define LOG_TRACE_ENABLE
#undef LOG_TRACE_ENABLE 
#define LOG_DEBUG_ENABLE
#undef LOG_DEBUG_ENABLE 
#include <log-api.h> /* Syslog wrapper routines (ccs::log) */

/* Local header files */

#include "any-type.h" /* This class definition */

/* Constants */

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ccs::base"
#if 0
#define ANYTYPE_ATTR_BIT_BASE_MULT 8 /* WARNING - 'bit' is supported only for 'bit [n * ANYTYPE_ATTR_BIT_BASE_MULT]' */
#endif
/* Type definition */

namespace ccs {

namespace base {

typedef struct TypeDef {

  TypeIdentifier_t id; /* Type identifier */
  uint_t size; /* Type memory footprint */

} TypeDef_t;

/* Global variables */

static KeywordValuePair<TypeDef_t> TypeList [] = 
{

  { "bool", { ANYTYPE_ATTR_BOOL, sizeof(bool) } },
  { "char", { ANYTYPE_ATTR_CHAR, sizeof(char) } },
  { "string", { ANYTYPE_ATTR_CHAR, STRING_MAX_LENGTH * sizeof(char) } }, /* WARNING */
  { "int8",    { ANYTYPE_ATTR_SINT8, sizeof(int8_t) } },
  { "int8_t",  { ANYTYPE_ATTR_SINT8, sizeof(int8_t) } }, 
  { "uint8",   { ANYTYPE_ATTR_UINT8, sizeof(uint8_t) } },
  { "uint8_t", { ANYTYPE_ATTR_UINT8, sizeof(uint8_t) } },
  { "int16",    { ANYTYPE_ATTR_SINT16, sizeof(int16_t) } }, 
  { "int16_t",  { ANYTYPE_ATTR_SINT16, sizeof(int16_t) } }, 
  { "uint16",   { ANYTYPE_ATTR_UINT16, sizeof(uint16_t) } }, 
  { "uint16_t", { ANYTYPE_ATTR_UINT16, sizeof(uint16_t) } }, 
  { "int32",    { ANYTYPE_ATTR_SINT32, sizeof(int32_t) } }, 
  { "int32_t",  { ANYTYPE_ATTR_SINT32, sizeof(int32_t) } },
  { "int",      { ANYTYPE_ATTR_SINT32, sizeof(int32_t) } }, 
  { "int_t",    { ANYTYPE_ATTR_SINT32, sizeof(int32_t) } },
  { "uint32",   { ANYTYPE_ATTR_UINT32, sizeof(uint32_t) } }, 
  { "uint32_t", { ANYTYPE_ATTR_UINT32, sizeof(uint32_t) } },
  { "uint",     { ANYTYPE_ATTR_UINT32, sizeof(uint32_t) } }, 
  { "uint_t",   { ANYTYPE_ATTR_UINT32, sizeof(uint32_t) } },
  { "int64",    { ANYTYPE_ATTR_SINT64, sizeof(int64_t) } },
  { "int64_t",  { ANYTYPE_ATTR_SINT64, sizeof(int64_t) } },
  { "uint64",   { ANYTYPE_ATTR_UINT64, sizeof(uint64_t) } },
  { "uint64_t", { ANYTYPE_ATTR_UINT64, sizeof(uint64_t) } },
  { "float",   { ANYTYPE_ATTR_FLOAT, sizeof(float) } },
  { "float32", { ANYTYPE_ATTR_FLOAT, sizeof(float) } },   /* MARTe2 Integration */
  { "double",  { ANYTYPE_ATTR_DOUBLE, sizeof(double) } },
  { "float64", { ANYTYPE_ATTR_DOUBLE, sizeof(double) } }, /* MARTe2 Integration */
#if 0
  { "bit", { ANYTYPE_ATTR_BIT, sizeof(char) } }, /* WARNING */
#endif
  { EOT_KEYWORD, { ANYTYPE_ATTR_UNDEF, 0 } }

};

static LUTable<TypeDef_t> _type_list ((KeywordValuePair<TypeDef_t>*) &TypeList);

/* Function declaration */

/* Function definition */

static inline void AnyType_ClearAttr (TypeInfo_t& attr) { attr.rank = 0; attr.size = 0; attr.type = ANYTYPE_ATTR_UNDEF; attr.mult = 0; attr.p_value = attr.p_deflt = NULL; return; }

TypeIdentifier_t AnyType_GetTypeId (const char* type_name) { TypeDef_t type_info; type_info.id = ANYTYPE_ATTR_UNDEF; _type_list.GetValue(type_info, type_name); return (TypeIdentifier_t) type_info.id; }
char* AnyType_GetTypeName (TypeIdentifier_t type_id) { uint_t index = 0; for (index = 0; index < _type_list.GetSize(); index += 1) { TypeDef_t type_info; _type_list.GetValue(type_info, index); if (type_info.id == type_id) break; } return _type_list.GetKeyword(index); }
uint_t AnyType_GetTypeSize (const char* type_name) { TypeDef_t type_info; type_info.size = 0; _type_list.GetValue(type_info, type_name); return (uint_t) type_info.size; }
uint_t AnyType_GetTypeSize (TypeIdentifier_t type_id) { return AnyType_GetTypeSize(AnyType_GetTypeName(type_id)); }
bool AnyType_IsAttrValid (TypeInfo_t& attr) { return (((attr.size != 0) && (attr.type != ANYTYPE_ATTR_UNDEF)) ? true : false); }

/* Initializer methods */

RET_STATUS AnyType::AddAttribute (uint_t rank, const char* name, uint_t size) 
{ 

  RET_STATUS status = STATUS_ERROR; 
#if 0
  if ((rank > MAXIMUM_ATTRIBUTE_NUM) || ((this->m_size + size) > MAXIMUM_DATATYPE_SIZE)) /* Scalability issue */
#else
  if (rank >= MAXIMUM_ATTRIBUTE_NUM) /* Scalability issue */
#endif
    {
      log_error("AnyType::AddAttribute - Out-of-bound attribute '%s' of size '%d' at rank '%d'", name, size, rank);
      this->m_defined = false;
      return status;
    }

  if (this->HasAttribute(rank) == true) 
    {
      log_error("AnyType::AddAttribute - An attribute '%s' already exists at rank '%d'", this->GetAttributeName(rank), rank);
      this->m_defined = false;
      return status; 
    }

  TypeInfo_t info; 

  info.rank = rank; info.size = size; info.p_value = NULL; info.p_deflt = NULL;
  info.type = ANYTYPE_ATTR_UINT8; info.mult = size; /* Undefined type */

  if ((this->m_type)->AddPair(name, info, rank) != STATUS_SUCCESS) /* Create attribute at LUTable index corresponding to rank and not in the order they are declared */
    { 
      log_error("AnyType::AddAttribute - Unable to add attribute '%s' of size '%d' at rank '%d'", name, size, rank);
      return status; 
    }

  this->m_defined = true; 

  //this->m_rank += 1; /* Buggy for type definition which have non-zero-starting ranks */
  this->m_size += size;

  log_debug("AnyType::AddAttribute - Added attribute '%s' of size '%d' at rank '%d'", name, size, rank);

  status = STATUS_SUCCESS;

  return status; 

}

RET_STATUS AnyType::AddAttribute (uint_t rank, const char* name, TypeIdentifier_t type, uint_t mult, void* p_value) 
{ 

  RET_STATUS status = STATUS_ERROR; 
#if 0
  /* Identify type */
  if (_type_list.IsValid((uint_t) type) != true)
#else
  if (type == ANYTYPE_ATTR_UNDEF)
#endif
    {
      log_error("AnyType::AddAttribute - Unknown attribute type '%d'", (uint_t) type);
      return status;
    }

  uint_t size = AnyType_GetTypeSize(type); size = mult * size;

  /* Add attribute */
  if (this->AddAttribute(rank, name, size) != STATUS_SUCCESS) /* Create attribute at LUTable index corresponding to rank */
    { 
      return status; 
    }

  /* Complement with type and multiplicity information */
  TypeInfo_t info; (this->m_type)->GetValue(info, rank); info.type = type; info.mult = mult; (this->m_type)->SetValue(info, rank); 

  log_debug("AnyType::AddAttribute - Added attribute '%s' of type '%d' and multiplicity '%d' at rank '%d'", name, type, mult, rank);

  if (p_value != NULL)
    {
      log_debug("AnyType::AddAttribute - Provide default value for attribute '%s'", name);
      this->SetAttribute(rank, p_value);
    }

  status = STATUS_SUCCESS;

  return status; 

}

RET_STATUS AnyType::AddAttribute (const char* rank, const char* name, const char* type, const char* mult, const char* value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  log_trace("AnyType::AddAttribute('%s, %s, %s, %s, %s') - Entering method", rank, name, type, mult, value);

  if ((IsUndefined(name) == true) || (IsUndefined(type) == true)) 
    {
      log_error("AnyType::AddAttribute('%s, %s, %s, %s, %s') - Attribute name and type must be defined", rank, name, type, mult, value);
      return status;
    }

  /* WARNING - Multiplicity may also be carried as 'mult mult' '[mult][mult]' */

  uint_t mult_ = 1; 
  
  if (IsUndefined(mult) != true) 
    {
      const char* p_buf = mult;
      uint_t temp = 1;

      while (*p_buf != 0)
	{
	  while ((*p_buf != 0) && ((*p_buf == ' ') || (*p_buf == ',') || (*p_buf == '[') || (*p_buf == ']'))) p_buf += 1; /* Skip characters */

	  if (*p_buf != 0) { sscanf(p_buf, "%u", &temp); mult_ *= temp; }
	  
	  while ((*p_buf != 0) && (*p_buf != ' ') && (*p_buf != ',') && (*p_buf != '[') && (*p_buf != ']')) p_buf += 1; /* Skip characters */
	}

      log_debug("AnyType::AddAttribute - Set '%s' attribute multiplicity to '%u'", name, mult_);
    }
  else /* May be defined using [must] as part of type */
    {
      char* p_buf = const_cast<char*>(type);
      uint_t temp = 1;

      while (*p_buf != 0) 
	{ 
	  if (*p_buf == '[') 
	    { 
	      /* Insert null byte */ 
	      *p_buf = 0; p_buf += 1; 
	      sscanf(p_buf, "%u", &temp); mult_ *= temp; 

	      log_debug("AnyType::AddAttribute - Set '%s' attribute multiplicity to '%u'", name, mult_);
	    } 
	  else p_buf += 1; 
	}
    }

  uint_t rank_ = this->GetRank(); if (IsUndefined(rank) != true) sscanf(rank, "%u", &rank_);

  /* WARNING - Type may also be carried as 'type type' in case of multidimentional arrays */

  if (IsUndefined(mult) != true) 
    {
      char* p_buf = const_cast<char*>(type);

      while (*p_buf != 0) { if (*p_buf == ' ') { /* Insert null byte in lieu of space and terminate */ *p_buf = 0; break; } else p_buf += 1; }

      log_debug("AnyType::AddAttribute - Set '%s' attribute type to '%s'", name, type);
    }

  /* Identify type */
  TypeIdentifier_t type_id = AnyType_GetTypeId(type);

  if (type_id == ANYTYPE_ATTR_UNDEF)
    {
      log_error("AnyType::AddAttribute - Unknown attribute type '%s'", type);
      return status;
    }

  /* WARNING - 'string' is supported as 'char [STRING_MAX_LENGTH]' */
  if (type_id == ANYTYPE_ATTR_CHAR)
    {
      mult_ *= AnyType_GetTypeSize(type);
      log_debug("AnyType::AddAttribute - Set '%s' attribute multiplicity to '%u'", name, mult_);
    }
#if 0  
  /* WARNING - 'bit' is supported only for 'bit [n * ANYTYPE_ATTR_BIT_BASE_MULT]' */
  if (type_id == ANYTYPE_ATTR_BIT)
    {
      if (mult_ % ANYTYPE_ATTR_BIT_BASE_MULT != 0) { log_error("AnyType::AddAttribute('%s, %s, %s, %s, %s') - Attribute multiplicity must be a multiple of %d", rank, name, type, mult, value, ANYTYPE_ATTR_BIT_BASE_MULT); return status; }
      else mult_ = mult_ / ANYTYPE_ATTR_BIT_BASE_MULT;
      log_debug("AnyType::AddAttribute - Set '%s' attribute multiplicity to '%u'", name, mult_);
    }
#endif  
  /* Add attribute */
  if (this->AddAttribute(rank_, name, type_id, mult_) != STATUS_SUCCESS) /* Create attribute at LUTable index corresponding to rank */
    { 
      return status; 
    }

  if (IsUndefined(value) != true)
    {
#if 0
      char buffer [STRING_MAX_LENGTH]; resolve_variable_string(value, buffer, STRING_MAX_LENGTH); /* May be set be reference to an environment variable */

      if (this->ParseAttribute(rank_, buffer) != STATUS_SUCCESS) /* Provide default value */
	{
	  log_error("AnyType::AddAttribute - ParseAttribute('%d, %s') failed", rank_, buffer);
	  return status;
	}
#else
      if (this->ParseAttribute(rank_, value) != STATUS_SUCCESS) /* Provide default value */
	{
	  log_error("AnyType::AddAttribute - ParseAttribute('%d, %s') failed", rank_, value);
	  return status;
	}
#endif
    }

  log_trace("AnyType::AddAttribute('%s, %s, %s, %s, %s') - Leaving method", rank, name, type, mult, value);

  status = STATUS_SUCCESS;

  return status;
 
}

RET_STATUS AnyType::Initialize (void) 
{ 

  RET_STATUS status = STATUS_ERROR;

  /* Initialize resources */
  strncpy(this->m_name, DEFAULT_DATATYPE_NAME, STRING_MAX_LENGTH); 
  this->m_allocated = false; this->m_instance = NULL; this->m_defined = false; 
  //this->m_rank = 0; /* Buggy for type definition which have non-zero-starting ranks */
  this->m_size = 0; 

  this->m_type = new LUTable<TypeInfo> (MAXIMUM_ATTRIBUTE_NUM); TypeInfo_t info; AnyType_ClearAttr(info); (this->m_type)->SetDefault(info); (this->m_type)->Clear();

  status = STATUS_SUCCESS;

  return status; 

}

RET_STATUS AnyType::Dump (char* file_path)
{ 

  RET_STATUS status = STATUS_ERROR; 

  FILE* p_file;

  if ((p_file = fopen(file_path, "w")) == NULL)
    {
      log_error("AnyType::Dump - fopen('%s') failed with '%d - %s'", file_path, errno, strerror(errno));
      return status;
    }

  char buffer [2048]; this->SerializeType(buffer, 2048);

  fprintf(p_file, "%s", buffer); fclose(p_file);

  status = STATUS_SUCCESS;

  return status; 

}

RET_STATUS AnyType::Load (char* file_path) { log_warning("AnyType::Load - Invoked unimplemented virtual method"); (void) file_path; return STATUS_ERROR; }

/* Accessor methods */

RET_STATUS AnyType::SetName (const char* name) { sstrncpy(this->m_name, name, STRING_MAX_LENGTH); return STATUS_SUCCESS; } /* Datatype identifier */

RET_STATUS AnyType::GetAttributeInfo (TypeInfo& info, uint_t rank) const { RET_STATUS status = STATUS_ERROR; if ((this->m_type)->IsValid(rank) == true) status = (this->m_type)->GetValue(info, rank); else log_error("AnyType::GetAttributeInfo('%d') - Out-of-bound attribute", rank); return status; }
RET_STATUS AnyType::SetAttributeInfo (TypeInfo& info, uint_t rank) { RET_STATUS status = STATUS_ERROR; if ((this->m_type)->IsValid(rank) == true) status = (this->m_type)->SetValue(info, rank); else log_error("AnyType::SetAttributeInfo('%d') - Out-of-bound attribute", rank); return status; }

uint_t AnyType::GetAttributeRank (const char* name) const
{ 

  uint_t rank = (this->m_type)->GetIndex(name);

  if ((this->m_type)->IsValid(name) != true) 
    { /* Try and find '[%u]' */ 

      log_warning("AnyType::GetAttributeRank - Undefined attribute '%s'", name);
      log_info("AnyType::GetAttributeRank - Try and extract attribute rank from '%s'", name);

      char buffer [STRING_MAX_LENGTH] = STRING_UNDEFINED; 
      char* p_buf = (char*) buffer; char* p_rank = (char*) buffer; 

      sstrncpy(p_buf, name, STRING_MAX_LENGTH);

      while (*p_buf != 0) { if (*p_buf == '[') p_rank = p_buf + 1; /* Multiplicity defined */ if ((*p_buf == ' ') || (*p_buf == '[') || (*p_buf == ']')) *p_buf = 0; /* Zero all these characters */ p_buf += 1; }

      log_debug("AnyType::GetAttributeRank - Try and extract attribute rank from '%s'", p_rank);

      sscanf(p_rank, "%u", &rank);

      log_debug("AnyType::GetAttributeRank - Extracted attribute rank '%u'", rank);
    } 

  return rank; 

}

void* AnyType::GetAttributeReference (uint_t rank) const
{ 

  TypeInfo_t info; AnyType_ClearAttr(info); this->GetAttributeInfo(info, rank); /* Unchanged if failed */

  if (info.p_value == NULL) log_error("AnyType::GetAttributeReference - Undefined attribute reference");

  return info.p_value; 

}

RET_STATUS AnyType::SetAttributeReference (uint_t rank, void* p_value)
{ 

  TypeInfo_t info; AnyType_ClearAttr(info); this->GetAttributeInfo(info, rank); /* Unchanged if failed */

  info.p_value = p_value;

  return this->SetAttributeInfo(info, rank);

}

RET_STATUS AnyType::GetAttribute (uint_t rank, void* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if ((this->m_type)->IsValid(rank) != true) 
    {
      log_error("AnyType::GetAttribute('%d') - Out-of-bound attribute", rank); 
      return status;
    }

  TypeInfo_t info; AnyType_ClearAttr(info); (this->m_type)->GetValue(info, rank); /* Unchanged if failed */
#if 0
  if (info.p_value == NULL) 
    {
      log_error("AnyType::GetAttribute('%d') - Undefined attribute reference", rank);
      return status;
    }

  memcpy(p_value, info.p_value, info.size);
#else
  void * p_src = info.p_value;

  if ((info.p_value == NULL) && (info.p_deflt == NULL))
    {
      log_error("AnyType::GetAttribute('%d') - Undefined attribute reference", rank);
      return status;
    }

  if ((info.p_value == NULL) && (info.p_deflt != NULL))
    {
      log_warning("AnyType::GetAttribute('%d') - Return default value", rank);
      p_src = info.p_deflt;
    }

  memcpy(p_value, p_src, info.size);
#endif
  status = STATUS_SUCCESS;

  return status; 

}

RET_STATUS AnyType::SetAttribute (uint_t rank, void* p_value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if ((this->m_type)->IsValid(rank) != true) 
    {
      log_error("AnyType::SetAttribute('%d') - Out-of-bound attribute", rank); 
      return status;
    }

  TypeInfo_t info; AnyType_ClearAttr(info); (this->m_type)->GetValue(info, rank); /* Unchanged if failed */
#if 0
  if (info.p_value == NULL) 
    {
      log_error("AnyType::SetAttribute('%d') - Undefined attribute reference", rank);
      return status;
    }

  memcpy(info.p_value, p_value, info.size);
#else
  void * p_dst = info.p_value;

  if ((info.p_value == NULL) && (info.p_deflt == NULL))
    {
      log_debug("AnyType::SetAttribute('%d') - Unallocated attribute '%s'", rank, (this->m_type)->GetKeyword(rank));

      if ((info.size == 0) || (info.p_deflt = new uint8_t [info.size]) == NULL) 
	{
	  log_error("AnyType::SetAttribute('%d') - Unable to allocate memory for attribute '%s'", rank, (this->m_type)->GetKeyword(rank));
	  return status;
	}
      else
	{
	  log_debug("AnyType::SetAttribute('%d') - Temporarily allocated memory to store attribute '%s'", rank, (this->m_type)->GetKeyword(rank));
	  memset(info.p_deflt, 0, info.size);
	}
      
      (this->m_type)->SetValue(info, rank);
    }

  if ((info.p_value == NULL) && (info.p_deflt != NULL))
    {
      log_warning("AnyType::SetAttribute('%d') - Overwrite default value", rank);
      p_dst = info.p_deflt;
    }

  memcpy(p_dst, p_value, info.size);
#endif
  status = STATUS_SUCCESS;

  return status; 

}

/* Miscellaneous methods */

void AnyType::ClearInstance (void) 
{ 

  log_trace("AnyType::ClearInstance - Entering method");

  if (this->m_instance != NULL) 
    {
      log_debug("AnyType::ClearInstance - Erase memory buffer '%p' of size '%u'", this->m_instance, this->m_size);
      memset(this->m_instance, 0, this->m_size); 
    }

  log_debug("AnyType::ClearInstance - Try and restore default value, if any");
  for (this->m_index = 0; this->m_index < this->GetRank(); this->m_index += 1)
    {
      TypeInfo_t info; (this->m_type)->GetValue(info, this->m_index);

      if (AnyType_IsAttrValid(info) != true) continue;

      /* Restore default, if provided */
      if ((info.p_value != NULL) && (info.p_deflt != NULL)) { log_debug("AnyType::ClearInstance - Restore default attribute '%s'", (this->m_type)->GetKeyword(this->m_index)); memcpy(info.p_value, info.p_deflt, info.size); }
    }

  log_trace("AnyType::ClearInstance - Leaving method");

  return; 

}

RET_STATUS AnyType::CopyInstance (void* buffer) 
{ 

  log_trace("AnyType::CopyInstance - Entering method");

  RET_STATUS status = STATUS_ERROR;

  /* Through overriding references, an instance may be assembled from disjoint memory locations */

  for (this->m_index = 0; this->m_index < this->GetRank(); this->m_index += 1)
    {
      TypeInfo_t info; (this->m_type)->GetValue(info, this->m_index);

      if (AnyType_IsAttrValid(info) != true) continue;

      /* Copy attribute at location */
      if (info.p_value != NULL) { memcpy(((int8_t*) buffer) + info.offset, info.p_value, info.size); }
    }

  status = STATUS_SUCCESS;

  log_trace("AnyType::CopyInstance - Leaving method");

  return status; 

}

RET_STATUS AnyType::CreateInstance (void) 
{ 
  
  log_trace("AnyType::CreateInstance - Entering method");

  RET_STATUS status = STATUS_ERROR;
#if 0
  /* Try and compress LUTable */
  while (this->Compress() != STATUS_ERROR) { log_info("AnyType::CreateInstance - Reduced datatype definition to rank '%d'", this->GetRank()); }
#else
  this->Compress();
#endif

  if ((this->m_size == 0) || (this->m_instance = new uint8_t [this->m_size]) == NULL) 
    { 
      log_error("AnyType::CreateInstance - Unable to allocate buffer of size '%d'", this->m_size);
      return status; 
    }
  else
    {
      this->m_allocated = true;
    }

  /* Assign reference to all attributes in the type definition */
  this->AssignReference();

  /* Clear instance */
  this->ClearInstance();

  status = STATUS_SUCCESS;

  log_trace("AnyType::CreateInstance - Leaving method");

  return status; 

}

void AnyType::AssignReference (void* instance) 
{ 
  
  /* Assign reference to all attributes in the type definition */
  uint32_t offset = 0;
  uint8_t* reference = (uint8_t*) instance;

  for (this->m_index = 0; this->m_index < this->GetRank(); this->m_index += 1)
    {
      TypeInfo_t info; (this->m_type)->GetValue(info, this->m_index);

      if (AnyType_IsAttrValid(info) != true) continue;

      /* Dummy attribute may not have contributed to the allocated buffer */
      if (info.size != 0) { info.offset = offset; info.p_value = reference; }

      (this->m_type)->SetValue(info, this->m_index);

      offset += info.size; if (reference != NULL) reference += info.size;
    }

  return; 

}
#if 0
RET_STATUS AnyType::Compress (void) { return (this->m_type)->Compress(); };
#else
RET_STATUS AnyType::Compress (void) { while ((this->m_type)->Compress() != STATUS_ERROR) { log_info("AnyType::Compress - Compression iteration"); } return STATUS_SUCCESS; }
#endif
/* Constructor methods */
#if 1
AnyType::AnyType (const char* name)
{

  /* Initialize attributes */
  this->Initialize();
  strncpy(this->m_name, name, STRING_MAX_LENGTH);

  return;

}
#endif
/* Destructor method */

AnyType::~AnyType (void) 
{ 
  
  log_trace("AnyType::~AnyType - Entering method '%p'", this); 

  this->DeleteInstance(); /* Requires LUTable */ 

  if (this->m_type != NULL) delete this->m_type; this->m_type = NULL; 

  log_trace("AnyType::~AnyType - Leaving method '%p'", this); 

  return; 

}

/* Display methods */

uint_t AnyType::SerializeType (char* buffer, int max_size) const
{

  log_trace("AnyType::SerializeType - Entering method");

  char* p_buf = buffer;
  char* p_tmp = buffer;

  if (buffer == NULL) /* Only interested in hash key */
    {
      p_tmp = static_cast<char*>(malloc(256));
      p_buf = p_tmp;
      max_size = 256; 
    }

  /* WARNING - Check if size could be negative - yes in case snprintf truncates ... does not append zero  ... and the buffer is not cleared */

  /* Zero the buffer */
  memset(p_buf, 0, max_size); max_size -= 1; /* Make sure the buffer is zero'ed (and the trailing zero won't get overwrittent) so as to avoid strlen failure in case snprintf truncates */

  if (this->IsDefined() != true)
    {
      sstrncpy(p_buf, "undefined", max_size);
      return 0;
    }

  snprintf(p_buf, max_size, "%s [%d] - ", AnyType_GetTypeName(ANYTYPE_ATTR_UINT8), this->m_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

  for (uint_t index = 0; index < this->GetRank(); index += 1)
    {
      TypeInfo_t info; (this->m_type)->GetValue(info, index);

      if (AnyType_IsAttrValid(info) != true) continue;

      char name [STRING_MAX_LENGTH]; (this->m_type)->GetKeyword(name, index); /* Get attribute name */
      char* type = AnyType_GetTypeName((TypeIdentifier_t) info.type); /* Get type identifier */

      snprintf(p_buf, max_size, "%s %s", type, name); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */

      if (info.mult != 1) 
	{
#if 0
	  /* WARNING - 'bit' is supported only for 'bit [n * ANYTYPE_ATTR_BIT_BASE_MULT]' */
	  snprintf(p_buf, max_size, " [%d], ", (((TypeIdentifier_t) info.type == ANYTYPE_ATTR_BIT) ? ANYTYPE_ATTR_BIT_BASE_MULT : 1) * info.mult); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
#else
	  snprintf(p_buf, max_size, " [%d], ", info.mult); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
#endif
	}
      else
	{
	  sstrncpy(p_buf, ", ", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}
    }

  /* Terminate buffer */
  p_tmp[strlen(p_tmp)-2] = 0;

  if (buffer == NULL) /* Only interested in hash key */
    {
      free(p_tmp);
    }

  log_trace("AnyType::SerializeType - Leaving method");

  /* Hask key from datatype definition */
  return HelperTools::Hash<uint16_t>(buffer);

}

RET_STATUS AnyType::ParseAttribute (uint_t rank, const char* buffer)
{

  log_trace("AnyType::ParseAttribute - Entering method");

  RET_STATUS status = STATUS_ERROR;

  if ((this->m_type)->IsValid(rank) != true) return status;

  TypeInfo_t info; (this->m_type)->GetValue(info, rank);

  if (AnyType_IsAttrValid(info) != true) 
    {
      log_error("AnyType::ParseAttribute - Invalid attribute at rank '%d'", rank);
      return status;
    }

  void* p_dst = NULL;

  if (info.p_value != NULL) p_dst = info.p_value; /* Write to instance */
  else /* Parse and set as default value */
    {
      if (info.p_deflt == NULL) 
	{
	  log_debug("AnyType::ParseAttribute - Unallocated attribute '%s'", (this->m_type)->GetKeyword(rank));
	  
	  if ((info.size == 0) || (info.p_deflt = new uint8_t [info.size]) == NULL) 
	    {
	      log_error("AnyType::ParseAttribute - Unable to allocate memory for attribute '%s'", (this->m_type)->GetKeyword(rank));
	      return status;
	    }
	  else
	    {
	      log_debug("AnyType::ParseAttribute - Temporarily allocated memory to store attribute '%s'", (this->m_type)->GetKeyword(rank));
	      memset(info.p_deflt, 0, info.size);
	    }
	  
	  (this->m_type)->SetValue(info, rank);
	}
      
      p_dst = info.p_deflt;
    }
  
  const char* p_buf = buffer;

  while ((*p_buf == '[') || (*p_buf == ' ')) p_buf += 1; /* Skip characters */

  if ((TypeIdentifier_t) info.type == ANYTYPE_ATTR_CHAR)
    {
      sstrncpy((char*) p_dst, p_buf, info.size);
      log_debug("AnyType::ParseAttribute - Set '%s' attribute value to '%s'", (this->m_type)->GetKeyword(rank), (char*) p_dst);
    }
  else for (uint_t index = 0; index < info.mult; index += 1)
    {
      log_debug("AnyType::ParseAttribute - Parse buffer '%s'", p_buf);

      switch ((TypeIdentifier_t) info.type)
	{
	  case ANYTYPE_ATTR_BOOL:
	    if (strcmp(p_buf, "true") == 0) ((bool*) p_dst)[index] = true; else ((bool*) p_dst)[index] = false; 
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%s'", (this->m_type)->GetKeyword(rank), index, ((((bool*) p_dst)[index] == true) ? "true" : "false"));
	    break;
	  case ANYTYPE_ATTR_SINT8:
	    sscanf(p_buf, "%hhd", &((int8_t*) p_dst)[index]);
	    log_info("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%hhd'", (this->m_type)->GetKeyword(rank), index, ((int8_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_UINT8:
            sscanf(p_buf, "%hhu", &((uint8_t*) p_dst)[index]);
	    log_info("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%hhu'", (this->m_type)->GetKeyword(rank), index, ((uint8_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_SINT16:
	    sscanf(p_buf, "%hd", &((int16_t*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%hd'", (this->m_type)->GetKeyword(rank), index, ((int16_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_UINT16:
	    sscanf(p_buf, "%hu", &((uint16_t*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%hu'", (this->m_type)->GetKeyword(rank), index, ((uint16_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_SINT32:
	    sscanf(p_buf, "%d", &((int32_t*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%d'", (this->m_type)->GetKeyword(rank), index, ((int32_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_UINT32:
	    sscanf(p_buf, "%u", &((uint32_t*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%u'", (this->m_type)->GetKeyword(rank), index, ((uint32_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_SINT64:
	    sscanf(p_buf, "%ld", &((int64_t*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%ld'", (this->m_type)->GetKeyword(rank), index, ((int64_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_UINT64:
	    sscanf(p_buf, "%lu", &((uint64_t*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%lu'", (this->m_type)->GetKeyword(rank), index, ((uint64_t*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_FLOAT:
	    sscanf(p_buf, "%f", &((float*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%f'", (this->m_type)->GetKeyword(rank), index, ((float*) p_dst)[index]);
	    break;
	  case ANYTYPE_ATTR_DOUBLE:
	    sscanf(p_buf, "%lf", &((double*) p_dst)[index]);
	    log_debug("AnyType::ParseAttribute - Set '%s[%d]' attribute value to '%lf'", (this->m_type)->GetKeyword(rank), index, ((double*) p_dst)[index]);
	    break;
#if 0
	  case ANYTYPE_ATTR_BIT:
	    sscanf(p_buf, "%hhi", &((uint8_t*) p_dst)[index]);
	    log_info("AnyType::ParseAttribute - Set '%s[%d]' attribute value '%hhx'", (this->m_type)->GetKeyword(rank), index, ((uint8_t*) p_dst)[index]);
	    break;
#endif
	  default:
	    break;
	}

      /* Find next key */
      while ((*p_buf != ',') && (*p_buf != 0)) p_buf += 1; /* Skip characters */
      if (*p_buf == ',') p_buf += 1; /* Skip characters */
      if (*p_buf == 0) break; 
    }

  status = STATUS_SUCCESS;

  log_trace("AnyType::ParseAttribute - Leaving method");

  return status; 

}

RET_STATUS AnyType::ParseInstance (const char* buffer)
{

  RET_STATUS status = STATUS_ERROR;

  if (this->IsDefined() != true) return status;

  const char* p_buf = buffer;

  for (uint_t rank = 0; rank < this->GetRank(); rank += 1)
    {
      if (this->ParseAttribute(rank, p_buf) != STATUS_SUCCESS) continue;
      else
	{
	  /* Locate next attribute */
	  while (*p_buf != 0)
	    {

	      while (*p_buf == ' ') p_buf += 1; /* Skip characters */

	      if (*p_buf == '[') while ((*p_buf != ']') && (*p_buf != 0)) p_buf += 1;
	      if (*p_buf == ',') { p_buf += 1; break; }
	      if (*p_buf == 0) { break; }

	      p_buf += 1;
	    }
	}
    }

  status = STATUS_SUCCESS;

  return status;

}

RET_STATUS AnyType::SerializeAttribute (uint_t rank, char* buffer, int max_size) const
{

  log_trace("AnyType::SerializeAttribute - Enterng method");

  RET_STATUS status = STATUS_ERROR;

  if (max_size <= 0) return status;
  if ((this->m_type)->IsValid(rank) != true) return status;

  /* WARNING - Check that instance is initialized */

  /* WARNING - Check if size could be negative - yes in case snprintf truncates ... does not append zero  ... and the buffer is not cleared */

  /* Zero the buffer */
  memset(buffer, 0, max_size); max_size -= 1; /* Make sure the buffer is zero'ed (and the trailing zero won't get overwrittent) so as to avoid strlen failure in case snprintf truncates */

  char* p_buf = buffer;

  TypeInfo_t info; (this->m_type)->GetValue(info, rank);

  if (AnyType_IsAttrValid(info) != true)
    {
      log_error("AnyType::SerializeAttribute - Invalid attribute at rank '%d'", rank);
      return status;
    }

  if (info.mult != 1)
    {
      sstrncpy(p_buf, "[ ", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }

  if ((TypeIdentifier_t) info.type == ANYTYPE_ATTR_CHAR)
    {
      sstrncpy(p_buf, (char*) info.p_value, max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
      sstrncpy(p_buf, ", ", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }
  else for (uint_t index = 0; index < info.mult; index += 1)
    {
      switch ((TypeIdentifier_t) info.type)
	{
	  case ANYTYPE_ATTR_BOOL:
	    sstrncpy(p_buf, ((((bool*) info.p_value)[index] == true) ? "true" : "false"), max_size); 
	    break;
	  case ANYTYPE_ATTR_SINT8:
	    snprintf(p_buf, max_size, "%hd", ((int8_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_UINT8:
	    snprintf(p_buf, max_size, "%hu", ((uint8_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_SINT16:
	    snprintf(p_buf, max_size, "%hd", ((int16_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_UINT16:
	    snprintf(p_buf, max_size, "%hu", ((uint16_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_SINT32:
	    snprintf(p_buf, max_size, "%d", ((int32_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_UINT32:
	    snprintf(p_buf, max_size, "%u", ((uint32_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_SINT64:
	    snprintf(p_buf, max_size, "%ld", ((int64_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_UINT64:
	    snprintf(p_buf, max_size, "%lu", ((uint64_t*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_FLOAT:
	    snprintf(p_buf, max_size, "%g", ((float*) info.p_value)[index]); 
	    break;
	  case ANYTYPE_ATTR_DOUBLE:
	    snprintf(p_buf, max_size, "%g", ((double*) info.p_value)[index]); 
	    break;
#if 0
	  case ANYTYPE_ATTR_BIT:
	    snprintf(p_buf, max_size, "0x%2.2hhx", ((uint8_t*) info.p_value)[index]);
	    break;
#endif
	  default:
	    break;
	}

      max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
      sstrncpy(p_buf, ", ", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
    }

  /* Terminate buffer - remove final comma character */
  if (strlen(buffer) >=2) buffer[strlen(buffer)-2] = 0; p_buf = buffer + strlen(buffer); /* Re-align pointer */
  
  if (info.mult != 1)
    {
      sstrncpy(p_buf, " ]", max_size); 
    }

  status = STATUS_SUCCESS;

  log_trace("AnyType::SerializeAttribute - Leaving method");

  return status; 

}

RET_STATUS AnyType::SerializeInstance (char* buffer, int max_size) const
{

  char* p_buf = buffer;

  if (this->IsDefined() != true)
    {
      sstrncpy(p_buf, "undefined", max_size);
      return STATUS_SUCCESS;
    }

  /* WARNING - Check if size could be negative - yes in case snprintf truncates ... does not append zero  ... and the buffer is not cleared */

  /* Zero the buffer */
  memset(buffer, 0, max_size); max_size -= 1; /* Make sure the buffer is zero'ed (and the trailing zero won't get overwrittent) so as to avoid strlen failure in case snprintf truncates */

  for (uint_t rank = 0; rank < this->GetRank(); rank += 1)
    {
      if (this->SerializeAttribute(rank, p_buf, max_size) != STATUS_SUCCESS) continue;
      else
	{
	  max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	  sstrncpy(p_buf, ", ", max_size); max_size -= strlen(p_buf); p_buf += strlen(p_buf); /* Re-align pointer */
	}
    }

  /* Terminate buffer */
  if (strlen(buffer) >=2) buffer[strlen(buffer)-2] = 0;

  return STATUS_SUCCESS;

}

} /* namespace base */

} /* namespace ccs */

#undef LOG_ALTERN_SRC

#ifndef ANY_TYPE_H
#define ANY_TYPE_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/any-type.h $
* $Id: any-type.h 99192 2019-04-19 08:32:14Z bauvirb $
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

/* Done:
 *
 *   - Support type-specific accessors through type-casting the instance reference, see void* AnyType::GetInstance (void).
 *   - Support dummy attributed with zero size.
 *   - Retrieve attribute rank by name for future access.
 *   - Parse attribute from string, incl. reference to environment variables.
 *   - Re-structure code, base class distinct from XML parsing class to allow for deciding at application-level
 *   - Support type-specific accessors, e.g. RET_STATUS AnyType::GetAttribute(uint_t, double&).
 *   - Safe implementation of Parse and Serialize methods.
 *
 */

/* ToDo:
 *
 *   - Verify behaviour in case of absence of environment variable definition.
 *   - Code simplification urgently needed.
 *   - Robustify destructor ... still segfaults.
 *
 */

/* Global header files */

#include <types.h> /* Condensed integer type definition, RET_STATUS, etc. */
#include <lu-table.h> /* LUTable class definition, etc. */

/* Local header files */

/* Constants */

#define DEFAULT_DATATYPE_NAME "undefined"
#define MAXIMUM_ATTRIBUTE_NUM  1024 /* Scalability issue - Yet something is required for LUTable instantiation */ 
//#define MAXIMUM_DATATYPE_SIZE 8192 /* 8kB */

typedef enum TypeIdentifier {
//typedef enum TypeIdentifier : uint32_t {

  ANYTYPE_ATTR_BOOL = 0,
  ANYTYPE_ATTR_CHAR,
  ANYTYPE_ATTR_SINT8,
  ANYTYPE_ATTR_UINT8,
  ANYTYPE_ATTR_SINT16,
  ANYTYPE_ATTR_UINT16,
  ANYTYPE_ATTR_SINT32,
  ANYTYPE_ATTR_UINT32,
  ANYTYPE_ATTR_SINT64,
  ANYTYPE_ATTR_UINT64,
  ANYTYPE_ATTR_FLOAT,
  ANYTYPE_ATTR_DOUBLE,
#if 0
  ANYTYPE_ATTR_BIT,
#endif
  ANYTYPE_ATTR_UNDEF

} TypeIdentifier_t;

#define ANYTYPE_ATTR_INT8  ANYTYPE_ATTR_SINT8
#define ANYTYPE_ATTR_INT16 ANYTYPE_ATTR_SINT16
#define ANYTYPE_ATTR_INT32 ANYTYPE_ATTR_SINT32
#define ANYTYPE_ATTR_INT64 ANYTYPE_ATTR_SINT64

#define ANYTYPE_ATTR_UNDEFINED ANYTYPE_ATTR_UNDEF /* Maintained for backward compatibility reason */

/* Type definition */

namespace ccs {

namespace base {

typedef struct AttrInfo { /* Rank will be derived from index within the table */

  char name [STRING_MAX_LENGTH]; /* Attribute name */
  char type [STRING_MAX_LENGTH]; /* Type identifier */
  char mult [STRING_MAX_LENGTH]; /* Type multiplicity */
  char deflt [STRING_MAX_LENGTH]; /* Default value, if any */

} AttrInfo_t;

typedef struct TypeInfo { /* For each attribute within a structure, the name will be defined externally as keyword in LUTable */

  uint32_t rank; /* Attribute rank within the structure */
  uint32_t size; /* Attribute memory footprint */
  uint32_t type; /* Type identifier */
  uint32_t mult; /* Type multiplicity */
  uint32_t offset; /* Relative location in instance */
  void* p_value; /* Pointer to location in instance */
  void* p_deflt; /* Pointer to default value */

} TypeInfo_t;

class AnyType
{

  private:

    char m_name [STRING_MAX_LENGTH]; /* Datatype identifier */

    bool m_allocated; /* Internally provided instance*/

    uint8_t* m_instance;
    uint_t m_index, /* m_rank, */ m_size; /* Rank is unused, code handles it badly */

    /* Initializer methods */
    RET_STATUS Initialize (void);

    /* Accessor methods */
    RET_STATUS GetAttributeInfo (TypeInfo_t& info, uint_t rank) const;
    RET_STATUS SetAttributeInfo (TypeInfo_t& info, uint_t rank);

    /* Miscellaneous methods */
    void AssignReference (void* instance);
    void AssignReference (void) { return AssignReference((void*) this->m_instance); };
    void RemoveReference (void) { return AssignReference((void*) NULL); };

  public:

    bool m_defined; /* Datatype defined */
    LUTable<TypeInfo_t>* m_type; /* Attribute look-up table */

    /* Initializer methods */
    RET_STATUS AddAttribute (uint_t rank, const char* name, uint_t size);
    RET_STATUS AddAttribute (uint_t rank, const char* name, TypeIdentifier_t type, uint_t mult = 1, void* p_value = NULL);
    RET_STATUS AddAttribute (const char* rank, const char* name, const char* type, const char* mult = NULL, const char* value = NULL);

    template <class Type> RET_STATUS AddAttribute (uint_t rank, const char* name, uint_t mult = 1) { /* Error - Unsupported attribute type */ return STATUS_ERROR; }
#if 1
    virtual RET_STATUS Dump (char* file); /* Save current configuration to file */
    virtual RET_STATUS Load (char* file); /* Load configuration from file */
#endif
    /* Accessor methods */
    bool HasAttribute (uint_t rank) const { return (this->m_type)->IsValid(rank); }
    bool HasAttribute (const char* name) const { return (this->m_type)->IsValid(name); }

    bool IsDefined (void) const { return this->m_defined; }
    bool IsInitialized (void) const { return this->m_defined; }

    const char* GetName (void) const { return this->m_name; } /* Datatype identifier */
    RET_STATUS SetName (const char* name); /* Datatype identifier */

    uint_t GetRank (void) const { return (this->m_type)->GetSize(); } /* Number of attributes */

    uint_t GetSize (void) const { return this->m_size; } /* Memory footprint */
    RET_STATUS SetSize (uint_t size) { this->m_size = size; return STATUS_SUCCESS; } /* For backward compatibility purposes - To be removed */

    void* GetInstance (uint_t offset = 0u) const { return (void*) (this->m_instance + offset); } /* Pointer to buffer */
    void SetInstance (void* instance) { this->m_instance = (uint8_t*) instance; this->AssignReference(); return; } /* Pointer to buffer - Externally provided */
    void SetInstance (void* instance, uint_t size) { this->SetSize(size); this->SetInstance(instance); return; } /* For backward compatibility purposes - To be removed */

    uint_t GetAttributeMultiplicity (uint_t rank) const { TypeInfo_t info; info.mult = 0; this->GetAttributeInfo(info, rank); /* Unchanged if failed */ return info.mult; }
    uint_t GetAttributeMultiplicity (const char* name) const { return this->GetAttributeMultiplicity(this->GetAttributeRank(name)); }

    const char* GetAttributeName (uint_t rank) const { return (this->m_type)->GetKeyword(rank); }

    uint_t GetAttributeOffset (uint_t rank) const { TypeInfo_t info; info.offset = 0; this->GetAttributeInfo(info, rank); /* Unchanged if failed */ return info.offset; }
    uint_t GetAttributeOffset (const char* name) const { return this->GetAttributeOffset(this->GetAttributeRank(name)); }

    uint_t GetAttributeRank (const char* name) const;

    void* GetAttributeReference (uint_t rank) const;
    void* GetAttributeReference (const char* name) const { return this->GetAttributeReference(this->GetAttributeRank(name)); }

    RET_STATUS SetAttributeReference (uint_t rank, void* p_value); /* Pointer to buffer - Externally provided */
    RET_STATUS SetAttributeReference (const char* name, void* p_value) { return this->SetAttributeReference(this->GetAttributeRank(name), p_value); }

    uint_t GetAttributeSize (uint_t rank) const { TypeInfo_t info; info.size = 0; this->GetAttributeInfo(info, rank); /* Unchanged if failed */ return info.size; }
    uint_t GetAttributeSize (const char* name) const { return this->GetAttributeSize(this->GetAttributeRank(name)); }

    TypeIdentifier_t GetAttributeType (uint_t rank) const { TypeInfo_t info; info.type = ANYTYPE_ATTR_UNDEFINED; this->GetAttributeInfo(info, rank); /* Unchanged if failed */ return (TypeIdentifier_t) info.type; }
    TypeIdentifier_t GetAttributeType (const char* name) const { return this->GetAttributeType(this->GetAttributeRank(name)); }

    RET_STATUS GetAttribute (uint_t rank, void* p_value) const;
    RET_STATUS GetAttribute (const char* name, void* p_value) const { return this->GetAttribute(this->GetAttributeRank(name), p_value); }

    RET_STATUS SetAttribute (uint_t rank, void* p_value);
    RET_STATUS SetAttribute (const char* name, void* p_value) { return this->SetAttribute(this->GetAttributeRank(name), p_value); }

    template <class Type> RET_STATUS GetAttribute (uint_t rank, Type* p_value) const { if (this->HasAttribute(rank) != true) { return STATUS_ERROR; } return this->GetAttribute(rank, (void*) p_value); } /* Default does not provide type check though */
    template <class Type> RET_STATUS GetAttribute (const char* name, Type* p_value) const { if (this->HasAttribute(name) != true) { return STATUS_ERROR; } return this->GetAttribute<Type>(this->GetAttributeRank(name), p_value); }
    template <class Type> RET_STATUS SetAttribute (uint_t rank, Type value) { if (this->HasAttribute(rank) != true) { return STATUS_ERROR; } return this->SetAttribute(rank, (void*) &value); } /* Default does not provide type check though */ 
    template <class Type> RET_STATUS SetAttribute (const char* name, Type value) { if (this->HasAttribute(name) != true) { return STATUS_ERROR; } return this->SetAttribute<Type>(this->GetAttributeRank(name), value); }

    RET_STATUS GetAttribute (uint_t rank, char* buffer, int max_size = STRING_MAX_LENGTH) const { return SerializeAttribute(rank, buffer, max_size); } /* Value to be serialized */
    RET_STATUS SetAttribute (uint_t rank, const char* buffer) { return ParseAttribute(rank, buffer); } /* Value to be parsed */

    /* Miscellaneous methods */
    void ClearInstance (void);
    RET_STATUS CopyInstance (void* buffer);
    RET_STATUS CreateInstance (void);
    void DeleteInstance (void) { if (this->m_allocated && (this->m_instance != NULL)) delete [] this->m_instance; this->m_allocated = false; this->m_instance = NULL; /* Requires LUTable */ this->RemoveReference(); }
    RET_STATUS Compress (void); /* Would the type definition be incomplete */

    /* Constructor methods */
    AnyType (void) { this->Initialize(); return; }
    AnyType (uint_t size) { this->Initialize(); this->AddAttribute(0, "undefined", size); /* this->SetSize(size); */ return; } /* Size matters */
    AnyType (AttrInfo_t* list, uint_t list_size = MAXIMUM_ATTRIBUTE_NUM) { this->Initialize(); for (uint_t index = 0; index < list_size; index += 1) { if (IsUndefined(list[index].name) == true) break; else this->AddAttribute(STRING_UNDEFINED, list[index].name, list[index].type, list[index].mult, list[index].deflt); } return; } /* Scalability issue */
#if 1 /* Maintained for backward compatibility reason - 'Topic' class in m-sdn relies on this */
    AnyType (const char* name);
#endif
    /* Destructor method */
    virtual ~AnyType (void);

    /* Display methods */
    virtual uint_t SerializeType (char* buffer, int max_size = STRING_MAX_LENGTH) const;
    virtual uint_t SerializeType (void) const { char buffer [256] = STRING_UNDEFINED; return this->SerializeType((char*) buffer, 256); } /* Only interested in hash key */

    virtual RET_STATUS ParseAttribute (uint_t rank, const char* buffer);
    virtual RET_STATUS SerializeAttribute (uint_t rank, char* buffer, int max_size = STRING_MAX_LENGTH) const;

    virtual RET_STATUS ParseInstance (const char* buffer);
    virtual RET_STATUS SerializeInstance (char* buffer, int max_size = STRING_MAX_LENGTH) const;

};

/* Global variables */

/* Function declaration */

TypeIdentifier_t AnyType_GetTypeId (const char* type_name);
char* AnyType_GetTypeName (TypeIdentifier_t type_id);
uint_t AnyType_GetTypeSize (const char* type_name);
uint_t AnyType_GetTypeSize (TypeIdentifier_t type_id);
bool AnyType_IsAttrValid (TypeInfo_t& attr);

/* Function definition */

/* Accessor methods */

template <> inline RET_STATUS AnyType::AddAttribute<bool> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_BOOL, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<char> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_CHAR, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<char*> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_CHAR, mult); } /* Deprecated in v1.0.6 */
template <> inline RET_STATUS AnyType::AddAttribute<int8_t>  (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_SINT8, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<uint8_t> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_UINT8, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<int16_t>  (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_SINT16, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<uint16_t> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_UINT16, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<int32_t>  (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_SINT32, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<uint32_t> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_UINT32, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<int64_t>  (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_SINT64, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<uint64_t> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_UINT64, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<float>  (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_FLOAT, mult); }
template <> inline RET_STATUS AnyType::AddAttribute<double> (uint_t rank, const char* name, uint_t mult) { return this->AddAttribute(rank, name, ANYTYPE_ATTR_DOUBLE, mult); }

template <> inline RET_STATUS AnyType::GetAttribute<bool> (uint_t rank, bool* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<bool> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_BOOL:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<bool> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<char> (uint_t rank, char* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<char*> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_CHAR:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<char*> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<char*> (uint_t rank, char** p_value) const /* WARNING - Potentially problematic */ /* Deprecated in v1.0.6 */
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<char*> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_CHAR:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<char*> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<int8_t> (uint_t rank, int8_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<int8> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT8:
      case ANYTYPE_ATTR_UINT8:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<int8> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<uint8_t> (uint_t rank, uint8_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<uint8> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT8:
      case ANYTYPE_ATTR_UINT8:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<uint8> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<int16_t> (uint_t rank, int16_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<int16> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT16:
      case ANYTYPE_ATTR_UINT16:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<int16> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<uint16_t> (uint_t rank, uint16_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<uint16> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT16:
      case ANYTYPE_ATTR_UINT16:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<uint16> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<int32_t> (uint_t rank, int32_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<int32> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<int32> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<uint32_t> (uint_t rank, uint32_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<uint32> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<uint32> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}
#if 0
template <> inline RET_STATUS AnyType::GetAttribute<int> (uint_t rank, int* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<int> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<int> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<uint_t> (uint_t rank, uint_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<uint> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<uint> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}
#endif
template <> inline RET_STATUS AnyType::GetAttribute<int64_t> (uint_t rank, int64_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<int64> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT64:
      case ANYTYPE_ATTR_UINT64:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<int64> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<uint64_t> (uint_t rank, uint64_t* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<uint64> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT64:
      case ANYTYPE_ATTR_UINT64:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<uint64> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<float> (uint_t rank, float* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<float> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_FLOAT:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<float> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::GetAttribute<double> (uint_t rank, double* p_value) const
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::GetAttribute<double> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_DOUBLE:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::GetAttribute<double> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->GetAttribute(rank, (void*) p_value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<bool> (uint_t rank, bool value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<bool> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_BOOL:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<bool> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<char*> (uint_t rank, char* value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<char*> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_CHAR:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<char*> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  /* ToDo - Check string length */

  return this->SetAttribute(rank, (void*) value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<int8_t> (uint_t rank, int8_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<int8> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT8:
      case ANYTYPE_ATTR_UINT8:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<int8> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<uint8_t> (uint_t rank, uint8_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<uint8> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT8:
      case ANYTYPE_ATTR_UINT8:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<uint8> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<int16_t> (uint_t rank, int16_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<int16> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT16:
      case ANYTYPE_ATTR_UINT16:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<int16> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<uint16_t> (uint_t rank, uint16_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<uint16> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT16:
      case ANYTYPE_ATTR_UINT16:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<uint16> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<int32_t> (uint_t rank, int32_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<int32> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<int32> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<uint32_t> (uint_t rank, uint32_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<uint32> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<uint32> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}
#if 0
template <> inline RET_STATUS AnyType::SetAttribute<int> (uint_t rank, int value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<int> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<int> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<uint_t> (uint_t rank, uint_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<uint> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT32:
      case ANYTYPE_ATTR_UINT32:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<uint> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}
#endif
template <> inline RET_STATUS AnyType::SetAttribute<int64_t> (uint_t rank, int64_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<int64> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT64:
      case ANYTYPE_ATTR_UINT64:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<int64> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<uint64_t> (uint_t rank, uint64_t value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<uint64> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_SINT64:
      case ANYTYPE_ATTR_UINT64:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<uint64> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<float> (uint_t rank, float value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<float> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_FLOAT:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<float> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

template <> inline RET_STATUS AnyType::SetAttribute<double> (uint_t rank, double value) 
{ 

  RET_STATUS status = STATUS_ERROR;

  if (this->HasAttribute(rank) != true)
    {
      //log_error("AnyType::SetAttribute<double> - Invalid attribute '%u'", rank);
      return status;
    }

  switch (this->GetAttributeType(rank))
    {
      case ANYTYPE_ATTR_DOUBLE:
	break;
      default:
	/* Error - Wrong attribute type */ 
	//log_error("AnyType::SetAttribute<double> - Wrong attribute type '%u' for '%s'", this->GetAttributeType(rank), this->GetAttributeName(rank));
	return status;
    }

  return this->SetAttribute(rank, (void*) &value); 

}

} /* namespace base */

} /* namespace ccs */

#endif /* ANY_TYPE_H */

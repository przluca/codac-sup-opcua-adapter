#ifndef SDN_TOPIC_H
#define SDN_TOPIC_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/sdn-topic.h $
* $Id: sdn-topic.h 96477 2019-01-09 14:53:55Z sivecm $
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

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */
#include "sdn-tools.h"

//#include "sdn-base.h" /* Privately scoped case classes definition */
#include "any-type-ext.h" /* Extended type definition, description, units, etc. */

/* Constants */

#define InitializeMetadataStatic(name, size, vers) { name, STRING_UNDEFINED, 0u, size, vers, STRING_UNDEFINED, 0u}

/* Type definition */

namespace sdn {

typedef struct Metadata
{

  char name [STRING_MAX_LENGTH];
  //char vers [STRING_MAX_LENGTH];
  char desc [128]; // Bug 10852 - Topic description

  types::uint32 hash; /* UID computed from name, version, and type definition */
  types::uint32 size;
  types::uint32 vers;
  
  /* Derived from topic name */
  char mcast_group [MAX_IP_ADDR_LENGTH];
  types::uint32 mcast_port;
    
} Metadata_t; 

class Topic
{

  private:

    sdn::base::AnyType* m_type_def;
    uint8_t* m_instance;

    bool m_allocated;

    /* Initializer methods */
    void Initialize (void);
    //RET_STATUS Load (const char* file_path);

  public:

    Metadata_t m_meta; /* To collect all metadata information */

    /* Initializer methods */
    RET_STATUS Configure (void);

    RET_STATUS AddAttribute (uint_t rank, const char* name, const char* type, uint_t mult = 1); /* WARNING - MARTe2 Integration - Interface used by SDNPublisher DataSource */
    template <class Type> RET_STATUS AddAttribute (uint_t rank, const char* name, uint_t mult = 1) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def != NULL) status = (this->m_type_def)->AddAttribute<Type>(rank, (char*) name, mult); this->SetUID(); return status; }
 
    void ClearInstance (void);
    RET_STATUS CreateInstance (void);
    void DeleteInstance (void) { if ((this->m_type_def != NULL) && ((this->m_type_def)->IsInitialized() == true)) (this->m_type_def)->DeleteInstance(); else if ((this->m_allocated == true) && (this->m_instance != NULL)) delete [] this->m_instance; this->m_instance = NULL; return; }

    /* Accessor methods */
    bool IsInitialized (void) const { return (((this->GetSize() != 0) && (this->GetSize() <= MAX_TOPIC_SIZE) && ((this->m_type_def)->IsDefined() == true))? true : false ); }

    const char* HasInstanceIndex (void) const;
    const char* HasInstanceTimestamp (void) const;

    void GetMetadata (Metadata_t& mdata) const;
    void SetMetadata (Metadata_t& mdata);

    bool IsName (const char* name) const;
    const char* GetName (void) const;
    void SetName (const char* name);

    uint_t GetInstanceChecksum (void) const;

    // Bug 10852 - Topic description
    bool HasDescription (void) const;
    const char* GetDescription (void) const;
    bool GetDescription (char* desc, types::uint32 size = STRING_MAX_LENGTH) const;
    void SetDescription (const char* desc);

    uint_t GetSize (void) const;
    void SetSize (uint_t size);
    void SetSize (const char* size_str);

    uint_t GetUID (void) const;
    void SetUID (uint_t hash);
    void SetUID (const char* uid_str);
    void SetUID (void);

    uint_t GetVersion (void) const;
    void SetVersion (const char* vers);
    void SetVersion (uint_t vers);

    const char* GetMCastGroup (void) const;
    void SetMCastGroup (const char* group); /* Warning - Size of string */

    uint_t GetMCastPort (void) const;
    void SetMCastPort (uint_t port);
    void SetMCastPort (const char* port_str);

    void* GetInstance (void) const;
    void SetInstance (void* instance);

    sdn::base::AnyType* GetTypeDefinition (void) { return this->m_type_def; };
    RET_STATUS SetTypeDefinition (sdn::base::AnyType* p_def) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def == NULL) { this->m_type_def = p_def; status = STATUS_SUCCESS; } return status; }

    template <class Type> RET_STATUS GetAttribute (uint_t rank, Type* p_value) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def != NULL) status = (this->m_type_def)->GetAttribute((uint_t) rank, (Type*) p_value); return status; }
    RET_STATUS GetAttribute(uint_t rank, void* p_value) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def != NULL) status = (this->m_type_def)->GetAttribute(rank, p_value); return status; }
    RET_STATUS GetAttribute(const char* name, void* p_value) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def != NULL) status = (this->m_type_def)->GetAttribute((char*) name, p_value); return status; }

    template <class Type> RET_STATUS SetAttribute (uint_t rank, Type value) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def != NULL) status = (this->m_type_def)->SetAttribute((uint_t) rank, (Type) value); return status; }
    RET_STATUS SetAttribute(uint_t rank, void* p_value) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def != NULL) status = (this->m_type_def)->SetAttribute(rank, p_value); return status; }
    RET_STATUS SetAttribute(const char* name, void* p_value) { RET_STATUS status = STATUS_ERROR; if (this->m_type_def != NULL) status = (this->m_type_def)->SetAttribute((char*) name, p_value); return status; }

    /* Miscellaneous methods */

    /* Constructor methods */
    Topic (void);
    Topic (Metadata_t& mdata);
    //Topic (const char* name);

    /* Destructor method */
    virtual ~Topic (void);

    /* Display methods */
    uint_t SerializeType (char* buffer, int max_size = STRING_MAX_LENGTH);

    RET_STATUS SerializeInstance (char* buffer, int max_size = STRING_MAX_LENGTH);

};

/* Global variables */

/* Function declaration */

namespace TopicHelper {

RET_STATUS GenerateMCastAddress (const char* name, char* group, uint_t& port); // Bug 9729 - This method missing for final porting of SDN API
bool IsMetadataValid (Metadata_t& mdata);

/* 
   Documentation - The topic metadata could be instantiated in short form using 'sdn://<address>:<port>/<name>'.
 */

void InitializeMetadata (Metadata_t& mdata);
void InitializeMetadata (Metadata_t& mdata, const char* name);
void InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size);
void InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size, const char* addr, uint_t port = DEFAULT_MCAST_PORT);
void InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size, uint_t vers);

/* Function definition */

static inline RET_STATUS GenerateMCastAddress (Metadata_t& mdata) { return GenerateMCastAddress((const char*) mdata.name, (char*) mdata.mcast_group, mdata.mcast_port); }

static inline bool IsAddressValid (Metadata_t& mdata) { return HelperTools::IsAddressValid((const char*) mdata.mcast_group); }
static inline bool IsMCastAddress (Metadata_t& mdata) { return HelperTools::IsMCastAddress((const char*) mdata.mcast_group); }
static inline bool IsUCastAddress (Metadata_t& mdata) { return HelperTools::IsUCastAddress((const char*) mdata.mcast_group); }

static inline Topic* InstantiateFromMetadata (Metadata_t& mdata)
{

  Topic* ref = static_cast<Topic*>(NULL);

  if (IsMetadataValid(mdata) == true)
    {
      ref = new Topic;
      ref->SetMetadata(mdata);
    }

  return ref;

}

} /* namespace TopicHelper */

/* For backward compatibility purposes with CCSv5.3 */
static inline RET_STATUS Topic_GenerateMCastAddress (Metadata_t& mdata) { return TopicHelper::GenerateMCastAddress(mdata); }
static inline bool Topic_IsMetadataValid (Metadata_t& mdata) { return TopicHelper::IsMetadataValid(mdata); }

static inline void Topic_InitializeMetadata (Metadata_t& mdata) { return TopicHelper::InitializeMetadata(mdata); }
static inline void Topic_InitializeMetadata (Metadata_t& mdata, const char* name) { return TopicHelper::InitializeMetadata(mdata, name); }
static inline void Topic_InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size) { return TopicHelper::InitializeMetadata(mdata, name, size); }
static inline void Topic_InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size, const char* addr, uint_t port = DEFAULT_MCAST_PORT) { return TopicHelper::InitializeMetadata(mdata, name, size, addr, port); }
static inline void Topic_InitializeMetadata (Metadata_t& mdata, const char* name, uint_t size, uint_t vers) { return TopicHelper::InitializeMetadata(mdata, name, size, vers); }

static inline bool Topic_IsAddressValid (Metadata_t& mdata) { return TopicHelper::IsAddressValid(mdata); }
static inline bool Topic_IsMCastAddress (Metadata_t& mdata) { return TopicHelper::IsMCastAddress(mdata); }
static inline bool Topic_IsUCastAddress (Metadata_t& mdata) { return TopicHelper::IsUCastAddress(mdata); }

} /* namespace sdn */

#endif /* SDN_TOPIC_H */
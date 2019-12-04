#ifndef ANY_TYPE_EXT_H
#define ANY_TYPE_EXT_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/any-type-ext.h $
* $Id: any-type-ext.h 99192 2019-04-19 08:32:14Z bauvirb $
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

//#include <any-type.h> /* Runtime datatype definition */

/* Local header files */

#include "any-type.h" /* Runtime datatype definition */

#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */

//#include "sdn-base.h" /* Privately scoped case classes definition */

/* Constants */

/* Type definition */

namespace sdn {

namespace base {

typedef struct ExtTypeInfo { /* For each attribute within a structure, the name will be defined externally as keyword in LUTable */

  uint32_t rank; /* Attribute rank within the structure */
  char desc [STRING_MAX_LENGTH]; /* Attribute free description string */
  char qual [STRING_MAX_LENGTH]; /* Attribute qualifier */
  char unit [STRING_MAX_LENGTH]; /* Attribute unit */
  char mult [STRING_MAX_LENGTH]; /* Attribute multiplicity string */

} ExtTypeInfo_t;

class AnyType : public ccs::base::AnyType
{

  private:

    LUTable<ExtTypeInfo_t>* m_type_ext; /* Extended attribute look-up table */

    /* Initializer methods */
    void Initialize (void);

  public:

    /* Initializer methods */
    RET_STATUS AddExtAttribute (const char* rank, const char* name, const char* desc, const char* qual, const char* unit, const char* mult);

    /* Accessor methods */
    const char* GetAttributeDescription (uint_t rank) const;
    const char* GetAttributeDescription (const char* name) const { return this->GetAttributeDescription(this->GetAttributeRank(name)); };

    const char* GetAttributeQualifier (uint_t rank) const;
    const char* GetAttributeQualifier (const char* name) const { return this->GetAttributeQualifier(this->GetAttributeRank(name)); };

    const char* GetAttributeUnit (uint_t rank) const;
    const char* GetAttributeUnit (const char* name) const { return this->GetAttributeUnit(this->GetAttributeRank(name)); };

    const char* GetAttributeMultiplicity (uint_t rank) const;
    const char* GetAttributeMultiplicity (const char* name) const { return this->GetAttributeMultiplicity(this->GetAttributeRank(name)); };

    /* Miscellaneous methods */
    RET_STATUS Compress (void); /* Would the type definition be incomplete */

    /* Constructor methods */
    AnyType (void); /* Type definition using the class API */

    /* Destructor method */
    virtual ~AnyType (void) { /* Nothing further than the default destructor */ return; };

    /* Display methods */
    virtual uint_t SerializeType (char* buffer, int max_size = STRING_MAX_LENGTH) const; /* Specializes virtual method - XML format */
    uint_t SerializeType (void) const { char buffer [2048] = STRING_UNDEFINED; return this->SerializeType((char*) buffer, 2048); }; /* Only interested in hash key */

};

/* Global variables */

/* Function declaration */

/* Function definition */

} /* namespace base */

} /* namespace sdn */

#endif /* ANY_TYPE_EXT_H */
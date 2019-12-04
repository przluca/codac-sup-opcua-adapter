/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/PublisherImpl.h $
* $Id: PublisherImpl.h 100239 2019-06-23 15:40:27Z bauvirb $
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

/**
 * @file PublisherImpl.h
 * @brief Header file for sdn::Publisher implementation classes
 * @date 18/04/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 *
 * @detail This header file contains the definition of the Publisher
 * implementation classes.
 */

#ifndef _PublisherImpl_h_
#define _PublisherImpl_h_

// Global header files

// Local header files

#include "constants.h" // Constants valid for this scope
#include "sdn-types.h" // Misc. type definition, e.g. RET_STATUS

#include "sdn-base.h" // Base classes definition

#include "ParticipantImpl.h"

// Constants

#define OBJTYPE_SDN_SUBSCRIBER (char*) "sdn::Publisher_Base"

#ifdef __cplusplus

// Type definition

namespace sdn {

class Publisher_Base : public ParticipantImpl // Base class
{

  public:

    // Initialiser methods

    // Accessor methods

    // Miscellaneous methods
    virtual RET_STATUS Do (void) { RET_STATUS status = this->Publish(); this->DoBackgroundActivity();  return status; }; // Specialises virtual method
    virtual RET_STATUS Publish (void) = 0; // Pure virtual method

    // Constructor methods
    Publisher_Base (void);

    // Destructor method
    virtual ~Publisher_Base (void) {};

};

class PublisherImpl : public Publisher_Base
{

  private:

    // Initialiser methods
    void Initialise (void);

  public:

    // Initialiser methods

    // Accessor methods
    types::Endianness_t GetPayloadOrder (void) const;
    types::Endianness_t SetPayloadOrder (types::Endianness_t order);

    RET_STATUS SetInterface (const char* name, uint_t port);

    // Miscellaneous methods
    virtual RET_STATUS Configure (void); // Specialises virtual method
    virtual RET_STATUS Publish (void);   // Specialises virtual method

    // Constructor methods
    PublisherImpl (void);

    // Destructor method
    virtual ~PublisherImpl (void);

};

// Global variables

// Function declaration

// Function definition

} // namespace sdn

extern "C" {

#endif // __cplusplus

// ToDo - Insert C API declaration

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _PublisherImpl_h_

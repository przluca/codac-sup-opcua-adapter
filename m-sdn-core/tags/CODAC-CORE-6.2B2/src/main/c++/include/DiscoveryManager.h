/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/main/c++/include/DiscoveryManager.h $
* $Id: DiscoveryManager.h 100239 2019-06-23 15:40:27Z bauvirb $
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
 * @file DiscoveryManager.h
 * @brief Header file for sdn::DiscoveryManager interface class.
 * @date 15/06/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 *
 * @detail This header file contains the definition of the DiscoveryManager
 * interface class.
 *
 * @todo Provide sdn::Topic factory services.
 */

#ifndef _DiscoveryManager_h_
#define _DiscoveryManager_h_

/* Global header files */

/* Local header files */

#include "constants.h" /* Constants valid for this scope */
#include "sdn-types.h" /* Misc. type definition, e.g. RET_STATUS */

/* Constants */

#ifdef __cplusplus

/* Type definition */

namespace sdn {

class DiscoveryManagerImpl; /* Forward class declaration */

/**
 * @brief The class provides ... .
 * @detail After instantiation, the network interface can be specified
 * using the SetInterface method. ... . 
 *
 * @notice The design is based on the 'bridge' pattern hiding implementation
 * details to the application through the API and, as such, enables changes
 * of implementation with limited impact on the application themselves.
 */

class DiscoveryManagerIface
{

  private:
        
    /**
     * @brief Reference to the actual implementation.
     */
    DiscoveryManagerImpl* p_impl;
        
  public:

    /**
     * @brief Constructor. 
     */
    DiscoveryManagerIface (void);

    /**
     * @brief Destructor. 
     */
    virtual ~DiscoveryManagerIface (void);

    /**
     * @brief Accessor method.
     * @detail Returns the interface bound to by instance.
     * @return Interface name.
     */
    const char* GetInterface (void) const;

    /**
     * @brief SetInterface method. Specifies the network interface to bind to.
     * @detail The method can be called to overwrite the default SDN interface
     * which is defined through SDN_INTERFACE_NAME environment variable or set
     * to 'lo' if undefined.
     * @pre 
     *   ccs::base::IsInterfaceValid(GetInterface()) == true
     * @return STATUS_SUCCESS if preconditions are met, STATUS_ERROR otherwise.
     */
    RET_STATUS SetInterface (const char* name);
        
    /**
     * @brief DoBackgroundActivity method.
     * @detail The method performs house-keeping operations and should be
     * called at regular intervals in order to handle e.g. the discovery
     * protocol and replies to topic introspection requests, etc.
     */
    RET_STATUS DoBackgroundActivity (void);

};

typedef DiscoveryManagerIface DiscoveryManager;

/* Global variables */

/* Function declaration */

namespace GlobalDiscoveryManager {

DiscoveryManager* GetInstance (void);
const char* GetInterface (void);
RET_STATUS SetInterface (const char* name);
RET_STATUS DoBackgroundActivity (void);

} /* namespace GlobalDiscoveryManager */

/* Function definition */

} /* namespace sdn */

extern "C" {

#endif /* __cplusplus */

/* ToDo - Insert C API declaration */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DiscoveryManager_h_ */

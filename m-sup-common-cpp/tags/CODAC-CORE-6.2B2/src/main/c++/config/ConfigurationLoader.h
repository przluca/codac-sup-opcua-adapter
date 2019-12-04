/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationLoader.h $
* $Id: ConfigurationLoader.h 100089 2019-06-07 18:32:32Z bauvirb $
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
 * @file ConfigurationLoader.h
 * @brief Header file for ConfigurationLoader class.
 * @date 13/05/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the ConfigurationLoader class interface.
 * The ConfigurationLoader class implementation is not exposed through this header file. A bridge
 * pattern is favoured to try and avoid exposing potentially changing implementation details
 * to calling applications. The configuration load protocol over the selected middleware is managed
 * by this class. The class provides interface to query/load configurationto a remote
 * ConfigurationService instance.
 */

#ifndef _ConfigurationLoader_h_
#define _ConfigurationLoader_h_

// Global header files

#include <BasicTypes.h>

// Local header files

#include "ConfigurationHasher.h"

// Constants

// Type definition

namespace sup {

namespace core {

class ConfigurationLoaderImpl; // Forward class declaration

/**
 * @brief Interface class providing support for configuration function.
 * @detail The implementation provides a RPC client to a named service and
 * support for potentially named data sets.
 *
 * Support for named data sets is foreseen to account for use-cases where the adaptation
 * is provided for multiple underlying interfaces, or configuration in several steps, e.g.
 * first configuration action affects the application behaviour (application framework
 * instance configuration) and hence the structure of more detailed configuration data.
 *
 * @note The design is based on a bridge pattern to avoid exposing implementation
 * specific details through the interface class.
 *
 * @todo Reassess the need or not for named data sets.
 *
 * @todo Introspect expected data set names.
 */

class ConfigurationLoader
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    ConfigurationLoaderImpl* __impl;

  protected:

  public:

    /**
     * @brief Constructor.
     * @param service Named RPC service to connect to.
     */

    ConfigurationLoader (const std::string& service);

    /**
     * @brief Destructor.
     */

    virtual ~ConfigurationLoader (void);

    /**
     * @brief Accessor.
     */

    bool RegisterHasher (ConfigurationHasher* hasher);

    bool IsConnected (void) const;
    bool ReadConfiguration (const std::string& name, ccs::types::AnyValue& value) const;
    bool LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value) const;

};

// Global variables

} // namespace core

} // namespace sup

#endif // _ConfigurationLoader_h_ 


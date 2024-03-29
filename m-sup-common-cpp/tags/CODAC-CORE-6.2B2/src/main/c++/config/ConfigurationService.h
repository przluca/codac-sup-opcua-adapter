/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationService.h $
* $Id: ConfigurationService.h 100195 2019-06-19 13:35:27Z bauvirb $
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
 * @file ConfigurationService.h
 * @brief Header file for ConfigurationService class.
 * @date 13/05/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the ConfigurationService class interface.
 * The ConfigurationService class implementation is not exposed through this header file. A bridge
 * pattern is favoured to try and avoid exposing potentially changing implementation details
 * to calling applications. The configuration load protocol over the selected middleware is managed
 * by this class.
 * The ConfigurationService implements a Remote Procedure Call (RPC) mechanism over the network
 * for loading configuration variable.
 * The API is that of a ccs::base::ConfigurableObject and ccs::base::MsgableObject to be manageable
 * through the sup::core::ObjectFactoryEngine.
 * The interface may also be addeded through composition to a compiled application.
 */

#ifndef _ConfigurationService_h_
#define _ConfigurationService_h_

// Global header files

// Local header files

#include <BasicTypes.h>
#include <AnyObject.h>

// Constants

// Type definition

namespace sup {

namespace core {

class ConfigurationHandler; // Forward class declaration
class ConfigurationHasher; // Forward class declaration
class ConfigurationServiceImpl; // Forward class declaration

/**
 * @brief Interface class providing support for configuration function.
 * @detail The implementation provides a named service and potentially named data sets.
 *
 * @note The design is based on a bridge pattern to avoid exposing implementation
 * specific details through the interface class.
 *
 * @todo Ensure/verify proper termination upon deletion.
 *
 * @todo Introspect expected data set names.
 */

class ConfigurationService : public ccs::base::AnyObject, public ccs::base::CfgableObject, public ccs::base::MsgableObject
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    ConfigurationServiceImpl* __impl;

  protected:

  public:

    /**
     * @brief Constructor.
     */

    ConfigurationService (void);

    /**
     * @brief Destructor.
     */

    virtual ~ConfigurationService (void);

    /**
     * @brief Accessor. See ccs::base::CfgableObject::SetAttribute.
     * @detail Sets service name, etc.
     * @code
       <object application="codac:obj-factory" name="55a0-loader">
         <libraries>
           <!-- Load basic library -->
           <library>libsup-cvvf.so</library> <!-- CVVF related classes -->
           <library>lib55a0-config.so</library> <!-- Plant System classes -->
         </libraries>
         <instances>
           <instance name="55a0-adapter" type="55a0::ConfigurationHandler">
             <parameter name="etc">true</parameter>
           </instance>
           <instance name="55a0-service" type="sup::core::ConfigurationService">
             <parameter name="service">55a0::Cfg::Interface</parameter>
             <parameter name="handler">55a0-adapter</parameter> <!-- Instance name -->
             <message>Launch()</message>
           </instance>
         </instances>
       </object>
       @endcode
     */

    virtual bool SetParameter (const char* name, const char* value); // Specialises virtual method
 
    /**
     * @brief Accessor. See ccs::base::MsgableObject::ProcessMessage.
     * @detail Messages correspond to orders to start RPC service, etc.
     */

    virtual bool ProcessMessage (const char* msg); // Specialises virtual method

    /**
     * @brief Accessor.
     * @detail Sets service name and starts the RPC service.
     * @param service RPC service name.
     * @return True is service successfully started, false otherwise
     */

    bool SetService (const char* service); // Implicit launch

    /**
     * @brief Accessor.
     * @detail Provides ConfigurationHandler implementation.
     * @param handler Instance providing the ConfigurationHandler interface.
     * @return True.
     */

    bool RegisterHandler (ConfigurationHandler* handler);

    /**
     * @brief Accessor.
     * @detail Provides ConfigurationHandler implementation by name.
     * @param handler Instance providing the ConfigurationHandler interface.
     * @return True if the instance name is registered in the GlobalObjectDatabase.
     */

    bool RegisterHandler (const char* handler); // Instance name

    /**
     * @brief Accessor.
     * @detail Provides ConfigurationHasher implementation. By default, the
     * ConfigurationService will instatiate ConfigurationHasher providing
     * CRC-32 implementation.
     * @param hasher Instance providing the ConfigurationHasher interface.
     * @return True.
     */

    bool RegisterHasher (ConfigurationHasher* hasher);

    /**
     * @brief Accessor.
     * @detail Provides ConfigurationHasher implementation by name.
     * @param handler Instance providing the ConfigurationHasher interface.
     * @return True if the instance name is registered in the GlobalObjectDatabase.
     */

    bool RegisterHasher (const char* hasher); // Instance name

};

// Global variables

} // namespace core

} // namespace sup

#endif // _ConfigurationService_h_ 


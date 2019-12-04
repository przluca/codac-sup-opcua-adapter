/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationHandler.h $
* $Id: ConfigurationHandler.h 100195 2019-06-19 13:35:27Z bauvirb $
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
 * @file ConfigurationHandler.h
 * @brief Header file for ConfigurationHandler class.
 * @date 13/05/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the ConfigurationHandler interface.
 * Applications provide interface adapters by overloading virtual methods of the base class.
 */

#ifndef _ConfigurationHandler_h_
#define _ConfigurationHandler_h_

// Global header files

// Local header files

#include <BasicTypes.h>

#include <AnyObject.h>

#include <AnyType.h>
#include <AnyTypeHelper.h>

#include <AnyValue.h>
#include <AnyValueHelper.h>

// Constants

// Type definition

namespace sup {

namespace core {

/**
 * @brief Interface class providing support for configuration related function.
 * @detail Interface adaptation is provided by inheriting from this base class and
 * specialising at least two methods.
 *
 * Support for named data sets is foreseen to account for use-cases where the adaptation
 * is provided for mltiple underlying interfaces, or configuration in several steps, e.g.
 * first configuration action affects the application behaviour (application framework
 * instance configuration) and hence the structure of more detailed configuration data.
 *
 * @note Inherits from ccs::base::AnyObject so that sup::core::Configuration service may
 * find instances through the ccs::base::GlobalObjectDatabase and verify interface by means
 * of RTTI.
 *
 * @todo Reassess the need or not for named data sets.
 *
 * @todo Propose a pattern similar to that of the FunctionDatabase where callbacks are
 * being registered and called directly with User-defined C-like structures.
 *
 * @todo Introspect expected data set names.
 */

class ConfigurationHandler : public ccs::base::AnyObject
{

  private:

  protected:

  public:

    /**
     * @brief Constructor.
     */

    ConfigurationHandler (void);

    /**
     * @brief Destructor.
     */

    virtual ~ConfigurationHandler (void);

    /**
     * @brief Accessor. 
     * @detail Method provided to return the current named configuration data set. The
     * configuration variable is provided as a placeholder. The type definition and instance
     * needs to be provided by the implementation.
     * @param name Named data set.
     * @param value Introspectable variable set-up by the body of the method implementation.
     * @return True in case of success, false otherwise.
     * @code
       namespace user {
       class Handler : public sup::core::ConfigurationHandler {

         private:

           struct __attribute__((packed)) Config_t {
             ccs::types::boolean enabled = true;
             ccs::types::float64 setpoint = 0.0;
           } __config_data;

           struct __attribute__((packed)) Limits_t {
             ccs::types::float64 max = +10.0;
             ccs::types::float64 min = -10.0;
           } __limits_data;

           std::shared_ptr<const ccs::types::AnyType> __config_type;
           std::shared_ptr<const ccs::types::AnyType> __limits_type;

         public:

           Handler (void) {
             // Introspectable type definition
	     __config_type = std::shared_ptr<const ccs::types::AnyType>(((new (std::nothrow) ccs::types::CompoundType type ("user::Config_t"))
	                                                                 ->AddAttribute("enabled", "boolean")
	                                                                 ->AddAttribute("setpoint", "float64")));
	     __limits_type = std::shared_ptr<const ccs::types::AnyType>(((new (std::nothrow) ccs::types::CompoundType type ("user::Limits_t"))
	                                                                 ->AddAttribute("maximum", "float64")
	                                                                 ->AddAttribute("minimum", "float64")));
	   };

           virtual ~Handler (void) {};

           // Read configuration data set
           virtual bool ReadConfiguration (const std::string& name, ccs::types::AnyValue& value) const {

             bool status = true;

	     if (name == "config")
	       {
                 //value.SetType(__config_type);
		 value = __config_data;
	       }
	     else if (name == "limits")
	       {
                 //value.SetType(__limits_type);
		 value = __limits_data;
	       }
	     else
	       {
	         status = false;
	       }

	     return status;

           };

	   // Etc.
       };
       } // namespace user
       @endcode
     *
     * @note sup::core::ConfigurationService class tries and catch exceptions calling this interface.
     * In case an exception is thrown, the reason will be encapsulated by sup::core::ConfigurationService
     * into the RPC reply.
     *
     * @todo Reassess the need or not for named data sets.
     */

    virtual bool ReadConfiguration (const std::string& name, ccs::types::AnyValue& value) const = 0;

    /**
     * @brief Accessor. 
     * @detail Method provided to return the current named configuration data set. The
     * configuration variable is provided as a placeholder. The type definition is provided by the
     * implementation when ReadConfiguration is called.
     * The seed was generated by a previous call to GetSeed method. The checksum was computed at the
     * client side and can be propagated down to ensure the configuration data set is not aletered in
     * its way down the the appropriate target.
     * @param name Named data set.
     * @param value Introspectable variable conforming to the type provided for the named data set.
     * @param seed Unique key used to generate the checksum produced at client level.
     * @param checksum CRC-32 computed using the configuration data set and the seed.
     * @return True in case of success, false otherwise.
     * @code
       namespace user {
       class Handler {

         private:

           struct __attribute__((packed)) Config_t {
             ccs::types::boolean enabled = true;
             ccs::types::float64 setpoint = 0.0;
           } __config_data;

           struct __attribute__((packed)) Limits_t {
             ccs::types::float64 max = +10.0;
             ccs::types::float64 min = -10.0;
           } __limits_data;

           std::shared_ptr<const ccs::types::AnyType> __config_type;
           std::shared_ptr<const ccs::types::AnyType> __limits_type;

         public:

           Handler (void) : public sup::core::ConfigurationHandler {
             // Introspectable type definition
	     __config_type = std::shared_ptr<const ccs::types::AnyType>(((new (std::nothrow) ccs::types::CompoundType type ("user::Config_t"))
	                                                                 ->AddAttribute("enabled", "boolean")
	                                                                 ->AddAttribute("setpoint", "float64")));
	     __limits_type = std::shared_ptr<const ccs::types::AnyType>(((new (std::nothrow) ccs::types::CompoundType type ("user::Limits_t"))
	                                                                 ->AddAttribute("maximum", "float64")
	                                                                 ->AddAttribute("minimum", "float64")));
	   };

           virtual ~Handler (void) {};

           // Load configuration data set
           virtual bool LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum) {

             bool status;

	     if (name == "config")
	       {
		 __config_data = static_cast<Config_t>(value);
		 status = (checksum == ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(&__config_data), sizeof(Config_t), seed));
	       }
	     else if (status && (name == "limits"))
	       {
		 __limits_data = static_cast<Limits_t>(value);
		 status = (checksum == ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(&__limits_data), sizeof(Limits_t), seed));
	       }
	     else
	       {
	         status = false;
	       }

	     return status;

           };

	   // Etc.
       };
       } // namespace user
       @endcode
     *
     * @note sup::core::ConfigurationService class tries and catch exceptions calling this interface.
     * In case an exception is thrown, the reason will be encapsulated by sup::core::ConfigurationService
     * into the RPC reply.
     *
     * @todo Reassess the need or not for named data sets.
     */

    virtual bool LoadConfiguration (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum) = 0;

    /**
     * @brief Accessor. 
     * @detail Provides optional capability for the implementation to deliver the seed for the client-side
     * checksum computation. The seed can therefore be delivered by the appropriate target which will 
     * eventually host the configuration data set and therefore can ensure consistency of the data down through
     * its final location.
     *
     * The default implementation provided by the base class is to compute the CRC-32 of the current ITER time
     * using a 32-bits hash generated from the name of the data set.
     * @param name Named data set.
     * @param seed Placeholder for the returned value.
     * @return True if successful, false otherwise.
     *
     * @note sup::core::ConfigurationService class tries and catch exceptions calling this interface.
     * In case an exception is thrown, the reason will be encapsulated by sup::core::ConfigurationService
     * into the RPC reply.
     */

    virtual bool GetSeed (const std::string& name, ccs::types::uint32& seed) const;
 
};

// Global variables

} // namespace core

} // namespace sup

#endif // _ConfigurationHandler_h_ 


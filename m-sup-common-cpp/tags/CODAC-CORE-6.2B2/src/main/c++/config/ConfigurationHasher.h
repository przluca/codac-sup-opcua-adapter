/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/config/ConfigurationHasher.h $
* $Id: ConfigurationHasher.h 100195 2019-06-19 13:35:27Z bauvirb $
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
 * @file ConfigurationHasher.h
 * @brief Header file for ConfigurationHasher class.
 * @date 13/05/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the ConfigurationHasher interface.
 * Applications provide interface adapters by overloading virtual methods of the base class.
 */

#ifndef _ConfigurationHasher_h_
#define _ConfigurationHasher_h_

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

class ConfigurationHasher : public ccs::base::AnyObject
{

  private:

  protected:

  public:

    /**
     * @brief Constructor.
     */

    ConfigurationHasher (void);

    /**
     * @brief Destructor.
     */

    virtual ~ConfigurationHasher (void);

    /**
     * @brief ComputeChecksum method. 
     * @detail Provides optional capability for the implementation to specialise the checksum computation
     * algorithm used to ensure configuration data integrity through to ConfigurationHandler.
     *
     * The default implementation provided by the base class is to compute the CRC-32 of the packed memory
     * footprint of the configuration variable.
     * @param name Named data set.
     * @param value Introspectable configuration variable.
     * @param seed Computation seed returned by the ConfigurationHandler for the named data set.
     * @param checksum Placeholder for the returned value.
     * @return True if successful, false otherwise.
     * @code
       namespace user {
       class Hasher : public sup::core::ConfigurationHasher {

         private:

         public:

           Hasher (void) {};
           virtual ~Hasher (void) {};

           // Specialised implementation
           virtual bool ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum) const {

	     bool status = (NULL != value.GetInstance());

	     if (status)
	       {
	         checksum = ccs::HelperTools::CyclicRedundancyCheck<ccs::types::uint32>(reinterpret_cast<ccs::types::uint8*>(value.GetInstance()), value.GetSize(), seed);
	       }

	     return status;

           };

	   // Etc.
       };
       } // namespace user
       @endcode
     *
     * @note sup::core::ConfigurationService and sup::core::ConfigurationLoader classes do not try and catch
     * exceptions calling this interface. Exception handling is provided in the form of returning boolean
     * status. This is not marked using noexcept specifier to avoid imposing std=c++11 in User-supplied code.
     */

    virtual bool ComputeChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, ccs::types::uint32& checksum) const; 
    virtual bool VerifyChecksum (const std::string& name, const ccs::types::AnyValue& value, const ccs::types::uint32 seed, const ccs::types::uint32 checksum) const; 

};

// Global variables

} // namespace core

} // namespace sup

#endif // _ConfigurationHasher_h_ 

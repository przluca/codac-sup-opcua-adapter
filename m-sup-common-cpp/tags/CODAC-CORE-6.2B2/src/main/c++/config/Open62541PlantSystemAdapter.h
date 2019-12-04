/**
 * @file Open62541PlantSystemAdapter.h
 * @brief Header file for Open62541PlantSystemAdapter class.
 * @date 11/11/2018
 * @author Luca Porzio (F4E-Ext)
 * @detail This header file contains the definition of the Open62541PlantSystemAdapter class interface.
 * The Open62541PlantSystemAdapter class implementation is not exposed through this header file. A bridge
 * pattern is favoured to try and avoid exposing potentially changing implementation details
 * to calling applications.
 * The Open62541PlantSystemAdapter implements a Remote Procedure Call (RPC) mechanism over the network
 * for loading configuration variable and mapping them to OPCUA records through Channel Access.
 * The API is that of a ccs::base::ConfigurableObject and ccs::base::MsgableObject to be managed
 * through the sup::core::ObjectFactoryEngine.
 */

#ifndef _Open62541PlantSystemAdapter_h_
#define _Open62541PlantSystemAdapter_h_

// Global header files

#include <BasicTypes.h>
#include <AnyObject.h>

// Local header files

#include "ConfigurationHandler.h"

// Constants

// Type definition

namespace sup {

namespace core {

class Open62541PlantSystemAdapterImpl; // Forward class declaration

/**
 * @brief Interface class providing support for configuration over Remote Procedure
 * Call (RPC) and serialisation to OPCUA records through Channel Access.
 * @detail ToDo.
 *
 * @note The design is based on a bridge pattern to avoid exposing implementation
 * specific details through the interface class.
 */

class Open62541PlantSystemAdapter: public sup::core::ConfigurationHandler, public ccs::base::CfgableObject, public ccs::base::MsgableObject {

private:

    /**
     * @brief Attribute.
     * @detail Bridge (PIMPL) pattern.
     */

    Open62541PlantSystemAdapterImpl *__impl;

protected:

public:

    /**
     * @brief Constructor.
     */

    Open62541PlantSystemAdapter(void);

    /**
     * @brief Destructor.
     */

    virtual ~Open62541PlantSystemAdapter(void);

    /**
     * @brief Accessor. See ccs::base::CfgableObject::SetAttribute.
     * @detail Sets service name, etc.
     */

    virtual bool SetParameter(const char *name,
                              const char *value); // Specialises virtual method

    /**
     * @brief Accessor. See ccs::base::MsgableObject::ProcessMessage.
     * @detail Messages correspond to orders to load library of User-supplied
     * types, start RPC service, etc.
     */

    virtual bool ProcessMessage(const char *msg); // Specialises virtual method

    /**
     * @brief Behaviour. See sup::core::ConfigurationHandler::ReadConfiguration.
     * @detail Configuration attributes are filled with current values of EPICSv3 channels.
     */

    virtual bool ReadConfiguration(const std::string &name,
                                   ccs::types::AnyValue &value) const; // Specialises sup::core::ConfigurationHandler interface

    /**
     * @brief Behaviour. See sup::core::ConfigurationHandler::LoadConfiguration.
     * @detail EPICSv3 channels are written from configuration attributes.
     */

    virtual bool LoadConfiguration(const std::string &name,
                                   const ccs::types::AnyValue &value,
                                   const ccs::types::uint32 seed,
                                   const ccs::types::uint32 checksum); // Specialises sup::core::ConfigurationHandler interface

};

// Global variables

}// namespace core

} // namespace sup

#endif // _Open62541PlantSystemAdapter_h_


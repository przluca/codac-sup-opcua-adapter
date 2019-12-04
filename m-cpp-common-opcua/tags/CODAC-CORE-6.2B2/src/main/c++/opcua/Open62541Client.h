/******************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common-opcua/tags/CODAC-CORE-6.2B2/src/main/c++/opcua/Open62541Client.h $
 * $Id: Open62541Client.h 101452 2019-08-08 10:38:23Z bauvirb $
 *
 * Project       : CODAC Core System
 *
 * Description   : Infrastructure tools - Prototype
 *
 * Author        : Bertrand Bauvir
 *
 * Copyright (c) : 2010-2019 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

/**
 * @file Open62541Client.h
 * @brief Header file for Open62541Client class.
 * @date 11/11/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the Open62541Client class.
 */

#ifndef _Open62541Client_h_
#define _Open62541Client_h_

// Global header files

#include <functional> // std::function

#include <BasicTypes.h> // Global type definition

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

class Open62541ClientImpl;
// Forward class declaration

/**
 * @brief Interface class providing support for CA client with variable cache.
 * @detail The class provides access to a variable cache and asynchronous CA
 * update to ensure non-blocking calls on the application side. The variable
 * cache is implemented by means of ccs::types::AnyValue instances which allow
 * assignment to/from C-like structures.
 *
 * The class also offers a callback mechanism for input or bi-directional
 * variables to allow for application-specific synchronous handling of OPC UA
 * monitor events.
 *
 * @note The design is based on a bridge pattern to avoid exposing OPC UA specific
 * internals through the interface class.
 *
 * @todo Get rid of the singleton pattern or provide support for multiplvarInfo.referencee servers.
 */

class Open62541Client {

private:

    /**
     * @brief Attribute.
     * @detail Bridge (PIMPL) pattern.
     */

    Open62541ClientImpl *__impl;

protected:

public:

    Open62541Client(const ccs::types::char8 *const service);

    virtual ~Open62541Client(void);

    bool AddVariable(const ccs::types::char8 *const name,
                     ccs::types::DirIdentifier direction,
                     const std::shared_ptr<const ccs::types::AnyType> &type,
                     ccs::types::uint32 index);

    bool AddMethod(const ccs::types::char8 *const methodId,
                   const ccs::types::uint32 index);

    bool SetExtensionObject(const ccs::types::char8 *const extObj,
                            const ccs::types::uint32 index);

    bool SetEONodeId(const std::string chan);

    bool SetNumberOfNodes(const ccs::types::uint32 dim);

    bool Launch(void); // Should be called after the variable table is populated

    bool IsValid(const ccs::types::char8 *const name) const;

    ccs::types::AnyValue* GetVariable(const ccs::types::char8 *const name) const;

    template<typename Type> bool GetVariable(const ccs::types::char8 *const name,
                                             Type &value) const;
    template<typename Type> bool SetVariable(const ccs::types::char8 *const name,
                                             Type &value);

    bool UpdateVariable(const ccs::types::char8 *const name);

    bool SetBodyLength(const ccs::types::uint32 length);

    /**
     * @brief Accessor. SetCallback method.
     * @detail The method installs an application callback to be called synchronously when
     * an input or bi-directional variable has been externally updated. The application can
     * use other methods to access the variable cache asynchronously.
     * @param name Variable identifier.
     * @param cb Callback method.
     * @return True if successful.
     *
     * @code
     // Synchronous callback on record change
     void HandleUpdate (const ccs::types::char8 * const name, const ccs::types::AnyValue& value)
     {
     // Handle update ..
     log_info("OPC UA record '%s' updated", name);
     return;
     }

     bool status = ccs::base::OPCUAInterface::GetInstance<ccs::base::Open62541Client>()
     ->SetCallback("MyVariable", HandleUpdate);
     @endcode
     *
     * @todo Mechanism for callback to identify the corresponding variable
     */

    bool SetCallback(const ccs::types::char8 *const name,
                     const std::function<void(const ccs::types::char8* const,
                                              const ccs::types::AnyValue&)> &cb);

};

// Global variables

// Function declaration

// Function definition

template<typename Type> bool Open62541Client::GetVariable(const ccs::types::char8 *const name,
                                                          Type &value) const {

    const ccs::types::AnyValue *var = static_cast<ccs::types::AnyValue*>(NULL);

    bool status = this->IsValid(name);

    if (status) {
        var = this->GetVariable(name);
        value = static_cast<Type>(*var);
    }

    return status;

}

template<typename Type> bool Open62541Client::SetVariable(const ccs::types::char8 *const name,
                                                          Type &value) {

    ccs::types::AnyValue *var = static_cast<ccs::types::AnyValue*>(NULL);

    bool status = this->IsValid(name);

    if (status) {
        var = this->GetVariable(name);
        *var = value;
        this->UpdateVariable(name);
    }

    return status;

}

} // namespace base

} // namespace ccs

#endif // _Open62541Client_h_

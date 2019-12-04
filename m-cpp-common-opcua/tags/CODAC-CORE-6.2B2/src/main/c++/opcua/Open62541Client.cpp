/******************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common-opcua/tags/CODAC-CORE-6.2B2/src/main/c++/opcua/Open62541Client.cpp $
 * $Id: Open62541Client.cpp 101452 2019-08-08 10:38:23Z bauvirb $
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

/* ToDo
 - Manage default value from configuration
 */

// Global header files
#include <functional> // std::function
#include <new> // std::nothrow
#include <string>
#include <cstdlib>
#include <vector>

#include <open62541.h> //open62541 SDK

#include <tools.h> // Misc. helper functions, e.g. hash, etc.
#include <types.h> // Global type definition

//#define LOG_TRACE_ENABLE
//#undef LOG_TRACE_ENABLE
//#define LOG_DEBUG_ENABLE
//#undef LOG_DEBUG_ENABLE
#include <log-api.h> // Syslog wrapper routines

#include <lu-table.h> // Look-up table class definition, etc.

#include <AnyObject.h> // Abstract base class definition ..
#include <ObjectDatabase.h> // .. associated object database

#include <any-thread.h> // Thread management class

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

#include "AnyTypeToUA.h" // .. associated helper routines

#include "Open62541Client.h" // This class definition

// Constants

#define DEFAULT_OPCUAINTERFACE_THREAD_PERIOD 10000000ul // 100Hz
#define DEFAULT_OPCUAINTERFACE_INSTANCE_NAME "ua-interface"

#define MAXIMUM_VARIABLE_NUM 50000

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ua-if"

#ifndef log_trace
#define log_trace(arg_msg...) {}
#endif

// Type definition

namespace ccs {

namespace base {

class Open62541ClientImpl: public AnyObject {

private:

    uint64_t m_sleep;

    ccs::base::SynchronisedThreadWithCallback *m_thread;

    ccs::types::string __service;
    bool __connected = false;

    // Initialiser methods
    bool Initialise(void);
    bool Connect(void);
    bool Disconnect(void);

protected:

public:

    ccs::types::uint32 *entryTypes;
    ccs::types::uint32 *entryArrayElements;
    ccs::types::uint32 *entryNumberOfMembers;
    ccs::types::uint32 entryArraySize;

    ccs::types::uint8 *tempDataPtr; //allocate dataPtr
    void *dataPtr;

    UA_MonitoredItemCreateRequest *items;
    UA_Client_DataChangeNotificationCallback *callbacks;
    UA_Client_DeleteMonitoredItemCallback *deleteCallbacks;
    void **contexts;

    ccs::types::uint32 bodyLength;

    UA_Client *client;

    ccs::types::CompoundType *m_type; // Introspectable type definition for the variable cache ..
    ccs::types::AnyValue *m_value; // Introspectable variable mapped to the whole cache ..

    bool m_initialized;

    typedef struct VariableInfo {

        bool update;
        ccs::types::DirIdentifier direction;

        std::function<void(const ccs::types::char8* const,
                           const ccs::types::AnyValue&)> cb;

        ccs::types::string name;

#if 0
		chid channel;
		chtype type; // OPCUA native type
#endif
        ccs::types::uint32 mult;

        void *reference; // Reference in the cache

        std::shared_ptr<const ccs::types::AnyType> __type; // Introspectable type definition for the variable cache ..
        ccs::types::AnyValue *value;

    } VariableInfo_t;

    LUTable<VariableInfo_t> *m_var_table;

    const ccs::types::char8 **methodId;
    const ccs::types::char8 **extObj;

    ccs::types::char8 *eoNodeId;

    // Initializer methods
    bool AddVariable(const ccs::types::char8 *const name,
                     ccs::types::DirIdentifier direction,
                     const std::shared_ptr<const ccs::types::AnyType> &type,
                     ccs::types::uint32 index);

    bool AddMethod(const ccs::types::char8 *const methodId,
                   const ccs::types::uint32 index);

    bool SetExtensionObject(const ccs::types::char8 *const extObj,
                            const ccs::types::uint32 index);

    bool GetExtensionObjectByteString(const ccs::types::uint32 *const&entryTypes,
                                      const ccs::types::uint32 *const&entryArrayElements,
                                      const ccs::types::uint32 *const&entryNumberOfMembers,
                                      const ccs::types::uint32 entryArraySize,
                                      ccs::types::uint32 &nodeCounter,
                                      ccs::types::uint32 &index);

    bool SetNumberOfNodes(const ccs::types::uint32 dim);

    bool SetEONodeId(const std::string chan);

    void SeekDataPtr(const ccs::types::uint32 bodyLength);

    bool SetBodyLength(const ccs::types::uint32 length);

    bool Launch(void); // Should be called after the variable table is populated

    // Accessor methods
    bool IsValid(ccs::types::uint32 id) const;
    bool IsValid(const ccs::types::char8 *const name) const;

    bool IsConnected(void) const;

    ccs::types::uint32 GetVariableId(const ccs::types::char8 *const name) const;

    ccs::types::AnyValue* GetVariable(ccs::types::uint32 id) const;
    ccs::types::AnyValue* GetVariable(const ccs::types::char8 *const name) const;

    bool UpdateVariable(ccs::types::uint32 id);
    bool UpdateVariable(const ccs::types::char8 *const name);

    // Miscellaneous methods
    bool SetCallback(ccs::types::uint32 id,
                     const std::function<void(const ccs::types::char8* const,
                                              const ccs::types::AnyValue&)> &cb);
    bool SetCallback(const ccs::types::char8 *const name,
                     const std::function<void(const ccs::types::char8* const,
                                              const ccs::types::AnyValue&)> &cb);

    bool SetService(const ccs::types::char8 *const service);

    // Constructor methods
    Open62541ClientImpl(void);

    // Destructor method
    virtual ~Open62541ClientImpl(void);

};

// Global variables
Open62541ClientImpl *objPtr;

} // namespace base

} // namespace ccs

// Function declaration
//stateChange Callback
void stateCallback(UA_Client *client,
                   UA_ClientState clientState) {
    switch (clientState) {
    case UA_CLIENTSTATE_DISCONNECTED:
        log_info("Open62541Client -- The client is disconnected.");
        break;
    case UA_CLIENTSTATE_WAITING_FOR_ACK:
        log_info("Open62541Client -- Waiting for ack.");
        break;
    case UA_CLIENTSTATE_CONNECTED:
        log_info("Open62541Client -- Client Connected. A TCP Connection is open.");
        break;
    case UA_CLIENTSTATE_SECURECHANNEL:
        log_info("Open62541Client -- A Secure Channel with the server is open.");
        break;
    case UA_CLIENTSTATE_SESSION: {
        log_info("Open62541Client -- A Session with the self->server is active.");
    }
        break;
    case UA_CLIENTSTATE_SESSION_RENEWED:
        log_info("Open62541Client -- A Session with the server is active (renewed).");
        /* The session was renewed. We don't need to recreate the subscription. */
        break;
    case UA_CLIENTSTATE_SESSION_DISCONNECTED:
        log_info("Open62541Client -- Session disconnected.");
        break;
    }
    return;
}

//dataChange Callback
void dataChange(UA_Client *client,
                UA_UInt32 subId,
                void *subContext,
                UA_UInt32 monId,
                void *monContext,
                UA_DataValue *value) {

    log_info("Open62541ClientImpl::dataChange - Notification received for item '%u' (handle) ..", monId - 1);
    bool ok = true;
    if (value->value.type->typeId.identifier.numeric == 22u) {
        UA_ExtensionObject *valuePtr = reinterpret_cast<UA_ExtensionObject*>(value->value.data);
        UA_ExtensionObject *eos;
        if (value->value.arrayLength > 1u) {
            eos = new UA_ExtensionObject[value->value.arrayLength];
            for (ccs::types::uint32 j = 0u; j < value->value.arrayLength; j++) {
                if (ok) {
                    ok = (UA_ExtensionObject_copy(&valuePtr[j], &eos[j]) == 0x00U);
                }
            }
            if (ok) {
                for (ccs::types::uint32 i = 0u; i < value->value.arrayLength; i++) {
                    if (ok) {
                        memcpy(ccs::base::objPtr->dataPtr, eos[i].content.encoded.body.data,
                               static_cast<ccs::types::uint32>(eos[i].content.encoded.body.length));
                        ccs::base::objPtr->dataPtr = &reinterpret_cast<ccs::types::uint8*>(ccs::base::objPtr->dataPtr)[eos[i].content.encoded.body.length];
                    }
                }
                log_info("Value copied");
            }
            ccs::types::uint32 actualBodyLength = (static_cast<ccs::types::uint32>(value->value.arrayLength)
                    * static_cast<ccs::types::uint32>(eos[0u].content.encoded.body.length));
            ccs::base::objPtr->SeekDataPtr(actualBodyLength);
            delete[] eos;
        }
        else {
            eos = UA_ExtensionObject_new();
            (void) UA_ExtensionObject_copy(valuePtr, eos);
            ok = memcpy(ccs::base::objPtr->dataPtr, eos->content.encoded.body.data, static_cast<ccs::types::uint32>(eos->content.encoded.body.length));
            if (ok) {
                log_info("Value copied");
                (void) UA_ExtensionObject_clear(eos);
                UA_ExtensionObject_delete(eos);
            }
        }
    }


    if (strcmp(ccs::base::objPtr->extObj[monId - 1], "NULL") == 0) {

        // Update cache
        ccs::base::Open62541ClientImpl::VariableInfo_t varInfo;

        if ((ccs::base::objPtr->m_var_table)->GetValue(varInfo, monId - 1) != STATUS_SUCCESS) {
            log_warning("Open62541ClientImpl::dataChange - LUTable<>::GetValue failed");
        }
    }

    return;

}
// Function definition

void OPCUAInterface_Thread_PRBL(ccs::base::Open62541ClientImpl *self) {

    log_info("Entering '%s' routine", __FUNCTION__);

    bool status = true;

    self->dataPtr = malloc(self->bodyLength);
    self->tempDataPtr = reinterpret_cast<ccs::types::uint8*>(self->dataPtr);

    // Create variable cache
    self->m_value = new (std::nothrow) ccs::types::AnyValue(self->m_type);

    // Create subscription
    UA_CreateSubscriptionRequest subRequest = UA_CreateSubscriptionRequest_default();
    UA_CreateSubscriptionResponse subResponse = UA_Client_Subscriptions_create(self->client, subRequest, NULL, NULL, NULL);
    if (subResponse.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        log_info("'%s' - Create subscription succeeded, id %u", __FUNCTION__, subResponse.subscriptionId);
    }
    else {
        log_error("'%s' - Create subscription failed.", __FUNCTION__);
        return;
    }

    // GETTING EXTENSION OBJECT
    ccs::types::uint32 nodeCounter = 0u;
    ccs::types::uint32 index;
    ccs::types::uint32 arraySize = 20u;
    for (ccs::types::uint32 j = 0u; j < arraySize; j++) { //SCU arraySize
        index = 0u;
        ccs::types::uint32 numberOfNodesForEachIteration = ((self->m_var_table)->GetSize() / arraySize) * (j + 1u);
        while (nodeCounter < numberOfNodesForEachIteration) {
            if (status) {
                status = self->GetExtensionObjectByteString(self->entryTypes, self->entryArrayElements, self->entryNumberOfMembers, self->entryArraySize,
                                                            nodeCounter, index);
            }
        }
    }
    // Setting entryArrayTypes and others.
    // END GETTING EXTENSION OBJECT

    ccs::types::string name;

    for (ccs::types::uint32 index = 0; index < (self->m_var_table)->GetSize(); index += 1u) {

        if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS) {
            log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
            continue;
        }

        ccs::base::Open62541ClientImpl::VariableInfo_t varInfo;

        if ((self->m_var_table)->GetValue(varInfo, index) != STATUS_SUCCESS) {
            log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
            continue;
        }

        // Store cache reference

        //varInfo.reference = ccs::HelperTools::GetAttributeReference(self->m_value, name);

        varInfo.value = new (std::nothrow) ccs::types::AnyValue(varInfo.__type, varInfo.reference);

        (self->m_var_table)->SetValue(varInfo, index); //Store for future use

    }

    strcpy(name, self->eoNodeId);

    ccs::types::uint32 ns;

    std::string nsName = name;
    std::string delimiter = ";";
    std::string s = nsName.substr(0, nsName.find(delimiter));
    ns = static_cast<ccs::types::uint32>(std::stoul(s.erase(0, 3), nullptr, 0));

    UA_MonitoredItemCreateRequest item;

    std::string idName = name;
    if (idName.find("i=") != std::string::npos) {
        ccs::types::uint32 identifier;
        identifier = static_cast<ccs::types::uint32>(std::stoul(idName.erase(0, 7), nullptr, 0));

        item = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(ns, identifier));
    }
    else if (idName.find("s=") != std::string::npos) {
        std::string identifier;
        identifier = idName.erase(0, 7);
        size_t pos;
        pos = identifier.find("\'");
        while (pos != std::string::npos) {
            identifier.replace(pos, 1, "\"");
            pos = identifier.find("\'");
        }
        item = UA_MonitoredItemCreateRequest_default(UA_NODEID_STRING(ns, const_cast<char*>(identifier.c_str())));
    }

    UA_MonitoredItemCreateResult res = UA_Client_MonitoredItems_createDataChange(self->client, subResponse.subscriptionId, UA_TIMESTAMPSTORETURN_BOTH, item,
                                                                                 NULL, dataChange, NULL);

    if (res.statusCode == UA_STATUSCODE_GOOD) {
        log_info("'%s' - Monitored Item createDataChange request succeeded!", __FUNCTION__);
    }
    else {
        log_warning("%s - Monitored Item registration failed!", __FUNCTION__);
    }

    self->m_initialized = true;

    UA_Client_run_iterate(self->client, 1000);

    log_trace("Leaving '%s' routine", __FUNCTION__);

    return;

}

void OPCUAInterface_Thread_CB(ccs::base::Open62541ClientImpl *self) {

    log_trace("Entering '%s' routine", __FUNCTION__);

    bool ok = self->m_initialized;

    for (ccs::types::uint32 index = 0u; (ok && (index < (self->m_var_table)->GetSize())); index += 1u) {

        ccs::types::string name;

        if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS) {
            log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
            continue;
        }

        ccs::base::Open62541ClientImpl::VariableInfo_t varInfo;

        if ((self->m_var_table)->GetValue(varInfo, index) != STATUS_SUCCESS) {
            log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
            continue;
        }

        if ((varInfo.direction == ccs::types::InputVariable) || (varInfo.update != true)) {
            continue; // Nothing to do for this channel
        }
        //else {
//            memcpy(varInfo.reference, ccs::HelperTools::GetAttributeReference(self->m_value, name), varInfo.__type->GetSize());
//            log_info("VAL: %d", *reinterpret_cast<ccs::types::uint8*>(varInfo.reference));
//            (self->m_var_table)->SetValue(varInfo, index);
        //}

    }

    ccs::base::Open62541ClientImpl::VariableInfo_t var;
    if ((self->m_var_table)->GetValue(var, 1) != STATUS_SUCCESS) {
        log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
    }
   if (var.update == true) {

        ccs::types::string name;
        //Parsing method nodeId
        std::string delimiter = ";";
        UA_ReadValueId *readValues = UA_ReadValueId_new();
        readValues[0u].attributeId = 13u; /* UA_ATTRIBUTEID_VALUE */

//    if (strcmp(self->methodId[index], "NULL") != 0) {
//        ccs::types::uint32 mns;
//        std::string mnsName = self->methodId[index];
//        std::string ms = mnsName.substr(0, mnsName.find(delimiter));
//        mns = static_cast<ccs::types::uint32>(std::stoul(ms.erase(0, 3), nullptr, 0));
//
//        std::string midName = self->methodId[index];
//        if (midName.find("i=") != std::string::npos) {
//            ccs::types::uint32 identifier;
//            identifier = static_cast<ccs::types::uint32>(std::stoul(midName.erase(0, 7), nullptr, 0));
//        }
//        else if (midName.find("s=") != std::string::npos) {
//            std::string identifier;
//            identifier = midName.erase(0, 7);
//            size_t pos;
//            pos = identifier.find("\'");
//            while (pos != std::string::npos) {
//                identifier.replace(pos, 1, "\"");
//                pos = identifier.find("\'");
//            }
//        }
//    }

        strcpy(name, self->eoNodeId);

        ccs::types::uint32 ns;

        std::string nsName = name;
        std::string s = nsName.substr(0, nsName.find(delimiter));
        ns = static_cast<ccs::types::uint32>(std::stoul(s.erase(0, 3), nullptr, 0));

        std::string idName = name;
        if (idName.find("i=") != std::string::npos) {
            ccs::types::uint32 identifier;
            identifier = static_cast<ccs::types::uint32>(std::stoul(idName.erase(0, 7), nullptr, 0));

            readValues[0u].nodeId = UA_NODEID_NUMERIC(ns, identifier);
        }
        else if (idName.find("s=") != std::string::npos) {
            std::string identifier;
            identifier = idName.erase(0, 7);
            size_t pos;
            pos = identifier.find("\'");
            while (pos != std::string::npos) {
                identifier.replace(pos, 1, "\"");
                pos = identifier.find("\'");
            }
//        readValues[0u].nodeId = UA_NODEID_STRING(ns, const_cast<char*>(identifier.c_str()));
            readValues[0u].nodeId = UA_NODEID_STRING(ns, const_cast<char*>("\"SCUs_Config\".\"SCU_Config\""));
        }

        //READ EXTENSION OBJECT
        UA_ExtensionObject *eos;
        ccs::types::uint32 nOfEos;
        UA_Variant *tempVariant = reinterpret_cast<UA_Variant*>(UA_Array_new(1u, &UA_TYPES[UA_TYPES_VARIANT]));
        UA_ReadRequest readRequest;
        UA_ReadRequest_init(&readRequest);
        readRequest.nodesToRead = readValues;
        readRequest.nodesToReadSize = 1u;
        UA_ReadResponse readResponse;
        readResponse = UA_Client_Service_read(self->client, readRequest);
        UA_ExtensionObject *valuePtr;
        ok = (readResponse.responseHeader.serviceResult == 0x00U);
        if (ok) {
            /* Setting EO Memory */
            if (readResponse.results[0].value.arrayLength > 1u) {
                nOfEos = static_cast<ccs::types::uint32>(readResponse.results[0].value.arrayLength);
                eos = reinterpret_cast<UA_ExtensionObject*>(UA_Array_new(nOfEos, &UA_TYPES[UA_TYPES_EXTENSIONOBJECT]));
                UA_Variant_setArray(&tempVariant[0u], eos, readResponse.results[0].value.arrayLength, &UA_TYPES[UA_TYPES_EXTENSIONOBJECT]);
            }
            else {
                nOfEos = 1u;
                eos = UA_ExtensionObject_new();
                UA_ExtensionObject_init(eos);
                /*lint -e{1055} function defined in open62541*/
                (void) UA_Variant_setScalar(&tempVariant[0u], eos, &UA_TYPES[UA_TYPES_EXTENSIONOBJECT]);
            }
        }

        UA_StatusCode retval;
        retval = readResponse.responseHeader.serviceResult;
        if (retval == 0x00U) {
            valuePtr = reinterpret_cast<UA_ExtensionObject*>(readResponse.results[0].value.data);
            ccs::types::uint32 actualBodyLength;
            if (nOfEos > 1u) {
                for (ccs::types::uint32 j = 0u; j < nOfEos; j++) {
                    if (ok) {
                        (void) UA_ExtensionObject_copy(&valuePtr[j], &eos[j]);
                        ok = memcpy(eos[j].content.encoded.body.data, self->dataPtr, static_cast<ccs::types::uint32>(eos[j].content.encoded.body.length));
                        self->dataPtr = &reinterpret_cast<ccs::types::uint8*>(self->dataPtr)[eos[j].content.encoded.body.length];
                    }
                }
                actualBodyLength = (nOfEos * static_cast<ccs::types::uint32>(eos[0u].content.encoded.body.length));
                self->SeekDataPtr(actualBodyLength);
            }
            else {
                (void) UA_ExtensionObject_copy(valuePtr, eos);
                ok = memcpy(eos->content.encoded.body.data, self->dataPtr, static_cast<ccs::types::uint32>(eos->content.encoded.body.length));
            }
            UA_Variant output;
            UA_Variant *output2 = &output;
            UA_Variant_init(&output);
            size_t outSize[1] = { 0u };
            size_t inSize = 1u;
            if (ok) {
                retval = UA_Client_call(self->client, UA_NODEID_STRING(3, const_cast<char*>("\"OPC_UA_Method_DB\"")),
                                        UA_NODEID_STRING(3, const_cast<char*>("\"OPC_UA_Method_DB\".Method")), inSize, tempVariant, &outSize[0], &output2);
                log_info("METHOD CALLED!");
            }
        }
        ok = (retval == 0x00U); /* UA_STATUSCODE_GOOD */

    }

    for (ccs::types::uint32 index = 0u; (ok && (index < (self->m_var_table)->GetSize())); index += 1u) {

        ccs::types::string name;

        if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS) {
            log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
            continue;
        }

        ccs::base::Open62541ClientImpl::VariableInfo_t varInfo;

        if ((self->m_var_table)->GetValue(varInfo, index) != STATUS_SUCCESS) {
            log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
            continue;
        }

        varInfo.update = false;
        (self->m_var_table)->SetValue(varInfo, index);
    }

    UA_Client_run_iterate(self->client, 1000);

    log_trace("Leaving '%s' routine", __FUNCTION__);

    return;

}

void OPCUAInterface_Thread_POST(ccs::base::Open62541ClientImpl *self) {

    (void) self;

    return;

}

namespace ccs {

namespace base {

// Initializer methods

bool Open62541Client::AddVariable(const ccs::types::char8 *const name,
                                  ccs::types::DirIdentifier direction,
                                  const std::shared_ptr<const ccs::types::AnyType> &type,
                                  ccs::types::uint32 index) {

    return __impl->AddVariable(name, direction, type, index);
}

bool Open62541ClientImpl::AddVariable(const ccs::types::char8 *const name,
                                      ccs::types::DirIdentifier direction,
                                      const std::shared_ptr<const ccs::types::AnyType> &type,
                                      ccs::types::uint32 index) {

    bool status = true;
    VariableInfo_t varInfo;

    varInfo.cb = NULL;
    varInfo.update = false;
    varInfo.direction = direction;
    ccs::HelperTools::SafeStringCopy(varInfo.name, name, sizeof(ccs::types::string));

    status = (ccs::HelperTools::Is < ccs::types::ScalarType > (type)
            || (ccs::HelperTools::Is < ccs::types::ArrayType > (type)
                    && ccs::HelperTools::Is < ccs::types::ScalarType > (std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType())));

    if (status) {
        varInfo.mult = (
                ccs::HelperTools::Is < ccs::types::ScalarType > (type) ? 1 : std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementNumber());
        varInfo.__type = type;
    }

    if (status) {
        status = ((static_cast<ccs::types::CompoundType*>(NULL) != this->m_type) && (static_cast<LUTable<VariableInfo_t>*>(NULL) != this->m_var_table));
    }

    if (status) {
        status = (this->m_type)->AddAttribute(name, varInfo.__type); // Type definition for the variable cache

    }

    if (status) {
        status = (this->m_var_table)->AddPair(name, varInfo, index);
    }

    return status;

}

bool Open62541Client::SetNumberOfNodes(const ccs::types::uint32 dim) {

    return __impl->SetNumberOfNodes(dim);

}

bool Open62541ClientImpl::SetNumberOfNodes(const ccs::types::uint32 dim) {

    methodId = new const ccs::types::char8*[dim];
    extObj = new const ccs::types::char8*[dim];
    return true;

}

bool Open62541Client::SetExtensionObject(const ccs::types::char8 *const extObj,
                                         const ccs::types::uint32 index) {
    return __impl->SetExtensionObject(extObj, index);
}

bool Open62541ClientImpl::SetExtensionObject(const ccs::types::char8 *const extObj,
                                             const ccs::types::uint32 index) {

    this->extObj[index] = extObj;

    return true;
}

bool Open62541Client::SetEONodeId(const std::string chan) {
    return __impl->SetEONodeId(chan);
}

bool Open62541ClientImpl::SetEONodeId(const std::string chan) {
    eoNodeId = new ccs::types::char8[chan.length() + 1];
    chan.copy(eoNodeId, chan.length(), 0);
    return true;
}

bool Open62541Client::AddMethod(const ccs::types::char8 *const methodId,
                                const ccs::types::uint32 index) {

    return __impl->AddMethod(methodId, index);
}

bool Open62541ClientImpl::AddMethod(const ccs::types::char8 *const methodId,
                                    const ccs::types::uint32 index) {

    this->methodId[index] = methodId;
    return true;
}

bool Open62541ClientImpl::Initialise(void) {

    // Initialize resources
    this->m_sleep = DEFAULT_OPCUAINTERFACE_THREAD_PERIOD;
    this->m_var_table = new LUTable<VariableInfo_t>(MAXIMUM_VARIABLE_NUM); // The table will be filled with application-specific variable list
    this->m_initialized = false;

    this->m_type = new (std::nothrow) ccs::types::CompoundType("uaif::VariableCache_t");
    this->m_value = static_cast<ccs::types::AnyValue*>(NULL);

    log_info("Open62541ClientImpl::Initialise - Creating OPCUA Client and Initialising configuration");

    //open62541 Client configuration
    client = UA_Client_new();
    UA_ClientConfig *cc = UA_Client_getConfig(client);
    UA_ClientConfig_setDefault(cc);

    cc->stateCallback = stateCallback;

    this->m_thread = new ccs::base::SynchronisedThreadWithCallback("OPC UA Interface");
    (this->m_thread)->SetPeriod(this->m_sleep);
    (this->m_thread)->SetAccuracy(this->m_sleep);
    (this->m_thread)->SetPreamble((void (*)(void*)) &OPCUAInterface_Thread_PRBL, (void*) this);
    (this->m_thread)->SetCallback((void (*)(void*)) &OPCUAInterface_Thread_CB, (void*) this);
    (this->m_thread)->SetPostface((void (*)(void*)) &OPCUAInterface_Thread_POST, (void*) this);

    objPtr = this;

    return true;

}

bool Open62541ClientImpl::Connect(void) {

    // Connect, if necessary
    log_info("Open62541ClientImpl::Connect - Connecting to '%s' ..", __service);

    UA_StatusCode retval = UA_Client_connect(client, __service);
    if (retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int) retval;
    }

    bool status = this->IsConnected();

    return status;

}

bool Open62541ClientImpl::IsConnected(void) const {

    UA_Variant value;
    UA_Variant_init(&value);
    UA_Client_readValueAttribute(client, UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME), &value);
    if (UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_DATETIME])) {
        UA_DateTimeStruct dts = UA_DateTime_toStruct(*(UA_DateTime*) value.data);
        log_info("Open62541ClientImpl::IsConnected Successfull with server time: %02u-%02u-%04u %02u:%02u:%02u.%03u", dts.day, dts.month, dts.year, dts.hour,
                 dts.min, dts.sec, dts.milliSec);
    }
    else {
        log_info("Open62541ClientImpl::IsConnected -  Failed.");
        UA_Variant_clear(&value);
        return false;
    }
    UA_Variant_clear(&value);
    return true;

}

bool Open62541ClientImpl::Disconnect(void) {

    UA_Client_disconnect(client);
    UA_Client_delete(client);

    return true;

}

bool Open62541Client::Launch(void) {
    return __impl->Launch();
}
bool Open62541ClientImpl::Launch(void) {
    return ((this->m_thread)->Launch() == STATUS_SUCCESS);
} // Should be called after the variable table is populated

// Accessor methods

bool Open62541Client::IsValid(const ccs::types::char8 *const name) const {
    return __impl->IsValid(name);
}

bool Open62541ClientImpl::IsValid(uint_t id) const {
    return ((this->m_var_table)->IsValid(id) == STATUS_SUCCESS);
}
bool Open62541ClientImpl::IsValid(const ccs::types::char8 *const name) const {
    return ((this->m_var_table)->IsValid(name) == STATUS_SUCCESS);
}

uint_t Open62541ClientImpl::GetVariableId(const ccs::types::char8 *const name) const {
    uint_t id = 0;
    (this->m_var_table)->GetIndex(name, id);
    return id;
}

ccs::types::AnyValue* Open62541Client::GetVariable(const ccs::types::char8 *const name) const {
    return __impl->GetVariable(name);
}

ccs::types::AnyValue* Open62541ClientImpl::GetVariable(uint_t id) const {
    VariableInfo_t varInfo;
    varInfo.value = static_cast<ccs::types::AnyValue*>(NULL);
    if (this->IsValid(id))
        if ((this->m_var_table)->GetValue(varInfo, id))
            return varInfo.value;
}
ccs::types::AnyValue* Open62541ClientImpl::GetVariable(const ccs::types::char8 *const name) const {
    return this->GetVariable(this->GetVariableId(name));
}

bool Open62541Client::UpdateVariable(const ccs::types::char8 *const name) {
    return __impl->UpdateVariable(name);
}

bool Open62541ClientImpl::UpdateVariable(uint_t id) {
    bool status = this->IsValid(id);
    if (status) {
        VariableInfo_t varInfo;
        (this->m_var_table)->GetValue(varInfo, id);
        varInfo.update = true;
        (this->m_var_table)->SetValue(varInfo, id);
    }
    return status;
}

bool Open62541ClientImpl::UpdateVariable(const ccs::types::char8 *const name) {
    return this->UpdateVariable(this->GetVariableId(name));
}

bool Open62541Client::SetCallback(const ccs::types::char8 *const name,
                                  const std::function<void(const ccs::types::char8* const,
                                                           const ccs::types::AnyValue&)> &cb) {
    return __impl->SetCallback(name, cb);
}

bool Open62541ClientImpl::SetCallback(ccs::types::uint32 id,
                                      const std::function<void(const ccs::types::char8* const,
                                                               const ccs::types::AnyValue&)> &cb) {
    VariableInfo_t varInfo;
    bool status = this->IsValid(id);
    if (status) {
        (this->m_var_table)->GetValue(varInfo, id);
        varInfo.cb = cb;
        (this->m_var_table)->SetValue(varInfo, id);
    }
    return status;
}
bool Open62541ClientImpl::SetCallback(const ccs::types::char8 *const name,
                                      const std::function<void(const ccs::types::char8* const,
                                                               const ccs::types::AnyValue&)> &cb) {
    return this->SetCallback(this->GetVariableId(name), cb);
}

bool Open62541ClientImpl::SetService(const ccs::types::char8 *const service) {

    (void) ccs::HelperTools::SafeStringCopy(__service, service, STRING_MAX_LENGTH);

    return this->Connect();

}

bool Open62541ClientImpl::GetExtensionObjectByteString(const ccs::types::uint32 *const&entryTypes,
                                                       const ccs::types::uint32 *const&entryArrayElements,
                                                       const ccs::types::uint32 *const&entryNumberOfMembers,
                                                       const ccs::types::uint32 entryArraySize,
                                                       ccs::types::uint32 &nodeCounter,
                                                       ccs::types::uint32 &index) {
    bool ok = true;
    if (!entryTypes[index]) {
        if (entryArrayElements[index] == 1u) {
            ccs::types::uint32 nOfBytes = sizeof(ccs::types::uint8);

            ccs::types::string name;

            if (m_var_table->GetKeyword(name, nodeCounter) != STATUS_SUCCESS) {
                log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
            }

            ccs::base::Open62541ClientImpl::VariableInfo_t varInfo;

            if (m_var_table->GetValue(varInfo, nodeCounter) != STATUS_SUCCESS) {
                log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
            }

            varInfo.reference = reinterpret_cast<void*>(tempDataPtr);
            m_var_table->SetValue(varInfo, nodeCounter);
            tempDataPtr = &(tempDataPtr[nOfBytes]);
        }
        else {
            ccs::types::uint32 nOfBytesUint32 = sizeof(ccs::types::uint32);
            ok = memcpy(tempDataPtr, &entryArrayElements[index], nOfBytesUint32);
            if (ok) {
                tempDataPtr = &(tempDataPtr[nOfBytesUint32]); /* Skip 4 bytes */
                ccs::types::uint32 nOfBytes = sizeof(ccs::types::uint8);
                nOfBytes *= entryArrayElements[index];

                ccs::types::string name;

                if (m_var_table->GetKeyword(name, nodeCounter) != STATUS_SUCCESS) {
                    log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
                }

                ccs::base::Open62541ClientImpl::VariableInfo_t varInfo;

                if (m_var_table->GetValue(varInfo, nodeCounter) != STATUS_SUCCESS) {
                    log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
                }
                varInfo.reference = reinterpret_cast<void*>(tempDataPtr);
                m_var_table->SetValue(varInfo, nodeCounter);

                tempDataPtr = &(tempDataPtr[nOfBytes]);
            }
        }
        if (ok) {
            nodeCounter++;
            index++;
        }

    }
    else { /* if isStructuredData */
        ccs::types::uint32 newIndex;
        if (entryArrayElements[index] == 1u) {
            index++;
            for (ccs::types::uint32 j = 0u; j < entryNumberOfMembers[index - 1u]; j++) {
                ok = GetExtensionObjectByteString(entryTypes, entryArrayElements, entryNumberOfMembers, entryArraySize, nodeCounter, index);
            }
        }
        if (entryArrayElements[index] > 1u) {
            ccs::types::uint32 nOfBytesUint32 = sizeof(ccs::types::uint32);
            ok = memcpy(tempDataPtr, &entryArrayElements[index], nOfBytesUint32);
            if (ok) {
                tempDataPtr = &(tempDataPtr[nOfBytesUint32]);
                ccs::types::uint32 nMembers = entryNumberOfMembers[index];
                for (ccs::types::uint32 i = 0u; i < entryArrayElements[index]; i++) {
                    newIndex = index + 1u;
                    for (ccs::types::uint32 j = 0u; j < entryNumberOfMembers[index]; j++) {
                        ok = GetExtensionObjectByteString(entryTypes, entryArrayElements, entryNumberOfMembers, entryArraySize, nodeCounter, newIndex);
                    }
                }
                index += (nMembers + 1u);
            }
        }
    }
    return ok;
}

void Open62541ClientImpl::SeekDataPtr(const ccs::types::uint32 bodyLength) {
    dataPtr = reinterpret_cast<ccs::types::uint8*>(dataPtr) - bodyLength;
}

bool Open62541Client::SetBodyLength(const ccs::types::uint32 length) {
    return __impl->SetBodyLength(length);
}

bool Open62541ClientImpl::SetBodyLength(const ccs::types::uint32 length) {
    bodyLength = length;
}

// Constructor methods

Open62541Client::Open62541Client(const ccs::types::char8 *const service) {

    // Instantiate implementation class
    __impl = new (std::nothrow) Open62541ClientImpl;

    bool status = (static_cast<Open62541ClientImpl*>(NULL) != __impl);

    if (status) {
        (void) __impl->SetService(service);
    }

    return;

}

Open62541ClientImpl::Open62541ClientImpl(void) {

    entryArraySize = 8u;
    entryTypes = new ccs::types::uint32[entryArraySize];
    entryArrayElements = new ccs::types::uint32[entryArraySize];
    entryNumberOfMembers = new ccs::types::uint32[entryArraySize];

    entryTypes[0] = 0u;
    entryTypes[1] = 1u;
    entryTypes[2] = 1u;
    entryTypes[3] = 0u;
    entryTypes[4] = 0u;
    entryTypes[5] = 1u;
    entryTypes[6] = 0u;
    entryTypes[7] = 0u;

    entryArrayElements[0] = 1u;
    entryArrayElements[1] = 16u;
    entryArrayElements[2] = 11u;
    entryArrayElements[3] = 1u;
    entryArrayElements[4] = 1u;
    entryArrayElements[5] = 11u;
    entryArrayElements[6] = 1u;
    entryArrayElements[7] = 1u;

    entryNumberOfMembers[0] = 1u;
    entryNumberOfMembers[1] = 2u;
    entryNumberOfMembers[2] = 2u;
    entryNumberOfMembers[3] = 1u;
    entryNumberOfMembers[4] = 1u;
    entryNumberOfMembers[5] = 2u;
    entryNumberOfMembers[6] = 1u;
    entryNumberOfMembers[7] = 1u;

    // Initialize resources
    (void) this->Initialise();

    // Register instance in object database - Necessary for globally scoped accessors
    AnyObject *p_ref = (AnyObject*) this;
    ccs::base::GlobalObjectDatabase::Register(
    DEFAULT_OPCUAINTERFACE_INSTANCE_NAME,
                                              p_ref);

}

// Destructor method

Open62541Client::~Open62541Client(void) {

    if (static_cast<Open62541ClientImpl*>(NULL) != __impl) {
        delete __impl;
    }

    __impl = static_cast<Open62541ClientImpl*>(NULL);

    return;

}

Open62541ClientImpl::~Open62541ClientImpl(void) {

    delete[] methodId;
    delete[] extObj;

    free(dataPtr);

    // Release resources
    if (this->m_thread != NULL)
        delete this->m_thread;
    if (this->m_var_table != NULL)
        delete this->m_var_table;

    (void) this->Disconnect();

    // Remove instance from object database
    (void) ccs::base::GlobalObjectDatabase::Remove(
    DEFAULT_OPCUAINTERFACE_INSTANCE_NAME);

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

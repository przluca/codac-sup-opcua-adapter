// Global header files

#include <functional> // std::function<>
#include <map> // std::map
#include <new> // std::nothrow
#include <utility> // std::pair
#include <vector> // std::vector

#include <BasicTypes.h> // Misc. type definition
#include <SysTools.h> // ccs::HelperTools::SafeStringCopy, etc.
#include <CyclicRedundancyCheck.h>

#include <log-api.h> // Logging helper functions

#include <ObjectFactory.h>

#include <AnyTypeDatabase.h>

#include <Open62541Client.h>

// Local header files

#include "Open62541PlantSystemAdapter.h"

// Constants

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "sup::core"

namespace sup {

namespace core {

// Bridge class

class Open62541PlantSystemAdapterImpl {

private:

    ccs::types::AnyValue *__config_cache;

    ccs::types::string __ua_srvr;
    ccs::base::Open62541Client *__ua_clnt;

    std::vector<std::tuple<std::string, std::string, const std::shared_ptr<const ccs::types::ScalarType>, std::string, std::string>> __assoc;

    ccs::types::string eoNodeId;

    ccs::types::uint32 nodeCounter;

    ccs::types::uint32 bodyLength;

public:

    Open62541PlantSystemAdapterImpl(void);
    virtual ~Open62541PlantSystemAdapterImpl(void);

    bool ReadConfiguration(const std::string &name,
                           ccs::types::AnyValue &value) const;

    bool LoadConfiguration(const std::string &name,
                           const ccs::types::AnyValue &value,
                           const ccs::types::uint32 seed,
                           const ccs::types::uint32 checksum);

    bool SetServer(const char *server) {
        ccs::HelperTools::SafeStringCopy(__ua_srvr, server, STRING_MAX_LENGTH);
        return true;
    }
    ;

    bool CreateConfigurationCache(const std::string &name,
                                  const std::string &type);

    bool CreateChannelAssociation(const std::string &name,
                                  const std::string &chan,
                                  const std::string &type,
                                  const std::string &mthd,
                                  const std::string &extobj);
    bool StartOPCUAClient(void);

    bool SetOPCUAStructure(const std::string extobj);

    bool SetBodyLength(ccs::types::AnyValue * cache, const ccs::types::uint32 length);

};

// Function declaration

ccs::base::AnyObject* Open62541PlantSystemAdapter_Constructor(void);

// Global variables

bool Open62541PlantSystemAdapter_IsRegistered = (ccs::base::GlobalObjectFactory::Register(
        "sup::core::Open62541PlantSystemAdapter", (ccs::base::AnyObject_Constructor_t) & Open62541PlantSystemAdapter_Constructor) == STATUS_SUCCESS);

// Function definition

ccs::base::AnyObject* Open62541PlantSystemAdapter_Constructor(void) {

    sup::core::Open62541PlantSystemAdapter *ref = new (std::nothrow) sup::core::Open62541PlantSystemAdapter();

    return dynamic_cast<ccs::base::AnyObject*>(ref);

}

bool Open62541PlantSystemAdapter::SetParameter(const char *name,
                                               const char *value) {

    bool status = ((static_cast<const char*>(NULL) != name) && (static_cast<const char*>(NULL) != value));

    if (status) {
        if (std::string(name) == "serverUrl") {
            status = (static_cast<Open62541PlantSystemAdapterImpl*>(NULL) != __impl);

            if (status) {
                status = __impl->SetServer(value);
            }
        }

        if ((std::string(name) == "verbose") && (std::string(value) == "true")) {
            ccs::log::SetStdout();
        }
    }

    return status;
}

bool Open62541PlantSystemAdapter::ProcessMessage(const char *msg) {

    bool status = !ccs::HelperTools::IsUndefinedString(msg);

    log_info("Open62541PlantSystemAdapter::ProcessMessage - Entering method");

    if (status) {
        if (true == ccs::HelperTools::Contain(msg, "Create")) {
            const char *p_buf = msg;

            while ((*p_buf != 0) && (*p_buf != '{')) {
                p_buf += 1;
            }
            status = ((*p_buf != 0) && (-1 != ccs::HelperTools::FindMatchingBrace(p_buf)));

            ccs::types::string name = STRING_UNDEFINED;
            ccs::types::string type = STRING_UNDEFINED;

            if (status) {
                status = ccs::HelperTools::GetAttributeFromJSONContent(p_buf, "name", name, ccs::types::MaxStringLength);

                if (status) {
                    status = ccs::HelperTools::Strip(name, "\"");
                }
            }

            if (status) {
                status = ccs::HelperTools::GetAttributeFromJSONContent(p_buf, "type", type, ccs::types::MaxStringLength);

                if (status) {
                    status = ccs::HelperTools::Strip(type, "\"");
                }
            }

            if (status) {
                status = (static_cast<Open62541PlantSystemAdapterImpl*>(NULL) != __impl);
            }

            if (status) {
                log_info("Open62541PlantSystemAdapter::ProcessMessage - Create structured variable '%s' with type '%s' ..", name, type);
                status = __impl->CreateConfigurationCache(std::string(name), std::string(type));

                if (status) {
                    log_info(".. success");
                }
                else {
                    log_error(".. failure");
                }
            }
        }

        if (true == ccs::HelperTools::Contain(msg, "Associate")) {
            const char *p_buf = msg;

            while ((*p_buf != 0) && (*p_buf != '{')) {
                p_buf += 1;
            }
            status = ((*p_buf != 0) && (-1 != ccs::HelperTools::FindMatchingBrace(p_buf)));

            ccs::types::string name = STRING_UNDEFINED;
            ccs::types::string type = STRING_UNDEFINED;
            ccs::types::string chan = STRING_UNDEFINED;
            ccs::types::string mthd = STRING_UNDEFINED;
            ccs::types::string extobj = STRING_UNDEFINED;

            if (status) {
                status = ccs::HelperTools::GetAttributeFromJSONContent(p_buf, "name", name, ccs::types::MaxStringLength);

                if (status) {
                    status = ccs::HelperTools::Strip(name, "\"");
                }
            }

            if (status) {
                status = ccs::HelperTools::GetAttributeFromJSONContent(p_buf, "type", type, ccs::types::MaxStringLength);

                if (status) {
                    status = ccs::HelperTools::Strip(type, "\"");
                }

                if (!status) { // Type will be inferred from the configuration variable
                    status = true;
                }
            }

            if (status) {
                status = ccs::HelperTools::GetAttributeFromJSONContent(p_buf, "nodeId", chan, ccs::types::MaxStringLength);

                if (status) {
                    status = ccs::HelperTools::Strip(chan, "\"");
                }
            }
            // Method Integration
            if (status) {
                status = ccs::HelperTools::GetAttributeFromJSONContent(p_buf, "methodId", mthd, ccs::types::MaxStringLength);

                if (status) {
                    status = ccs::HelperTools::Strip(mthd, "\"");
                }

                if (!status) { // MethodId is optional
                    ccs::HelperTools::SafeStringCopy(mthd, "NULL", STRING_MAX_LENGTH);
                    status = true;
                }
            }

            //ExtensionObject Integration
            if (status) {
                status = ccs::HelperTools::GetAttributeFromJSONContent(p_buf, "extensionObject", extobj, ccs::types::MaxStringLength);

                if (status) {
                    status = ccs::HelperTools::Strip(extobj, "\"");
                    __impl->SetOPCUAStructure(std::string(chan));
                }

                if (!status) { // MethodId is optional
                    ccs::HelperTools::SafeStringCopy(extobj, "NULL", STRING_MAX_LENGTH);
                    status = true;
                }
            }

            if (status) {
                status = (static_cast<Open62541PlantSystemAdapterImpl*>(NULL) != __impl);
            }

            if (status) {
                log_info("Open62541PlantSystemAdapter::ProcessMessage - Create association ..");
                status = __impl->CreateChannelAssociation(std::string(name), std::string(chan), std::string(type), std::string(mthd), std::string(extobj));

                if (status) {
                    log_info(".. success");
                }
                else {
                    log_error(".. failure");
                }
            }
        }

        if (true == ccs::HelperTools::Contain(msg, "Start")) {
            if (status) {
                status = (static_cast<Open62541PlantSystemAdapterImpl*>(NULL) != __impl);
            }

            if (status) {
                log_info("Open62541PlantSystemAdapter::ProcessMessage - Start OPC UA client ..");
                status = __impl->StartOPCUAClient();
            }
        }
    }

    if (status) {
        log_info("Open62541PlantSystemAdapter::ProcessMessage - .. success");
    }
    else {
        log_error("Open62541PlantSystemAdapter::ProcessMessage - .. failure");
    }

    log_info("Open62541PlantSystemAdapter::ProcessMessage - Leaving method");

    return status;
}

bool Open62541PlantSystemAdapter::ReadConfiguration(const std::string &name,
                                                    ccs::types::AnyValue &value) const {

    bool status = (static_cast<Open62541PlantSystemAdapterImpl*>(NULL) != __impl);

    if (status) {
        status = __impl->ReadConfiguration(name, value);
    }

    return status;
}

bool Open62541PlantSystemAdapter::LoadConfiguration(const std::string &name,
                                                    const ccs::types::AnyValue &value,
                                                    const ccs::types::uint32 seed,
                                                    const ccs::types::uint32 checksum) {

    bool status = (static_cast<Open62541PlantSystemAdapterImpl*>(NULL) != __impl);

    if (status) {
        status = __impl->LoadConfiguration(name, value, seed, checksum);
    }

    return status;
}

// Bridge functions definition

bool Open62541PlantSystemAdapterImpl::ReadConfiguration(const std::string &name,
                                                        ccs::types::AnyValue &value) const {

    bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __config_cache);

    log_info("Reading Configuration...");

    for (ccs::types::uint32 index = 0u; (index < __assoc.size()) && status; index += 1u) {
        //log_info("Open62541PlantSystemAdapterImpl::ReadConfiguration - Update cache '%s' attribute ..", std::get < 1 > (__assoc[index]).c_str());

        memcpy(ccs::HelperTools::GetAttributeReference(__config_cache, std::get < 0 > (__assoc[index]).c_str()),
               __ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetInstance(),
               __ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetSize());
    }

    if (status) {
        value = *__config_cache; // AnyValue::operator= (const AnyValue&)
    }

    log_info("... done!");

    return status;
}

bool Open62541PlantSystemAdapterImpl::LoadConfiguration(const std::string &name,
                                                        const ccs::types::AnyValue &value,
                                                        const ccs::types::uint32 seed,
                                                        const ccs::types::uint32 checksum) {

    bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __config_cache);

    // Snapshot
    ccs::types::AnyValue copy;

    if (status) {
        status = ReadConfiguration(name, copy);
    }

    if (status) {
        // Update cache
        *__config_cache = value; // AnyValue::operator= (const AnyValue&)
        status = (checksum
                == ccs::HelperTools::CyclicRedundancyCheck < ccs::types::uint32
                        > (reinterpret_cast<ccs::types::uint8*>(__config_cache->GetInstance()), __config_cache->GetSize(), seed));
    }
    if(status) log_info("YES");

    log_info("Open62541PlantSystemAdapterImpl::LoadConfiguration - Update from cache ..");
    for (ccs::types::uint32 index = 0u; (status && (index < __assoc.size())); index += 1u) {
        memcpy(__ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetInstance(),
               ccs::HelperTools::GetAttributeReference(__config_cache, std::get < 0 > (__assoc[index]).c_str()),
               __ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetSize());

        status = __ua_clnt->UpdateVariable(std::get < 1 > (__assoc[index]).c_str());
    }
    log_info("... done!");

    // Let the CA cache also update through monitors
    if (status) {
        log_info("Open62541PlantSystemAdapterImpl::LoadConfiguration - .. check monitors ..");
        ccs::HelperTools::SleepFor(1000000000ul); // 1sec
    }

    for (ccs::types::uint32 index = 0u; (status && (index < __assoc.size())); index += 1u) {
        memcpy(ccs::HelperTools::GetAttributeReference(__config_cache, std::get < 0 > (__assoc[index]).c_str()),
               __ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetInstance(),
               __ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetSize());
    }

//    if (status) {
//        log_info("Open62541PlantSystemAdapterImpl::LoadConfiguration - .. verify checksum again ..");
//        status = (checksum
//                == ccs::HelperTools::CyclicRedundancyCheck < ccs::types::uint32
//                        > (reinterpret_cast<ccs::types::uint8*>(__config_cache->GetInstance()), __config_cache->GetSize(), seed));
//    }

    if (status) {
        log_info("Open62541PlantSystemAdapterImpl::LoadConfiguration - .. success");
    }
    else {
        log_error("Open62541PlantSystemAdapterImpl::LoadConfiguration - .. failure");
    }

    if (!status) {
        log_error("Open62541PlantSystemAdapterImpl::LoadConfiguration - Restore snapshot");

        for (ccs::types::uint32 index = 0u; index < __assoc.size(); index += 1u) {
            memcpy(__ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetInstance(),
                   ccs::HelperTools::GetAttributeReference(&copy, std::get < 0 > (__assoc[index]).c_str()),
                   __ua_clnt->GetVariable(std::get < 1 > (__assoc[index]).c_str())->GetSize());

            __ua_clnt->UpdateVariable(std::get < 1 > (__assoc[index]).c_str());
        }
    }

    return status;
}

bool Open62541PlantSystemAdapterImpl::CreateConfigurationCache(const std::string &name,
                                                               const std::string &type) {

    bool status = (static_cast<ccs::types::AnyValue*>(NULL) == __config_cache);

    std::shared_ptr<const ccs::types::AnyType> desc;

    if (status) {
        desc = ccs::types::GlobalTypeDatabase::GetType(type.c_str());
        status = static_cast<bool>(desc);
    }

    if (status) {
        __config_cache = new (std::nothrow) ccs::types::AnyValue(desc);
        status = (static_cast<ccs::types::AnyValue*>(NULL) != __config_cache);
    }

    return status;
}

bool Open62541PlantSystemAdapterImpl::CreateChannelAssociation(const std::string &name,
                                                               const std::string &chan,
                                                               const std::string &type,
                                                               const std::string &mthd,
                                                               const std::string &extobj) {

    bool status = (static_cast<ccs::types::AnyValue*>(NULL) != __config_cache);

    std::shared_ptr<const ccs::types::AnyType> desc;

    if (status) {
        desc = ccs::types::GlobalTypeDatabase::GetType(type.c_str());
        status = static_cast<bool>(desc);

        if (!status && type.empty()) { // Get the type description from the configuration variable
            desc = ccs::HelperTools::GetAttributeType(__config_cache, name.c_str());
            status = static_cast<bool>(desc);
        }
    }

    if (ccs::HelperTools::Is < ccs::types::CompoundType > (desc)) {
        ccs::types::uint32 nElem = std::dynamic_pointer_cast<const ccs::types::CompoundType>(desc)->GetAttributeNumber();
        if (nElem > 0) {
            for (ccs::types::uint32 index = 0u; (status && (index < nElem)); index += 1u) {
                const std::string cName = std::dynamic_pointer_cast<const ccs::types::CompoundType>(desc)->GetAttributeName(index);
                char bufferName[name.length() + cName.length()];
                std::sprintf(bufferName, "%s.%s", name.c_str(), cName.c_str());
                const std::string cType = (std::dynamic_pointer_cast<const ccs::types::CompoundType>(desc)->GetAttributeType(index))->GetName();
                this->CreateChannelAssociation(bufferName, chan, cType.c_str(), mthd, extobj);
            }
        }
    }
    else if (ccs::HelperTools::Is < ccs::types::ArrayType > (desc)) {
        ccs::types::uint32 nElem = std::dynamic_pointer_cast<const ccs::types::ArrayType>(desc)->GetElementNumber();
        for (ccs::types::uint32 index = 0u; (status && (index < nElem)); index += 1u) {
            char bufferName[name.length() + 5];
            std::sprintf(bufferName, "%s.[%d]", name.c_str(), index);
            this->CreateChannelAssociation(bufferName, chan, (std::dynamic_pointer_cast<const ccs::types::ArrayType>(desc)->GetElementType())->GetName(), mthd,
                                           extobj);
        }
    }
    else if (ccs::HelperTools::Is < ccs::types::ScalarType > (desc)) {

//        if (status) {
//            status = ccs::HelperTools::HasAttribute(__config_cache, name.c_str());
//        }

        // Register association
        if (status) {
            char bufferChan[chan.length() + 10];
            std::sprintf(bufferChan, "chan_%d", nodeCounter);
            std::string newChan = bufferChan;
            __assoc.push_back(std::make_tuple(name, newChan, std::dynamic_pointer_cast<const ccs::types::ScalarType>(desc), mthd, extobj));
            nodeCounter++;
        }
    }

    return status;
}

bool Open62541PlantSystemAdapterImpl::StartOPCUAClient(void) {

    bool status = (static_cast<ccs::base::Open62541Client*>(NULL) == __ua_clnt);

    if (status) {
        __ua_clnt = new (std::nothrow) ccs::base::Open62541Client(__ua_srvr);
        status = (static_cast<ccs::base::Open62541Client*>(NULL) != __ua_clnt);
    }

    // Configure OPCUA client
    // Setting Method
    __ua_clnt->SetNumberOfNodes(__assoc.size());
    __ua_clnt->SetEONodeId(eoNodeId);
    __ua_clnt->SetBodyLength(16740);

    for (ccs::types::uint32 index = 0u; (status && (index < __assoc.size())); index += 1u) {
        //using namespace std::placeholders;

        status = __ua_clnt->AddVariable(std::get < 1 > (__assoc[index]).c_str(), ccs::types::AnyputVariable, std::get < 2 > (__assoc[index]), index);
//        status = __ua_clnt->AddVariable(std::get < 0 > (__assoc[index]).c_str(), ccs::types::AnyputVariable, std::get < 2 > (__assoc[index]), index);

        if (std::get < 3 > (__assoc[index]).compare("NULL") != 0) {
            __ua_clnt->AddMethod(std::get < 3 > (__assoc[index]).c_str(), index);
        }
        if (std::get < 4 > (__assoc[index]).compare("NULL") != 0) {
            __ua_clnt->SetExtensionObject(std::get < 4 > (__assoc[index]).c_str(), index);
        }
    }

    if (status) {
        status = __ua_clnt->Launch();
    }

    return status;
}

bool Open62541PlantSystemAdapterImpl::SetOPCUAStructure(const std::string extobj) {
    ccs::HelperTools::SafeStringCopy(eoNodeId, extobj.c_str(), STRING_MAX_LENGTH);
    return true;
}

bool Open62541PlantSystemAdapterImpl::SetBodyLength(ccs::types::AnyValue * cache, const ccs::types::uint32 length) {
    return true;
}

Open62541PlantSystemAdapter::Open62541PlantSystemAdapter(void) {

    log_info("Open62541PlantSystemAdapter::Open62541PlantSystemAdapter - Entering method");

    __impl = new (std::nothrow) Open62541PlantSystemAdapterImpl();

    return;
}

Open62541PlantSystemAdapterImpl::Open62541PlantSystemAdapterImpl(void) {

    // Initialise attributes
    __config_cache = static_cast<ccs::types::AnyValue*>(NULL);

    nodeCounter = 0u;

    // Create CA client
    __ua_clnt = static_cast<ccs::base::Open62541Client*>(NULL);

    return;
}

Open62541PlantSystemAdapter::~Open62541PlantSystemAdapter(void) {

    if (__builtin_expect((static_cast<Open62541PlantSystemAdapterImpl*>(NULL) != __impl), 1)) // Likely
                         {
        delete __impl;
        __impl = static_cast<Open62541PlantSystemAdapterImpl*>(NULL);
    }

    return;
}

Open62541PlantSystemAdapterImpl::~Open62541PlantSystemAdapterImpl(void) {

    if (__builtin_expect((static_cast<ccs::base::Open62541Client*>(NULL) != __ua_clnt), 1)) // Likely
                         {
        delete __ua_clnt;
        __ua_clnt = static_cast<ccs::base::Open62541Client*>(NULL);
    }

    return;
}

} // namespace core

} // namespace sup

#undef LOG_ALTERN_SRC

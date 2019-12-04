/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/opcua/OPCUAClient.cpp $
* $Id: OPCUAClient.cpp 99960 2019-05-28 06:07:33Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Infrastructure tools - Prototype
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

/* ToDo 
     - Manage default value from configuration
*/

// Global header files

#include <functional> // std::function
#include <new> // std::nothrow

#include <uabase.h> // OPC UA client SDK
#include <uaplatformlayer.h>
#include <uaclientsdk.h>
#include <uasession.h>

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

#include "OPCUAClient.h" // This class definition

// Constants

#define DEFAULT_OPCUAINTERFACE_THREAD_PERIOD 10000000ul // 100Hz
#define DEFAULT_OPCUAINTERFACE_INSTANCE_NAME "ua-interface"

#define MAXIMUM_VARIABLE_NUM 256

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ua-if"

#ifndef log_trace
#define log_trace(arg_msg...) {}
#endif

// Type definition

namespace ccs {

namespace base {

class OPCUAClientImpl : public AnyObject, public UaClientSdk::UaSessionCallback, public UaClientSdk::UaSubscriptionCallback
{

  private:

    uint64_t m_sleep;

    ccs::base::AnyThread* m_thread;

    ccs::types::string __service;
    bool __connected = false;

    // Initialiser methods
    bool Initialise (void);
    bool Connect (void);
    bool Disconnect (void);
    bool Subscribe (void);
    bool Unsubscribe (void);

  protected:

  public:

    UaClientSdk::UaSession* __session = static_cast<UaClientSdk::UaSession*>(NULL);
    UaClientSdk::UaSubscription* __subscription = static_cast<UaClientSdk::UaSubscription*>(NULL);

    ccs::types::CompoundType* m_type; // Introspectable type definition for the variable cache ..
    ccs::types::AnyValue* m_value; // Introspectable variable mapped to the whole cache ..

    bool m_initialized;

    typedef struct VariableInfo {

      bool update;
      ccs::types::DirIdentifier direction;
      
      std::function<void(const char*, const ccs::types::AnyValue&)> cb;

      ccs::types::string name;
#if 0
      chid channel;
      chtype type; // OPCUA native type
#endif
      ccs::types::uint32 mult;
      
      void* reference; // Reference in the cache
      
      std::shared_ptr<const ccs::types::AnyType> __type; // Introspectable type definition for the variable cache ..
      ccs::types::AnyValue* value;
      
    } VariableInfo_t;

    LUTable<VariableInfo_t>* m_var_table;

    // Initializer methods
    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const std::shared_ptr<const ccs::types::AnyType>& type);

    bool Launch (void); // Should be called after the variable table is populated

    // Accessor methods
    bool IsValid (ccs::types::uint32 id) const;
    bool IsValid (const char* name) const;

    bool IsConnected (void) const;

    ccs::types::uint32 GetVariableId (const char* name) const;

    ccs::types::AnyValue* GetVariable (ccs::types::uint32 id) const;
    ccs::types::AnyValue* GetVariable (const char* name) const;

    template <typename Type> bool GetVariable (const char* name, Type& value) const;

    std::shared_ptr<const ccs::types::AnyType> GetVariableType (ccs::types::uint32 id) const;
    std::shared_ptr<const ccs::types::AnyType> GetVariableType (const char* name) const;

    template <typename Type> bool SetVariable (const char* name, Type& value);

    bool UpdateVariable (ccs::types::uint32 id);
    bool UpdateVariable (const char* name);

    // Miscellaneous methods
    bool SetCallback (ccs::types::uint32 id, std::function<void(const char*, const ccs::types::AnyValue&)> cb);
    bool SetCallback (const char* name, std::function<void(const char*, const ccs::types::AnyValue&)> cb);

    bool SetService (const char* service);

    // Constructor methods
    OPCUAClientImpl (void);

    // Destructor method
    virtual ~OPCUAClientImpl (void);

    // Implementation-specific specialisation
    // UaSessionCallback methods ----------------------------------------------------
    virtual void connectionStatusChanged (OpcUa_UInt32 clientConnectionId, UaClientSdk::UaClient::ServerStatus serverStatus);

    // UaSubscriptionCallback implementation ----------------------------------------------------
    virtual void subscriptionStatusChanged (OpcUa_UInt32 clientSubscriptionHandle, const UaStatus& status);
    virtual void dataChange (OpcUa_UInt32 clientSubscriptionHandle, const UaDataNotifications& dataNotifications, const UaDiagnosticInfos& diagnosticInfos);
    virtual void newEvents (OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists& eventFieldList);

};

// Global variables

} // namespace base

} // namespace ccs

// Function declaration

// Function definition

void OPCUAInterface_Thread_PRBL (ccs::base::OPCUAClientImpl* self)
{

  log_info("Entering '%s' routine", __FUNCTION__);

  // Create variable cache
  self->m_value = new (std::nothrow) ccs::types::AnyValue (self->m_type);

  // Create subscription specification
  UaMonitoredItemCreateRequests items;
  items.create((self->m_var_table)->GetSize());

  for (ccs::types::uint32 index = 0; index < (self->m_var_table)->GetSize(); index += 1u)
    {

      ccs::types::string name;

      if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS)
	{
	  log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
	  continue;
	}
  
      ccs::base::OPCUAClientImpl::VariableInfo_t varInfo; 

      if ((self->m_var_table)->GetValue(varInfo, index) != STATUS_SUCCESS)
	{
	  log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
	  continue;
	}

      // Store cache reference
      varInfo.reference = ccs::HelperTools::GetAttributeReference(self->m_value, name);
      varInfo.value = new (std::nothrow) ccs::types::AnyValue (varInfo.__type, varInfo.reference);

      // Install subscription for all variables
      UaString variable (name); // ns=<namespace_index>;s=<fully_qualified_path>
      UaNodeId::fromXmlString(variable).copyTo(&items[index].ItemToMonitor.NodeId);
      items[index].ItemToMonitor.AttributeId = OpcUa_Attributes_Value;
      items[index].RequestedParameters.ClientHandle = index;
      items[index].RequestedParameters.SamplingInterval = 100;
      items[index].RequestedParameters.QueueSize = 1;
      items[index].RequestedParameters.DiscardOldest = OpcUa_True;
      items[index].MonitoringMode = OpcUa_MonitoringMode_Reporting;

      // Store for future use
      (self->m_var_table)->SetValue(varInfo, index);

    }

  bool status = (NULL != self->__subscription);

  if (status)
    {
      // Load subscription specification
      UaClientSdk::ServiceSettings settings;
      UaMonitoredItemCreateResults results;

      UaStatus result;
      result = self->__subscription->createMonitoredItems(settings, OpcUa_TimestampsToReturn_Both, items, results);
      bool status = result.isGood();

      for (ccs::types::uint32 index = 0; (status && (index < results.length())); index += 1u)
	{
	  log_info("%s - Subscription for '%s' ..", __FUNCTION__, UaNodeId(items[index].ItemToMonitor.NodeId).toXmlString().toUtf8());
	  
	  if (OpcUa_IsGood(results[index].StatusCode))
	    {
	      log_info("%s - .. successful", __FUNCTION__);
	    }
	  else
	    {
	      log_error("%s - .. failed with status '%s'", __FUNCTION__, UaStatus(results[index].StatusCode).toString().toUtf8());
	    }
	}
    }

  self->m_initialized = true;

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void OPCUAInterface_Thread_CB (ccs::base::OPCUAClientImpl* self)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  bool status = self->m_initialized;

  for (ccs::types::uint32 index = 0u; (status && (index < (self->m_var_table)->GetSize())); index += 1u)
    {

      ccs::types::string name;

      if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS)
	{
	  log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
	  continue;
	}
  
      ccs::base::OPCUAClientImpl::VariableInfo_t varInfo;

      if ((self->m_var_table)->GetValue(varInfo, index) != STATUS_SUCCESS)
	{
	  log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
	  continue;
	}

      if ((varInfo.direction == ccs::types::InputVariable) || (varInfo.update != true))
	{
	  continue; // Nothing to do for this channel
	}

      UaWriteValues node;
      node.create(1);
      node[0].AttributeId = OpcUa_Attributes_Value;

      UaString variable (name); // ns=<namespace_index>;s=<fully_qualified_path>
      UaNodeId::fromXmlString(variable).copyTo(&node[0].NodeId);

      UaVariant value;

      // Set type, set value
      if (ccs::types::Boolean == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setBoolean(*(reinterpret_cast<OpcUa_Boolean*>(varInfo.reference)));
      else if (ccs::types::SignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setSByte(*(reinterpret_cast<ccs::types::int8*>(varInfo.reference)));
      else if (ccs::types::UnsignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setByte(*(reinterpret_cast<ccs::types::uint8*>(varInfo.reference)));
      else if (ccs::types::SignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setInt16(*(reinterpret_cast<ccs::types::int16*>(varInfo.reference)));
      else if (ccs::types::UnsignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setUInt16(*(reinterpret_cast<ccs::types::uint16*>(varInfo.reference)));
      else if (ccs::types::SignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setInt32(*(reinterpret_cast<ccs::types::int32*>(varInfo.reference)));
      else if (ccs::types::UnsignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setUInt32(*(reinterpret_cast<ccs::types::uint32*>(varInfo.reference)));
      else if (ccs::types::SignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setInt64(*(reinterpret_cast<ccs::types::int64*>(varInfo.reference)));
      else if (ccs::types::UnsignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setUInt64(*(reinterpret_cast<ccs::types::uint64*>(varInfo.reference)));
      else if (ccs::types::Float32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setFloat(*(reinterpret_cast<ccs::types::float32*>(varInfo.reference)));
      else if (ccs::types::Float64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	value.setDouble(*(reinterpret_cast<ccs::types::float64*>(varInfo.reference)));
      else
	continue;

      value.copyTo(&node[0].Value.Value);

      UaClientSdk::ServiceSettings settings;
      UaStatusCodeArray results;
      UaDiagnosticInfos diagnostics;

      UaStatus result; // No copy constructor
      result = self->__session->write(settings, node, results, diagnostics);
      status = result.isGood();

      if (status) // Write successful
	{
	  status = (OpcUa_IsGood(results[0]));
	}

      if (status)
	{
          log_info("%s - Write successful for '%s'", __FUNCTION__, name);
        }
      else
	{
          log_error("%s - Write failed for '%s' with status '%s'", __FUNCTION__, name, UaStatus(results[0]).toString().toUtf8());
        }

      // Store value for later use - Only the update request flag which has been cleared
      (self->m_var_table)->SetValue(varInfo, index);

    }

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void OPCUAInterface_Thread_POST (ccs::base::OPCUAClientImpl* self)
{

  return;

}

namespace ccs {

namespace base {

void OPCUAClientImpl::connectionStatusChanged (OpcUa_UInt32 clientConnectionId, UaClientSdk::UaClient::ServerStatus serverStatus)
{

  log_info("OPCUAClientImpl::connectionStatusChanged - Entering method");

  OpcUa_ReferenceParameter(clientConnectionId);

  switch (serverStatus)
    {
      case UaClientSdk::UaClient::Disconnected:
	log_info("OPCUAClientImpl::connectionStatusChanged - .. disconnected");
	__connected = false;
        break;
      case UaClientSdk::UaClient::Connected:
	log_info("OPCUAClientImpl::connectionStatusChanged - .. connected");
	__connected = true;
        break;
      case UaClientSdk::UaClient::ConnectionWarningWatchdogTimeout:
	log_info("OPCUAClientImpl::connectionStatusChanged - .. timeout");
	__connected = false;
        break;
      case UaClientSdk::UaClient::ConnectionErrorApiReconnect:
	log_info("OPCUAClientImpl::connectionStatusChanged - .. reconnect");
	__connected = false;
        break;
      case UaClientSdk::UaClient::ServerShutdown:
	log_info("OPCUAClientImpl::connectionStatusChanged - .. shutdown");
	__connected = false;
        break;
      case UaClientSdk::UaClient::NewSessionCreated:
	log_info("OPCUAClientImpl::connectionStatusChanged - .. session");
	__connected = true;
        break;
    }

  log_info("OPCUAClientImpl::connectionStatusChanged - Leaving method");

  return;

}

void OPCUAClientImpl::subscriptionStatusChanged (OpcUa_UInt32 clientSubscriptionHandle, //!< [in] Client defined handle of the affected subscription
						 const UaStatus& status)                //!< [in] Changed status for the subscription
{

  log_info("OPCUAClientImpl::subscriptionStatusChanged - Method called");

  OpcUa_ReferenceParameter(clientSubscriptionHandle); // Only one subscription registered for the session

  log_info("OPCUAClientImpl::subscriptionStatusChanged - Status '%s'", status.toString().toUtf8());

  return;

}

void OPCUAClientImpl::dataChange (OpcUa_UInt32 clientSubscriptionHandle,        //!< [in] Client defined handle of the affected subscription
				  const UaDataNotifications& dataNotifications, //!< [in] List of data notifications sent by the server
				  const UaDiagnosticInfos& diagnosticInfos)     //!< [in] List of diagnostic info related to the data notifications. This list can be empty.
{

  OpcUa_ReferenceParameter(clientSubscriptionHandle); // Only one subscription registered for the session
  OpcUa_ReferenceParameter(diagnosticInfos);
  
  for (ccs::types::uint32 index = 0; index < dataNotifications.length(); index += 1u)
    {
      log_info("OPCUAClientImpl::dataChange - Notification received for variable '%u' (handle) ..", dataNotifications[index].ClientHandle);
      
      if (OpcUa_IsGood(dataNotifications[index].Value.StatusCode))
	{
	  UaVariant value = dataNotifications[index].Value.Value;
	  log_info("OPCUAClientImpl::dataChange - .. value '%s'", value.toString().toUtf8());

	  ccs::types::string name;
	  
	  if ((this->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS)
	    {
	      log_warning("OPCUAClientImpl::dataChange - LUTable<>::GetKeyword failed", __FUNCTION__);
	      continue;
	    }
	  
	  // Update cache
	  ccs::base::OPCUAClientImpl::VariableInfo_t varInfo; 

	  if ((this->m_var_table)->GetValue(varInfo, dataNotifications[index].ClientHandle) != STATUS_SUCCESS)
	    {
	      log_warning("OPCUAClientImpl::dataChange - LUTable<>::GetValue failed");
	      continue;
	    }

	  // Set type, set value
	  if (ccs::types::Boolean == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toBoolean(*(reinterpret_cast<OpcUa_Boolean*>(varInfo.reference)));
	  else if (ccs::types::SignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toSByte(*(reinterpret_cast<ccs::types::int8*>(varInfo.reference)));
	  else if (ccs::types::UnsignedInteger8 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toByte(*(reinterpret_cast<ccs::types::uint8*>(varInfo.reference)));
	  else if (ccs::types::SignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toInt16(*(reinterpret_cast<ccs::types::int16*>(varInfo.reference)));
	  else if (ccs::types::UnsignedInteger16 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toUInt16(*(reinterpret_cast<ccs::types::uint16*>(varInfo.reference)));
	  else if (ccs::types::SignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toInt32(*(reinterpret_cast<ccs::types::int32*>(varInfo.reference)));
	  else if (ccs::types::UnsignedInteger32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toUInt32(*(reinterpret_cast<ccs::types::uint32*>(varInfo.reference)));
	  else if (ccs::types::SignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toInt64(*(reinterpret_cast<ccs::types::int64*>(varInfo.reference)));
	  else if (ccs::types::UnsignedInteger64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toUInt64(*(reinterpret_cast<ccs::types::uint64*>(varInfo.reference)));
	  else if (ccs::types::Float32 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toFloat(*(reinterpret_cast<ccs::types::float32*>(varInfo.reference)));
	  else if (ccs::types::Float64 == std::dynamic_pointer_cast<const ccs::types::ScalarType>(varInfo.__type))
	    value.toDouble(*(reinterpret_cast<ccs::types::float64*>(varInfo.reference)));
	  else
	    continue;

	  // Invoke callback, if any
	  if (NULL != varInfo.cb)
	    {
	      log_debug("%OPCUAClientImpl::dataChange -  Invoke callback for '%s' variable", name);
	      varInfo.cb(name, *varInfo.value);
	    }
	}
      else
	{
	  UaStatus itemError (dataNotifications[index].Value.StatusCode);
	  log_info("OPCUAClientImpl::dataChange - .. error with status '%s'", itemError.toString().toUtf8());
	}
    }

  return;

}

void OPCUAClientImpl::newEvents (OpcUa_UInt32 clientSubscriptionHandle, //!< [in] Client defined handle of the affected subscription
				 UaEventFieldLists& eventFieldList)     //!< [in] List of event notifications sent by the server
{

  log_info("OPCUAClientImpl::newEvents - Method called");

  OpcUa_ReferenceParameter(clientSubscriptionHandle);
  OpcUa_ReferenceParameter(eventFieldList);

  return;

}

// Initializer methods
 
bool OPCUAClient::AddVariable (const char* name, ccs::types::DirIdentifier direction, const std::shared_ptr<const ccs::types::AnyType>& type)
{
  return __impl->AddVariable(name, direction, type);
}

bool OPCUAClientImpl::AddVariable (const char* name, ccs::types::DirIdentifier direction, const std::shared_ptr<const ccs::types::AnyType>& type)
{

  VariableInfo_t varInfo;

  varInfo.cb = NULL;
  varInfo.update = false;
  varInfo.direction = direction;
  ccs::HelperTools::SafeStringCopy(varInfo.name, name, sizeof(ccs::types::string));

  bool status = (ccs::HelperTools::Is<ccs::types::ScalarType>(type) || 
		 (ccs::HelperTools::Is<ccs::types::ArrayType>(type) && 
		  ccs::HelperTools::Is<ccs::types::ScalarType>(std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementType())));

  if (status)
    {
      varInfo.mult = (ccs::HelperTools::Is<ccs::types::ScalarType>(type) ?
		      1 :
		      std::dynamic_pointer_cast<const ccs::types::ArrayType>(type)->GetElementNumber());
      varInfo.__type = type;
    }

  if (status)
    {
      status = ((static_cast<ccs::types::CompoundType*>(NULL) != this->m_type) &&
		(static_cast<LUTable<VariableInfo_t>*>(NULL) != this->m_var_table));
    }

  if (status)
    {
      log_info("OPCUAClientImpl::AddVariable - Name '%s' and type '%s'", name, type->GetName());
      status = (this->m_type)->AddAttribute(name, varInfo.__type); // Type definition for the variable cache
    }

  if (status)
    {
      status = (this->m_var_table)->AddPair(name, varInfo);
    }

  return status;

}

bool OPCUAClientImpl::Initialise (void) 
{ 

  // Initialize resources
  this->m_sleep = DEFAULT_OPCUAINTERFACE_THREAD_PERIOD;
  this->m_var_table = new LUTable<VariableInfo_t> (MAXIMUM_VARIABLE_NUM); // The table will be filled with application-specific variable list
  this->m_initialized = false;

  this->m_type = new (std::nothrow) ccs::types::CompoundType ("uaif::VariableCache_t");
  this->m_value = static_cast<ccs::types::AnyValue*>(NULL);

  this->m_thread = new ccs::base::AnyThread ("OPC UA Interface"); 
  (this->m_thread)->SetPeriod(this->m_sleep); (this->m_thread)->SetAccuracy(this->m_sleep);
  (this->m_thread)->SetPreamble((void (*)(void*)) &OPCUAInterface_Thread_PRBL, (void*) this); 
  (this->m_thread)->SetCallback((void (*)(void*)) &OPCUAInterface_Thread_CB, (void*) this); 
  (this->m_thread)->SetPostface((void (*)(void*)) &OPCUAInterface_Thread_POST, (void*) this); 
  //(this->m_thread)->Launch(); // Should be performed after the variable table is populated

  log_info("OPCUAClientImpl::Initialise - Creating UaClientSdk::UaSession");

  UaPlatformLayer::init();

  // Create session
  __session = new (std::nothrow) UaClientSdk::UaSession ();

  return true;

}

bool OPCUAClientImpl::Connect (void)
{

  bool status = (NULL != __session);

  if (status)
    {
      // Connect, if necessary
      if (__session->isConnected() == OpcUa_False)
        {
	  log_info("OPCUAClientImpl::Connect - Connecting to '%s' ..", __service);

	  ccs::types::string buffer; 

	  status = ccs::HelperTools::GetHostName(buffer);

	  if (status)
	    {
	      UaString url (__service);
	      UaString host (buffer);
	      
	      UaClientSdk::SessionConnectInfo connection;
	      connection.sApplicationName = "OPC UA Client";
	      connection.sApplicationUri  = UaString("urn:%1:ccs:OPCUAClient").arg(host); // Use the host name to generate a unique application URI
	      //connection.sApplicationUri  = "urn:ccs:OPCUAClient";
	      connection.sProductUri      = "urn:ccs:OPCUAClient";
	      //connection.sSessionName     = connection.sApplicationUri;

	      connection.bAutomaticReconnect = true;
	      connection.bRetryInitialConnect = true;	      

	      // No security for now .. leave uninitialised
	      UaClientSdk::SessionSecurityInfo security;
	      
	      UaStatus result; // No copy constructor
	      result = __session->connect(url, connection, security, this);
	      status = result.isGood();
	    }
        }
    }

  if (status)
    {
      status = this->IsConnected();
    }

  if (status)
    { 
      // Create subscription
      status = this->Subscribe();
    }

  return status;

}

bool OPCUAClientImpl::IsConnected (void) const
{

  bool status = (NULL != __session);

  if (status)
    {
      status = (__session->isConnected() != OpcUa_False);
    }

  if (status)
    {
      // Try and read server timestamp
      UaReadValueIds node;
      node.create(1);
      node[0].AttributeId = OpcUa_Attributes_Value;
      node[0].NodeId.Identifier.Numeric = OpcUaId_Server_ServerStatus_CurrentTime;

      UaClientSdk::ServiceSettings settings;
      UaDataValues      values;
      UaDiagnosticInfos diagnostics;

      UaStatus result; // No copy constructor
      result = __session->read(settings, 0, OpcUa_TimestampsToReturn_Both, node, values, diagnostics);
      status = result.isGood();

      if (status) // Read successful
	{
	  status = (OpcUa_IsGood(values[0].StatusCode));
	}

      if (status) // Value valid
        {
	  log_info("OPCUAClientImpl::IsConnected - Success with '%s'", UaVariant(values[0].Value).toString().toUtf8());
        }
    }

  return status;

}

bool OPCUAClientImpl::Disconnect (void)
{

  bool status = ((NULL != __session) &&
		 (NULL != __subscription));

  if (status)
    {
      // Cancel subscription
      status = this->Unsubscribe();
    }

  if (status)
    {
      // Disconnect, if necessary
      if (__session->isConnected() != OpcUa_False)
        {
	  UaClientSdk::ServiceSettings settings;
	  UaStatus result; // No copy constructor
	  result = __session->disconnect(settings, OpcUa_True);
	  status = result.isGood();
        }
    }

  return status;

}

bool OPCUAClientImpl::Subscribe (void)
{

  bool status = ((NULL != __session) &&
		 (NULL == __subscription));

  if (status)
    {
      // Create subscription
      UaClientSdk::ServiceSettings service;
      UaClientSdk::SubscriptionSettings settings;
      settings.publishingInterval = 100;
      
      log_info("OPCUAClientImpl::Subscribe - Creating subscription ..");

      UaStatus result; // No copy constructor
      result = __session->createSubscription(service, this, 1, settings, OpcUa_True, &__subscription);

      status = result.isGood();
    }

  return status;

}

bool OPCUAClientImpl::Unsubscribe (void)
{

  bool status = ((NULL != __session) &&
		 (NULL != __subscription));

  if (status)
    {
      // Cancel subscription
      UaClientSdk::ServiceSettings settings;
      UaStatus result;
      result = __session->deleteSubscription(settings, &__subscription);
      status = result.isGood();
    }

  if (status)
    {
      __subscription = static_cast<UaClientSdk::UaSubscription*>(NULL);
    }

  return status;

}

bool OPCUAClient::Launch (void) { return __impl->Launch(); }
bool OPCUAClientImpl::Launch (void) { return ((this->m_thread)->Launch() == STATUS_SUCCESS); } // Should be called after the variable table is populated

// Accessor methods

bool OPCUAClient::IsValid (const char* name) const { return __impl->IsValid(name); }

bool OPCUAClientImpl::IsValid (uint_t id) const { return ((this->m_var_table)->IsValid(id) == STATUS_SUCCESS); }
bool OPCUAClientImpl::IsValid (const char* name) const { return ((this->m_var_table)->IsValid(name) == STATUS_SUCCESS); }

uint_t OPCUAClientImpl::GetVariableId (const char* name) const { uint_t id = 0; (this->m_var_table)->GetIndex(name, id); return id; }

ccs::types::AnyValue* OPCUAClient::GetVariable (const char* name) const { return __impl->GetVariable(__impl->GetVariableId(name)); }

ccs::types::AnyValue* OPCUAClientImpl::GetVariable (uint_t id) const { VariableInfo_t varInfo; varInfo.value = static_cast<ccs::types::AnyValue*>(NULL); if (this->IsValid(id)) (this->m_var_table)->GetValue(varInfo, id); return varInfo.value; }
ccs::types::AnyValue* OPCUAClientImpl::GetVariable (const char* name) const { return this->GetVariable(this->GetVariableId(name)); }

std::shared_ptr<const ccs::types::AnyType> OPCUAClientImpl::GetVariableType (uint_t id) const { std::shared_ptr<const ccs::types::AnyType> type; if (this->IsValid(id)) type = this->GetVariable(id)->GetType(); return type; }
std::shared_ptr<const ccs::types::AnyType> OPCUAClientImpl::GetVariableType (const char* name) const { return this->GetVariableType(this->GetVariableId(name)); }

bool OPCUAClient::UpdateVariable (const char* name) { return __impl->UpdateVariable(__impl->GetVariableId(name)); }

bool OPCUAClientImpl::UpdateVariable (uint_t id) { bool status = this->IsValid(id); if (status) { VariableInfo_t varInfo; (this->m_var_table)->GetValue(varInfo, id); varInfo.update = true; (this->m_var_table)->SetValue(varInfo, id); } return status; }
bool OPCUAClientImpl::UpdateVariable (const char* name) { return this->UpdateVariable(this->GetVariableId(name)); }

bool OPCUAClient::SetCallback (const char* name, std::function<void(const char*, const ccs::types::AnyValue&)> cb) { return __impl->SetCallback(name, cb); }

bool OPCUAClientImpl::SetCallback (ccs::types::uint32 id, std::function<void(const char*, const ccs::types::AnyValue&)> cb) { VariableInfo_t varInfo; bool status = this->IsValid(id); if (status) { (this->m_var_table)->GetValue(varInfo, id); varInfo.cb = cb; (this->m_var_table)->SetValue(varInfo, id); } return status; }
bool OPCUAClientImpl::SetCallback (const char* name, std::function<void(const char*, const ccs::types::AnyValue&)> cb) { return this->SetCallback(this->GetVariableId(name), cb); }

bool OPCUAClientImpl::SetService (const char* service) 
{ 

  ccs::HelperTools::SafeStringCopy(__service, service, STRING_MAX_LENGTH);

  return this->Connect(); 

}

// Constructor methods

OPCUAClient::OPCUAClient (const char* service)
{ 

  // Instantiate implementation class
  __impl = new (std::nothrow) OPCUAClientImpl;

  bool status = (static_cast<OPCUAClientImpl*>(NULL) != __impl);

  if (status)
    {
      status = __impl->SetService(service);
    }

  return; 

}

OPCUAClientImpl::OPCUAClientImpl (void)
{ 

  // Initialize resources
  this->Initialise();

  // Register instance in object database - Necessary for globally scoped accessors
  AnyObject* p_ref = (AnyObject*) this; ccs::base::GlobalObjectDatabase::Register(DEFAULT_OPCUAINTERFACE_INSTANCE_NAME, p_ref); 

}

// Destructor method

OPCUAClient::~OPCUAClient (void)
{ 

  if (static_cast<OPCUAClientImpl*>(NULL) != __impl)
    { 
      delete __impl; 
    }

  __impl= static_cast<OPCUAClientImpl*>(NULL);

  return;

}

OPCUAClientImpl::~OPCUAClientImpl (void)
{ 

  // Release resources
  if (this->m_thread != NULL) delete this->m_thread;
  if (this->m_var_table != NULL) delete this->m_var_table;

  this->Disconnect();

  if (NULL != __session)
    {
      delete __session;
      __session = NULL;
    }

  // Remove instance from object database
  ccs::base::GlobalObjectDatabase::Remove(DEFAULT_OPCUAINTERFACE_INSTANCE_NAME); 

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC
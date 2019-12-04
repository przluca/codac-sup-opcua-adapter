/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/ca/ChannelAccessClient.cpp $
* $Id: ChannelAccessClient.cpp 99960 2019-05-28 06:07:33Z bauvirb $
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

#include <cadef.h> // Channel Access API definition, etc.

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

#include "AnyTypeToCA.h" // .. associated helper routines

#include "ChannelAccessClient.h" // This class definition

// Constants

#define DEFAULT_CAINTERFACE_THREAD_PERIOD 10000000ul // 100Hz
#define DEFAULT_CAINTERFACE_INSTANCE_NAME "ca-interface"

#define MAXIMUM_VARIABLE_NUM 256

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "ca-if"

#ifndef log_trace
#define log_trace(arg_msg...) {}
#endif

// Type definition

namespace ccs {

namespace base {

class ChannelAccessClient_Impl : public AnyObject
{

  private:

    uint64_t m_sleep;

    ccs::base::AnyThread* m_thread;

    // Initializer methods
    void Initialize (void);

  protected:

  public:

    ccs::types::CompoundType* m_type; // Introspectable type definition for the variable cache ..
    ccs::types::AnyValue* m_value; // Introspectable variable mapped to the whole cache ..

    bool m_initialized;

    typedef struct VariableInfo {

      bool connected;
      bool update;
      ccs::types::DirIdentifier direction;
      
      std::function<void(const char*, const ccs::types::AnyValue&)> cb;

      ccs::types::string name;
      chid channel;
      chtype type; // ChannelAccess native type
      ccs::types::uint32 mult;
      
      void* reference; // Reference in the cache
      
      std::shared_ptr<const ccs::types::AnyType> __type; // Introspectable type definition for the variable cache ..
      ccs::types::AnyValue* value;
      
    } VariableInfo_t;

    LUTable<VariableInfo_t>* m_var_table;

    // Initializer methods
    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const std::shared_ptr<const ccs::types::AnyType>& type);
    bool AddVariable (const char* name, bool isInput, chtype type, ccs::types::uint32 mult = 1u); // From SDD PV list */

    bool Launch (void); // Should be called after the variable table is populated

    // Accessor methods
    bool IsValid (ccs::types::uint32 id) const;
    bool IsValid (const char* name) const;

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

    // Constructor methods
    ChannelAccessClient_Impl (void);

    // Destructor method
    virtual ~ChannelAccessClient_Impl (void);

};

// Global variables

namespace ChannelAccessInterface {

ChannelAccessClient* __ca_if = static_cast<ChannelAccessClient*>(NULL);

} // namespace ChannelAccessInterface

} // namespace base

} // namespace ccs

// Function declaration

// Function definition

void ChannelAccessInterface_Get_CB (struct event_handler_args args)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  bool status = (args.status == ECA_NORMAL);

  if (!status)
    {
      log_warning("%s - Routine invoked with '%d'", __FUNCTION__, args.status);
    }

  ccs::base::ChannelAccessClient_Impl* self = (ccs::base::ChannelAccessClient_Impl*) args.usr;
  bool found = false;
  
  for (uint_t index = 0; (status && (index < (self->m_var_table)->GetSize())); index += 1)
    {

      ccs::base::ChannelAccessClient_Impl::VariableInfo_t varInfo;

      if ((self->m_var_table)->GetValue(varInfo, index) != STATUS_SUCCESS)
	{
	  log_warning("%s - LUTable<>::GetValue failed", __FUNCTION__);
	  continue;
	}
      
      char name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

      if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS)
	{
	  log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
	  break;
	}

      if (varInfo.channel == args.chid)
	{
	  log_debug("Notification received for '%s' channel", (char*) name);
#if 0
	  ccs::HelperTools::SetAttributeValue(&(varInfo.value), name, (void*) args.dbr); // Copy to cache
#else
	  memcpy(varInfo.reference, (void*) args.dbr, (varInfo.__type)->GetSize());
#endif
	  // Invoke callback, if any
	  if (NULL != varInfo.cb)
	    {
	      log_debug("%s - Invoke callback for '%s' channel", __FUNCTION__, name);
	      varInfo.cb(name, *varInfo.value);
	    }

	  if (varInfo.type == DBR_STRING) log_debug("Variable '%s' holds '%s'", name, (char*) args.dbr);
	  if ((varInfo.type == DBR_CHAR) && (varInfo.mult > 1)) log_debug("Variable '%s' holds '%s'", name, (char*) args.dbr);

	  found = true; 
	  break; 
	}
      else
	{
	  continue;
	}

    }

  if (found == false)
    {
      log_warning("Notification received for unknown channel");
    }

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void ChannelAccessInterface_Thread_PRBL (ccs::base::ChannelAccessClient_Impl* self)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  // Create variable cache
  self->m_value = new (std::nothrow) ccs::types::AnyValue (self->m_type);

  // CA context
  log_info("Create CA context");
  ca_context_create(ca_disable_preemptive_callback);

  log_info("Create CA channels");
  for (uint_t index = 0u; index < (self->m_var_table)->GetSize(); index += 1u)
    {

      ccs::types::string name;

      if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS)
	{
	  log_warning("%s - LUTable<>::GetKeyword failed", __FUNCTION__);
	  continue;
	}
  
      ccs::base::ChannelAccessClient_Impl::VariableInfo_t varInfo; 
      (self->m_var_table)->GetValue(varInfo, index);

      // Store cache reference
      varInfo.reference = ccs::HelperTools::GetAttributeReference(self->m_value, name);
      varInfo.value = new (std::nothrow) ccs::types::AnyValue (varInfo.__type, varInfo.reference);
  
      // Connect to channel
      if (ca_create_channel(name, NULL, NULL, 10, &(varInfo.channel)) != ECA_NORMAL)
	{
	  log_error("%s - ca_create_channel failed", __FUNCTION__);
	  continue;
	}
  
      log_debug("Trying and connect to '%s' channel", name);

      // Wait for connections
      if (ca_pend_io(1) != ECA_NORMAL)
	{
#if 0
	  log_error("%s - ca_pend_io failed", __FUNCTION__);
	  continue;
#endif
	}

      // Verify channel
      if (ca_state(varInfo.channel) != cs_conn) 
	{
	  log_warning("Connection to channel '%s' has not been successful", name);
	}
      else
	{
	  log_debug("Connection to channel '%s' has been successfully verified", name);
	  varInfo.connected = true;
	}

      // Install subscription for INPUT or ANY variable
      if (varInfo.direction != ccs::types::OutputVariable)
	{ // Element count set to '0' to support arrays
	  if (ca_create_subscription(varInfo.type, 0, varInfo.channel, DBE_VALUE, &ChannelAccessInterface_Get_CB, (void*) self, NULL) != ECA_NORMAL)
	    {
	      log_error("%s - ca_create_subscription failed", __FUNCTION__);
	      continue;
	    }
	  else
	    {
	      log_debug("Subscription to channel '%s' has been successfully created", name);
	    }
	}
      
      // Store for future use
      (self->m_var_table)->SetValue(varInfo, index);

    }

  self->m_initialized = true;

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void ChannelAccessInterface_Thread_CB (ccs::base::ChannelAccessClient_Impl* self)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  bool status = self->m_initialized;

  for (uint_t index = 0; (status && (index < (self->m_var_table)->GetSize())); index += 1)
    {

      ccs::base::ChannelAccessClient_Impl::VariableInfo_t varInfo;
      (self->m_var_table)->GetValue(varInfo, index);

      if ((varInfo.direction == ccs::types::InputVariable) || (varInfo.update != true)) // Inputs are managed through notification - Proceed only for OUTPUT or ANY variable which require update
	{
	  continue; // Nothing to do for this channel
	}

      char name [STRING_MAX_LENGTH] = STRING_UNDEFINED;

      if ((self->m_var_table)->GetKeyword(name, index) != STATUS_SUCCESS)
	{
	  // Don't care, just for display purposes
	}
	  
      if (ca_state(varInfo.channel) != cs_conn)
	{
	  if (varInfo.connected == true) log_warning("%s - Connection to channel '%d %s' has been lost", __FUNCTION__, index, name);
	  varInfo.connected = false; (self->m_var_table)->SetValue(varInfo, index);
	  continue;
	}
      else
	{
	  if (varInfo.connected == false) log_info("%s - Connection to channel '%d %s' has been re-established", __FUNCTION__, index, name);
	  varInfo.connected = true;
	}
#if 0 // Inputs are managed through notification - Always true if control reaches this section of the code
      if ((varInfo.direction != ccs::types::InputVariable) && (varInfo.update == true)) // Proceed only for OUTPUT or ANY variable which require update
#endif
	{
	  log_debug("%s - Channel '%s' needs update", __FUNCTION__, name);

	  if (varInfo.type == DBR_STRING) log_debug("Variable '%s' holds '%s'", name, (char*) varInfo.reference);
	  if ((varInfo.type == DBR_CHAR) && (varInfo.mult > 1)) log_debug("Variable '%s' holds '%s'", name, (char*) varInfo.reference);

	  varInfo.update = false;
	  
	  if (ca_array_put(varInfo.type, varInfo.mult, varInfo.channel, varInfo.reference) != ECA_NORMAL) // Copy from cache
	    {
	      log_warning("%s - ca_array_put '%s' failed", __FUNCTION__, name);
	    }
	  else
	    {
	      log_debug("Update CA record '%s'", name);
	    }
	}
#if 0 // Not necessary anymore - Replaced with a global ca_poll below
      // Flush the request
      if (ca_pend_io(1) != ECA_NORMAL)
	{
	  log_warning("%s - ca_pend_io failed", __FUNCTION__);
	}
#endif
      // Store value for later use - Only the update request flag which has been cleared
      (self->m_var_table)->SetValue(varInfo, index);

    }

  // Let CA perform any necessary background activity
  if (ca_poll() != ECA_TIMEOUT)
    {
      log_warning("%s - ca_poll failed", __FUNCTION__);
    }
      
  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void ChannelAccessInterface_Thread_POST (ccs::base::ChannelAccessClient_Impl* self)
{

  return;

}

namespace ccs {

namespace base {

// Initializer methods
 
bool ChannelAccessClient::AddVariable (const char* name, ccs::types::DirIdentifier direction, const std::shared_ptr<const ccs::types::AnyType>& type)
{
  return __impl->AddVariable(name, direction, type);
}

bool ChannelAccessClient::AddVariable (const char* name, bool isInput, ccs::types::uint32 type, ccs::types::uint32 mult)
{
  return __impl->AddVariable(name, isInput, static_cast<chtype>(type), mult);
}

bool ChannelAccessClient_Impl::AddVariable (const char* name, ccs::types::DirIdentifier direction, const std::shared_ptr<const ccs::types::AnyType>& type)
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
      varInfo.type = ccs::HelperTools::AnyTypeToCAScalar(type);
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
      status = (this->m_type)->AddAttribute(name, varInfo.__type); // Type definition for the variable cache
    }

  if (status)
    {
      status = (this->m_var_table)->AddPair(name, varInfo);
    }

  return status;

}

bool ChannelAccessClient_Impl::AddVariable (const char* name, bool isInput, chtype type, ccs::types::uint32 mult)
{

  VariableInfo_t varInfo;

  varInfo.update = false;
  varInfo.direction = ((isInput == true) ? ccs::types::AnyputVariable : ccs::types::OutputVariable);
  ccs::HelperTools::SafeStringCopy(varInfo.name, name, sizeof(ccs::types::string));

  bool status = ((static_cast<ccs::types::CompoundType*>(NULL) != this->m_type) &&
		 (static_cast<LUTable<VariableInfo_t>*>(NULL) != this->m_var_table));

  if (status)
    {
      varInfo.type = type;
      varInfo.mult = mult;
      varInfo.__type = ccs::HelperTools::CAScalarToAnyType(type, mult);
    }

  if (status)
    {
      status = (this->m_type)->AddAttribute(name, varInfo.__type); // Type definition for the variable cache
    }

  if (status)
    {
      status = (this->m_var_table)->AddPair(name, varInfo);
    }

  return status;

}

void ChannelAccessClient_Impl::Initialize (void) 
{ 

  // Initialize resources
  this->m_sleep = DEFAULT_CAINTERFACE_THREAD_PERIOD;
  this->m_var_table = new LUTable<VariableInfo_t> (MAXIMUM_VARIABLE_NUM); // The table will be filled with application-specific variable list
  this->m_initialized = false;

  this->m_type = new (std::nothrow) ccs::types::CompoundType ("caif::VariableCache_t");
  this->m_value = static_cast<ccs::types::AnyValue*>(NULL);

  this->m_thread = new ccs::base::AnyThread ("CA Interface"); 
  (this->m_thread)->SetPeriod(this->m_sleep); (this->m_thread)->SetAccuracy(this->m_sleep);
  (this->m_thread)->SetPreamble((void (*)(void*)) &ChannelAccessInterface_Thread_PRBL, (void*) this); 
  (this->m_thread)->SetCallback((void (*)(void*)) &ChannelAccessInterface_Thread_CB, (void*) this); 
  (this->m_thread)->SetPostface((void (*)(void*)) &ChannelAccessInterface_Thread_POST, (void*) this); 
  //(this->m_thread)->Launch(); // Should be performed after the variable table is populated

  return;

}

bool ChannelAccessClient::Launch (void) { return __impl->Launch(); }
bool ChannelAccessClient_Impl::Launch (void) { return ((this->m_thread)->Launch() == STATUS_SUCCESS); } // Should be called after the variable table is populated

// Accessor methods

bool ChannelAccessClient::IsValid (const char* name) const { return __impl->IsValid(name); }

bool ChannelAccessClient_Impl::IsValid (uint_t id) const { return ((this->m_var_table)->IsValid(id) == STATUS_SUCCESS); }
bool ChannelAccessClient_Impl::IsValid (const char* name) const { return ((this->m_var_table)->IsValid(name) == STATUS_SUCCESS); }

uint_t ChannelAccessClient_Impl::GetVariableId (const char* name) const { uint_t id = 0; (this->m_var_table)->GetIndex(name, id); return id; }

ccs::types::AnyValue* ChannelAccessClient::GetVariable (const char* name) const { return __impl->GetVariable(__impl->GetVariableId(name)); }

ccs::types::AnyValue* ChannelAccessClient_Impl::GetVariable (uint_t id) const { VariableInfo_t varInfo; varInfo.value = static_cast<ccs::types::AnyValue*>(NULL); if (this->IsValid(id)) (this->m_var_table)->GetValue(varInfo, id); return varInfo.value; }
ccs::types::AnyValue* ChannelAccessClient_Impl::GetVariable (const char* name) const { return this->GetVariable(this->GetVariableId(name)); }

std::shared_ptr<const ccs::types::AnyType> ChannelAccessClient_Impl::GetVariableType (uint_t id) const { std::shared_ptr<const ccs::types::AnyType> type; if (this->IsValid(id)) type = this->GetVariable(id)->GetType(); return type; }
std::shared_ptr<const ccs::types::AnyType> ChannelAccessClient_Impl::GetVariableType (const char* name) const { return this->GetVariableType(this->GetVariableId(name)); }

bool ChannelAccessClient::UpdateVariable (const char* name) { return __impl->UpdateVariable(__impl->GetVariableId(name)); }

bool ChannelAccessClient_Impl::UpdateVariable (uint_t id) { bool status = this->IsValid(id); if (status) { VariableInfo_t varInfo; (this->m_var_table)->GetValue(varInfo, id); varInfo.update = true; (this->m_var_table)->SetValue(varInfo, id); } return status; }
bool ChannelAccessClient_Impl::UpdateVariable (const char* name) { return this->UpdateVariable(this->GetVariableId(name)); }

bool ChannelAccessClient::SetCallback (const char* name, std::function<void(const char*, const ccs::types::AnyValue&)> cb) { return __impl->SetCallback(name, cb); }

bool ChannelAccessClient_Impl::SetCallback (ccs::types::uint32 id, std::function<void(const char*, const ccs::types::AnyValue&)> cb) { VariableInfo_t varInfo; bool status = this->IsValid(id); if (status) { (this->m_var_table)->GetValue(varInfo, id); varInfo.cb = cb; (this->m_var_table)->SetValue(varInfo, id); } return status; }
bool ChannelAccessClient_Impl::SetCallback (const char* name, std::function<void(const char*, const ccs::types::AnyValue&)> cb) { return this->SetCallback(this->GetVariableId(name), cb); }

// Constructor methods

ChannelAccessClient::ChannelAccessClient (void)
{ 

  // Instantiate implementation class
  __impl = new (std::nothrow) ChannelAccessClient_Impl;

  bool status = (static_cast<ChannelAccessClient_Impl*>(NULL) != __impl);

  if (!status)
    {
      log_error("Unable to instantiate mplementation class");
    }

  return; 

}

ChannelAccessClient_Impl::ChannelAccessClient_Impl (void)
{ 

  // Initialize resources
  this->Initialize();

  // Register instance in object database - Necessary for globally scoped accessors
  AnyObject* p_ref = (AnyObject*) this; ccs::base::GlobalObjectDatabase::Register(DEFAULT_CAINTERFACE_INSTANCE_NAME, p_ref); 

}

// Destructor method

ChannelAccessClient::~ChannelAccessClient (void)
{ 

  if (static_cast<ChannelAccessClient_Impl*>(NULL) != __impl)
    { 
      delete __impl; 
    }

  __impl= static_cast<ChannelAccessClient_Impl*>(NULL);

  return;

}

ChannelAccessClient_Impl::~ChannelAccessClient_Impl (void)
{ 

  // Release resources
  if (this->m_thread != NULL) delete this->m_thread;
  if (this->m_var_table != NULL) delete this->m_var_table;

  // Remove instance from object database
  ccs::base::GlobalObjectDatabase::Remove(DEFAULT_CAINTERFACE_INSTANCE_NAME); 

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

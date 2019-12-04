/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessClient.cpp $
* $Id: PVAccessClient.cpp 96477 2019-01-09 14:53:55Z sivecm $
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

#include <new> // std::nothrow

#include <pv/pvData.h>
#include <pva/client.h>

#include <pv/pvTimeStamp.h>
#include <pv/standardPVField.h>

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

#include "AnyValueToPVA.h" // .. associated helper routines
#include "PVAToAnyValue.h" // .. associated helper routines

#include "PVAccessClient.h" // This class definition

// Constants

#define DEFAULT_PVAINTERFACE_THREAD_PERIOD 10000000ul // 100Hz
#define DEFAULT_PVAINTERFACE_INSTANCE_NAME "pvac-if"

#define MAXIMUM_VARIABLE_NUM 256

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "pvac-if"

#ifndef log_trace
#define log_trace(arg_msg...) {}
#endif

// Type definition

namespace ccs {

namespace base {

class PVAccessClient_Impl : public AnyObject
{

  private:

    uint64_t m_sleep;

    ccs::base::AnyThread* m_thread;

    // Initializer methods
    void Initialize (void);

  protected:

  public:

    pvac::ClientProvider* channelProvider;

    bool m_initialized;

    typedef struct VariableInfo {

      bool update;
      ccs::types::DirIdentifier direction;
      
      ccs::types::string name;
      ccs::types::AnyValue* value; // Introspectable structured variable
      std::shared_ptr<epics::pvData::PVStructure> pvvalue; // Equivalent PVA introspectable structure
      pvac::ClientChannel* channel; 

    } VariableInfo_t;

    LUTable<VariableInfo_t>* m_var_table;

    // Initializer methods
    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyType* type);
    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyValue* value);

    bool SetPeriod (ccs::types::uint64 period);
    bool Launch (void); // Should be called after the variable table is populated

    // Accessor methods
    bool IsValid (uint_t id) const;
    bool IsValid (const char* name) const;

    ccs::types::uint32 GetVariableId (const char* name) const;

    ccs::types::AnyValue* GetVariable (ccs::types::uint32 id) const;
    ccs::types::AnyValue* GetVariable (const char* name) const;

    std::shared_ptr<const ccs::types::AnyType> GetVariableType (ccs::types::uint32 id) const;
    std::shared_ptr<const ccs::types::AnyType> GetVariableType (const char* name) const;

    void UpdateVariable (ccs::types::uint32 id);
    void UpdateVariable (const char* name);

    // Miscellaneous methods

    // Constructor methods
    PVAccessClient_Impl (void);

    // Destructor method
    virtual ~PVAccessClient_Impl (void);

};

struct PutCallback : public pvac::ClientChannel::PutCallback
{

  ccs::base::PVAccessClient_Impl::VariableInfo_t __variable;
  pvac::Operation* __oper;
  bool __done;
  bool __stat;

  PutCallback (ccs::base::PVAccessClient_Impl::VariableInfo_t& variable);
  virtual ~PutCallback (void);

  bool Put (void); // Synchronous put operation

  // The callbacks
  virtual void putBuild(const std::shared_ptr<const epics::pvData::Structure>& build, pvac::ClientChannel::PutCallback::Args& args);
  virtual void putDone(const pvac::PutEvent& evt);

};

// Global variables

namespace PVAccessInterface {

PVAccessClient* __pvac_if = static_cast<PVAccessClient*>(NULL);

} // namespace PVAccessInterface

// Function declaration

// Function definition

PutCallback::PutCallback (ccs::base::PVAccessClient_Impl::VariableInfo_t& variable)
{

  __done = false;
  __stat = false;
  __oper = static_cast<pvac::Operation*>(NULL);
  __variable = variable;

  return;

}

PutCallback::~PutCallback (void) 
{
 
  if (static_cast<pvac::Operation*>(NULL) != __oper) 
    {
      __oper->cancel(); 
      while (static_cast<pvac::Operation*>(NULL) != __oper) ccs::HelperTools::SleepFor(10000ul);
    }

  return;

}

bool PutCallback::Put (void) // Synchronous put operation
{

  bool status = (static_cast<pvac::Operation*>(NULL) == __oper);

  if (status)
    {
      __oper = new (std::nothrow) pvac::Operation ((__variable.channel)->put(this));
      status = (static_cast<pvac::Operation*>(NULL) != __oper);
    }

  if (status)
    {
      while (false == __done) ccs::HelperTools::SleepFor(10000ul);
      status = (true == __stat);
    }

  delete __oper;

  __done = false;
  __stat = false;
  __oper = static_cast<pvac::Operation*>(NULL);

  return status;

}

void PutCallback::putBuild(const std::shared_ptr<const epics::pvData::Structure>& build, pvac::ClientChannel::PutCallback::Args& args)
{

  // Arguments are :
  //   - build, the type returned for the channel from the server .. we already have it in case the channel was added to the cache without type specification
  //   - args, the structure to write

  //std::shared_ptr<epics::pvData::PVStructure> pvvalue = epics::pvData::getPVDataCreate()->createPVStructure(ccs::HelperTools::AnyTypeToPVStruct((__variable.value)->GetType()));
  //std::shared_ptr<epics::pvData::PVStructure> pvvalue = epics::pvData::getPVDataCreate()->createPVStructure(build);

  args.root = __variable.pvvalue;
  args.tosend.set(0);

  return;

}

void PutCallback::putDone(const pvac::PutEvent& evt)
{

  __stat = false;

  switch(evt.event) 
    {
      case pvac::PutEvent::Fail:
	std::cerr<<__oper->name()<<" Error: "<<evt.message<<"\n";
	break;
      case pvac::PutEvent::Cancel:
	std::cerr<<__oper->name()<<" Cancelled\n";
	break;
      case pvac::PutEvent::Success:
	std::cout<<__oper->name()<<" Done\n";
	__stat = true;
	break;
    }

  __done = true;

  return;

}

} // namespace base

} // namespace ccs

void PVAccessInterface_Thread_PRBL (ccs::base::PVAccessClient_Impl* self)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  // PVA context
  log_info("Create PVA context");
  self->channelProvider = new (std::nothrow) pvac::ClientProvider ("pva");

  bool status = (static_cast<pvac::ClientProvider*>(NULL) != self->channelProvider);

  log_info("Create PVA channels");
  for (ccs::types::uint32 index = 0u; (status && (index < (self->m_var_table)->GetSize())); index += 1u)
    {

      ccs::base::PVAccessClient_Impl::VariableInfo_t varInfo; 
      (self->m_var_table)->GetValue(varInfo, index);

      varInfo.channel = new (std::nothrow) pvac::ClientChannel ((self->channelProvider)->connect(varInfo.name));

      status = (static_cast<pvac::ClientChannel*>(NULL) != varInfo.channel);

      if (static_cast<ccs::types::AnyValue*>(NULL) != varInfo.value)
	{
	  varInfo.pvvalue = epics::pvData::getPVDataCreate()->createPVStructure(ccs::HelperTools::AnyTypeToPVStruct((varInfo.value)->GetType()));
	  status = (varInfo.pvvalue ? true : false);
	}
      else
	{
	  // Will have to determine the type from the type returned by the get() operation
	  try
	    {
	      // Retrieve channel type
	      log_info("Try and retrieve channel '%s' type ..", varInfo.name);
	      varInfo.pvvalue = std::const_pointer_cast<epics::pvData::PVStructure>((varInfo.channel)->get());
	      varInfo.value = new (std::nothrow) ccs::types::AnyValue (ccs::HelperTools::PVStructToAnyType((varInfo.pvvalue)->getStructure()));
	    }
	  catch (std::exception& e)
	    {
	      //status = false;
	      log_warning(".. exception caught");
	      continue;
	    }
	}

      // Store for future use
      (self->m_var_table)->SetValue(varInfo, index);

    }

  self->m_initialized = true;

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void PVAccessInterface_Thread_CB (ccs::base::PVAccessClient_Impl* self)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  bool status = self->m_initialized;

  for (ccs::types::uint32 index = 0; (status && (index < (self->m_var_table)->GetSize())); index += 1)
    {

      ccs::base::PVAccessClient_Impl::VariableInfo_t varInfo;
      (self->m_var_table)->GetValue(varInfo, index);

      if ((varInfo.direction != ccs::types::InputVariable) && varInfo.update)
	{

	  log_info("Update PVA record '%s'", varInfo.name);

	  status = ccs::HelperTools::AnyValueToPVStruct(varInfo.value, varInfo.pvvalue);

	  if (status)
	    {
	      ccs::base::PutCallback cb (varInfo);
	      status = cb.Put();
	    }

	  varInfo.update = false;

	  // Store for future use
	  (self->m_var_table)->SetValue(varInfo, index);

	}

      if (varInfo.direction != ccs::types::OutputVariable)
	{
	  log_debug("Read PVA record '%s' ..", varInfo.name);

	  try
	    {
	      // Update variable cache
	      varInfo.pvvalue = std::const_pointer_cast<epics::pvData::PVStructure>((varInfo.channel)->get());

	      status = ccs::HelperTools::PVStructToAnyValue(varInfo.value, varInfo.pvvalue);

	      if (status)
		{
		  char buffer [1024];
		  (varInfo.value)->SerialiseInstance(buffer, 1024u);
		  log_debug(".. '%s'", buffer);
		}
	      else
		{
		  log_warning("ccs::HelperTools::PVStructToAnyValue failed for '%s'", varInfo.name);
		}
	    }
	  catch (std::exception& e)
	    {
	      //status = false;
	      continue;
	    }
	}

    }

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void PVAccessInterface_Thread_POST (ccs::base::PVAccessClient_Impl* self)
{

  return;

}

namespace ccs {

namespace base {

// Initializer methods
 
bool PVAccessClient::AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyType* type)
{
  return __impl->AddVariable(name, direction, type);
}

bool PVAccessClient_Impl::AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyType* type)
{

  VariableInfo_t varInfo;

  varInfo.update = false;
  varInfo.direction = direction;
  ccs::HelperTools::SafeStringCopy(varInfo.name, name, sizeof(ccs::types::string));

  bool status = (static_cast<const ccs::types::AnyType*>(NULL) != type);

  if (status)
    {
      varInfo.value = new (std::nothrow) ccs::types::AnyValue (type);
      status = (static_cast<ccs::types::AnyValue*>(NULL) != varInfo.value);
    }
  else
    {
      varInfo.value = static_cast<ccs::types::AnyValue*>(NULL);
      status = true;
    }

  if (status)
    {
      status = (this->m_var_table)->AddPair(name, varInfo);
    }

  return status;

}

bool PVAccessClient_Impl::AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyValue* value)
{

  VariableInfo_t varInfo;

  varInfo.update = false;
  varInfo.direction = direction;
  ccs::HelperTools::SafeStringCopy(varInfo.name, name, sizeof(ccs::types::string));
  varInfo.value = const_cast<ccs::types::AnyValue*>(value);

  return (this->m_var_table)->AddPair(name, varInfo);

}

void PVAccessClient_Impl::Initialize (void) 
{ 

  // Initialize resources
  this->m_sleep = DEFAULT_PVAINTERFACE_THREAD_PERIOD;
  this->m_var_table = new LUTable<VariableInfo_t> (MAXIMUM_VARIABLE_NUM); // The table will be filled with application-specific variable list
  this->m_initialized = false;

  this->m_thread = new ccs::base::AnyThread ("PVA Interface"); 
  (this->m_thread)->SetPeriod(this->m_sleep); (this->m_thread)->SetAccuracy(this->m_sleep);
  (this->m_thread)->SetPreamble((void (*)(void*)) &PVAccessInterface_Thread_PRBL, (void*) this); 
  (this->m_thread)->SetCallback((void (*)(void*)) &PVAccessInterface_Thread_CB, (void*) this); 
  (this->m_thread)->SetPostface((void (*)(void*)) &PVAccessInterface_Thread_POST, (void*) this); 
  //(this->m_thread)->Launch(); // Should be performed after the variable table is populated

  return;

}

bool PVAccessClient::SetPeriod (ccs::types::uint64 period) { return __impl->SetPeriod(period); }
bool PVAccessClient_Impl::SetPeriod (ccs::types::uint64 period) { return ((this->m_thread)->SetPeriod(period) == STATUS_SUCCESS); }

bool PVAccessClient::Launch (void) { return __impl->Launch(); }
bool PVAccessClient_Impl::Launch (void) { return ((this->m_thread)->Launch() == STATUS_SUCCESS); } // Should be called after the variable table is populated

// Accessor methods

bool PVAccessClient::IsValid (const char* name) const { return __impl->IsValid(name); }

bool PVAccessClient_Impl::IsValid (ccs::types::uint32 id) const { return ((this->m_var_table)->IsValid(id) == STATUS_SUCCESS); }
bool PVAccessClient_Impl::IsValid (const char* name) const { return ((this->m_var_table)->IsValid(name) == STATUS_SUCCESS); }

ccs::types::uint32 PVAccessClient_Impl::GetVariableId (const char* name) const { ccs::types::uint32 id = 0; (this->m_var_table)->GetIndex(name, id); return id; }

ccs::types::AnyValue* PVAccessClient::GetVariable (const char* name) const { return __impl->GetVariable(__impl->GetVariableId(name)); }

ccs::types::AnyValue* PVAccessClient_Impl::GetVariable (ccs::types::uint32 id) const { VariableInfo_t varInfo; varInfo.value = static_cast<ccs::types::AnyValue*>(NULL); if (this->IsValid(id)) (this->m_var_table)->GetValue(varInfo, id); return varInfo.value; }
ccs::types::AnyValue* PVAccessClient_Impl::GetVariable (const char* name) const { return this->GetVariable(this->GetVariableId(name)); }

std::shared_ptr<const ccs::types::AnyType> PVAccessClient_Impl::GetVariableType (ccs::types::uint32 id) const { std::shared_ptr<const ccs::types::AnyType> type; if (this->IsValid(id)) type = this->GetVariable(id)->GetType(); return type; };
std::shared_ptr<const ccs::types::AnyType> PVAccessClient_Impl::GetVariableType (const char* name) const { return this->GetVariableType(this->GetVariableId(name)); };

void PVAccessClient::UpdateVariable (const char* name) { return __impl->UpdateVariable(__impl->GetVariableId(name)); return; }

void PVAccessClient_Impl::UpdateVariable (ccs::types::uint32 id) { VariableInfo_t varInfo; if (this->IsValid(id)) { (this->m_var_table)->GetValue(varInfo, id); varInfo.update = true; (this->m_var_table)->SetValue(varInfo, id); } return; }
void PVAccessClient_Impl::UpdateVariable (const char* name) { return this->UpdateVariable(this->GetVariableId(name)); return; }

// Constructor methods

PVAccessClient::PVAccessClient (void)
{ 

  // Instantiate implementation class
  __impl = new (std::nothrow) PVAccessClient_Impl;

  bool status = (static_cast<PVAccessClient_Impl*>(NULL) != __impl);

  if (!status)
    {
      log_error("Unable to instantiate mplementation class");
    }

  return; 

}

PVAccessClient_Impl::PVAccessClient_Impl (void)
{ 

  // Initialize resources
  this->Initialize();

  // Register instance in object database - Necessary for globally scoped accessors
  AnyObject* p_ref = (AnyObject*) this; ccs::base::GlobalObjectDatabase::Register(DEFAULT_PVAINTERFACE_INSTANCE_NAME, p_ref); 

}

// Destructor method

PVAccessClient::~PVAccessClient (void)
{ 

  if (static_cast<PVAccessClient_Impl*>(NULL) != __impl)
    { 
      delete __impl; 
    }

  __impl= static_cast<PVAccessClient_Impl*>(NULL);

  return;

}

PVAccessClient_Impl::~PVAccessClient_Impl (void)
{ 

  // Release resources
  if (this->m_thread != NULL) delete this->m_thread;
  if (this->m_var_table != NULL) delete this->m_var_table;

  // Remove instance from object database
  ccs::base::GlobalObjectDatabase::Remove(DEFAULT_PVAINTERFACE_INSTANCE_NAME); 

}

} // namespace base

} // namespace ccs

#undef LOG_ALTERN_SRC

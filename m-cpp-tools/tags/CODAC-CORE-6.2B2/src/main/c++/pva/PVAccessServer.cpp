/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessServer.cpp $
* $Id: PVAccessServer.cpp 98392 2019-03-13 20:09:51Z zagara $
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

#include <pv/pvData.h>
#include <pv/pvDatabase.h>
#include <pv/serverContext.h>
#include <pv/channelProviderLocal.h>

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

#include "PVAccessServer.h" // This class definition

// Constants

#define DEFAULT_PVAINTERFACE_THREAD_PERIOD 10000000ul // 100Hz
#define DEFAULT_PVAINTERFACE_INSTANCE_NAME "pvas-if"

#define MAXIMUM_VARIABLE_NUM 256

#undef LOG_ALTERN_SRC
#define LOG_ALTERN_SRC "pvas-if"

#ifndef log_trace
#define log_trace(arg_msg...) {}
#endif

// Type definition

namespace ccs {

namespace base {

class Record : public epics::pvDatabase::PVRecord
{

  public:
    POINTER_DEFINITIONS(Record);

    static std::shared_ptr<Record> create (std::string const & recordName, std::shared_ptr<epics::pvData::PVStructure> const & pvStructure);

    Record (std::string const & recordName, std::shared_ptr<epics::pvData::PVStructure> const & pvStructure)
      : epics::pvDatabase::PVRecord(recordName, pvStructure) {};

    virtual void process (void);

};

class PVAccessServer_Impl : public AnyObject
{

  private:

    uint64_t m_sleep;

    ccs::base::AnyThread* m_thread;

    // Initializer methods
    void Initialize (void);

  protected:

  public:

    // To keep the instances live as long as this class lives
    epics::pvDatabase::PVDatabasePtr master;
    epics::pvDatabase::ChannelProviderLocalPtr channelProvider;
    epics::pvAccess::ServerContext::shared_pointer context;

    bool m_initialized;

    typedef struct VariableInfo {

      bool update;
      ccs::types::DirIdentifier direction;

      std::function<void(const ccs::types::AnyValue&)> cb;

      ccs::types::string name;
      ccs::types::AnyValue* value; // Introspectable structured variable
      
      // Done - INPUT or ANY record should implement a specialised process method
      //std::shared_ptr<epics::pvDatabase::PVRecord> pvrecord;
      std::shared_ptr<ccs::base::Record> pvrecord;
      std::shared_ptr<epics::pvData::PVStructure> pvvalue;
      
    } VariableInfo_t;

    LUTable<VariableInfo_t>* m_var_table;

    // Initializer methods
    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyType* type);
    bool AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyValue* value);
    bool SetPeriod (ccs::types::uint64 period);
    bool Launch (void); // Should be called after the variable table is populated

    // Accessor methods
    bool IsValid (ccs::types::uint32 id) const;
    bool IsValid (const char* name) const;

    ccs::types::uint32 GetVariableId (const char* name) const;

    ccs::types::AnyValue* GetVariable (ccs::types::uint32 id) const;
    ccs::types::AnyValue* GetVariable (const char* name) const;

    std::shared_ptr<const ccs::types::AnyType> GetVariableType (ccs::types::uint32 id) const;
    std::shared_ptr<const ccs::types::AnyType> GetVariableType (const char* name) const;

    void UpdateVariable (ccs::types::uint32 id);
    void UpdateVariable (const char* name);

    // Miscellaneous methods
    bool SetCallback (ccs::types::uint32 id, std::function<void(const ccs::types::AnyValue&)> cb);
    bool SetCallback (const char* name, std::function<void(const ccs::types::AnyValue&)> cb);

    // Constructor methods
    PVAccessServer_Impl (void);

    // Destructor method
    virtual ~PVAccessServer_Impl (void);

};

// Global variables

namespace PVAccessInterface {

PVAccessServer* __pvas_if = static_cast<PVAccessServer*>(NULL);

} // namespace PVAccessInterface

} // namespace base

} // namespace ccs

// Function declaration

// Function definition

void PVAInterface_Thread_PRBL (ccs::base::PVAccessServer_Impl* self)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  // PVA context
  log_info("Create PVA context");
  self->master = epics::pvDatabase::PVDatabase::getMaster();
  self->channelProvider = epics::pvDatabase::getChannelProviderLocal();

  log_info("Create PVA channels");
  for (ccs::types::uint32 index = 0u; index < (self->m_var_table)->GetSize(); index += 1u)
    {

      ccs::base::PVAccessServer_Impl::VariableInfo_t varInfo; 
      (self->m_var_table)->GetValue(varInfo, index);

      varInfo.pvvalue = epics::pvData::getPVDataCreate()->createPVStructure(ccs::HelperTools::AnyTypeToPVStruct((varInfo.value)->GetType()));
      //varInfo.pvrecord = epics::pvDatabase::PVRecord::create(std::string(varInfo.name), varInfo.pvvalue);
      varInfo.pvrecord = ccs::base::Record::create(std::string(varInfo.name), varInfo.pvvalue);
      log_info("Add PVA record '%s'", varInfo.name);
      (self->master)->addRecord(varInfo.pvrecord);
      varInfo.pvrecord->setTraceLevel(2);

      // Store for future use
      (self->m_var_table)->SetValue(varInfo, index);

    }

  log_info("Start PVA server");
  self->context = epics::pvAccess::startPVAServer(epics::pvAccess::PVACCESS_ALL_PROVIDERS, 0, true, true);
  self->m_initialized = true;

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void PVAInterface_Thread_CB (ccs::base::PVAccessServer_Impl* self)
{

  log_trace("Entering '%s' routine", __FUNCTION__);

  bool status = self->m_initialized;

  for (ccs::types::uint32 index = 0; (status && (index < (self->m_var_table)->GetSize())); index += 1)
    {

      ccs::base::PVAccessServer_Impl::VariableInfo_t varInfo;
      (self->m_var_table)->GetValue(varInfo, index);

      if ((varInfo.direction != ccs::types::InputVariable) && varInfo.update)
	{

	  log_debug("Update PVA record '%s'", varInfo.name);

	  // Update record
	  varInfo.pvrecord->lock();
	  varInfo.pvrecord->beginGroupPut();

	  status = ccs::HelperTools::AnyValueToPVStruct(varInfo.value, varInfo.pvvalue);

	  if (status)
	    {
#ifdef LOG_DEBUG_ENABLE
	      std::cout << "Update PVA record" << std::endl;
	      std::cout << *varInfo.pvrecord << std::endl;
#endif
	    }
	  else
	    {
	      log_warning("ccs::HelperTools::AnyValueToPVStruct failed for '%s'", varInfo.name);
	    }

	  varInfo.pvrecord->process();
	  varInfo.pvrecord->endGroupPut();
	  varInfo.pvrecord->unlock();

	  varInfo.update = false;

	  // Store for future use
	  (self->m_var_table)->SetValue(varInfo, index);

	}

      if ((varInfo.direction != ccs::types::OutputVariable) && varInfo.update)
	{
	  log_debug("Read PVA record '%s'", varInfo.name);

	  // Update record
	  varInfo.pvrecord->lock();

	  status = ccs::HelperTools::PVStructToAnyValue(varInfo.value, varInfo.pvvalue);
#ifdef LOG_DEBUG_ENABLE
	  std::cout << "Read PVA record" << std::endl;
	  std::cout << *varInfo.pvrecord << std::endl;
#endif
	  varInfo.pvrecord->unlock();
	}

    }

  log_trace("Leaving '%s' routine", __FUNCTION__);

  return;

}

void PVAInterface_Thread_POST (ccs::base::PVAccessServer_Impl* self)
{

  return;

}

namespace ccs {

namespace base {

std::shared_ptr<Record> Record::create (std::string const & recordName, std::shared_ptr<epics::pvData::PVStructure> const & pvStructure) 
{ 

  log_trace("Record::create('%s') - Entering method", recordName.c_str());

  std::shared_ptr<Record> pvRecord (new Record (recordName, pvStructure)); 
  
  // Need to be explicitly called .. not part of the base constructor
  if(!pvRecord->init()) pvRecord.reset();

  log_trace("Record::create('%s') - Leaving method", recordName.c_str());

  return pvRecord; 

}

void Record::process (void)
{

  std::string name = this->getRecordName();

  log_trace("Record::process('%s') - Entering method", name.c_str());

  PVAccessServer_Impl* pvaif = dynamic_cast<PVAccessServer_Impl*>(ccs::base::GlobalObjectDatabase::GetInstance(DEFAULT_PVAINTERFACE_INSTANCE_NAME)); 

  bool status = (static_cast<PVAccessServer_Impl*>(NULL) != pvaif);
  
  if (status)
    {
      status = pvaif->IsValid(name.c_str());
    }
  
  ccs::base::PVAccessServer_Impl::VariableInfo_t varInfo; 

  if (status)
    {
      (pvaif->m_var_table)->GetValue(varInfo, name.c_str());
      status = (varInfo.direction != ccs::types::OutputVariable);
    }

  if (status)
    {
      // Update value from record
      log_debug("Record::process('%s') - Update cache", name.c_str());
      status = ccs::HelperTools::PVStructToAnyValue(varInfo.value, varInfo.pvvalue);
    }

  if (status && (NULL != varInfo.cb))
    {
      log_debug("Record::process('%s') - Invoke callback '%s'", name.c_str());
      varInfo.cb(*varInfo.value);
    }

  if (status)
    {
      char buffer [1024] = STRING_UNDEFINED;
      (varInfo.value)->SerialiseInstance(buffer, 1024u);
      log_debug("Record::process('%s') - Read '%s'", name.c_str(), buffer);
    }

  log_trace("Record::process('%s') - leaving method", name.c_str());

  return;

}


// Initializer methods
 
bool PVAccessServer::AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyType* type)
{
  return __impl->AddVariable(name, direction, type);
}

bool PVAccessServer_Impl::AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyType* type)
{

  VariableInfo_t varInfo;

  varInfo.cb = NULL;
  varInfo.update = false;
  varInfo.direction = direction;
  ccs::HelperTools::SafeStringCopy(varInfo.name, name, sizeof(ccs::types::string));
  varInfo.value = new (std::nothrow) ccs::types::AnyValue (type);

  return (this->m_var_table)->AddPair(name, varInfo);

}

bool PVAccessServer_Impl::AddVariable (const char* name, ccs::types::DirIdentifier direction, const ccs::types::AnyValue* value)
{

  VariableInfo_t varInfo;

  varInfo.update = false;
  varInfo.direction = direction;
  ccs::HelperTools::SafeStringCopy(varInfo.name, name, sizeof(ccs::types::string));
  varInfo.value = const_cast<ccs::types::AnyValue*>(value);

  return (this->m_var_table)->AddPair(name, varInfo);

}

void PVAccessServer_Impl::Initialize (void) 
{ 

  // Initialize resources
  this->m_sleep = DEFAULT_PVAINTERFACE_THREAD_PERIOD;
  this->m_var_table = new LUTable<VariableInfo_t> (MAXIMUM_VARIABLE_NUM); // The table will be filled with application-specific variable list
  this->m_initialized = false;

  this->m_thread = new ccs::base::AnyThread ("PVA Interface"); 
  (this->m_thread)->SetPeriod(this->m_sleep); (this->m_thread)->SetAccuracy(this->m_sleep);
  (this->m_thread)->SetPreamble((void (*)(void*)) &PVAInterface_Thread_PRBL, (void*) this); 
  (this->m_thread)->SetCallback((void (*)(void*)) &PVAInterface_Thread_CB, (void*) this); 
  (this->m_thread)->SetPostface((void (*)(void*)) &PVAInterface_Thread_POST, (void*) this); 
  //(this->m_thread)->Launch(); // Should be performed after the variable table is populated

  return;

}

bool PVAccessServer::SetPeriod (ccs::types::uint64 period) { return __impl->SetPeriod(period); }
bool PVAccessServer_Impl::SetPeriod (ccs::types::uint64 period) { return ((this->m_thread)->SetPeriod(period) == STATUS_SUCCESS); }

bool PVAccessServer::Launch (void) { return __impl->Launch(); }
bool PVAccessServer_Impl::Launch (void) { return ((this->m_thread)->Launch() == STATUS_SUCCESS); }

// Accessor methods

bool PVAccessServer::IsValid (const char* name) const { return __impl->IsValid(name); }

bool PVAccessServer_Impl::IsValid (ccs::types::uint32 id) const { return ((this->m_var_table)->IsValid(id) == STATUS_SUCCESS); }
bool PVAccessServer_Impl::IsValid (const char* name) const { return ((this->m_var_table)->IsValid(name) == STATUS_SUCCESS); }

ccs::types::uint32 PVAccessServer_Impl::GetVariableId (const char* name) const { ccs::types::uint32 id = static_cast<ccs::types::uint32>(-1); (this->m_var_table)->GetIndex(name, id); return id; }

ccs::types::AnyValue* PVAccessServer::GetVariable (const char* name) const { return __impl->GetVariable(__impl->GetVariableId(name)); }

ccs::types::AnyValue* PVAccessServer_Impl::GetVariable (ccs::types::uint32 id) const { VariableInfo_t varInfo; varInfo.value = static_cast<ccs::types::AnyValue*>(NULL); if (this->IsValid(id)) (this->m_var_table)->GetValue(varInfo, id); return varInfo.value; }
ccs::types::AnyValue* PVAccessServer_Impl::GetVariable (const char* name) const { return this->GetVariable(this->GetVariableId(name)); }

std::shared_ptr<const ccs::types::AnyType> PVAccessServer_Impl::GetVariableType (ccs::types::uint32 id) const { std::shared_ptr<const ccs::types::AnyType> type; if (this->IsValid(id)) type = this->GetVariable(id)->GetType(); return type; };
std::shared_ptr<const ccs::types::AnyType> PVAccessServer_Impl::GetVariableType (const char* name) const { return this->GetVariableType(this->GetVariableId(name)); };

void PVAccessServer::UpdateVariable (const char* name) { return __impl->UpdateVariable(__impl->GetVariableId(name)); return; }

void PVAccessServer_Impl::UpdateVariable (ccs::types::uint32 id) { VariableInfo_t varInfo; if (this->IsValid(id)) { (this->m_var_table)->GetValue(varInfo, id); varInfo.update = true; (this->m_var_table)->SetValue(varInfo, id); } return; }
void PVAccessServer_Impl::UpdateVariable (const char* name) { return this->UpdateVariable(this->GetVariableId(name)); return; }

bool PVAccessServer::SetCallback (const char* name, std::function<void(const ccs::types::AnyValue&)> cb) { return __impl->SetCallback(name, cb); }

bool PVAccessServer_Impl::SetCallback (ccs::types::uint32 id, std::function<void(const ccs::types::AnyValue&)> cb) { VariableInfo_t varInfo; bool status = this->IsValid(id); if (status) { (this->m_var_table)->GetValue(varInfo, id); varInfo.cb = cb; (this->m_var_table)->SetValue(varInfo, id); } return status; }
bool PVAccessServer_Impl::SetCallback (const char* name, std::function<void(const ccs::types::AnyValue&)> cb) { return this->SetCallback(this->GetVariableId(name), cb); }

// Constructor methods

PVAccessServer::PVAccessServer (void)
{ 

  // Instantiate implementation class
  __impl = new (std::nothrow) PVAccessServer_Impl;

  bool status = (static_cast<PVAccessServer_Impl*>(NULL) != __impl);

  if (!status)
    {
      log_error("Unable to instantiate mplementation class");
    }

  return; 

}

PVAccessServer_Impl::PVAccessServer_Impl (void)
{ 

  // Initialize resources
  this->Initialize();

  // Register instance in object database - Necessary for globally scoped accessors
  AnyObject* p_ref = (AnyObject*) this; ccs::base::GlobalObjectDatabase::Register(DEFAULT_PVAINTERFACE_INSTANCE_NAME, p_ref); 

}

// Destructor method

PVAccessServer::~PVAccessServer (void)
{ 

  if (static_cast<PVAccessServer_Impl*>(NULL) != __impl)
    { 
      delete __impl; 
    }

  __impl= static_cast<PVAccessServer_Impl*>(NULL);

  return;

}

PVAccessServer_Impl::~PVAccessServer_Impl (void)
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

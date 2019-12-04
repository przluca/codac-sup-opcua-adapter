/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/unit/ObjectHandler-tests.cpp $
* $Id: ObjectHandler-tests.cpp 101473 2019-08-09 05:43:47Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Unit test code
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

// Global header files

#include <gtest/gtest.h> // Google test framework

#include <common/BasicTypes.h> // Misc. type definition
#include <common/SysTools.h> // Misc. helper functions

#include <common/log-api.h> // Syslog wrapper routines

#include <common/AnyObject.h>
#include <common/ObjectDatabase.h>
#include <common/ObjectFactory.h>

// Local header files

#include "ObjectHandler.h"
#include "ObjectHandlerHelper.h"

// Constants

// Type definition

class MyAnyObject : public ccs::base::AnyObject {
  
  private:

  public:
  
    MyAnyObject (void) {};
    virtual ~MyAnyObject (void) {};
  
};
  
class MyCfgableObject : public ccs::base::AnyObject, public  ccs::base::CfgableObject {
  
  private:

    ccs::types::uint32 __value = 0u;

  public:
  
    MyCfgableObject (void) {};
    virtual ~MyCfgableObject (void) {};

    ccs::types::uint32 GetValue (void) { return __value; };

    virtual bool SetParameter (const char* name, const char* value) {

      log_info("MyCfgableObject::SetParameter('%s, %s')", name, value);

      bool status = true;

      std::string __name (name);
 
      if (__name == "incr") __value += 1u; 
      if (__name == "reset") __value = 0u; 
      if (__name == "false") status = false; 
      if (__name == "value")
	{
	  status = (0 < sscanf(value, "%u", &__value));
	} 

      return status;

    };
  
};
  
class MyMsgableObject : public ccs::base::AnyObject, public  ccs::base::MsgableObject {
  
  private:

    ccs::types::uint32 __count = 0u;

  public:
  
    MyMsgableObject (void) {};
    virtual ~MyMsgableObject (void) {};

    ccs::types::uint32 GetCount (void) { return __count; };

    virtual bool ProcessMessage (const char* msg) {

      log_info("MyMsgableObject::ProcessMessage('%s')", msg);

      bool status = true;
 
      if (std::string(msg) == "incr") __count += 1u; 
      if (std::string(msg) == "reset") __count = 0u; 
      if (std::string(msg) == "false") status = false; 

      return status;

    };
  
};
  
class MyAnyableObject : public ccs::base::AnyObject, public  ccs::base::CfgableObject, public  ccs::base::MsgableObject {
  
  private:

    ccs::types::uint32 __count = 0u;
    ccs::types::uint32 __value = 0u;

  public:
  
    MyAnyableObject (void) {};
    virtual ~MyAnyableObject (void) {};

    ccs::types::uint32 GetCount (void) { return __count; };
    ccs::types::uint32 GetValue (void) { return __value; };

    virtual bool SetParameter (const char* name, const char* value) {

      log_info("MyAnyableObject::SetParameter('%s, %s')", name, value);

      bool status = true;

      std::string __name (name);
 
      if (__name == "incr") __value += 1u; 
      if (__name == "reset") __value = 0u; 
      if (__name == "false") status = false; 
      if (__name == "value")
	{
	  status = (0 < sscanf(value, "%u", &__value));
	} 

      return status;

    };
  
    virtual bool ProcessMessage (const char* msg) {

      log_info("MyAnyableObject::ProcessMessage('%s')", msg);

      bool status = true;
 
      std::string __msg (msg);
 
      if (__msg == "incr") __count += 1u; 
      if (__msg == "reset") __count = 0u; 
      if (__msg == "false") status = false; 

      return status;

    };
  
};
  
// Function declaration

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();

REGISTER_CLASS_WITH_OBJECT_FACTORY(MyAnyObject) // Auto-register constructor with the ObjectFactory
REGISTER_CLASS_WITH_OBJECT_FACTORY(MyCfgableObject) // Auto-register constructor with the ObjectFactory
REGISTER_CLASS_WITH_OBJECT_FACTORY(MyMsgableObject) // Auto-register constructor with the ObjectFactory
REGISTER_CLASS_WITH_OBJECT_FACTORY(MyAnyableObject) // Auto-register constructor with the ObjectFactory

// Function definition
  
TEST(ObjectHandler_Test, Constructor)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, IsTypeValid)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->IsTypeValid("MyAnyObject") &&
	     handler->IsTypeValid("MyCfgableObject") &&
	     handler->IsTypeValid("MyMsgableObject") &&
	     handler->IsTypeValid("MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->IsTypeValid(NULL) &&
	     !handler->IsTypeValid("") &&
	     !handler->IsTypeValid("UndefinedType"));
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, Instantiate)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->IsTypeValid("MyAnyObject") &&
	     handler->IsTypeValid("MyCfgableObject") &&
	     handler->IsTypeValid("MyMsgableObject") &&
	     handler->IsTypeValid("MyAnyableObject"));
    }

  if (ret)
    {
      ret = (handler->Instantiate("MyAnyObject","MyAnyObject") &&
	     handler->Instantiate("MyCfgableObject","MyCfgableObject") &&
	     handler->Instantiate("MyMsgableObject","MyMsgableObject") &&
	     handler->Instantiate("MyAnyableObject","MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->Instantiate(NULL,NULL) &&
	     !handler->Instantiate("","UndefinedObject") &&
	     !handler->Instantiate("MyAnyObject",""));
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, IsValid)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->Instantiate("MyAnyObject","MyAnyObject") &&
	     handler->Instantiate("MyCfgableObject","MyCfgableObject") &&
	     handler->Instantiate("MyMsgableObject","MyMsgableObject") &&
	     handler->Instantiate("MyAnyableObject","MyAnyableObject"));
    }

  if (ret)
    {
      ret = (handler->IsValid("MyAnyObject") &&
	     handler->IsValid("MyCfgableObject") &&
	     handler->IsValid("MyMsgableObject") &&
	     handler->IsValid("MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->IsValid(static_cast<const char*>(NULL)) &&
	     !handler->IsValid("") &&
	     !handler->IsValid("UndefinedObject"));
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, IsInstanceOf)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->Instantiate("MyAnyObject","MyAnyObject") &&
	     handler->Instantiate("MyCfgableObject","MyCfgableObject") &&
	     handler->Instantiate("MyMsgableObject","MyMsgableObject") &&
	     handler->Instantiate("MyAnyableObject","MyAnyableObject"));
    }

  if (ret)
    {
      ret = (handler->IsInstanceOf<ccs::base::AnyObject>("MyAnyObject") &&
	     handler->IsInstanceOf<ccs::base::AnyObject>("MyCfgableObject") &&
	     handler->IsInstanceOf<ccs::base::AnyObject>("MyMsgableObject") &&
	     handler->IsInstanceOf<ccs::base::AnyObject>("MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->IsInstanceOf<ccs::base::CfgableObject>("MyAnyObject") &&
	     handler->IsInstanceOf<ccs::base::CfgableObject>("MyCfgableObject") &&
	     !handler->IsInstanceOf<ccs::base::CfgableObject>("MyMsgableObject") &&
	     handler->IsInstanceOf<ccs::base::CfgableObject>("MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->IsInstanceOf<ccs::base::MsgableObject>("MyAnyObject") &&
	     !handler->IsInstanceOf<ccs::base::MsgableObject>("MyCfgableObject") &&
	     handler->IsInstanceOf<ccs::base::MsgableObject>("MyMsgableObject") &&
	     handler->IsInstanceOf<ccs::base::MsgableObject>("MyAnyableObject"));
    }

  if (ret)
    {
      ret = (handler->IsInstanceOf<MyAnyObject>("MyAnyObject") &&
	     handler->IsInstanceOf<MyCfgableObject>("MyCfgableObject") &&
	     handler->IsInstanceOf<MyMsgableObject>("MyMsgableObject") &&
	     handler->IsInstanceOf<MyAnyableObject>("MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->IsInstanceOf<ccs::base::AnyObject>(NULL) &&
	     !handler->IsInstanceOf<ccs::base::AnyObject>("") &&
	     !handler->IsInstanceOf<ccs::base::AnyObject>("UndefinedObject"));
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, GetInstance)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->Instantiate("MyAnyObject","MyAnyObject") &&
	     handler->Instantiate("MyCfgableObject","MyCfgableObject") &&
	     handler->Instantiate("MyMsgableObject","MyMsgableObject") &&
	     handler->Instantiate("MyAnyableObject","MyAnyableObject"));
    }

  if (ret)
    {
      ret = ((static_cast<ccs::base::AnyObject*>(NULL) != handler->GetInstance<ccs::base::AnyObject>("MyAnyObject")) &&
	     (static_cast<ccs::base::AnyObject*>(NULL) != handler->GetInstance<ccs::base::AnyObject>("MyCfgableObject")) &&
	     (static_cast<ccs::base::AnyObject*>(NULL) != handler->GetInstance<ccs::base::AnyObject>("MyMsgableObject")) &&
	     (static_cast<ccs::base::AnyObject*>(NULL) != handler->GetInstance<ccs::base::AnyObject>("MyAnyableObject")));
    }

  if (ret)
    {
      ret = ((static_cast<ccs::base::CfgableObject*>(NULL) == handler->GetInstance<ccs::base::CfgableObject>("MyAnyObject")) &&
	     (static_cast<ccs::base::CfgableObject*>(NULL) != handler->GetInstance<ccs::base::CfgableObject>("MyCfgableObject")) &&
	     (static_cast<ccs::base::CfgableObject*>(NULL) == handler->GetInstance<ccs::base::CfgableObject>("MyMsgableObject")) &&
	     (static_cast<ccs::base::CfgableObject*>(NULL) != handler->GetInstance<ccs::base::CfgableObject>("MyAnyableObject")));
    }

  // Etc.

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, Terminate)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->Instantiate("MyAnyObject","MyAnyObject") &&
	     handler->Instantiate("MyCfgableObject","MyCfgableObject") &&
	     handler->Instantiate("MyMsgableObject","MyMsgableObject") &&
	     handler->Instantiate("MyAnyableObject","MyAnyableObject"));
    }

  if (ret)
    {
      ret = (handler->IsValid("MyAnyObject") &&
	     handler->IsValid("MyCfgableObject") &&
	     handler->IsValid("MyMsgableObject") &&
	     handler->IsValid("MyAnyableObject"));
    }

  // Should have been all regsitered to the GODB

  if (ret)
    {
      ret = ccs::base::GlobalObjectDatabase::IsValid("MyAnyObject"); // Etc.
    }

  if (ret)
    {
      ret = handler->Terminate("MyAnyObject");
    }

  if (ret)
    {
      ret = (!handler->IsValid("MyAnyObject") &&
	     handler->IsValid("MyCfgableObject") &&
	     handler->IsValid("MyMsgableObject") &&
	     handler->IsValid("MyAnyableObject"));
    }

  if (ret)
    {
      delete handler;
    }

  // Should have been all deleted from the GODB

  if (ret)
    {
      ret = !ccs::base::GlobalObjectDatabase::IsValid("MyAnyObject"); // Etc.
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, SetParameter)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  log_info("TEST(ObjectHandler_Test, SetParameter) - sup::core::ObjectHandler instance '%p'", handler);

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->Instantiate("MyAnyObject","MyAnyObject") &&
	     handler->Instantiate("MyCfgableObject","MyCfgableObject") &&
	     handler->Instantiate("MyMsgableObject","MyMsgableObject") &&
	     handler->Instantiate("MyAnyableObject","MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->SetParameter("MyAnyObject","name","value") &&
	     handler->SetParameter("MyCfgableObject","name","value") &&
	     !handler->SetParameter("MyMsgableObject","name","value") &&
	     handler->SetParameter("MyAnyableObject","name","value"));
    }

  if (ret)
    {
      ret = ((handler->SetParameter("MyCfgableObject","value","1234") &&
	      (1234u == handler->GetInstance<MyCfgableObject>("MyCfgableObject")->GetValue())) &&
	     (handler->SetParameter("MyAnyableObject","value","1234") &&
	      (1234u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetValue())));
    }

  if (ret)
    {
      ret = ((handler->SetParameter("MyCfgableObject","reset","") &&
	      (0u == handler->GetInstance<MyCfgableObject>("MyCfgableObject")->GetValue())) &&
	     (handler->SetParameter("MyAnyableObject","reset","") &&
	      (0u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetValue())));
    }

  if (ret)
    {
      ret = ((handler->SetParameter("MyCfgableObject","incr","") &&
	      (1u == handler->GetInstance<MyCfgableObject>("MyCfgableObject")->GetValue())) &&
	     (handler->SetParameter("MyAnyableObject","incr","") &&
	      (1u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetValue())));
    }

  if (ret)
    {
      ret = (!handler->SetParameter("MyCfgableObject","false","") &&
	     !handler->SetParameter("MyAnyableObject","false",""));
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, ProcessMessage)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ret = (handler->Instantiate("MyAnyObject","MyAnyObject") &&
	     handler->Instantiate("MyCfgableObject","MyCfgableObject") &&
	     handler->Instantiate("MyMsgableObject","MyMsgableObject") &&
	     handler->Instantiate("MyAnyableObject","MyAnyableObject"));
    }

  if (ret)
    {
      ret = (!handler->ProcessMessage("MyAnyObject","msg") &&
	     !handler->ProcessMessage("MyCfgableObject","msg") &&
	     handler->ProcessMessage("MyMsgableObject","msg") &&
	     handler->ProcessMessage("MyAnyableObject","msg"));
    }

  if (ret)
    {
      ret = ((handler->ProcessMessage("MyMsgableObject","reset") &&
	      (0u == handler->GetInstance<MyMsgableObject>("MyMsgableObject")->GetCount())) &&
	     (handler->ProcessMessage("MyAnyableObject","reset") &&
	      (0u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetCount())));
    }

  if (ret)
    {
      ret = ((handler->ProcessMessage("MyMsgableObject","incr") &&
	      (1u == handler->GetInstance<MyMsgableObject>("MyMsgableObject")->GetCount())) &&
	     (handler->ProcessMessage("MyAnyableObject","incr") &&
	      (1u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetCount())));
    }

  if (ret)
    {
      ret = ((handler->ProcessMessage("MyMsgableObject","reset") &&
	      (0u == handler->GetInstance<MyMsgableObject>("MyMsgableObject")->GetCount())) &&
	     (handler->ProcessMessage("MyAnyableObject","reset") &&
	      (0u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetCount())));
    }

  if (ret)
    {
      ret = (!handler->ProcessMessage("MyMsgableObject","false") &&
	     !handler->ProcessMessage("MyAnyableObject","false"));
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, ParseXMLContent)
{
  sup::core::ObjectHandler* handler = new (std::nothrow) sup::core::ObjectHandler ();

  bool ret = (static_cast<sup::core::ObjectHandler*>(NULL) != handler);

  if (ret)
    {
      ccs::types::char8 buffer [] = "<?xml version=\"1.0\"?>"
"<engine>"
"  <libraries/>"
"  <instances>"
"    <instance name=\"MyAnyableObject\" type=\"MyAnyableObject\">"
"      <parameter name=\"value\">0u</parameter>"
"      <message>reset</message>"
"    </instance>"
"  </instances>"
"</engine>";
      ret = sup::HelperTools::ParseXMLContent(handler, buffer);
    }

  if (ret)
    {
      ret = (handler->IsValid("MyAnyableObject") &&
	     handler->IsInstanceOf<MyAnyableObject>("MyAnyableObject"));
    }

  if (ret)
    {
      ret = ((0u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetCount()) &&
	     (0u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetValue()));
    }

  if (ret)
    {
      ccs::types::char8 buffer [] = "<?xml version=\"1.0\"?>"
"<engine>"
"  <instances>"
"    <instance name=\"MyAnyableObject\">"
"      <parameter name=\"value\">1234</parameter>"
"      <message>incr</message>"
"    </instance>"
"  </instances>"
"</engine>";
      ret = sup::HelperTools::ParseXMLContent(handler, buffer);
    }

  if (ret)
    {
      ret = ((1u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetCount()) &&
	     (1234u == handler->GetInstance<MyAnyableObject>("MyAnyableObject")->GetValue()));
    }

  if (ret)
    {
      ccs::types::char8 buffer [] = "<?xml version=\"1.0\"?>"
"<engine>"
"  <libraries>"
"    <library>UndefinedLibrary</library>"
"  </libraries>"
"</engine>";
      ret = !sup::HelperTools::ParseXMLContent(handler, buffer); // Expect failure
    }

  if (ret)
    {
      ccs::types::char8 buffer [] = "<?xml version=\"1.0\"?>"
"<engine>"
"  <instances>"
"    <instance name=\"UndefinedObject\" type=\"UndefinedType\"/>"
"  </instances>"
"</engine>";
      ret = !sup::HelperTools::ParseXMLContent(handler, buffer); // Expect failure
    }

  if (ret)
    {
      ccs::types::char8 buffer [] = "<?xml version=\"1.0\"?>"
"<engine>"
"  <instances>"
"    <instance name=\"MyAnyObject\" type=\"MyAnyObject\">"
"      <parameter name=\"value\">1234</parameter>"
"      <message>incr</message>"
"    </instance>"
"  </instances>"
"</engine>";
      ret = !sup::HelperTools::ParseXMLContent(handler, buffer); // Expect failure
    }

  if (ret)
    {
      delete handler;
    }

  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, Executable)
{
  bool ret = true;

  if (ret)
    { // Start test IOC
      // Compose system command
      std::string command = std::string("/usr/bin/bash -c '../../../target/bin/obj-factory --help > /tmp/obj.out'");
      ret = (std::system(command.c_str()) == 0);
      std::stringstream sstr; sstr << std::ifstream("/tmp/obj.out").rdbuf();
      std::string result = sstr.str();
      log_info("TEST(ObjectHandler_Test, Executable) - 'obj-factory --help' returns ..");
      std::cout << result << std::endl;
     }

  if (ret)
    { // Start test IOC
      // Compose system command
      std::string command = std::string("/usr/bin/bash -c '../../../target/bin/obj-factory --file UndefinedFile > /tmp/obj.out'");
      ret = (std::system(command.c_str()) == 0);
      std::stringstream sstr; sstr << std::ifstream("/tmp/obj.out").rdbuf();
      std::string result = sstr.str();
      log_info("TEST(ObjectHandler_Test, Executable) - 'obj-factory --file <no_valid_file>' returns ..");
      std::cout << result << std::endl;
     }
  /*
      std::string command = std::string("/usr/bin/screen -d -m /usr/bin/softIoc -d ./unit/ChannelAccessClient-tests.db > /tmp/ioc.out");
      status = (std::system(command.c_str()) == 0);
  */
  ASSERT_EQ(true, ret);
}

TEST(ObjectHandler_Test, PythonAPI) // From Cosylab executable test code
{
  sup::core::ObjectEngine* engine = new_ObjectFactoryEngine();

  bool ret = static_cast<sup::core::ObjectEngine*>(NULL) != engine;

  printf("engine.Load: %d\n", ObjectFactoryEngine_Load(engine, "libsup-common.so"));
  
  ccs::base::AnyObject* instance1 = ObjectFactoryEngine_Instantiate(engine, "COSInterface", "CTRL-SUP-AUTO");
  printf("instance.SetParameter: %d\n", AnyObject_SetParameter(instance1, "instance", "CTRL-SUP-AUTO"));
  printf("instance.ProcessMessage: %d\n", AnyObject_ProcessMessage(instance1, "Initialize"));
  
  
  ccs::base::AnyObject* instance2 = ObjectFactoryEngine_Instantiate(engine,"COSInterface", "CBS2-COS");
  printf("instance2.SetParameter: %d\n",  AnyObject_SetParameter(instance2, "instance", "BUIL"));
  printf("instance2.ProcessMessage: %d\n",  AnyObject_ProcessMessage(instance2, "Initialize"));
  
  ccs::base::AnyObject* instance3 = ObjectFactoryEngine_Instantiate(engine,"COSContainer", "CTRL");
  printf("instance3.SetParameter: %d\n",  AnyObject_SetParameter(instance3, "instance", "CTRL"));
  printf("instance3.SetParameter: %d\n",  AnyObject_SetParameter(instance3, "period", "1000000000"));
  
  printf("instance3.ProcessMessage: %d\n",  AnyObject_ProcessMessage(instance3, "Pair CTRL-SUP-AUTO"));
  printf("instance3.ProcessMessage: %d\n",  AnyObject_ProcessMessage(instance3, "Pair CBS2-COS"));
  printf("instance3.ProcessMessage: %d\n",  AnyObject_ProcessMessage(instance3, "Initialize"));
  printf("instance3.ProcessMessage: %d\n",  AnyObject_ProcessMessage(instance3, "Start"));
  
  printf("Sleeing 10 seconds\n");
  usleep(10*1000*1000);
  
  printf("Done\n");
  ObjectFactoryEngine_Terminate_Ref(engine, instance1);
  ObjectFactoryEngine_Terminate_All(engine);
  delete_ObjectFactoryEngine(engine);
  
  ASSERT_EQ(true, ret);
}


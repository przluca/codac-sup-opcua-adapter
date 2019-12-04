/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/test/c++/unit/AnyValue-tests.cpp $
* $Id: AnyValue-tests.cpp 101444 2019-08-08 06:27:33Z bauvirb $
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

// Local header files

#include "BasicTypes.h" // Misc. type definition
#include "tools.h" // Misc. helper functions

//#define LOG_TRACE_ENABLE
//#define LOG_DEBUG_ENABLE
#include "log-api.h" // Syslog wrapper routines

#include "AnyType.h"
#include "CompoundType.h"
#include "ScalarType.h"

#include "AnyValue.h"
#include "AnyValueHelper.h"

// Constants

// Type definition

typedef struct {

  ccs::types::uint64 counter;
  ccs::types::uint64 timestamp;

} MyTypeDefinition_t;

typedef struct __attribute__((__packed__)) {

  char name [STRING_MAX_LENGTH];
  uint32_t affinity;
  uint64_t period;
  uint64_t phase;
  char policy [STRING_MAX_LENGTH];
  uint32_t priority;

} sdncfg_t;

// Global variables

static ccs::log::Func_t __handler = ccs::log::SetStdout();
#ifdef LOG_TRACE_ENABLE
static ccs::log::Severity_t __level = ccs::log::SetFilter(LOG_TRACE);
#else
#ifdef LOG_DEBUG_ENABLE
static ccs::log::Severity_t __level = ccs::log::SetFilter(LOG_DEBUG);
#endif
#endif
// Function declaration

// Function definition

TEST(AnyValue_Test, Constructor_default)
{
  ccs::types::AnyValue value;

  bool ret = ((0u == value.GetSize()) &&
	      (static_cast<bool>(value.GetType()) == false));

  ccs::types::ArrayType type ("test::array", ccs::types::UnsignedInteger64, 8u);

  if (ret)
    {
      ccs::types::AnyValue copy (type);
      value = copy; // Copy assignment operator from AnyValue
    }

  if (ret)
    {
      ret = (value.GetSize() == type.GetSize());
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Constructor_array)
{
  ccs::types::ArrayType type ("test::array", ccs::types::UnsignedInteger64, 8u); 

  bool ret = (64u == type.GetSize());;

  if (ret)
    {
      ccs::types::AnyValue value (type);
      ret = (value.GetSize() == type.GetSize());
      log_info("TEST(AnyValue_Test, Constructor_array) - Type '%s' use count '%d'", value.GetType()->GetName(), value.GetType().use_count()); // 2 since one is stored and one is used for the test
    }

  if (ret) // Externally hosted buffer
    {
      ccs::types::uint64 buffer [8] = { 0ul, ccs::HelperTools::GetCurrentTime(), 0ul, 0ul, 0ul,0ul,0ul,0ul };

      log_info("TEST(AnyValue_Test, Constructor_array) - Creating variable with type '%s' and external buffer '%p' ..", type.GetName(), static_cast<void*>(&buffer));

      ccs::types::AnyValue value (type, static_cast<void*>(&buffer));

      log_info("TEST(AnyValue_Test, Constructor_array) - .. '%u at %p'", 0u, ccs::HelperTools::GetElementReference(&value, 0u));
      log_info("TEST(AnyValue_Test, Constructor_array) - .. '%u at %p'", 1u, ccs::HelperTools::GetElementReference(&value, 1u));
#if 0
      ccs::types::uint64 element = 0ul;

      ret = (ccs::HelperTools::GetElementValue<ccs::types::uint64>(&value, 1u, element) &&
	     (element == buffer[1]));
#else
      ret = ((ccs::HelperTools::GetElementValue<ccs::types::uint64>(&value, 0u) == buffer[0]) &&
	     (ccs::HelperTools::GetElementValue<ccs::types::uint64>(&value, 1u) == buffer[1]));
#endif
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Constructor_struct)
{
  // Since v1.3.3 - Type composition is managed through smart pointers ..
  log_info("TEST(AnyValue_Test, Constructor_struct) - Type '%s' use count '%d'", ccs::types::UnsignedInteger64->GetName(), ccs::types::UnsignedInteger64.use_count());
  //bool ret = (ccs::types::UnsignedInteger64.use_count() == 1);

  ccs::types::CompoundType type; 
  type.AddAttribute("counter", ccs::types::UnsignedInteger64);
  type.AddAttribute("timestamp", ccs::types::UnsignedInteger64);

  bool ret = (16u == type.GetSize());

  if (ret)
    {
      log_info("TEST(AnyValue_Test, Constructor_struct) - Type '%s' use count '%d'", ccs::types::UnsignedInteger64->GetName(), ccs::types::UnsignedInteger64.use_count());
    }

  // AnyValue instantiated with reference to a AnyType instance will make
  // a copy before storing it as a smart pointer so as to ensure the 
  // AnyType instance does not get deleted before going out of scope.

  if (ret)
    {
      ccs::types::AnyValue value (type);
      log_info("TEST(AnyValue_Test, Constructor_struct) - Type '%s' use count '%d'", ccs::types::UnsignedInteger64->GetName(), ccs::types::UnsignedInteger64.use_count());
      ret = (value.GetSize() == type.GetSize());
    }

  if (ret) // Externally hosted buffer
    {
      struct {
	ccs::types::uint64 counter;
	ccs::types::uint64 timestamp;
      } buffer = { 0ul, ccs::HelperTools::GetCurrentTime() };

      ccs::types::AnyValue value (type, static_cast<void*>(&buffer));

      ret = ((ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "counter") == buffer.counter) &&
	     (ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "timestamp") == buffer.timestamp));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Constructor_scalar)
{
  ccs::types::ScalarType type ("bitmap", 4u);
  bool ret = (4u == type.GetSize());

  if (ret)
    {
      ccs::types::AnyValue value (type);
      ret = (value == 0);
    }

  if (ret) // Externally hosted buffer
    {
      ccs::types::uint32 buffer = 0x01234567u;
      ccs::types::AnyValue value (type, static_cast<void*>(&buffer));
      ret = (value == 0x01234567u);
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Constructor_pointer)
{
  ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("scalar", 4u);

  bool ret = ((static_cast<ccs::types::AnyType*>(NULL) != type) && (4u == type->GetSize()));

  if (ret)
    {
      ccs::types::AnyValue value (type);
      ret = (value == 0);
      // WARNING - Variable is getting out of scope. Pointer is deleted
    }

  if (ret) // Externally hosted buffer
    {
      ccs::types::AnyType* type = new (std::nothrow) ccs::types::ScalarType ("scalar", 4u);
      ccs::types::uint32 buffer = 0x01234567u;
      ccs::types::AnyValue value (type, static_cast<void*>(&buffer));
      ret = (value == 0x01234567u);
    }

  if (ret) // Shared pointer will take care of deletign this one
    {
      //delete type;
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Constructor_stream)
{
  bool ret = true;

  ccs::types::AnyValue value ("{\"type\":\"uint64\",\"size\":8}");

  if (ret)
    {
      ret = (ccs::types::UnsignedInteger64 == value.GetType());
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Constructor_copy)
{
  bool ret = true;

  ccs::types::AnyValue value ("{\"type\":\"uint64\",\"size\":8}");
  ccs::types::AnyValue copy (value);

  if (ret)
    {
      ret = (value.GetSize() == copy.GetSize());
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Operator_copy)
{
  ccs::types::AnyValue value ("{\"type\":\"uint64\",\"size\":8}");
  ccs::types::AnyValue copy ("{\"type\":\"bytearray\",\"multiplicity\":8,\"element\":{\"type\":\"uint8\",\"size\":1}}");

  value = 1ul;

  copy = value; // Copy buffers

  bool ret = (1 == ccs::HelperTools::GetElementValue<ccs::types::uint8>(&copy, 0u));

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Assignment_scalar)
{
  ccs::types::AnyValue value (ccs::types::SignedInteger32);

  bool ret = (value == 0);

  if (ret)
    {
      value = 8;
      ret = (value == 8);
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Assignment_string)
{
  ccs::types::AnyValue value (ccs::types::String);

  bool ret = true;

  if (ret)
    {
      ccs::types::string str = "This is a string variable";
      log_info("TEST(AnyValue_Test, Assignment_string) - Assignment to '%s'", str);
      value = str;
      ret = (value == str);
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, Assignment_string) - .. instance '%s'", static_cast<char*>(value.GetInstance()));
    }

  if (ret)
    {
      ccs::types::string str = STRING_UNDEFINED;
#if 0 // This does not work
      str = static_cast<ccs::types::string>(value);
#else
      ccs::HelperTools::SafeStringCopy(str, static_cast<char*>(value.GetInstance()), ccs::types::MaxStringLength);
#endif
      log_info("TEST(AnyValue_Test, Assignment_string) - .. cast back to '%s'", str);
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Assignment_structure)
{
  ccs::types::CompoundType type; 
  type.AddAttribute("counter", ccs::types::UnsignedInteger64);
  type.AddAttribute("timestamp", ccs::types::UnsignedInteger64);

  bool ret = (type.GetSize() == 16u);

  ccs::types::AnyValue value (type);

  MyTypeDefinition_t data; 
  data.counter = 1ul;
  data.timestamp = get_time();

  if (ret)
    {
      value = data;
      ret = (value == data);
    }

  if (ret)
    {
      ret = ((ccs::HelperTools::HasAttribute(&value, "counter") == true) &&
	     (ccs::types::UnsignedInteger64 == ccs::HelperTools::GetAttributeType(&value, "counter")) &&
	     (data.counter == ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "counter")));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, ParseInstance_structure)
{
  ccs::types::CompoundType type; 
  type.AddAttribute("counter", ccs::types::UnsignedInteger64);
  type.AddAttribute("timestamp", ccs::types::UnsignedInteger64);

  bool ret = (type.GetSize() == 16u);

  ccs::types::AnyValue value (type);

  MyTypeDefinition_t data; 

  if (ret)
    {
      char string [] = "{ \"counter\":0x0123456789abcdef, \"timestamp\":0x0000000000000000 }";

      ret = (value.ParseInstance(string) == true);
    }

  if (ret)
    {
      ret = ((ccs::HelperTools::HasAttribute(&value, "counter") == true) &&
	     (ccs::HelperTools::Is<ccs::types::ScalarType>(ccs::HelperTools::GetAttributeType(&value, "counter"))) &&
	     (ccs::types::UnsignedInteger64 == ccs::HelperTools::GetAttributeType(&value, "counter")) &&
	     (0x0123456789abcdeful == ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "counter")));
    }

  if (ret)
    {
      ret = ((ccs::HelperTools::HasAttribute(&value, "timestamp") == true) &&
	     (ccs::HelperTools::Is<ccs::types::ScalarType>(ccs::HelperTools::GetAttributeType(&value, "timestamp"))) &&
	     (ccs::types::UnsignedInteger64 == ccs::HelperTools::GetAttributeType(&value, "timestamp")) &&
	     (0x0000000000000000ul == ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "timestamp")));
    }

  std::shared_ptr<const ccs::types::AnyType> cmp_type = value.GetType();
  std::shared_ptr<const ccs::types::CompoundType> inp_type;
  
  if (ret)
    {
      ret = ccs::HelperTools::Is<ccs::types::CompoundType>(cmp_type);
    }

  if (ret)
    {
      inp_type = std::dynamic_pointer_cast<const ccs::types::CompoundType>(cmp_type);
      ret = (inp_type ? true : false);
    }

  for (uint_t index = 0u; (ret && (index < inp_type->GetAttributeNumber())); index += 1u)
    {
      const char* attr_name = inp_type->GetAttributeName(index);
      std::shared_ptr<const ccs::types::AnyType> attr_type = ccs::HelperTools::GetAttributeType(&value, attr_name);

      log_info("TEST(AnyValue_Test, ParseInstance_structure) - Attribute '%s' with type '%s (%u %p)' ..", attr_name, attr_type->GetName(), attr_type.use_count(), attr_type.get());
      ccs::types::AnyValue attr_value = ccs::HelperTools::GetAttributeValue<ccs::types::AnyValue>(&value, attr_name);
      const ccs::types::AnyValue* nst_value = &attr_value;

      ret = ((ccs::HelperTools::Is<ccs::types::ScalarType>(nst_value)) &&
	     (ccs::HelperTools::Is<ccs::types::ScalarType>(attr_type)));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, ParseInstance_nested)
{

  ccs::types::CompoundType* outer = static_cast<ccs::types::CompoundType*>(NULL);
  ccs::types::CompoundType* inner = static_cast<ccs::types::CompoundType*>(NULL);
  ccs::types::CompoundType* array = static_cast<ccs::types::CompoundType*>(NULL);

  bool ret = ((array = new (std::nothrow) ccs::types::CompoundType ("array")) != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      array->AddAttribute("enable", ccs::types::Boolean);
      array->AddAttribute("setpoint", ccs::types::Float64);
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, ParseInstance_nested) - Testing '%s' size '%u'", array->GetName(), array->GetSize());
      ret = (array->GetSize() == 9u);
    }

  if (ret)
    {
      ret = ((inner = new (std::nothrow) ccs::types::CompoundType ("inner")) != static_cast<ccs::types::CompoundType*>(NULL));
    }

  if (ret)
    {
      inner->AddAttribute("mode", ccs::types::String);
      inner->AddAttribute("devices", ccs::HelperTools::NewArrayType("device", array, 10));
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, ParseInstance_nested) - Testing '%s' size '%u'", inner->GetName(), inner->GetSize());
      ret = (inner->GetSize() == 154u);
    }

  if (ret)
    {
      ret = ((outer = new (std::nothrow) ccs::types::CompoundType ("outer")) != static_cast<ccs::types::CompoundType*>(NULL));
    }

  if (ret)
    {
      outer->AddAttribute("counter", ccs::types::UnsignedInteger64);
      outer->AddAttribute("timestamp", ccs::types::UnsignedInteger64);
      outer->AddAttribute("control", inner);
      outer->AddAttribute("reserved", ccs::HelperTools::NewArrayType("reserved", ccs::types::UnsignedInteger8, 8));
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, ParseInstance_nested) - Testing '%s' size '%u'", outer->GetName(), outer->GetSize());
      ret = (outer->GetSize() == 178u);
    }

  ccs::types::AnyValue value (outer);

  if (ret)
    {
      log_info("TEST(AnyValue_Test, ParseInstance_nested) - Testing '%s' size '%u'", (value.GetType())->GetName(), (value.GetType())->GetSize());
      ret = ((value.GetType())->GetSize() == 178u);
    }

  if (ret)
    {
      char buffer [1024] = 
	"{"
	"\"counter\":0x0123456789abcdef,"
	"\"timestamp\":0x0000000000000000,"
	"\"control\": {"
	"\"mode\":\"current\","
	"\"devices\": [ { \"enable\":true, \"setpoint\":0.0 },"
	"{ \"enable\":true, \"setpoint\":1.0 },"
	"{ \"enable\":true, \"setpoint\":2.0 },"
	"{ \"enable\":true, \"setpoint\":3.0 },"
	"{ \"enable\":true, \"setpoint\":4.0 },"
	"{ \"enable\":true, \"setpoint\":5.0 },"
	"{ \"enable\":true, \"setpoint\":6.0 },"
	"{ \"enable\":true, \"setpoint\":7.0 },"
	"{ \"enable\":true, \"setpoint\":8.0 },"
	"{ \"enable\":true, \"setpoint\":9.0 }"
	"]"
	"},"
	"\"reserved\":[ 0, 1, 2, 3, 4, 5, 6, 7 ] }";
      
      log_info("TEST(AnyValue_Test, ParseInstance_nested) - Parsing '%s'", buffer);
      ret = (value.ParseInstance(buffer) == true);
    }

  if (ret)
    {
      ret = ((ccs::HelperTools::HasAttribute(&value, "counter") == true) &&
	     (ccs::types::UnsignedInteger64 == ccs::HelperTools::GetAttributeType(&value, "counter")) &&
	     (0x0123456789abcdeful == ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "counter")));
    }

  ccs::types::AnyValue control (ccs::HelperTools::GetAttributeType(&value, "control"), 
				ccs::HelperTools::GetAttributeReference(&value, "control"));

  if (ret)
    {
      ret = ((ccs::HelperTools::HasAttribute(&control, "mode") == true) &&
	     (ccs::types::String == ccs::HelperTools::GetAttributeType(&control, "mode")) &&
	     (strcmp(static_cast<const char*>(ccs::HelperTools::GetAttributeReference(&control, "mode")), "current") == 0));
    }

  ccs::types::AnyValue devices (ccs::HelperTools::GetAttributeType(&control, "devices"), 
				ccs::HelperTools::GetAttributeReference(&control, "devices"));

  if (ret)
    {
      ret = ((ccs::HelperTools::Is<ccs::types::ArrayType>(&devices) == true) &&
	     (ccs::HelperTools::HasElement(&devices, 2) == true) &&
	     (static_cast<void*>(NULL) != ccs::HelperTools::GetElementReference(&devices, 2)));
    }

  ccs::types::AnyValue device (ccs::HelperTools::GetElementType(&devices), 
			       ccs::HelperTools::GetElementReference(&devices, 2));

  if (ret)
    {
      ret = ((ccs::HelperTools::HasAttribute(&device, "enable") == true) &&
	     (ccs::types::Boolean == ccs::HelperTools::GetAttributeType(&device, "enable")) &&
	     (static_cast<void*>(NULL) != ccs::HelperTools::GetAttributeReference(&device, "enable")) &&
	     (true == ccs::HelperTools::GetAttributeValue<ccs::types::boolean>(&device, "enable")));
    }

  if (ret)
    {
      ret = ((ccs::HelperTools::HasAttribute(&device, "setpoint") == true) &&
	     (ccs::types::Float64 == ccs::HelperTools::GetAttributeType(&device, "setpoint")) &&
	     (static_cast<void*>(NULL) != ccs::HelperTools::GetAttributeReference(&device, "setpoint")) &&
	     (2.0 == ccs::HelperTools::GetAttributeValue<ccs::types::float64>(&device, "setpoint")));
    }

  ccs::types::AnyValue reserved (ccs::HelperTools::GetAttributeType(&value, "reserved"), 
				 ccs::HelperTools::GetAttributeReference(&value, "reserved"));

  if (ret)
    {
      ret = ((ccs::HelperTools::HasElement(&reserved, 2u) == true) &&
	     (ccs::types::UnsignedInteger8 == ccs::HelperTools::GetElementType(&reserved)) &&
	     (2 == ccs::HelperTools::GetElementValue<ccs::types::uint8>(&reserved, 2u)));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, SerialiseInstance_nested)
{

  ccs::types::CompoundType* outer = static_cast<ccs::types::CompoundType*>(NULL);
  ccs::types::CompoundType* inner = static_cast<ccs::types::CompoundType*>(NULL);
  ccs::types::CompoundType* array = static_cast<ccs::types::CompoundType*>(NULL);

  bool ret = ((array = new (std::nothrow) ccs::types::CompoundType ("array")) != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      array->AddAttribute("enable", ccs::types::Boolean);
      array->AddAttribute("setpoint", ccs::types::Float64);
    }

  if (ret)
    {
      ret = ((inner = new (std::nothrow) ccs::types::CompoundType ("inner")) != static_cast<ccs::types::CompoundType*>(NULL));
    }

  if (ret)
    {
      inner->AddAttribute("mode", ccs::types::String);
      inner->AddAttribute("devices", ccs::HelperTools::NewArrayType("device", array, 10));
    }

  if (ret)
    {
      ret = ((outer = new (std::nothrow) ccs::types::CompoundType ("outer")) != static_cast<ccs::types::CompoundType*>(NULL));
    }

  if (ret)
    {
      outer->AddAttribute("counter", ccs::types::UnsignedInteger64);
      outer->AddAttribute("timestamp", ccs::types::UnsignedInteger64);
      outer->AddAttribute("control", inner);
      outer->AddAttribute("reserved", ccs::HelperTools::NewArrayType("reserved", ccs::types::UnsignedInteger8, 8));
    }

  ccs::types::AnyValue value (outer);

  if (ret)
    {
      ret = ((value.GetType())->GetSize() == 178u);
    }

  char string [1024] = STRING_UNDEFINED;

  if (ret)
    {
      log_info("TEST(AnyValue_Test, SerialiseInstance_nested) - Convert type to string ...");
      ret = (value.SerialiseType(string, 1024u) == true); 
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, SerialiseInstance_nested) - .. '%s'", string);
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, SerialiseInstance_nested) - Convert instance to string ...");
      ret = (value.SerialiseInstance(string, 1024u) == true); 
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, SerialiseInstance_nested) - .. '%s'", string);
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Composition_rtpl)
{
  ccs::types::CompoundType thread_cfg; 
  thread_cfg.AddAttribute("name", ccs::types::String);
  thread_cfg.AddAttribute("affinity", ccs::types::UnsignedInteger32);
  thread_cfg.AddAttribute("period", ccs::types::UnsignedInteger64);
  thread_cfg.AddAttribute("phase", ccs::types::SignedInteger64);
  thread_cfg.AddAttribute("policy", ccs::types::String); // Should be enum
  thread_cfg.AddAttribute("priority", ccs::types::UnsignedInteger32);

  ccs::types::CompoundType thread_sta; 
  thread_sta.AddAttribute("status", ccs::types::String);
  thread_sta.AddAttribute("cycles", ccs::types::UnsignedInteger64);
  thread_sta.AddAttribute("init-time", ccs::types::UnsignedInteger64);
  thread_sta.AddAttribute("exec-time", ccs::types::UnsignedInteger64);

  ccs::types::CompoundType pid_cfg; 
  pid_cfg.AddAttribute("Kp", ccs::types::Float64);
  pid_cfg.AddAttribute("Td", ccs::types::Float64);
  pid_cfg.AddAttribute("Ti", ccs::types::Float64);
  pid_cfg.AddAttribute("Uh", ccs::types::Float64);
  pid_cfg.AddAttribute("Ul", ccs::types::Float64);

  // Since v1.3.3 - Type composition is managed through smart pointers ..
  // CompoundType instantiated with reference to a AnyType instance will
  // make a copy before storing it as a smart pointer so as to ensure the 
  // AnyType instance does not get deleted before going out of scope.

  ccs::types::CompoundType pon_cfg;
  pon_cfg.AddAttribute("thread", thread_cfg);

  ccs::types::CompoundType rtc_cfg;
  rtc_cfg.AddAttribute("pid", pid_cfg);
  rtc_cfg.AddAttribute("thread", thread_cfg);

  ccs::types::CompoundType sdn_cfg;
  sdn_cfg.AddAttribute("thread", thread_cfg);

  ccs::types::CompoundType type;
  type.AddAttribute("pon", pon_cfg);
  type.AddAttribute("rtc", rtc_cfg);
  type.AddAttribute("sdn", sdn_cfg);

  ccs::types::AnyValue value (type);

  bool ret = (value.GetSize() == 496u);

  if (ret)
    {
      char string [1024] = 
	"{"
	"\"pon\": {"
	         "\"thread: {"
                           "\"name\":\"CAIf Thread\","
                           "\"affinity\":0,"
                           "\"period\":10000000,"
                           "\"phase\":0,"
                           "\"policy\":\"NORMAL\","
                           "\"priority\":0"
                           "}"
                 "},"
	"\"rtc\": {"
	         "\"pid: {"
	                "\"Kp\":1.0,"
                        "\"Td\":0.0,"
                        "\"Ti\":0.1,"
                        "\"Uh\":10,"
                        "\"Ul\":-10"
                        "},"
	         "\"thread: {"
                           "\"name\":\"RTC Thread\","
                           "\"affinity\":3,"
                           "\"period\":1000000,"
                           "\"phase\":0,"
                           "\"policy\":\"SCHED_FIFO\","
                           "\"priority\":80"
                           "}"
                 "},"
	"\"sdn\": {"
	         "\"thread: {"
                           "\"name\":\"SDN Thread\","
                           "\"affinity\":3,"
                           "\"period\":1000000,"
                           "\"phase\":500000,"
                           "\"policy\":\"SCHED_FIFO\","
                           "\"priority\":80"
                           "}"
                 "}"
        "}";
      
      ret = value.ParseInstance(string);
    }

  if (ret)
    {
      char string [1024] = STRING_UNDEFINED;

      log_info("TEST(AnyValue_Test, Composition_rtpl) - Convert to string ...");
      ret = (value.SerialiseInstance(string, 1024u) == true); 
      log_info("TEST(AnyValue_Test, Composition_rtpl) - .. '%s'", string);
    }

  if (ret)
    {
      ret = (ccs::HelperTools::HasAttribute(&value, "sdn") &&
	     ccs::HelperTools::HasAttribute(&value, "sdn.thread") &&
	     ccs::HelperTools::HasAttribute(&value, "sdn.thread.name") &&
	     ccs::HelperTools::HasAttribute(&value, "sdn.thread.affinity") &&
	     ccs::HelperTools::HasAttribute(&value, "sdn.thread.period"));
    }

  if (ret)
    {
      ret = (ccs::HelperTools::IsCompoundType(ccs::HelperTools::GetAttributeType(&value, "sdn")) &&
	     ccs::HelperTools::IsCompoundType(ccs::HelperTools::GetAttributeType(&value, "sdn.thread")) &&
	     ccs::HelperTools::IsScalarType(ccs::HelperTools::GetAttributeType(&value, "sdn.thread.name")) &&
	     ccs::HelperTools::IsScalarType(ccs::HelperTools::GetAttributeType(&value, "sdn.thread.affinity")) &&
	     ccs::HelperTools::IsScalarType(ccs::HelperTools::GetAttributeType(&value, "sdn.thread.period")));
    }

  if (ret)
    {
      ret = ((ccs::HelperTools::GetAttributeOffset(value.GetType(), "sdn") == 344u) &&
	     (ccs::HelperTools::GetAttributeOffset(value.GetType(), "sdn.thread") == 344u) &&
	     (ccs::HelperTools::GetAttributeOffset(value.GetType(), "sdn.thread.name") == 344u) &&
	     (ccs::HelperTools::GetAttributeOffset(value.GetType(), "sdn.thread.affinity") == 408u) &&
	     (ccs::HelperTools::GetAttributeOffset(value.GetType(), "sdn.thread.period") == 412u));
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, Composition_rtpl) - Attributes are '%u' and '%lu'", ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&value, "sdn.thread.affinity"), ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "sdn.thread.period"));
      ret = ((ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&value, "sdn.thread.affinity") == 3u) &&
	     (ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "sdn.thread.period") == 1000000ul));
    }

  ccs::types::AnyValue sdn_thread (ccs::HelperTools::GetAttributeType(&value, "sdn.thread"), 
				   ccs::HelperTools::GetAttributeReference(&value, "sdn.thread"));

  sdncfg_t sdncfg = sdn_thread;

  if (ret)
    {
      log_info("TEST(AnyValue_Test, Composition_rtpl) - Attributes are '%u' and '%lu'", sdncfg.affinity, sdncfg.period);
      ret = ((strncmp(sdncfg.name, "SDN Thread", 10) == 0) && (sdncfg.affinity == 3u) && 
	     (sdncfg.period == 1000000ul) && (sdncfg.phase == 500000) && 
	     (strncmp(sdncfg.policy, "SCHED_FIFO", 10) == 0) && (sdncfg.priority == 80));
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, ToNetworkByteOrder)
{

  ccs::types::CompoundType* header = static_cast<ccs::types::CompoundType*>(NULL);

  bool ret = ((header = new (std::nothrow) ccs::types::CompoundType ("header")) != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      header->AddAttribute("uid", ccs::types::UnsignedInteger64);
      header->AddAttribute("size", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_uid", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_version", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_size", ccs::types::UnsignedInteger32);
      header->AddAttribute("counter", ccs::types::UnsignedInteger64);
      header->AddAttribute("send_time", ccs::types::UnsignedInteger64);
      header->AddAttribute("recv_time", ccs::types::UnsignedInteger64);
      ret = (header->GetSize() == 48u);
    }

  struct { 
    uint64_t uid;
    uint32_t size;
    uint32_t t_uid;
    uint32_t t_version;
    uint32_t t_size;
    uint64_t counter;
    uint64_t send_t;
    uint64_t recv_t;
  } buffer;

  buffer.uid = 0x0123456789abcdeful;
  buffer.size = 48u;
  buffer.t_uid = 0x01234567u;
  buffer.t_version = 0x89abcdefu;
  buffer.t_size = 1024u;
  buffer.counter = 0ul;
  buffer.send_t = 0ul;
  buffer.recv_t = 0ul;

  if (ret)
    {
      ret = (header->GetSize() == sizeof(buffer));
    }

  ccs::types::AnyValue value (header);
  value = buffer;

  if (ret)
    {
      ret = (value.ToNetworkByteOrder() == true); 
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "uid") == 0xefcdab8967452301ul);
      else
	ret = (ccs::HelperTools::GetAttributeValue<ccs::types::uint64>(&value, "uid") == 0x0123456789abcdeful);
    }

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&value, "size") == 805306368u); // 0x30.00.00.00
      else
	ret = (ccs::HelperTools::GetAttributeValue<ccs::types::uint32>(&value, "size") == 48u);        // 0x00.00.00.30
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, FromNetworkByteOrder)
{

  ccs::types::CompoundType* header = static_cast<ccs::types::CompoundType*>(NULL);

  bool ret = ((header = new (std::nothrow) ccs::types::CompoundType ("header")) != static_cast<ccs::types::CompoundType*>(NULL));

  if (ret)
    {
      header->AddAttribute("uid", ccs::types::UnsignedInteger64);
      header->AddAttribute("size", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_uid", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_version", ccs::types::UnsignedInteger32);
      header->AddAttribute("topic_size", ccs::types::UnsignedInteger32);
      header->AddAttribute("counter", ccs::types::UnsignedInteger64);
      header->AddAttribute("send_time", ccs::types::UnsignedInteger64);
      header->AddAttribute("recv_time", ccs::types::UnsignedInteger64);
      ret = (header->GetSize() == 48u);
    }

  struct { 
    uint64_t uid;
    uint32_t size;
    uint32_t t_uid;
    uint32_t t_version;
    uint32_t t_size;
    uint64_t counter;
    uint64_t send_t;
    uint64_t recv_t;
  } buffer;

  buffer.uid = 0x0123456789abcdeful;
  buffer.size = 48u;
  buffer.t_uid = 0x01234567u;
  buffer.t_version = 0x89abcdefu;
  buffer.t_size = 1024u;
  buffer.counter = 0ul;
  buffer.send_t = 0ul;
  buffer.recv_t = 0ul;

  ccs::types::AnyValue value (header);
  value = buffer; // This sets the buffer in native byte order

  if (ret)
    {
      if (ccs::HelperTools::IsLittleEndian())
	ret = (value.FromNetworkByteOrder() == false); 
      else
	ret = (value.FromNetworkByteOrder() == true); 
    }

  ASSERT_EQ(true, ret);
}

TEST(AnyValue_Test, Operator_loose)
{
  ccs::types::AnyValue value (
    "{\"type\":\"test::loose::Scalars_t\","
     "\"attributes\":["
                     "{\"boolean\":{\"type\":\"bool\",\"size\":1}},"
                     "{\"uint32\":{\"type\":\"uint32\",\"size\":4}},"
                     "{\"float64\":{\"type\":\"float64\",\"size\":8}}"
                    "]"
    "}"
			      );

  ccs::types::AnyValue copy (
    "{\"type\":\"test::loose::Scalars_t\","
     "\"attributes\":["
                     "{\"uint32\":{\"type\":\"uint32\",\"size\":4}},"
                     "{\"boolean\":{\"type\":\"bool\",\"size\":1}},"
                     "{\"float64\":{\"type\":\"float64\",\"size\":8}}"
                    "]"
    "}"
			     );

  bool ret = ((value.GetSize() == copy.GetSize()) && (ccs::HelperTools::GetAttributeNumber(&value) == ccs::HelperTools::GetAttributeNumber(&copy)));

  if (ret)
    {
      ccs::types::uint32 attr = 10u;
      ret = ccs::HelperTools::SetAttributeValue(&value, "uint32", attr);
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, Operator_loose) - Original ..");
      ccs::HelperTools::LogSerialisedType(&value);
      ccs::HelperTools::LogSerialisedInstance(&value);
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, Operator_loose) - ccs::HelperTools::CopyOver");
      ret = ccs::HelperTools::CopyOver(&copy, &value);
    }

  if (ret)
    {
      log_info("TEST(AnyValue_Test, Operator_loose) - Copy ..");
      ccs::HelperTools::LogSerialisedType(&copy);
      ccs::HelperTools::LogSerialisedInstance(&copy);
    }

  if (ret)
    {
      ccs::types::uint32 attr = 0u;
      ret = ccs::HelperTools::GetAttributeValue(&copy, "uint32", attr);
    }

  ASSERT_EQ(true, ret);
}


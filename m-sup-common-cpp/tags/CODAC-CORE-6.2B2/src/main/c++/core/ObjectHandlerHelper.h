/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/core/ObjectHandlerHelper.h $
* $Id: ObjectHandlerHelper.h 100195 2019-06-19 13:35:27Z bauvirb $
*
* Project	: CODAC Core System
*
* Description	: Generic type class definition
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

/**
 * @file ObjectHandlerHelper.h
 * @brief Header file for sup::HelperTools routines
 * @date 19/06/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of various sup helper
 * routines supporting the definitioninstantiation of classes from configuration.
 * @note The code herein has dependency to libxml2 v2.7.6 installed with CODAC Core
 * System v5.4 or above. Inclusion of this header files requires availability
 * of <libxml/tree.h> and linking the application code with the appropriate libraries.
 */

#ifndef _ObjectHandlerHelper_h_
#define _ObjectHandlerHelper_h_

// Global header files

#include <BasicTypes.h>

#include <AnyObject.h>

#include <libxml/tree.h> 
//#include <libxml/parser.h>

// Local header files

#include "ObjectHandler.h"

// Constants

// Type definition

// Global variables

// Function declaration

// Function definition

namespace sup {

namespace HelperTools {

/**
 * @brief Object instantiation from XML file.
 */

static inline bool ParseXMLContent (sup::core::ObjectHandler* handler, const char* content)
{

  bool status = ((static_cast<sup::core::ObjectHandler*>(NULL) != handler) && 
		 !ccs::HelperTools::IsUndefinedString(content));

  if (status)
    {
      // Rely on libxml2 to load and parse configuration file
      xmlInitParser();
      LIBXML_TEST_VERSION;
    }
      
  xmlDocPtr xDoc = NULL;

  if (status)
    {
      xDoc = xmlReadMemory(content, strlen(content), "undefined.xml", NULL, 0);
      status = (NULL != xDoc); 
    } 

  if (status)
    {

      xmlNodePtr xCurNode = xmlDocGetRootElement(xDoc);
	
      xmlChar* prop; 
      
      for (xCurNode = xCurNode->children; xCurNode; xCurNode = xCurNode->next) 
	{
	  // Only interested in XML elements at this level
	  if (xCurNode->type != XML_ELEMENT_NODE) 
	    {
	      continue;
	    }
	  
	  // Only interested in 'libraries' ..
	  if (strcmp((char*) xCurNode->name, (char*) "libraries") == 0) 
	    { 
	      for (xmlNodePtr xLibNode = xCurNode->children; xLibNode; xLibNode = xLibNode->next) 
		{
		  if (strcmp((char*) xLibNode->name, (char*) "library") == 0) 
		    { 
		      char* lib_name = (char*) xLibNode->children->content; char* p_buf = lib_name; while (*p_buf != 0) { if (*p_buf == '\n') *p_buf = ' '; p_buf += 1; } // Replace 'newline' characters

		      log_info("ParseXMLContent - Found library '%s' ..", lib_name); 
		  
		      // Try and load library
		      status = handler->LoadLibrary(lib_name);

		      if (status)
			{
			  log_info("ParseXMLContent - .. successfully loaded");
			}
		      else
			{
			  log_warning("ParseXMLContent - .. failure");
			}
		    }
		}
	    }

	  // .. or 'instances' elements
	  if (strcmp((char*) xCurNode->name, (char*) "instances") == 0) 
	    { 
	      for (xmlNodePtr xObjNode = xCurNode->children; xObjNode; xObjNode = xObjNode->next) 
		{
		  if (strcmp((char*) xObjNode->name, (char*) "instance") == 0)
		    {
		      ccs::types::string inst_name = STRING_UNDEFINED; 
		      ccs::types::string inst_type = STRING_UNDEFINED; 
		  
		      log_info("ParseXMLContent - Found instance .."); 
		  
		      // Must have 'name' property
		      status = (NULL != xmlHasProp(xObjNode, (xmlChar*) "name"));

		      if (status)
			{
			  prop = xmlGetProp(xObjNode, (xmlChar*) "name"); ccs::HelperTools::SafeStringCopy(inst_name, (char*) prop, STRING_MAX_LENGTH); xmlFree(prop);
			}

		      if (status && (NULL != xmlHasProp(xObjNode, (xmlChar*) "type")))
			{ 
			  prop = xmlGetProp(xObjNode, (xmlChar*) "type"); ccs::HelperTools::SafeStringCopy(inst_type, (char*) prop, STRING_MAX_LENGTH); xmlFree(prop); 
			}
		  
		      log_info("... with name '%s' and type '%s'", inst_name, inst_type);
		  
		      if (status)
			{
			  // Try and instantiate or retrieve already existing instance
			  log_info("ParseXMLContent - Try and instantiate class .."); 
			  status = handler->Instantiate(inst_type, inst_name);

			  if (!status)
			    { // Continue if the instance is already existing ..
			      log_info("ParseXMLContent - .. test if already existing ..");
			      status = handler->IsValid(inst_name);
			    }

			  if (status)
			    {
			      log_info("ParseXMLContent - .. success");
			    }
			  else
			    {
			      log_warning("ParseXMLContent - .. failure");
			    }
			}

		      for (xmlNodePtr xChdNode = xObjNode->children; (status && xChdNode); xChdNode = xChdNode->next) 
			{
			  if (strcmp((char*) xChdNode->name, (char*) "parameter") == 0) 
			    { 
			      ccs::types::string attr_name = STRING_UNDEFINED; 
			      char* attr_value = static_cast<char*>(NULL);

			      // Must have 'name' property
			      status = (NULL != xmlHasProp(xChdNode, (xmlChar*) "name"));

			      if (status)
				{
				  prop = xmlGetProp(xChdNode, (xmlChar*) "name"); ccs::HelperTools::SafeStringCopy(attr_name, (char*) prop, STRING_MAX_LENGTH); xmlFree(prop);
				}

			      log_info("ParseXMLContent - Found parameter '%s' ..", attr_name); 

			      if (status)
				{
				  attr_value = (char*) xChdNode->children->content; char* p_buf = attr_value; while (*p_buf != 0) { if (*p_buf == '\n') *p_buf = ' '; p_buf += 1; } // Replace 'newline' characters
				  log_info("ParseXMLContent - .. with value '%s'", attr_value);
				}

			      if (status)
				{
				  status = handler->IsInstanceOf<ccs::base::CfgableObject>(inst_name);
				}

			      if (status)
				{
				  log_info("ParseXMLContent - Set parameter ..");
				  status = handler->SetParameter(inst_name, attr_name, attr_value);
				}

			      if (status)
				{
				  log_info("ParseXMLContent - .. success");
				}
			      else
				{
				  log_warning("ParseXMLContent - .. failure");
				}
			    }

			  if (strcmp((char*) xChdNode->name, (char*) "message") == 0) 
			    { 

			      char* msg_value = (char*) xChdNode->children->content; char* p_buf = msg_value; while (*p_buf != 0) { if (*p_buf == '\n') *p_buf = ' '; p_buf += 1; } // Replace 'newline 'characters
			  
			      log_info("Found message '%s' ..", msg_value); 

			      if (status)
				{
				  status = handler->IsInstanceOf<ccs::base::MsgableObject>(inst_name);
				}

			      if (status)
				{
				  log_info("ParseXMLContent - Process message ..");
				  status = handler->ProcessMessage(inst_name, msg_value);
				}

			      if (status)
				{
				  log_info("ParseXMLContent - .. success");
				}
			      else
				{
				  log_warning("ParseXMLContent - .. failure");
				}
			    }
			}
		    }
		}
	    }
	}
	  
      // Shutdown libxml
      xmlFreeDoc(xDoc);
      xmlCleanupParser();
      xmlMemoryDump();
    }

  return status;

}

static inline bool ParseXMLFile (sup::core::ObjectHandler* handler, const char* file_path)
{

  bool status = ccs::HelperTools::Exist(file_path);

  if (status)
    {
      std::ifstream file (file_path);
      std::string content ((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

      status = ParseXMLContent(handler, content.c_str());
    }

  return status;

}
  
} // namespace HelperTools

} // namespace sup

#endif // _ObjectHandlerHelper_h_ 


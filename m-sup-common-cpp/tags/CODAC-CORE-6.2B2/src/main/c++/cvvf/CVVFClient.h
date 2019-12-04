/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/cvvf/CVVFClient.h $
* $Id: CVVFClient.h 100159 2019-06-18 07:13:41Z bauvirb $
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
 * @file CVVFClient.h
 * @brief Header file for CVVFClient class.
 * @date 20/04/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the CVVFClient class interface.
 * The CVVFClient class implementation is not exposed through this header file. A bridge
 * pattern is favoured to try and avoid exposing potentially changing implementation details
 * to calling applications. 
 */

#ifndef _CVVFClient_h_
#define _CVVFClient_h_

// Global header files

// Local header files

#include <BasicTypes.h>

#include <AnyType.h>
#include <AnyTypeHelper.h>

#include <AnyValue.h>
#include <AnyValueHelper.h>

// Constants

// Type definition

namespace sup {

namespace core {

typedef struct {

  ccs::types::string name = "";
  std::shared_ptr<const ccs::types::AnyType> cfg;
  std::shared_ptr<const ccs::types::AnyType> inp;
  std::shared_ptr<const ccs::types::AnyType> out;

} CVVFFunctionDefinition;

class CVVFClientImpl; // Forward class declaration

/**
 * @brief Interface class providing support for remote incocation of functions.
 * @detail ToDo.
 *
 * @note The design is based on a bridge pattern to avoid exposing implementation
 * specific details through the interface class.
 */

class CVVFClient
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    CVVFClientImpl* __impl;

  protected:

  public:

    /**
     * @brief Constructor.
     * @param service Named RPC service.
     */

    CVVFClient (const char* service);

    /**
     * @brief Destructor.
     */

    virtual ~CVVFClient (void);

    /**
     * @brief Accessor. 
     * @detail Tests if the client is connected to the CVVF service.
     * @return True if connection established, false otherwise.
     */

    bool IsConnected (void) const;

    /**
     * @brief Accessor. 
     * @detail Tests if the CVVF service serves the particular named entity, i.e.
     * either a remote procedure or type specification.
     * @param name Named entity, can be procedure or type specification.
     * @return True if CVVF service holds the named entity, false otherwise.
     */

    bool IsValid (const char* name) const;
 
    /**
     * @brief Accessor. 
     * @detail Tests if the CVVF service holds a function implementation corresponding to 
     * the named parameter.
     * @param name Function name.
     * @return True if CVVF service holds the named function, false otherwise.
     */

    bool HasFunction (const char* name) const;
 
    /**
     * @brief Accessor. 
     * @detail Tests if the CVVF service holds a type specification corresponding to 
     * the named parameter.
     * @param name Type name.
     * @return True if CVVF service holds the named type, false otherwise.
     */

    bool HasType (const char* name) const;

    /**
     * @brief Accessor. 
     * @detail Provide function prototype incl. all the introspectable types required
     * which are associated to the named parameter.
     * @param name Function name.
     * @return Function definition.
     * @code
       sup::core::CVVFClient client ("cvvf-service");

       bool status = client.IsConnected();

       if (status)
         {
           status = client.HasFunction("55a0::types::BestIPWeightsComputation/v1.0");
         }

       sup::core::CVVFFunctionDefinition fundef;

       if (status)
	 {
	   // Query function definition
	   fundef = client.GetFunctionDefinition("55a0::types::BestIPWeightsComputation/v1.0");
	 }

       if (status)
         {
	   // Etc.
	 }
       @endcode
     */

    const CVVFFunctionDefinition GetFunctionDefinition (const char* name) const;

    /**
     * @brief Accessor. 
     * @detail Provide introspectable type definition corresponding to the named parameter.
     * @param name Type name.
     * @return Type definition if successful, invalid pointer otherwise.
     * @code
       sup::core::CVVFClient client ("cvvf-service");

       bool status = client.IsConnected();

       if (status)
         {
           status = client.HasType("55a0::types::BestIPWeights/v1.0");
         }

       std::shared_ptr<const ccs::types::AnyType> type;

       if (status)
	 {
	   // Query type definition
	   type = client.GetTypeDefinition("55a0::types::BestIPWeights/v1.0");
	   status = static_cast<bool>(type);
	 }

       if (status)
         {
           char buffer [1024];
	   status = ccs::HelperTools::Serialise(type, buffer, 1024u);

	   if (status)
	     {
	       log_info("Retrieved type definition '%s'", buffer);
	     }
	 }
       @endcode
     */

    std::shared_ptr<const ccs::types::AnyType> GetTypeDefinition (const char* name) const;

    /**
     * @brief Execute method. 
     * @detail Requests synchronous function execution.
     * @param name Function name.
     * @param cfg Instance of ccs::types::AnyValue hosting the configuration data.
     * @param inp Instance of ccs::types::AnyValue hosting the input variable.
     * @param out Instance of ccs::types::AnyValue which will host the returned variable, if any.
     * @return True if successful, false otherwise.
     * @code
       sup::core::CVVFClient client ("cvvf-service");

       bool status = client.IsConnected();

       if (status)
         {
           status = client.HasFunction("55a0::types::BestIPWeightsComputation/v1.0");
         }

       sup::core::CVVFFunctionDefinition fundef;

       if (status)
	 {
	   // Query function definition
	   fundef = client.GetFunctionDefinition("55a0::types::BestIPWeightsComputation/v1.0");
	 }

       if (status)
         {
	   // Create associated variables
           ccs::types::AnyValue cfg_var (fundef.cfg); // From recovered type
           ccs::types::AnyValue inp_var (fundef.inp); // From recovered type

	   // Fill variable instances
	   // Etc.

           ccs::types::AnyValue out_var; // Invalid instance .. placeholder for returned value

	   status = client.Execute(fundef.name, cfg_var, inp_var, out_var);

	   if (status)
	     {
	       // Process returned variable
	       // Etc.
	     }
	 }
       @endcode
     */

    bool Execute (const char* name, const ccs::types::AnyValue& cfg, const ccs::types::AnyValue& inp, ccs::types::AnyValue& out) const;

    ccs::types::uint32 Launch (const char* name, const ccs::types::AnyValue& cfg, const ccs::types::AnyValue& inp) const;
    bool Abort (const ccs::types::uint32 uid) const;
    bool GetResult (const ccs::types::uint32 uid, ccs::types::AnyValue& out) const;

};

// Global variables

} // namespace core

} // namespace sup

#endif // _CVVFClient_h_ 


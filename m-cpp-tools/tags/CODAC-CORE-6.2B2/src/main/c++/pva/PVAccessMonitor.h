/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva/PVAccessMonitor.h $
* $Id: PVAccessMonitor.h 98392 2019-03-13 20:09:51Z zagara $
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

/**
 * @file PVAccessMonitor.h
 * @brief Header file for PVAccessMonitor class.
 * @date 15/10/2018
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the PVAccessMonitor class.
 */

#ifndef _PVAccessMonitor_h_
#define _PVAccessMonitor_h_

// Global header files

#include <AnyValue.h> // Variable with introspectable data type ..
#include <AnyValueHelper.h> // .. associated helper routines

// Local header files

// Constants

// Type definition

namespace ccs {

namespace base {

class PVAccessMonitor_Impl; // Forward class declaration

/**
 * @brief Interface class providing support for PVA client channel monitor.
 * @detail The base class is instantiated with the name of the PVA channel.
 * The base class constructor and destructor take care of PVA channel
 * life-cycle management.
 *
 * Implementation classes specialise the HandleMonitor method to be notified
 * of PVA channel value change and perform application-specific processing.
 *
 * @code
   class PVAccessMonitorImpl : public ccs::base::PVAccessMonitor
   {

     private:

       std::ofstream __file;

     public:

       PVAccessMonitorImpl (const char* name) : ccs::base::PVAccessMonitor(name) {
         std::string file = std::string("/tmp/") + std::string(this->GetChannel()) + std::string(".record");
         __file.open (file, { std::ios::out | std::ios::trunc })
       };

       virtual ~PVAccessMonitorImpl (void) {
         __file.close(); 
       };

       virtual void HandleMonitor (const ccs::types::AnyValue& value) {

         log_info("Channel '%s' updated at '%lu'", this->GetChannel(), ccs::HelperTools::GetCurrentTime());

	 char buffer [1024u] = STRING_UNDEFINED;
	 bool status = value.SerialiseInstance(buffer, 1024u);

	 if (status)
	 {
	   __file << buffer << std::endl;
	 }

       };

   };

   @endcode
 *
 * @note The design is based on a bridge pattern to avoid exposing PVA specific
 * internals through the interface class.
 */

class PVAccessMonitor
{

  private:

    /**
     * @brief Attribute. 
     * @detail Bridge (PIMPL) pattern.
     */

    PVAccessMonitor_Impl* __impl;

  protected:

  public:

    /**
     * @brief Constructor.
     * @detail Client PVA channel is instantiated and connection
     * established.
     * @param channel Channel name (nil-terminated character array).
     */

    PVAccessMonitor (const char* channel);

    /**
     * @brief Destructor.
     * @detail Cancels ongoing monitor and deletes client PVA channel.
     */

    virtual ~PVAccessMonitor (void);

    /**
     * @brief Accessor.
     * @return True if the channel is currently connected.
     */

    bool IsConnected (void) const;

    /**
     * @brief Accessor. Associated PVA channel name.
     * @return Associated PVA channel name (nil-terminated character array).
     */

    const char* GetChannel (void) const;

    enum Event {
      Connect,
      Disconnect,
    };

    /**
     * @brief Virtual event handler method.
     * @param event Event to be handled.
     */

    virtual void HandleEvent (const Event& event); // Virtual method

    /**
     * @brief Virtual data handler method.
     * @param value Buffer associated to an introspectable type definition.
     */

    virtual void HandleMonitor (const ccs::types::AnyValue& value) = 0; // Pure virtual method

};

// Global variables

// Function declaration

// Function definition

} // namespace base

} // namespace ccs

#endif // _PVAccessMonitor_h_


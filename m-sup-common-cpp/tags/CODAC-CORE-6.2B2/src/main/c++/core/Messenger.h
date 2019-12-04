/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/main/c++/core/Messenger.h $
* $Id: Messenger.h 100236 2019-06-21 11:00:02Z bauvirb $
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
 * @file Messenger.h
 * @brief Header file for Messenger interface class.
 * @date 19/06/2019
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the Messenger interface class.
 */

#ifndef _Messenger_h_
#define _Messenger_h_

// Global header files

#ifdef __cplusplus

#include <chrono> // std::chrono, etc.
#include <string> // std::string, etc.

// Local header files

// Constants

// Type definition

namespace sup {

namespace core {

typedef enum Channel
{
  SDN = 0,
  TCN
} Channel_t;

typedef enum Role
{
  MONITOR = 0,    // SDN Monitor / TCN Monitor
  AGENT           // SDN Participant / TCN Agent
} Role_t;

class MessengerImpl; // Forward class declaration

/**
 * @brief Interface class.
 * @note Used by TCN monitor and SDN monitor Java classes over the discovery protocol.
 * @todo Client/server requires the capability to UCAST reply to sender. Implementation
 * available with RPC service unit tests, see m-cpp-common-epics.
 */

typedef class Messenger
{

  private:

    MessengerImpl* __impl;

  public:

    Messenger (const Channel_t channel, const Role_t role);
    virtual ~Messenger (void);

    virtual bool HandleReceive (const std::string& msg) = 0; // Pure virtual method
    bool Send (const std::string& msg);

} MessengerI; // Backward compatibility

typedef class MessengerClient
{

  private:

    MessengerImpl* __impl;

  public:

    MessengerClient (const enum Channel& channel, const enum Role& role);
    virtual ~MessengerClient (void);

    std::string SendRPCMessage (const std::string& msg, const std::chrono::seconds& timeout = std::chrono::seconds(10));

} MessengerClientI; // Backward compatibility

typedef class MessengerServer
{

  private:

    MessengerImpl* __impl;

  public:

    MessengerServer (const enum Channel& channel, const enum Role& role);
    virtual ~MessengerServer (void);

    virtual std::string HandleRPCMessage (const std::string& msg) = 0; // Pure virtual method

} MessengerServerI; // Backward compatibility

// Global variables

namespace sdn {

const char DiscoveryMCastAddr [] = "239.0.1.2"; // SDN discovery protocol
const uint32_t DiscoveryMCastPort = 10002u; // SDN discovery protocol

} // namespace sdn

namespace tcn {

const char DiscoveryMCastAddr [] = "239.0.2.2"; // TCN discovery protocol
const uint32_t DiscoveryMCastPort = 20002u; // TCN discovery protocol

} // namespace tcn

// Function declaration

// Function definition

} // namespace core

} // namespace sup

extern "C" {

#endif // __cplusplus

// ToDo - Insert C API declaration

#ifdef __cplusplus
} // extern C
#endif // __cplusplus

#endif // _Messenger_h_

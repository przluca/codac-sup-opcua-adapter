/******************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-common-cpp/tags/CODAC-CORE-6.2B2/src/test/c++/cfg-client/cfg-client.cpp $
 * $Id: cfg-client.cpp 101405 2019-08-06 14:39:31Z bauvirb $
 *
 * Project	: CODAC Core System
 *
 * Description	: SDN Software - Communication API - Simple examples
 *
 * Author        : Bertrand Bauvir
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
#include <stdio.h> // sscanf, printf, etc.
#include <string.h> // strncpy, etc.
#include <stdarg.h> // va_start, etc.
#include <signal.h> // sigset, etc.

#include <new> // std::nothrow

#include <types.h> // Misc. type definition, e.g. RET_STATUS
#include <tools.h> // Misc. helper functions, e.g. hash, etc.

#include <log-api.h> // Logging helper functions
#include <CyclicRedundancyCheck.h>

// Local header files

#include "ConfigurationLoader.h"

// Constants

#define DEFAULT_AFFINITY       0
#define DEFAULT_ITERATIONS     1
#define DEFAULT_PAYLOAD     1024 // 1kB
#define DEFAULT_PERIOD   1000000000ul

// Type definition

// Global variables

bool _terminate = false;

// Function definition

void print_usage(void) {

    char prog_name[STRING_MAX_LENGTH] = STRING_UNDEFINED;

    get_program_name((char*) prog_name);

    fprintf(stdout, "Usage: %s <options>\n", prog_name);
    fprintf(stdout, "Options: -h|--help: Print usage.\n");
    fprintf(stdout,
            "         -c|--count <sample_nb>: Stop after <sample_nb> are published, -1 for undefined number of counts (stops with Ctrl-C), defaults to 10000.\n");
    fprintf(stdout, "         -p|--period <period_ns>: Publication period, defaults to 1000000 (1kHz).\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "The program instantiates a MCAST publisher streams <payload> bytes packets on <iface_name> with configurable rate, etc.\n");
    fprintf(stdout, "\n");

    return;

}
;

void signal_handler(int signal) {

    log_info("Received signal '%d' to terminate", signal);
    _terminate = true;

}
;

int main(int argc,
         char **argv) {

    // Install signal handler to support graceful termination
    sigset(SIGTERM, signal_handler);
    sigset(SIGINT, signal_handler);
    sigset(SIGHUP, signal_handler);

    char service[STRING_MAX_LENGTH] = STRING_UNDEFINED;
    ccs::HelperTools::SafeStringCopy(service, "rpc@L3-Component", STRING_MAX_LENGTH);
    char buffer[1000000] = STRING_UNDEFINED;
    char fileName[1024] = STRING_UNDEFINED;
    char alias[32] = STRING_UNDEFINED;

    if (argc > 1) {
        for (uint_t index = 1; index < (uint_t) argc; index++) {
            if ((strcmp(argv[index], "-h") == 0) || (strcmp(argv[index], "--help") == 0)) {
                // Display usage
                print_usage();
                return (0);
            }
            else if ((strcmp(argv[index], "-a") == 0) || (strcmp(argv[index], "--alias") == 0)) {
                if ((index + 1) < (uint_t) argc) {
                    ccs::HelperTools::SafeStringCopy(alias, argv[index + 1], 32u);
                }
                else {
                    print_usage();
                    return (0);
                } // Display usage
                index += 1;

            }
            else if ((strcmp(argv[index], "-c") == 0) || (strcmp(argv[index], "--config") == 0)) {
                if ((index + 1) < (uint_t) argc) {
                    ccs::HelperTools::SafeStringCopy(buffer, argv[index + 1], 1000000u);
                }
                else {
                    print_usage();
                    return (0);
                } // Display usage
                index += 1;

            }
            else if ((strcmp(argv[index], "-f") == 0) || (strcmp(argv[index], "--config-file") == 0)) {
                if ((index + 1) < (uint_t) argc) {
                    ccs::HelperTools::SafeStringCopy(fileName, argv[index + 1], 1024u);
                }
                else {
                    print_usage();
                    return (0);
                } // Display usage

                FILE *cfg;
                cfg = fopen(fileName, "r");
                if (cfg == NULL) {
                    fprintf(stdout, "Error opening file %s.\n", fileName);
                }
                else {
                    fseek(cfg, 0, SEEK_END);
                    long fsize = ftell(cfg);
                    fseek(cfg, 0, SEEK_SET);

                    fread(buffer, 1, fsize, cfg);
                    fclose(cfg);
                }

                index += 1;

            }
            else if ((strcmp(argv[index], "-s") == 0) || (strcmp(argv[index], "--service") == 0)) {
                if ((index + 1) < (uint_t) argc)
                    ccs::HelperTools::SafeStringCopy(service, argv[index + 1], STRING_MAX_LENGTH);
                else {
                    print_usage();
                    return (0);
                } // Display usage
                index += 1;

            }
            else if ((strcmp(argv[index], "-v") == 0) || (strcmp(argv[index], "--verbose") == 0)) {
                // Log to stdout
                ccs::log::Func_t old_plugin = ccs::log::SetStdout();
                ccs::log::Severity_t old_filter = ccs::log::SetFilter(LOG_DEBUG);

            }
        }
    }
    else {
    }

    // Create ConfigurationLoader instance
    log_info("Create RPC client on '%s' service", service);
    sup::core::ConfigurationLoader loader(service);
    ccs::HelperTools::SleepFor(500000000ul);

    ccs::types::AnyValue config;
    std::shared_ptr<const ccs::types::AnyType> config_t;

    // Connect to RPC server
    bool status = loader.IsConnected();

    if (status) {
        log_info("Connected to '%s' service", service);
    }
    else {
        log_error("Unable to connect to '%s'", service);
    }

    if (status) {
        // Snapshot
        status = loader.ReadConfiguration(alias, config);
    }

    if (status) {
        char buffer[1024] = STRING_UNDEFINED;
        config.SerialiseInstance(buffer, 1024u);
        log_info("Current value '%s'", buffer);
    }
    else {
        log_error("Read configuration failed.");
    }

    if (status) {
        config_t = config.GetType();
        status = (config_t ? true : false);
    }

    if (status && !ccs::HelperTools::IsUndefinedString(buffer)) // Load configuration
            {
        status = config.ParseInstance(buffer);

        if (status) {
            status = loader.LoadConfiguration(alias, config);
        }
    }

    if (!status) {
        log_error("Failure");
    }

    // Terminate
    log_info("Terminate program");

    return (0);

}

#ifndef ANY_THREAD_H
#define ANY_THREAD_H

/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/include/any-thread.h $
* $Id: any-thread.h 100703 2019-07-11 15:23:56Z bauvirb $
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
#if 1

#include "AnyThread.h"

#ifndef DEFAULT_THREAD_NAME
#define DEFAULT_THREAD_NAME "undefined"
#endif
#ifndef DEFAULT_THREAD_SCHEDULING_AFFINITY
#define DEFAULT_THREAD_SCHEDULING_AFFINITY 0 /* Core identifier */
#endif
#ifndef DEFAULT_THREAD_SCHEDULING_POLICY
#define DEFAULT_THREAD_SCHEDULING_POLICY SCHED_OTHER /* Scheduling policy */
#endif
#ifndef DEFAULT_THREAD_SCHEDULING_PRIORITY
#define DEFAULT_THREAD_SCHEDULING_PRIORITY 0 /* Scheduling priority */
#endif
#ifndef DEFAULT_THREAD_ACCURACY
#define DEFAULT_THREAD_ACCURACY   10000UL /* 10us */
#endif
#ifndef DEFAULT_THREAD_PERIOD
#define DEFAULT_THREAD_PERIOD 100000000UL /* 10Hz */
#endif
#ifndef DEFAULT_THREAD_PHASE
#define DEFAULT_THREAD_PHASE 0L
#endif

#else

/* Global header files */

#include <pthread.h>

#include <stdio.h> /* sscanf, printf, etc. */
#include <string.h> /* strncpy, etc. */

/* Local header files */

#include "tools.h" /* Misc. helper functions, e.g. hash, etc. */
#include "types.h" /* Global type definition */

//#include "any-object.h" /* Abstract base class definition and associated object database */

/* Constants */

#define DEFAULT_THREAD_NAME "undefined"

#ifndef DEFAULT_THREAD_SCHEDULING_AFFINITY
#define DEFAULT_THREAD_SCHEDULING_AFFINITY 0 /* Core identifier */
#endif
#ifndef DEFAULT_THREAD_SCHEDULING_POLICY
#define DEFAULT_THREAD_SCHEDULING_POLICY SCHED_OTHER /* Scheduling policy */
#endif
#ifndef DEFAULT_THREAD_SCHEDULING_PRIORITY
#define DEFAULT_THREAD_SCHEDULING_PRIORITY 0 /* Scheduling priority */
#endif

#define DEFAULT_THREAD_ACCURACY   10000L /* 10us */
#define DEFAULT_THREAD_PERIOD 100000000L /* 10Hz */
#define DEFAULT_THREAD_PHASE 0

/* Type definition */

namespace ccs {

namespace base {

typedef struct AnyThread_CB {

    void (* cb) (void*);    /* Routine */
    void* attr;             /* Routine attribute */

} AnyThread_CB_t;

class AnyThread
//class AnyThread : public AnyObject
{

  private:

    uint_t m_affinity, m_priority, m_policy;

    bool m_synchronous; /* Thread running synchronously (i.e. periodically on time) */

    AnyThread_CB_t m_do;  /* Thread do routine */
    AnyThread_CB_t m_pre; /* Thread preamble routine */
    AnyThread_CB_t m_pos; /* Thread closing routine */

    /* Initializer methods */
    void Initialize (void);

    /* Miscellaneous methods */
    void Update (void); 

  protected:

    pthread_t m_thread;

  public:

    char m_name [STRING_MAX_LENGTH]; /* Instance name */

    volatile bool m_running;   /* Thread running status */
    volatile bool m_terminate; /* Thread terminate command */
    volatile bool m_trigger;   /* Thread activated on event */

    uint64_t m_accuracy;
    uint64_t m_period;
    int64_t m_phase;

    uint64_t m_curr_time, m_till_time, m_interm;
    void (* m_sleep) (void*); /* Thread sleep routine */

    /* Initializer methods */
    RET_STATUS Launch (uint64_t at_time = 0);
    RET_STATUS Terminate (void);

    /* Accessor methods */
    bool IsRunning (void) { return __sync_bool_compare_and_swap(&m_running, true, true); };
    bool IsSynchronous (void) { return this->m_synchronous; };

    RET_STATUS SetAffinity (uint_t core) { this->m_affinity = core; if (this->IsRunning() == true) return set_thread_affinity_to_core(this->m_thread, this->m_affinity); else return STATUS_SUCCESS; };
    RET_STATUS SetPriority (uint_t priority) { this->m_priority = priority; if (this->IsRunning() == true) return set_thread_priority(this->m_thread, this->m_policy, this->m_priority); else return STATUS_SUCCESS; };
    RET_STATUS SetPriority (uint_t policy, uint_t priority) { this->m_policy = policy; this->m_priority = priority; if (this->IsRunning() == true) return set_thread_priority(this->m_thread, this->m_policy, this->m_priority); else return STATUS_SUCCESS; };

    RET_STATUS SetAttribute (void* attr) { this->m_do.attr = attr; return STATUS_SUCCESS; };
    RET_STATUS SetCallback (void (* cb)(void*)) { this->m_do.cb = cb; return STATUS_SUCCESS; };
    RET_STATUS SetCallback (void (* cb)(void*), void* attr) { this->m_do.attr = attr; this->m_do.cb = cb; return STATUS_SUCCESS; };

    RET_STATUS SetPreamble (void (* cb)(void*)) { this->m_pre.cb = cb; return STATUS_SUCCESS; };
    RET_STATUS SetPreamble (void (* cb)(void*), void* attr) { this->m_pre.attr = attr; this->m_pre.cb = cb; return STATUS_SUCCESS; };
    RET_STATUS SetPostface (void (* cb)(void*)) { this->m_pos.cb = cb; return STATUS_SUCCESS; };
    RET_STATUS SetPostface (void (* cb)(void*), void* attr) { this->m_pos.attr = attr; this->m_pos.cb = cb; return STATUS_SUCCESS; };

    RET_STATUS SetAccuracy (uint64_t accuracy) { this->m_accuracy = accuracy; return STATUS_SUCCESS; };
    RET_STATUS SetPeriod (uint64_t period); /* Zero means that the application-specific callback is blocking */ 
    RET_STATUS SetPhase (int64_t phase) { this->m_phase = phase; return STATUS_SUCCESS; };
    //RET_STATUS SetPeriod (uint64_t period, int64_t phase = DEFAULT_THREAD_PHASE, uint64_t accuracy = DEFAULT_THREAD_ACCURACY);

    RET_STATUS SetNextTime (uint64_t at_time) { this->m_till_time = at_time; return STATUS_SUCCESS; };
    RET_STATUS SetSynchronous (bool flag = true);

    /* Miscellaneous methods */
    virtual void Preamble (void) { if (this->m_pre.cb != NULL) (*(this->m_pre.cb))(this->m_pre.attr); return; }; /* Can be overloaded in specialized implementations */
    virtual void Closing (void) { if (this->m_pos.cb != NULL) (*(this->m_pos.cb))(this->m_pos.attr); return; }; /* Can be overloaded in specialized implementations */
    virtual void Do (void) { if (this->m_do.cb != NULL) (*(this->m_do.cb))(this->m_do.attr); return; }; /* Can be overloaded in specialized implementations */
    virtual void Sleep (void) { if (this->m_sleep != NULL) (*(this->m_sleep))(this); return; }; /* Can be overloaded in specialized implementations */

    RET_STATUS Trigger (void) { RET_STATUS status = STATUS_ERROR; if ((this->IsRunning() == true) && (this->IsSynchronous() == false)) status = ((__sync_val_compare_and_swap(&(this->m_trigger), false, true)) ? STATUS_ERROR : STATUS_SUCCESS); return status; };

    /* Constructor methods */
    AnyThread (void) { this->Initialize(); /* this->Launch(); */ return; };
    AnyThread (const char* name) { this->Initialize(); sstrncpy(this->m_name, name, STRING_MAX_LENGTH); /* this->Launch(); */ return; };

    AnyThread (void (* cb)(void*)) { this->Initialize(); this->SetCallback(cb); this->Launch(); return; };
    AnyThread (void (* cb)(void*), void* attr) { this->Initialize(); this->SetCallback(cb, attr); this->Launch(); return; };

    /* Destructor method */
    virtual ~AnyThread (void) { this->Terminate(); return; };

    /* Display methods */

};

/* Global variables */

/* Function declaration */

/* Function definition */

} /* namespace base */

} /* namespace ccs */
#endif
#endif /* ANY_THREAD_H */

program Adapter002

// Added BBR - Optional logging for debugging purposes
%%#include <common/log-wrap.h>

%%#include "FUEL_FIG_PSOS.h"

%%#include "COSInterface.h"

int COS_OPREQ;
assign COS_OPREQ to "{CBS_COS}:COS-OPREQ"; monitor COS_OPREQ;

#define IsCOS(state) COS_OPREQ == COS_Request_ ## state

int COS_OPSTATE;
assign COS_OPSTATE to "{CBS_COS}:COS-OPSTATE"; 

// Modified BBR
#define PSOS_State_Undefined 0
#define PSOS_State_Off 1
#define PSOS_State_Init 2
#define PSOS_State_Idle 3
#define PSOS_State_Config 4
#define PSOS_State_Running 5
#define PSOS_State_Error 6

// Added BBR - Effects are managed as pvPut in lieu of calling C routines
#define PSOS_Request_Undefined 0
#define PSOS_Request_Exit 1
#define PSOS_Request_Init 2
#define PSOS_Request_Config 4
#define PSOS_Request_Start 5
#define PSOS_Request_Stop 6

int PSOS_OPSTATE;
// Modified BBR - Use PSOS as defined by SDD translator and INP/OUT links
//assign PSOS_OPSTATE to "{CBS_PSOS}:PSOS-OPSTATE"; monitor PSOS_OPSTATE;
assign PSOS_OPSTATE to "{CBS_PSOS}:{VAR_OPSTATE}"; monitor PSOS_OPSTATE;

#define IsPSOS(state) PSOS_OPSTATE == PSOS_State_ ## state

// Added BBR - Set PSOS request
#define SetPSOS(request) PSOS_OPEVENT = PSOS_Request_ ## request; pvPut(PSOS_OPEVENT,SYNC); wait_for(1,0)

int PSOS_OPEVENT;
// Modified BBR - Use PSOS as defined by SDD translator and INP/OUT links
//assign PSOS_OPEVENT to "{CBS_PSOS}:PSOS-OPREQ"; 
assign PSOS_OPEVENT to "{CBS_PSOS}:{VAR_OPREQ}"; 

// Added BBR - Delay management
%%static inline void wait_for (int sec, int nsec) { struct timespec time; time.tv_sec = sec; while (nsec >= 1000000000L) { time.tv_sec += 1; nsec -= 1000000000L; } time.tv_nsec = nsec; nanosleep(&time, NULL); return ; };

evflag resetFlag;

ss LifeCycle {

   // Initial state
   state Initial { 
         entry {
	      log_info("LifeCycle Initial state");
	 }
   	 when (pvConnectCount() == pvAssignCount()) {} state Connected 
   	 when (pvConnectCount() < pvAssignCount()) {} state Disconnected 
   }

   state Connected { 
         entry {
	      log_info("LifeCycle Connected state");
	 }
   	 when (pvConnectCount() < pvAssignCount()) {
	      log_warning("PV disconnected !!");
	      efClear(resetFlag);
	 } state Disconnected 
   }

   state Disconnected { 
         entry {
	      log_info("LifeCycle Disconnected state");
	 }
   	 when (delay(1.0)) {
	      log_info("Try and re-establish connection !!");
// Added BBR - Is there a way to make this faster by forcing the sequencer to re-connect
	 } state Disconnected 
   	 when (pvConnectCount() == pvAssignCount()) {
	      log_notice("PV connected !!");
	      log_info("Re-initialise adapter state machine !!");
	      efSet(resetFlag);
	 } state Connected 
   }

}

ss Adapter002 {

   // Initial state
   state Initial { 
         entry {
	      log_info("Adapter Initial state");
	 }
   	 when () {} state Undefined 
   }

// Added BBR - Start in the appropriate COS state
    state Undefined {
        entry {
	    log_info("Entering COS Undefined state");
            COS_OPSTATE = COS_State_Undefined; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Off) ) {
	     log_info("PSOS is Off - Restore adapter state accordingly");
        } state Off
        when ( IsPSOS(Init) ) {
	     log_info("PSOS is Init - Restore adapter state accordingly");
        } state NotReady
        when ( IsPSOS(Error) ) {
	     log_info("PSOS is Error - Restore adapter state accordingly");
        } state Fault
        when ( IsPSOS(Idle) ) {
	     log_info("PSOS is Idle - Restore adapter state accordingly");
        } state Ready
        when ( IsPSOS(Config) ) {
	     log_info("PSOS is Config - Restore adapter state accordingly");
        } state Initialising
        when ( IsPSOS(Running) ) {
	     log_info("PSOS is Running - Restore adapter state accordingly");
        } state Executing
    }

// Added BBR - Manage the PSOS off state - To be added to the model
    state Off {
        entry {
	      log_info("Entering COS Off state");
            COS_OPSTATE = COS_State_Off; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Error) ) {
	     log_info("PSOS is Error");
        } state Fault
        when ( IsPSOS(Init) || IsCOS(SwitchOn) ) {
// Modified BBR - Effects are managed as pvPut in lieu of calling C routines
   	    log_info("Invoke Initialise()");
            SetPSOS(Init);
            //Initialise();
        } state NotReady
// Added BBR - Reset upon re-connect
	when (efTestAndClear(resetFlag)) {} state Undefined	
    }

    state NotReady {
        entry {
log_info("Entering COS NotReady state");
            COS_OPSTATE = COS_State_NotReady; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Error) ) {
log_info("PSOS is Error");
        } state Fault
        when ( IsPSOS(Idle) ) {
log_info("PSOS is Idle");
        } state Ready
// Added BBR - Reset upon re-connect
	when (efTestAndClear(resetFlag)) {} state Undefined	
    }

    state Executing {
        entry {
log_info("Entering COS Executing state");
            COS_OPSTATE = COS_State_Executing; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Error) ) {
log_info("PSOS is Error");
        } state Fault
        when ( IsPSOS(Idle) || IsCOS(Terminate) || IsCOS(PostCheck) ) {
// Modified BBR - Effects are managed as pvPut in lieu of calling C routines
log_info("Invoke Stop()");
            SetPSOS(Stop); wait_for(2,500000000);
            //Stop();
        } state Ready
// Added BBR - Reset upon re-connect
	when (efTestAndClear(resetFlag)) {} state Undefined	
    }

    state Fault {
        entry {
log_info("Entering COS Fault state");
            COS_OPSTATE = COS_State_Fault; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Error) ) {
log_info("PSOS is Error");
        } state Fault
        when ( IsPSOS(Idle) ) {
log_info("PSOS is Idle");
        } state Ready
// Added BBR - Reset upon re-connect
	when (efTestAndClear(resetFlag)) {} state Undefined	
    }

    state Initialised {
        entry {
log_info("Entering COS Initialised state");
            COS_OPSTATE = COS_State_Initialised; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Error) ) {
log_info("PSOS is Error");
        } state Fault
        when ( IsCOS(Abort) ) {
        } state Ready
        when ( IsPSOS(Running) || IsCOS (Execute) ) {
// Modified BBR - Effects are managed as pvPut in lieu of calling C routines
log_info("Invoke Start()");
            SetPSOS(Start);
            //Start();
        } state Executing
// Added BBR - Reset upon re-connect
	when (efTestAndClear(resetFlag)) {} state Undefined	
    }

    state Initialising {
        entry {
log_info("Entering COS Initialising state");
            COS_OPSTATE = COS_State_Initialising; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Error) ) {
log_info("PSOS is Error");
        } state Fault
        when ( IsPSOS(Idle) ) {
log_info("PSOS is Idle");
        } state Initialised
// Added BBR - Reset upon re-connect
	when (efTestAndClear(resetFlag)) {} state Undefined	
    }

    state Ready {
        entry {
log_info("Entering COS Ready state");
            COS_OPSTATE = COS_State_Ready; pvPut(COS_OPSTATE,SYNC); wait_for(1,0);
        }
        when ( IsPSOS(Error) ) {
log_info("PSOS is Error");
        } state Fault
        when ( IsCOS(Initialise) ) {
// Modified BBR - Effects are managed as pvPut in lieu of calling C routines
log_info("Invoke Configure()");
            SetPSOS(Config);
            //Configure();
        } state Initialising
        when ( IsPSOS(Running) ) {
log_info("PSOS is Running");
        } state Executing
// Added BBR - Reset upon re-connect
	when (efTestAndClear(resetFlag)) {} state Undefined	
    }

}

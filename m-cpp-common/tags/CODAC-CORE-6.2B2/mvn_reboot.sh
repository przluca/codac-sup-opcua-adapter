#!/bin/bash

#+======================================================================
#+======================================================================

function reboot_host {

    HOST=${1-${HOSTNAME}}

    # Reboot ...
    xterm -T "Reboot ${HOST} ..." -e "ssh codac-dev@${HOST} 'sudo reboot'" > /dev/null 2>&1 &

}

if [ X${1} == Xicm ]; then 

    # Perform on all ICM machines ...
    . ./icm_machines.sh; HOSTS=${ICM_MACHINES}
   
fi

if [ X${1} == X ]; then 

    # This host ...
    HOSTS=${HOSTNAME}
   
else

    HOSTS=${1}

fi

for HOST in ${HOSTS}; do
    
    # Reboot ...
    reboot_host ${HOST}

fi


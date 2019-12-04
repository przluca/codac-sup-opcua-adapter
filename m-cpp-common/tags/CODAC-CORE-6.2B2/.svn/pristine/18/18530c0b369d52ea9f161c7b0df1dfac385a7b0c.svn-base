#!/bin/bash

#+======================================================================
#+======================================================================

#rpm -e --nodeps $(rpm -qa --qf "%{NAME}\n" "${CODAC_RPM_PREFIX}-cpp-common*")

function remove_package {

    HOST=${1-${HOSTNAME}}

    # Remove ...
    xterm -T "Remove from ${HOST} ..." -e "ssh codac-dev@${HOST} 'sudo rpm -e --nodeps $(rpm -qa --qf "%{NAME}\n" "${CODAC_RPM_PREFIX}-cpp-common*")'" &

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
    
    # Remove ...
    remove_package ${HOST}
    
done


#!/bin/bash

#+======================================================================
#+======================================================================

function deploy_package {

    HOST=${1-${HOSTNAME}}

    # Deploy ...
    scp ./target/${CODAC_RPM_PREFIX}-cpp-common-*.rpm codac-dev@${HOST}:/tmp

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
    
    # Deploy ...
    deploy_package ${HOST}
    
done


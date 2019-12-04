#!/bin/bash

#+======================================================================
#+======================================================================

function install_package {

    HOST=${1-${HOSTNAME}}

    # Install ...
    xterm -T "Install on ${HOST} ..." -e "ssh codac-dev@${MACHINE} 'sudo yum install -y /tmp/*cpp-common*.rpm'" &

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
    
    # Install
    install_package ${HOST}

done


#!/bin/bash
#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common-opcua/tags/CODAC-CORE-6.2B2/src/test/c++/test.sh $
# $Id: test.sh 101424 2019-08-07 11:28:18Z bauvirb $
#
# Project       : CODAC Core System
#
# Description   : Test script
#
# Author        : B.Bauvir (IO)
#
# Copyright (c) : 2010-2019 ITER Organization,
#                 CS 90 046
#                 13067 St. Paul-lez-Durance Cedex
#                 France
#
# This file is part of ITER CODAC software.
# For the terms and conditions of redistribution or use of this software
# refer to the file ITER-LICENSE.TXT located in the top level directory
# of the distribution package.
#
#-======================================================================

TEST_SCRIPT_DIR=`dirname $0`

no_error=0
error=1
retcode=${no_error}

echo "***** Starting test OPC UA server *****"

/usr/bin/screen -d -m ${MARTe2_DIR}/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f ./unit/OPCUAClient-tests.marte -s Running > /tmp/srv.out
sleep 5

echo "***** Starting unit tests *****"

../../../target/bin/unit-tests
retcode=$?

echo "***** End of unit tests *****"

echo "***** Starting regression tests *****"

../../../target/bin/regression-tests
retcode=$?

echo "***** End of regression tests *****"

echo "***** Stopping test OPC UA server *****"

/usr/bin/kill -9 `/usr/sbin/pidof MARTeApp.ex`

exit ${retcode}

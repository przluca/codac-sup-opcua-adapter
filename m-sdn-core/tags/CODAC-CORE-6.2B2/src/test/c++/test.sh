#!/bin/bash
#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/test.sh $
# $Id: test.sh 97179 2019-01-24 13:06:54Z bauvirb $
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

echo "***** Starting unit tests *****"

../../../target/bin/unit-tests

if [ $? -ne 0 ]; then
  retcode=${error}
fi

echo "***** End of unit tests *****"

echo "***** Starting regression tests *****"

../../../target/bin/regression-tests

if [ $? -ne 0 ]; then
  retcode=${error}
fi

echo "***** End of regression tests *****"

exit ${retcode}

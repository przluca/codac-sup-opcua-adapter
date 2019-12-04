#!/bin/bash
#+======================================================================
# $HeadURL: https://svnpub.codac.iter.org/codac/iter/codac/dev/units/m-cpp-common/branches/codac-core-6.1/src/test/c++/test.sh $
# $Id: test.sh 96475 2019-01-09 13:47:38Z sivecm $
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
#retcode=$?

echo "***** End of unit tests *****"

##echo "***** Starting regression tests *****"

##../../../target/bin/regression-tests
##retcode=$?

##echo "***** End of regression tests *****"

##echo "***** Generating coverage report *****"

##INCDIR="../../../target/cov/main/c++/include"
##SRCDIR="../../../target/cov/main/c++/library"
##OBJDIR="../../../target/cov/library"
##OUTDIR="../../../target/cov/html"

##mkdir -p ${INCDIR}
##mkdir -p ${SRCDIR}

# Copy the sources
##cp ../../../target/main/c++/include/*.h ${INCDIR}/.
##cp ../../../target/main/c++/library/*.cpp ${SRCDIR}/.
# Copy the test logs with the sources
##cp ${OBJDIR}/*.gcda ${SRCDIR}/.
##cp ${OBJDIR}/*.gcno ${SRCDIR}/.
# Generate the coverage analysis report
##lcov -o ../../../target/cov/cpp-common.cov -c -d ${SRCDIR}/.
##genhtml -o ${OUTDIR} ../../../target/cov/cpp-common.cov

##echo "Coverage report available at ${OUTDIR}/index.html"

exit ${retcode}

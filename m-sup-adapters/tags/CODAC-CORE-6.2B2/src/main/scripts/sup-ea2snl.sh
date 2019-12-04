#!/bin/bash
########################################################################
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sup-adapters/tags/CODAC-CORE-6.2B2/src/main/scripts/sup-ea2snl.sh $
# $Id: sup-ea2snl.sh 96477 2019-01-09 14:53:55Z sivecm $
#
# Project       : <Project name>
#
# Description   : A Test script to test correct performance of the psps-extract.py script
#
# Author(s)     : Kevin Meyer, Cosylab
#
# Copyright (c) : 2010-2019 ITER Organization,
#                 CS 90 046
#                 13067 St. Paul-lez-Durance Cedex
#                 France
#
# This file is part of the ITER CODAC software.
# For the terms and conditions of redistribution or use of this software
# refer to the file ITER-LICENSE.TXT located in the top level directory
# of the distribution package.
########################################################################

# Simple script that wraps the call to Saxon 

if [ -f $CODAC_ROOT/examples/sup/snl/xmi2snl_adapter001.xsl ]; then
    RES_DIR=$CODAC_ROOT/examples/sup/snl/xmi2snl_adapter001.xsl
else
    RES_DIR=$TEST_DIR/../main/resources/xmi2snl_adapter001.xsl
fi

if [ ! -f $RES_DIR ]; then
    echo "Configuration error, required resource ($RES_DIR) does not exist!"
    exit 3
fi

saxon_path=`build-classpath saxon91/saxon9.jar`
rc=$?
if [ $? -ne 0 ]; then
    echo "Configuration error, saxon is not installed!"
    exit 3
fi

if [ "$#" -ne 2 ]; then
  echo "Usage: sup-ea2snl MODEL TOP_STATE"
  echo "  where:"
  echo "     MODEL is the EA-generated XMI file (e.g. model.xml) and"
  echo "     TOP_STATE is the name of the top state machine (e.g. StateMachine)"
  echo ""
  
  exit 2
fi

if [ "$#" -eq 2 ]; then
	if [ ! -f $1 ]; then
		echo "SOURCE input file $1 does not exist!"
		exit 3
	fi

    echo "Parsing model in $1, with top state of $2"
    java -jar $saxon_path -xsl:$RES_DIR -s:$1 top_state=$2
    ret_code=$?
    echo "Returned: $ret_code"
    exit 
fi

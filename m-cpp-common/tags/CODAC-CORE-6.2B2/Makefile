#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/Makefile $
# $Id: Makefile 101427 2019-08-07 11:55:16Z bauvirb $
#
# Project       : CODAC Core System
#
# Description   : C++ Makefile
#
# Author        : This file was generated by CODAC development toolkit
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

SUBDIRS=./src/main/c++ ./src/test/c++

BOLD=\e[1m
NC=\e[0m

all:

%:
	@$(foreach dir, $(SUBDIRS), echo -e "$(BOLD)Building $(dir:/=)...$(NC)" && $(MAKE) -C $(dir) $@ &&) :
#	echo -e "$(BOLD)Building ./main/c++ ...$(NC)" && $(MAKE) -C ./main/c++ $@
#	echo -e "$(BOLD)Building ./test/c++ ...$(NC)" && $(MAKE) -C ./test/c++ $@

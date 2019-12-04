#======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/main/c++/pva-record/Makefile $
# $Id: Makefile 99177 2019-04-18 17:47:43Z bauvirb $
#
# Project       : m-cpp-common
#
# Description   : Makefile
#
# Author        : Bertrand Bauvir (IO)
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

PROGNAME = pva-record

C=gcc
CC=g++

#INCLUDE_DIR := ../../../main/c++/include 
#INCLUDE_DIR += ../../../main/c++/base
#INCLUDE_DIR += ../../../main/c++/common
INCLUDE_DIR += ../../../main/c++/pva
#INCLUDE_DIR += ../../../main/c++/tools
#INCLUDE_DIR += ../../../main/c++/types
#INCLUDE_DIR += $(CODAC_ROOT)/include
INCLUDE_DIR += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux $(EPICS_BASE)/include/compiler/gcc
INCLUDE_DIR += $(CODAC_ROOT)/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base
INCLUDE_DIR += $(CODAC_ROOT)/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/include
INCLUDE_DIR += $(CODAC_ROOT)/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/types
INCLUDE_DIR += $(CODAC_ROOT)/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/tools
INCLUDE_DIR += $(CODAC_ROOT)/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/common

LIBRARY_DIR := ../../../../../../lib
LIBRARY_DIR += $(CODAC_ROOT)/lib
LIBRARY_DIR += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)

SOURCE_DIR := .
TARGET_DIR := $(CODAC_ROOT)

BINARY_DIR := $(TARGET_DIR)/bin
OBJECT_DIR := $(TARGET_DIR)/cov/$(PROGNAME)

SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)

OBJECTS := $(addprefix $(OBJECT_DIR)/,$(patsubst %.cpp,%.obj,$(notdir $(SOURCES))))
OBJECTS += $(wildcard $(TARGET_DIR)/obj/library/*.obj)

DEPENDS := rt pthread 
DEPENDS += ccs-common ccs-base ccs-types ccs-pva
## For thread
DEPENDS += ccs-core ccs-pva
DEPENDS += pvData pvAccess pvDatabase

EXECUTABLE := $(BINARY_DIR)/$(PROGNAME)

CCFLAGS := -c -g -std=c++11
#CCFLAGS += -pedantic -Wall -Wextra
#CCFLAGS += -fprofile-arcs -ftest-coverage --coverage -fno-inline -fno-inline-small-functions -fno-default-inline
CCFLAGS += $(foreach dir,$(INCLUDE_DIR),-I$(dir))

LDFLAGS := $(foreach dir,$(LIBRARY_DIR),-Wl,-rpath,$(dir))
LDFLAGS += $(foreach dir,$(LIBRARY_DIR),-L$(dir))
LDFLAGS += --coverage

LDLIBS := $(foreach libs,$(DEPENDS),-l$(libs))

all: $(SOURCES) $(EXECUTABLE)

clean:
	@$(RM) $(PROGNAME) *.o
	@$(RM) -rf $(OBJECT_DIR)

run: $(SOURCES) $(EXECUTABLE)
	$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BINARY_DIR)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

$(OBJECT_DIR)/%.obj: $(SOURCE_DIR)/%.cpp
	mkdir -p $(OBJECT_DIR)
	$(CC) $(CCFLAGS) $< -o $@

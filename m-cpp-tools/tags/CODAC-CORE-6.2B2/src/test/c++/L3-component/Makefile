#======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/L3-component/Makefile $
# $Id: Makefile 96477 2019-01-09 14:53:55Z sivecm $
#
# Project       : m-cpp-common
#
# Description   : Makefile
#
# Author        : William Ledda (Vitrociset S.p.A.)
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

PROGNAME = L3-component

CC=g++

INCLUDE_DIR := ../../../main/c++/include
INCLUDE_DIR += ../../../main/c++/common ../../../main/c++/base ../../../main/c++/types ../../../main/c++/tools ../../../main/c++/pva ../../../main/c++/ca
INCLUDE_DIR += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux $(EPICS_BASE)/include/compiler/gcc 

LIBRARY_DIR := ../../../../target/lib
LIBRARY_DIR += $(CODAC_ROOT)/lib
LIBRARY_DIR += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)

SOURCE_DIR := .
TARGET_DIR := ../../../../target

BINARY_DIR := $(TARGET_DIR)/bin
OBJECT_DIR := $(TARGET_DIR)/obj/$(PROGNAME)

SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS := $(addprefix $(OBJECT_DIR)/,$(patsubst %.cpp,%.obj,$(notdir $(SOURCES))))

DEPENDS := rt pthread 
DEPENDS += ccs-common ccs-base ccs-types ccs-pva ccs-ca
## For thread
DEPENDS += ccs-core
DEPENDS += pvData pvAccess pvDatabase ca

EXECUTABLE := $(BINARY_DIR)/$(PROGNAME)

CCFLAGS := -c -g -std=c++11
#CCFLAGS += -pedantic -Wall -Wextra
CCFLAGS += $(foreach dir,$(INCLUDE_DIR),-I$(dir))

LDFLAGS := $(foreach dir,$(LIBRARY_DIR),-Wl,-rpath,$(dir))
LDFLAGS += $(foreach dir,$(LIBRARY_DIR),-L$(dir))

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

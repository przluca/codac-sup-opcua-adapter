#======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-tools/tags/CODAC-CORE-6.2B2/src/test/c++/unit/Makefile $
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

PROGNAME = unit-tests

C=gcc
CC=g++

INCLUDE_DIR := ../../../main/c++/include 
INCLUDE_DIR += ../../../main/c++/base
INCLUDE_DIR += ../../../main/c++/common
INCLUDE_DIR += ../../../main/c++/tools
INCLUDE_DIR += ../../../main/c++/types
INCLUDE_DIR += ../../../main/c++/pva
INCLUDE_DIR += $(CODAC_ROOT)/include
INCLUDE_DIR += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux $(EPICS_BASE)/include/compiler/gcc

LIBRARY_DIR := ../../../../target/lib
LIBRARY_DIR += $(CODAC_ROOT)/lib
LIBRARY_DIR += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)

SOURCE_DIR := .
TARGET_DIR := ../../../../target

BINARY_DIR := $(TARGET_DIR)/bin
OBJECT_DIR := $(TARGET_DIR)/cov/$(PROGNAME)

CSOURCES := $(wildcard $(SOURCE_DIR)/*.c)
CCSOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCES := $(CSOURCES) $(CCSOURCES)

OBJECTS := $(addprefix $(OBJECT_DIR)/,$(patsubst %.c,%.cov,$(notdir $(CSOURCES))))
OBJECTS += $(addprefix $(OBJECT_DIR)/,$(patsubst %.cpp,%.cov,$(notdir $(CCSOURCES))))
OBJECTS += $(wildcard $(TARGET_DIR)/cov/library/*.cov)

DEPENDS := rt pthread 
DEPENDS += gtest gtest_main
DEPENDS += ca
DEPENDS += pvData pvAccess pvDatabase

EXECUTABLE := $(BINARY_DIR)/$(PROGNAME)

CFLAGS := -c -g
#CFLAGS += -pedantic -Wall -Wextra
#CFLAGS += --coverage
CFLAGS += $(foreach dir,$(INCLUDE_DIR),-I$(dir))

CCFLAGS := -c -g -std=c++11
#CCFLAGS += -pedantic -Wall -Wextra
CCFLAGS += -fprofile-arcs -ftest-coverage --coverage -fno-inline -fno-inline-small-functions -fno-default-inline
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

$(OBJECT_DIR)/%.cov: $(SOURCE_DIR)/%.cpp
	mkdir -p $(OBJECT_DIR)
	$(CC) $(CCFLAGS) $< -o $@

$(OBJECT_DIR)/%.cov: $(SOURCE_DIR)/%.c
	mkdir -p $(OBJECT_DIR)
	$(C) $(CFLAGS) $< -o $@

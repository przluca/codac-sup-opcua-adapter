#======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-sdn-core/tags/CODAC-CORE-6.2B2/src/test/c++/regression/Makefile $
# $Id: Makefile 99199 2019-04-19 10:35:18Z bauvirb $
#
# Project       : m-log-lib
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

PROGNAME = regression-tests

C=gcc
CC=g++

INCLUDE_DIR := ../../../main/c++/include 
INCLUDE_DIR += $(CODAC_ROOT)/include

LIBRARY_DIR := ../../../../target/lib
LIBRARY_DIR += $(CODAC_ROOT)/lib

SOURCE_DIR := .
TARGET_DIR := ../../../../target

BINARY_DIR := $(TARGET_DIR)/bin
## Bug 10805 - Coverage report handled through MVN
#OBJECT_DIR := $(TARGET_DIR)/cov/$(PROGNAME)
#OBJECT_DIR := $(TARGET_DIR)/obj/$(PROGNAME)
OBJECT_DIR := $(SOURCE_DIR)/.obj

CSOURCES := $(wildcard $(SOURCE_DIR)/*.c)
CCSOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCES := $(CSOURCES) $(CCSOURCES)

#OBJECTS := $(addprefix $(OBJECT_DIR)/,$(patsubst %.c,%.cov,$(notdir $(CSOURCES))))
#OBJECTS += $(addprefix $(OBJECT_DIR)/,$(patsubst %.cpp,%.cov,$(notdir $(CCSOURCES))))
#OBJECTS += $(wildcard $(TARGET_DIR)/cov/library/*.cov)
OBJECTS := $(addprefix $(OBJECT_DIR)/,$(patsubst %.c,%.obj,$(notdir $(CSOURCES))))
OBJECTS += $(addprefix $(OBJECT_DIR)/,$(patsubst %.cpp,%.obj,$(notdir $(CCSOURCES))))
OBJECTS += $(wildcard $(TARGET_DIR)/main/c++/library/.obj/*.obj)

DEPENDS := rt pthread 
DEPENDS += ccs-core 
DEPENDS += gtest gtest_main

EXECUTABLE := $(BINARY_DIR)/$(PROGNAME)

CFLAGS := -c -g
#CFLAGS += -pedantic -Wall -Wextra
CFLAGS += $(foreach dir,$(INCLUDE_DIR),-I$(dir))

CCFLAGS := -c -g -std=c++11
#CCFLAGS += -pedantic -Wall -Wextra
CCFLAGS += $(foreach dir,$(INCLUDE_DIR),-I$(dir))
CCFLAGS += `xml2-config --cflags`

LDFLAGS := $(foreach dir,$(LIBRARY_DIR),-Wl,-rpath,$(dir))
LDFLAGS += $(foreach dir,$(LIBRARY_DIR),-L$(dir))

## Bug 10805 - Coverage report handled through MVN
ifeq ($(COVERAGE),true)
CFLAGS += -DDEBUG -O0 -g --coverage
CCFLAGS += -DDEBUG -O0 -g -fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline --coverage
LDFLAGS += --coverage
endif

LDLIBS := $(foreach libs,$(DEPENDS),-l$(libs))
LDLIBS += `xml2-config --libs`

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

$(OBJECT_DIR)/%.obj: $(SOURCE_DIR)/%.c
	mkdir -p $(OBJECT_DIR)
	$(C) $(CFLAGS) $< -o $@

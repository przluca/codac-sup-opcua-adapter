# Makefile to build a stand-alone state-machine from an EPICS SNL .st file.
# Set the name of the .st file below

ifndef SRC
SRC=pcs-adapter
else
# call with SRC=target parameter, e.g. make SRC=SUPPulseCounter
endif

# Temporary directory where to build the intermediate files
ODIR=obj

# single source. If there are multiple sources see below...
SRCI=$(SRC).st
SRCC=$(ODIR)/$(SRC).c
OBJS=$(ODIR)/$(SRC).o

# Multiple sources must be expanded: Specify as a comma separated list
# DOES NOT WORK.. which one is main?
#SRCS=eg1,eg2
#SRCI=$(shell echo {$(SRCS)}.st)
#SRCC=$(shell echo $(ODIR)/{$(SRCS)}.c)
#OBJS=$(shell echo $(ODIR)/{$(SRCS)}.o)

# EPICS sequencer libraries
LIBS=-lseq -lpv -lCom

# Default target is the first one specified. By convention, called "all"
# Adding target "clean" as solution to intermittent Jenkins failure. "[WARN] /bin/sh: obj/pcs-adapter.i: No such file or directory"
all: $(ODIR) $(SRC)

# The compiler to use
CC=gcc

# The platform (Not the same as host architecture, set by EPICS)
OS=Linux

# The list of directories to search for header files.
# This must be comma separated list
IDIRS=.,$(CODAC_ROOT)/include,$(CODAC_ROOT)/include/sup,$(EPICS_MODULES)/sncseq/include,$(EPICS_BASE)/include,$(EPICS_BASE)/include/compiler/$(CC),$(EPICS_BASE)/include/os/$(OS),$(EPICS_BASE)/src/libCom/osi/os/$(OS)
CFLAGS=$(shell echo -I{$(IDIRS)})

# This must be comma separated list
LDIRS=$(CODAC_ROOT)/lib,$(EPICS_MODULES)/sncseq/lib/$(EPICS_HOST_ARCH),$(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)
LDIR=$(shell echo -L{$(LDIRS)})

RPATHDIR=$(shell echo -Wl,-rpath={$(LDIRS)})

_DEPSI = $(SRCI)
DEPSI = $(patsubst %,$(IDIR)/%,$(_DEPSI))

_DEPS = $(SRCC)
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = $(SRC)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Get a .i from a .st
$(ODIR)/%.i: %.st
	$(CC) -E -x c $< > $@

# Get a .c from a .i
$(ODIR)/%.c: $(ODIR)/%.i
	snc +m $< -o $@

# Get a .o from a .c
$(ODIR)/%.o: $(ODIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

# Get the target from .o list
$(SRC): $(OBJS) 
	$(CC) -o $@ $^ $(LIBS) $(LDIR) $(RPATHDIR)

.PHONY: clean

$(ODIR):
	mkdir -p $(ODIR)

clean:
	rm -f $(SRC) -rf $(ODIR) *~ 


#
# OMNeT++/OMNEST Makefile for libstorage-simulation
#
# This file was generated with the command:
#  opp_makemake -f --deep --make-so -O out
#

# Name of target to be created (-o option)
TARGET = libstorage-simulation$(SHARED_LIB_SUFFIX)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -Iresults \
    -Istorage \
    -Istorage/algorithms \
    -Istorage/networks \
    -Istorage/networks/results \
    -Istorage/server

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = $O/storage/networks/Routing.o $O/storage/networks/Switch.o $O/storage/networks/Net.o $O/storage/networks/Server.o

# Message files
MSGFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(TARGET)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f storage-simulation storage-simulation.exe libstorage-simulation.so libstorage-simulation.a libstorage-simulation.dll libstorage-simulation.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h
	$(Q)-rm -f results/*_m.cc results/*_m.h
	$(Q)-rm -f storage/*_m.cc storage/*_m.h
	$(Q)-rm -f storage/algorithms/*_m.cc storage/algorithms/*_m.h
	$(Q)-rm -f storage/networks/*_m.cc storage/networks/*_m.h
	$(Q)-rm -f storage/networks/results/*_m.cc storage/networks/results/*_m.h
	$(Q)-rm -f storage/server/*_m.cc storage/server/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc results/*.cc storage/*.cc storage/algorithms/*.cc storage/networks/*.cc storage/networks/results/*.cc storage/server/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/storage/networks/Net.o: storage/networks/Net.cc \
	storage/networks/Flow.h \
	storage/networks/Messages.h \
	storage/networks/Net.h \
	storage/networks/Resource.h \
	storage/networks/Routing.h \
	storage/networks/Server.h \
	storage/networks/Solver.h
$O/storage/networks/Routing.o: storage/networks/Routing.cc \
	storage/networks/Routing.h
$O/storage/networks/Server.o: storage/networks/Server.cc \
	storage/networks/Flow.h \
	storage/networks/Messages.h \
	storage/networks/Net.h \
	storage/networks/Resource.h \
	storage/networks/Routing.h \
	storage/networks/Server.h \
	storage/networks/Solver.h
$O/storage/networks/Switch.o: storage/networks/Switch.cc \
	storage/networks/Flow.h \
	storage/networks/Messages.h \
	storage/networks/Net.h \
	storage/networks/Resource.h \
	storage/networks/Routing.h \
	storage/networks/Server.h \
	storage/networks/Switch.h


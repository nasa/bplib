###############################################################################
# File: Makefile
#
#   Copyright 2019 United States Government as represented by the
#   Administrator of the National Aeronautics and Space Administration.
#   All Other Rights Reserved.
#
#   This software was created at NASA's Goddard Space Flight Center.
#   This software is governed by the NASA Open Source Agreement and may be
#   used, distributed and modified only pursuant to the terms of that
#   agreement.
#
# Maintainer(s):
#  Joe-Paul Swinski, Code 582 NASA GSFC
#
# Notes:
#  1. Most settings used to build the library can be overridden in the config.mk
#     file.  To create a custom build of this library, copy and edit your own
#     config.mk file and then issue the following commands to build the library:
#
#       make CONFIG=config.mk
#
#  2. In order to use clang instead of gcc, set the CC and AR variables.
#     For example, if you wanted to run AddressSanitizer via clang, issue the following
#     commands to build the library:
#
#       make CC=clang USER_COPT=-fsanitize=address USER_LOPT=-fsanitize=address
#		sudo make CC=clang USER_COPT=-fsanitize=address USER_LOPT=-fsanitize=address install
#
#  3. Running the clang static code analysis is accomplished by preceding whichever
#     build command you use with "scan-build".  For example, a nominal scan is performed by:
#
#       scan-build make
#
#     Once the scan is complete, the use of scan-view prompted by the program on error, requires
#     python2.7 to be the default python interpreter.  Check the system path if scan-view reports
#     a SyntaxError when it tries to run.
#
#  4. The libabi.version script controls the export of any symbols in the library.  By convention
#     anything that starts with bplib_ is exported.  Run the following command to verify which
#     symbols are available to a linking application:
#
#       nm -CD /usr/local/lib/libbp.so | grep " T "
#

##############################################################################
## DEFINITIONS

# bplib repository root directory
ROOT	    := .

# bplib repository application interface include directory
API         := $(ROOT)/inc

# library objects
APP_OBJ     := bplib.o

# common objects
APP_OBJ     += crc.o
APP_OBJ     += rb_tree.o
APP_OBJ     += rh_hash.o
APP_OBJ	    += cbuf.o
APP_OBJ     += lrc.o

# version 6 objects
APP_OBJ     += v6.o
APP_OBJ     += bib.o
APP_OBJ     += cteb.o
APP_OBJ     += pay.o
APP_OBJ     += pri.o
APP_OBJ     += dacs.o
APP_OBJ     += sdnv.o

# storage service objects
APP_OBJ     += file.o
APP_OBJ     += ram.o
APP_OBJ     += flash.o
APP_OBJ     += flash_sim.o

# search path for application objects (note this is a make system variable)
VPATH	    := $(ROOT)/lib
VPATH	    += $(ROOT)/common
VPATH	    += $(ROOT)/v6
VPATH	    += $(ROOT)/os
VPATH	    += $(ROOT)/store
VPATH	    += $(ROOT)/unittest

# compiler options for search path for include headers (in form of -I_header_)
INCLUDES    := -I$(ROOT)/inc
INCLUDES    += -I$(ROOT)/lib
INCLUDES    += -I$(ROOT)/common
INCLUDES    += -I$(ROOT)/v6
INCLUDES    += -I$(ROOT)/os
INCLUDES    += -I$(ROOT)/store
INCLUDES    += -I$(ROOT)/unittest

##############################################################################
## CUSTOMIZATION

# definitions needed by the application (used to declare things like -D_APP_NAME_)
APP_DEFS    ?= $(USER_DEFS)

# compiler options needed by the application
APP_COPT    ?= $(USER_COPT)

# linker options needed by the application
APP_LOPT    ?= $(USER_LOPT)

# location to install bplib
PREFIX	    ?= /usr/local

# configuration makefile, if not set then uses default
CONFIG      ?= posix.mk

# include configuration makefile to override and add to above definitions
include $(CONFIG)

# unit test objects #
APP_OBJ     += unittest.o
APP_OBJ     += ut_assert.o
ifeq ($(BUILD_UNITTESTS),1)
APP_OBJ     += ut_crc.o
APP_OBJ     += ut_rb_tree.o
APP_OBJ     += ut_rh_hash.o
APP_OBJ     += ut_flash.o
endif

###############################################################################
##  COMPILER/LINKER CONFIGURATION

TGTLIB      :=   bp
TGTVER      :=   $(shell cat version.txt)
LIBDIR      :=   $(PREFIX)/lib
INCDIR      :=   $(PREFIX)/include/$(TGTLIB)
BLDDIR      :=   build

COPT        :=   -g -Wall -Wextra -D'LIBID="$(TGTVER)"' $(INCLUDES) $(APP_DEFS)
COPT        +=   -DLIBPATH=\"$(LIBDIR)\"
COPT        +=   -DINCPATH=\"$(INCDIR)\"
COPT        +=   -Wshadow
COPT        +=   -pthread
COPT        +=   -fPIC # position independent code needed for shared library

LOPT        :=   -lrt
LOPT        +=   -lpthread

ALL_OBJ     := $(addprefix $(BLDDIR)/, $(APP_OBJ))
ALL_COPT    := $(COPT) $(APP_COPT)
ALL_LOPT    := $(LOPT) $(APP_LOPT)

###############################################################################
##  COMPILER RULES

$(BLDDIR)/%.o: %.c
	$(CC) -c $(ALL_COPT) -o $@ $<

##############################################################################
##  TARGET RULES

all: clean lib

dev: clean lib bindings example

lib: static-lib shared-lib

static-lib: $(BLDDIR) $(ALL_OBJ)
	$(AR) crs $(BLDDIR)/lib$(TGTLIB).a $(ALL_OBJ)

shared-lib: $(BLDDIR) $(ALL_OBJ)
	$(CC) $(ALL_OBJ) $(ALL_LOPT) -shared -Wl,--version-script=libabi.version -o $(BLDDIR)/lib$(TGTLIB).so.$(TGTVER)
	ln -sf lib$(TGTLIB).so.$(TGTVER) $(BLDDIR)/lib$(TGTLIB).so

bindings:
	make -C binding/lua

example:
	make -C app

install: install-lib

install-dev: install-lib install-bindings install-example

install-lib: install-headers install-static install-shared

install-headers: $(INCDIR)
	cp $(foreach element,$(API),$(subst -I,,$(element)/*.h)) $(INCDIR)
	chmod 644 $(INCDIR)/*

install-static: $(PREFIX) $(LIBDIR)
	cp $(BLDDIR)/lib$(TGTLIB).a $(LIBDIR)
	chmod 644 $(LIBDIR)/lib$(TGTLIB).a

install-shared: $(PREFIX) $(LIBDIR)
	cp $(BLDDIR)/lib$(TGTLIB).so.$(TGTVER) $(LIBDIR)
	ln -sf $(LIBDIR)/lib$(TGTLIB).so.$(TGTVER) $(LIBDIR)/lib$(TGTLIB).so
	ldconfig
	chmod 644 $(LIBDIR)/lib$(TGTLIB).so
	chmod 644 $(LIBDIR)/lib$(TGTLIB).so.$(TGTVER)

install-bindings:
	make -C binding/lua install

install-example:
	make -C app install

$(BLDDIR):
	-mkdir -p $(BLDDIR)

$(PREFIX):
	-mkdir -p $(PREFIX)

$(LIBDIR):
	-mkdir -p $(LIBDIR)

$(INCDIR):
	-mkdir -p $(INCDIR)


clean ::
	-rm -Rf $(BLDDIR)

##############################################################################
##  TEST RULES

luaexec = lua5.3

testcase ?= binding/lua/test/test_runner.lua

testmem:
	valgrind --leak-check=full --track-origins=yes --track-fds=yes $(luaexec) $(testcase)

testcpu:
	valgrind --tool=callgrind $(luaexec) $(testcase)
	# kcachegrind callgrind.out.<pid>

testheap:
	valgrind --tool=massif --time-unit=B --pages-as-heap=yes $(luaexec) $(testcase)
	# ms_print massif.out.<pid>

testcov:
	lcov -c --directory build --output-file build/coverage.info
	genhtml build/coverage.info --output-directory build/coverage_html
	# firefox build/coverage_html/index.html

##############################################################################
##  STATIC ANALYSIS RULES

CHECK_OPT  = --enable=all --inconclusive --error-exitcode=1 -q
CHECK_OPT += --suppress=unusedFunction --suppress=missingInclude
CHECK_OPT += --suppress=objectIndex:common/crc.c
CHECK_OPT += --suppress=redundantAssignment:v6/dacs.c
CHECK_OPT += --suppress=memleak:os/cfe.c
CHECK_OPT += --suppress=memleak:os/posix.c

check:
	cppcheck $(ROOT) $(CHECK_OPT) $(INCLUDES) $(APP_DEFS)

# end of file

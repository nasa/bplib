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
#  2. In order to use clang instead of gcc, set the COMPILER and TOOLCHAIN variables.
#     For example, if you wanted to run AddressSanitizer via clang, issue the following
#     commands to build the library:  
# 
#       make COMPILER=clang TOOLCHAIN=llvm USER_COPT=-fsanitize=address USER_LOPT=-fsanitize=address 
#		sudo make COMPILER=clang TOOLCHAIN=llvm USER_COPT=-fsanitize=address USER_LOPT=-fsanitize=address install
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
## DEFINITIONS and CONFIGURATION (populated/overridden in application includes)

# bplib repository root directory
ROOT	    := .
	
# bplib repository application interface include directory
API         := $(ROOT)/inc

# location to install bplib
PREFIX	    := /usr/local
	
# library objects
APP_OBJ     := bplib.o

# common objects
APP_OBJ     += crc.o
APP_OBJ     += rb_tree.o
APP_OBJ     += rh_hash.o
APP_OBJ	    += cbuf.o

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

# unit test objects #
APP_OBJ     += ut_assert.o
APP_OBJ     += unittest.o
APP_OBJ     += ut_crc.o
APP_OBJ     += ut_rb_tree.o
APP_OBJ     += ut_rh_hash.o

# definitions needed by the application (used to declare things like -D_APP_NAME_)
APP_DEFS    ?= $(USER_DEFS)

# compiler options needed by the application
APP_COPT    ?= $(USER_COPT)

# linker options needed by the application
APP_LOPT    ?= $(USER_LOPT)

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

# c compiler and linker used for c file extensions
COMPILER    ?= gcc

# tool chain used as prefix for binary utilities like archival tool
TOOLCHAIN   ?= gcc

# configuration makefile, if not set then uses default 
CONFIG      ?= posix.mk 

# include configuration makefile to override and add to above definitions
include $(CONFIG)

###############################################################################
##  DEFINES

TGTLIB      :=   bp
TGTVER      :=   $(shell cat version.txt)
LIBDIR      :=   $(PREFIX)/lib
INCDIR      :=   $(PREFIX)/include/$(TGTLIB)
BLDDIR      :=   build
O           ?=   3 # default optimization level

COPT        :=   -g -Wall -Wextra -O$(O) -D'LIBID="$(TGTVER)"' $(INCLUDES) $(APP_DEFS)
COPT        +=   -DLIBPATH=\"$(LIBDIR)\"
COPT        +=   -DINCPATH=\"$(INCDIR)\"
COPT        +=   -Wshadow
COPT        +=   -pthread
COPT        +=   -fPIC # position independent code needed for shared library 

LOPT        :=   -lrt
LOPT        +=   -lpthread

###############################################################################
##  TOOLS

CC           =   $(COMPILER)
AR           =   $(TOOLCHAIN)-ar
RM           =   rm -f
CP           =   cp
DIFF         =   diff
LN           =   ln
MKDIR	     =   mkdir

###############################################################################
##  COMPILER RULES

ALL_OBJ     := $(addprefix $(BLDDIR)/, $(APP_OBJ))
ALL_COPT    := $(COPT) $(APP_COPT)
ALL_LOPT    := $(LOPT) $(APP_LOPT)

$(BLDDIR)/%.o: %.c
	$(CC) -c $(ALL_COPT) -o $@ $<

##############################################################################
##  TARGET RULES

all: clean $(BLDDIR) static-lib shared-lib bindings

static-lib: $(BLDDIR) $(ALL_OBJ)
	$(AR) crs $(BLDDIR)/lib$(TGTLIB).a $(ALL_OBJ)

shared-lib: $(BLDDIR) $(ALL_OBJ)
	$(CC) $(ALL_OBJ) $(ALL_LOPT) -shared -Wl,--version-script=libabi.version -o $(BLDDIR)/lib$(TGTLIB).so.$(TGTVER)

bindings:
	make -C binding/lua
	

install: install-headers install-static install-shared install-bindings

install-headers: $(INCDIR)
	$(CP) $(foreach element,$(API),$(subst -I,,$(element)/*.h)) $(INCDIR)
	chmod 644 $(INCDIR)/*

install-static: $(PREFIX) $(LIBDIR)
	$(CP) $(BLDDIR)/lib$(TGTLIB).a $(LIBDIR)
	chmod 644 $(LIBDIR)/lib$(TGTLIB).a

install-shared: $(PREFIX) $(LIBDIR)
	$(CP) $(BLDDIR)/lib$(TGTLIB).so.$(TGTVER) $(LIBDIR)
	$(LN) -sf $(LIBDIR)/lib$(TGTLIB).so.$(TGTVER) $(LIBDIR)/lib$(TGTLIB).so
	ldconfig
	chmod 644 $(LIBDIR)/lib$(TGTLIB).so
	chmod 644 $(LIBDIR)/lib$(TGTLIB).so.$(TGTVER)

install-bindings:
	make -C binding/lua install
	

$(BLDDIR):
	-$(MKDIR) -p $(BLDDIR)

$(PREFIX):
	-$(MKDIR) -p $(PREFIX)

$(LIBDIR):
	-$(MKDIR) -p $(LIBDIR)

$(INCDIR):
	-$(MKDIR) -p $(INCDIR)


clean ::
	-$(RM) -R $(BLDDIR)
	make -C binding/lua clean

##############################################################################
##  TEST RULES

luaexec = lua5.3

testcase ?= bindings/lua/test/test_runner.lua

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

# end of file

###############################################################################
# File: Makefile
#
#  Copyright 2019 United States Government as represented by the
#  Administrator of the National Aeronautics and Space Administration.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# Maintainer(s):
#  Joe-Paul Swinski, Code 582 NASA GSFC
#
# Notes:
#  1. Most settings used to build the library can be overridden in the config.mk
#     file.  To create a custom build of this library, copy and edit your own
#     config.mk file and then issue the following commands to build the library:
#
#        make CONFIG=config.mk
#
#  2. In order to use clang instead of gcc, set the COMPILER and TOOLCHAIN variables.
#     For example, if you wanted to run AddressSanitizer via clang, issue the following
#     commands to build the library:  
# 
#        make COMPILER=clang TOOLCHAIN=llvm APP_COPT=-fsanitize=address APP_LOPT=-fsanitize=address 
#

##############################################################################
## DEFINITIONS and CONFIGURATION (populated/overridden in application includes)

# bplib repository root directory
ROOT	    := .
	
# bplib repository application interface include directory
API         := $(ROOT)/inc

# location to install bplib
PREFIX	    := /usr/local
	
# application object files, application should add their objects to this variable
APP_OBJ     := bplib.o
APP_OBJ     += bplib_blk_bib.o
APP_OBJ     += bplib_blk_cteb.o
APP_OBJ     += bplib_blk_pay.o
APP_OBJ     += bplib_blk_pri.o
APP_OBJ     += bplib_crc.o
APP_OBJ     += bplib_sdnv.o
APP_OBJ     += bplib_rb_tree.o

# definitions needed by the application (used to declare things like -D_APP_NAME_)
APP_DEFS    ?=

# compiler options needed by the application
APP_COPT    ?=

# linker options needed by the application
APP_LOPT    ?=

# search path for application objects (note this is a make system variable)
VPATH	    := $(ROOT)/src

# compiler options for search path for include headers (in form of -I_header_)  
INCLUDES    := -I$(ROOT)/src
INCLUDES    += -I$(ROOT)/inc

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

TGTLIB  :=   bp
TGTVER  :=   $(shell cat version.txt)
CONSOLE :=   bpc
LIBDIR  :=   $(PREFIX)/lib
INCDIR  :=   $(PREFIX)/include/$(TGTLIB)
BLDDIR  :=   build
O       ?=   3 # default optimization level

COPT    :=   -g -Wall -Wextra -O$(O) -D'LIBID="$(TGTVER)"' $(INCLUDES) $(APP_DEFS)
COPT    +=   -DLIBPATH=\"$(LIBDIR)\"
COPT    +=   -DINCPATH=\"$(INCDIR)\"
COPT    +=   -D_GNU_
COPT    +=   -pthread
COPT    +=   -Wshadow
 
LOPT    :=   -lpthread
LOPT    +=   -lm
LOPT    +=   -lrt

###############################################################################
##  TOOLS

CC       =   $(COMPILER)
AR       =   $(TOOLCHAIN)-ar
RM       =   rm -f
CP       =   cp
DIFF     =   diff
LN       =   ln
MKDIR	 =   mkdir

###############################################################################
##  COMPILER RULES

ALL_OBJ := $(addprefix $(BLDDIR)/, $(APP_OBJ))
ALL_COPT := $(COPT) $(APP_COPT)
ALL_LOPT := $(LOPT) $(APP_LOPT)

$(BLDDIR)/%.o: %.cpp
	$(CC) -c -fpic $(ALL_COPT) -o $@ $<

$(BLDDIR)/%.o: %.c
	$(CC) -c -fpic $(ALL_COPT) -o $@ $<

##############################################################################
##  TARGET RULES

all: clean prep lib

lib: $(ALL_OBJ)
	$(AR) crs $(BLDDIR)/lib$(TGTLIB).a $^
	$(CC) $^ $(ALL_LOPT) -shared -o $(BLDDIR)/lib$(TGTLIB).so.$(TGTVER)

install: lib
	-$(MKDIR) -p $(PREFIX)
	-$(MKDIR) -p $(LIBDIR)
	-$(MKDIR) -p $(INCDIR)
	$(CP) $(BLDDIR)/lib$(TGTLIB).a $(LIBDIR)
	$(CP) $(BLDDIR)/lib$(TGTLIB).so.$(TGTVER) $(LIBDIR)
	$(LN) -sf $(LIBDIR)/lib$(TGTLIB).so.$(TGTVER) $(LIBDIR)/lib$(TGTLIB).so
	ldconfig
	$(MKDIR) -p $(INCDIR)
	$(CP) $(foreach element,$(API),$(subst -I,,$(element)/*.h)) $(INCDIR)
	chmod 644 $(INCDIR)/*
	chmod 644 $(LIBDIR)/lib$(TGTLIB).a
	chmod 644 $(LIBDIR)/lib$(TGTLIB).so
	chmod 644 $(LIBDIR)/lib$(TGTLIB).so.$(TGTVER)

prep: $(BLDDIR)

$(BLDDIR):
	-$(MKDIR) -p $(BLDDIR)

clean ::
	-$(RM) -R $(BLDDIR)

# end of file



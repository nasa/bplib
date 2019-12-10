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
#

##############################################################################
## DEFINITIONS and CONFIGURATION (populated/overridden in application includes)

# repository root directory
ROOT	    := .
	
# location to install extension
PREFIX	    := /usr/local
LUA_VERSION	:= 5.3
	
# object files
LUA_OBJ     := lua_bplib.o

# compiler options needed by the extension
LUA_COPT    ?= -I/usr/include/lua$(LUA_VERSION)

# linker options needed by the application
LUA_LOPT    ?= -L/usr/lib/x86_64-linux-gnu

# search path for extension objects (note this is a make system variable)
VPATH	    := $(ROOT)

# compiler options for search path for include headers (in form of -I_header_)  
INCLUDES    := -I$(ROOT)
INCLUDES    += -I$(ROOT)/../../inc

# c compiler and linker used for c file extensions
COMPILER    ?= gcc

# tool chain used as prefix for binary utilities like archival tool
TOOLCHAIN   ?= gcc

###############################################################################
##  DEFINES

TGTEXT      :=   bplib
LIBDIR      :=   $(PREFIX)/lib/lua/$(LUA_VERSION)
BLDDIR      :=   build
O           ?=   3 # default optimization level

COPT        :=   -g -O$(O) $(INCLUDES)
COPT        +=   -Wall -Wextra -Wshadow $(LUA_COPT)
COPT        +=   -fPIC

LOPT        :=   -llua$(LUA_VERSION) -lbp -L../../build $(LUA_LOPT)

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

ALL_OBJ     := $(addprefix $(BLDDIR)/, $(LUA_OBJ))
ALL_COPT    := $(COPT)
ALL_LOPT    := $(LOPT)

$(BLDDIR)/%.o: %.c
	$(CC) -c $(ALL_COPT) -o $@ $<

##############################################################################
##  TARGET RULES

all: clean $(BLDDIR) extension

extension: $(BLDDIR) $(ALL_OBJ)
	$(CC) $(ALL_OBJ) $(ALL_LOPT) -shared -o $(BLDDIR)/$(TGTEXT).so

install: $(PREFIX) $(LIBDIR)
	$(CP) $(BLDDIR)/$(TGTEXT).so $(LIBDIR)
	chmod 644 $(LIBDIR)/$(TGTEXT).so
	$(CP) bp.lua $(LIBDIR)
	$(CP) bptest.lua $(LIBDIR)

$(BLDDIR):
	-$(MKDIR) -p $(BLDDIR)

$(PREFIX):
	-$(MKDIR) -p $(PREFIX)

$(LIBDIR):
	-$(MKDIR) -p $(LIBDIR)

clean ::
	-$(RM) -R $(BLDDIR)

# end of file

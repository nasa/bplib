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

##############################################################################
## DEFINITIONS and CONFIGURATION (populated/overridden in application includes)

# repository root directory
ROOT	    := .

# object files
LUA_OBJ     := lua_bplib.o

# search path for extension objects (note this is a make system variable)
VPATH	    := $(ROOT)

# compiler options for search path for include headers (in form of -I_header_)
INCLUDES    := -I$(ROOT)
INCLUDES    += -I$(ROOT)/../../inc
INCLUDES    += -I$(ROOT)/../../unittest

# user customized options
MY_COPT     ?=
MY_LOPT     ?=

###############################################################################
##  CUSTOMIZATION

# location of installed lua
PREFIX	    ?= /opt/lua5.3

# location to install extension
LIBDIR      ?= $(PREFIX)/lib/lua/5.3

###############################################################################
##  DEFINES

TGTEXT      :=   bplib
BLDDIR      :=   build

COPT        :=   -g -O0 $(INCLUDES) $(MY_COPT)
COPT        +=   -I$(PREFIX)/include
COPT        +=   -Wall -Wextra -Wshadow
COPT        +=   -fPIC

LOPT        :=    $(MY_LOPT)
LOPT        +=   -L../../build
LOPT        +=   -lbp

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
	cp $(BLDDIR)/$(TGTEXT).so $(LIBDIR)
	chmod 644 $(LIBDIR)/$(TGTEXT).so
	cp bp.lua $(LIBDIR)
	cp bptest.lua $(LIBDIR)

$(BLDDIR):
	-mkdir -p $(BLDDIR)

$(PREFIX):
	-mkdir -p $(PREFIX)

$(LIBDIR):
	-mkdir -p $(LIBDIR)

clean ::
	ls
	-rm -R $(BLDDIR)

# end of file

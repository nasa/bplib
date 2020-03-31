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
APP_OBJ     := bpsend.o
APP_OBJ     += bpsock.o

# search path for extension objects (note this is a make system variable)
VPATH	    := $(ROOT)

# compiler options for search path for include headers (in form of -I_header_)
INCLUDES    := -I$(ROOT)

# user customized options
MY_COPT     ?= -DSOCK_VERBOSE
MY_LOPT     ?=

###############################################################################
##  CUSTOMIZATION

# location of installed binaries
PREFIX	    ?= /opt/bpio
BINDIR       = $(PREFIX)/bin

###############################################################################
##  DEFINES

TGTEXE      :=   bpsend
BLDDIR      :=   build

COPT        :=   -g -O3 $(INCLUDES) $(MY_COPT)
COPT        +=   -Wall -Wextra -Wshadow

LOPT        :=    $(MY_LOPT)
LOPT        +=   -lbp
LOPT        +=   -lpthread

###############################################################################
##  COMPILER RULES

ALL_OBJ     := $(addprefix $(BLDDIR)/, $(APP_OBJ))
ALL_COPT    := $(COPT)
ALL_LOPT    := $(LOPT)

$(BLDDIR)/%.o: %.c
	$(CC) -c $(ALL_COPT) -o $@ $<

##############################################################################
##  TARGET RULES

all: clean $(BLDDIR) app

app: $(BLDDIR) $(ALL_OBJ)
	$(CC) $(ALL_OBJ) $(ALL_LOPT) -o $(BLDDIR)/$(TGTEXE)

install: $(BINDIR)
	cp $(BLDDIR)/$(TGTEXE) $(BINDIR)
	chmod 755 $(BINDIR)/$(TGTEXE)
	cp bptest.lua $(LIBDIR)

$(BLDDIR):
	-mkdir -p $(BLDDIR)

$(BINDIR):
	-mkdir -p $(BINDIR)

clean ::
	ls
	-rm -R $(BLDDIR)

# end of file

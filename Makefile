###############################################################################
# File: Bundle Protocol Library Makefile
###############################################################################

##############################################################################
## DEFINITIONS and CONFIGURATION (populated/overridden in application includes)

# bplib repository root directory
ROOT	    := .
	
# bplib repository application interface include directory
API         := $(ROOT)/inc

# location to install bplib
PREFIX	    := /usr/local
	
# default console application
CONSOLE_OBJ := bpc.o

# application object files, application should add their objects to this variable
APP_OBJ     := bplib.o
APP_OBJ     += bplib_blk_bib.o
APP_OBJ     += bplib_blk_cteb.o
APP_OBJ     += bplib_blk_pay.o
APP_OBJ     += bplib_blk_pri.o
APP_OBJ     += bplib_crc.o
APP_OBJ     += bplib_sdnv.o

# definitions needed by the application (used to declare things like -D_APP_NAME_)
APP_DEFS    :=

# compiler options needed by the application
APP_COPT    :=

# linker options needed by the application
APP_LOPT    :=

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
CONFIG      ?= config.mk 

# include configuration makefile to override and add to above definitions
include $(CONFIG)

###############################################################################
##  DEFINES

TGTLIB  :=   bp
TGTVER  :=   $(shell cat version.txt)
CONSOLE :=   bpc
EXECDIR :=   $(PREFIX)/bin
LIBDIR  :=   $(PREFIX)/lib
INCDIR  :=   $(PREFIX)/include/$(TGTLIB)
BLDDIR  :=   build
O       :=   3 # optimization level

COPT    :=   -g -Wall -Wextra -O$(O) -D'LIBID="$(TGTVER)"' $(INCLUDES) $(APP_DEFS)
COPT    +=   -DEXECPATH=\"$(EXECDIR)\"
COPT    +=   -DLIBPATH=\"$(LIBDIR)\"
COPT    +=   -DINCPATH=\"$(INCDIR)\"
COPT    +=   -D_GNU_
COPT    +=   -pthread
COPT    +=   -Wno-format-security # this poses future security risk and should be removed at some point
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
	$(AR) -crs $(BLDDIR)/lib$(TGTLIB).a $^
	$(CC) $^ $(ALL_LOPT) -shared -o $(BLDDIR)/lib$(TGTLIB).so.$(TGTVER)

install: lib
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

console: $(BLDDIR)/$(CONSOLE_OBJ)
	$(CC) $^ -l$(TGTLIB) $(ALL_LOPT) -o $(BLDDIR)/$(CONSOLE)

installconsole: console
	$(CP) $(BLDDIR)/$(CONSOLE) $(EXECDIR)
	chmod 755 $(EXECDIR)/$(CONSOLE)

prep: $(BLDDIR)

$(BLDDIR):
	-$(MKDIR) -p $(BLDDIR)

clean ::
	-$(RM) -R $(BLDDIR)

# end of file



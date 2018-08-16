###############################################################################
# File: Bundle Protocol Library Configuration Makefile
# Note: See 'Makefile' in same directory for where this is included
###############################################################################


###############################################################################
##  PATHS

VPATH	    += $(ROOT)/src

INCLUDES    += -I$(ROOT)/src
INCLUDES    += -I$(ROOT)/inc

###############################################################################
##  OBJECTS

APP_OBJ += bplib.o
APP_OBJ += bplib_blk_bib.o
APP_OBJ += bplib_blk_cteb.o
APP_OBJ += bplib_blk_pay.o
APP_OBJ += bplib_blk_pri.o
APP_OBJ += bplib_crc.o
APP_OBJ += bplib_sdnv.o
APP_OBJ += bplib_os_posix.o
APP_OBJ += bplib_store_pfile.o

###############################################################################
##  OPTIONS

# Disable Asserts #
# APP_COPT += -DNDEBUG

# Enable Stack Checker #
# APP_COPT += -fstack-protector-all

# Enable 32-bit Target #
# APP_COPT += -m32
# APP_LOPT += -m32

# Enable Toolchain Specific Checks #
ifeq ($(TOOLCHAIN), gcc)
APP_COPT += -Wlogical-op
endif


###############################################################################
# File: posix.mk
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
# Note: 
#  See 'Makefile' in same directory for where this is included
###############################################################################

###############################################################################
##  PLATFORM SPECIFIC OBJECTS

APP_OBJ += posix.o

###############################################################################
##  OPTIONS

# Optimization Level #
O = 0

# Disable Asserts #
# APP_COPT += -DNDEBUG

# Position Independent Code #
APP_COPT += -fPIC

# POSIX Compile Options #
APP_COPT += -pthread

# GNU Code Coverage # 
APP_COPT += -fprofile-arcs -ftest-coverage
APP_LOPT += -lgcov --coverage

# Enable Stack Checker #
APP_COPT += -fstack-protector-all

# Enable 32-bit Target #
# APP_COPT += -m32
# APP_LOPT += -m32

# POSIX Libraries #
APP_LOPT += -lpthread
APP_LOPT += -lrt

# Enable Toolchain Specific Checks #
ifeq ($(TOOLCHAIN), gcc)
APP_COPT += -Wlogical-op
endif


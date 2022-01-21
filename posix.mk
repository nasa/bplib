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
APP_COPT += -O0

# Build Unit Tests #
BUILD_UNITTESTS=1
APP_COPT += -DUNITTESTS
APP_COPT += -DBP_LOCAL_SCOPE="" # removes static designator so that local functions can be unit tested

# GNU Code Coverage #
APP_COPT += -fprofile-arcs -ftest-coverage
APP_LOPT += -lgcov --coverage

# Enable Stack Checker #
APP_COPT += -fstack-protector-all

# Enable Toolchain Specific Checks #
ifeq ($(CC), cc)
APP_COPT += -Wlogical-op
endif


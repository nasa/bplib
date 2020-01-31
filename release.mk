###############################################################################
# File: release.mk
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
APP_COPT += -O3

# Disable Asserts #
APP_COPT += -DNDEBUG

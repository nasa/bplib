###############################################################################
# File: posix.mk
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
# Note: 
#  See 'Makefile' in same directory for where this is included
###############################################################################

###############################################################################
##  PLATFORM SPECIFIC OBJECTS

APP_OBJ += bplib_os_posix.o

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


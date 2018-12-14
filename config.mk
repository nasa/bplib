###############################################################################
# File: Bundle Protocol Library Configuration Makefile
# Note: See 'Makefile' in same directory for where this is included
###############################################################################


###############################################################################
##  PLATFORM SPECIFIC OBJECTS

APP_OBJ += bplib_os_posix.o
APP_OBJ += bplib_store_pfile.o
APP_OBJ += bplib_store_pram.o

###############################################################################
##  OPTIONS

# Disable Asserts #
# APP_COPT += -DNDEBUG

# Optimization Level #
O = 0

# GNU Code Coverage # 
APP_COPT += -fprofile-arcs -ftest-coverage
APP_LOPT += -lgcov --coverage

# Enable Stack Checker #
APP_COPT += -fstack-protector-all

# Enable 32-bit Target #
# APP_COPT += -m32
# APP_LOPT += -m32

# Enable Toolchain Specific Checks #
ifeq ($(TOOLCHAIN), gcc)
APP_COPT += -Wlogical-op
endif


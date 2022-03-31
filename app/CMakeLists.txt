##################################################################
#
# Bundle Protocol Library (BPLib) Test App CMake build recipe
#
# This file provides two executables, bprecv and bpsend, that
# can be used to initiate or terminate BP/DTN traffic from the
# command line on a simulated node
#
##################################################################

cmake_minimum_required(VERSION 3.5)
project(BPLIB_TEST_APP C)

# Sanity check: BPLib test app assumes a POSIX system
# and uses pthread calls.  If not POSIX, then this tool
# will not build.
if (NOT BPLIB_INCLUDE_POSIX)
  message(FATAL_ERROR "BPLib Test applications require POSIX")
endif()

set(BPAPP_COMPILE_FEATURES "c_std_99")
set(BPAPP_COMPILE_OPTIONS "$<$<C_COMPILER_ID:GNU>:-Wall;-Werror>")
set(BPAPP_LINK_LIBRARIES bplib)

# The socket code is shared between the apps.  Using an OBJECT lib saves building it twice
add_library(bpapp_sock OBJECT sock.c)

#add_executable(bprecv bprecv.c $<TARGET_OBJECTS:bpapp_sock>)
#add_executable(bpsend bpsend.c $<TARGET_OBJECTS:bpapp_sock>)
add_executable(bptest bptest.c)
add_executable(rbtest rbtest.c)

# compile this app as c99 (but this does not impose the same requirement on other users)
#target_compile_features(bprecv PRIVATE ${BPAPP_COMPILE_FEATURES})
#target_compile_features(bpsend PRIVATE ${BPAPP_COMPILE_FEATURES})
target_compile_features(bptest PRIVATE ${BPAPP_COMPILE_FEATURES})
target_compile_features(rbtest PRIVATE ${BPAPP_COMPILE_FEATURES})

# If using GNU GCC, then also enable full warning reporting
#target_compile_options(bprecv PRIVATE ${BPAPP_COMPILE_OPTIONS})
#target_compile_options(bpsend PRIVATE ${BPAPP_COMPILE_OPTIONS})
target_compile_options(bptest PRIVATE ${BPAPP_COMPILE_OPTIONS})
target_compile_options(rbtest PRIVATE ${BPAPP_COMPILE_OPTIONS})

# Low level test apps may include "private" headers, whereas higher level tests should not
target_include_directories(bptest PRIVATE ${BPLIB_PRIVATE_INCLUDE_DIRS})
target_include_directories(rbtest PRIVATE ${BPLIB_PRIVATE_INCLUDE_DIRS})

# link with bplib
#target_link_libraries(bprecv ${BPAPP_LINK_LIBRARIES})
#target_link_libraries(bpsend ${BPAPP_LINK_LIBRARIES})
target_link_libraries(bptest ${BPAPP_LINK_LIBRARIES})
target_link_libraries(rbtest ${BPAPP_LINK_LIBRARIES})

##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the Version utility
#
##################################################################

add_library(bplib_version OBJECT
    src/bplib_version.c
)

target_include_directories(bplib_version PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

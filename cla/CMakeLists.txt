##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the CLA module
#
##################################################################

add_library(bplib_cla OBJECT
    src/bplib_cla.c
    src/bplib_cla_internal.c
)

target_include_directories(bplib_cla PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_bi,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

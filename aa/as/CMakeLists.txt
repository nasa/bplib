##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the Admin Statistics (AS) module
#
##################################################################

add_library(bplib_as OBJECT
    src/bplib_as.c
    src/bplib_as_internal.c
)

target_include_directories(bplib_as PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_nc,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_fwp,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_time,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

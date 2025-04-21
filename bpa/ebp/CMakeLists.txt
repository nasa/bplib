##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the EBP module
#
##################################################################

add_library(bplib_ebp OBJECT
    src/bplib_ebp.c
)

target_include_directories(bplib_ebp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_nc,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_time,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

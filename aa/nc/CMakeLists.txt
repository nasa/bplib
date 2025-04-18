##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the NC module
#
##################################################################

add_library(bplib_nc OBJECT
    src/bplib_nc.c
    src/bplib_nc_directives.c
    src/bplib_nc_rwlock.c
)

target_include_directories(bplib_nc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_as,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_stor,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_fwp,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_cla,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_crc,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

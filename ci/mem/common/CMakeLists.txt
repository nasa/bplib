##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "mem common" submodule
#
##################################################################

set(bplib_mem_common_SOURCES
    src/bplib_crc.c
    src/bplib_rbtree.c
)

add_library(bplib_mem_common OBJECT
    ${bplib_mem_common_SOURCES}
)

target_include_directories(bplib_mem_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_libmgr,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES>
)

if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  # STOR add_subdirectory(ut-coverage)
  add_subdirectory(ut-functional)
endif(BPLIB_ENABLE_UNIT_TESTS)

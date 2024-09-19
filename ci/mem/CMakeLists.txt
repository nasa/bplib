##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the MEM module.
#
#
##################################################################

add_library(bplib_mem OBJECT
    src/bplib_mem.c
)

target_include_directories(bplib_mem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_mem_common,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

add_subdirectory(common)

if(BPLIB_ENABLE_UNIT_TESTS)
    # STOR (mostly cache tests) add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

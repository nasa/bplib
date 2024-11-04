##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "crc" submodule, which applies only to the
# bplib_crc build.
#
##################################################################

add_library(bplib_crc OBJECT
    src/bplib_crc.c
)

target_include_directories(bplib_crc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

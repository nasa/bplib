##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the MEM module
#
##################################################################

add_library(bplib_rbt OBJECT
    src/bplib_rbt_impl.c
    src/bplib_rbt_iterator.c
    src/bplib_rbt_api.c
)

target_include_directories(bplib_rbt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

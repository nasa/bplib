##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the MEM module
#
##################################################################

add_library(bplib_mem OBJECT
    src/bplib_mem.c
    src/bplib_mem_rbtree.c
    src/bplib_mem_internal.c
)

target_link_libraries(bplib_mem osal)
target_include_directories(bplib_mem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

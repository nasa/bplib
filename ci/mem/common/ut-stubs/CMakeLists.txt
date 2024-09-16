##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "mem_common" stub functions
#
##################################################################

add_library(bplib_mem_common_stubs STATIC
    bplib_crc_stubs.c
    bplib_crc_stub_objs.c
    bplib_rbtree_stubs.c
)

target_include_directories(bplib_mem_common_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_mem_common,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_mem_common_stubs PUBLIC
  ut_assert
)

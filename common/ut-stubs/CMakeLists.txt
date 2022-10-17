##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "common" stub functions
#
##################################################################

add_library(bplib_common_stubs STATIC
    crc_stubs.c
    crc_stub_objs.c
    v7_rbtree_stubs.c
)

target_include_directories(bplib_common_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_common,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_common_stubs PUBLIC
  ut_assert
)

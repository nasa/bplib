##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "mpool" stub functions
#
##################################################################

add_library(bplib_mpool_stubs STATIC
    v7_mpool_stubs.c
    v7_mpool_ref_stubs.c
    v7_mpool_job_stubs.c
    v7_mpool_bblocks_stubs.c
    v7_mpool_flows_stubs.c
    v7_mpstream_stubs.c
)

target_include_directories(bplib_mpool_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_mpool_stubs PUBLIC
  ut_assert
)

##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "stor_qme" stub functions
#
##################################################################

add_library(bplib_stor_qm_stubs STATIC
    bplib_stor_cache_ducts_stubs.c
    bplib_stor_cache_job_stubs.c
    bplib_stor_cache_mpstream_stubs.c
    bplib_stor_cache_types_stubs.c
    bplib_stor_qm_stubs.c
)

target_include_directories(bplib_stor_qm_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_stor_cache,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_stor_qm_stubs PUBLIC
  ut_assert
)

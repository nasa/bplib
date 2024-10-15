##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "stor_qm" stub functions
#
##################################################################

add_library(bplib_stor_qm_stubs STATIC
    bplib_stor_qm_codec_stubs.c
    bplib_stor_qm_dataservice_stubs.c
    bplib_stor_qm_decode_stubs.c
    bplib_stor_qm_ducts_stubs.c
    bplib_stor_qm_encode_stubs.c
    bplib_stor_qm_job_stubs.c
    bplib_stor_qm_mpstream_stubs.c
    bplib_stor_qm_ref_stubs.c
    bplib_stor_qm_stubs.c
)

target_include_directories(bplib_stor_qm_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_stor_qm,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_stor_cache,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_stor_qm_stubs PUBLIC
    ut_coverage_link
    bplib_mem_stubs
    coverage-bplib-stubs
    ut_assert
)

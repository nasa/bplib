##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "mem" submodule, which applies only to the
# v7 build.
#
##################################################################

add_library(bplib_mem OBJECT
    src/v7_mpool.c
    src/v7_mpool_ref.c
    src/v7_mpool_job.c
    src/v7_mpool_bblocks.c
    src/v7_mpool_flows.c
    src/v7_mpstream.c
)

target_include_directories(bplib_mem PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc

    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_v7,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
)

if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(ut-coverage)
endif(BPLIB_ENABLE_UNIT_TESTS)

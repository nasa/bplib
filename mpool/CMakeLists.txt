##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "mpool" submodule, which applies only to the
# v7 build.
#
##################################################################

add_library(bplib_mpool OBJECT
    src/v7_mpool.c
    src/v7_mpool_ref.c
    src/v7_mpool_job.c
    src/v7_mpool_bblocks.c
    src/v7_mpool_flows.c
    src/v7_mpstream.c
)

target_include_directories(bplib_mpool PRIVATE
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_v7,INTERFACE_INCLUDE_DIRECTORIES>

    # Other parts of bplib common code (until they can become submodules)
    ${BPLIB_SOURCE_DIR}/common
)
target_include_directories(bplib_mpool PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Use the same basic compiler flags as bplib
target_compile_features(bplib_mpool PRIVATE c_std_99)
target_compile_options(bplib_mpool PRIVATE ${BPLIB_COMMON_COMPILE_OPTIONS})
set_target_properties(bplib_mpool PROPERTIES ${BPLIB_COMMON_PROPERTIES})

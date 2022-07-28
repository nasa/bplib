##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "cache" submodule, which applies only to the
# v7 build.
#
##################################################################

add_library(bplib_cache OBJECT
    src/v7_cache.c
    src/v7_cache_custody.c
    src/v7_cache_fsm.c
)

target_include_directories(bplib_cache PRIVATE
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_v7,INTERFACE_INCLUDE_DIRECTORIES>

    # Other parts of bplib common code (until they can become submodules)
    ${BPLIB_SOURCE_DIR}/common
    ${BPLIB_SOURCE_DIR}/lib
)
target_include_directories(bplib_cache PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Use the same basic compiler flags as bplib
target_compile_features(bplib_cache PRIVATE c_std_99)
target_compile_options(bplib_cache PRIVATE ${BPLIB_COMMON_COMPILE_OPTIONS})
set_target_properties(bplib_cache PROPERTIES ${BPLIB_COMMON_PROPERTIES})

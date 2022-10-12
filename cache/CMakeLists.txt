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
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_v7,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
)
target_include_directories(bplib_cache PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_base,INTERFACE_INCLUDE_DIRECTORIES>
)

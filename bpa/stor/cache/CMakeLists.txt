##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "cache" submodule, which applies only to the
# bplib_stor_cache build.
#
##################################################################

add_library(bplib_stor_cache OBJECT
    src/bplib_stor_cache.c
    src/bplib_stor_cache_bblocks.c
    src/bplib_stor_cache_internal.c
    src/bplib_stor_cache_lists.c
    src/bplib_stor_cache_module_api.c
    src/bplib_stor_cache_ref.c
    src/crc.c
)

target_include_directories(bplib_stor_cache PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
)
target_include_directories(bplib_stor_cache PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_time,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_stor_qm,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

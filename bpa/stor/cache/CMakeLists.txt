##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "cache" submodule, which applies only to the
# bplib_stor_cache build.
#
##################################################################

# TODO Use the new codec stubs when available.
# Build the heritage v7 codec stubs
add_subdirectory(unit-test/codec-unit-test/stubs)

add_library(bplib_stor_cache OBJECT
    src/bplib_stor_cache.c
    src/bplib_stor_cache_lists.c
    src/bplib_stor_cache_bblocks.c
    # QM src/bplib_stor_cache_cla_api.c
    # CUSTODY src/bplib_stor_cache_custody.c
    # QM src/bplib_stor_cache_dataservice_api.c
    # QM src/bplib_stor_qm.c
    src/bplib_stor_cache_fsm.c
    # QM JOB src/bplib_stor_cache_job.c
    src/bplib_stor_cache_ref.c
    # PS src/bplib_stor_cache_storage.c
    src/crc.c
)

target_include_directories(bplib_stor_cache PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
)
target_include_directories(bplib_stor_cache PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

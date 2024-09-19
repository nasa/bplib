##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "cache" submodule, which applies only to the
# v7 build.
#
##################################################################

add_library(bplib_cache OBJECT
    src/bplib_cache.c
    src/bplib_cache_bblocks.c
    src/bplib_cache_cla_api.c
    src/bplib_cache_custody.c
    src/bplib_cache_dataservice_api.c
    src/bplib_cache_enqueueing.c
    src/bplib_cache_fsm.c
    src/bplib_cache_ref.c
)

target_include_directories(bplib_cache PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
        
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_libmgr,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem_common,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

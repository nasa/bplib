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
)
# TODO Removed src/v7_cache_fsm.c

target_include_directories(bplib_cache PUBLIC
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
)

target_include_directories(bplib_cache PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
        
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_libmgr,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(ut-coverage)
endif(BPLIB_ENABLE_UNIT_TESTS)

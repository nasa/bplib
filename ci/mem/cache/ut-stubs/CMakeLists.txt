##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "cache" stub functions
#
##################################################################

add_library(bplib_cache_stubs STATIC
    v7_cache_stubs.c
)

target_include_directories(bplib_cache_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_cache,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_cache_stubs PUBLIC
  ut_assert
)

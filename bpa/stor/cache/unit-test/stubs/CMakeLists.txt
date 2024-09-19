##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "stor_cache" stub functions
#
##################################################################

add_library(bplib_stor_cache_stubs STATIC
    bplib_dataservice_stubs.c
)

target_include_directories(bplib_stor_cache_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_stor_cache,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_stor_cache_stubs PUBLIC
  ut_assert
)

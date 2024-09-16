##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "libmgr" stub functions
#
##################################################################

add_library(bplib_libmgr_stubs STATIC
    bplib_dataservice_stubs.c
)

target_include_directories(bplib_libmgr_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_libmgr,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_libmgr_stubs PUBLIC
  ut_assert
)

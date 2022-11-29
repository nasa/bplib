##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "os" stub functions
#
##################################################################

add_library(bplib_os_stubs STATIC
  bplib_os_handlers.c
  bplib_os_stubs.c
)

target_include_directories(bplib_os_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_os,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_os_stubs PUBLIC
  ut_assert
)

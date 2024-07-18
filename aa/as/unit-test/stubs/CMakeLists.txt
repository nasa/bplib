##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "as" stub functions
#
##################################################################

add_library(bplib_as_stubs STATIC
    bplib_as_stubs.c
)

target_include_directories(bplib_as_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_as,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_as_stubs PUBLIC
  ut_assert
)

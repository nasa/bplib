##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "cla" stub functions
#
##################################################################

add_library(bplib_cla_stubs STATIC
    bplib_cla_stubs.c
)

target_include_directories(bplib_cla_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_cla,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_cla_stubs PUBLIC
  ut_assert
)

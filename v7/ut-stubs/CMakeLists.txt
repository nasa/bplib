##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "v7" stub functions
#
##################################################################

add_library(bplib_v7_stubs STATIC
    v7_stubs.c
    v7_codec_stubs.c
    v7_encode_stubs.c
    v7_decode_stubs.c
)

target_include_directories(bplib_v7_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_v7,INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_v7_stubs PUBLIC
  ut_assert
)

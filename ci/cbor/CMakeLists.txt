##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the CBOR module
#
##################################################################

add_library(bplib_cbor OBJECT
    src/bplib_cbor.c
    src/bplib_cbor_decode_bundle.c
    src/bplib_cbor_decode_types.c
    src/bplib_cbor_decode_primary.c
    src/bplib_cbor_decode_canonical.c
)

target_include_directories(bplib_cbor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${QCBOR_INCLUDE_DIRS}
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

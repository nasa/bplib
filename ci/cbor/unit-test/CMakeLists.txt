##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_cbor_stubs STATIC
    stubs/bplib_cbor_stubs.c
)

target_include_directories(bplib_cbor_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_cbor,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_cbor_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_cbor OBJECT
    ../src/bplib_cbor_decode_bundle.c
    ../src/bplib_cbor_decode_canonical.c
    ../src/bplib_cbor_decode_primary.c
    ../src/bplib_cbor_decode_types.c
    ../src/bplib_cbor_encode_primary.c
    ../src/bplib_cbor_encode_extension.c
    ../src/bplib_cbor_encode_payload.c
    ../src/bplib_cbor_encode_types.c
)

target_compile_definitions(utobj_bplib_cbor PRIVATE
    $<TARGET_PROPERTY:bplib_cbor,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_cbor PRIVATE
    $<TARGET_PROPERTY:bplib_cbor,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_cbor PRIVATE
    $<TARGET_PROPERTY:bplib_cbor,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_cbor-testrunner
    utilities/bplib_cbor_test_utils.c
    bplib_cbor_test.c
    $<TARGET_OBJECTS:utobj_bplib_cbor>
)

target_include_directories(coverage-bplib_cbor-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_cbor,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_cbor-testrunner PUBLIC
    qcbor
    m
    bplib_em_stubs
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_cbor-testrunner coverage-bplib_cbor-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_cbor-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

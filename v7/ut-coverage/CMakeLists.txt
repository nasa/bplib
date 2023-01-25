##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_v7 OBJECT
    ../src/v7.c
    ../src/v7_codec_common.c
    ../src/v7_encode_api.c
    ../src/v7_decode_api.c
    ../src/v7_bp_basetypes.c
    ../src/v7_bp_container.c
    ../src/v7_bp_bitmap.c
    ../src/v7_bp_crc.c
    ../src/v7_bp_endpointid.c
    ../src/v7_bp_primary_block.c
    ../src/v7_bp_admin_record.c
    ../src/v7_bp_canonical_block.c
    ../src/v7_bp_bundle_age_block.c
    ../src/v7_bp_hop_count_block.c
    ../src/v7_bp_previous_node_block.c
    ../src/v7_custody_tracking_block.c
    ../src/v7_custody_acknowledgement_record.c
)

target_compile_definitions(utobj_bplib_v7 PRIVATE
    $<TARGET_PROPERTY:bplib_v7,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_v7 PRIVATE
    $<TARGET_PROPERTY:bplib_v7,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_v7 PRIVATE
    $<TARGET_PROPERTY:bplib_v7,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_v7-testrunner
    test_bplib_v7_setup.c
    cbor_stubs.c
    test_v7.c
    test_v7_bp_admin_record.c
    test_v7_bp_basetypes.c
    test_v7_bp_bitmap.c
    test_v7_bp_bundle_age_block.c
    test_v7_bp_canonical_block.c
    test_v7_bp_container.c
    test_v7_bp_crc.c
    test_v7_bp_endpointid.c
    test_v7_bp_hop_count_block.c
    test_v7_bp_previous_node_block.c
    test_v7_bp_primary_block.c
    test_v7_codec_common.c
    test_v7_custody_acknowledgement_record.c
    test_v7_custody_tracking_block.c
    test_v7_decode_api.c
    test_v7_encode_api.c
    $<TARGET_OBJECTS:utobj_bplib_v7>
)

target_include_directories(coverage-bplib_v7-testrunner PRIVATE
    ../src
    $<TARGET_PROPERTY:bplib_v7,INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_v7-testrunner PUBLIC
    ut_coverage_link
    bplib_mpool_stubs
    bplib_common_stubs
    bplib_os_stubs
    ut_assert
)

add_test(coverage-bplib_v7-testrunner coverage-bplib_v7-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_v7-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

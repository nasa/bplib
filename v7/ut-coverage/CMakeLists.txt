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
    ../src/v7_codec.c
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
    test_bplib_v7.c
    test_bplib_v7_codec.c
    cbor_stubs.c
    $<TARGET_OBJECTS:utobj_bplib_v7>
)

target_include_directories(coverage-bplib_v7-testrunner PRIVATE
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

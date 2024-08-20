##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_version_stubs STATIC
    stubs/bplib_version_stubs.c
)

target_include_directories(bplib_version_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_version,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_version_stubs ut_assert ut_core_api_stubs ut_osapi_stubs)

# Create unit test object
add_library(utobj_bplib_version OBJECT
    ../src/bplib_version.c
)

target_compile_definitions(utobj_bplib_version PRIVATE
    $<TARGET_PROPERTY:bplib_version,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_version PRIVATE
    $<TARGET_PROPERTY:bplib_version,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_version PRIVATE
    $<TARGET_PROPERTY:bplib_version,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_version-testrunner
    utilities/bplib_version_test_utils.c
    bplib_version_test.c
    $<TARGET_OBJECTS:utobj_bplib_version>
)

target_include_directories(coverage-bplib_version-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_version,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_version-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    ut_osapi_stubs
)

add_test(coverage-bplib_version-testrunner coverage-bplib_version-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_version-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_fwp_stubs STATIC
    stubs/bplib_fwp_stubs.c
)

target_include_directories(bplib_fwp_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_fwp,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_fwp_stubs ut_assert ut_core_api_stubs)

# Create unit test object
add_library(utobj_bplib_fwp OBJECT
    ../src/bplib_fwp.c
)

target_compile_definitions(utobj_bplib_fwp PRIVATE
    $<TARGET_PROPERTY:bplib_fwp,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_fwp PRIVATE
    $<TARGET_PROPERTY:bplib_fwp,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_fwp PRIVATE
    $<TARGET_PROPERTY:bplib_fwp,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_fwp-testrunner
    utilities/bplib_fwp_test_utils.c
    bplib_fwp_test.c
    $<TARGET_OBJECTS:utobj_bplib_fwp>
)

target_include_directories(coverage-bplib_fwp-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_fwp,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_fwp-testrunner PUBLIC
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_fwp-testrunner coverage-bplib_fwp-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_fwp-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

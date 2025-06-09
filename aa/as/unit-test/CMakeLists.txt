##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_as_stubs STATIC
    stubs/bplib_as_stubs.c
    stubs/bplib_as_internal_stubs.c
    stubs/bplib_as_handlers.c
)

target_include_directories(bplib_as_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_as,INTERFACE_INCLUDE_DIRECTORIES>
    ../src
    stubs
)

target_link_libraries(bplib_as_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_as OBJECT
    ../src/bplib_as.c
    ../src/bplib_as_internal.c
)

target_compile_definitions(utobj_bplib_as PRIVATE
    $<TARGET_PROPERTY:bplib_as,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_as PRIVATE
    $<TARGET_PROPERTY:bplib_as,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_as PRIVATE
    $<TARGET_PROPERTY:bplib_as,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_as-testrunner
    utilities/bplib_as_test_utils.c
    bplib_as_test.c
    bplib_as_internal_test.c
    $<TARGET_OBJECTS:utobj_bplib_as>
)

target_include_directories(coverage-bplib_as-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_as,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_fwp,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bpa_fwp_stubs,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_as-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    bplib_fwp_stubs
    bplib_em_stubs
    bpa_fwp_stubs
    bplib_time_stubs
    ut_osapi_stubs
    bplib_eid_stubs
)

add_test(coverage-bplib_as-testrunner coverage-bplib_as-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_as-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

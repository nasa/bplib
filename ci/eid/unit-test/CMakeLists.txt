##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_eid_stubs STATIC
    stubs/bplib_eid_stubs.c
)

target_include_directories(bplib_eid_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
    stubs/
)

target_link_libraries(bplib_eid_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_eid OBJECT
    ../src/bplib_eid.c
)

target_compile_definitions(utobj_bplib_eid PRIVATE
    $<TARGET_PROPERTY:bplib_eid,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_eid PRIVATE
    $<TARGET_PROPERTY:bplib_eid,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_eid PRIVATE
    $<TARGET_PROPERTY:bplib_eid,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_eid-testrunner
    utilities/bplib_eid_test_utils.c
    bplib_eid_test.c
    $<TARGET_OBJECTS:utobj_bplib_eid>
)

target_include_directories(coverage-bplib_eid-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_eid-testrunner PUBLIC
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_eid-testrunner coverage-bplib_eid-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_eid-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_rbt_stubs STATIC
    stubs/bplib_rbt_stubs.c
)

target_include_directories(bplib_rbt_stubs PUBLIC
    ../src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_rbt,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_rbt_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_rbt OBJECT
    ../src/bplib_rbt_impl.c
    ../src/bplib_rbt_iterator.c
    ../src/bplib_rbt_api.c
)

target_compile_definitions(utobj_bplib_rbt PRIVATE
    $<TARGET_PROPERTY:bplib_rbt,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_rbt PRIVATE
    $<TARGET_PROPERTY:bplib_rbt,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_rbt PRIVATE
    $<TARGET_PROPERTY:bplib_rbt,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_crc,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_rbt-testrunner
    utilities/bplib_rbt_test_utils.c
    bplib_rbt_test.c
    $<TARGET_OBJECTS:utobj_bplib_rbt>
)

target_include_directories(coverage-bplib_rbt-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_rbt,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_rbt-testrunner PUBLIC
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_rbt-testrunner coverage-bplib_rbt-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_rbt-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

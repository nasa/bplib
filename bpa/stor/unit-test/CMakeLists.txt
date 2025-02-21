##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_stor_stubs STATIC
    stubs/bplib_stor_stubs.c
)

target_include_directories(bplib_stor_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_stor,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_stor_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_stor OBJECT
    ../src/bplib_stor.c
)

target_compile_definitions(utobj_bplib_stor PRIVATE
    $<TARGET_PROPERTY:bplib_stor,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_stor PRIVATE
    $<TARGET_PROPERTY:bplib_stor,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_stor PRIVATE
    $<TARGET_PROPERTY:bplib_stor,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_stor-testrunner
    utilities/bplib_stor_test_utils.c
    bplib_stor_test.c
    $<TARGET_OBJECTS:utobj_bplib_stor>
)

target_include_directories(coverage-bplib_stor-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_stor,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_stor-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    bplib_em_stubs
    bplib_qm_stubs
)

add_test(coverage-bplib_stor-testrunner coverage-bplib_stor-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_stor-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

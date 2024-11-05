##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_em_stubs STATIC
    stubs/bplib_em_stubs.c
)

target_include_directories(bplib_em_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_em_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_em OBJECT
    ../src/bplib_em.c
)

target_compile_definitions(utobj_bplib_em PRIVATE
    $<TARGET_PROPERTY:bplib_em,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_em PRIVATE
    $<TARGET_PROPERTY:bplib_em,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_em PRIVATE
    $<TARGET_PROPERTY:bplib_em,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_em-testrunner
    utilities/bplib_em_test_utils.c
    bplib_em_test.c
    $<TARGET_OBJECTS:utobj_bplib_em>
)

target_include_directories(coverage-bplib_em-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bpa_fwp_stubs,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_em-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    bplib_fwp_stubs
    bpa_fwp_stubs
)

add_test(coverage-bplib_em-testrunner coverage-bplib_em-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_em-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

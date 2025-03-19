##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_pi_stubs STATIC
    stubs/bplib_pi_stubs.c
)

target_include_directories(bplib_pi_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_pi,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_pi_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_pi OBJECT
    ../src/bplib_pi.c
)

target_compile_definitions(utobj_bplib_pi PRIVATE
    $<TARGET_PROPERTY:bplib_pi,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_pi PRIVATE
    $<TARGET_PROPERTY:bplib_pi,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_pi PRIVATE
    $<TARGET_PROPERTY:bplib_pi,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_pi-testrunner
    utilities/bplib_pi_test_utils.c
    bplib_pi_test.c
    $<TARGET_OBJECTS:utobj_bplib_pi>
)

target_include_directories(coverage-bplib_pi-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_pi,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_as,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_qm,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_fwp,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_time,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_nc,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_pi-testrunner PUBLIC
    bplib_as_stubs
    bplib_mem_stubs
    bplib_qm_stubs
    bplib_fwp_stubs
    bplib_eid_stubs
    bplib_time_stubs
    bplib_nc_stubs
    bplib_em_stubs
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_pi-testrunner coverage-bplib_pi-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_pi-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

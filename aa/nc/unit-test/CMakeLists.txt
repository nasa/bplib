##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_nc_stubs STATIC
    stubs/bplib_nc_stubs.c
    stubs/bplib_nc_directives_stubs.c
    stubs/bplib_nc_rwlock_stubs.c
)

target_include_directories(bplib_nc_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_nc,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_nc_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_nc OBJECT
    ../src/bplib_nc.c
    ../src/bplib_nc_directives.c
    ../src/bplib_nc_rwlock.c
    ../src/bplib_nc_internal.c
)

target_compile_definitions(utobj_bplib_nc PRIVATE
    $<TARGET_PROPERTY:bplib_nc,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_nc PRIVATE
    $<TARGET_PROPERTY:bplib_nc,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_nc PRIVATE
    $<TARGET_PROPERTY:bplib_nc,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_nc-testrunner
    utilities/bplib_nc_test_utils.c
    bplib_nc_test.c
    bplib_nc_rwlock_test.c
    $<TARGET_OBJECTS:utobj_bplib_nc>
)

target_include_directories(coverage-bplib_nc-testrunner PRIVATE
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib_nc,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_stor,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_as,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bpa_fwp_stubs,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em_stubs,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_eid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_cla,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_crc,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_nc-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    bplib_fwp_stubs
    bplib_em_stubs
    bplib_as_stubs
    bpa_fwp_stubs
    bplib_stor_stubs
    bplib_eid_stubs
    bplib_cla_stubs
    bplib_crc_stubs
    bplib_pi_stubs
)

add_test(coverage-bplib_nc-testrunner coverage-bplib_nc-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_nc-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

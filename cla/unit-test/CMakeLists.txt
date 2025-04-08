##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_cla_stubs STATIC
    stubs/bplib_cla_stubs.c
    stubs/bplib_cla_internal_stubs.c
    stubs/bplib_cla_handlers.c
)

target_include_directories(bplib_cla_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_cla,INTERFACE_INCLUDE_DIRECTORIES>
    ../src
)

target_link_libraries(bplib_cla_stubs PUBLIC ut_assert)

# Create unit test object
add_library(utobj_bplib_cla OBJECT
    ../src/bplib_cla.c
    ../src/bplib_cla_internal.c
)

target_compile_definitions(utobj_bplib_cla PRIVATE
    $<TARGET_PROPERTY:bplib_cla,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_cla PRIVATE
    $<TARGET_PROPERTY:bplib_cla,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_cla PRIVATE
    $<TARGET_PROPERTY:bplib_cla,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_cla-testrunner
    utilities/bplib_cla_test_utils.c
    bplib_cla_test.c
    bplib_cla_internal_test.c
    $<TARGET_OBJECTS:utobj_bplib_cla>
)

target_include_directories(coverage-bplib_cla-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_qm,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_bi,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_cla,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_nc,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bpa_fwp_stubs,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em_stubs,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_cla-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    bplib_bi_stubs
    bplib_mem_stubs
    bplib_qm_stubs
    bplib_nc_stubs
    bplib_fwp_stubs
    bpa_fwp_stubs
    bplib_em_stubs
)

add_test(coverage-bplib_cla-testrunner coverage-bplib_cla-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_cla-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

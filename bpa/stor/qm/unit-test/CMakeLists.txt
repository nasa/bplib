##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_subdirectory(stubs)

# Create unit test object
add_library(utobj_bplib_stor_qm OBJECT
    # ../src/bplib_ct_custody.c
    ../src/bplib_stor_qm.c
    ../src/bplib_stor_qm_cla_api.c
    # ../src/bplib_stor_qm_dataservice_api.c
    # ../src/bplib_stor_qm_fsm.c
    # ../src/bplib_stor_qm_job.c
    # ../src/crc.c
)

target_compile_definitions(utobj_bplib_stor_qm PRIVATE
    $<TARGET_PROPERTY:bplib_mem,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:bplib_stor_cache,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_stor_qm PRIVATE
    $<TARGET_PROPERTY:bplib_mem,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:bplib_stor_cache,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_stor_qm PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_stor_cache,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# TODO Add QM Unit Tests
# Add executable
add_executable(coverage-bplib_stor_qm-testrunner
    # test_bplib_stor_qm_bblocks.c
    # test_bplib_stor_qm_cla_api.c
    # test_bplib_stor_qm_custody.c
    # test_bplib_stor_qm_dataservice_api.c
    # test_bplib_stor_qm_queueing.c
    # test_bplib_stor_qm_ref.c
    test_bplib_stor_qm_setup.c
    $<TARGET_OBJECTS:utobj_bplib_stor_qm>
)

target_include_directories(coverage-bplib_stor_qm-testrunner PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}../inc
    ${CMAKE_CURRENT_SOURCE_DIR}../src

    $<TARGET_PROPERTY:bplib_stor_cache,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_time,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_stor_qm-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    coverage-bplib-stubs
    bplib_mem_stubs
    bplib_time_stubs
    bplib_stor_cache_stubs
)

# TODO Add the qm testrunner when QM is ready.
add_test(coverage-bplib_stor_qm-testrunner coverage-bplib_stor_qm-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_stor_qm-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

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
add_library(utobj_bplib_stor_cache OBJECT
    ../src/bplib_stor_cache.c
    ../src/bplib_stor_cache_internal.c
    ../src/bplib_stor_cache_lists.c
    ../src/bplib_stor_cache_bblocks.c
    # Incomplete state entry ../src/bplib_stor_cache_fsm.c
    # Used but never defined error ../src/bplib_stor_cache_module_api.c
    ../src/bplib_stor_cache_ref.c
    ../src/crc.c
    utilities/bplib_stor_cache_utils.c
)

target_compile_definitions(utobj_bplib_stor_cache PRIVATE
    $<TARGET_PROPERTY:bplib_stor_cache,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_stor_cache PRIVATE
    $<TARGET_PROPERTY:bplib_stor_cache,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_stor_cache PRIVATE
    $<TARGET_PROPERTY:bplib_stor_cache,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_stor_cache-testrunner
    test_bplib_stor_cache.c
    # test_bplib_stor_cache_fsm.c
    # test_bplib_stor_cache_module_api.c
    test_bplib_stor_cache_setup.c
    $<TARGET_OBJECTS:utobj_bplib_stor_cache>
)

target_include_directories(coverage-bplib_stor_cache-testrunner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}../inc
    ${CMAKE_CURRENT_SOURCE_DIR}../src

    $<TARGET_PROPERTY:bplib_stor_cache,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem_stubs,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_time_stubs,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_stor_qm_stubs,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_stor_cache-testrunner PUBLIC
    ut_coverage_link
    bplib_mem
    bplib_time_stubs
    bplib_stor_qm_stubs
    ut_assert
)

add_test(coverage-bplib_stor_cache-testrunner coverage-bplib_stor_cache-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_stor_cache-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

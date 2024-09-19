##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(bplib_stor_cache_coverage OBJECT
    ../src/bplib_cache.c
    ../src/bplib_cache_bblocks.c
    ../src/bplib_cache_cla_api.c
    ../src/bplib_cache_custody.c
    ../src/bplib_cache_dataservice_api.c
    ../src/bplib_cache_enqueueing.c
    ../src/bplib_cache_fsm.c
    ../src/bplib_cache_ref.c
)

target_compile_definitions(bplib_stor_cache_coverage PRIVATE
    $<TARGET_PROPERTY:bplib_stor_cache,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(bplib_stor_cache_coverage PRIVATE
    $<TARGET_PROPERTY:bplib_stor_cache,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(bplib_stor_cache_coverage PRIVATE
    $<TARGET_PROPERTY:bplib_stor_cache,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

add_subdirectory(stubs)

# Add executable
add_executable(coverage-bplib_stor_cache-testrunner
    test_bplib_stor_cache.c
    test_bplib_stor_cache_bblocks.c
    test_bplib_stor_cache_custody.c
    test_bplib_stor_cache_fsm.c
    test_bplib_stor_cache_module.c
    test_bplib_stor_cache_ref.c
    test_bplib_stor_cache_setup.c
    test_bplib_stor_cla_api.c
    test_bplib_stor_dataservice_api.c
    test_bplib_stor_enqueueing.c
    $<TARGET_OBJECTS:bplib_stor_cache_coverage>
    $<TARGET_OBJECTS:bplib_stor_cache_stubs>
)

target_include_directories(coverage-bplib_stor_cache-testrunner PRIVATE
    $<TARGET_PROPERTY:bplib_stor_cache,INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_stor_cache-testrunner PUBLIC
    ut_coverage_link
    bplib_stor_cache_stubs
    bplib_mem_stubs
    bplib_mem_common_stubs
    bplib_os_stubs
    ut_assert
)

add_test(coverage-bplib_stor_cache-testrunner coverage-bplib_stor_cache-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_stor_cache-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

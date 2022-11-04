##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_cache OBJECT
    ../src/v7_cache.c
    ../src/v7_cache_custody.c
    ../src/v7_cache_fsm.c
)

target_compile_definitions(utobj_bplib_cache PRIVATE
    $<TARGET_PROPERTY:bplib_cache,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_cache PRIVATE
    $<TARGET_PROPERTY:bplib_cache,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_cache PRIVATE
    $<TARGET_PROPERTY:bplib_cache,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_cache-testrunner
    test_bplib_cache_setup.c
    test_v7_cache.c
    test_v7_cache_custody.c
    test_v7_cache_fsm.c
    $<TARGET_OBJECTS:utobj_bplib_cache>
)

target_include_directories(coverage-bplib_cache-testrunner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_cache,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_cache-testrunner PUBLIC
    ut_coverage_link
    coverage-bplib-stubs
    bplib_base_stubs
    bplib_mpool_stubs
    bplib_common_stubs
    bplib_os_stubs
    bplib_v7_stubs
    ut_assert
)

add_test(coverage-bplib_cache-testrunner coverage-bplib_cache-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_cache-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_mpool OBJECT
    ../src/v7_mpool_bblocks.c
    ../src/v7_mpool.c
    ../src/v7_mpool_flows.c
    ../src/v7_mpool_job.c
    ../src/v7_mpool_ref.c
    ../src/v7_mpstream.c
)

target_compile_definitions(utobj_bplib_mpool PRIVATE
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_mpool PRIVATE
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_mpool PRIVATE
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_mpool-testrunner
    test_bplib_mpool_setup.c
    test_bplib_v7_mpool_bblocks.c
    test_bplib_v7_mpool.c
    test_bplib_v7_mpool_flows.c
    test_bplib_v7_mpool_job.c
    test_bplib_v7_mpool_ref.c
    test_bplib_v7_mpstream.c
    $<TARGET_OBJECTS:utobj_bplib_mpool>
)

target_include_directories(coverage-bplib_mpool-testrunner PRIVATE
    ../src
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_mpool-testrunner PUBLIC
    ut_coverage_link
    bplib_common_stubs
    bplib_os_stubs
    ut_assert
)

add_test(coverage-bplib_mpool-testrunner coverage-bplib_mpool-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_mpool-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_mem OBJECT
    ../src/bplib_mem.c
    ../src/bplib_mem_bblocks.c
    ../src/bplib_mem_ducts.c
    ../src/bplib_mem_ref.c)

target_compile_definitions(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_mem-testrunner
    test_bplib_mpool_setup.c
    test_bplib_v7_mpool_bblocks.c
    test_bplib_v7_mpool.c
    # STOR test_bplib_v7_mpool_flows.c
    # STOR test_bplib_v7_mpool_job.c
    test_bplib_v7_mpool_ref.c
    # STOR test_bplib_v7_mpstream.c
    $<TARGET_OBJECTS:utobj_bplib_mem>
)

target_include_directories(coverage-bplib_mem-testrunner PRIVATE
    ../src
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_mem-testrunner PUBLIC
    ut_coverage_link
    bplib_os_stubs
    ut_assert
)

# STOR add_test(coverage-bplib_mem-testrunner coverage-bplib_mem-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_mem-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

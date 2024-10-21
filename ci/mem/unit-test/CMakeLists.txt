##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_mem_stubs STATIC
    stubs/bplib_mem_stubs.c
    stubs/bplib_mem_rbtree_stubs.c
)

target_include_directories(bplib_mem_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    ../src
)

target_link_libraries(
    bplib_mem_stubs
    ut_assert
    ut_core_api_stubs
)

# Create unit test object
add_library(utobj_bplib_mem OBJECT
    ../src/bplib_mem.c
    ../src/bplib_mem_rbtree.c
    ../src/bplib_mem_internal.c
)

target_compile_definitions(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_crc,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_mem-testrunner
    utilities/bplib_mem_test_utils.c
    test_bplib_mem.c
    test_bplib_mem_rbtree.c
    test_bplib_mem_setup.c
    test_bplib_mem_subqs.c
    $<TARGET_OBJECTS:utobj_bplib_mem>
)

target_include_directories(coverage-bplib_mem-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_mem-testrunner PUBLIC
    ut_coverage_link
    ut_assert
    bplib_time
    bplib_fwp_stubs
    bplib_mem_stubs
    bplib_stor_qm_stubs
)

add_test(coverage-bplib_mem-testrunner coverage-bplib_mem-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_mem-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(bplib_mem_stubs STATIC
    stubs/bplib_mem_bblocks_stubs.c
    stubs/bplib_mem_ref_stubs.c
    stubs/bplib_mem_stubs.c
)

target_include_directories(bplib_mem_stubs PUBLIC
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_assert,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(
    bplib_mem_stubs
    bplib_os_stubs
    ut_assert
)

add_library(utobj_bplib_mem OBJECT
    ../src/bplib_mem.c
)

target_include_directories(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_assert,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_mem PRIVATE
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

# Add executable
add_executable(coverage-bplib_mem-testrunner
# STOR TODO Add test_bplib_mem.c. The original moved to test_bplib_cache.c
#    test_bplib_mem.c
    test_bplib_mem_setup.c
    utilities/bplib_mem_test_utils.c
    $<TARGET_OBJECTS:utobj_bplib_mem>
)

target_include_directories(coverage-bplib_mem-testrunner PRIVATE
    ../src
    utilities/
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_mem-testrunner PUBLIC
    ut_coverage_link
    bplib_os_stubs
    ut_assert
)

add_test(coverage-bplib_mem-testrunner coverage-bplib_mem-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_mem-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

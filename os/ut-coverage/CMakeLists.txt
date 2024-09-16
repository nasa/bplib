##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_os OBJECT
    ../src/osal.c
    ../src/heap.c
)

# the "override_inc" dir contains replacement versions of the C-library include files.
# This is only applied to the source files that strictly need this (i.e. those that call
# c library functions directly)
set_source_files_properties(../src/heap.c PROPERTIES INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/override_inc
)


target_compile_definitions(utobj_bplib_os PRIVATE
    $<TARGET_PROPERTY:bplib_os,INTERFACE_COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_os PRIVATE
    $<TARGET_PROPERTY:bplib_os,INTERFACE_COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)


target_include_directories(utobj_bplib_os PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES> in the list.
)

# Add executable
add_executable(coverage-bplib_os-testrunner
    test_bplib_os.c
    libc_stdlib_stubs.c
    $<TARGET_OBJECTS:utobj_bplib_os>
)

target_include_directories(coverage-bplib_os-testrunner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_os-testrunner PUBLIC
    ut_coverage_link
    ut_osapi_stubs
    ut_assert
)

add_test(coverage-bplib_os-testrunner coverage-bplib_os-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_os-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

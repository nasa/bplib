##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_common OBJECT
    ../src/crc.c
    ../src/v7_rbtree.c
)

target_compile_definitions(utobj_bplib_common PRIVATE
    $<TARGET_PROPERTY:bplib_common,INTERFACE_COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_common PRIVATE
    $<TARGET_PROPERTY:bplib_common,INTERFACE_COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_common PRIVATE
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_common-testrunner
    test_bplib_common.c
    test_bplib_crc.c
    test_bplib_v7_rbtree.c
    $<TARGET_OBJECTS:utobj_bplib_common>
)

target_include_directories(coverage-bplib_common-testrunner PRIVATE
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_common-testrunner PUBLIC
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_common_testrunner bplib_common-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_common-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

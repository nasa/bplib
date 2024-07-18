##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_cla OBJECT
    ../src/bplib_cla.c
)

target_compile_definitions(utobj_bplib_cla PRIVATE
    $<TARGET_PROPERTY:bplib_cla,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_cla PRIVATE
    $<TARGET_PROPERTY:bplib_cla,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_cla PRIVATE
    $<TARGET_PROPERTY:bplib_cla,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_cla-testrunner
    utilities/bplib_cla_test_utils.c
    bplib_cla_test.c
    $<TARGET_OBJECTS:utobj_bplib_cla>
)

target_include_directories(coverage-bplib_cla-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_cla,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_cla-testrunner PUBLIC
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_cla-testrunner coverage-bplib_cla-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_cla-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

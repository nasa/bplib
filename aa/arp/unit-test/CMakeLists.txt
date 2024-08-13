##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_arp_stubs STATIC
    stubs/bplib_arp_stubs.c
)

target_include_directories(bplib_arp_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_arp,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_arp_stubs ut_assert ut_core_api_stubs)

# Create unit test object
add_library(utobj_bplib_arp OBJECT
    ../src/bplib_arp.c
)

target_compile_definitions(utobj_bplib_arp PRIVATE
    $<TARGET_PROPERTY:bplib_arp,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_arp PRIVATE
    $<TARGET_PROPERTY:bplib_arp,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_arp PRIVATE
    $<TARGET_PROPERTY:bplib_arp,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_arp-testrunner
    utilities/bplib_arp_test_utils.c
    bplib_arp_test.c
    $<TARGET_OBJECTS:utobj_bplib_arp>
)

target_include_directories(coverage-bplib_arp-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_arp,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_arp-testrunner PUBLIC
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_arp-testrunner coverage-bplib_arp-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_arp-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

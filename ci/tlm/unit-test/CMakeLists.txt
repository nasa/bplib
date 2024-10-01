##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_tlm_stubs STATIC
    stubs/bplib_tlm_stubs.c
)

target_include_directories(bplib_tlm_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_tlm,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_tlm_stubs ut_assert ut_core_api_stubs)

# Create unit test object
add_library(utobj_bplib_tlm OBJECT
    ../src/bplib_tlm.c
)

target_compile_definitions(utobj_bplib_tlm PRIVATE
    $<TARGET_PROPERTY:bplib_tlm,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_tlm PRIVATE
    $<TARGET_PROPERTY:bplib_tlm,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_tlm PRIVATE
    $<TARGET_PROPERTY:bplib_tlm,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Create test runner executable
add_executable(coverage-bplib_tlm-testrunner
    utilities/bplib_tlm_test_utils.c
    bplib_tlm_test.c
    $<TARGET_OBJECTS:utobj_bplib_tlm>
)

target_include_directories(coverage-bplib_tlm-testrunner PRIVATE
    ../src
    utilities/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_tlm,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_tlm-testrunner PUBLIC
    ut_coverage_link
    ut_assert
)

add_test(coverage-bplib_tlm-testrunner coverage-bplib_tlm-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_tlm-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

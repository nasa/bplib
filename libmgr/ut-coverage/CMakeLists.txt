##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

add_library(utobj_bplib_base OBJECT
  ../src/v7_bplib.c
  ../src/v7_cla_api.c
  ../src/v7_dataservice_api.c
  ../src/v7_routing.c
)

target_compile_definitions(utobj_bplib_base PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(utobj_bplib_base PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(utobj_bplib_base PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(coverage-bplib_libmgr-testrunner
    test_bplib_base_setup.c
    test_v7_bplib.c
    test_v7_cla_api.c
    test_v7_dataservice_api.c
    test_v7_routing.c
    $<TARGET_OBJECTS:utobj_bplib_base>
)

target_include_directories(coverage-bplib_libmgr-testrunner PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_libmgr-testrunner PUBLIC
    ut_coverage_link
    bplib_common_stubs
    bplib_mpool_stubs
    bplib_cache_stubs
    bplib_v7_stubs
    bplib_os_stubs
    ut_assert
)

add_test(coverage-bplib_libmgr-testrunner coverage-bplib_libmgr-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS coverage-bplib_libmgr-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# TODO STOR Move CLA API an Routing to STOR
#   ../src/v7_cla_api.c
#   ../src/v7_routing.c

add_library(bplib_libmgr_coverage OBJECT
  ../src/v7_bplib.c
  ../src/v7_dataservice_api.c
)

target_compile_definitions(bplib_libmgr_coverage PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,COMPILE_DEFINITIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(bplib_libmgr_coverage PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,COMPILE_OPTIONS>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_COMPILE_OPTIONS>
)

target_include_directories(bplib_libmgr_coverage PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add executable
# TODO STOR move to STOR: test_v7_cla_api.c test_v7_dataservice_api.c test_v7_routing.c
# TODO STOR also libs: bplib_cache_stubs BPLib_MEM_stubs bplib_v7_stubs

add_executable(coverage-bplib_libmgr-testrunner
    test_bplib_base_setup.c
    test_v7_bplib.c
    $<TARGET_OBJECTS:bplib_libmgr_coverage>
)

target_include_directories(coverage-bplib_libmgr-testrunner PRIVATE
    $<TARGET_PROPERTY:bplib_libmgr,INCLUDE_DIRECTORIES>
)

target_link_libraries(coverage-bplib_libmgr-testrunner PUBLIC
    ut_coverage_link
    bplib_mem
    bplib_mem_common_stubs
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

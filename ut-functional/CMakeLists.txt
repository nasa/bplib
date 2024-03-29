##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Add executable
add_executable(functional-bplib_sanity-testrunner
    sanity-test.c
)

target_compile_features(functional-bplib_sanity-testrunner PUBLIC c_std_99)
target_compile_options(functional-bplib_sanity-testrunner PUBLIC ${BPLIB_COMMON_COMPILE_OPTIONS})

# Allow backdoor function calls, in some submodules, for now
# The sanity check is currently calling some submodule functions directly
# These are public at the submodule scope, but not external to bplib
target_include_directories(functional-bplib_sanity-testrunner PRIVATE
    $<TARGET_PROPERTY:bplib_cache,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(functional-bplib_sanity-testrunner PUBLIC
    bplib
    ut_assert
    osal
)

add_test(functional-bplib_sanity-testrunner functional-bplib_sanity-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS functional-bplib_sanity-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

##################################################################
#
# coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Add executable
# this links with the normal FSW RB-Tree object, does not rebuild it
add_executable(functional-bplib_rbtree-testrunner
    rbtest.c
    $<TARGET_OBJECTS:bplib_mem_common>
)

target_include_directories(functional-bplib_rbtree-testrunner PRIVATE
    $<TARGET_PROPERTY:bplib_mem_common,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(functional-bplib_rbtree-testrunner PUBLIC
    ut_assert
    osal
)

add_test(functional-bplib_rbtree-testrunner functional-bplib_rbtree-testrunner)

# Install the executables to a staging area for test in cross environments
if (INSTALL_TARGET_LIST)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS functional-bplib_rbtree-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()
endif()

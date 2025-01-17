##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "Types" submodule
#
##################################################################

add_library(bplib_ci_types OBJECT
    src/bplib_bundle.c
)

target_include_directories(bplib_ci_types PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
#if(BPLIB_ENABLE_UNIT_TESTS)
#    add_subdirectory(unit-test)
#endif(BPLIB_ENABLE_UNIT_TESTS)

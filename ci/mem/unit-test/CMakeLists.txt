##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_mem_stubs STATIC
    stubs/bplib_mem_stubs.c
)

target_include_directories(bplib_mem_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(bplib_mem_stubs PUBLIC ut_assert)

##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_crc_stubs STATIC
    stubs/bplib_crc_stubs.c
)

target_include_directories(bplib_crc_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_crc,INTERFACE_INCLUDE_DIRECTORIES>
    ../src
    $<TARGET_PROPERTY:ut_coverage_compile,INTERFACE_INCLUDE_DIRECTORIES>
)

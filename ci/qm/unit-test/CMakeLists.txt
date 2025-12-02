##################################################################
#
# Coverage test build recipe
#
# This CMake file contains the recipe for building the coverage tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Create stubs (for external use)
add_library(bplib_qm_stubs STATIC
    stubs/bplib_qm_stubs.c
    stubs/bplib_qm_waitqueue_stubs.c
    stubs/bplib_qm_handlers.c
)

target_include_directories(bplib_qm_stubs PUBLIC
    $<TARGET_PROPERTY:bplib_qm,INTERFACE_INCLUDE_DIRECTORIES>
    stubs/
)

target_link_libraries(bplib_qm_stubs PUBLIC ut_assert)

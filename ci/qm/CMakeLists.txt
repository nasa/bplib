##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "Queue Manager (QM)" submodule, which applies only to the
# bplib_stor_qm build.
#
##################################################################

add_library(bplib_qm OBJECT
    src/bplib_qm.c
    src/bplib_qm_job.c
    src/bplib_qm_waitqueue.c
)

target_include_directories(bplib_qm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_time,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_as,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_em,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_ebp,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

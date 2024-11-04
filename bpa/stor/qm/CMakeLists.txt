##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "Queue Manager (QM)" submodule, which applies only to the
# bplib_stor_qm build.
#
##################################################################

add_library(bplib_stor_qm OBJECT
    # TODO ct_custody build and qm builds broken by header file dependencies.
    # src/bplib_ct_custody.c
    src/bplib_stor_qm.c
    src/bplib_stor_qm_decode.c
    src/bplib_stor_qm_encode.c
    # src/bplib_stor_qm_cla_api.c
    # src/bplib_stor_qm_dataservice_api.c
    # src/bplib_stor_qm_fsm.c
    # src/bplib_stor_qm_job.c
    # src/crc.c
)

target_include_directories(bplib_stor_qm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_crc,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

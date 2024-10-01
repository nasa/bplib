##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the TLM module
#
##################################################################

add_library(bplib_tlm OBJECT
    src/bplib_tlm.c
)

target_include_directories(bplib_tlm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

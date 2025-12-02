##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the PDB module
#
##################################################################

add_library(bplib_pdb OBJECT
    src/bplib_pdb.c
)

target_include_directories(bplib_pdb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

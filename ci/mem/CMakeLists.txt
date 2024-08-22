##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the MEM module.
#
#
##################################################################

add_library(bplib_mem OBJECT
    src/bplib_mem_bblocks.c
    src/bplib_mem_ducts.c
    src/bplib_mem_ref.c
    src/bplib_mem.c
)

target_include_directories(bplib_mem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc

    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(ut-coverage)
endif(BPLIB_ENABLE_UNIT_TESTS)

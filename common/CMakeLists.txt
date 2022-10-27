##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "common" submodule
#
##################################################################

set(bplib_common_SOURCES
    src/crc.c
    src/v7_rbtree.c
)

add_library(bplib_common OBJECT
    ${bplib_common_SOURCES}
)

target_include_directories(bplib_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_include_directories(bplib_common PUBLIC
    $<TARGET_PROPERTY:bplib_base,INTERFACE_INCLUDE_DIRECTORIES>
)

if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(ut-coverage)
  add_subdirectory(ut-functional)
endif(BPLIB_ENABLE_UNIT_TESTS)

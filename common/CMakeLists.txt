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

target_include_directories(bplib_common PRIVATE
    $<TARGET_PROPERTY:bplib_base,INTERFACE_INCLUDE_DIRECTORIES>
)

# Use the same basic compiler flags as bplib
target_compile_features(bplib_common PRIVATE c_std_99)
target_compile_options(bplib_common PRIVATE ${BPLIB_COMMON_COMPILE_OPTIONS})
set_target_properties(bplib_common PROPERTIES ${BPLIB_COMMON_PROPERTIES})

if(ENABLE_UNIT_TESTS)
  #add_subdirectory(ut-stubs)
  #add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "base" submodule
#
##################################################################

set(bplib_base_SOURCES
    src/v7_bplib.c
    src/v7_cla_api.c
    src/v7_dataservice_api.c
    src/v7_routing.c
)

add_library(bplib_base OBJECT
    ${bplib_base_SOURCES}
)

target_include_directories(bplib_base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc

    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
)

target_include_directories(bplib_base PRIVATE
    $<TARGET_PROPERTY:bplib_cache,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_v7,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(ut-coverage)
endif(BPLIB_ENABLE_UNIT_TESTS)

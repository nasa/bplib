##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "libmgr" submodule
#
##################################################################

set(bplib_libmgr_SOURCES
    src/v7_bplib.c
    src/v7_dataservice_api.c
    src/v7_routing.c
)
# Removed src/v7_cla_api.c // TODO STOR

add_library(bplib_libmgr OBJECT
    ${bplib_libmgr_SOURCES}
)

target_include_directories(bplib_libmgr PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc

    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(ut-coverage)
endif(BPLIB_ENABLE_UNIT_TESTS)

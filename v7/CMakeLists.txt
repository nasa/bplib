##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "mpool" submodule, which applies only to the
# v7 build.
#
##################################################################

# The BPv7 codec code currently uses the TinyCBOR external library
# Use pkg-config to locate tinycbor dependency (for now)
find_package(PkgConfig)
pkg_search_module(TINYCBOR tinycbor REQUIRED)
message(STATUS "Found tinycbor version ${TINYCBOR_VERSION}")

add_library(bplib_v7 OBJECT
    src/v7_codec.c
    src/v7.c
)

target_include_directories(bplib_v7 PRIVATE
    # nothing outside of here should directly call TinyCBOR, so this
    # can be considered private to this submodule
    ${TINYCBOR_INCLUDE_DIRS}
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
)

target_include_directories(bplib_v7 PUBLIC
    # Local include files
    ${CMAKE_CURRENT_SOURCE_DIR}/inc

    # other bplib submodules used by this submodule
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_base,INTERFACE_INCLUDE_DIRECTORIES>
)

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
    src/v7_codec_common.c
    src/v7_encode_api.c
    src/v7_decode_api.c
    src/v7_bp_basetypes.c
    src/v7_bp_container.c
    src/v7_bp_bitmap.c
    src/v7_bp_crc.c
    src/v7_bp_endpointid.c
    src/v7_bp_primary_block.c
    src/v7_bp_admin_record.c
    src/v7_bp_canonical_block.c
    src/v7_bp_bundle_age_block.c
    src/v7_bp_hop_count_block.c
    src/v7_bp_previous_node_block.c
    src/v7_custody_tracking_block.c
    src/v7_custody_acknowledgement_record.c
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

# Add unit test coverage subdirectory
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
  add_subdirectory(ut-coverage)
endif(BPLIB_ENABLE_UNIT_TESTS)

########################################################
#
# CMake Recipe to build BPLib API guide documentation
#
########################################################

#
# This CMake script currently defines a top-level target "bplib-apiguide"
# to build the BPLib API documentation.  This may be invoked either
# from the main BPLib CMakeLists.txt as a subdirectory (useful in the
# case of a self-contained/standalone build) or by a separate script
# (useful if integrating into a larger project with a separate doc build)
#
# To invoke from a separate documentation build, the following vars
# should be defined by the caller, before adding this subdirectory:
#
# BPLIB_API_INCLUDE_DIRECTORIES :
#   The list of directories that have the BPLib API headers
#   This should include the path to bplib_cfg.h to avoid warnings
#   about undefined references.
#
# BPLIBDOC_PREDEFINED :
#   Not used directly, but passed through to the "bplib-common.doxyfile"
#   This may be used to indicate preprocessor definitions that the
#   documentation generator tool should be aware of
#
# Note that BPLIB_API_INCLUDE_DIRECTORIES is defined by the parent script
# in a standalone build environment.
#

cmake_minimum_required(VERSION 3.5)
project(BPLIB_DOCS NONE)

# List of dox files to include -
# note that order is relevant here, doxygen processes in the order listed.
set(BPLIB_DOCFILE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/bplib_mainpage.dox
)

# For the generated Doxyfiles, the various paths should be in native form
set(BPLIB_NATIVE_APIGUIDE_SOURCEFILES)
set(BPLIB_NATIVE_INCLUDE_DIRS)
set(BPLIB_DOC_DEPENDENCY_LIST)

foreach(SRC ${BPLIB_DOCFILE_LIST})
    file(TO_NATIVE_PATH "${SRC}" SRC)
    string(APPEND BPLIB_NATIVE_APIGUIDE_SOURCEFILES " \\\n  ${SRC}")
endforeach()

# The complete list of public API include directories may be determined by reading the property of
# the "bplib_public_api" interface target.  However, the CFE documentation build may want to influence
# this and provide an alternate path, mainly to provide its own alternate version of "bplib_cfg.h" for
# docs.  It should be preferred to pull this info from the actual source such that it will track any
# changes made within BPLib as far as header organization goes.
if (TARGET bplib_public_api AND NOT BPLIB_API_INCLUDE_DIRECTORIES)
    get_target_property(BPLIB_API_INCLUDE_DIRECTORIES bplib_public_api INTERFACE_INCLUDE_DIRECTORIES)
    if (NOT BPLIB_API_INCLUDE_DIRECTORIES)
        set (BPLIB_API_INCLUDE_DIRECTORIES)
    endif()
    get_target_property(BPLIB_API_COMPILE_DEFINITIONS bplib_public_api INTERFACE_COMPILE_DEFINITIONS)
    if (NOT BPLIB_API_COMPILE_DEFINITIONS)
        set (BPLIB_API_COMPILE_DEFINITIONS)
    endif()
endif ()

# Generate the list of actual header files from the directories specified.  This is done
# as a target that runs a separate script such that generator expressions can be evaluated.
# This is done as a custom target such that it runs and gets updated every build
add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/docs/bplib-public-api.doxyfile"
    COMMAND ${CMAKE_COMMAND}
        -DINCLUDE_DIRECTORIES="${BPLIB_API_INCLUDE_DIRECTORIES}"
        -DCOMPILE_DEFINITIONS="${BPLIB_API_COMPILE_DEFINITIONS}"
        -DINPUT_TEMPLATE="${CMAKE_CURRENT_SOURCE_DIR}/bplib-public-api.doxyfile.in"
        -DOUTPUT_FILE="${CMAKE_BINARY_DIR}/docs/bplib-public-api.doxyfile"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/generate-public-api-doxyfile.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_target(bplib_public_api_headerlist
    DEPENDS "${CMAKE_BINARY_DIR}/docs/bplib-public-api.doxyfile")

# if building as part of CFS, then generate the doxygen header list as part of the prebuild step
# The "doc-prebuild" target is defined by the CFS build, and thus will not exist if building standalone
if (TARGET doc-prebuild)
   add_dependencies(doc-prebuild bplib_public_api_headerlist)
endif ()

file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/bplib-apiguide-warnings.log BPLIB_NATIVE_LOGFILE)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR}/docs/bplib-common.doxyfile BPLIB_NATIVE_COMMON_CFGFILE)
file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/bplib-apiguide.doxyfile BPLIB_NATIVE_APIGUIDE_CFGFILE)
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/default-settings.doxyfile BPLIB_NATIVE_DEFAULT_SETTINGS)

# Add a top level source directory if not defined
if (NOT DEFINED MISSION_SOURCE_DIR)
    set(MISSION_SOURCE_DIR ${CMAKE_SOURCE_DIR})
endif()

# generate the configuration files
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/bplib-common.doxyfile.in
        ${CMAKE_BINARY_DIR}/docs/bplib-common.doxyfile
        @ONLY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/bplib-apiguide.doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/bplib-apiguide.doxyfile
        @ONLY
)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
    COMMAND doxygen ${BPLIB_NATIVE_APIGUIDE_CFGFILE}
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/bplib-apiguide.doxyfile"
            "${CMAKE_BINARY_DIR}/docs/bplib-common.doxyfile"
            "${CMAKE_BINARY_DIR}/docs/bplib-public-api.doxyfile"
            ${BPLIB_DOCFILE_LIST} ${BPLIB_DOC_DEPENDENCY_LIST}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_target(bplib-apiguide
    COMMAND echo "BPLib API Guide: file://${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
)

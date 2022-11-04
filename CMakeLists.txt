##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This file allows BPLib to be configured and built via the CMake tool
# It also permits the BPLib module to be used in conjunction with CFE/CFS
#
##################################################################

cmake_minimum_required(VERSION 3.5)
project(BPLIB C)

set(BPLIB_OS_LAYER OSAL CACHE STRING "Operating system adapter layer to enable (POSIX or NASA OSAL)")

# Check if this is a subdirectory within a larger project, or if it was built standalone
# This affects how to find dependencies (e.g. OSAL) and whether to install the libraries
get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if (HAS_PARENT)
    set(BPLIB_STANDALONE_BUILD_MODE OFF)
else()
    set(BPLIB_STANDALONE_BUILD_MODE ON)
endif()

# The functional test program (bpcat) will only be built by default in a standalone build using POSIX OS adapter
# This is because the bpcat tool relies on direct file handle I/O as well as signals and other POSIX concepts
if (BPLIB_STANDALONE_BUILD_MODE AND BPLIB_OS_LAYER STREQUAL POSIX)
    set(BPLIB_DEFAULT_BUILD_TEST_TOOLS ON)
else()
    set(BPLIB_DEFAULT_BUILD_TEST_TOOLS OFF)
endif()

# The other unit tests (coverage) can only be built in conjuction with NASA OSAL, as this provides the UT asser
# framework that these tests are built upon.  The CFS build communicates this through a variable called ENABLE_UNIT_TESTS.
# If this is set, then inherit its value as the default for BPLIB_ENABLE_UNIT_TESTS (the user can still set it specifically though)
# Otherwise BPLIB_ENABLE_UNIT_TESTS can get its default based on whether this build is using OSAL
if (DEFINED ENABLE_UNIT_TESTS)
    set(BPLIB_DEFAULT_BUILD_UNIT_TESTS ${ENABLE_UNIT_TESTS})
elseif (BPLIB_OS_LAYER STREQUAL OSAL AND NOT IS_CFS_ARCH_BUILD)
    set(BPLIB_DEFAULT_BUILD_UNIT_TESTS ON)
else()
    set(BPLIB_DEFAULT_BUILD_UNIT_TESTS OFF)
endif()

option(BPLIB_BUILD_TEST_TOOLS "Whether or not to build the test programs as part of BPLib (standalone POSIX builds only)" ${BPLIB_DEFAULT_BUILD_TEST_TOOLS})
option(BPLIB_INSTALL_LIBRARIES "Whether or not to install the libraries" ${BPLIB_STANDALONE_BUILD_MODE})
option(BPLIB_USE_EXTERNAL_OSAL "Whether to use an external OSAL package, if OSAL is selected as OS layer" ${BPLIB_STANDALONE_BUILD_MODE})
option(BPLIB_ENABLE_UNIT_TESTS "Whether to build unit tests (requires NASA OSAL and UT Assert)" ${BPLIB_DEFAULT_BUILD_UNIT_TESTS})

set(BPLIB_VERSION_STRING "3.0.99") # development

# no extra link libraries at first
set(BPLIB_SRC)
set(BPLIB_COMMON_COMPILE_OPTIONS)

# If using an external OSAL package, then locate it now
# This needs to be done at the top level so the imported targets are
# available to all submodules, for unit testing in particular.
if (BPLIB_OS_LAYER STREQUAL OSAL)

  if (BPLIB_USE_EXTERNAL_OSAL)
    find_package(NasaOsal REQUIRED)
    message(STATUS "Using external OSAL ${NasaOsal_VERSION}")
  endif()

else()

    # Sanity Check - NASA OSAL provides UT assert, so unit tests cannot be built without this
    if (BPLIB_ENABLE_UNIT_TESTS)
        message(FATAL_ERROR "NASA OSAL is required to build unit tests")
    endif()
    if (IS_CFS_ARCH_BUILD)
        message(FATAL_ERROR "NASA OSAL is required to build CFS library")
    endif()

endif()

# If using a GNU-style compiler then enable full warnings here.
# May want to add equivalent for other compiler flavors too
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
   list(APPEND BPLIB_COMMON_COMPILE_OPTIONS -Wall -Werror -pedantic -Wno-format-truncation -Wno-stringop-truncation)
endif()

# If standalone build and not cross compile, then enable creation of the "make test" target
if (BPLIB_ENABLE_UNIT_TESTS AND BPLIB_STANDALONE_BUILD_MODE AND NOT CMAKE_CROSSCOMPILING)
   enable_testing()
endif()

add_subdirectory(os)
add_subdirectory(common)
add_subdirectory(lib)
add_subdirectory(mpool)
add_subdirectory(cache)
add_subdirectory(v7)

# v7 implementation parts
list(APPEND BPLIB_SRC
  store/file_offload.c

  $<TARGET_OBJECTS:bplib_os>
  $<TARGET_OBJECTS:bplib_common>
  $<TARGET_OBJECTS:bplib_cache>
  $<TARGET_OBJECTS:bplib_v7>
  $<TARGET_OBJECTS:bplib_mpool>
  $<TARGET_OBJECTS:bplib_base>
)

list(APPEND BPLIB_PRIVATE_INCLUDE_DIRS
  $<TARGET_PROPERTY:bplib_os,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:bplib_base,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:bplib_v7,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:bplib_mpool,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:bplib_cache,INTERFACE_INCLUDE_DIRECTORIES>
  $<TARGET_PROPERTY:bplib_common,INTERFACE_INCLUDE_DIRECTORIES>
)

# If building as part of CFE/CFS, then the "IS_CFS_ARCH_BUILD" should be set
# this allows simply dropping this module into a CFS project
if (IS_CFS_ARCH_BUILD)

  # Create the app module using the CFE/OSAL adapter layer
  # Note this ignores the setting of BPLIB_INCLUDE_POSIX, as OSAL provides this service
  # The CFE build system determines whether to create a shared or static object inside this routine
  add_cfe_app(bplib ${BPLIB_SRC})

  target_link_libraries(bplib ${TINYCBOR_LDFLAGS})

else()

  # Building as a standalone library
  # Add the generic library target
  # The flavor of library (shared/static) being built here depends on the BUILD_SHARED_LIBS option
  # This directory may be built twice, setting this option differently to build both flavors
  add_library(bplib ${BPLIB_SRC})

  # link with the requisite dependencies
  target_link_libraries(bplib ${TINYCBOR_LDFLAGS})

  # Add in the required link libraries based on OS adapter selection
  # this should preferably be in OS subdirectory, but it needs to be done
  # at the bplib target which is created here.
  if (BPLIB_OS_LAYER STREQUAL OSAL)
    target_link_libraries(bplib osal)
  elseif(BPLIB_OS_LAYER STREQUAL POSIX)
    target_link_libraries(bplib rt pthread)
  endif()

  # Install and also export this library, so it can be found via
  # "find_package()" from some other CMake build
  install(TARGETS bplib
    EXPORT bplib-export
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
  install(EXPORT bplib-export
    FILE BPLibTargets.cmake
    DESTINATION lib/cmake/bplib
  )
  install(DIRECTORY inc/
    DESTINATION include/bplib
  )

  # Set API/ABI version info if building a shared library
  if(BUILD_SHARED_LIBS)

    set_target_properties(bplib PROPERTIES
      SOVERSION "${BPLIB_VERSION_STRING}"
    )

    # If using GNU, then also add ABI version to final link
    if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
      set_target_properties(bplib PROPERTIES
        LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/libabi.version"
      )
    endif()

  endif()

  # Build the test tools, if directed
  if (BPLIB_BUILD_TEST_TOOLS)
    add_subdirectory(app)
  endif()

endif()

get_target_property(IS_PIC bplib POSITION_INDEPENDENT_CODE)
if (IS_PIC)
  set_target_properties(
    bplib_base bplib_common bplib_mpool bplib_cache bplib_v7 bplib_os
    PROPERTIES POSITION_INDEPENDENT_CODE ${IS_PIC}
  )
endif()

# Set the standard compile options for all submodules (c99, full warnings)
foreach(TGT bplib bplib_base bplib_common bplib_mpool bplib_cache bplib_v7 bplib_os)
  target_compile_features(${TGT} PRIVATE c_std_99)
  target_compile_options(${TGT} PRIVATE ${BPLIB_COMMON_COMPILE_OPTIONS})
endforeach()

# Internal/private header files exist within the implementation directories
target_include_directories(bplib PRIVATE ${BPLIB_PRIVATE_INCLUDE_DIRS})

# The API to this library (which may be invoked/referenced from other apps)
# is stored in fsw/public_inc.  Using "target_include_directories" is the
# preferred method of indicating this (vs. directory-scope "include_directories").
target_include_directories(bplib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<INSTALL_INTERFACE:include/bplib>
)

if (BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(ut-stubs)
endif (BPLIB_ENABLE_UNIT_TESTS)

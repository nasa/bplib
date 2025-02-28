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
   list(APPEND BPLIB_COMMON_COMPILE_OPTIONS -Wall -Werror -pedantic -Wno-format-truncation -Wno-stringop-truncation -g)
endif()

# If standalone build and not cross compile, then enable creation of the "make test" target
if (BPLIB_ENABLE_UNIT_TESTS AND BPLIB_STANDALONE_BUILD_MODE AND NOT CMAKE_CROSSCOMPILING)
   enable_testing()
endif()

list (APPEND BPLIB_COMPONENT_DIRS
  version cla aa/arp aa/as aa/fwp aa/nc bpa/bi bpa/ct
  bpa/ebp bpa/pdb bpa/pi bpa/stor
  ci/cbor ci/em ci/mem ci/pl ci/qm ci/time ci/crc ci/eid ci/rbt
)

foreach(COMPONENT ${BPLIB_COMPONENT_DIRS})
  add_subdirectory(${COMPONENT})
endforeach()

list(APPEND BPLIB_COMPONENTS
  bplib_cla bplib_arp bplib_as bplib_fwp bplib_nc
  bplib_bi bplib_ct bplib_ebp bplib_pdb bplib_pi
  bplib_stor bplib_cbor bplib_em bplib_mem bplib_pl bplib_time
  bplib_crc bplib_version bplib_qm bplib_eid bplib_rbt
)

foreach(COMPONENT ${BPLIB_COMPONENTS})
  list(APPEND BPLIB_SRC 
    $<TARGET_OBJECTS:${COMPONENT}>
  )
  
  list(APPEND BPLIB_COMPONENT_INCLUDE_DIRS
    $<TARGET_PROPERTY:${COMPONENT},INTERFACE_INCLUDE_DIRECTORIES>
  )

endforeach()

# If building as part of CFE/CFS, then the "IS_CFS_ARCH_BUILD" should be set
# this allows simply dropping this module into a CFS project
if (IS_CFS_ARCH_BUILD)

  # Create the app module using the CFE/OSAL adapter layer
  # Note this ignores the setting of BPLIB_INCLUDE_POSIX, as OSAL provides this service
  # The CFE build system determines whether to create a shared or static object inside this routine
  add_cfe_app(bplib ${BPLIB_SRC})

  # link with the requisite dependencies
  target_link_libraries(bplib qcbor m)

else()

  # Building as a standalone library
  # Add the generic library target
  # The flavor of library (shared/static) being built here depends on the BUILD_SHARED_LIBS option
  # This directory may be built twice, setting this option differently to build both flavors
  add_library(bplib ${BPLIB_SRC})

  # link with the requisite dependencies
  target_link_libraries(bplib qcbor m)

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

  target_compile_definitions(bplib_time PRIVATE 
    BPLIB_TIME_FILE_NAME="/bplib_time.dat"
  )
  add_subdirectory(app)


endif()

get_target_property(IS_PIC bplib POSITION_INDEPENDENT_CODE)
if (IS_PIC)
  set_target_properties(${BPLIB_COMPONENTS} PROPERTIES POSITION_INDEPENDENT_CODE ${IS_PIC})
endif()

# Set the standard compile options for all submodules (c99, full warnings)
foreach(COMPONENT ${BPLIB_COMPONENTS})
  target_compile_features(${COMPONENT} PRIVATE c_std_99)
  target_compile_options(${COMPONENT} PRIVATE ${BPLIB_COMMON_COMPILE_OPTIONS})
endforeach()

target_compile_features(bplib PRIVATE c_std_99)
target_compile_options(bplib PRIVATE ${BPLIB_COMMON_COMPILE_OPTIONS})

# The API to this library (which may be invoked/referenced from other apps)
# is stored in fsw/public_inc.  Using "target_include_directories" is the
# preferred method of indicating this (vs. directory-scope "include_directories").
target_include_directories(bplib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<INSTALL_INTERFACE:include/bplib>
)

# This makes the public header files for each component accessible to external apps
foreach(COMPONENT ${BPLIB_COMPONENT_DIRS})
  target_include_directories(bplib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/inc>
    $<INSTALL_INTERFACE:${COMPONENT}/inc>
  )

endforeach()

# BPLib Sanity checks are only for standalone builds, sanity checks
# for CFE/CFS builds should be part of the BP app, as opposed to BPLib
if (BPLIB_ENABLE_UNIT_TESTS)

  # Link all stubs into single library
  add_library(coverage-bplib-stubs INTERFACE)

  foreach(COMPONENT ${BPLIB_COMPONENTS})
    target_link_libraries(coverage-bplib-stubs INTERFACE ${COMPONENT}_stubs)
  endforeach()

  if (NOT IS_CFS_ARCH_BUILD)
    add_subdirectory(ut-functional)
  endif (NOT IS_CFS_ARCH_BUILD)
  
endif (BPLIB_ENABLE_UNIT_TESTS)

# Prepare the BPLib Doxygen build.
set(BPLIB_API_INCLUDE_DIRECTORIES ${BPLIB_COMPONENT_INCLUDE_DIRS})
add_subdirectory(docs/doc-src)

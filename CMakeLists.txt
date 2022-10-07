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

option(BPLIB_INCLUDE_POSIX "Whether or not to the POSIX operating system abstraction as part of BPLib (standalone builds only)" ON)

set(BPLIB_VERSION_STRING "3.0.99") # development

set(BPLIB_COMMON_COMPILE_OPTIONS)
set(BPLIB_COMMON_PROPERTIES)

# If doing a CFS build, then build position independent code (-fPIC) for all objects
if(IS_CFS_ARCH_BUILD OR BUILD_SHARED_LIBS)
  list(APPEND BPLIB_COMMON_PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
else()
  list(APPEND BPLIB_COMMON_PROPERTIES POSITION_INDEPENDENT_CODE FALSE)
endif()


# If using a GNU-style compiler then enable full warnings here.
# May want to add equivalent for other compiler flavors too
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
   list(APPEND BPLIB_COMMON_COMPILE_OPTIONS -Wall -Werror -pedantic -Wno-format-truncation -Wno-stringop-truncation)
endif()

# no extra link libraries at first
set(BPLIB_LINK_LIBRARIES)

add_subdirectory(common)
add_subdirectory(lib)
add_subdirectory(mpool)
add_subdirectory(cache)
add_subdirectory(v7)

# v7 implementation parts
list(APPEND BPLIB_SRC
  store/file_offload.c

  $<TARGET_OBJECTS:bplib_common>
  $<TARGET_OBJECTS:bplib_cache>
  $<TARGET_OBJECTS:bplib_v7>
  $<TARGET_OBJECTS:bplib_mpool>
  $<TARGET_OBJECTS:bplib_base>
)

list(APPEND BPLIB_PRIVATE_INCLUDE_DIRS
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
  # TEMPORARY - for now this will also use POSIX until OSAL has the needed features.
  add_cfe_app(bplib ${BPLIB_SRC} os/posix.c)

  target_link_libraries(bplib ${TINYCBOR_LDFLAGS})

else()

  # Building as a standalone library
  # This by will usually require a POSIX OS layer
  if (BPLIB_INCLUDE_POSIX)

    list(APPEND BPLIB_SRC os/posix.c)
    list(APPEND BPLIB_LINK_LIBRARIES rt pthread)

  endif()

  # Add the generic library target
  # The flavor of library (shared/static) being built here depends on the BUILD_SHARED_LIBS option
  # This directory may be built twice, setting this option differently to build both flavors
  add_library(bplib ${BPLIB_SRC})

  # compile this library as c99 (but this does not impose the same requirement on other users)
  target_compile_features(bplib PRIVATE c_std_99)

  # If using GNU GCC, then also enable full warning reporting
  target_compile_options(bplib PRIVATE ${BPLIB_COMMON_COMPILE_OPTIONS})

  # link with the requisite dependencies (this is mainly for POSIX, if using that adapter)
  target_link_libraries(bplib ${TINYCBOR_LDFLAGS} ${BPLIB_LINK_LIBRARIES})

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

# Internal/private header files exist within the implementation directories
target_include_directories(bplib PRIVATE ${BPLIB_PRIVATE_INCLUDE_DIRS})

# The API to this library (which may be invoked/referenced from other apps)
# is stored in fsw/public_inc.  Using "target_include_directories" is the
# preferred method of indicating this (vs. directory-scope "include_directories").
target_include_directories(bplib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  $<INSTALL_INTERFACE:include/bplib>
)

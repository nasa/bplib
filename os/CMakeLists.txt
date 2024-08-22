##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the "os" submodule
#
##################################################################

set(bplib_os_SOURCES
    src/heap.c
)

if (BPLIB_OS_LAYER STREQUAL OSAL)

    if (IS_CFS_ARCH_BUILD)
        list(APPEND bplib_os_SOURCES src/cfe.c)
    endif()

    list(APPEND bplib_os_SOURCES src/osal.c)

elseif(BPLIB_OS_LAYER STREQUAL POSIX)

    list(APPEND bplib_os_SOURCES src/posix.c)

else()
    message(FATAL_ERROR "Unknown OS adapter: ${BPLIB_OS_LAYER}")
endif()

add_library(bplib_os OBJECT
    ${bplib_os_SOURCES}
)

target_include_directories(bplib_os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_libmgr,INTERFACE_INCLUDE_DIRECTORIES>
)

if(BPLIB_ENABLE_UNIT_TESTS)
    add_subdirectory(ut-stubs)
    add_subdirectory(ut-coverage)
endif(BPLIB_ENABLE_UNIT_TESTS)

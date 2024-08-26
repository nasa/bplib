##################################################################
#
# Bundle Protocol Library (BPLib) CMake build recipe
#
# This is for the TIME module
#
##################################################################

add_library(bplib_time OBJECT
    src/bplib_time.c
    src/bplib_time_internal.c
)

target_link_libraries(bplib_time osal)

target_include_directories(bplib_time PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)

# Add unit tests
if(BPLIB_ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif(BPLIB_ENABLE_UNIT_TESTS)

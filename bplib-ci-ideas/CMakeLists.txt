cmake_minimum_required(VERSION 3.16)
project(bplibideas C)

add_subdirectory(ci)
add_subdirectory(qm)

add_executable(qmtest
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    $<TARGET_OBJECTS:bplib_mem>
    $<TARGET_OBJECTS:bplib_types>
    $<TARGET_OBJECTS:bplib_qm>
)
target_include_directories(qmtest PUBLIC
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_types,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_qm,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(qmtest -lpthread)

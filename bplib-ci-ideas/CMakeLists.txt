cmake_minimum_required(VERSION 3.16)
project(bplibideas C)

add_subdirectory(ci)

add_executable(driver
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    $<TARGET_OBJECTS:bplib_mem>
    $<TARGET_OBJECTS:bplib_queue>
)
target_include_directories(driver PUBLIC $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(driver PUBLIC $<TARGET_PROPERTY:bplib_queue,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(driver -lpthread)
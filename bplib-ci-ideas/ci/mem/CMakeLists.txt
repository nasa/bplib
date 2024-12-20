add_library(bplib_mem OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/allocator.c
)

target_include_directories(bplib_mem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

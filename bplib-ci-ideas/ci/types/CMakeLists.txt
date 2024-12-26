add_library(bplib_types OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/waitqueue.c
)

target_include_directories(bplib_types PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

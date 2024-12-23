add_library(bplib_queue OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/waitqueue.c
)

target_include_directories(bplib_queue PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

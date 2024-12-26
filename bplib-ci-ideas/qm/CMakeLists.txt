add_library(bplib_qm OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qm.c
)

target_include_directories(bplib_qm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    $<TARGET_PROPERTY:bplib_mem,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:bplib_types,INTERFACE_INCLUDE_DIRECTORIES>
)

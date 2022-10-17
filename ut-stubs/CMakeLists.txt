add_library(coverage-bplib-stubs
    bplib_stubs.c
    bplib_routing_stubs.c
)
target_include_directories(coverage-bplib-stubs PUBLIC
    $<TARGET_PROPERTY:bplib,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(coverage-bplib-stubs PUBLIC
    ut_assert
)

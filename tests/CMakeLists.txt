enable_testing()

add_executable(tinymcp_smoketest
    smoketest.cpp)

target_link_libraries(tinymcp_smoketest PRIVATE tinymcp)

add_test(NAME tinymcp_smoketest COMMAND tinymcp_smoketest)


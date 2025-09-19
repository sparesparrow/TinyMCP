# Basic test setup (can be expanded with GoogleTest later)
add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic TinyMCP)
add_test(NAME BasicTest COMMAND test_basic)

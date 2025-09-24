cmake_minimum_required(VERSION 3.20)
project(MCP_SDK VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Build type options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(PkgConfig REQUIRED)

# Find JSON library (nlohmann/json or jsoncpp)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    pkg_check_modules(JSONCPP jsoncpp)
    if(NOT JSONCPP_FOUND)
        message(FATAL_ERROR "Neither nlohmann/json nor jsoncpp found. Please install one of them.")
    endif()
endif()

# Find curl for HTTP transport
find_package(CURL REQUIRED)

# Find OpenSSL for HTTPS support
find_package(OpenSSL REQUIRED)

# Find Threads
find_package(Threads REQUIRED)

# Find WebSocket library (libwebsockets or similar)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWEBSOCKETS libwebsockets)
if(NOT LIBWEBSOCKETS_FOUND)
    message(WARNING "libwebsockets not found. WebSocket transport will not be available.")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(nlohmann_json_FOUND)
    target_compile_definitions(mcp_sdk PRIVATE MCP_USE_NLOHMANN_JSON)
else()
    include_directories(${JSONCPP_INCLUDE_DIRS})
endif()

# Source files
set(MCP_SDK_SOURCES
    src/core/types.cpp
    src/core/message.cpp
    src/core/error.cpp
    src/transport/stdio_transport.cpp
    src/transport/http_transport.cpp
    src/transport/websocket_transport.cpp
    src/transport/sse_transport.cpp
    src/client/client.cpp
    src/server/server.cpp
    src/tools/tools.cpp
    src/resources/resources.cpp
    src/prompts/prompts.cpp
    src/capabilities/capabilities.cpp
    src/utils/json.cpp
    src/utils/logger.cpp
    src/utils/thread_pool.cpp
    src/examples/basic_server.cpp
    src/examples/basic_client.cpp
)

# Create the main library
add_library(mcp_sdk STATIC ${MCP_SDK_SOURCES})

# Set target properties
set_target_properties(mcp_sdk PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Link libraries
target_link_libraries(mcp_sdk
    Threads::Threads
    ${CURL_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(nlohmann_json_FOUND)
    target_link_libraries(mcp_sdk nlohmann_json::nlohmann_json)
else()
    target_link_libraries(mcp_sdk ${JSONCPP_LIBRARIES})
endif()

if(LIBWEBSOCKETS_FOUND)
    target_link_libraries(mcp_sdk ${LIBWEBSOCKETS_LIBRARIES})
    target_include_directories(mcp_sdk PRIVATE ${LIBWEBSOCKETS_INCLUDE_DIRS})
    target_compile_definitions(mcp_sdk PRIVATE MCP_USE_LIBWEBSOCKETS)
endif()

# Compile definitions
if(nlohmann_json_FOUND)
    target_compile_definitions(mcp_sdk PRIVATE MCP_USE_NLOHMANN_JSON)
else()
    target_compile_definitions(mcp_sdk PRIVATE ${JSONCPP_CFLAGS_OTHER})
endif()

# Example executables
add_executable(mcp_server_example examples/server_example.cpp)
target_link_libraries(mcp_server_example mcp_sdk)

add_executable(mcp_client_example examples/client_example.cpp)
target_link_libraries(mcp_client_example mcp_sdk)

add_executable(mcp_echo_server examples/echo_server.cpp)
target_link_libraries(mcp_echo_server mcp_sdk)

add_executable(mcp_echo_client examples/echo_client.cpp)
target_link_libraries(mcp_echo_client mcp_sdk)

add_executable(mcp_tools_demo examples/tools_demo.cpp)
target_link_libraries(mcp_tools_demo mcp_sdk)

add_executable(mcp_resources_demo examples/resources_demo.cpp)
target_link_libraries(mcp_resources_demo mcp_sdk)

add_executable(mcp_prompts_demo examples/prompts_demo.cpp)
target_link_libraries(mcp_prompts_demo mcp_sdk)

# Test executable
if(BUILD_TESTS)
    enable_testing()
    
    add_executable(mcp_tests
        tests/test_types.cpp
        tests/test_message.cpp
        tests/test_error.cpp
        tests/test_transport.cpp
        tests/test_client.cpp
        tests/test_server.cpp
        tests/test_tools.cpp
        tests/test_resources.cpp
        tests/test_prompts.cpp
        tests/test_capabilities.cpp
        tests/test_utils.cpp
        tests/test_integration.cpp
    )
    
    target_link_libraries(mcp_tests mcp_sdk)
    
    add_test(NAME MCPTests COMMAND mcp_tests)
endif()

# Install targets
install(TARGETS mcp_sdk mcp_server_example mcp_client_example 
               mcp_echo_server mcp_echo_client mcp_tools_demo 
               mcp_resources_demo mcp_prompts_demo
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY include/ DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install CMake config
install(FILES
    cmake/MCPSDKConfig.cmake
    cmake/MCPSDKConfigVersion.cmake
    DESTINATION lib/cmake/MCPSDK
)

# CPack configuration
set(CPACK_PACKAGE_NAME "MCP-SDK")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Model Context Protocol C++ SDK")
set(CPACK_PACKAGE_VENDOR "TinyMCP Team")
set(CPACK_PACKAGE_CONTACT "contact@tinymcp.org")

include(CPack)
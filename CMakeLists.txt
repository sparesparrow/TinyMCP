cmake_minimum_required(VERSION 3.15)

project(TinyMCP
    VERSION 0.1.0
    DESCRIPTION "TinyMCP - Lightweight C++ SDK for MCP Server"
    LANGUAGES CXX)

option(TINYMCP_BUILD_SHARED "Build TinyMCP as a shared library" OFF)
option(TINYMCP_BUILD_EXAMPLES "Build example server/client" OFF)
option(TINYMCP_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(TINYMCP_BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources: SDK core
file(GLOB_RECURSE TINYMCP_PROTOCOL_SRC
    ${CMAKE_SOURCE_DIR}/Source/Protocol/*/*.cpp)
file(GLOB_RECURSE TINYMCP_JSONCPP_SRC
    ${CMAKE_SOURCE_DIR}/Source/External/jsoncpp/src/lib_json/*.cpp)

add_library(tinymcp
    ${TINYMCP_PROTOCOL_SRC}
    ${TINYMCP_JSONCPP_SRC}
)

if(TINYMCP_BUILD_SHARED)
    set_target_properties(tinymcp PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(tinymcp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Protocol>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/External/jsoncpp/include>
        $<INSTALL_INTERFACE:include/tinymcp>
        $<INSTALL_INTERFACE:include>
)

if(TINYMCP_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(tinymcp PRIVATE /W3)
    else()
        target_compile_options(tinymcp PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Examples (optional)
if(TINYMCP_BUILD_EXAMPLES)
    add_subdirectory(Example/MCPServer/Build/Linux EXCLUDE_FROM_ALL)
    add_subdirectory(Example/MCPClient/Build/Linux EXCLUDE_FROM_ALL)
endif()

include(GNUInstallDirs)

install(TARGETS tinymcp
    EXPORT tinymcp-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Public headers: ship Protocol and jsoncpp headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Source/Protocol/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tinymcp
        FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Source/External/jsoncpp/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT tinymcp-targets
    NAMESPACE tinymcp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinymcp)

if(TINYMCP_BUILD_TESTS)
    add_subdirectory(tests)
endif()


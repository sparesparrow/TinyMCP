cmake_minimum_required(VERSION 3.10)

project(MCPServer VERSION 1.0.0.1001)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Optional: codegen for MCP schema (2025-06-18) using FlatBuffers or gRPC.
# Set -DMCP_CODEGEN=ON and provide MCP_SCHEMA_DIR if you want to generate headers at build time.
option(MCP_CODEGEN "Enable MCP schema code generation" OFF)
set(MCP_SCHEMA_DIR "${CMAKE_SOURCE_DIR}/schemas" CACHE PATH "Directory with MCP schemas")

if(MCP_CODEGEN)
    find_package(Threads REQUIRED)
    find_program(FLATC_EXECUTABLE NAMES flatc)
    if(FLATC_EXECUTABLE)
        file(GLOB_RECURSE MCP_FBS "${MCP_SCHEMA_DIR}/*.fbs")
        if(MCP_FBS)
            message(STATUS "Generating FlatBuffers headers for MCP schema")
            foreach(f IN LISTS MCP_FBS)
                get_filename_component(fname ${f} NAME)
                add_custom_command(
                    OUTPUT ${CMAKE_BINARY_DIR}/generated/flatbuffers/${fname}
                    COMMAND ${FLATC_EXECUTABLE} --cpp -o ${CMAKE_BINARY_DIR}/generated/flatbuffers ${f}
                    DEPENDS ${f}
                    COMMENT "flatc ${fname}"
                )
            endforeach()
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated/flatbuffers)
            add_custom_target(mcp_flatbuffers ALL DEPENDS ${CMAKE_BINARY_DIR}/generated/flatbuffers)
            list(APPEND ALL_SRC_FILES ${CMAKE_BINARY_DIR}/generated/flatbuffers)
            target_include_directories(MCPServer PRIVATE ${CMAKE_BINARY_DIR}/generated/flatbuffers)
        endif()
    endif()

    find_program(PROTOC_EXECUTABLE NAMES protoc)
    if(PROTOC_EXECUTABLE)
        file(GLOB_RECURSE MCP_PROTO "${MCP_SCHEMA_DIR}/*.proto")
        if(MCP_PROTO)
            message(STATUS "Generating gRPC/Protobuf headers for MCP schema")
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated/proto)
            foreach(p IN LISTS MCP_PROTO)
                get_filename_component(pname ${p} NAME_WE)
                add_custom_command(
                    OUTPUT ${CMAKE_BINARY_DIR}/generated/proto/${pname}.pb.cc ${CMAKE_BINARY_DIR}/generated/proto/${pname}.pb.h
                    COMMAND ${PROTOC_EXECUTABLE} --cpp_out=${CMAKE_BINARY_DIR}/generated/proto -I ${MCP_SCHEMA_DIR} ${p}
                    DEPENDS ${p}
                    COMMENT "protoc ${pname}.proto"
                )
                list(APPEND ALL_SRC_FILES ${CMAKE_BINARY_DIR}/generated/proto/${pname}.pb.cc)
            endforeach()
            target_include_directories(MCPServer PRIVATE ${CMAKE_BINARY_DIR}/generated/proto)
        endif()
    endif()
endif()

set(ALL_SRC_FILES "")
file(GLOB_RECURSE EXAMPLE_SRC_FILES "../../Source/*.cpp")
list(APPEND ALL_SRC_FILES ${EXAMPLE_SRC_FILES})
file(GLOB_RECURSE EXTERNAL_SRC_FILES "../../../../Source/External/jsoncpp/src/lib_json/*.cpp")
list(APPEND ALL_SRC_FILES ${EXTERNAL_SRC_FILES})
file(GLOB_RECURSE PROTOCOL_SRC_FILES "../../../../Source/Protocol/*/*.cpp")
list(APPEND ALL_SRC_FILES ${PROTOCOL_SRC_FILES})

add_executable(MCPServer ${ALL_SRC_FILES})

target_include_directories(MCPServer PRIVATE
    ../../Source
    ../../../../Source/Protocol
    ../../../../Source/External/jsoncpp/include
)

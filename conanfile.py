from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.files import copy
import os


class TinyMCPConan(ConanFile):
    name = "tinymcp"
    version = "0.1.0"
    license = "MIT"
    author = "TinyMCP Contributors"
    url = "https://github.com/Qihoo360/TinyMCP"
    description = "TinyMCP - Lightweight C++ SDK for MCP Server"
    topics = ("mcp", "sdk", "json-rpc", "llm")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = (
        "CMakeLists.txt",
        "Source/*",
        "Example/*",
        "Readme.md",
        "LICENSE",
    )

    def layout(self):
        # Rely on CMake helper defaults (build/ and generators/)
        pass

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["TINYMCP_BUILD_SHARED"] = "ON" if self.options.shared else "OFF"
        tc.variables["TINYMCP_BUILD_EXAMPLES"] = "OFF"  # keep package lean
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        # CMake install for library and headers
        cmake = CMake(self)
        cmake.install()

        # Ensure license present
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.set_property("cmake_target_name", "tinymcp::tinymcp")
        self.cpp_info.libs = ["tinymcp"]


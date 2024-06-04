from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("libxmlpp/5.2.0")
        self.requires("zlib/1.2.11")

    # def build_requirements(self):
    #     self.tool_requires("cmake/3.22.6")

    def layout(self):
        cmake_layout(self)

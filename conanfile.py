# This file is managed by Conan, contents will be overwritten.
# To keep your changes, remove these comment lines, but the plugin won't be able to modify your requirements

from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain

class ConanApplication(ConanFile):
    name = "iridium"
    version = "0.1"
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def requirements(self):
        self.requires("rclone_cpp/[>=0.1]")
        self.requires("boost/[>=1.80.0]")
        self.requires("libcurl/[>=8.0.0]")
        self.requires("libzip/[>=1.10.0]")


    def build_requirements(self):
        self.build_requires("cmake/[>=3.25]")

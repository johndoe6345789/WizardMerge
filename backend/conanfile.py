"""Conan package configuration for WizardMerge backend."""
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class WizardMergeConan(ConanFile):
    """WizardMerge C++ backend package."""

    name = "wizardmerge"
    version = "0.1.0"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    # Sources
    exports_sources = "CMakeLists.txt", "src/*", "include/*"
    
    # Dependencies
    requires = []
    
    generators = "CMakeDeps", "CMakeToolchain"
    
    def config_options(self):
        """Configure platform-specific options."""
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def layout(self):
        """Define project layout."""
        cmake_layout(self)
    
    def build(self):
        """Build the project using CMake."""
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        """Package the built artifacts."""
        cmake = CMake(self)
        cmake.install()
    
    def package_info(self):
        """Define package information for consumers."""
        self.cpp_info.libs = ["wizardmerge"]

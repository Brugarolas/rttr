from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import collect_libs

class RTTRConan(ConanFile):
    name = "rttr"
    version = "0.9.6"
    user = "es"
    settings = "os", "compiler", "arch" ,"build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_rtti": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_rtti": False,
    }
    
    generators = "CMakeDeps"

    exports_sources = ["*"]

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_DOCUMENTATION"] = False
        tc.variables["BUILD_UNIT_TESTS"] = False
        tc.variables["BUILD_EXAMPLES"] = False
        tc.variables["BUILD_PACKAGE"] = False
        tc.variables["BUILD_STATIC"] = True
        tc.variables["BUILD_RTTR_DYNAMIC"] = False
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        cmake_target = "Core" if self.options.shared else "Core_Lib"
        self.cpp_info.set_property("cmake_file_name", "rttr")
        self.cpp_info.set_property("cmake_target_name", f"RTTR::{cmake_target}")
        # TODO: back to global scope in conan v2 once cmake_find_package* generators removed
        self.cpp_info.components["_rttr"].libs = collect_libs(self)
        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.components["_rttr"].system_libs = ["dl", "pthread"]
        if self.options.shared:
            self.cpp_info.components["_rttr"].defines = ["RTTR_DLL"]

        # TODO: to remove in conan v2 once cmake_find_package* generators removed
        self.cpp_info.filenames["cmake_find_package"] = "rttr"
        self.cpp_info.filenames["cmake_find_package_multi"] = "rttr"
        self.cpp_info.names["cmake_find_package"] = "RTTR"
        self.cpp_info.names["cmake_find_package_multi"] = "RTTR"
        self.cpp_info.components["_rttr"].names["cmake_find_package"] = cmake_target
        self.cpp_info.components["_rttr"].names["cmake_find_package_multi"] = cmake_target
        self.cpp_info.components["_rttr"].set_property("cmake_target_name", f"RTTR::{cmake_target}")
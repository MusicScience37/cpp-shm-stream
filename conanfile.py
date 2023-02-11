from conans import ConanFile
from conans.tools import load
import os
import re


class LibraryConan(ConanFile):
    name = "cpp-shm-stream"
    description = "A C++ library of streams on shared memory."
    homepage = "https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-shm-stream"
    url = "https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-shm-stream.git"
    license = "Apache-2.0"
    author = "Kenta Kabashima (kenta_program37@hotmail.co.jp)"
    topics = ()
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "requirements_for_tests": [True, False],
    }
    default_options = {
        "requirements_for_tests": False,
    }
    exports_sources = "include/*"
    no_copy_source = True
    generators = "cmake", "cmake_find_package"

    def set_version(self):
        contents = load(
            os.path.join(
                self.recipe_folder,
                "include/shm_stream/version.h",
            )
        )
        major_version = re.search(r"VERSION_MAJOR (\d+)", contents).group(1)
        minor_version = re.search(r"VERSION_MINOR (\d+)", contents).group(1)
        patch_version = re.search(r"VERSION_PATCH (\d+)", contents).group(1)
        self.version = f"{major_version}.{minor_version}.{patch_version}"

    def requirements(self):
        pass
        # self.requires("fmt/9.0.0")

    def build_requirements(self):
        if self.options.requirements_for_tests:
            self.build_requires("catch2/3.1.0")

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()

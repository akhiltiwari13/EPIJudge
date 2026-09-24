import os

from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.scm import Version

from conan import ConanFile


class EpiJudge(ConanFile):
    name = "epijudge"
    version = "0.1"
    author = "akhiltiwari13"

    # Package metadata
    description = "Elements of programming interview cpp problems."
    topics = ("epi", "algorithms", "conan", "cmake", "cpp17")
    license = "MIT"

    # Binary configuration
    settings = "os", "arch", "compiler", "build_type"

    # Minimum compiler versions for the C++17 baseline this project builds at.
    _min_compiler = {
        "gcc": "9",
        "clang": "10",
        "apple-clang": "12",
        "msvc": "192",
    }

    def requirements(self):
        self.requires("benchmark/[>=1.9.5]")
        self.requires("gtest/[>=1.18.0 <2.0]")
        # 0.14.0 is NOT on ConanCenter -- it comes from the local recipe in
        # conan/recipes/tracy, exported by build.sh. The version is pinned
        # rather than ranged because it must match the Tracy SERVER installed
        # on this machine: client and server negotiate a wire ProtocolVersion
        # and Tracy hard-rejects a mismatch. 0.14.0 == protocol 82.
        #   server side: grep ProtocolVersion /usr/include/Tracy/common/TracyProtocol.hpp
        self.requires("tracy/0.14.0")

        # -- Intentionally NOT required ---------------------------------------
        # boost and yaml-cpp were declared here but no source file in this
        # directory includes them. There are no ConanCenter binaries for this
        # toolchain (it ships gcc 13 / gnu17 only), so each unused dependency
        # was a from-source build for nothing. To bring one back, add the
        # requires() line here plus the matching find_package() and
        # target_link_libraries() in CMakeLists.txt.

    def validate(self):
        check_min_cppstd(self, 17)
        compiler = str(self.settings.compiler)
        minimum = self._min_compiler.get(compiler)
        if minimum and Version(self.settings.compiler.version) < minimum:
            raise ConanInvalidConfiguration(
                f"{self.ref} needs C++17; {compiler} "
                f"{self.settings.compiler.version} is below the required "
                f"{compiler} >= {minimum}."
            )

    def _variant_tag(self, with_build_type=True):
        """Unique name for this toolchain+variant, used for both the build
        folder and the CMake preset prefix.

        Sanitizer profiles share compiler and build_type with the plain debug
        profile, so without the `user.epi:variant` discriminator gcc-asan and
        gcc-debug would collide on one build tree and one preset name.
        """
        parts = [str(self.settings.compiler)]
        variant = self.conf.get("user.epi:variant", default="", check_type=str)
        if variant:
            parts.append(variant)
        if with_build_type:
            parts.append(str(self.settings.build_type).lower())
        return "-".join(parts)

    def layout(self):
        # Four toolchain combinations share this source tree, so the build
        # folder has to encode the compiler as well as the build type.
        #
        # Depth matters here, and is not arbitrary: GenericTestMain() resolves
        # config.json through GetDefaultTestDataDirPath() (test_utils.h), which
        # searches at most THREE parent directories for test_data/ -- and does
        # so before --test-data-dir is ever parsed. A build tree deeper than
        # build/<compiler>-<build_type>/ makes every binary abort with
        # "Can't find test data directory" no matter what flags it is given.
        cmake_layout(self, generator="Ninja")
        self.folders.build = os.path.join("build", self._variant_tag())
        self.folders.generators = os.path.join(
            "build", self._variant_tag(), "generators"
        )

    def generate(self):
        # Generate CMake dependencies
        deps = CMakeDeps(self)
        deps.generate()

        # Configure CMake toolchain with modern practices
        tc = CMakeToolchain(self)

        # Without this every profile would emit presets named conan-debug /
        # conan-release, and CMakeUserPresets.json includes all of them at
        # once -- CMake rejects the duplicate names. Prefixing by compiler
        # gives gcc-debug, gcc-release, clang-debug, clang-release.
        tc.presets_prefix = self._variant_tag(with_build_type=False)

        # C++17. This must agree with compiler.cppstd in conan/profiles/* and
        # with set(CMAKE_CXX_STANDARD 17) in CMakeLists.txt -- all three pin the
        # standard, and a mismatch is silent (a plain set() in CMakeLists.txt
        # shadows the toolchain's value, which is how this project spent a long
        # time compiling at 17 while claiming 23).
        #
        # EXTENSIONS is ON because the profiles specify gnu17, not 17; the
        # resulting compile line is -std=gnu++17.
        tc.variables["CMAKE_CXX_STANDARD"] = "17"
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        tc.variables["CMAKE_CXX_EXTENSIONS"] = "ON"
        tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = "ON"

        tc.generate()

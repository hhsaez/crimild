import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import check_colocated_sources as checker


class ColocatedSourceTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.library = self.root / "libs" / "coding"
        self.source = self.library / "src" / "crimild" / "coding"
        self.source.mkdir(parents=True)
        (self.library / "test").mkdir(parents=True)
        (self.library / "src" / "CMakeLists.txt").write_text("add_subdirectory(crimild)\n")
        (self.library / "src" / "crimild" / "CMakeLists.txt").write_text("add_subdirectory(coding)\n")
        (self.source / "Codec.hpp").write_text("// public header\n")
        (self.source / "Codec.cpp").write_text("// implementation\n")
        (self.source / "Codec.test.cpp").write_text("// feature test\n")
        (self.library / "test" / "TestRunner.cpp").write_text("// test infrastructure\n")
        self.source_cmake = self.source / "CMakeLists.txt"
        self.source_cmake.write_text(
            "target_sources(crimild_coding PUBLIC Codec.hpp PRIVATE Codec.cpp)\n"
            "target_sources(crimild_coding_test PRIVATE Codec.test.cpp)\n"
        )
        self.test_cmake = self.library / "test" / "CMakeLists.txt"
        self.test_cmake.write_text("target_sources(crimild_coding_test PRIVATE TestRunner.cpp)\n")

    def check(self):
        return checker.check_library(self.library, "coding")

    def test_accepts_colocated_feature_tests_and_test_runner(self):
        self.assertEqual(self.check(), [])

    def test_requires_cmake_in_each_source_directory(self):
        (self.library / "src" / "crimild" / "CMakeLists.txt").unlink()
        self.assertTrue(any("missing CMakeLists" in e for e in self.check()))

    def test_rejects_unregistered_source(self):
        (self.source / "Missing.cpp").write_text("// unregistered\n")
        self.assertTrue(any("Missing.cpp: expected exactly one" in e for e in self.check()))

    def test_rejects_test_in_production_target(self):
        self.source_cmake.write_text(self.source_cmake.read_text().replace(
            "crimild_coding_test PRIVATE Codec.test.cpp",
            "crimild_coding PRIVATE Codec.test.cpp",
        ))
        self.assertTrue(any("must belong to crimild_coding_test" in e for e in self.check()))

    def test_rejects_test_in_test_directory(self):
        (self.source / "Codec.test.cpp").rename(self.library / "test" / "Codec.test.cpp")
        self.assertTrue(any("feature tests belong next to their sources" in e for e in self.check()))

    def test_rejects_nonlocal_source_entry(self):
        self.source_cmake.write_text(self.source_cmake.read_text() +
            "target_sources(crimild_coding PRIVATE ../other.cpp)\n")
        self.assertTrue(any("source entry must be local" in e for e in self.check()))

    def test_required_library_without_layout_fails(self):
        with tempfile.TemporaryDirectory() as temporary:
            library = Path(temporary) / "foundation"
            errors = checker.check_library(library, "foundation", require_layout=True)
            self.assertTrue(any("required colocated source layout" in e for e in errors))

    def test_math_layout_is_required(self):
        with tempfile.TemporaryDirectory() as temporary:
            library = Path(temporary) / "math"
            errors = checker.check_library(library, "math", require_layout=True)
            self.assertTrue(any("required colocated source layout" in e for e in errors))

    def test_core_layout_is_required(self):
        with tempfile.TemporaryDirectory() as temporary:
            library = Path(temporary) / "core"
            errors = checker.check_library(library, "core", require_layout=True)
            self.assertTrue(any("required colocated source layout" in e for e in errors))

    def test_unregistered_shader_asset_is_rejected(self):
        (self.source / "LitShaderProgram.frag").write_text("// shader")
        self.assertTrue(any("LitShaderProgram.frag: expected exactly one" in e for e in self.check()))

    def test_known_preexisting_unregistered_core_tests_are_exempt(self):
        core = self.root / "libs" / "core"
        location = core / "src" / "crimild" / "core" / "Assemblies"
        location.mkdir(parents=True)
        (core / "test").mkdir()
        (core / "src" / "CMakeLists.txt").write_text("add_subdirectory(crimild)\n")
        (core / "src" / "crimild" / "CMakeLists.txt").write_text("add_subdirectory(core)\n")
        (core / "src" / "crimild" / "core" / "CMakeLists.txt").write_text("add_subdirectory(Assemblies)\n")
        (core / "test" / "CMakeLists.txt").write_text("")
        (location / "CMakeLists.txt").write_text("")
        (location / "AssemblyDAG.test.cpp").write_text("// preexisting unregistered test\n")
        self.assertEqual(checker.check_library(core, "core", require_layout=True), [])
        (location / "NewUnregistered.test.cpp").write_text("// new unregistered test\n")
        errors = checker.check_library(core, "core", require_layout=True)
        self.assertTrue(any("NewUnregistered.test.cpp: expected exactly one" in e for e in errors))

    def test_unmigrated_library_is_skipped(self):
        with tempfile.TemporaryDirectory() as temporary:
            self.assertEqual(checker.check_library(Path(temporary) / "coding", "coding"), [])


if __name__ == "__main__":
    unittest.main()

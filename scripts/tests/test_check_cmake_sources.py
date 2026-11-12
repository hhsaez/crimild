import importlib.util
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


spec = importlib.util.spec_from_file_location(
    "check_cmake_sources", Path(__file__).resolve().parents[1] / "check_cmake_sources.py"
)
checker = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = checker
spec.loader.exec_module(checker)


class CMakeSourceStyleTest(unittest.TestCase):
    def test_accepts_explicit_mixed_scopes_and_expressions(self):
        self.assertEqual(checker.check('''
            target_sources(example
                PRIVATE first.cpp
                PUBLIC "public header.hpp"
                INTERFACE ${GENERATED_SOURCES}
                PRIVATE "$<$<CONFIG:Debug>:debug.cpp>"
            )
        '''), [])

    def test_rejects_grouped_sources_with_location(self):
        errors = checker.check("target_sources(example PRIVATE first.cpp\nsecond.cpp)")
        self.assertEqual(len(errors), 1)
        self.assertEqual(errors[0][0], 2)
        self.assertIn("second.cpp", errors[0][1])

    def test_rejects_scope_on_separate_line(self):
        self.assertTrue(checker.check("target_sources(example PRIVATE\nfirst.cpp)"))

    def test_rejects_missing_or_unused_scope(self):
        for source in ("first.cpp", "PRIVATE PUBLIC first.cpp", "PRIVATE"):
            with self.subTest(source=source):
                self.assertTrue(checker.check(f"target_sources(example {source})"))

    def test_checks_each_command_case_insensitively(self):
        errors = checker.check('''
            target_sources(first PRIVATE first.cpp)
            TARGET_SOURCES(second PRIVATE second.cpp third.cpp)
        ''')
        self.assertEqual(len(errors), 1)
        self.assertIn("third.cpp", errors[0][1])

    def test_ignores_comments_and_command_text_in_strings(self):
        self.assertEqual(checker.check('''
            # target_sources(example missing.cpp)
            #[=[ target_sources(example missing.cpp) ]=]
            set(example "target_sources(example missing.cpp)")
            set(example [==[target_sources(example missing.cpp)]==])
            target_sources(example PRIVATE "a#b.cpp" # source comment
                PRIVATE "escaped\\\"quote.cpp")
        '''), [])

    def test_accepts_bracket_and_escaped_arguments(self):
        self.assertEqual(checker.check(r'''
            target_sources(example PRIVATE [=[a b.cpp]=] PRIVATE a\ b.cpp)
        '''), [])

    def test_leaves_file_set_grammar_alone(self):
        self.assertEqual(checker.check('''
            target_sources(example PUBLIC FILE_SET HEADERS
                BASE_DIRS include FILES include/first.hpp include/second.hpp)
        '''), [])

    def test_does_not_apply_to_other_target_commands(self):
        self.assertEqual(checker.check('''
            target_link_libraries(example PRIVATE first second)
            target_include_directories(example PRIVATE first second)
        '''), [])

    def test_reports_unclosed_commands(self):
        with self.assertRaises(ValueError):
            checker.check("target_sources(example PRIVATE first.cpp")

    def test_cli_exit_status_and_repository_file_selection(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "scripts").mkdir()
            script = root / "scripts" / "check_cmake_sources.py"
            script.write_text(Path(checker.__file__).read_text())
            cmake = root / "CMakeLists.txt"
            cmake.write_text("target_sources(example PRIVATE first.cpp second.cpp)")
            (root / "third-party").mkdir()
            (root / "third-party" / "CMakeLists.txt").write_text(
                "target_sources(vendor missing.cpp)"
            )
            subprocess.run(["git", "init", "-q", str(root)], check=True)
            subprocess.run(["git", "add", "."], cwd=root, check=True)
            # Generated, untracked build files must not be checked.
            (root / "build").mkdir()
            (root / "build" / "CMakeLists.txt").write_text("invalid(")
            failed = subprocess.run([sys.executable, str(script)], capture_output=True, text=True)
            self.assertEqual(failed.returncode, 1)
            self.assertIn("CMakeLists.txt:1:", failed.stderr)
            self.assertIn("second.cpp", failed.stderr)
            self.assertNotIn("third-party", failed.stderr)
            cmake.write_text("target_sources(example PRIVATE first.cpp PRIVATE second.cpp)")
            passed = subprocess.run([sys.executable, str(script)], capture_output=True, text=True)
            self.assertEqual(passed.returncode, 0, passed.stderr)
            self.assertIn("1 first-party files", passed.stdout)


if __name__ == "__main__":
    unittest.main()

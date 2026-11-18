#!/usr/bin/env python3
"""Check file ownership and target separation in migrated first-party libraries.

Only libraries in COLOCATED_LIBRARIES are enforced. An absent layout is skipped
so this checker can land before an incremental migration is merged.
"""
from pathlib import Path

from check_cmake_sources import source_calls

COLOCATED_LIBRARIES = ("libs/coding", "libs/foundation")
SOURCE_EXTENSIONS = {".hpp", ".h", ".cpp", ".cc", ".cxx"}
SOURCE_TARGETS = {\n    "coding": ("crimild_coding", "crimild_coding_test"),\n    "foundation": ("crimild_foundation", "crimild_foundation_test"),\n}


def check_library(library: Path, name: str, *, require_layout=False):
    errors = []
    src = library / "src"
    migrated_root = src / "crimild" / name
    if not migrated_root.is_dir():
        return errors

    library_target, test_target = SOURCE_TARGETS[name]
    declared = {}

    for directory in [src, *sorted(p for p in src.rglob("*") if p.is_dir()), library / "test"]:
        cmake = directory / "CMakeLists.txt"
        if not cmake.is_file():
            errors.append(f"{directory}: missing CMakeLists.txt")
            continue
        for args in source_calls(cmake.read_text(encoding="utf-8")):
            if not args or args[0].value not in (library_target, test_target):
                continue
            if any(a.value == "FILE_SET" for a in args):
                continue
            arguments = args[1:]
            for scope, entry in zip(arguments[::2], arguments[1::2]):
                if scope.value not in ("PRIVATE", "PUBLIC", "INTERFACE"):
                    continue  # The existing source style checker reports this.
                path = entry.value.strip('"')
                if path.startswith("$") or "/" in path or "\\\\" in path:
                    errors.append(f"{cmake}:{entry.line}: source entry must be local: {path}")
                    continue
                source = directory / path
                if not source.is_file():
                    errors.append(f"{cmake}:{entry.line}: source does not exist: {path}")
                    continue
                if source.suffix not in SOURCE_EXTENSIONS:
                    continue
                expected_target = test_target if path.endswith(".test.cpp") or directory == library / "test" else library_target
                if args[0].value != expected_target:
                    errors.append(f"{cmake}:{entry.line}: {path} must belong to {expected_target}")
                if path.endswith(".test.cpp") and scope.value != "PRIVATE":
                    errors.append(f"{cmake}:{entry.line}: feature test must be PRIVATE: {path}")
                declared[source] = declared.get(source, 0) + 1

    for directory in [src, *sorted(p for p in src.rglob("*") if p.is_dir()), library / "test"]:
        for source in directory.iterdir():
            if not source.is_file() or source.suffix not in SOURCE_EXTENSIONS:
                continue
            if source.name == "TestRunner.cpp" and directory != library / "test":
                errors.append(f"{source}: test infrastructure belongs in test/")
            if source.name.endswith(".test.cpp") and not source.is_relative_to(src):
                errors.append(f"{source}: feature tests belong next to their sources")
            if declared.get(source, 0) != 1:
                errors.append(f"{source}: expected exactly one local target_sources() entry, found {declared.get(source, 0)}")

    return errors


def check_repo(root: Path):
    errors = []
    for relative in COLOCATED_LIBRARIES:
        library = root / relative
        errors.extend(check_library(library, Path(relative).name, require_layout=True))
    return errors


def main():
    root = Path(__file__).resolve().parents[1]
    errors = check_repo(root)
    if errors:
        for error in errors:
            print(error)
        return 1
    print("Colocated library source ownership passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

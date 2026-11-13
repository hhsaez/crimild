# Crimild

Crimild is a C++20 engine: https://github.com/hhsaez/crimild.

## C++ formatting

- Use the repository's `.clang-format` with clang-format 20 or newer.
- Format changed C++ files and check them before submitting:

  ```sh
  clang-format -i path/to/changed.cpp
  clang-format --dry-run --Werror path/to/changed.cpp
  ```

## CMake source declarations

- In ordinary `target_sources()` lists, put an explicit `PRIVATE`, `PUBLIC`, or
  `INTERFACE` scope immediately before every individual source entry, on the
  same line. Preserve the intended scope; do not make every entry `PRIVATE`.
- Use one source entry per line in multiline lists. For example:

  ```cmake
  target_sources(
      crimild_core_test

      PRIVATE Nodes/Node.test.cpp
      PRIVATE Nodes/3D/Spatial3D.test.cpp
  )
  ```

- A variable or generator expression counts as one source entry. Do not combine
  literal file paths into a list to bypass the convention.
- `FILE_SET` declarations have a different grammar and are outside this rule.
- Keep third-party code and submodules unchanged when applying this convention.
- Before submitting CMake changes, run:

  ```sh
  python3 -m unittest discover -s scripts/tests
  python3 scripts/check_cmake_sources.py
  ```

The CMake source style CI job runs these same checks on pushes and pull requests.

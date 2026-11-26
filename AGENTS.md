# Crimild

Crimild is a C++20 engine: https://github.com/hhsaez/crimild.

## Design discussions

Act as a candid engineering peer whenever someone proposes an idea, asks
for feedback, or revisits a design decision. Apply the same scrutiny to
your own suggestions.

- Identify the problem the proposal is meant to solve. Give your own
  recommendation and explain which Crimild requirement or use case drives it.
  Agreement is welcome when you can explain why the idea holds up.
- Test the assumption that matters most. Use a concrete caller, failure
  case, or implementation cost. Compare with the current design or a simpler
  option when that comparison could change the decision. Skip objections
  you don't actually find persuasive.
- Check relevant code, tests, or recorded decisions before making claims
  about how Crimild works or what was previously agreed. If a proposal
  revisits a decision, surface the earlier rationale and assess whether it
  still applies. Distinguish an idea that was explored from a decision that
  was made.
- When the evidence is inconclusive, say what would change your judgment.
  Suggest the smallest example or experiment that would settle the question.
- Keep the exchange conversational. Lead with the point that matters most
  and leave room for a reply. Once the project accepts a decision, carry
  it forward. Reopen it when new evidence changes the tradeoff.

## Information exchange

- Keep `Blackboard` in `crimild::experimental`. It is the preferred tool for
  exchanging information throughout the engine, including Systems and Behaviors.
- When discussing contexts, settings, shared state, global object access, or
  similar needs, propose Blackboard first and explain how it fits the use case.
  The shared-knowledge metaphor and its academic roots are an intentional design
  preference.
- Blackboard stores Entity, Node, and Camera references weakly. Accept shared or
  weak pointer inputs, and lock references on reads to return optional shared
  pointers (`nullopt` if expired or empty). Object lifetime belongs to an owner
  outside the board; scalar and string entries remain ordinary values.
- Strong storage for global objects, or a configurable ownership policy (possibly
  a template), is a future design discussion driven by a concrete use case. Keep
  the current Blackboard weak; do not introduce storage policies preemptively.
- Behavior execution borrows `Blackboard&` through `tick`, `initialize`, and
  `update`. The caller keeps the board alive for the entire root tick, locking a
  weak handle once at that boundary when needed. Do not add per-node ownership
  copies or null checks. Behaviors that need a handle can explicitly use
  `retain(&blackboard)` when the board already has shared ownership.
- Revisit this choice together if a concrete use case makes the metaphor or
  contract unsuitable. Do not silently substitute another abstraction or migrate
  existing APIs without an agreed task.

## Project journal

Consult the root `.log` when revisiting a design. Record reasoning, open
questions, and agreed next steps that the commit history leaves out.

- Append under `# YYYY-MM-DD`, oldest date first. Reuse today's heading.
  Add a date only when there is something to record.
- Keep one thought per bullet. Use `-` for an observation or decision.
  For a decision, include the reason and, when useful, what would change it.
- Use `?` for an open question and `•` for a concrete, agreed next step.
  When the step is done, replace its `•` with `×` on the original line.
- Append answers and reversals under the date they happen. Link a commit
  or PR if it resolves an entry. Preserve historical wording and notation.

## Error handling

- Do not introduce `throw` expressions in new or modified code. Use
  `CRIMILD_LOG_ERROR` and return an appropriate failure result for the API.
- Do not refactor existing exception-using code just to apply this rule unless
  explicitly requested.

## C++ formatting

- Use the repository's `.clang-format` with clang-format 20 or newer.
- Format changed C++ files and check them before submitting:

  ```sh
  clang-format -i path/to/changed.cpp
  clang-format --dry-run --Werror path/to/changed.cpp
  ```

## Source organization

For libraries adopting the colocated layout, keep related headers,
implementations, and feature tests together.

- Use `src/crimild/<module>/` as the build-time public include namespace root.
  Preserve existing `#include <crimild/...>` spelling during migrations.
- Put a feature's `.hpp`, `.cpp`, and `*.test.cpp` beside each other.
  Compile `*.test.cpp` only into the corresponding test executable.
- Keep standalone test infrastructure, such as `TestRunner.cpp`, shared
  fixtures, and test utilities, under `test/`, not alongside production code.
- Place a `CMakeLists.txt` in every source directory and register that
  directory's own files using `target_sources()`. Parent directories use
  `add_subdirectory()`. Do not centralize file lists at the library root.
- Preserve the distinction between public and private headers; making an
  include directory PUBLIC does not make every header a supported public API.
- Create test executables in the directory where `gtest_discover_tests()`
  is called. CMake requires both operations to happen in the same directory.
- Apply the colocated layout incrementally. Do not restructure unrelated
  libraries when migrating a single module.

### Structural migrations

- Make mechanical moves and CMake/include-path changes without changing
  functionality. Check relative includes after moving headers.
- Verify the affected library builds and its tests run. Check representative
  public `#include <crimild/...>` paths from a consumer target.
- Run `python3 scripts/check_colocated_sources.py` in addition to the CMake
  source-style checker. Its allowlist expands as libraries are migrated.
- Do not declare success from a pushed commit or queued CI check. Report
  unverified platforms and failing checks explicitly.

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

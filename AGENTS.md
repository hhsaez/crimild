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

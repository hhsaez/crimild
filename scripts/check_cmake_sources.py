#!/usr/bin/env python3
"""Check explicit scope per entry in first-party target_sources() lists.

Uses lexical arguments, without evaluating CMake variables or executing project
code. FILE_SET declarations are excluded because they use a different grammar.
"""

import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


SCOPES = {"PRIVATE", "PUBLIC", "INTERFACE"}
TOKEN = re.compile(
    r"(?P<space>\s+)"
    r"|(?P<bracket_comment>\#\[(?P<ceq>=*)\[.*?\](?P=ceq)\])"
    r"|(?P<comment>\#[^\n]*)"
    r"|(?P<bracket>\[(?P<beq>=*)\[.*?\](?P=beq)\])"
    r'|(?P<quoted>"(?:\\.|[^"\\])*")'
    r"|(?P<paren>[()])"
    r'|(?P<argument>(?:\\.|[^\s()"#\\])+)',
    re.DOTALL,
)


@dataclass
class Token:
    value: str
    start: int
    end: int
    line: int


def tokens(text):
    position = 0
    line = 1
    while position < len(text):
        match = TOKEN.match(text, position)
        if match is None:
            raise ValueError(f"line {line}: cannot parse CMake argument")
        if match.lastgroup not in {"space", "comment", "bracket_comment"}:
            yield Token(match.group(), position, match.end(), line)
        line += match.group().count("\n")
        position = match.end()


def source_calls(text):
    """Yield argument tokens for each actual target_sources command."""
    items = list(tokens(text))
    index = 0
    while index + 1 < len(items):
        if items[index + 1].value != "(":
            index += 1
            continue
        name = items[index].value.lower()
        start = index + 2
        index = start
        depth = 1
        while index < len(items) and depth:
            if items[index].value == "(":
                depth += 1
            elif items[index].value == ")":
                depth -= 1
            index += 1
        if depth:
            raise ValueError(f"line {items[start - 2].line}: unclosed CMake command")
        if name == "target_sources":
            yield items[start:index - 1]


def check(text):
    errors = []
    for args in source_calls(text):
        if any(arg.value == "FILE_SET" for arg in args):
            continue
        scope = None
        for arg in args[1:]:  # The first argument is the target name.
            if arg.value in SCOPES:
                if scope is not None:
                    errors.append((scope.line, "scope must be followed by one source entry"))
                scope = arg
                continue
            if scope is None or scope.line != arg.line:
                errors.append((arg.line, f"put an explicit scope on the same line before {arg.value}"))
            scope = None
        if scope is not None:
            errors.append((scope.line, "scope must be followed by one source entry"))
    return errors


def cmake_files(root):
    # Git supplies tracked paths, excluding build trees and submodule contents.
    paths = subprocess.check_output(
        ["git", "ls-files", "-z"], cwd=root
    ).decode().split("\0")
    return [
        root / path for path in paths
        if path and Path(path).parts[0] != "third-party"
        and (Path(path).name == "CMakeLists.txt" or path.endswith(".cmake"))
    ]


def main():
    root = Path(__file__).resolve().parents[1]
    files = cmake_files(root)
    failed = False
    for path in files:
        try:
            errors = check(path.read_text(encoding="utf-8"))
        except ValueError as error:
            print(f"{path.relative_to(root)}: {error}", file=sys.stderr)
            failed = True
            continue
        for line, message in errors:
            print(f"{path.relative_to(root)}:{line}: {message}", file=sys.stderr)
            failed = True
    if failed:
        return 1
    print(f"CMake source style passed ({len(files)} first-party files).")
    return 0


if __name__ == "__main__":
    sys.exit(main())

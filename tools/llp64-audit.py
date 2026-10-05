#!/usr/bin/env python3
"""LLP64 audit of the C shims — no bare `long` in an exported signature.

Why this is a checker of its own and not a line in abi-audit.py:
abi-audit.py compares Scaly declarations against REAL C headers and can
therefore only check what is declared on THIS host. The LLP64 question is
a different one: it asks whether a signature changes its width on a target
we do not compile here at all.

The mechanism, in one sentence: the committed seed delivers ONE scaly.ll for
every target, so a Scaly extern declaration cannot be target-dependent
— it says `i64` or `size_t` once, for all of them. But C's `long` is
64 bits on LP64 (mac/linux) and 32 bits on LLP64 (Win64). An exported
shim function with `long` thus contradicts its own declaration on
exactly one target — and silently.

The RESULT is the dangerous half (the same class
tests/abi/run.sh was built for): a 32-bit result, read as i64, leaves
the upper 32 bits unspecified, that flips the SIGN, and every
`if r < 0` error check on it becomes a coin flip. Parameters are the
benign direction, but are reported too: `unsigned long count` against a
declared `size_t` is simply a different function on Win64.

ONLY exported (non-`static`) definitions are checked, because only those
are the ABI boundary. A `long` in a local intermediate value is fine
and sometimes even right (sysconf() returns `long`).

The fix is always on the C SIDE: `long long` for results, `size_t` for
sizes/counts — both 64 bits on every target we ship. The
Scaly declaration stays untouched, so there is no emission change
and no seed refresh.

Usage: tools/llp64-audit.py [--quiet] [file.c ...]
       without files: every .c under packages/
"""

import os
import re
import sys

# Neutralise `long long` first, otherwise \blong\b fires on it.
LONGLONG = re.compile(r"\blong\s+long\b")
BARE_LONG = re.compile(r"\blong\b")
BLOCK_COMMENT = re.compile(r"/\*.*?\*/", re.S)
LINE_COMMENT = re.compile(r"//[^\n]*")


def strip_comments(text):
    """Replace comments by empty lines so that the line numbers stay right."""
    text = BLOCK_COMMENT.sub(lambda m: "\n" * m.group(0).count("\n"), text)
    return LINE_COMMENT.sub("", text)


def has_bare_long(fragment):
    return bool(BARE_LONG.search(LONGLONG.sub("longlong", fragment)))


def find_definitions(lines):
    """Top-level function definitions in the shims' Allman style.

    A definition is a line without indentation that closes a parameter list
    and whose next non-empty line begins with '{'. That drops
    prototypes (they end in ';') and function-pointer variables.
    """
    for i, line in enumerate(lines):
        if not line or line[0].isspace():
            continue
        stripped = line.strip()
        if "(" not in stripped or not stripped.endswith(")"):
            continue
        nxt = next((l.strip() for l in lines[i + 1:] if l.strip()), "")
        if not nxt.startswith("{"):
            continue
        yield i + 1, stripped


def split_signature(sig):
    """(return type+name, parameter list) — at the FIRST opening parenthesis."""
    depth = 0
    for i, ch in enumerate(sig):
        if ch == "(":
            if depth == 0:
                return sig[:i], sig[i + 1:-1]
            depth += 1
        elif ch == ")":
            depth -= 1
    return sig, ""


def audit(path):
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        lines = strip_comments(fh.read()).split("\n")

    findings = []
    for lineno, sig in find_definitions(lines):
        head, params = split_signature(sig)
        if re.match(r"^\s*static\b", head):
            continue
        where = []
        if has_bare_long(head):
            where.append("RESULT")
        if has_bare_long(params):
            where.append("PARAMETER")
        if where:
            findings.append((path, lineno, "+".join(where), sig))
    return findings


def main():
    args = [a for a in sys.argv[1:] if a != "--quiet"]
    quiet = "--quiet" in sys.argv[1:]

    if args:
        files = args
    else:
        files = []
        for root, _, names in os.walk("packages"):
            files.extend(os.path.join(root, n) for n in sorted(names)
                         if n.endswith(".c"))
        files.sort()

    findings = []
    for path in files:
        findings.extend(audit(path))

    if not quiet:
        print(f"C shims checked: {len(files)}")
        for path in files:
            print(f"  {path}")
        print()

    for path, lineno, where, sig in findings:
        print(f"{path}:{lineno}: {where}: {sig}")

    print(f"LLP64 FINDINGS: {len(findings)}")
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main())

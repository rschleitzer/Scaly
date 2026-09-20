#!/usr/bin/env python3
"""Find C symbols this tree declares `extern` more than once with signatures
that differ at the ABI level.

Two declarations of one C symbol may spell a parameter differently and still
agree — `pointer[void]` and a single-ptr-field handle struct are both `ptr` in
the emitted IR. What matters is the LLVM-level shape, so every Scaly type is
normalised to `ptr` / `iN` / `void` before comparing. `poll` was the reason this
check exists: it was declared `(ptr, i32, i32) -> i32` in one package and
`(ptr, i64, i64) -> i64` in another, both linked into the same binary.
"""
import collections
import glob
import re
import sys

DECL = re.compile(
    r"^\s*(?:function|procedure)\s+([A-Za-z_][A-Za-z0-9_]*)\s*\((.*?)\)"
    r"(?:\s+returns\s+(.+?))?\s+extern\s*$"
)
HANDLE_DEF = re.compile(r"^define\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*handle\s*:\s*pointer\[void\]\s*\)")

WIDTH = {
    "int": "i64", "uint": "i64", "i64": "i64", "u64": "i64",
    "size_t": "i64", "size": "i64",
    "i32": "i32", "u32": "i32", "i16": "i16", "u16": "i16",
    "i8": "i8", "u8": "i8", "char": "i8", "bool": "i1",
    "double": "double", "f64": "double", "float": "float", "f32": "float",
    "void": "void",
}


def norm(t, handles):
    t = t.strip()
    if t.startswith("pointer[") or t.startswith("ref[") or t in handles:
        return "ptr"
    return WIDTH.get(t, t)


def split_params(sig):
    out, depth, cur = [], 0, ""
    for ch in sig:
        if ch == "[":
            depth += 1
        elif ch == "]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return [p.split(":", 1)[1].strip() if ":" in p else p.strip() for p in out]


def main():
    # Walked in Python, not through `find`: on the Windows box a subprocess
    # named `find` is DOS's find.exe from System32, which answers nothing and
    # made this check report 0 of 0 symbols (tests/win32/WINDOWS-BOX.md §4a).
    # The same set of files on every host; order does not reach the verdict.
    files = sorted(glob.glob("packages/**/*.scaly", recursive=True))
    handles = set()
    for f in files:
        for line in open(f):
            m = HANDLE_DEF.match(line)
            if m:
                handles.add(m.group(1))
    sigs = collections.defaultdict(list)
    for f in files:
        for ln, line in enumerate(open(f), 1):
            m = DECL.match(line.split(";")[0])
            if not m:
                continue
            name = m.group(1)
            ps = tuple(norm(p, handles) for p in split_params(m.group(2)))
            ret = norm(m.group(3) or "void", handles)
            sigs[name].append((ps, ret, f, ln))
    bad = 0
    for name, entries in sorted(sigs.items()):
        shapes = {(e[0], e[1]) for e in entries}
        if len(shapes) < 2:
            continue
        bad += 1
        print(f"### {name}")
        seen = set()
        for ps, ret, f, ln in entries:
            if (ps, ret) in seen:
                continue
            seen.add((ps, ret))
            print(f"    {f}:{ln}  ({', '.join(ps)}) -> {ret}")
    print(f"\nSymbole mit ABI-widersprüchlichen Deklarationen: {bad} von {len(sigs)}")
    dupes = sum(1 for n, e in sigs.items() if len(e) > 1)
    print(f"mehrfach deklarierte Symbole insgesamt: {dupes}")


if __name__ == "__main__":
    main()

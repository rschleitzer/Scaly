#!/usr/bin/env python3
"""Rewrite ONE buffer parameter and its adjacent length into a `Slice[T]`.

Usage: convert.py <file> <routine> <buffer> <length> <elem>

Scoped to the routine's own body, because a bare length name like `n` occurs in
every second function in these files and a file-wide substitution answers about
the wrong one -- `refstar.py` made exactly that mistake from the other side.

★★★It rewrites the DECLARATION and the BODY only. Call sites are left to the
compiler, which rejects a pointer where a `Slice` is declared; that is the
intended driver. ★And it does NOT check what the length COUNTS -- see the
scanner's note on `make_translate`, where the length counts PAIRS and the slice
is twice as long as the parameter says.
"""
import re, sys

def store_to_put(line, buf):
    """`set *(buf + <expr>): <value>` -> `buf.put(<expr>, <value>)`.

    ★★★It MUST run before deref_to_subscript#, which would otherwise turn the
    same statement into `set buf[i]: v` -- a hard rc-4 since
    report_subscript_assignment#, because `operator []` answers T BY VALUE and
    there is no slot behind a subscript. `put` is the only write spelling, and
    it is the reason a WRITTEN buffer became convertible at all (the scanner's
    W-* verdicts, 2026-08-30).

    ★The index is read with BALANCED parentheses for the reason the read side
    is: `set *(codes + (i as size_t)): v` is the shape whose first matcher
    missed it and left a live deref behind.
    """
    m = re.match(rf'(\s*)set\s+\*\(\s*{re.escape(buf)}\s*\+\s*', line)
    if not m: return line
    depth, end = 1, None
    for k in range(m.end(), len(line)):
        if line[k] == '(': depth += 1
        elif line[k] == ')':
            depth -= 1
            if depth == 0: end = k; break
    if end is None: return line
    index = line[m.end():end].strip()
    rest = line[end + 1:]
    cm = re.match(r'\s*:\s*(.*)$', rest)
    if not cm: return line
    return f'{m.group(1)}{buf}.put({index}, {cm.group(1).rstrip()})'


def bare_store_to_put(line, buf):
    """`set *buf: v` with no arithmetic is element ZERO."""
    m = re.match(rf'(\s*)set\s+\*{re.escape(buf)}(?![\w\[])\s*:\s*(.*)$', line)
    if not m: return line
    return f'{m.group(1)}{buf}.put(0, {m.group(2).rstrip()})'


def deref_to_subscript(line, buf):
    """`*(buf + <expr>)` -> `buf[<expr>]`, with a BALANCED index: the index may
    itself contain parentheses (`*(codes + (i as size_t))`)."""
    out = line
    while True:
        m = re.search(rf'\*\(\s*{re.escape(buf)}\s*\+\s*', out)
        if not m: return out
        depth, end = 1, None
        for k in range(m.end(), len(out)):
            if out[k] == '(': depth += 1
            elif out[k] == ')':
                depth -= 1
                if depth == 0: end = k; break
        if end is None: return out
        out = out[:m.start()] + f'{buf}[' + out[m.end():end].strip() + ']' + out[end + 1:]

def bare_deref_to_subscript(line, buf):
    """`*name` with no arithmetic is element ZERO -- `(*d) as u8` and
    `*d as int` are how these ports read the first byte."""
    return re.sub(rf'\*{re.escape(buf)}(?![\w\[])', f'{buf}[0]', line)

STR = re.compile(r'"(?:\\.|[^"\\])*"')

def code_only(line):
    """Comment and STRING LITERALS removed.

    ★Brace counting must not see a `{` inside a string: TeXFOTBuilder writes
    `b_raw(b, "\\Character{")`, whose brace never closes, so the routine's range
    ran to the end of the FILE and the length substitution rewrote three
    neighbouring parameter lists into `s.length: size_t`.
    """
    return STR.sub('""', line.split(';')[0])

def routine_range(lines, name):
    start = None
    for i, l in enumerate(lines):
        if re.match(rf'\s*(function|procedure)\s+{re.escape(name)}\s*\(', l):
            start = i; break
    if start is None: return None
    indent = len(lines[start]) - len(lines[start].lstrip())
    j = start + 1
    depth = 0; seen = False
    while j < len(lines):
        s = code_only(lines[j])
        depth += s.count('{') - s.count('}')
        if '{' in s: seen = True
        if seen and depth <= 0: return (start, j)
        if not seen and s.strip() and (len(s) - len(s.lstrip())) <= indent and j > start + 1:
            return (start, j - 1)
        j += 1
    return (start, len(lines) - 1)

def main(path, routine, buf, length, elem):
    lines = open(path).read().split('\n')
    rng = routine_range(lines, routine)
    if rng is None: sys.exit(f"routine not found: {routine}")
    a, b = rng
    # declaration: `buf: pointer[E], length: size_t` -> `buf: Slice[E]`
    for i in range(a, min(a + 8, len(lines))):
        new = re.sub(rf'{re.escape(buf)}: pointer\[{re.escape(elem)}\],\s*{re.escape(length)}: \w+',
                     f'{buf}: Slice[{elem}]', lines[i])
        if new != lines[i]:
            lines[i] = new; break
    else:
        sys.exit(f"declaration not matched in {routine}")
    # body
    for i in range(a, b + 1):
        s = lines[i]
        s = bare_store_to_put(store_to_put(s, buf), buf)
        s = bare_deref_to_subscript(deref_to_subscript(s, buf), buf)
        if i > a:
            s = re.sub(rf'(?<![\w.]){re.escape(length)}(?![\w])', f'{buf}.length', s)
        lines[i] = s

    # ★★★VERIFY, because the failure is SILENT: pointer arithmetic on a `Slice`
    # COMPILES -- `*(codes + (i as size_t))` on a converted parameter built and
    # ran and read garbage (SIGBUS in AllowedParams::allow, corpus 380 -> 124).
    # The first version of this tool used `\*\(name \+ ([^()]*?)\)`, which
    # cannot match an index that itself contains parentheses, and left exactly
    # that site behind. A leftover deref is a hard failure here, never a warning.
    # ★A BARE `*name` counts too, not only `*(name + i)`: `*data = 0x0D` on a
    # converted parameter compiled, desugared the `=` to `Slice.equals`, and
    # SIGSEGV'd inside it -- the deref does not need arithmetic to be wrong.
    left = [i + 1 for i in range(a, b + 1)
            if re.search(rf'\*\(\s*{re.escape(buf)}\s*[+\-]', lines[i])
            or re.search(rf'\*{re.escape(buf)}(?![\w])', lines[i])]
    if left:
        sys.exit(f"REFUSED: {routine} still derefs {buf} at line(s) {left} -- "
                 f"pointer arithmetic on a Slice compiles and reads garbage")
    # ★And a surviving `set buf[i]:` is the OTHER half: rc 4 rather than
    # garbage, but the tool must not emit a form the compiler rejects.
    stores = [i + 1 for i in range(a, b + 1)
              if re.search(rf'set\s+{re.escape(buf)}\[', lines[i])]
    if stores:
        sys.exit(f"REFUSED: {routine} assigns through a subscript on {buf} at "
                 f"line(s) {stores} -- the write spelling is {buf}.put(i, v)")
    open(path, 'w').write('\n'.join(lines))
    print(f"{path}: {routine}({buf}) -> Slice[{elem}]")

if __name__ == '__main__':
    main(*sys.argv[1:6])

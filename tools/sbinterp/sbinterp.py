#!/usr/bin/env python3
"""sbinterp.py -- turn straight-line StringBuilder runs into interpolated strings.

    tools/sbinterp/sbinterp.py [--apply] [--max-width N] [files...]

A run qualifies when, at ONE indentation level and with nothing in between:

    var sb StringBuilder()          (or `let`)
    sb.append <one argument>        (parenless, or `sb.append(<one argument>)`)
    ...
    <a line that uses sb.to_string() exactly once>

and `sb` is not mentioned again in the rest of the routine. The run becomes the
use line with `sb.to_string()` replaced by `"...`...`..."`. An interpolated
string desugars to exactly this StringBuilder block (the Modeler), and every
embedded expression goes through the same `append` overload resolution, so the
program means the same thing.

Refused (left as they are), each for a reason the language gives:
  * a two-argument append (`append(ptr, len)`) -- no interpolation form;
  * a backtick anywhere in the run -- it would open or close an embedding;
  * a `"` inside an embedded (non-literal) argument -- inside an embedding a
    quote must be escaped, and a literal inside an embedding may not contain
    backticks; kept simple on purpose;
  * a comment line inside the run -- it would be lost;
  * a result line wider than --max-width (default 110) -- readability.

Without --apply it prints what it would do and a count per package. Generated
files (GENERATED in the first 15 lines) are never touched. SBINTERP_RANGE=a:b
limits a run to builders declared on lines a..b (bisecting a file).

The first run (2026-09-25, 93 builders) exposed three compiler gaps, all
closed before it landed: an explicit `return "...`x`..."` was refused, a stored
interpolation was not promoted (restamp_operand_life# had no block arm), and a
grouped parenless call `(N.enc name)` -- what an embedding desugars to --
evaluated to its head. It is not instruction-neutral by design: a one-char
literal segment appends a char where the hand code appended a String, adjacent
literal pieces merge, a builder without embeddings folds to a constant. Prove a
run with tools/ifmerge/o2check.sh for the shape and the full bar for the rest.
"""
import os
import re
import subprocess
import sys

MAX_WIDTH = 110
DECL = re.compile(r'^(\s*)(?:var|let) (\w+) StringBuilder\(\)\s*$')
STRLIT = re.compile(r'^"((?:[^"\\]|\\.)*)"$')
CHARLIT = re.compile(r'^\(?"((?:[^"\\]|\\.)*)" as char\)?$')


def split_arg(line, name):
    """The single argument of `name.append ...`, or None."""
    s = line.strip()
    m = re.match(r'^' + re.escape(name) + r'\.append(\s*\((.*)\)\s*|\s+(.*))$', s)
    if not m:
        return None
    if m.group(2) is not None:
        arg = m.group(2).strip()
        # `append(a, b)` has a top-level comma: refuse
        depth = 0
        inq = False
        i = 0
        while i < len(arg):
            c = arg[i]
            if inq:
                if c == '\\':
                    i += 2
                    continue
                if c == '"':
                    inq = False
            else:
                if c == '"':
                    inq = True
                elif c in '([':
                    depth += 1
                elif c in ')]':
                    depth -= 1
                elif c == ',' and depth == 0:
                    return None
            i += 1
        if depth != 0:
            return None
        return arg
    return m.group(3).strip()


def piece(arg):
    """The interpolated-string text for one append argument, or None."""
    m = STRLIT.match(arg)
    if m:
        return m.group(1)
    m = CHARLIT.match(arg)
    if m:
        return m.group(1)
    if '"' in arg or '`' in arg:
        return None
    return '`' + arg + '`'


def indent_of(line):
    return len(line) - len(line.lstrip())


def routine_end(lines, i, ind):
    """First line after i that leaves the routine the run sits in: a line
    indented less than the run's own level, past the closing brace."""
    j = i
    while j < len(lines):
        t = lines[j]
        if t.strip() and indent_of(t) < ind and not t.strip().startswith(';'):
            if t.strip() != '}':
                return j
            # a closing brace ends a block; keep scanning at the lower level
            ind = indent_of(t)
            if ind <= 4:
                return j
        j += 1
    return j


def convert_file(path, apply, max_width, report):
    lines = open(path).read().split('\n')
    out = []
    i = 0
    n = 0
    refused = {}
    while i < len(lines):
        m = DECL.match(lines[i])
        if not m:
            out.append(lines[i])
            i += 1
            continue
        ind, name = m.group(1), m.group(2)
        j = i + 1
        pieces = []
        reason = None
        use = None
        while j < len(lines):
            t = lines[j]
            s = t.strip()
            if s == '' :
                reason = 'blank line'
                break
            if s.startswith(';'):
                reason = 'comment'
                break
            if indent_of(t) != len(ind):
                reason = 'nesting'
                break
            if re.match(r'^' + re.escape(name) + r'\.append\b', s):
                arg = split_arg(t, name)
                if arg is None:
                    reason = 'two-argument append'
                    break
                p = piece(arg)
                if p is None:
                    reason = 'quote or backtick'
                    break
                pieces.append(p)
                j += 1
                continue
            if t.count(name + '.to_string()') == 1 and not re.search(
                    r'\b' + re.escape(name) + r'\b', t.replace(name + '.to_string()', '')):
                use = j
            else:
                reason = 'not a plain use'
            break
        if use is None or not pieces:
            refused[reason or 'no use'] = refused.get(reason or 'no use', 0) + 1
            out.append(lines[i])
            i += 1
            continue
        end = routine_end(lines, use + 1, len(ind))
        if any(re.search(r'\b' + re.escape(name) + r'\b', l) for l in lines[use + 1:end]):
            refused['reused after'] = refused.get('reused after', 0) + 1
            out.append(lines[i])
            i += 1
            continue
        text = '"' + ''.join(pieces) + '"'
        new = lines[use].replace(name + '.to_string()', text)
        if len(new) > max_width:
            refused['too wide'] = refused.get('too wide', 0) + 1
            out.append(lines[i])
            i += 1
            continue
        rng = os.environ.get('SBINTERP_RANGE')
        if rng:
            lo, hi = (int(x) for x in rng.split(':'))
            if not (lo <= i + 1 <= hi):
                out.append(lines[i])
                i += 1
                continue
        if report:
            print(f"{path}:{i + 1}: {len(pieces)} appends")
            print(f"    {new.strip()}")
        out.append(new)
        n += 1
        i = use + 1
    if apply and n:
        open(path, 'w').write('\n'.join(out))
    return n, refused


def main():
    apply = '--apply' in sys.argv
    max_width = MAX_WIDTH
    args = [a for a in sys.argv[1:] if a != '--apply']
    if '--max-width' in args:
        k = args.index('--max-width')
        max_width = int(args[k + 1])
        del args[k:k + 2]
    files = args or subprocess.run(
        "find packages -name '*.scaly' -not -path '*/interface/*'",
        shell=True, capture_output=True, text=True).stdout.split()
    total = {}
    reasons = {}
    for f in files:
        if 'GENERATED' in open(f).read(2000):
            continue
        n, r = convert_file(f, apply, max_width, not apply)
        if n:
            pkg = f.split('/')[1]
            total[pkg] = total.get(pkg, 0) + n
        for k, v in r.items():
            reasons[k] = reasons.get(k, 0) + v
    print('converted:' if apply else 'would convert:', total, 'sum', sum(total.values()))
    print('left as they are:', reasons)


if __name__ == '__main__':
    main()

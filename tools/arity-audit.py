#!/usr/bin/env python3
"""tools/arity-audit.py — flag call sites whose argument count disagrees with
the callee's definition in emitted LLVM IR.

    tools/arity-audit.py <file.ll> [more.ll ...]      exit 1 if any DANGEROUS

Why this exists (2026-08-01): `FotSink.start_line_field` called
`TeXFOTBuilder.start_line_field()` one argument short. scalyc accepted it
silently (the mirror of the documented too-many-arguments hole, CLAUDE.md
"Calls & operands"), and at -O0 the callee read whatever the ABI register still
held — the caller's own `nic`, by luck. Turning on `opt -O2` reallocated that
register and the value became garbage: a silent wrong-output miscompile that
only one fixture in one suite caught. Run this over emitted IR BEFORE blaming
LLVM for an opt-only failure.

Two severities:
  DANGEROUS  the callee READS a parameter the call site never passes -> the
             callee consumes an undefined register. This is the miscompile.
  benign     the surplus parameter is dead in the callee (today this is the
             emitter appending an unused implicit caller-page slot). Harmless
             now, but it is the same defect one edit away from biting.
"""
import re
import sys
import collections


def split_args(s):
    """Split a parenthesised argument list at top-level commas."""
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch in '([{<':
            depth += 1
        elif ch in ')]}>':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return out


def inside_parens(s, open_idx):
    """Return the substring inside the parens that open at `open_idx`."""
    depth = 0
    for i in range(open_idx, len(s)):
        if s[i] == '(':
            depth += 1
        elif s[i] == ')':
            depth -= 1
            if depth == 0:
                return s[open_idx + 1:i]
    return None


def collect_signatures(src):
    """name -> (param_count, body_text or None); None count means variadic."""
    sigs = {}
    bodies = {}
    for m in re.finditer(r'^(define|declare)[^@\n]*@([\w.$]+)\(', src, re.M):
        name = m.group(2)
        inner = inside_parens(src, m.end() - 1)
        if inner is None:
            continue
        args = split_args(inner)
        count = None if any('...' in a for a in args) else len(
            [a for a in args if a.strip()])
        if m.group(1) == 'define':
            end = src.find('\n}', m.start())
            bodies[name] = src[m.start():end if end > 0 else len(src)]
            sigs[name] = count
        elif name not in sigs:
            sigs[name] = count
    return sigs, bodies


def param_is_read(body, index):
    """Is the index-th unnamed parameter (%<index>) used in the body?"""
    if body is None:
        return True          # no body here -> assume the worst
    header, _, rest = body.partition('\n')
    return re.search(r'%%%d\b' % index, rest) is not None


def audit(path):
    src = open(path).read()
    sigs, bodies = collect_signatures(src)
    findings = collections.OrderedDict()
    caller = None
    for line in src.split('\n'):
        dm = re.match(r'^define[^@\n]*@([\w.$]+)\(', line)
        if dm:
            caller = dm.group(1)
        cm = re.search(r'\bcall\b.*?@([\w.$]+)\(', line)
        if not cm:
            continue
        callee = cm.group(1)
        if callee.startswith('llvm.') or sigs.get(callee) is None:
            continue
        inner = inside_parens(line, line.index('(', cm.end() - 1))
        if inner is None:
            continue
        passed = len([a for a in split_args(inner) if a.strip()])
        want = sigs[callee]
        if passed == want:
            continue
        # Missing arguments are dangerous only if the callee reads them.
        dangerous = any(param_is_read(bodies.get(callee), i)
                        for i in range(passed, want)) if passed < want else True
        findings.setdefault((caller, callee, passed, want, dangerous), 0)
        findings[(caller, callee, passed, want, dangerous)] += 1
    return findings


def main(argv):
    rc = 0
    for path in argv:
        findings = audit(path)
        bad = [k for k in findings if k[4]]
        ok = [k for k in findings if not k[4]]
        if not findings:
            print("%s: no arity mismatches" % path)
            continue
        print("%s: %d DANGEROUS, %d benign" % (path, len(bad), len(ok)))
        for caller, callee, passed, want, _ in bad:
            print("  DANGEROUS in %s\n    -> %s: passes %d, callee takes %d "
                  "(callee READS the missing argument)"
                  % (caller, callee, passed, want))
        for caller, callee, passed, want, _ in ok:
            print("  benign    in %s\n    -> %s: passes %d, callee takes %d "
                  "(surplus parameter is dead)" % (caller, callee, passed, want))
        if bad:
            rc = 1
    return rc


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

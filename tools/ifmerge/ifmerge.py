#!/usr/bin/env python3
"""ifmerge.py -- fold nested `if`s into `and`, and `if X return true` runs into `or`.

    tools/ifmerge/ifmerge.py [--apply] [--max N] FILE...

Cosmetic only: the folded form optimizes to the same instructions (measured
2026-09-24), and tools/ifmerge/o2check.sh proves it per function, old tree
against new tree, before anything is committed.

Two shapes:

  A  nested braceless `if`s, none with an `else`
         if c1                     if c1 and c2 and c3
             if c2           ->        stmt
                 if c3
                     stmt

  M  a run of `if x = LIT` / `return true` over one plain name, three or
     more: a `match` (cases wrapped at --max, a branch per line, spread
     evenly); `if x = A or x = B` lines and `match x` blocks whose branches
     all `return true` join the same run, so a run is ONE `match`

  B  a run of `if X` / `return true` at one indent
         if c1                     if c1 or c2
             return true     ->        return true
         if c2
             return true

Left alone, each for a reason: an `else` at any level (it binds to the inner
`if`), a comment between the levels, a condition that is a PARENLESS call
(`if contains x` -- it is only a call as the whole operand run, and `and`
next to it parses as something else: the emitter aborted on the probe), a
block `{` in a condition, `if let`, a GENERATED file (change its generator),
and a folded line longer than --max (default 120) columns. A condition
holding `or`/`||` is parenthesized in an `and` fold (`and` binds tighter);
`and` inside an `or` fold needs nothing.

Without --apply it reports what it would change.
"""
import re
import sys

OP_CHARS = set('+-*/=%&|~<>')
WORD_OPS = {'and', 'or', 'not', 'xor', 'as', 'is'}


def ind(line):
    return len(line) - len(line.lstrip(' '))


def is_generated(lines):
    # `GENERATED`, or `DO NOT EDIT` -- grammar.scaly says only the latter in
    # capitals, and mkp regenerated it under the rewrite
    # (tools/gencheck.sh's convention is `GENERATED`; prose that merely says
    # "generated" -- "ast_generated.go" -- is no marker)
    head = '\n'.join(lines[:15])
    return 'GENERATED' in head or 'DO NOT EDIT' in head


def top_level_tokens(cond):
    """Split a condition at spaces outside quotes, parens and brackets."""
    toks, cur, depth, q = [], '', 0, None
    for ch in cond:
        if q:
            cur += ch
            if ch == q:
                q = None
            continue
        if ch in '"\'':
            q = ch
            cur += ch
            continue
        if ch in '([':
            depth += 1
        elif ch in ')]':
            depth -= 1
        if ch == ' ' and depth == 0:
            if cur:
                toks.append(cur)
            cur = ''
            continue
        cur += ch
    if cur:
        toks.append(cur)
    return toks


def is_operator_token(t):
    return t in WORD_OPS or (t and all(c in OP_CHARS for c in t))


def condition_ok(cond):
    """A condition that survives being joined with `and`/`or`."""
    if not cond or '{' in cond or ';' in cond or cond.startswith('let '):
        return False
    toks = top_level_tokens(cond)
    # `if x: y` -- the colon form carries its body on the same line
    if any(t.endswith(':') or t == ':' for t in toks):
        return False
    # a parenless call: two operands in a row with no operator between them
    for a, b in zip(toks, toks[1:]):
        if not is_operator_token(a) and not is_operator_token(b):
            return False
    return True


def needs_parens_in_and(cond):
    toks = top_level_tokens(cond)
    return any(t in ('or', '||') for t in toks)


def cond_of(line):
    s = line.strip()
    if not s.startswith('if ') or s.startswith('if let '):
        return None
    return s[3:]


def fold_and(lines, i, max_len):
    """Shape A at line i: return (new_lines, consumed) or None."""
    chain = [i]
    j = i
    while True:
        k = j + 1
        if k >= len(lines):
            break
        nxt = lines[k]
        if ind(nxt) == ind(lines[j]) + 4 and cond_of(nxt) is not None:
            chain.append(k)
            j = k
            continue
        break
    if len(chain) < 2:
        return None
    conds = [cond_of(lines[c]) for c in chain]
    if not all(condition_ok(c) for c in conds):
        return None
    inner = chain[-1]
    body_ind = ind(lines[inner]) + 4
    # the body: lines deeper than the innermost `if`
    e = inner + 1
    while e < len(lines) and (lines[e].strip() == '' or ind(lines[e]) >= body_ind):
        e += 1
    if e == inner + 1:
        return None                      # an `if` with no body on its own line
    # trailing blank lines belong to what follows
    while e > inner + 1 and lines[e - 1].strip() == '':
        e -= 1
    if e < len(lines):
        s = lines[e].strip()
        if s.startswith('else') and ind(lines[e]) >= ind(lines[i]):
            return None
        # anything else at an INTERMEDIATE level is not ours to move
        if s and ind(lines[e]) > ind(lines[i]):
            return None
    parts = ['(' + c + ')' if needs_parens_in_and(c) else c for c in conds]
    head = ' ' * ind(lines[i]) + 'if ' + ' and '.join(parts)
    if len(head) > max_len:
        return None
    shift = 4 * (len(chain) - 1)
    body = []
    for l in lines[inner + 1:e]:
        body.append(l[shift:] if l.strip() else l)
    return [head] + body, e - i


def fold_or(lines, i, max_len):
    """Shape B at line i: return (new_lines, consumed) or None."""
    base = ind(lines[i])
    conds = []
    j = i
    while j + 1 < len(lines):
        c = cond_of(lines[j])
        if c is None or ind(lines[j]) != base:
            break
        r = lines[j + 1]
        if r.strip() != 'return true' or ind(r) != base + 4:
            break
        conds.append(c)
        j += 2
    if len(conds) < 2:
        return None
    if not all(condition_ok(c) for c in conds):
        return None
    # an `else` after the run would bind to its last `if` only
    if j < len(lines) and lines[j].strip().startswith('else') and ind(lines[j]) == base:
        return None
    head = ' ' * base + 'if ' + ' or '.join(conds)
    if len(head) > max_len:
        return None
    return [head, ' ' * (base + 4) + 'return true'], j - i


MATCH_LIT = r'("([^"\\]|\\.)*"(\s+as\s+char)?|0x[0-9A-Fa-f]+|-?\d+|[A-Z][A-Z0-9_]*)'


def fold_match(lines, i, max_len):
    """Shape M at line i: a run of `if x = LIT` / `return true` over ONE plain
    name becomes a `match` -- a branch per line of cases, each `return true`
    (the grammar keeps a branch's cases on one line)."""
    base = ind(lines[i])
    subject, lits, j = None, [], i
    while j + 1 < len(lines):
        c = cond_of(lines[j])
        if c is None or ind(lines[j]) != base:
            break
        r = lines[j + 1]
        if r.strip() != 'return true' or ind(r) != base + 4:
            break
        m = re.fullmatch(r'([a-z][A-Za-z0-9_]*) = ' + MATCH_LIT, c)
        if not m:
            return None
        if subject is None:
            subject = m.group(1)
        elif m.group(1) != subject:
            return None
        lits.append(c[len(subject) + 3:])
        j += 2
    if len(lits) < 3:
        return None                       # two cases read fine as `or`
    if j < len(lines) and lines[j].strip().startswith('else') and ind(lines[j]) == base:
        return None
    return [' ' * base + 'match ' + subject] + wrap_cases(' ' * (base + 4), lits, max_len), j - i


def wrap_cases(pad, lits, max_len):
    """Branches of `case ...: return true`, as few as fit --max, the cases
    spread evenly over them (the narrowest width that needs no more lines)."""
    def greedy(width):
        rows, line = [], ''
        for lit in lits:
            cand = (line + ' ' if line else '') + 'case ' + lit
            if line and len(pad + cand + ': return true') > width:
                rows.append(line)
                line = 'case ' + lit
            else:
                line = cand
        rows.append(line)
        return rows
    n = len(greedy(max_len))
    width = max_len
    while width > 0 and len(greedy(width - 1)) == n:
        width -= 1
    return [pad + r + ': return true' for r in greedy(width)]


CASE_LINE = re.compile(r'((case\s+' + MATCH_LIT + r')\s*)+: return true')


def subject_lits(cond):
    """`x = A` or `x = A or x = B ...` over one plain name: (x, [A, B, ...])."""
    subject, lits = None, []
    for part in cond.split(' or '):
        m = re.fullmatch(r'([a-z][A-Za-z0-9_]*) = ' + MATCH_LIT, part)
        if not m or (subject is not None and m.group(1) != subject):
            return None
        subject = m.group(1)
        lits.append(part[len(subject) + 3:])
    return subject, lits


def case_lits(line):
    """The literals of a `case A case B: return true` line."""
    body = line.strip()[:-len(': return true')]
    return [m.group(1) for m in re.finditer(r'case\s+' + MATCH_LIT, body)]


def fold_subject_run(lines, i, max_len):
    """Shape M over a whole run at one indent: `if x = A` / `return true`,
    `if x = B or x = C` / `return true` and `match x` blocks whose branches
    all `return true`, over ONE plain name, become ONE `match` (a first pass
    left runs split where a line grew past --max, and a second one would put
    two `match x` side by side)."""
    base = ind(lines[i])
    subject, lits, items, has_match, j = None, [], 0, False, i
    while j < len(lines) and ind(lines[j]) == base and lines[j].strip():
        s = lines[j].strip()
        c = cond_of(lines[j])
        if c is not None:
            sl = subject_lits(c)
            if not sl or (subject is not None and sl[0] != subject):
                break
            if j + 1 >= len(lines) or lines[j + 1].strip() != 'return true' or ind(lines[j + 1]) != base + 4:
                break
            subject = sl[0]
            lits += sl[1]
            items += 1
            j += 2
            continue
        m = re.fullmatch(r'match ([a-z][A-Za-z0-9_]*)', s)
        if m and (subject is None or m.group(1) == subject):
            k = j + 1
            got = []
            while k < len(lines) and lines[k].strip() and ind(lines[k]) > base:
                if ind(lines[k]) != base + 4 or not CASE_LINE.fullmatch(lines[k].strip()):
                    got = None
                    break
                got += case_lits(lines[k])
                k += 1
            if not got:
                break
            subject = m.group(1)
            lits += got
            items += 1
            has_match = True
            j = k
            continue
        break
    if items < 2 or len(lits) < 3:
        return None
    # an `else` after the run would bind to its last `if` -- or, after a
    # `match`, become the match's own `else`
    if j < len(lines) and lines[j].strip().startswith('else'):
        return None
    return [' ' * base + 'match ' + subject] + wrap_cases(' ' * (base + 4), lits, max_len), j - i


def wrap_cases(pad, lits, max_len):
    """Branches of `case ...: return true`, as few as fit --max, the cases
    spread evenly over them (the narrowest width that needs no more lines)."""
    def greedy(width):
        rows, line = [], ''
        for lit in lits:
            cand = (line + ' ' if line else '') + 'case ' + lit
            if line and len(pad + cand + ': return true') > width:
                rows.append(line)
                line = 'case ' + lit
            else:
                line = cand
        rows.append(line)
        return rows
    n = len(greedy(max_len))
    width = max_len
    while width > 0 and len(greedy(width - 1)) == n:
        width -= 1
    return [pad + r + ': return true' for r in greedy(width)]


def fold_match_or(lines, i, max_len):
    """Shape M on an already folded line: `if x = A or x = B or x = C` with a
    one-line body and no `else` becomes `match x  case A case B case C: body`.
    Wrapped over several branches only for `return true` (the body repeats)."""
    c = cond_of(lines[i])
    if c is None or i + 2 > len(lines):
        return None
    base = ind(lines[i])
    parts = c.split(' or ')
    if len(parts) < 3:
        return None
    subject, lits = None, []
    for part in parts:
        m = re.fullmatch(r'([a-z][A-Za-z0-9_]*) = ' + MATCH_LIT, part)
        if not m:
            return None
        if subject is None:
            subject = m.group(1)
        elif m.group(1) != subject:
            return None
        lits.append(part[len(subject) + 3:])
    body = lines[i + 1]
    if ind(body) != base + 4 or not body.strip() or body.strip().startswith(';'):
        return None
    k = i + 2
    if k < len(lines) and lines[k].strip() and ind(lines[k]) > base:
        return None                       # the body is more than one line
    if k < len(lines) and lines[k].strip().startswith('else') and ind(lines[k]) == base:
        return None
    stmt = body.strip()
    pad = ' ' * (base + 4)
    out, line = [' ' * base + 'match ' + subject], ''
    for lit in lits:
        cand = (line + ' ' if line else '') + 'case ' + lit
        if line and len(pad + cand + ': ' + stmt) > max_len:
            if stmt != 'return true':
                return None
            out.append(pad + line + ': ' + stmt)
            line = 'case ' + lit
        else:
            line = cand
    out.append(pad + line + ': ' + stmt)
    return out, 2


def process(path, apply, max_len, shapes):
    text = open(path, encoding='utf-8').read()
    lines = text.split('\n')
    if is_generated(lines):
        return 0, 0
    out, i, n_and, n_or = [], 0, 0, 0
    while i < len(lines):
        r = fold_subject_run(lines, i, max_len) if 'match' in shapes else None
        if r:
            out.extend(r[0]); i += r[1]; n_or += 1
            continue
        r = fold_match_or(lines, i, max_len) if 'match' in shapes else None
        if r:
            out.extend(r[0]); i += r[1]; n_or += 1
            continue
        r = fold_match(lines, i, max_len) if 'match' in shapes else None
        if r:
            out.extend(r[0]); i += r[1]; n_or += 1
            continue
        r = fold_or(lines, i, max_len) if 'or' in shapes else None
        if r:
            out.extend(r[0]); i += r[1]; n_or += 1
            continue
        r = fold_and(lines, i, max_len) if 'and' in shapes else None
        if r:
            # the folded body may itself start a foldable run: fold again
            out.extend(r[0]); i += r[1]; n_and += 1
            continue
        out.append(lines[i]); i += 1
    if apply and (n_and or n_or):
        open(path, 'w', encoding='utf-8').write('\n'.join(out))
    return n_and, n_or


def main():
    args = sys.argv[1:]
    apply = '--apply' in args
    max_len = 120
    if '--max' in args:
        max_len = int(args[args.index('--max') + 1])
    shapes = {'and', 'or', 'match'}
    if '--only' in args:
        shapes = {args[args.index('--only') + 1]}
    files = [a for a in args if a.endswith('.scaly')]
    ta = to = 0
    for f in files:
        a, o = process(f, apply, max_len, shapes)
        if a or o:
            print(f'{f}: and-folds {a}, or-folds {o}')
        ta += a; to += o
    print(f'total: and-folds {ta}, or-folds {to}' + ('' if apply else ' (dry run)'))


if __name__ == '__main__':
    main()

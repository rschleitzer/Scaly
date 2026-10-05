#!/usr/bin/env python3
"""Collect the HAND-WRITTEN spelling of `allocate_slice[T]`.

    Slice[T](L, R.allocate(B, A) as pointer[T])   ->   allocate_slice[T](R, L)

`allocate_slice` (2026-09-04, `scaly/containers/Slice.scaly`) IS this
construction -- its own comment names the sites that write it by
hand.  Nine were harvested in the first round, ALL with a scalar
element type; the ones found here are the rest, and among them lie the two
element classes that have never run through this function: `ref[X]?`
(NPO option, opensp's Id/Lpd/Partition/Syntax) and a 16-byte STRUCT
(`Slice[StringC]` in Syntax.scaly).  Both are established by a probe BEFORE
anything was rewritten here -- a put/get round trip over `allocate_slice`.

★★★THE REAL ARGUMENT is not a count but an ASYMMETRY:
`Escape.scaly` and `Jit.scaly` call `allocate_slice` in the `init` (line 275/597)
and write the same allocation by hand in the REHASH of the same file (346/672).
Exactly this shape is the proof and not the hint: when a
sibling routine already has the form and this one does not, that is the find.

★★★WHAT THE GAIN IS: each of these lines MINTS an `as pointer[T]`.
`Page.allocate` answers raw memory, so somebody has to give it a
type -- done at the call site, that is exactly the pointer the campaign looks for.
In `allocate_slice` the cast stands ONCE.

★★★WHAT THIS SCRIPT DOES NOT DECIDE: whether `B` really is `L * sizeof T`.
It CHECKS that textually (below) and REFUSES what it cannot prove -- the
equality `alignof pointer[Id]` == `alignof ref[Id]?`, say, stands here as a
table and not as a guess.  A rest the table does not carry is
reported and left alone; the reader decides, never the tool.

Usage: tools/allocslice_inline.py [--apply]
"""
import re, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'derefcensus'))
import scan

# Element size in bytes, as far as it is PROVABLE here.  A type that is not
# in this table is accepted only when `B` names it SYMBOLICALLY
# (`n * sizeof StringC`) -- then the equality is textual and needs no
# number.
SIZE = {'u8': 1, 'i8': 1, 'char': 1, 'u16': 2, 'i16': 2, 'u32': 4, 'i32': 4,
        'u64': 8, 'i64': 8, 'int': 8, 'size_t': 8}
# A `ref[X]?` is Option[ref[X]] and therefore NPO -- pointer-sized.  The tree
# writes the allocation for it as `sizeof pointer[X]`, which is the same number.
PTRLIKE = 8

def norm(s):
    return re.sub(r'\s+', ' ', s).strip()

def elem_size(t):
    t = t.strip()
    if t in SIZE: return SIZE[t]
    if re.match(r'^ref\[.*\]\?$', t) or t.startswith('pointer['): return PTRLIKE
    return None

def bytes_match(L, B, T):
    """Is `B` provably `L * sizeof T`?"""
    L, B = norm(L), norm(B)
    sz = elem_size(T)
    # (a) symbolic: B is L * sizeof T  (parenthesisation does not matter)
    for form in (f'{L} * sizeof {T}', f'({L}) * sizeof {T}',
                 f'{L} * (sizeof {T})', f'({L}) * (sizeof {T})'):
        if norm(form) == B: return True
    # (b) for ref[X]?/pointers the tree writes `sizeof pointer[X]`
    if sz == PTRLIKE:
        inner = re.match(r'^ref\[(.*)\]\?$', T)
        names = ['void'] + ([inner.group(1)] if inner else [])
        for nm in names:
            for form in (f'{L} * (sizeof pointer[{nm}])', f'{L} * sizeof pointer[{nm}]',
                         f'({L}) * (sizeof pointer[{nm}])', f'({L}) * sizeof pointer[{nm}]'):
                if norm(form) == B: return True
    if sz is None: return False
    # (c) numeric: B is L with `* <sz>` appended, or sz == 1 and B == L
    if sz == 1 and B == L: return True
    # L may be an `X as size_t`, B the same base without the cast
    Lb = re.sub(r'\s+as\s+size_t\b', '', L).strip()
    Lb = Lb[1:-1].strip() if Lb.startswith('(') and Lb.endswith(')') else Lb
    Bb = B
    for cand in (L, Lb):
        if sz == 1 and norm(cand) == Bb: return True
        for form in (f'{cand} * {sz}', f'({cand}) * {sz}'):
            if norm(form) == Bb: return True
    return False

def align_match(A, T):
    A = norm(A)
    sz = elem_size(T)
    if A == f'alignof {T}': return True
    if sz == PTRLIKE:
        inner = re.match(r'^ref\[(.*)\]\?$', T)
        for nm in ['void'] + ([inner.group(1)] if inner else []):
            if A == f'alignof pointer[{nm}]': return True
    return sz is not None and A == str(sz)

def split_args(s):
    out, depth, cur = [], 0, []
    for c in s:
        if c in '([': depth += 1
        elif c in ')]': depth -= 1
        if c == ',' and depth == 0:
            out.append(''.join(cur)); cur = []
        else: cur.append(c)
    out.append(''.join(cur))
    return [x.strip() for x in out]

def balanced_end(text, open_at):
    """Index AFTER the ')' that belongs to text[open_at]=='('."""
    depth = 0
    for i in range(open_at, len(text)):
        if text[i] in '([': depth += 1
        elif text[i] in ')]':
            depth -= 1
            if depth == 0: return i + 1
    return -1

HEAD = re.compile(r'\bSlice\[')

def convert_text(text):
    """Replace every site in a (possibly multi-line) source text. Returns
    (new_text, [(old, new)], [(old, reason)])."""
    done, held = [], []
    pos = 0
    while True:
        m = HEAD.search(text, pos)
        if not m: break
        tb = balanced_end(text, m.end() - 1)          # ] of the type argument
        if tb < 0: break
        T = text[m.end():tb - 1].strip()
        if tb >= len(text) or text[tb] != '(':
            pos = m.end(); continue
        ab = balanced_end(text, tb)
        if ab < 0: pos = m.end(); continue
        whole = text[m.start():ab]
        args = split_args(text[tb + 1:ab - 1])
        if len(args) != 2 or 'allocate' not in args[1]:
            pos = m.end(); continue
        L, R = args[0], norm(args[1])
        am = re.match(r'^([A-Za-z_][\w.]*)\.allocate\s*\(', R)
        if not am:
            pos = m.end(); continue
        recv = am.group(1)
        ae = balanced_end(R, am.end() - 1)
        aargs = split_args(R[am.end():ae - 1])
        tail = norm(R[ae:])
        if len(aargs) != 2 or not re.match(r'^as\s+pointer\[', tail):
            held.append((norm(whole), f'tail is not `as pointer[..]`: {tail[:40]}'))
            pos = m.end(); continue
        B, A = aargs[0], aargs[1]
        if not bytes_match(L, B, T):
            held.append((norm(whole), f'byte count not provably L*sizeof {T}: {norm(B)}'))
            pos = m.end(); continue
        if not align_match(A, T):
            held.append((norm(whole), f'alignment not provably alignof {T}: {norm(A)}'))
            pos = m.end(); continue
        new = f'allocate_slice[{T}]({recv}, {norm(L)})'
        done.append((norm(whole), new))
        text = text[:m.start()] + new + text[ab:]
        pos = m.start() + len(new)
    return text, done, held

def main():
    apply = '--apply' in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not files:
        files = [p for p in sorted(scan.files('packages'))
                 if not p.endswith('containers/Slice.scaly')]
    nd = nh = nf = 0
    for p in files:
        raw = open(p, encoding='utf-8').read()
        if 'Slice[' not in raw or 'allocate' not in raw: continue
        new, done, held = convert_text(raw)
        if not done and not held: continue
        print(f'--- {p}')
        for a, b in done: print(f'   OK   {a[:120]}\n     ->  {b}')
        for a, r in held: print(f'   HELD {r}\n        {a[:120]}')
        nd += len(done); nh += len(held)
        if done:
            nf += 1
            if apply: open(p, 'w', encoding='utf-8').write(new)
    print(f'\nTOTAL: {nd} convertible, {nh} left alone, {nf} files'
          + ('' if apply else '  (counted only)'))

if __name__ == '__main__':
    main()

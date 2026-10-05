#!/usr/bin/env python3
"""Turn the (pointer, length) PAIR back into ONE value.

    StringC^host(x.data_ptr(),   x.get_size())     ->  StringC^host(x.as_slice())
    StringC^host(b.get_buffer(), b.get_length())   ->  StringC^host(b.as_slice())

★★★WHY THIS IS THE LARGEST SINGLE CLASS: `define StringC (data:
pointer[u32], size: size_t)` -- opensp's central string type IS a
`Slice[u32]` under another name.  Its `data_ptr()` has 201 call sites, and
FOUR of them reach a C function; the rest hands the pointer on to Scaly
code, overwhelmingly as exactly this pair.  That is not a pointer problem
but a Slice that is taken apart and put together again at the receiver.

★THE CONDITION THE TOOL CHECKS is the identity of the RECEIVER:
`x.data_ptr(), x.get_size()` only when the same expression stands in front
both times.  `f(a.data_ptr(), b.get_size())` measures two different things and
is exactly the trap `refslice`'s LOOSE verdict hangs on -- a length
that measures something other than the buffer.

★THE SECOND CONDITION is that the receiver HAS an `as_slice()`.  Three
carriers are established: `StringC` (since 2026-09-04), `Array[T]` and `Vector[T]`
(`Slice[T](length, get_buffer())` -- the pair word for word) and `String`
(u8).  Everything else is reported and left alone; the tool does not guess
which type stands on the left -- the COMPILER rejects what does not hold, and
the driver then takes the file back.

★★★WHAT IT DOES NOT TOUCH: a site whose target concept has NO `Slice` init.
`String(buffer.get_buffer(), buffer.get_length())` stands in the stdlib and
would owe a seed -- `--only` selects the target concept, and without it
it is `StringC`.

★★★TWO LOCKS, EACH OF WHICH WOULD HAVE COST A RUN:

  (1) A HIT IN PROSE IS NOT A SITE.  The first version would have rewritten
      the doc comment `data_ptr` had just been given -- it
      QUOTES the old form in order to explain it, and would have turned into
      nonsense.  Exactly that ("it rewrites PROSE") is the
      trap of the Slice rework.  Everything after a `;` stays untouched.

  (2) A GENERATED FILE IS REFUSED.  A conversion that is not also
      in the generator is a silent revert at the next
      regeneration.  The marker is looked for in lines 1-15, CASE-INSENSITIVELY:
      `CharProps.scaly` writes "Generated" with a capital G, and a
      case-sensitive grep for it reported the file as clean.
      `--allow-generated` lifts the lock for a case the READER
      has decided -- for `CharProps.scaly`, say, where `charpropgen.py`
      demonstrably emits only 58 lines of TABLES and no function, so the
      site lies in the hand-written part.

Usage: tools/pairslice.py [--apply] [--only=StringC[,String]]
                           [--allow-generated] [<file>...]
"""
import re, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'derefcensus'))
import scan

PAIR = re.compile(
    r'(?P<target>\b[A-Z]\w*)(?P<sigil>\^\w+|#)?\('
    r'\s*(?P<r1>[\w.]+)\.(?P<acc>data_ptr|get_buffer)\s*\(\s*\)'
    r'\s*,\s*(?P<r2>[\w.]+)\.(?P<len>get_size|get_length)\s*\(\s*\)\s*\)')

def code_end(line):
    """Index from which the line is comment (a `;` outside a string)."""
    return len(scan.strip_comment(line).rstrip('\n'))

def convert_line(line, targets):
    done, held = [], []
    out, pos, ce = '', 0, code_end(line)
    for m in PAIR.finditer(line):
        if m.start() >= ce:
            continue                       # (1) prose, not a site
        if m.group('target') not in targets:
            held.append((m.group(0), f"target concept {m.group('target')} not selected"))
            continue
        if m.group('r1') != m.group('r2'):
            held.append((m.group(0), f"different receivers: {m.group('r1')} / {m.group('r2')}"))
            continue
        new = f"{m.group('target')}{m.group('sigil') or ''}({m.group('r1')}.as_slice())"
        out += line[pos:m.start()] + new
        pos = m.end()
        done.append((m.group(0), new))
    return out + line[pos:], done, held

def main():
    apply = '--apply' in sys.argv
    only = 'StringC'
    allow_gen = '--allow-generated' in sys.argv
    for a in sys.argv[1:]:
        if a.startswith('--only='): only = a.split('=', 1)[1]
    targets = set(only.split(','))
    files = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not files:
        files = sorted(scan.files('packages'))
    nd = nh = nf = 0
    ngen = 0
    for p in files:
        src = open(p, encoding='utf-8').readlines()
        if not allow_gen and re.search(r'generat', ''.join(src[:15]), re.I):
            d = []
            for raw in src:                # LINE BY LINE -- `code_end` on the
                _, dd, _ = convert_line(raw, targets)   # whole text finds the
                d += dd                    # file's first `;` and sees nothing
            if d:
                ngen += len(d)
                print(f'--- {p}\n   GENERATED: {len(d)} site(s) skipped '
                      f'-- a conversion without the generator is a silent '
                      f'revert (--allow-generated lifts it)')
            continue
        out, fd, fh = [], [], []
        for i, raw in enumerate(src):
            # Comments stay untouched -- a prose line is not a site.
            if scan.strip_comment(raw).strip() != raw.strip() and raw.lstrip().startswith(';'):
                out.append(raw); continue
            new, d, h = convert_line(raw, targets)
            out.append(new)
            fd += [(i + 1, a, b) for a, b in d]
            fh += [(i + 1, a, r) for a, r in h]
        if not fd and not fh: continue
        print(f'--- {p}')
        for ln, a, b in fd: print(f'   OK   :{ln}  {a[:95]}\n     ->  {b}')
        for ln, a, r in fh: print(f'   HELD :{ln}  {r}\n        {a[:95]}')
        nd += len(fd); nh += len(fh)
        if fd:
            nf += 1
            if apply: open(p, 'w', encoding='utf-8').writelines(out)
    print(f'\nTOTAL: {nd} convertible, {nh} left alone, '
          f'{ngen} skipped in generated files, {nf} files'
          + ('' if apply else '  (counted only)'))

if __name__ == '__main__':
    main()

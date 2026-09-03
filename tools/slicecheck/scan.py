#!/usr/bin/env python3
"""Find the two SILENT misuses of a `Slice`-typed name.  Neither is diagnosed
by the compiler and neither is visible in a suite until an output differs.

  NULLTEST  `s = null` / `s <> null` on a Slice.  A Slice is a STRUCT, so this
            is not a null test.  Where the Slice came back from a function it
            is an sret slot and the comparison lowers to
                %eq = icmp eq ptr %sret.result, null
            against the address of an `alloca` -- CONSTANT FALSE.  Measured
            2026-09-03 in dazzle's SchemeParser: a guard that could not fire,
            and `element-with-id` folded every id through a null table.
            The spelling is `s.is_empty()`.

  ARITH     `s + i` / `*(s + i)` on a Slice.  There is no gate:
            report_ref_arithmetic# covers `ref`, not `Slice`.  It lowers to a
            BYTE-stride GEP into the struct itself --
                %ptr.add = getelementptr inbounds i8, ptr %sret.result, i64 %i
            -- so the Slice is read as its own table.  ★A grep for
            `add %_Z5Slice` on the IR does NOT find this: only the
            field-plus-offset form becomes an `add`, the sret form a GEP.
            The spellings are `s[i]`, `s.put(i, v)`, `s.subslice(a, b)`, and
            `s.data` at a boundary to an unconverted callee.

★★★NAMES ARE SCOPED TO THEIR ROUTINE, and that is the whole difference between
an instrument and a noise generator.  The first version of this file collected
Slice-typed names per FILE: one `function hash_name(d: Slice[char])` in
tscaly's ast.scaly made every unrelated `d` in the file a finding, and the run
reported 607 where the truth was a handful.  CLAUDE.md already carries this as
"a name-keyed index answers about the wrong name" -- from the refslice
campaign, which paid for it once.

Usage: tools/slicecheck/scan.py [root]        (default: packages)
"""
import os, re, sys, collections

ROOT = sys.argv[1] if len(sys.argv) > 1 else 'packages'
HEAD = re.compile(r'^\s*(function|procedure|operator|init)\b')
CDEF = re.compile(r'^define\s+(\w+)\b')

def strip_comment(line):
    out, i, instr = [], 0, None
    while i < len(line):
        c = line[i]
        if instr:
            if c == '\\': out.append(line[i:i+2]); i += 2; continue
            if c == instr: instr = None
            out.append(c); i += 1; continue
        if c in '"\'': instr = c; out.append(c); i += 1; continue
        if c == ';': break
        out.append(c); i += 1
    return ''.join(out)

FIELD = re.compile(r'^\s{1,8}([a-z_]\w*)\s*:\s*Slice\[')
PARAM = re.compile(r'\b([a-z_]\w*)\s*:\s*Slice\[')
BINDC = re.compile(r'\b(?:let|var)\s+([a-z_]\w*)\s+Slice\[')
BINDT = re.compile(r'\b(?:let|var)\s+([a-z_]\w*)\s*:\s*Slice\[')
RETS  = re.compile(r'\b(?:function|procedure)\s+(\w+)\(.*\)\s*returns\s+Slice\[')

def routine_spans(src):
    heads = [i for i, l in enumerate(src) if HEAD.match(l)]
    heads.append(len(src))
    return [(heads[i], heads[i + 1]) for i in range(len(heads) - 1)]

def main():
    files, returns_slice = [], set()
    for d, _, fs in os.walk(ROOT):
        for f in sorted(fs):
            if not f.endswith('.scaly'): continue
            p = os.path.join(d, f)
            src = [strip_comment(l) for l in open(p, encoding='utf-8', errors='replace')]
            files.append((p, src))
            for l in src:
                m = RETS.search(l)
                if m: returns_slice.add(m.group(1))

    findings = []
    for p, src in files:
        # ★★★A FIELD IS SCOPED TO ITS CONCEPT, NOT TO THE FILE, and only an
        # UNQUALIFIED or `this.`-qualified use can be it.  Applying a file's
        # Slice fields everywhere reported `if a.keys <> null` in dazzle's
        # Escape.scaly, where `a` is a MakeFlowObjInsn whose `keys` is an
        # honest `ref[Array[...]]?` and the Slice-typed `keys` is EsMap's,
        # 1300 lines up.  Same class as the file-wide NAME bug above, one
        # level out.
        cbounds, cur = [], None
        for i, l in enumerate(src):
            m = CDEF.match(l)
            if m:
                if cur: cbounds.append((cur[0], i, cur[1]))
                cur = (i, m.group(1))
        if cur: cbounds.append((cur[0], len(src), cur[1]))
        cfields = {}
        for ca, cb, _ in cbounds:
            cfields[(ca, cb)] = {m.group(1) for l in src[ca:cb]
                                 for m in [FIELD.match(l)] if m}
        def fields_at(i):
            for (ca, cb), fs in cfields.items():
                if ca <= i < cb: return fs
            return set()
        for a, b in routine_spans(src):
            names = set(fields_at(a))
            qualified_ok = {n for n in names}
            for l in src[a:b]:
                for pat in (PARAM, BINDC, BINDT):
                    for m in pat.finditer(l): names.add(m.group(1))
                m = re.search(r'\b(?:let|var)\s+([a-z_]\w*)\s+(?:\w+\.)*(\w+)\s*\(', l)
                if m and m.group(2) in returns_slice: names.add(m.group(1))
            if not names: continue
            alt = '|'.join(sorted(map(re.escape, names)))
            # a bare name, or `this.` -- never `<other>.name`
            pre = r'(?<![.\w])(?:this\.)?'
            null = re.compile(rf'{pre}({alt})\s*(?:=|<>)\s*null\b')
            arith = re.compile(rf'\*\(\s*{pre}({alt})\s*[+\-]|'
                               rf'{pre}({alt})\s*[+\-]\s+\w')
            for off, l in enumerate(src[a:b]):
                for m in null.finditer(l):
                    findings.append(('NULLTEST', p, a + off + 1, m.group(1), l.strip()[:86]))
                for m in arith.finditer(l):
                    findings.append(('ARITH', p, a + off + 1,
                                     m.group(1) or m.group(2), l.strip()[:86]))
        # a null test directly on a Slice-returning CALL, which binds no name
        for i, l in enumerate(src):
            for m in re.finditer(r'\b(?:\w+\.)*(\w+)\([^()]*\)\s*(?:=|<>)\s*null\b', l):
                if m.group(1) in returns_slice:
                    findings.append(('NULLTEST', p, i + 1, m.group(1) + '()', l.strip()[:86]))

    seen = set(); uniq = []
    for f in findings:
        if f[:3] in seen: continue
        seen.add(f[:3]); uniq.append(f)
    for kind, p, ln, name, txt in uniq:
        print(f'{kind:8} {p}:{ln}  {name}   {txt}')
    c = collections.Counter(k for k, *_ in uniq)
    print(f'--- {len(uniq)} findings  ' + '  '.join(f'{k}={n}' for k, n in c.items()))
    return 1 if uniq else 0

sys.exit(main())

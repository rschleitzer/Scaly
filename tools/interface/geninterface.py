#!/usr/bin/env python3
"""Generate a package's INTERFACE: its module tree with every non-generic body
replaced by `linked`.

Usage: tools/interface/geninterface.py <package-root.scaly> <facts> [--out DIR]
       (from the repo root; DIR defaults to <package dir>/interface)

A dependent root compiled against the interface must emit exactly what it emits
against the sources: the interface is the package as a caller sees it, with the
bodies in the package's archive. So it carries, besides the source text of every
declaration, what the compiler derives from bodies and a caller depends on. The
compiler writes those facts (`scalyc --interface-facts`), this tool does the
text surgery:

  * a non-generic routine (function, procedure, operator, init, deinit) keeps
    its header and ends in ` linked`. What the planner derives from its body
    and a caller depends on goes after its parameter list as attributes: the
    caller page R1..R7 inferred (`@page true`; part of the mangled name; an
    explicit `rp` in the source stays an `rp`), its memberships in the page
    sets R6/R7 join through bodies (`@direct`, `@transitive`, `@returnpage`),
    and whether it stores an argument beyond its frame (`@persists true`, the
    escape check's pass-to-storing callee). `reads g, h` names the changeable
    globals it reads and `io` says it reaches I/O -- both computed, a source
    declares neither;
  * a concept whose initializers or methods place data on its own page says
    `@resident true` (a construction of it must be page-hosted);
  * a `mutable`/`shared` global declares `linked` instead of its initializer;
  * generic concepts and routines keep their bodies (they are monomorphized at
    the caller), and so do constants, type aliases, externs and comments.

Facts, one per line (offsets are byte offsets into the source file):
  F <file> <start> <end> <kind> <name> generic= page= explicit= persists=
  C <file> <start> <end> concept <name> generic= page=<resident> ...
  G <file> <start> <end> global <name> ...
  R <file> <start> <globals>        (a routine's reads)
  I <file> <start>                  (a routine that reaches I/O -- computed)
  M <file>                          (a module file of the package's root)
"""
import os
import re
import sys
from collections import defaultdict


def skip_ws(t, i):
    while i < len(t) and t[i] in ' \t':
        i += 1
    return i


def skip_type(t, i):
    """The end of one type token at i (brackets balanced, `?` included)."""
    depth = 0
    while i < len(t):
        c = t[i]
        if c == '[':
            depth += 1
        elif c == ']':
            depth -= 1
        elif c in ' \t\n;' and depth == 0:
            break
        i += 1
    return i


def skip_balanced(t, i, open_c, close_c):
    """t[i] == open_c; answers the offset after the matching close_c."""
    depth = 0
    while i < len(t):
        c = t[i]
        if c == '"':
            i = t.index('"', i + 1)
        elif c == ';':
            i = t.index('\n', i) - 1
        elif c == open_c:
            depth += 1
        elif c == close_c:
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    raise ValueError('unbalanced')


WORD = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')


def header(t, start, kind):
    """(end of the header, offset of the parameter list's closing paren or None)."""
    i = start
    m = WORD.match(t, i)                      # the keyword
    i = m.end()
    i = skip_ws(t, i)
    if kind in ('fn', 'op'):
        if t[i] != '(':                       # a name, maybe operator symbols
            while i < len(t) and t[i] not in ' \t([\n':
                i += 1
            i = skip_ws(t, i)
        if i < len(t) and t[i] == '[':
            i = skip_balanced(t, i, '[', ']')
            i = skip_ws(t, i)
    if kind == 'init' and t[i] == '#':
        i = skip_ws(t, i + 1)
    close = None
    if i < len(t) and t[i] == '(':
        i = skip_balanced(t, i, '(', ')')
        close = i - 1
    while True:
        j = skip_ws(t, i)
        if t.startswith('\n', j):
            # a clause may continue on the next line
            k = skip_ws(t, j + 1)
            if re.match(r'(returns|throws|io|mutable|reads)\b', t[k:k + 8]):
                j = k
            else:
                return i, close
        if t[j] == '@':
            k = WORD.match(t, j + 1).end()
            k = skip_ws(t, k)
            k = skip_type(t, k)
            i = k
            continue
        w = re.match(r'(returns|throws)\b', t[j:j + 8])
        if w:
            k = skip_ws(t, j + len(w.group(1)))
            i = skip_type(t, k)
            continue
        if re.match(r'io\b', t[j:j + 3]):
            i = j + 2
            continue
        w = re.match(r'(mutable|reads)\s+[A-Za-z_][A-Za-z0-9_]*(\s*,\s*[A-Za-z_][A-Za-z0-9_]*)*', t[j:])
        if w:
            i = j + w.end()
            continue
        if t[j] == ':':
            i = j + 1
            continue
        return i, close


def body_end(t, e):
    """A Model span ends where the NEXT declaration's first token begins, so it
    takes that declaration's leading comment along; back up over trailing
    blank lines and whole-line comments to the end of this body."""
    while True:
        k = e
        while k > 0 and t[k - 1] in ' \t\n':
            k -= 1
        line_start = t.rfind('\n', 0, k) + 1
        if t[line_start:k].lstrip().startswith(';'):
            e = line_start
            continue
        return k


def transform(text, facts):
    """Apply the facts of one file; answers the interface text."""
    edits = []                                # (start, end, replacement)
    concepts = [f for f in facts if f[0] == 'C']
    generic_spans = [(f[2], f[3]) for f in concepts if f[5]['generic'] == '1']

    def inside_generic(s):
        return any(a <= s < b for a, b in generic_spans)

    reads = {f[2]: f[3] for f in facts if f[0] == 'R'}
    io_starts = {f[2] for f in facts if f[0] == 'I'}
    for f in facts:
        tag = f[0]
        if tag == 'F':
            _, _, s, e, kind, name, a = f
            if a['generic'] == '1' or inside_generic(s):
                continue
            h, close = header(text, s, kind)
            head = text[s:h]
            if close is not None:
                # the body-derived facts as routine attributes, right after
                # the parameter list (the grammar's place for them)
                facts_attrs = ''
                if a['page'] == '1' and a['explicit'] == '0':
                    facts_attrs += ' @page true'
                for key in ('persists', 'direct', 'transitive', 'returnpage'):
                    if a.get(key) == '1':
                        facts_attrs += ' @' + key + ' true'
                if a.get('ret'):
                    cls, arg = a['ret']
                    rname = {'1': 'ret_caller', '2': 'ret_explicit_' + arg,
                             '3': 'ret_param_' + arg, '4': 'ret_nonlocal'}.get(cls)
                    if rname:
                        facts_attrs += ' @' + rname + ' true'
                # the deep persist masks (Planner.facts_deep_line#): `@deep` says
                # they are stated, a missing mask is zero
                if a.get('deep') is not None:
                    d = a['deep']
                    facts_attrs += ' @deep true'
                    for key, v in zip(('beyond', 'this', 'whole', 'pageof'), d[:4]):
                        if v:
                            facts_attrs += ' @deep_%s_%d true' % (key, v)
                    for k, v in enumerate(d[4:19], start=1):
                        if v:
                            facts_attrs += ' @deep_into_%d_%d true' % (k, v)
                rel = close + 1 - s
                head = head[:rel] + facts_attrs + head[rel:]
            if s in io_starts and not re.search(r'\)[^;]*\bio\b', head):
                # computed, never written in a source: before a `mutable`
                # clause, else at the end of the header
                m = re.search(r'\smutable\s', head[(close - s) if close is not None else 0:])
                if m:
                    at = ((close - s) if close is not None else 0) + m.start()
                    head = head[:at] + ' io' + head[at:]
                else:
                    head = head.rstrip() + ' io'
            if s in reads:
                head = head.rstrip() + ' reads ' + reads[s]
            edits.append((s, body_end(text, e), head.rstrip() + ' linked'))
        elif tag == 'C':
            _, _, s, e, name, a = f
            if a['generic'] == '1' or a['resident'] != '1':
                continue
            m = re.compile(r'define\s+' + re.escape(name)).match(text, s)
            if m:
                # right before the record's `(`: after the name an optional
                # separator takes a line break, after an attribute nothing does
                k = m.end()
                while k < len(text) and text[k] in ' \t\n':
                    k += 1
                edits.append((k, k, '@resident true '))
        elif tag == 'G':
            _, _, s, e, name = f
            if inside_generic(s):
                continue
            m = re.compile(r'(mutable|shared(\s+atomic)?)\s+' + re.escape(name) + r'\s*:\s*').match(text, s)
            if not m:
                continue
            k = skip_type(text, m.end())
            edits.append((k, body_end(text, e), ' linked'))
    # apply back to front; a nested edit (a method inside an edited concept
    # header) never overlaps a body edit. LINE-PRESERVING: whatever a
    # replacement removes, its line breaks stay, so every line that remains is
    # on the line it has in the source -- a diagnostic from a generic body
    # (kept verbatim) then names the right line of the source file too.
    out = text
    for s, e, rep in sorted(edits, key=lambda x: (x[0], x[1]), reverse=True):
        rep = rep + '\n' * (out[s:e].count('\n') - rep.count('\n'))
        out = out[:s] + rep + out[e:]
    return out


def parse_facts(path):
    """Lines `TAG file start end kind name generic= page= explicit= persists=`
    (a concept's `page` column is its residency), `R file start globals` and
    `M file`; booleans come as true/false."""
    by_file = defaultdict(list)
    last = None
    for line in open(path):
        p = line.rstrip('\n').split(' ')
        # D/T/P continue the F line before them: direct, transitive, returnpage
        if p[0] in ('D', 'T', 'P') and last is not None:
            last[6][{'D': 'direct', 'T': 'transitive', 'P': 'returnpage'}[p[0]]] = '1'
            continue
        # V continues the F line before it: the return residence, class + arg
        if p[0] == 'V' and last is not None:
            last[6]['ret'] = (p[1], p[2])
            continue
        # X continues the F line before it: the deep persist masks, beyond this
        # whole pageof and the fifteen into-parameter slots (bit 0 is `this`)
        if p[0] == 'X' and last is not None:
            last[6]['deep'] = [int(x) for x in p[1:]]
            continue
        if not p or p[0] not in ('F', 'C', 'G', 'R', 'I', 'M'):
            continue
        tag, fil = p[0], os.path.normpath(p[1])
        if tag == 'M':
            by_file.setdefault(fil, [])
            continue
        if tag == 'R':
            by_file[fil].append(('R', fil, int(p[2]), p[3]))
            continue
        if tag == 'I':
            by_file[fil].append(('I', fil, int(p[2])))
            continue
        attrs = {}
        for x in p[6:]:
            k, v = x.split('=')
            attrs[k] = '1' if v in ('true', '1') else '0'
        start, end, kind, name = int(p[2]), int(p[3]), p[4], p[5]
        if tag == 'F':
            last = ('F', fil, start, end, kind, name, attrs)
            by_file[fil].append(last)
        elif tag == 'C':
            attrs['resident'] = attrs.get('page', '0')
            by_file[fil].append(('C', fil, start, end, name, attrs))
        else:
            by_file[fil].append(('G', fil, start, end, name))
    return by_file


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    root, facts_path = args[0], args[1]
    pkg_dir = os.path.dirname(root)
    out_dir = os.path.join(pkg_dir, 'interface')
    if '--out' in sys.argv:
        out_dir = sys.argv[sys.argv.index('--out') + 1]
    by_file = parse_facts(facts_path)
    # every module file of the root (`M` lines), facts or not: a file of only
    # constants, externs or generics is copied as it is
    n = 0
    for src in sorted(by_file):
        rel = os.path.relpath(src, pkg_dir)
        if rel.startswith('..'):
            continue
        # a `module x` without its file loads nothing; the interface keeps
        # the declaration and so loads nothing either
        if not os.path.exists(src):
            continue
        # the compiler's offsets are BYTES: read as latin-1, one character per
        # byte, and write back the same way, so UTF-8 passes through unchanged
        text = open(src, 'rb').read().decode('latin-1')
        out = transform(text, by_file[src])
        dst = os.path.join(out_dir, rel)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        with open(dst, 'wb') as f:
            f.write(out.encode('latin-1'))
        n += 1
    # the prelude is not a module of the stdlib's root: the modeler hangs it
    # under every PROGRAM's main module, where its bodies are planned and
    # emitted -- so it goes into the interface as it is, bodies and all
    prelude = os.path.join(pkg_dir, 'scaly', 'prelude.scaly')
    if os.path.basename(root) == 'scaly.scaly' and os.path.exists(prelude):
        dst = os.path.join(out_dir, 'scaly', 'prelude.scaly')
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        with open(prelude, 'rb') as f_in, open(dst, 'wb') as f_out:
            f_out.write(f_in.read())
        n += 1
    print(f"geninterface: {n} files -> {out_dir}")


if __name__ == '__main__':
    main()

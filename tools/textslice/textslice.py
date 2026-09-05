#!/usr/bin/env python3
"""Collapse `(pointer[char], int)` TEXT PAIRS into `Slice[char]` -- parameters,
fields and accessor returns -- across one package, chain by chain.

Built 2026-09-05 for `packages/tscaly`, whose 165 `pointer[char]` parameters,
17 fields and 19 returns are all ONE convention: a borrowed byte range spelled
as two values (`name_data`/`name_len`, `(d, n)`, `(text, text_len)`). The
refslice tools could not see them -- `tools/refslice/scan.py` pairs by a
length VOCABULARY (`n`, `len`, `*_len`) and by `size_t`, and reported LENLESS
for `an`/`bn`/`slen`/`l1`, `int` throughout -- so this tool takes the pairing
as INPUT (a spec the reader wrote after reading the declarations) and does
the mechanical half:

  params     `X: pointer[char], N: int` -> `X: Slice[char]`; in the routine's
             own body `N` -> `X.length`, `*(X + e)` -> `X[e]`, `*X` -> `X[0]`,
             `set *(X + e): v` -> `X.put(e, v)`. The body is then GREPPED for
             the three silent shapes CLAUDE.md names (`X + `, `Slice[char](X`,
             `*(X`) and every hit is printed as CHECK for the reader.
  fields     `F: pointer[char]` + `L: int` -> `NEW: Slice[char]`; everywhere
             `.F` -> `.NEW`, `.L` -> `.NEW.length`; a positional construction
             of the record collapses its pair like a call.
  accessors  `P(x)` returning the pointer and `L(x)` the length: `P` is
             re-signed to return `Slice[char]` (its body's `null` becomes
             `Slice[char]()`), optionally renamed; `L` STAYS as
             `P(x).length as int`, so a standalone length keeps its `int`.
  calls      at every call of a converted routine (matched by the spelling
             the code uses -- `Concept.static(`, `.method(`, `free(` -- AND by
             the OLD arity, never by name alone), the argument pair `(A, B)`
             collapses: `A` when B is provably A's length (`A.length`, the
             accessor twin `L(x)` beside `P(x)`, the field twin `.L` beside
             `.F`, or a local bound to one of those), else `Slice[char](B, A)`
             -- {length, data} is the field order since 2026-08-30. A pair
             whose A is ALREADY a Slice but whose B is not its length is a
             sub-range or a mistake and is printed as CHECK, never guessed.

★★★What the tool cannot see and the reader owes: a `Slice` local named `data`
or `length` inside a routine that subscripts a Slice (the ACTIVE field-capture
class), a Slice reaching a still-unconverted `pointer[char]` parameter (the
census class `h-slice-into-pointer`, run `--pointer-report` on the root
afterwards), and whether an `int` that BECAME `size_t` through `.length` is
ever compared against a negative value. The compiler is the arbiter for arity;
`tools/opcheck.py` old-against-new for everything else.

Usage: textslice.py <spec.py> [--apply]   (dry run prints the plan)
"""
import re, sys, os, collections

STR = re.compile(r'"(?:\\.|[^"\\])*"')

def code_only(line):
    """Comment stripped and string literals blanked, for matching."""
    # a `;` inside a string is not a comment
    out = []; i = 0; instr = False
    while i < len(line):
        c = line[i]
        if instr:
            if c == '\\': out.append('""'[:0]); i += 2; continue
            if c == '"': instr = False
            i += 1; continue
        if c == '"': instr = True; i += 1; continue
        if c == ';': break
        out.append(c); i += 1
    return ''.join(out)

def split_code_comment(line):
    """(code part as-is, comment tail) -- string literals kept in the code part."""
    i = 0; instr = False
    while i < len(line):
        c = line[i]
        if instr:
            if c == '\\': i += 2; continue
            if c == '"': instr = False
        elif c == '"': instr = True
        elif c == ';': return line[:i], line[i:]
        i += 1
    return line, ''

def balanced_end(s, start, open_ch='(', close_ch=')'):
    """Index of the bracket closing the one at `start`, string-aware."""
    depth = 0; i = start; instr = False
    while i < len(s):
        c = s[i]
        if instr:
            if c == '\\': i += 2; continue
            if c == '"': instr = False
        elif c == '"': instr = True
        elif c in '([': depth += 1
        elif c in ')]':
            depth -= 1
            if depth == 0: return i
        i += 1
    return -1

def split_top(s):
    """Top-level comma split, string- and bracket-aware."""
    parts = []; depth = 0; cur = []; instr = False; i = 0
    while i < len(s):
        c = s[i]
        if instr:
            cur.append(c)
            if c == '\\': cur.append(s[i+1]); i += 2; continue
            if c == '"': instr = False
            i += 1; continue
        if c == '"': instr = True
        elif c in '([': depth += 1
        elif c in ')]': depth -= 1
        elif c == ',' and depth == 0:
            parts.append(''.join(cur)); cur = []; i += 1; continue
        cur.append(c); i += 1
    parts.append(''.join(cur))
    return parts

# ---------------------------------------------------------------- routines
DECL = re.compile(r'^(\s*)(function|procedure|init|operator)\b')

def routine_at(lines, i):
    """(sig_text, sig_end_line, body_start, body_end) for the routine whose
    declaration begins at line i. Body = until the brace block closes, or the
    single-expression body line(s) after the header."""
    indent = len(lines[i]) - len(lines[i].lstrip())
    j = i; sig = lines[i]
    while balanced_end(code_only(sig), code_only(sig).find('(')) < 0 and j + 1 < len(lines):
        j += 1; sig += '\n' + lines[j]
    k = j + 1
    # skip blank/comment lines
    while k < len(lines) and code_only(lines[k]).strip() == '': k += 1
    if k < len(lines) and code_only(lines[k]).strip() == '{':
        depth = 0; m = k
        while m < len(lines):
            s = code_only(lines[m])
            depth += s.count('{') - s.count('}')
            if depth == 0: return sig, j, k, m
            m += 1
        return sig, j, k, len(lines) - 1
    # single-expression body: lines more indented than the header
    m = k
    while m + 1 < len(lines):
        nxt = lines[m + 1]
        if code_only(nxt).strip() == '': m += 1; continue
        if len(nxt) - len(nxt.lstrip()) > indent: m += 1; continue
        break
    return sig, j, k, m

def all_routines(lines):
    """[(name, start, sig_end, body_start, body_end)] in file order."""
    out = []
    for i, l in enumerate(lines):
        m = DECL.match(code_only(l))
        if not m: continue
        if re.search(r'\bextern\b', code_only(l)): continue
        sig, je, bs, be = routine_at(lines, i)
        nm = re.match(r'\s*(function|procedure|init|operator)\s*(\S*?)\s*\(', code_only(sig))
        out.append((nm.group(2) if nm else '?', i, je, bs, be))
    return out

def routine_containing(routines, li):
    best = None
    for r in routines:
        if r[1] <= li <= r[4]:
            if best is None or r[1] > best[1]: best = r
    return best

# ---------------------------------------------------------------- body rewrites
def sub_outside_strings(pattern, repl, code):
    """re.sub applied to the segments of `code` that are not string literals."""
    out = []; i = 0
    for m in STR.finditer(code):
        out.append(re.sub(pattern, repl, code[i:m.start()])); out.append(m.group(0)); i = m.end()
    out.append(re.sub(pattern, repl, code[i:]))
    return ''.join(out)

def rewrite_len_to_length(line, ln, ptr):
    code, cmt = split_code_comment(line)
    code = sub_outside_strings(rf'(?<![\w.]){re.escape(ln)}(?![\w(])', f'{ptr}.length', code)
    return code + cmt

def rewrite_derefs(line, ptr):
    code, cmt = split_code_comment(line)
    P = re.escape(ptr)
    # set *(ptr + e): v  ->  ptr.put(e, v)
    m = re.match(rf'(\s*)set\s+\*\(\s*{P}\s*\+', code)
    if m:
        op = code.index('(', m.start())
        end = balanced_end(code, op)
        if end > 0:
            idx = re.sub(rf'^\s*{P}\s*\+\s*', '', code[op + 1:end]).strip()
            cm = re.match(r'\s*:\s*(.*)$', code[end + 1:])
            if cm:
                val = rewrite_derefs(cm.group(1).rstrip(), ptr)   # `set *(out + a): *(out + b)` -- the value reads the same buffer
                return f'{m.group(1)}{ptr}.put({idx}, {val})' + cmt
    # set *ptr: v -> ptr.put(0, v)
    m3 = re.match(rf'(\s*)set\s+\*{P}(?![\w\[])\s*:\s*(.*)$', code)
    if m3:
        return f'{m3.group(1)}{ptr}.put(0, {rewrite_derefs(m3.group(2).rstrip(), ptr)})' + cmt
    # *(ptr + e) -> ptr[e]
    while True:
        m = re.search(rf'\*\(\s*{P}\s*\+\s*', code)
        if not m: break
        op = code.index('(', m.start())
        end = balanced_end(code, op)
        if end < 0: break
        idx = code[m.end():end].strip()
        code = code[:m.start()] + f'{ptr}[{idx}]' + code[end + 1:]
    # *ptr -> ptr[0]
    code = re.sub(rf'\*{P}(?![\w\[])', f'{ptr}[0]', code)
    return code + cmt

def rewrite_field_derefs(line, fields):
    """`*(X.f + e)` -> `X.f[e]`, `set *(X.f + e): v` -> `X.f.put(e, v)` for the converted
    slice FIELDS (any receiver chain) -- found the hard way: `*(state.token_value + i)`
    passed the planner and died in the emitter as `cannot cast this value to int`."""
    code, cmt = split_code_comment(line)
    F = '|'.join(map(re.escape, fields))
    RECV = rf'((?:[\w]+(?:\([^()]*\))?\.)*(?:{F}))'
    m = re.match(rf'(\s*)set\s+\*\(\s*{RECV}\s*\+', code)
    if m:
        op = code.index('(', m.start() + len(m.group(1)) + 3)
        end = balanced_end(code, op)
        if end > 0:
            idx = re.sub(rf'^\s*{re.escape(m.group(2))}\s*\+\s*', '', code[op + 1:end]).strip()
            cm = re.match(r'\s*:\s*(.*)$', code[end + 1:])
            if cm: return f'{m.group(1)}{m.group(2)}.put({idx}, {cm.group(1).rstrip()})' + cmt
    while True:
        m = re.search(rf'\*\(\s*{RECV}\s*\+\s*', code)
        if not m: break
        op = code.index('(', m.start())
        end = balanced_end(code, op)
        if end < 0: break
        idx = code[m.end():end].strip()
        code = code[:m.start()] + f'{m.group(1)}[{idx}]' + code[end + 1:]
    return code + cmt

def hazards(line, ptr):
    code = code_only(line)
    hits = []
    if re.search(rf'(?<![\w.]){re.escape(ptr)}\s*\+', code): hits.append('arith')
    if re.search(rf'(?<![\w.]){re.escape(ptr)}\s*-\s*\w', code): hits.append('arith-')
    if re.search(rf'Slice\[char\]\(\s*\w+\s*,\s*{re.escape(ptr)}\s*\)', code): hits.append('rewrap')
    if re.search(rf'\*\(\s*{re.escape(ptr)}\b', code): hits.append('deref')
    if re.search(rf'(?<![\w.])(let|var)\s+(data|length)\b', code): hits.append('local-data/length')
    return hits

# ---------------------------------------------------------------- main
class Spec:
    def __init__(self, d):
        self.root = d['root']
        self.params = d.get('params', [])        # (file, fn, line, ptr, len)
        self.fields = d.get('fields', [])        # (file, record, data, len, new)
        self.accessors = d.get('accessors', [])  # (qual_ptr_fn, qual_len_fn, new_qual_name or None)
        self.callees = d.get('callees', [])      # (pattern, old_arity, pair_index) -- derived below too
        self.records = d.get('records', {})      # record -> [(field_idx_of_data)]
        self.manual = d.get('manual', [])        # notes only
        self.retlen = d.get('retlen', [])          # (file, fn, line, out_len) -- `buf/out_len` routines that now RETURN the Slice
        self.retlen_int = d.get('retlen_int', [])  # (file, fn, line, out_data) -- `(…, out_data: ref[pointer[char]]) returns int` -> returns Slice[char]
        self.outpair = d.get('outpair', [])        # (file, fn, line, out_data, out_len, new) -- the two out-params become ONE `new: ref[Slice[char]]`
        self.method_concepts = d.get('method_concepts', [])   # concepts whose accessors are METHODS (any receiver spelling)
        self.ambiguous_fields = d.get('ambiguous_fields', []) # new field names that other records use for a non-slice
        self.slice_fields = d.get('slice_fields', [])         # field names ALREADY of type Slice[char] (a later chain's knowledge)

def load_spec(path):
    ns = {}
    exec(open(path).read(), ns)
    return Spec(ns['SPEC'])

def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    apply = '--apply' in sys.argv
    spec = load_spec(args[0])
    files = {}
    for root, dirs, fs in os.walk(spec.root):
        for f in sorted(fs):
            if f.endswith('.scaly'):
                p = os.path.join(root, f)
                files[p] = open(p, encoding='utf-8').read().split('\n')
    checks = []
    log = collections.Counter()

    retlen_fns = {}
    # ---- 1. field declarations
    field_pairs = {}   # data_field -> (len_field, new)
    for (fpath, rec, dfld, lfld, new) in spec.fields:
        files[fpath] = [l for l in files[fpath] if l is not None]
        lines = files[fpath]
        # find record
        ri = next(i for i, l in enumerate(lines) if re.match(rf'\s*define\s+{re.escape(rec)}\b', code_only(l)))
        di = next(i for i in range(ri, ri + 400) if re.match(rf'\s*{re.escape(dfld)}\s*:\s*pointer\[char\]\s*$', code_only(lines[i])))
        li = next(i for i in range(ri, ri + 400) if re.match(rf'\s*{re.escape(lfld)}\s*:\s*int\s*$', code_only(lines[i])))
        ind = re.match(r'\s*', lines[di]).group(0)
        lines[di] = f'{ind}{new}: Slice[char]'
        lines[li] = None
        field_pairs[dfld] = (lfld, new)
        log['fields'] += 1
    for p in files: files[p] = [l for l in files[p] if l is not None]

    # ---- 2. accessor declarations
    acc_pairs = {}   # qual_ptr -> (qual_len, new_qual)
    acc_new_names = set()
    for (qp, ql, newq) in spec.accessors:
        acc_pairs[qp] = (ql, newq or qp)     # ql may be None: a slice-returning routine with no length twin
        acc_new_names.add((newq or qp))
    # re-sign the accessor bodies
    for (qp, ql, newq) in spec.accessors:
        bare = qp.split('.')[-1]; newbare = (newq or qp).split('.')[-1]
        found = False
        for p, lines in files.items():
            for (nm, s, je, bs, be) in all_routines(lines):
                if nm != bare: continue
                sig = '\n'.join(lines[s:je + 1])
                if 'returns pointer[char]' not in sig: continue
                found = True
                lines[s] = lines[s].replace('returns pointer[char]', 'returns Slice[char]')
                if newbare != bare:
                    lines[s] = re.sub(rf'\b{re.escape(bare)}\s*\(', f'{newbare}(', lines[s], count=1)
                for k in range(bs, be + 1):
                    code, cmt = split_code_comment(lines[k])
                    code2 = re.sub(r'(?<![\w.])return\s+null\s*$', 'return Slice[char]()', code)
                    code2 = re.sub(r'^(\s*)null\s*$', r'\1Slice[char]()', code2)
                    lines[k] = code2 + cmt
                log['accessors'] += 1
        if not found: print(f'WARN accessor not found: {qp}')
    # the length twin keeps its name and answers `P(x).length as int`: done by hand (reader).

    # ---- 3. parameter declarations + bodies
    slice_params_by_routine = {}   # (file, start) -> set of slice-typed param names
    callee_specs = []               # (bare_name, kind, old_arity, [pair_indices]) kind: static/method/free/init
    by_routine = collections.defaultdict(list)
    for (fpath, fn, line, ptr, ln) in spec.params:
        by_routine[(fpath, fn, line)].append((ptr, ln))
    for (fpath, fn, line), pairs in by_routine.items():
        lines = files[fpath]
        # find the declaration near `line` (line numbers may have shifted by the field deletions)
        pat = rf'\s*init\s*\(' if fn == 'init' else rf'\s*(function|procedure)\s+{re.escape(fn)}\s*\('
        cands = [i for i in range(max(0, line - 40), min(len(lines), line + 5))
                 if re.match(pat, code_only(lines[i]))]
        if not cands: print(f'WARN routine not found: {fpath}:{line} {fn}'); continue
        i = min(cands, key=lambda c: abs(c - (line - 1)))
        sig, je, bs, be = routine_at(lines, i)
        code, cmt = split_code_comment(sig)
        op = code.index('('); cl = balanced_end(code, op)
        params = [p.strip() for p in split_top(code[op + 1:cl])]
        names = [p.split(':')[0].strip() for p in params]
        has_this = names and names[0] in ('this',)
        pair_idx = []; lenless_idx = []
        for (ptr, ln) in pairs:
            pi = names.index(ptr)
            assert re.match(rf'{re.escape(ptr)}\s*:\s*pointer\[char\]$', params[pi]), (fpath, fn, params[pi])
            params[pi] = f'{ptr}: Slice[char]'
            if ln is None:
                lenless_idx.append(pi - (1 if has_this else 0)); continue      # a bounded VIEW whose offsets stay explicit
            li_ = names.index(ln)
            assert re.match(rf'{re.escape(ln)}\s*:\s*int$', params[li_]), (fpath, fn, params[li_])
            assert li_ == pi + 1, (fpath, fn, ptr, ln, 'length not adjacent')
            params[li_] = None
            pair_idx.append(pi - (1 if has_this else 0))
        newsig = code[:op + 1] + ', '.join(p for p in params if p is not None) + code[cl:] + cmt
        # replace the (possibly multi-line) signature by one line
        del lines[i:je + 1]; lines.insert(i, newsig)
        shift = -(je - i)
        bs += shift; be += shift
        for (ptr, ln) in pairs:
            for k in range(bs, be + 1):
                if ln is not None: lines[k] = rewrite_len_to_length(lines[k], ln, ptr)
                lines[k] = rewrite_derefs(lines[k], ptr)
            # shadowing check
            for k in range(bs, be + 1):
                if ln is not None and re.search(rf'(?<![\w.])(let|var)\s+{re.escape(ln)}\b', code_only(lines[k])):
                    checks.append(f'{fpath}:{k+1}: SHADOW length name {ln} rebound in body of {fn}: {lines[k].strip()}')
            for k in range(bs, be + 1):
                for h in hazards(lines[k], ptr):
                    checks.append(f'{fpath}:{k+1}: {h} on {ptr} in {fn}: {lines[k].strip()}')
        kind = 'init' if fn == 'init' else ('method' if has_this else 'free')
        cname = fn
        if fn == 'init':
            kind = 'record'
            di = max(j for j in range(0, i) if re.match(r'\s*define\s+\w+', code_only(lines[j])))
            cname = re.match(r'\s*define\s+(\w+)', code_only(lines[di])).group(1)
        if pair_idx or lenless_idx: callee_specs.append((cname, kind, len(names) - (1 if has_this else 0), sorted(pair_idx), fpath, i, sorted(lenless_idx)))
        log['params'] += len(pairs)

    # ---- 3b. retlen routines: drop `out_len: ref[int]`, return Slice[char]
    for (fpath, fn, line, oln) in spec.retlen:
        lines = files[fpath]
        cands = [i for i in range(max(0, line - 60), min(len(lines), line + 5))
                 if re.match(rf'\s*(function|procedure)\s+{re.escape(fn)}\s*\(', code_only(lines[i]))]
        if not cands: print(f'WARN retlen not found {fn}'); continue
        i = min(cands, key=lambda c: abs(c - (line - 1)))
        sig, je, bs, be = routine_at(lines, i)
        code, cmt = split_code_comment(sig)
        op = code.index('('); cl = balanced_end(code, op)
        params = [p_.strip() for p_ in split_top(code[op + 1:cl])]
        names = [p_.split(':')[0].strip() for p_ in params]
        has_this = names[0] == 'this'
        oi = names.index(oln)
        assert re.match(rf'{re.escape(oln)}\s*:\s*ref\[int\]$', params[oi]), (fn, params[oi])
        params[oi] = None
        rest = code[cl:].replace('returns pointer[char]', 'returns Slice[char]')
        newsig = code[:op + 1] + ', '.join(p_ for p_ in params if p_ is not None) + rest + cmt
        del lines[i:je + 1]; lines.insert(i, newsig)
        shift = -(je - i); bs += shift; be += shift
        retlen_fns[fn] = (len(names) - (1 if has_this else 0), oi - (1 if has_this else 0))
        pending = None; k = bs
        while k <= be:
            cd, cm = split_code_comment(lines[k])
            m = re.match(rf'^(\s*)set\s+{re.escape(oln)}\s*:\s*(.*)$', cd)
            if m:
                pending = (m.group(2).strip(), m.group(1)); del lines[k]; be -= 1; continue
            m = re.match(r'^(\s*)(return\s+)?(.+?)\s*$', cd)
            if m and pending is not None and cd.strip() and not re.match(r'^\s*[{}]\s*$', cd):
                ind, ret, expr = m.group(1), m.group(2) or '', m.group(3)
                E, _ = pending
                if expr == 'null':
                    assert E == '0', (fn, k, E)
                    lines[k] = f'{ind}{ret}Slice[char]()' + cm
                elif re.match(r'^\w+$', E) or re.match(r'^[\w.]+\([^()]*\)$', E) and not re.search(r'\bjsnum_', E):
                    lines[k] = f'{ind}{ret}Slice[char]({E}, {expr})' + cm
                else:
                    lines.insert(k, f'{ind}let out_n {E}'); k += 1; be += 1
                    lines[k] = f'{ind}{ret}Slice[char](out_n, {expr})' + cm
                pending = None
            k += 1
        log['retlen'] += 1

    # ---- 3c. retlen_int routines: `(…, out_data: ref[pointer[char]]) returns int` -> `returns Slice[char]`
    def find_decl(lines, fn, line, pat):
        cands = [i for i in range(max(0, line - 80), min(len(lines), line + 5)) if re.match(pat, code_only(lines[i]))]
        if not cands: cands = [i for i in range(len(lines)) if re.match(pat, code_only(lines[i]))]
        return min(cands, key=lambda c: abs(c - (line - 1))) if cands else None
    def outarg_matches(expr, name):
        """`f(a, b, name)` -> `f(a, b)` if `name` is a bare top-level argument; else None."""
        m = re.match(r'^([\w.]+)\(', expr)
        if not m or balanced_end(expr, expr.index('(')) != len(expr) - 1: return None
        op = expr.index('('); args = split_top(expr[op + 1:-1])
        if not any(a.strip() == name for a in args): return None
        return expr[:op + 1] + ', '.join(a.strip() for a in args if a.strip() != name) + ')'
    for (fpath, fn, line, oname) in spec.retlen_int:
        lines = files[fpath]
        i = find_decl(lines, fn, line, rf'\s*(function|procedure)\s+{re.escape(fn)}\s*\(')
        if i is None: print(f'WARN retlen_int not found {fn}'); continue
        sig, je, bs, be = routine_at(lines, i)
        code, cmt = split_code_comment(sig)
        op = code.index('('); cl = balanced_end(code, op)
        params = [p_.strip() for p_ in split_top(code[op + 1:cl])]
        names = [p_.split(':')[0].strip() for p_ in params]
        has_this = names[0] == 'this'
        oi = names.index(oname)
        assert re.match(rf'{re.escape(oname)}\s*:\s*ref\[pointer\[char\]\]$', params[oi]), (fn, params[oi])
        params[oi] = None
        rest = re.sub(r'returns\s+int\b', 'returns Slice[char]', code[cl:])
        newsig = code[:op + 1] + ', '.join(p_ for p_ in params if p_ is not None) + rest + cmt
        del lines[i:je + 1]; lines.insert(i, newsig)
        shift = -(je - i); bs += shift; be += shift
        retlen_fns[fn] = (len(names) - (1 if has_this else 0), oi - (1 if has_this else 0))   # same caller machinery as retlen
        pending = None; k = bs
        while k <= be:
            cd, cm = split_code_comment(lines[k])
            m = re.match(rf'^(\s*)set\s+{re.escape(oname)}\s*:\s*(.*)$', cd)
            if m:
                pending = m.group(2).strip(); del lines[k]; be -= 1; continue
            m = re.match(r'^(\s*)(return\s+)?(.+?)\s*$', cd)
            if m and cd.strip() and not re.match(r'^\s*[{}]\s*$', cd):
                ind, ret, expr = m.group(1), m.group(2) or '', m.group(3)
                last_code = max((x for x in range(bs, be + 1) if code_only(lines[x]).strip() not in ('', '}')), default=be)
                is_tail = (k == last_code)
                fwd = outarg_matches(expr, oname)
                if fwd is not None:
                    lines[k] = f'{ind}{ret}{fwd}' + cm
                elif pending is not None and (ret or is_tail):
                    lines[k] = f'{ind}{ret}Slice[char]({expr}, {pending})' + cm
                    pending = None
            k += 1
        log['retlen_int'] += 1

    # ---- 3d. outpair routines: `out_data: ref[pointer[char]], out_len: ref[int]` -> `new: ref[Slice[char]]`
    outpair_fns = {}   # bare fn -> (old_arity, d_idx, l_idx, new)
    outpair_bodies = []
    for (fpath, fn, line, od, ol, new) in spec.outpair:
        lines = files[fpath]
        i = find_decl(lines, fn, line, rf'\s*(function|procedure)\s+{re.escape(fn)}\s*\(')
        if i is None: print(f'WARN outpair not found {fn}'); continue
        sig, je, bs, be = routine_at(lines, i)
        code, cmt = split_code_comment(sig)
        op = code.index('('); cl = balanced_end(code, op)
        params = [p_.strip() for p_ in split_top(code[op + 1:cl])]
        names = [p_.split(':')[0].strip() for p_ in params]
        has_this = names[0] == 'this'
        di = names.index(od); li_ = names.index(ol)
        assert re.match(rf'{re.escape(od)}\s*:\s*ref\[pointer\[char\]\]$', params[di]), (fn, params[di])
        assert re.match(rf'{re.escape(ol)}\s*:\s*ref\[int\]$', params[li_]), (fn, params[li_])
        params[di] = f'{new}: ref[Slice[char]]'; params[li_] = None
        newsig = code[:op + 1] + ', '.join(p_ for p_ in params if p_ is not None) + code[cl:] + cmt
        del lines[i:je + 1]; lines.insert(i, newsig)
        shift = -(je - i); bs += shift; be += shift
        outpair_fns[fn] = (len(names) - (1 if has_this else 0), di - (1 if has_this else 0), li_ - (1 if has_this else 0), new)
        outpair_bodies.append((fpath, fn, od, ol, new, i))
        # forwarding calls `g(..., od, ol)` inside the body are rewritten by the call pass (bare names -> new)
        log['outpair'] += 1

    # forwarding calls between retlen routines and every caller: handled in the call pass via retlen_fns

    # ---- 4. record positional constructions as callees
    for rec, (ar, idxs) in spec.records.items():
        callee_specs.append((rec, 'record', ar, idxs, None, None, []))

    # ---- 5. call sites (and field/accessor reads) in every file
    conv_fns = collections.defaultdict(list)    # bare name -> [(kind, old_arity, pair_idx)]
    for (nm, kind, ar, idxs, fpath, i, lenless) in callee_specs:
        if nm is None: continue
        conv_fns[nm].append((kind, ar, idxs, lenless))
    acc_len_of = {}   # QUALIFIED len fn -> (bare ptr fn NEW name, qualified len fn); plus bare for METHOD accessors
    for qp, (ql, newq) in acc_pairs.items():
        if ql is None: continue
        acc_len_of[ql] = (newq.split('.')[-1], ql)
        if ql.split('.')[0] in spec.method_concepts: acc_len_of[ql.split('.')[-1]] = (newq.split('.')[-1], ql)
    acc_rename = {qp.split('.')[-1]: newq.split('.')[-1] for qp, (ql, newq) in acc_pairs.items() if qp != newq}

    def norm(s): return re.sub(r'\s+', '', s)

    def is_slice_expr(e, locals_, strict=False):
        """strict: an AMBIGUOUS field (`.name` is also an AST record's node
        field) does not count -- used where a wrong yes rewrites a null test."""
        e = e.strip()
        if e in locals_ and locals_[e][0] == 'slice': return True
        if e in locals_ and locals_[e][0] == 'slice?': return not strict
        if re.match(r'^Slice\[char\]\(', e): return True
        if re.search(r'\.(subslice|slice_from|as_slice)\([^()]*\)$', e): return True
        m = re.match(r'^([\w.\[\]()]+)\.(\w+)$', e)
        if m and m.group(2) in unamb_fields: return True
        if m and m.group(2) in AMBIGUOUS: return not strict
        m = re.match(r'^([\w.]+)\((.*)\)$', e)
        if m and balanced_end(e, e.index('(')) == len(e) - 1 and acc_call_is_slice(m.group(1)): return True
        return False

    def length_source(b, locals_):
        """The slice expression whose length `b` is, or None."""
        b = b.strip()
        b = re.sub(r'^\((.*)\)$', r'\1', b) if b.startswith('(') and balanced_end(b, 0) == len(b) - 1 else b
        b = re.sub(r'\s+as\s+(int|size_t)$', '', b).strip()
        m = re.match(r'^\(?(.+?)\.length(\s+as\s+(int|size_t))?\)?$', b)
        if m and not re.search(r'[+\-*/]', m.group(1)): return m.group(1).strip()
        m = re.match(r'^([\w.]+)\((.*)\)$', b)
        if m and balanced_end(b, b.index('(')) == len(b) - 1 and '.' not in m.group(1):
            hits = [v for k_, v in acc_len_of.items() if k_.split('.')[-1] == m.group(1)]
            if hits: return f'{hits[0][0]}({m.group(2)})'
        if m and balanced_end(b, b.index('(')) == len(b) - 1 and '.' in m.group(1):
            q, bare = m.group(1).rsplit('.', 1)
            key = m.group(1) if m.group(1) in acc_len_of else (bare if (q[0].islower() and bare in acc_len_of) else None)
            if key is not None:
                return f'{q}.{acc_len_of[key][0]}({m.group(2)})'
        m = re.match(r'^([\w.\[\]()]+)\.(\w+)$', b)
        if m:
            for dfld, (lfld, new) in field_pairs.items():
                if m.group(2) == lfld: return f'{m.group(1)}.{new}'
        if b in locals_ and locals_[b][0] == 'len': return locals_[b][1]
        return None

    acc_new_names_bare = {v[1].split('.')[-1] for v in acc_pairs.values()}
    acc_new_qual = {v[1] for v in acc_pairs.values()}
    method_acc_bare = {v[1].split('.')[-1] for v in acc_pairs.values() if v[1].split('.')[0] in spec.method_concepts}
    AMBIGUOUS = set(spec.ambiguous_fields)
    unamb_fields = ({v[1] for v in field_pairs.values()} | set(spec.slice_fields)) - AMBIGUOUS

    def acc_call_is_slice(name):
        """`Q.f` names a slice-returning accessor: exact for a concept static,
        bare for a method (`scanner.token_value`, `this.token_value`)."""
        if '.' not in name: return name in acc_new_names_bare   # a sibling static called bare inside its own concept
        q, f = name.rsplit('.', 1)
        if q[0].isupper() and '.' not in q: return name in acc_new_qual
        return f in method_acc_bare or f in retlen_fns

    def same_slice(a_s, src, locals_):
        if src is None: return False
        if norm(src) == norm(a_s): return True
        if a_s in locals_ and locals_[a_s][0] in ('slice', 'slice?') and norm(locals_[a_s][1]) == norm(src): return True
        if a_s in locals_ and src in locals_ and locals_[src][0] in ('slice', 'slice?') and norm(locals_[src][1]) == norm(locals_[a_s][1]): return True
        return False

    def collapse(a, b, locals_, where):
        a_s = a.strip(); b_s = b.strip()
        a_s = re.sub(r'^("(?:\\.|[^"\\])*")\s+as\s+pointer\[char\]$', r'\1', a_s)
        md = re.match(r'^([\w.]+)\.data$', a_s)
        if md and is_slice_expr(md.group(1), locals_) and norm(length_source(b_s, locals_) or '') == norm(md.group(1)):
            return md.group(1), 'collapse'
        if re.match(r'^"(?:\\.|[^"\\])*"$', a_s):
            lit = a_s[1:-1]; declen = len(re.sub(r'\\.', 'x', lit))
            if b_s != str(declen): checks.append(f'{where}: LITERAL-LENGTH a={a_s} b={b_s} decoded={declen}')
            return a_s, 'literal'
        if re.match(r'^null(\s+as\s+pointer\[char\])?$', a_s):
            if b_s != '0': checks.append(f'{where}: NULL-WITH-LENGTH a={a_s} b={b_s}')
            return 'Slice[char]()', 'empty'
        src = length_source(b_s, locals_)
        if is_slice_expr(a_s, locals_):
            if src is not None and (norm(src) == norm(a_s) or (a_s in locals_ and locals_[a_s][0] == 'slice' and norm(locals_[a_s][1]) == norm(src))):
                return a_s, 'collapse'
            if src is not None and a_s in locals_ and src in locals_ and locals_[src][0] == 'slice' and norm(locals_[src][1]) == norm(locals_[a_s][1]):
                return a_s, 'collapse'
            checks.append(f'{where}: SLICE-WITH-FOREIGN-LENGTH a={a_s} b={b_s}')
            return a_s, 'check'
        # pointer-shaped a
        return f'Slice[char]({b_s}, {a_s})', 'wrap'

    def rewrite_call_line(code, locals_, where):
        """Rewrite every converted call on one code line (no comment).
        ★A DECLARATION line is never rewritten: its parameter list parses like an
        argument list of the same arity (callfix.py's lesson, and this tool deleted
        a parameter from `get_property_name_from_type`'s header before the guard)."""
        if re.match(r'^\s*(function|procedure|init|operator)\b', code): return code
        changed = True
        guard = 0
        pos = 0
        out = code
        while True:
            m = re.compile(r'(?<![\w])(&?)([A-Za-z_][\w.]*)(\[[^\]]*\])?(\^\w+)?\s*\(').search(out, pos)
            if not m: break
            name = m.group(2); bare = name.split('.')[-1]
            if m.group(3) and bare != 'Slice': pos = m.end(); continue
            op = out.index('(', m.end() - 1)
            end = balanced_end(out, op)
            if end < 0: pos = m.end(); continue
            args = split_top(out[op + 1:end]) if out[op + 1:end].strip() else []
            specs = conv_fns.get(bare, [])
            if bare == 'Slice' and m.group(3) == '[char]' and len(args) == 2:
                args = [rewrite_call_line(a, locals_, where) for a in args]
                a_s = args[1].strip(); b_s = args[0].strip()
                # ★A literal may replace the wrap ONLY where a declared target type is in hand; the RHS
                # of an UNTYPED binding is not one -- `let nm "NaN"` binds a String (CLAUDE.md), and
                # three such bindings cost six stage-2 units (empty names, NaN, "module").
                untyped_binding = (re.match(r'^\s*(let|var)\s+\w+\s+Slice\[char\]\(', out) is not None and m.start() == re.match(r'^\s*(let|var)\s+\w+\s+', out).end()) or re.match(r'^\s*set\s+[\w.]+\s*:\s*Slice\[char\]\(', out) is not None
                if re.match(r'^"', a_s) and untyped_binding:
                    out = out[:op + 1] + ','.join(args) + out[end:]
                    pos = op + 1; continue
                if is_slice_expr(a_s, locals_) or re.match(r'^"', a_s) or re.match(r'^null', a_s):
                    na, how = collapse(a_s, b_s, locals_, where)
                    if how in ('collapse', 'literal', 'empty'):
                        log['rewrap-' + how] += 1
                        out = out[:m.start()] + na + out[end + 1:]
                        pos = m.start() + len(na); continue
                    if is_slice_expr(a_s, locals_) and re.match(r'^\w+$', a_s):
                        # `Slice[char](n, buf)` over a scratch VIEW: the first n elements -- read at the site
                        na = f'{a_s}.subslice(0, {b_s} as size_t)'
                        checks.append(f'{where}: REWRAP-PREFIX {na}  (read: is `{b_s}` the written length of `{a_s}`?)')
                        log['rewrap-prefix'] += 1
                        out = out[:m.start()] + na + out[end + 1:]
                        pos = m.start() + len(na); continue
                    checks.append(f'{where}: REWRAP a={a_s} b={b_s}')
                out = out[:op + 1] + ','.join(args) + out[end:]
                pos = op + 1; continue
            if bare == 'Slice': pos = op + 1; continue
            if bare in retlen_fns and len(args) == retlen_fns[bare][0]:
                ar_, oi_ = retlen_fns[bare]
                args = [rewrite_call_line(a, locals_, where) for a in args]
                la = args[oi_].strip()
                lm = re.match(r'^&(\w+)$', la)
                if lm: retlen_callers.append((where, lm.group(1)))
                elif re.match(r'^\w+$', la): pass   # forwarding an out_len that no longer exists
                else: checks.append(f'{where}: RETLEN unexpected length argument {la}')
                del args[oi_]
                out = out[:op + 1] + ','.join(args) + out[end:]
                log['retlen-call'] += 1
                pos = op + 1; continue
            if bare in outpair_fns and len(args) == outpair_fns[bare][0]:
                ar_, di_, li_, new_ = outpair_fns[bare]
                args = [rewrite_call_line(a, locals_, where) for a in args]
                da = args[di_].strip(); la = args[li_].strip()
                dm = re.match(r'^&(\w+)$', da); lm = re.match(r'^&(\w+)$', la)
                if dm and lm:
                    outpair_callers.append((where, dm.group(1), lm.group(1)))
                    args[di_] = ' &' + dm.group(1) if args[di_].startswith(' ') else '&' + dm.group(1)
                elif re.match(r'^\w+$', da) and re.match(r'^\w+$', la) and cur_outpair_new is not None:
                    args[di_] = (' ' if args[di_].startswith(' ') else '') + cur_outpair_new   # forwarding the routine's own out-param
                else:
                    checks.append(f'{where}: OUTPAIR unexpected arguments {da}, {la} of {name}')
                del args[li_]
                out = out[:op + 1] + ','.join(args) + out[end:]
                log['outpair-call'] += 1
                pos = op + 1; continue
            if bare == 'String' and '.' not in name and len(args) == 2:
                args = [rewrite_call_line(a, locals_, where) for a in args]
                a_s = args[0].strip(); b_s = args[1].strip()
                if is_slice_expr(a_s, locals_):
                    src = length_source(b_s, locals_)
                    if same_slice(a_s, src, locals_):
                        out = out[:op + 1] + f'{a_s}.data, {a_s}.length' + out[end:]
                        log['string-boundary'] += 1
                    else:
                        checks.append(f'{where}: STRING-SLICE-FOREIGN-LENGTH a={a_s} b={b_s}')
                pos = op + 1; continue
            if bare == 'append' and '.' in name and len(args) == 2:
                args = [rewrite_call_line(a, locals_, where) for a in args]
                a_s = args[0].strip(); b_s = args[1].strip()
                if is_slice_expr(a_s, locals_):
                    src = length_source(b_s, locals_)
                    if same_slice(a_s, src, locals_):
                        out = out[:op + 1] + f'{a_s}.data, {a_s}.length' + out[end:]
                        log['append-boundary'] += 1
                    else:
                        checks.append(f'{where}: APPEND-SLICE-FOREIGN-LENGTH a={a_s} b={b_s}')
                pos = op + 1; continue
            # record construction?  Name( or Name^page( or &Name^page(
            chosen = None
            for (kind, ar, idxs, lenless) in specs:
                if kind == 'record':
                    if name == bare and bare[0].isupper() and (ar is None or len(args) == ar):
                        chosen = (kind, idxs, lenless); break
                    continue
                if ar is not None and len(args) != ar: continue
                if kind == 'method' and '.' not in name: continue
                if kind == 'free' and '.' in name and not name.split('.')[-2][0].isupper(): continue
                if kind == 'static' and '.' not in name: continue
                chosen = (kind, idxs, lenless); break
            if chosen is None:
                # recurse into args? handled by scanning forward (pos = op+1)
                pos = op + 1; continue
            kind, idxs, lenless = chosen
            # first rewrite nested calls inside the args
            args = [rewrite_call_line(a, locals_, where) for a in args]
            # ★A POINTER handed to a Slice parameter of a STATIC or free routine COMPILES (rc 0,
            # probed 2026-09-05): only a METHOD call is type-checked at the argument. A lenless
            # conversion touches no call site, so every such argument is READ here.
            for li_ in lenless:
                if li_ < len(args):
                    a_s = args[li_].strip(); lead = re.match(r'\s*', args[li_]).group(0)
                    am = re.match(r'^&(\w+)\[0\](\s+as\s+pointer\[char\])?$', a_s)
                    if am and am.group(1) in stack_arrays:
                        n_, ty_ = stack_arrays[am.group(1)]
                        cast_ = ' as pointer[char]' if ty_ != 'char' else ''
                        args[li_] = f'{lead}Slice[char]({n_} as size_t, &{am.group(1)}[0]{cast_})'; log['stack-array-view'] += 1; continue
                    pm_ = re.match(r'^([\w.]+)\s*\+\s*(.+)$', a_s)
                    if pm_ and is_slice_expr(pm_.group(1), locals_):
                        args[li_] = f'{lead}{pm_.group(1)}.slice_from({pm_.group(2).strip()} as size_t)'; log['slice-from'] += 1; continue
                    if not (is_slice_expr(a_s, locals_) or re.match(r'^Slice\[char\]\(', a_s) or re.match(r'^"', a_s)):
                        checks.append(f'{where}: POINTER-INTO-SLICE? arg {li_} `{a_s}` of {name}')
            newargs = []; skip = set()
            for k, idx in enumerate(idxs):
                if idx + 1 >= len(args): break
                na, how = collapse(args[idx], args[idx + 1], locals_, where)
                # preserve leading whitespace style
                lead = re.match(r'\s*', args[idx]).group(0)
                args[idx] = lead + na; skip.add(idx + 1)
                log['call-' + how] += 1
            newargs = [a for j, a in enumerate(args) if j not in skip]
            repl = out[:op + 1] + ','.join(newargs) + out[end:]
            out = repl
            pos = op + 1
        return out

    # ---- 3d (body): the out-pair stores, once length_source is defined
    for (fpath, fn, od, ol, new, i0) in outpair_bodies:
        lines = files[fpath]
        i = find_decl(lines, fn, i0 + 1, rf'\s*(function|procedure)\s+{re.escape(fn)}\s*\(')
        sig, je, bs, be = routine_at(lines, i)
        k = bs
        while k <= be:
            cd, cm = split_code_comment(lines[k])
            m1 = re.match(rf'^(\s*)set\s+{re.escape(od)}\s*:\s*(.*)$', cd)
            m2 = re.match(rf'^(\s*)set\s+{re.escape(ol)}\s*:\s*(.*)$', cd)
            if m1:
                # partner: the next code line must be `set ol: B`
                j = k + 1
                while j <= be and code_only(lines[j]).strip() == '': j += 1
                pm = re.match(rf'^(\s*)set\s+{re.escape(ol)}\s*:\s*(.*)$', code_only(lines[j])) if j <= be else None
                a_s = m1.group(2).strip()
                if pm is None:
                    checks.append(f'{fpath}:{k+1}: OUTPAIR store of {od} without a {ol} partner in {fn}: {cd.strip()}'); k += 1; continue
                b_s = pm.group(2).strip(); ind = m1.group(1)
                # ★The escape checker refuses `set out: <construction>` and `set out: <call result>` through
                # a `ref[Slice[char]]` ("reference into a local page escapes via store" -- it cannot see that
                # a Slice does not own), and accepts a plain PATH (field, parameter) and the two MEMBER
                # stores. So: one store where the pair collapses to a path, else the members.
                md = re.match(r'^(.+)\.data$', a_s)
                src = length_source(b_s, {})
                pnames = set(names)
                if md and src is not None and norm(src) == norm(md.group(1)) and (re.match(r'^\w+\.[\w.]+$', md.group(1)) or md.group(1) in pnames):
                    lines[k] = f'{ind}set {new}: {md.group(1)}' + cm; del lines[j]; be -= 1
                else:
                    lines[k] = f'{ind}set {new}.data: {a_s}' + cm
                    lines[j] = f'{ind}set {new}.length: {b_s}' + cm
                log['outpair-store'] += 1
            elif m2:
                # `set ol: f(..., od)` -- a retlen_int forward: bind the returned view, store its members
                fwd = outarg_matches(m2.group(2).strip(), od)
                if fwd is not None:
                    ind = m2.group(1)
                    prev = max((z for z in range(bs, k) if code_only(lines[z]).strip()), default=None)
                    braced = prev is not None and re.match(r'^\s*(if|else|while|for)\b', code_only(lines[prev])) is not None and not code_only(lines[prev]).rstrip().endswith('{')
                    body_ = [f'{ind}let out_v {fwd}' + cm, f'{ind}set {new}.data: out_v.data', f'{ind}set {new}.length: out_v.length']
                    if braced:
                        pind = ind[:-4] if len(ind) >= 4 else ''
                        body_ = [f'{pind}{{'] + body_ + [f'{pind}}}']
                    lines[k:k + 1] = body_
                    be += len(body_) - 1; k += len(body_) - 1
                    log['outpair-forward'] += 1
                else:
                    checks.append(f'{fpath}:{k+1}: OUTPAIR lone store of {ol} in {fn}: {cd.strip()}')
            k += 1
    retlen_callers = []
    outpair_callers = []   # (where, X, Y): `f(..., &X, &Y)` -> `f(..., &X)`; X becomes the Slice local, Y its `.length`
    cur_outpair_new = None
    # local tables per routine while scanning a file top to bottom
    BIND = re.compile(r'^\s*(let|var)\s+(\w+)(?:\s*:\s*([^\s]+(?:\[[^\]]*\])?))?\s+(.*)$')
    for p, lines in files.items():
        routines = all_routines(lines)
        # -- pre-pass A: `set X.NEW: A` + `set X.NEW.length: B` (adjacent) -> one store
        newf = {v[1] for v in field_pairs.values()}
        k = 0
        while k + 1 < len(lines):
            c1 = code_only(lines[k]); c2 = code_only(lines[k + 1])
            hit = None
            for dfld, (lfld, new) in field_pairs.items():
                m1 = re.match(rf'^(\s*)set\s+((?:[\w.]+\.)?){re.escape(dfld)}\s*:\s*(.*)$', c1)
                m2 = re.match(rf'^(\s*)set\s+((?:[\w.]+\.)?){re.escape(lfld)}\s*:\s*(.*)$', c2)
                if m1 and m2 and m1.group(2) == m2.group(2): hit = (m1, m2, new); break
            if hit:
                m1, m2, new = hit
                a_s = m1.group(3).strip(); b_s = m2.group(3).strip()
                if a_s.startswith('null'): na = 'Slice[char]()'
                elif is_slice_expr(a_s, {}) and norm(length_source(b_s, {}) or '') == norm(a_s): na = a_s
                else: na = f'Slice[char]({b_s}, {a_s})'
                lines[k] = f'{m1.group(1)}set {m1.group(2)}{new}: {na}'
                del lines[k + 1]
                checks.append(f'{p}:{k+1}: FIELD-STORE-PAIR {m1.group(2)}{new} <- {na}  (read: is the length the data\'s?)')
                log['field-store-pair'] += 1
            k += 1
        routines = all_routines(lines)
        # slice params per routine
        rparams = {}
        for (nm, s, je, bs, be) in routines:
            sig = code_only('\n'.join(lines[s:je + 1]))
            rparams[s] = set(re.findall(r'(\w+)\s*:\s*Slice\[char\]', sig))
        rptr = {}
        for (nm, s_, je, bs, be) in routines:
            sig = code_only('\n'.join(lines[s_:je + 1]))
            rptr[s_] = set(re.findall(r'(\w+)\s*:\s*ref\[pointer\[char\]\]', sig))
            for k in range(bs, be + 1):
                mm = re.match(r'\s*(let|var)\s+(\w+)\s*:\s*pointer\[char\](?![\w])', code_only(lines[k])) or re.match(r'\s*(let|var)\s+(\w+)\s+null\s+as\s+pointer\[char\]\s*$', code_only(lines[k]))
                if mm: rptr[s_].add(mm.group(2))
        cur = None; locals_ = {}; ptr_targets = set(); slice_locals = set(); len_alias = {}
        for k in range(len(lines)):
            r = routine_containing(routines, k)
            if r is not cur:
                cur = r; locals_ = {}; ptr_targets = set(); slice_locals = set(); len_alias = {}
                cur_outpair_new = None
                stack_arrays = {}
                if r:
                    for x_ in range(r[3], r[4] + 1):
                        am_ = re.match(r'\s*var\s+(\w+)\s+(char|u8)\[(\w+)\]\s*$', code_only(lines[x_]))
                        if am_: stack_arrays[am_.group(1)] = (am_.group(3), am_.group(2))
                    sig_ = code_only('\n'.join(lines[r[1]:r[2] + 1]))
                    mo = re.search(r'(\w+)\s*:\s*ref\[Slice\[char\]\]', sig_)
                    if mo: cur_outpair_new = mo.group(1)
                    ptr_targets = rptr.get(r[1], set())
                    for nm_ in rparams.get(r[1], ()): locals_[nm_] = ('slice', nm_)
                    # -- pre-pass B (per routine): pointer locals whose every store is a slice, paired with a length local
                    nm0, s0, je0, bs0, be0 = r
                    body = [code_only(lines[x]) for x in range(bs0, be0 + 1)]
                    for pl in list(ptr_targets):
                        decl = [x for x, c in enumerate(body) if re.match(rf'\s*var\s+{re.escape(pl)}\s*:\s*pointer\[char\]\s+null\s*$', c)]
                        if len(decl) != 1: continue
                        stores = [x for x, c in enumerate(body) if re.match(rf'\s*set\s+{re.escape(pl)}\s*:', c)]
                        if not stores: continue
                        partner = None; ok = True; partner_lines = []
                        for x in stores:
                            rhs = re.match(rf'\s*set\s+{re.escape(pl)}\s*:\s*(.*)$', body[x]).group(1).strip()
                            if not is_slice_expr(rhs, locals_): ok = False; break
                            # partner: the next code line `set L: <length of rhs>`
                            y = x + 1
                            while y < len(body) and body[y].strip() == '': y += 1
                            pm = re.match(r'\s*set\s+(\w+)\s*:\s*(.*)$', body[y]) if y < len(body) else None
                            if not pm: ok = False; break
                            src = length_source(pm.group(2).strip(), locals_)
                            if src is None or norm(src) != norm(rhs): ok = False; break
                            if partner is None: partner = pm.group(1)
                            elif partner != pm.group(1): ok = False; break
                            partner_lines.append(y)
                        if not ok or partner is None: continue
                        pdecl = [x for x, c in enumerate(body) if re.match(rf'\s*var\s+{re.escape(partner)}\s+0\s*$', c)]
                        other_sets = [x for x, c in enumerate(body) if re.match(rf'\s*set\s+{re.escape(partner)}\s*:', c) and x not in partner_lines]
                        if len(pdecl) != 1 or other_sets: 
                            checks.append(f'{p}:{bs0 + decl[0] + 1}: PTR-LOCAL {pl} stores slices but partner {partner} is not clean (decl {len(pdecl)}, other sets {len(other_sets)})')
                            continue
                        ind = re.match(r'\s*', lines[bs0 + decl[0]]).group(0)
                        lines[bs0 + decl[0]] = f'{ind}var {pl} Slice[char]()'
                        for y in sorted(partner_lines + pdecl, reverse=True):
                            lines[bs0 + y] = '\x00DELETE'
                        for x in range(bs0, be0 + 1):
                            if lines[x] == '\x00DELETE': continue
                            cd_, cm_ = split_code_comment(lines[x])
                            lines[x] = sub_outside_strings(rf'(?<![\w.]){re.escape(partner)}(?![\w(])', f'{pl}.length', cd_) + cm_
                        ptr_targets.discard(pl); locals_[pl] = ('slice', pl); slice_locals.add(pl)
                        log['ptr-local-pair'] += 1
            if lines[k] == '\x00DELETE': continue
            code, cmt = split_code_comment(lines[k])
            orig = code
            for sl in slice_locals:
                code = rewrite_derefs(code, sl)
                for h in hazards(code, sl):
                    checks.append(f'{p}:{k+1}: {h} on slice local {sl} in {cur[0] if cur else "?"}: {code.strip()}')
            # field reads: .F -> .NEW ; .L -> .NEW.length
            for dfld, (lfld, new) in field_pairs.items():
                code = re.sub(rf'\.{re.escape(dfld)}(?![\w])', f'.{new}', code)
                code = re.sub(rf'\.{re.escape(lfld)}(?![\w])', f'.{new}.length', code)
            code = rewrite_field_derefs(code, [v[1] for v in field_pairs.values()])
            # accessor renames
            for old, new in acc_rename.items():
                code = re.sub(rf'\.{re.escape(old)}\s*\(', f'.{new}(', code)
            # `X = null` / `X <> null` on a slice-valued accessor call or slice local -> .data
            def nulltest(m):
                e = m.group(1)
                if is_slice_expr(e, locals_, strict=True):
                    log['nulltest'] += 1
                    return f'{e}.data {m.group(2)} null'
                if is_slice_expr(e, locals_):
                    checks.append(f'{p}:{k+1}: AMBIGUOUS-NULLTEST {e} {m.group(2)} null')
                return m.group(0)
            # ★`slice <> null` COMPILES: `=`/`<>` desugar to `.equals()` on a concept that declares
            # one, and Slice does -- so a missed null test is a synthesized `Slice.operator <>`
            # that dereferences the null it was handed (SIGSEGV, found by the stage-1 dumper).
            code = re.sub(r'((?:[\w.]+\([^()]*(?:\([^()]*\))?[^()]*\))|(?:(?<![\w.])[\w.]+))\s+(=|<>)\s+null\b', nulltest, code)
            # calls
            n_before = len(retlen_callers)
            code = rewrite_call_line(code, locals_, f'{p}:{k+1}')
            if len(retlen_callers) > n_before:
                # a retlen(_int) call on this line: if the statement binds the LENGTH (`let Y f(...)` /
                # `set Y: f(...)`) and the out-arg was `&X`, the statement becomes `set X: f(...)`
                where_, X_ = retlen_callers[-1]
                sm2 = re.match(r'^(\s*)(?:let\s+(\w+)\s+|set\s+(\w+)\s*:\s*)(.*)$', code)
                if sm2 and re.match(r'^[\w.]+\(', sm2.group(4).strip()) and balanced_end(sm2.group(4).strip(), sm2.group(4).strip().index('(')) == len(sm2.group(4).strip()) - 1:
                    Y_ = sm2.group(2) or sm2.group(3)
                    if Y_ != X_ and not re.search(r'\.length\b', Y_):
                        code = f'{sm2.group(1)}set {X_}: {sm2.group(4).strip()}'
                        retlen_callers.pop()
                        outpair_callers.append((where_, X_, Y_ if sm2.group(3) else '=' + Y_))   # '=Y': a let, no decl to delete
                        log['retlen-int-caller'] += 1
            # `set X: <slice>` where X is a pointer-typed local or a ref[pointer[char]] out-param: boundary, `.data`
            sm = re.match(r'^(\s*set\s+)(\*?\w+)\s*:\s*(.*)$', code)
            if sm and cur is not None:
                tgt = sm.group(2).lstrip('*'); rhs = sm.group(3).strip()
                rm = re.match(r'^[\w.]+\.(\w+)\(', rhs)
                if tgt in ptr_targets and is_slice_expr(rhs, locals_) and not (rm and rm.group(1) in retlen_fns):
                    code = f'{sm.group(1)}{sm.group(2)}: {rhs}.data'
                    checks.append(f'{p}:{k+1}: BOUNDARY set {tgt}: <slice>.data  (chain 2: {cur[0]})')
                    log['set-boundary'] += 1
            # local bindings: record kind
            bm = BIND.match(code)
            if bm:
                nm_, ty, init = bm.group(2), bm.group(3), bm.group(4).strip()
                if ty == 'Slice[char]' or (ty is None and is_slice_expr(init, locals_, strict=True)):
                    locals_[nm_] = ('slice', init); slice_locals.add(nm_)
                elif ty is None and is_slice_expr(init, locals_):
                    locals_[nm_] = ('slice?', init)
                else:
                    src = length_source(init, locals_)
                    if src is not None: locals_[nm_] = ('len', src)
                    elif nm_ in locals_: del locals_[nm_]
            if code != orig:
                lines[k] = code + cmt
                log['lines'] += 1

    # ---- 5a. pointer locals whose stores are all slice-valued now (a retlen_int result, a `.data`
    # boundary with its `set Y: <len>` partner, or a raw pointer with a partner length): one Slice var.
    for p, lines in files.items():
        for (nm, s_, je, bs, be) in all_routines(lines):
            # ★code_only DROPS string literals -- the first cut rewrote `f(host, "default")` into `f(host, )`;
            # match on the comment-free code WITH its strings.
            body = {x: split_code_comment(lines[x])[0] for x in range(bs, be + 1) if lines[x] != '\x00DELETE'}
            for x, c in list(body.items()):
                dm = re.match(r'(\s*)var\s+(\w+)\s*:\s*pointer\[char\]\s+null\s*$', c) or re.match(r'(\s*)var\s+(\w+)\s+null\s+as\s+pointer\[char\]\s*$', c)
                if not dm: continue
                pl = dm.group(2); ind = dm.group(1)
                stores = [y for y, cc in body.items() if re.match(rf'\s*set\s+{re.escape(pl)}\s*:', cc)]
                if not stores: continue
                plan = []; partner = None; ok = True
                sl_ = {}
                for y in stores:
                    rhs = re.match(rf'\s*set\s+{re.escape(pl)}\s*:\s*(.*)$', body[y]).group(1).strip()
                    nxt = min((z for z in body if z > y and body[z].strip()), default=None)
                    pm = re.match(r'\s*set\s+(\w+)\s*:\s*(.*)$', body[nxt]) if nxt is not None else None
                    if is_slice_expr(rhs, sl_) or re.match(r'^[\w.]+\.\w+\([^()]*\)$', rhs) and acc_call_is_slice(rhs[:rhs.index('(')]) or (rhs.split('(')[0].split('.')[-1] in retlen_fns):
                        plan.append((y, rhs, None)); continue
                    md = re.match(r'^(.+)\.data$', rhs)
                    if pm and md and norm(length_source(pm.group(2).strip(), sl_) or '') == norm(md.group(1)):
                        if partner is None: partner = pm.group(1)
                        if partner != pm.group(1): ok = False; break
                        plan.append((y, md.group(1), nxt)); continue
                    if pm and re.match(r'^[\w.\[\]()&+ ]+$', rhs) and re.match(r'^[\w.() +-]+$', pm.group(2).strip()) and pm.group(1) != pl:
                        if partner is None: partner = pm.group(1)
                        if partner != pm.group(1): ok = False; break
                        plan.append((y, f'Slice[char]({pm.group(2).strip()}, {rhs})', nxt)); continue
                    ok = False; break
                if not ok: 
                    checks.append(f'{p}:{x+1}: PTR-LOCAL2 {pl} in {nm}: a store the pass cannot type'); continue
                if partner is not None:
                    pdecl = [y for y, cc in body.items() if re.match(rf'\s*var\s+{re.escape(partner)}\s*(:\s*int)?\s+0\s*$', cc)]
                    others = [y for y, cc in body.items() if re.match(rf'\s*set\s+{re.escape(partner)}\s*:', cc) and y not in [q for (_, _, q) in plan]]
                    if len(pdecl) != 1 or others:
                        checks.append(f'{p}:{x+1}: PTR-LOCAL2 {pl} in {nm}: partner {partner} not clean (decl {len(pdecl)}, other sets {len(others)})'); continue
                    lines[pdecl[0]] = '\x00DELETE'
                lines[x] = f'{ind}var {pl} Slice[char]()'
                for (y, rhs2, q) in plan:
                    lines[y] = re.match(r'\s*', lines[y]).group(0) + f'set {pl}: {rhs2}'
                    if q is not None: lines[q] = '\x00DELETE'
                for z in range(bs, be + 1):
                    if lines[z] == '\x00DELETE': continue
                    cd_, cm_ = split_code_comment(lines[z])
                    if partner is not None:
                        cd_ = sub_outside_strings(rf'(?<![\w.]){re.escape(partner)}(?![\w(])', f'{pl}.length', cd_)
                    mm2 = re.match(rf'^(\s*)set\s+([\w.]+)\.data:\s*{re.escape(pl)}\s*$', cd_)
                    if mm2 and z + 1 <= be and re.match(rf'^\s*set\s+{re.escape(mm2.group(2))}\.length:\s*{re.escape(pl)}\.length\s*$', code_only(lines[z + 1])):
                        cd_ = f'{mm2.group(1)}set {mm2.group(2)}: {pl}'; lines[z + 1] = '\x00DELETE'
                    cd_ = re.sub(rf'(?<![\w.]){re.escape(pl)}\s+(=|<>)\s+null\b', rf'{pl}.data \1 null', cd_)
                    cd_ = re.sub(rf'Slice\[char\]\(\s*{re.escape(pl)}\.length\s*,\s*{re.escape(pl)}\s*\)', pl, cd_)
                    cd_ = re.sub(rf'(?<![\w.]){re.escape(pl)}\s+as\s+pointer\[', f'{pl}.data as pointer[', cd_)
                    cd_ = rewrite_derefs(cd_, pl)
                    lines[z] = cd_ + cm_
                log['ptr-local-pair2'] += 1

    # ---- 5b. retlen callers: `let nd f(..., &nl)` -> the local `nl` is `nd.length` from its
    # declaration to the end of the BLOCK that declares it (a routine may declare `nl` twice
    # in sibling blocks); a `var nd: pointer[char] null` receiver becomes `var nd Slice[char]()`.
    # Runs while the DELETE placeholders still hold the numbering the call pass recorded.
    def block_end(lines, start):
        depth = 0
        for x in range(start, len(lines)):
            c = code_only(lines[x]) if lines[x] != '\x00DELETE' else ''
            depth += c.count('{') - c.count('}')
            if depth < 0: return x - 1
        return len(lines) - 1
    jobs = []
    for (where, ln_) in retlen_callers:
        fp, li = where.rsplit(':', 1); li = int(li) - 1
        cd0 = code_only(files[fp][li])
        bm = re.match(r'^\s*(let|var)\s+(\w+)\s', cd0) or re.match(r'^\s*set\s+(\w+)\s*:', cd0)
        if not bm: checks.append(f'{where}: RETLEN caller without a binding for {ln_}: {cd0.strip()}'); continue
        jobs.append((where, bm.group(bm.lastindex), ln_))
    for (where, X_, Y_) in outpair_callers: jobs.append((where, X_, Y_))
    done_jobs = set()
    for (where, bound, ln_) in jobs:
        fp, li = where.rsplit(':', 1); li = int(li) - 1
        lines = files[fp]
        is_let = ln_.startswith('=')
        ln_ = ln_.lstrip('=')
        rs = all_routines(lines); r = routine_containing(rs, li)
        if not r:
            # a program ROOT's top-level statements: the scope is the rest of the file
            r = ('<top>', 0, 0, li, len(lines) - 1)
        nm0, s0, je0, bs0, be0 = r
        if not is_let and not any(re.search(rf'(?<![\w.]){re.escape(ln_)}(?![\w(])', code_only(lines[x])) for x in range(bs0, be0 + 1) if lines[x] != '\x00DELETE'):
            log['out-caller-already-clean'] += 1; continue   # the length local was already gone (an earlier chain)
        if is_let:
            d0 = li; e0 = block_end(lines, li + 1)
            other = [x for x in range(li + 1, e0 + 1) if re.match(rf'\s*(set|let|var)\s+{re.escape(ln_)}\b', code_only(lines[x]))]
            if other:
                checks.append(f'{where}: OUT caller: length `{ln_}` is rebound later in the block'); continue
        else:
            decls = [x for x in range(bs0, li) if re.match(rf'\s*var\s+{re.escape(ln_)}\s*(:\s*int)?\s+0\s*$', code_only(lines[x]))]
            if not decls:
                checks.append(f'{where}: OUT caller local {ln_} has no `var {ln_} 0` before the call'); continue
            d0 = decls[-1]; e0 = block_end(lines, d0)
            if (fp, d0) in done_jobs: continue      # a second call in the same block, same receiver pair
            done_jobs.add((fp, d0))
            other = [x for x in range(d0, e0 + 1) if re.match(rf'\s*set\s+{re.escape(ln_)}\s*:', code_only(lines[x]))]
            if other:
                checks.append(f'{where}: OUT caller local {ln_} is also assigned in its block'); continue
        pd = [x for x in range(bs0, li + 1) if re.match(rf'\s*var\s+{re.escape(bound)}\s*:\s*pointer\[char\]\s+null\s*$|\s*var\s+{re.escape(bound)}\s+null\s+as\s+pointer\[char\]\s*$', code_only(lines[x]))]
        if pd:
            ind = re.match(r'\s*', lines[pd[-1]]).group(0)
            lines[pd[-1]] = f'{ind}var {bound} Slice[char]()'
        elif not is_let and not re.search(rf'\b(let|var)\s+{re.escape(bound)}\b', '\n'.join(code_only(l) for l in lines[bs0:li] if l != '\x00DELETE')):
            checks.append(f'{where}: OUT caller: receiver `{bound}` has no `var {bound}: pointer[char] null` in reach')
        if not is_let: lines[d0] = '\x00DELETE'
        for x in range(d0, e0 + 1):
            if lines[x] == '\x00DELETE': continue
            cd_, cm_ = split_code_comment(lines[x])
            cd_ = sub_outside_strings(rf'(?<![\w.]){re.escape(ln_)}(?![\w(])', f'{bound}.length', cd_)
            cd_ = re.sub(rf'(?<![\w.]){re.escape(bound)}\s+(=|<>)\s+null\b', rf'{bound}.data \1 null', cd_)
            cd_ = re.sub(rf'Slice\[char\]\(\s*{re.escape(bound)}\.length\s*,\s*{re.escape(bound)}\s*\)', bound, cd_)
            cd_ = re.sub(rf'(?<![\w.]){re.escape(bound)}\s+as\s+pointer\[', f'{bound}.data as pointer[', cd_)
            cd_ = rewrite_derefs(cd_, bound)
            lines[x] = cd_ + cm_
        # second sweep: `set X.data: bound` + `set X.length: bound.length` -> `set X: bound` (the partner line
        # is only in its final form once the substitution above has passed it)
        for x in range(d0, e0 + 1):
            if lines[x] == '\x00DELETE': continue
            cd_, cm_ = split_code_comment(lines[x])
            mm2 = re.match(rf'^(\s*)set\s+([\w.]+)\.data:\s*{re.escape(bound)}\s*$', cd_)
            if mm2 and x + 1 <= e0 and re.match(rf'^\s*set\s+{re.escape(mm2.group(2))}\.length:\s*{re.escape(bound)}\.length\s*$', code_only(lines[x + 1])):
                lines[x] = f'{mm2.group(1)}set {mm2.group(2)}: {bound}' + cm_; lines[x + 1] = '\x00DELETE'
        log['retlen-caller'] += 1
    for p, lines in files.items():
        files[p] = [l for l in lines if l != '\x00DELETE']

    # ---- 5c. FINAL hazard scan on the text as it will be written: every routine's slice-typed
    # names (Slice[char] params, `var x Slice[char]()`, `let x <slice expr>`) against the
    # three silent shapes -- `Slice[char](n, x)`, `x + i`, `*(x + i)` / `*x`.
    final = []
    for p, lines in files.items():
        for (nm, s_, je, bs, be) in all_routines(lines):
            sig = code_only('\n'.join(lines[s_:je + 1]))
            sl = dict((n_, ('slice', n_)) for n_ in re.findall(r'(\w+)\s*:\s*Slice\[char\]', sig))
            for k in range(bs, be + 1):
                cd = code_only(lines[k])
                for x in list(sl):
                    if re.search(rf'Slice\[char\]\(\s*[^,()]+,\s*{re.escape(x)}\s*\)', cd): final.append(f'{p}:{k+1}: FINAL rewrap {x}: {cd.strip()}')
                    if re.search(rf'(?<![\w.]){re.escape(x)}\s*\+', cd): final.append(f'{p}:{k+1}: FINAL arith {x}: {cd.strip()}')
                    if re.search(rf'\*\(?\s*{re.escape(x)}(?![\w])', cd): final.append(f'{p}:{k+1}: FINAL deref {x}: {cd.strip()}')
                bm = re.match(r'^\s*(let|var)\s+(\w+)(?:\s*:\s*(\S+))?\s+(.*)$', cd)
                if bm:
                    n_, ty, init = bm.group(2), bm.group(3), bm.group(4).strip()
                    if ty == 'Slice[char]' or (ty is None and is_slice_expr(init, sl)): sl[n_] = ('slice', init)
                    elif n_ in sl: del sl[n_]
                FF = '|'.join(map(re.escape, [v[1] for v in field_pairs.values()]))
                if re.search(rf'(?<![\w.])[\w.]+\.({FF})\s+(=|<>)\s+null\b', cd): final.append(f'{p}:{k+1}: FINAL field-nulltest (synthesized Slice equals!): {cd.strip()}')
                for x in list(sl):
                    if re.search(rf'(?<![\w.]){re.escape(x)}\s+(=|<>)\s+null\b', cd): final.append(f'{p}:{k+1}: FINAL nulltest on slice {x}: {cd.strip()}')
                    if re.search(rf'(?<![\w.]){re.escape(x)}\s+as\s+pointer', cd): final.append(f'{p}:{k+1}: FINAL slice-as-pointer {x}: {cd.strip()}')
                for mm in re.finditer(r'([\w.]+)\(([^()]*)\)\s+as\s+pointer', cd):
                    if acc_call_is_slice(mm.group(1)): final.append(f'{p}:{k+1}: FINAL slice-as-pointer (accessor call): {cd.strip()}')
                if re.search(rf'\*\(?\s*[\w.]+\.({FF})(?![\w])', cd): final.append(f'{p}:{k+1}: FINAL field-deref: {cd.strip()}')
                if re.search(rf'[\w.]+\.({FF})\s*\+', cd): final.append(f'{p}:{k+1}: FINAL field-arith: {cd.strip()}')
                if re.search(rf'Slice\[char\]\(\s*[^,()]+,\s*[\w.]+\.({FF})\s*\)', cd): final.append(f'{p}:{k+1}: FINAL field-rewrap: {cd.strip()}')
    checks.extend(final)

    # ---- 6. report / write
    for c in checks: print('CHECK', c)
    print('---', dict(log))
    if apply:
        for p, lines in files.items():
            open(p, 'w', encoding='utf-8').write('\n'.join(lines))
        print('written')

if __name__ == '__main__':
    main()

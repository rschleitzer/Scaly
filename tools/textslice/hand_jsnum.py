#!/usr/bin/env python3
"""Hand edits after textslice.py for the jsnum write-buffer chain (spec_jsnum.py)."""
R = 'packages/tscaly/0.1.0/tscaly/'
def edit(f, pairs):
    p = R + f; s = open(p).read()
    for entry in pairs:
        old, new, why = entry[:3]; want = entry[3] if len(entry) > 3 else 1
        n = s.count(old); assert n == want, (f, old[:60], n, why)
        s = s.replace(old, new)
    open(p, 'w').write(s)
edit('LitCheck.scaly', [
 ('Checker.write_pseudo_big_int_value(out, Slice[char](m, dec + start), false)', 'Checker.write_pseudo_big_int_value(out, dec.subslice(start as size_t, (start + m) as size_t), false)',
  'the digits are a sub-range of the scratch view, not a re-wrap'),
])
edit('checker.scaly', [
 ('        let scratch String^host(((n + (n / 4)) + 4) as size_t)\n        let out scratch.get_buffer()\n',
  '        let cap ((n + (n / 4)) + 4) as size_t\n        let scratch String^host(cap)\n        let out Slice[char](cap, scratch.get_buffer() as pointer[char])\n',
  'bigint_literal_arm: the page scratch buffer with the capacity it was allocated with'),
])
edit('LitCheck.scaly', [
 ('        let digits String^host((n + 8) as size_t)\n        let dec digits.get_buffer()\n',
  '        let digits String^host((n + 8) as size_t)\n        let dec Slice[char]((n + 8) as size_t, digits.get_buffer() as pointer[char])\n',
  'the lit-check scratch for the decimal digits, with the capacity it was allocated with'),
])
edit('checker.scaly', [
 ('this.get_fresh_type_of_literal_type(this.get_bigint_literal_type(Slice[char](m, out + start), negative))',
  'this.get_fresh_type_of_literal_type(this.get_bigint_literal_type(out.subslice(start as size_t, (start + m) as size_t), negative))',
  'the digits are a sub-range of the scratch view; `out + start` on a Slice COMPILES (arithmetic on a struct) and answered garbage in batch mode -- 16 stage-2 units'),
])
edit('scanner.scaly', [
 ('        let scratch String^this(((n + (n / 4)) + 4) as size_t)\n        let out scratch.get_buffer()\n',
  '        let cap ((n + (n / 4)) + 4) as size_t\n        let scratch String^this(cap)\n        let out Slice[char](cap, scratch.get_buffer() as pointer[char])\n',
  'the scanner\'s bigint scratch buffer with its capacity'),
])
# the three scratch views declared by hand above: their remaining raw reads
import re
def fix_derefs(f, routine, name):
    p = R + f; s = open(p).read()
    i = s.index(routine); j = s.find('\n    procedure ', i + 1); j2 = s.find('\n    function ', i + 1)
    j = min(x for x in (j, j2, len(s)) if x > 0)
    body = s[i:j]
    body2 = re.sub(rf'\*\({name} \+ ([^()]+)\)', rf'{name}[\1]', body)
    body2 = re.sub(rf'\*{name}\b(?!\[)', f'{name}[0]', body2)
    assert body2 != body or '*(' + name not in body, (f, routine)
    # the hand-declared view must not be walked as a pointer anywhere in its routine
    assert not re.search(rf'(?<![\w.]){name}\s*\+', body2), (f, routine, 'arithmetic on the view')
    assert not re.search(rf'\*\(?\s*{name}(?![\w])', body2), (f, routine, 'deref of the view')
    open(p, 'w').write(s[:i] + body2 + s[j:])
fix_derefs('scanner.scaly', 'procedure set_bigint_token_value(this)', 'out')
fix_derefs('checker.scaly', 'function bigint_literal_arm(this', 'out')
fix_derefs('LitCheck.scaly', 'procedure write_scan(host', 'dec')
print('hand edits applied')

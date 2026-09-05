#!/usr/bin/env python3
"""Hand edits after textslice.py for the tscaly names chain -- the sites the
tool prints as CHECK and the reader decided. Exact-string replacements, so a
re-run from a clean checkout reproduces the tree; every entry says why."""
import sys, re
R = 'packages/tscaly/0.1.0/tscaly/'
def edit(f, pairs):
    p = R + f; s = open(p).read()
    for entry in pairs:
        old, new, why = entry[:3]; want = entry[3] if len(entry) > 3 else 1
        n = s.count(old)
        assert n == want, (f, old[:60], n, why)
        s = s.replace(old, new)
    open(p, 'w').write(s)

edit('binder.scaly', [
 ('remove_file_extension(Slice[char](file_name_len, file_name_data))', 'remove_file_extension(file_name)',
  'unqualified field pair inside a Binder method: the tool only sees `.field`'),
 ('set *(buf + 1 + i): *(file_name_data + i)', 'set *(buf + 1 + i): file_name[i]', 'same, the walk'),
])
edit('checker.scaly', [
 ('    function literal_text_of(t: ref[Type]) returns Slice[char]\n    {\n        Checker.literal_text_slice_of(t).data\n    }',
  '    function literal_text_of(t: ref[Type]) returns Slice[char]\n    {\n        Checker.literal_text_slice_of(t)\n    }',
  'the pointer accessor over an existing slice accessor: return the slice'),
 ('out.append(nd2 + 1, (nd2.length - 1) as size_t)', 'out.append(nd2.data + 1, (nd2.length - 1) as size_t)',
  'StringBuilder.append(pointer, len) is the stdlib boundary; the +1 skips the minus sign'),
 ('out.append(s + i, size as size_t)', 'out.append(s.data + i, size as size_t)', 'same boundary, one decoded rune'),
 ('jsnum_decode_rune(s.data, s.length as int, i, out_size)', 'jsnum_decode_rune(s, s.length as int, i, out_size)',
  'jsnum_decode_rune takes the view now; stop stays an explicit bound'),
 ('define SwitchWitness\n(\n    data: pointer[char]\n    length: int\n)', 'define SwitchWitness\n(\n    text: Slice[char]\n)',
  'a (data, length) record IS a slice; `.data` is too common a name for the tool to rewrite'),
 ('SymbolTable.names_equal(Slice[char](p.length, p.data), d)', 'SymbolTable.names_equal(p.text, d)', 'SwitchWitness reader'),
 ('Checker.typeof_ne_facts(Slice[char](w.length, w.data))', 'Checker.typeof_ne_facts(w.text)', 'SwitchWitness reader'),
 ('this.narrow_type_by_type_name(t, Slice[char](w.length, w.data))', 'this.narrow_type_by_type_name(t, w.text)', 'SwitchWitness reader'),
 ('            set jsx_namespace_name: "React"\n', '            set jsx_namespace_name: Slice[char](5, "React")\n',
  'a literal materialises into a Slice PARAMETER, not into a set on a Slice local (CLAUDE.md: untyped positions)'),
 ('if Checker.name_bytes_are(Slice[char](7, d), "@types/")', 'if Checker.name_bytes_are(d.subslice(0, 7), "@types/")',
  'a prefix test over a slice: the sub-range, never a re-wrap (guarded by d.length > 7 above)'),
 ('        var access_data name_data\n        var access_len name_len\n', '        var access_data name_data\n',
  'new_flow_access_name: the access range is a sub-slice of the name'),
 ('                    set access_data: name_data + ((at + 1) as size_t)\n                    set access_len: name_len - (at + 1)\n',
  '                    set access_data: name_data.slice_from((at + 1) as size_t)\n', 'same'),
 ('Checker.write_ascii_escaped_literal_name(out, Slice[char](nl - 2, nd + 1))', 'Checker.write_ascii_escaped_literal_name(out, nd.subslice(1, (nl - 1) as size_t))',
  'the module name without its quotes: bytes 1 .. nl-1'),
 ('            set *(buf + i): d[i]\n            set i: i + 1\n        }\n        buf\n    }\n\n    ; addDeclarationToLateBoundSymbol',
  '            set *(buf + i): d[i]\n            set i: i + 1\n        }\n        Slice[char](d.length, buf)\n    }\n\n    ; addDeclarationToLateBoundSymbol', 'same'),
])
edit('checker.scaly', [
 ('    function text_equals(a: Slice[char], b: Slice[char]) returns bool\n    {\n        var i 0\n',
  '    function text_equals(a: Slice[char], b: Slice[char]) returns bool\n    {\n        if a.length <> b.length\n            return false\n        var i 0\n',
  'both sides carry a length now: equality is equality (every caller checked the lengths first, measured)'),
 ('Checker.bytes_equal(ss.get_buffer() as pointer[char], 0, ts.get_buffer() as pointer[char], 0, start_len)',
  'Checker.bytes_equal(Slice[char](ss.get_length(), ss.get_buffer() as pointer[char]), 0, Slice[char](ts.get_length(), ts.get_buffer() as pointer[char]), 0, start_len)',
  'String buffers into the bounded view; String.as_slice() answers Slice[u8], so the wrap is explicit'),
 ('Checker.bytes_equal(se.get_buffer() as pointer[char], (se.get_length() as int) - end_len, te.get_buffer() as pointer[char], (te.get_length() as int) - end_len, end_len)',
  'Checker.bytes_equal(Slice[char](se.get_length(), se.get_buffer() as pointer[char]), (se.get_length() as int) - end_len, Slice[char](te.get_length(), te.get_buffer() as pointer[char]), (te.get_length() as int) - end_len, end_len)', 'same'),
 ('Checker.bytes_equal(source_start.get_buffer() as pointer[char], 0, target_start.get_buffer() as pointer[char], 0, ts_len)',
  'Checker.bytes_equal(Slice[char](source_start.get_length(), source_start.get_buffer() as pointer[char]), 0, Slice[char](target_start.get_length(), target_start.get_buffer() as pointer[char]), 0, ts_len)', 'same'),
 ('Checker.bytes_equal(source_end.get_buffer() as pointer[char], se_len - te_len, target_end.get_buffer() as pointer[char], 0, te_len)',
  'Checker.bytes_equal(Slice[char](source_end.get_length(), source_end.get_buffer() as pointer[char]), se_len - te_len, Slice[char](target_end.get_length(), target_end.get_buffer() as pointer[char]), 0, te_len)', 'same'),
 ('if Checker.bytes_equal(&nb[0], 0, sd, 0, sn) = false', 'if Checker.bytes_equal(Slice[char](JSNUM_MAX_TEXT as size_t, &nb[0]), 0, sd, 0, sn) = false',
  'the stack scratch buffer with its declared capacity'),
 ('if Checker.bytes_equal(Checker.literal_text_of(right), 0, s.data, 0, s.length as int)', 'if Checker.bytes_equal(Checker.literal_text_of(right), 0, s, 0, s.length as int)', 'a slice taken apart'),
 ('if Checker.bytes_equal(txt.get_buffer() as pointer[char], 0, s.data, 0, s.length as int)', 'if Checker.bytes_equal(Slice[char](txt.get_length(), txt.get_buffer() as pointer[char]), 0, s, 0, s.length as int)', 'same'),
])
edit('scanner.scaly', [
 ('        Scanner.utf8_size_in(buffer.data, end, p)\n', '        Scanner.utf8_size_in(buffer, end, p)\n', 'the source view itself; `end` stays the explicit bound'),
 ('        Scanner.utf8_decode_in(buffer.data, end, p)\n', '        Scanner.utf8_decode_in(buffer, end, p)\n', 'same'),
])
edit('checker.scaly', [
 ('if Checker.text_equals(source_name, Slice[char](target_len, target_name)) = false',
  'if Checker.text_equals(Slice[char](source_len, source_name), Slice[char](target_len, target_name)) = false',
  'is_matching_reference, both access arms: a POINTER local into the converted Slice parameter compiled silently (the static-call gap, probed 2026-09-05) and read garbage as its length', 2),
])
edit('checker.scaly', [
 ('let s String^host(AstNode.literal_text_of(n) as pointer[const_char], len as size_t)', 'let s String^host(AstNode.literal_text_of(n).data as pointer[const_char], len as size_t)',
  'a Slice cast `as pointer[const_char]` COMPILES (the struct is coerced like a String value) and the String then copies the length word as its first byte -- template literal types read `\\u0001a\\u0001` for `xay`'),
 ('let v String^host(Checker.literal_text_of(source) as pointer[const_char], Checker.literal_text_length_of(source) as size_t)', 'let v String^host(Checker.literal_text_of(source).data as pointer[const_char], Checker.literal_text_length_of(source) as size_t)', 'same'),
])
edit('jsnum.scaly', [
 ('if not dec_set(&d, Slice[char](len, s + off))', 'if not dec_set(&d, s.subslice(off as size_t, (off + len) as size_t))',
  'the trimmed range of the input'),
])
print('hand edits applied')

#!/usr/bin/env python3
"""R2: mark every property the write census reports as written after its
object was published as `var`.

Usage: tools/purify/mark_var.py <union.txt> [--apply] [--generated-too]

The input is the union tools/write-report.sh writes. A site line reads
`<file>:<line>:<col>: w-let <routine> writes <Record>.<field> through <what>`,
located at the STORE. The record's declaration is found in the package the
site belongs to (`define <Record>` with a field list), and `var ` is put in
front of the field's name there -- once per (record, field), whatever the
number of sites. A field that already says `var` or `let` is left alone.

The mark is an attribute in the plan and the emitter reads it nowhere, so the
change must leave every root's emission byte-identical; the caller proves that
with `cmp`, as R1's marks were proven.

Refused, each listed with its reason:
  generated   one of the generated files (its generator owns the text)
  ambiguous   the record name is declared in more than one file of the package
  missing     no `define <Record>` with a field list, or no such field in it
Without --apply nothing is written; --generated-too marks the generated files as
well, as the target their generators must reproduce.
"""
import glob
import os
import re
import sys
from collections import defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from relabel import is_generated   # noqa: E402

SITE = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): w-let (?P<fn>\S+) '
                  r'writes (?P<record>[A-Za-z0-9_]+)\.(?P<field>[a-z_0-9]+) through ')


def package_of(path):
    # a site's file may be spelled absolute (a dependency file remembered on
    # first sight) or relative: one key per package either way -- the absolute
    # spelling once made a second key for one file, and the file was written
    # twice, the second time against text the first had already moved
    m = re.search(r'(packages/[^/]+/[^/]+)/', path)
    return m.group(1) if m else os.path.dirname(path)


def find_declaration(pkg, record):
    """(file, start, end) of the ONE `define <record>` field list in the package,
    or a reason string."""
    hits = []
    root = pkg if pkg is not None else 'packages'
    for f in glob.glob(os.path.join(root, '**', '*.scaly'), recursive=True):
        if '/interface/' in f:
            continue
        text = open(f, encoding='utf-8', errors='surrogateescape').read()
        # a record may be nested in a namespace body (`define Planner { define Planner (...) }`),
        # so the line may be indented
        for m in re.finditer(r'^[ \t]*define %s(?:\[[^\]]*\])?[ \t]*\n?[ \t]*\(' % re.escape(record), text, re.M):
            hits.append((f, m.end() - 1))
    if not hits and pkg is not None:
        # the record lives in another package (a stdlib Slice written by a port,
        # opensp's TextChunk written by dazzle): the mark belongs to its declarer
        return find_declaration(None, record)
    if not hits:
        return 'missing'
    if len(hits) > 1:
        return 'ambiguous'
    return hits[0]


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    apply = '--apply' in sys.argv
    generated_too = '--generated-too' in sys.argv
    wanted = defaultdict(set)          # (pkg, record) -> fields
    for line in open(args[0], encoding='utf-8', errors='surrogateescape'):
        m = SITE.match(line.rstrip('\n'))
        if m:
            wanted[(package_of(m.group('file')), m.group('record'))].add(m.group('field'))
    marked = refused = 0
    reasons = defaultdict(list)
    edits = defaultdict(list)          # file -> [(offset, insert)]
    # ★Group by the DECLARATION, not by the writer's package: a stdlib record
    # written from three packages is three keys here and ONE field list there,
    # and computing its edits three times against one text wrote the second
    # and third sets against a text the first had already moved.
    by_decl = defaultdict(set)
    for (pkg, record), fields in sorted(wanted.items()):
        decl = find_declaration(pkg, record)
        if isinstance(decl, str):
            refused += len(fields)
            reasons[decl].append(f'{pkg} {record}')
            continue
        by_decl[(decl, record)] |= fields
    for (decl, record), fields in sorted(by_decl.items()):
        f, open_paren = decl
        if is_generated(f) and not generated_too:
            refused += len(fields)
            reasons['generated'].append(f'{f} {record}')
            continue
        text = open(f, encoding='utf-8', errors='surrogateescape').read()
        # the field list runs to the matching close paren; a `;` comment inside
        # it may carry an unbalanced paren and is skipped to its line's end
        depth = 0
        i = open_paren
        while i < len(text):
            c = text[i]
            if c == ';':
                i = text.find('\n', i)
                if i < 0:
                    i = len(text)
                continue
            if c == '(':
                depth += 1
            elif c == ')':
                depth -= 1
                if depth == 0:
                    break
            i += 1
        body = text[open_paren:i]
        for field in sorted(fields):
            m = re.search(r'(^|[ \t(])((?:let|var)[ \t]+)?%s[ \t]*:' % re.escape(field), body, re.M)
            if not m:
                refused += 1
                reasons['missing'].append(f'{f} {record}.{field}')
                continue
            if m.group(2):
                continue                    # already says let or var
            at = open_paren + m.start() + len(m.group(1))
            # a record written from several packages resolves to ONE declaration
            # (the fallback above): one mark per field, not one per writer
            if (at, 'var ') in edits[f]:
                continue
            edits[f].append((at, 'var '))
            marked += 1
    # the verification runs in the dry run as well, so a refusal is known
    # before anything is written
    for f, ins in edits.items():
        if apply:
            pass
        text = open(f, encoding='utf-8', errors='surrogateescape').read()
        if True:
            # ★Every insertion point is VERIFIED against the text before anything
            # is written: the first run put `var ` four bytes into `size_t`
            # (`length: sizvar e_t`) and nothing said so until the seed's parser
            # died on the file. An offset that does not sit right before a field
            # name refuses the whole file.
            bad = [at for at, _ in ins if not re.match(r'[a-z_][a-z_0-9]*[ \t]*:', text[at:at + 80])]
            if bad:
                print(f"mark_var: REFUSED {f}: {len(bad)} insertion point(s) do not sit before a field name")
                continue
            if not apply:
                continue
            for at, s in sorted(ins, reverse=True):
                text = text[:at] + s + text[at:]
            with open(f, 'w', encoding='utf-8', errors='surrogateescape') as out:
                out.write(text)
    print(f"mark_var: {marked} fields marked in {len(edits)} files"
          f"{'' if apply else ' (dry run)'}, {refused} refused")
    for why, items in sorted(reasons.items()):
        print(f"  {why}: {len(items)}")
        for it in items[:8]:
            print(f"    {it}")


if __name__ == '__main__':
    main()

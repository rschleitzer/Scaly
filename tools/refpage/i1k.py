#!/usr/bin/env python3
"""The free refuter for the s116 generic-substitution defect.

A function whose own mangled name is CONCRETE must not mention an UNSUBSTITUTED
generic parameter in its body.  `I1K` / `I1V` in a body under a concrete symbol
means `instantiate_generic` handed back a type whose parameter was never bound
to its concrete argument -- the emitter then reads it as a pointer.

Clean tree: zero.  Any finding is the defect, named by the function it lands in.
"""
import re, sys, collections

# `I1K` in an Itanium name is "template args begin, length-1 name K".  It is
# FOLLOWED by the next component's length digit, so a `(?![A-Za-z0-9_])` lookahead
# rejects every real occurrence -- the first draft of this instrument scored zero
# on a tree that was demonstrably broken.  The clean tree scoring zero is what
# licenses the bare pattern.
PARAM = re.compile(r'I1[KVT]')

def scan(path):
    findings = collections.defaultdict(set)
    cur = None
    nm = re.compile(r'@("?)([A-Za-z0-9_$.]+)\1\s*\(')
    for line in open(path, encoding='utf8', errors='replace'):
        if line.startswith('define'):
            m = nm.search(line)
            cur = m.group(2) if m else None
            if cur and PARAM.search(cur):
                cur = None          # a generic TEMPLATE body may mention them
            continue
        if line.startswith('}'):
            cur = None; continue
        if cur is None: continue
        for m in re.finditer(r'@("?)([A-Za-z0-9_$.]+)\1', line):
            if PARAM.search(m.group(2)):
                findings[cur].add(m.group(2))
        for m in re.finditer(r'%([A-Za-z0-9_$.]+)', line):
            if PARAM.search(m.group(1)):
                findings[cur].add('%' + m.group(1))
    return findings

if __name__ == '__main__':
    total = 0
    for p in sys.argv[1:]:
        f = scan(p)
        print(f"{p}: {len(f)} Funktion(en) mit unsubstituiertem Parameter im Rumpf")
        for k in sorted(f):
            print(f"   {k[:96]}")
            for t in sorted(f[k])[:3]:
                print(f"        -> {t[:92]}")
        total += len(f)
    sys.exit(1 if total else 0)

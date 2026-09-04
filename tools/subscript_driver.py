#!/usr/bin/env python3
"""Compiler-as-arbiter driver for a subscript rewrite.

`tools/subscript.py` cannot see TYPES, and the receiver's type is exactly what
decides whether `X[i]` exists: `Array`, `Vector` and `Slice` carry an
`operator []`, while `String` and the ports' own `StringC` carry a
`get_buffer()` and NO subscript at all.  So the rewrite is proposed by the
tool and JUDGED by the compiler, at LINE granularity rather than by file --
reverting a whole file for one bad site throws away the good ones in it.

Loop: emit every package root, collect the `file:line:` of every error,
revert exactly those lines to their pre-rewrite text, emit again.  Stops when
a pass is clean or when a pass reverts nothing (which means the error is NOT
one of our lines and the run has to be read by a human).

★★★DIE KANDIDATENZAHL DER DREI TOOLS IST KEINE ERNTEPROGNOSE, UND IM
PORT-CODE IST SIE FAST VOLLSTAENDIG UNECHT.  Gemessen 2026-09-04 ueber die
Port-Pakete (opensp, dazzle, tscaly, scalygpu): `subscript.py` 11 Sites,
`subscript_local.py` 14, `allocslice.py` 4 -- zusammen 29 Vorschlaege, von
denen dieser Driver **28 zurueckgebaut** hat und **genau EINER** ueberlebte
(`Partition.set_code`, ein `Array[u16]`).  Der Grund ist einer einziger und er
ist strukturell: die Ports laufen ueberwiegend STRING-Bytes, und `String` hat
`get_buffer()` und `as_slice()`, aber **weder `operator []` noch `put`** --
also ist jedes `pid[fs]` (`this.public_id`), `loc[k]` (`this.out_loc`),
`t4[3]`, `lo.name[t]` und `s.put(i, ..)` (`String^this(n)`) ein harter Fehler.
Die Tools sehen keine Typen und koennen das nicht wissen; `sliceview.py` ist
die Antwort fuer diese Receiver -- und **seit 2026-09-04 beansprucht es sie
auch selbst** (zweiter Modus, `convert_view`): der `String`-Receiver wird an
seiner BINDUNG zur Sicht (`let pn this.prog_name.as_slice()`), die Walks
werden `pn[pi]`, `get_length()` wird `.length`. Gemessen: 11 Sites, genau die
Menge, die dieser Driver am 2026-09-04 zurueckgebaut hat. **Ein Vorschlag von
`subscript.py` auf einem `String`-Receiver ist also kein Kandidat fuer diesen
Driver, sondern einer fuer `sliceview.py`** -- erst das eine Tool laufen
lassen, dann bleibt fuer den Driver nur noch, was wirklich ein Container ist.

**Konsequenz fuer den naechsten Sweep: die Tools NIE direkt --apply auf die
Ports, immer ueber diesen Driver** -- und eine Kandidatenzahl aus einem
Dry-Run erst dann als Ernte melden, wenn der Driver sie bestaetigt hat.

Usage: tools/subscript_driver.py <baseline-git-ref-or-'HEAD'>
"""
import os, re, subprocess, sys, collections

SC = os.environ.get('SCRATCH', '/tmp')
ROOTS = [r for r in subprocess.run(['sh', '-c', 'ls packages/*/0.1.0/*.scaly'],
         capture_output=True, text=True).stdout.split()
         if 'stdlib' not in r]
ERR = re.compile(r'^(packages/\S+\.scaly):(\d+):\d+: error:')

# ★★★AN UNLOCATED FAILURE IS STILL A FAILURE, AND THIS DRIVER ONCE CALLED TWO
# BROKEN ROOTS `CLEAN`.  The emitter reports `Emitter: member not found:
# String.put` with NO file and NO line, so the ERR regex above never matches
# it and `bad` stays empty -- which this loop read as success.  Both a
# `String.put` (the tool proposed a write on a receiver that has no `put`) and
# a wrong-receiver rewrite surfaced that way, and the sweep was banked as green
# until a package SUITE failed later.  A driver that cannot fail is worse than
# none: these lines are matched, reported, and make the run a hard failure.
UNLOCATED = re.compile(r'^(Emitter|Planner|Modeler):\s')

def changed_files():
    out = subprocess.run(['git', 'diff', '--name-only'], capture_output=True,
                         text=True).stdout.split()
    return [f for f in out if f.endswith('.scaly')]


def orig(path, ref):
    return subprocess.run(['git', 'show', f'{ref}:{path}'],
                          capture_output=True, text=True).stdout.split('\n')

def main():
    ref = sys.argv[1] if len(sys.argv) > 1 else 'HEAD'
    for rnd in range(1, 12):
        bad = collections.defaultdict(set)
        forced, unresolved = 0, []
        for r in ROOTS:
            p = subprocess.run(['./scalyc/build/scalyc', '-S', '--no-prelude',
                                '--no-tests', '-o', f'{SC}/drv.ll', r],
                               capture_output=True, text=True)
            unlocated = []
            for line in (p.stdout + p.stderr).split('\n'):
                m = ERR.match(line.strip())
                if m: bad[m.group(1)].add(int(m.group(2)))
                elif UNLOCATED.match(line.strip()): unlocated.append(line.strip())
            for u in dict.fromkeys(unlocated):
                print(f'  UNLOCATED in {r}: {u}')
                # The message names <Concept>.<member>.  We cannot get a line
                # from it, so revert the CHANGED lines that mention that member
                # -- over-reverting only loses sites, it cannot corrupt, and
                # the next round re-checks.  If nothing matches, the run fails
                # loudly below rather than reporting CLEAN.
                m = re.search(r'member not found:\s*\S*?\.(\w+)', u)
                if not m: unresolved.append((r, u)); continue
                member, hit = m.group(1), 0
                for path in {q for q in changed_files()}:
                    cur = open(path, encoding='utf8').read().split('\n')
                    old_ = orig(path, ref)
                    for i in range(min(len(cur), len(old_))):
                        if cur[i] != old_[i] and re.search(
                                rf'\.{re.escape(member)}\b', cur[i]):
                            print(f'  revert {path}:{i+1}  {cur[i].strip()[:66]}')
                            cur[i] = old_[i]; hit += 1
                    if hit: open(path, 'w', encoding='utf8').write('\n'.join(cur))
                if hit: forced += hit
                else: unresolved.append((r, u))
        if unresolved:
            print(f'round {rnd}: UNLOCATED FAILURE that matched no changed line '
                  f'-- NOT clean, read it by hand:')
            for r_, u in unresolved: print('   ', r_, u)
            return 1
        if not bad:
            if forced:
                print(f'round {rnd}: reverted {forced} (unlocated)'); continue
            print(f'round {rnd}: CLEAN'); return 0
        n = 0
        for path, lns in sorted(bad.items()):
            cur = open(path, encoding='utf8').read().split('\n')
            old = orig(path, ref)
            # ★★★REVERT-BY-INDEX IS ONLY SOUND WHILE THE LINE COUNT IS INTACT.
            # This driver reverts `cur[i]` to `old[i]`; the moment the rewrite
            # DELETED a line, every later index points at a different statement
            # and a revert writes a neighbouring line's text over a good one.
            # Measured: running tools/subscript_local.py WITHOUT
            # --keep-bindings dropped three `let b = ....get_buffer()` lines,
            # and a later revert in dazzle's Expression.scaly duplicated two
            # lines over an `else` branch -- which then COMPILED CLEAN, so the
            # driver reported success on corrupted source.  Both tools take
            # --keep-bindings for this reason and prune in a second pass; this
            # refuses to guess when they were not used that way.
            if len(cur) != len(old):
                print(f'  ABORT {path}: {len(cur)} lines vs {len(old)} at '
                      f'{ref} -- the rewrite deleted or added lines, so a '
                      f'revert by index would corrupt it.  Re-run the sweep '
                      f'with --keep-bindings and --prune afterwards.')
                return 1
            for ln in sorted(lns):
                # ★A CONFORMANCE error is reported at the FUNCTION's line, not
                # at the expression's (CLAUDE.md records this for exactly this
                # tool). So when the reported line is not one of ours, revert
                # the NEAREST changed line at or after it -- the body of the
                # routine the compiler just named.
                i = ln - 1
                if not (i < len(cur) and i < len(old) and cur[i] != old[i]):
                    j = i
                    while j < len(cur) and j < len(old) and j < i + 60:
                        if cur[j] != old[j]: i = j; break
                        j += 1
                if i < len(cur) and i < len(old) and cur[i] != old[i]:
                    print(f'  revert {path}:{i+1}  {cur[i].strip()[:70]}')
                    cur[i] = old[i]; n += 1
            open(path, 'w', encoding='utf8').write('\n'.join(cur))
        print(f'round {rnd}: reverted {n}')
        if n == 0:
            print('  no reverted line -- the error is not one of ours:')
            for path, lns in sorted(bad.items()):
                print('   ', path, sorted(lns)[:10])
            return 1
    print('did not converge'); return 1

sys.exit(main())

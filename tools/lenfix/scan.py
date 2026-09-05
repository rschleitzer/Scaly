#!/usr/bin/env python3
"""Der LAENGEN-FIXPUNKT: welche Pufferparameter tragen ihre Laenge, wenn man den
AUFRUFGRAPHEN mitliest statt nur den eigenen Rumpf?

★★★DAS IST DIE LUECKE, DIE FUENF KAMPAGNEN OFFEN GELASSEN HABEN, und sie ist eine
Eigenschaft der BEWEISREGEL, nicht des Baums.  `tools/refslice/scan.py` fragt
zuerst `walks(body, name)` -- beweist der eigene Rumpf durch Arithmetik, dass der
Name ein Puffer ist? -- und wenn nicht, bricht es mit `UNWALKED` ab, OHNE die
Paarung ueberhaupt zu suchen.  Ein reiner Weiterreicher zeigt aber nie
Arithmetik.  Gemessen 2026-09-05: 556 pufferfoermige Parameter, davon 430
UNWALKED, und `tools/unwalked.py` sagt, dass 280 davon reine Weiterreicher sind.

Die Kette, an der man es sieht -- jede Ebene traegt die Laenge, keine Ebene
beweist sie allein:

    ScannerState.token_value / .token_value_len     Feldpaar
      -> get_identifier_token(buf: pointer[char], len: int)   ADJACENT-Form,
                                                    aber der Rumpf zeigt nur
                                                    `*buf` -> refslice: UNWALKED
          -> kw_eq(buf: pointer[char], word: Slice[char])     Rumpf LAEUFT buf,
                                                    aber keine Laenge in der
                                                    Signatur -> refslice: LENLESS

★★★DIE BEWEISLAST WIRD GETEILT: die PAARUNG kommt aus der eigenen Signatur, die
PUFFERSCHAFT aus dem Aufrufgraphen.  Ein Parameter ist konvertierbar, wenn beides
zusammenkommt -- und genau diese Trennung ist neu.

Der Fixpunkt laeuft CALLEE -> CALLER: ein Parameter, der an eine als Puffer
bewiesene Position weitergereicht wird, ist selbst ein Puffer.  Gesaet wird aus
zwei Quellen, und die zweite ist die, die dieser Kampagne ihre Richtung gibt:

  (a) der eigene Rumpf laeuft ihn (refslice' `walks`) -- das Blatt;
  (b) er wird an einen bereits zu `Slice[...]` KONVERTIERTEN Parameter
      weitergereicht -- die geerntete Front, an der die Kette weiterwaechst.

★★★UND DIE ANDERE RICHTUNG WIRD NICHT GELAUFEN, mit Absicht.  Die Lehre von
`refout` steht im Wurzel-CLAUDE.md: `BUFFER` muss den Aliasgraphen in BEIDE
Richtungen fluten, sonst kommen Blattparameter falsch zurueck -- dort ging es
aber darum, ob ein Name ueberhaupt ein Puffer ist.  Hier ist die Frage die
PAARUNG, und eine Paarung, die nur der Aufrufer kennt, ist eine Behauptung ueber
die SEMANTIK ("die Laenge, die der Aufrufer hat, gehoert zu genau diesem
Puffer"), die kein Werkzeug beweisen kann.  Solche Faelle werden als
`CALLER-LEN` BERICHTET und nie als Kandidat gefuehrt: der Leser entscheidet.

★★★SELBSTPRUEFUNG: `--selftest` verlangt, dass der bekannte Fund wieder
herauskommt.  Ein Refuter, der nicht feuern kann, ist schlechter als keiner (das
`I1K`-Instrument stand einmal auf einem beweisbar kaputten Baum auf null), und
diese Datei hat genau eine Kette, an der sie sich kalibrieren laesst.
"""
import re, os, sys, collections

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'refout'))
from scan import collect, strip_comment, split_args           # noqa: E402

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'refslice'))
import importlib.util as _ilu                                  # noqa: E402
_spec = _ilu.spec_from_file_location(
    'refslice_scan', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'refslice', 'scan.py'))
_rs = _ilu.module_from_spec(_spec); _spec.loader.exec_module(_rs)
walks, writes, addressed = _rs.walks, _rs.writes, _rs.addressed
shares_stem, LEN_NAME, LEN_TYPE = _rs.shares_stem, _rs.LEN_NAME, _rs.LEN_TYPE
NON_ELEMENT, PEELED, frozen_names, pkg_of = _rs.NON_ELEMENT, _rs.PEELED, _rs.frozen_names, _rs.pkg_of

BUFPARAM = re.compile(r'pointer\[\s*(\w+)\s*\]$')
SLICEPARAM = re.compile(r'Slice\[\s*\w+\s*\]$')
CALL = re.compile(r'\b(\w+)\s*\(')


def arg_positions(body, name):
    """Every (callee, position) this NAME is handed to as a BARE argument.

    Bare on purpose: `f(p, n)` forwards the buffer, `f(p + 1, n)` and
    `f(p.length)` do not, and a tool that cannot tell them apart proves nothing.
    """
    out = []
    bare = re.compile(r'^\s*&?%s\s*$' % re.escape(name))
    for line in body.split('\n'):
        line = strip_comment(line)
        for m in CALL.finditer(line):
            args = split_args(line, m.end() - 1)
            if args is None: continue
            for k, a in enumerate(args):
                if bare.match(a):
                    out.append((m.group(1), k))
    return out


def main(paths, want_selftest=False):
    routines = collect(paths)
    frozen = frozen_names('.')
    by_name = collections.defaultdict(list)
    for ri, r in enumerate(routines):
        by_name[r['fn']].append(ri)

    # every buffer-shaped parameter, and every ALREADY-CONVERTED Slice parameter:
    # the second set is the harvested front the fixpoint grows from.
    buf, slice_param = {}, set()
    for ri, r in enumerate(routines):
        for pi, (pn, pt) in enumerate(r['params']):
            t = pt.strip()
            m = BUFPARAM.match(t)
            if m and m.group(1) not in NON_ELEMENT:
                buf[(ri, pi)] = m.group(1)
            elif SLICEPARAM.match(t):
                slice_param.add((ri, pi))

    # a `this` receiver occupies a slot the declaration does not, so argument k
    # of a CALL lands on parameter k+1 of a method. Ask both alignments rather
    # than guess which kind the callee is.
    def positions(fn, k):
        for cri in by_name.get(fn, ()):
            params = routines[cri]['params']
            has_this = bool(params) and params[0][0] == 'this'
            for pk in ({k, k + 1} if has_this else {k}):
                if 0 <= pk < len(params):
                    yield (cri, pk)

    # ---- fixpoint: proven buffer, callee -> caller
    proven = set()
    for key, elem in buf.items():
        ri, pi = key
        if walks(routines[ri]['body'], routines[ri]['params'][pi][0]):
            proven.add(key)

    changed = True
    while changed:
        changed = False
        for key in buf:
            if key in proven: continue
            ri, pi = key
            nm = routines[ri]['params'][pi][0]
            for fn, k in arg_positions(routines[ri]['body'], nm):
                if any(t in proven or t in slice_param for t in positions(fn, k)):
                    proven.add(key); changed = True; break

    # ---- verdict: the PAIRING comes from the routine's own signature
    rows = []
    for key, elem in sorted(buf.items()):
        ri, pi = key
        r = routines[ri]
        params = r['params']
        pn = params[pi][0]
        if addressed(r['body'], pn):
            rows.append(('ADDRESSED', r, pn, elem, None, key in proven)); continue
        if r['fn'] in frozen:
            rows.append(('FROZEN', r, pn, elem, None, key in proven)); continue
        lens = [(qi, qn) for qi, (qn, qt) in enumerate(params)
                if qi != pi and qn not in PEELED
                and LEN_NAME.match(qn) and qt.strip() in LEN_TYPE]
        pairing, ln = None, None
        for qi, qn in lens:
            if qi == pi + 1:
                pairing, ln = 'ADJACENT', qn; break
        if pairing is None:
            for qi, qn in lens:
                if shares_stem(pn, qn):
                    pairing, ln = 'NAMED', qn; break

        selfwalks = walks(r['body'], pn)
        if pairing is None:
            v = 'LENLESS'
        elif key in proven:
            # the pairing is in the signature and the buffer is proven -- by the
            # body itself (which refslice already reports) or, and this is what
            # the fixpoint adds, by a callee.
            v = pairing if selfwalks else 'CHAIN-' + pairing
        else:
            v = 'UNPROVEN-' + pairing
        if writes(r['body'], pn):
            v = 'W-' + v
        rows.append((v, r, pn, elem, ln, key in proven))

    if want_selftest:
        # the calibration case: get_identifier_token carries (buf, len) and its
        # body walks nothing -- only the chain through kw_eq can prove it.
        hit = [x for x in rows if x[1]['fn'] == 'get_identifier_token' and x[2] == 'buf']
        ok = bool(hit) and hit[0][0].startswith('CHAIN-')
        print(f"selftest: get_identifier_token(buf) -> {hit[0][0] if hit else 'NOT FOUND'}"
              f"   {'OK' if ok else 'FAILED — the fixpoint does not fire, every zero below is meaningless'}")
        if not ok: sys.exit(1)
    return rows


# ---------------------------------------------------------------------------
# `--chains`: what a candidate is CONNECTED to.
#
# ★★★A CANDIDATE IS NOT A WORK ITEM -- THE CHAIN IS.  A forwarder handed to an
# unconverted `pointer[T]` callee is the silent Slice-into-pointer bug (a Slice
# passed where a pointer is declared COMPILES and reads the struct as the
# buffer), so the callee side has to move in the same commit.  And the caller
# side decides what the conversion COSTS: a caller that holds a field PAIR must
# construct a Slice per call unless the FIELD moves too.
def chains(paths):
    routines = collect(paths)
    by_name = collections.defaultdict(list)
    for ri, r in enumerate(routines):
        by_name[r['fn']].append(ri)
    rows = main(paths)
    cand = [(v, r, pn, elem, ln) for v, r, pn, elem, ln, pr in rows
            if v.startswith(('CHAIN-', 'W-CHAIN-'))]
    for v, r, pn, elem, ln in cand:
        downstream = sorted({fn for fn, k in arg_positions(r['body'], pn) if fn in by_name})
        callers = []
        for r2 in routines:
            for m in CALL.finditer(r2['body']):
                if m.group(1) == r['fn']:
                    callers.append(r2['fn']); break
        print(f"{r['fn']}({pn}, len={ln})   [{v}]")
        print(f"    file      {r['file']}:{r['line']}")
        print(f"    forwards  {', '.join(downstream) or '(none — proven some other way)'}")
        print(f"    callers   {', '.join(sorted(set(callers))[:8]) or '(none)'}"
              f"{'  …' if len(set(callers)) > 8 else ''}")


if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    if '--chains' in sys.argv:
        chains(args or ['packages']); sys.exit(0)
    rows = main(args or ['packages'], want_selftest='--selftest' in sys.argv)
    verbose = '-v' in sys.argv
    tally = collections.Counter()
    for v, r, pn, elem, ln, pr in rows:
        tally[(v, pkg_of(r['file']))] += 1
        if verbose and v.startswith(('CHAIN-', 'W-CHAIN-')):
            print(f"{v:16s} pointer[{elem}] {r['file']}:{r['line']} {r['fn']}({pn}, len={ln})")
    print('---')
    for (v, pkg), n in sorted(tally.items(), key=lambda kv: (-kv[1], kv[0])):
        print(f"{n:5d}  {pkg:8s} {v}")
    print(f"--- {sum(tally.values())} buffer-shaped pointer parameters")
    chain = sum(n for (v, _), n in tally.items() if v.startswith(('CHAIN-', 'W-CHAIN-')))
    print(f"--- {chain} of them are CHAIN candidates: the pairing is in the signature and only a CALLEE proves the buffer")

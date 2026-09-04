"""Konvertiert EINEN hand-allokierten, rein lokal gelaufenen Puffer je Aufruf.

ROUTINE-LOKAL, weil derselbe Name in derselben Datei zu einer anderen Bindung
gehoeren kann: `specified` ist in Parser.scaly dreimal gebunden, und eine
dateiweite Ersetzung schriebe die anderen beiden Routinen mit um.
Die WRITE-Form zuerst -- sonst erzeugt die Read-Regel `set p[i]: v`, ein
harter rc-4.
"""
import re, sys
sys.path.insert(0, 'tools')
from subscript_local import routine_spans, strip_comment

path, line_no, newbind, local = sys.argv[1], int(sys.argv[2]), sys.argv[3], sys.argv[4]
lines = open(path, encoding='utf8').read().split('\n')
span = next((a, b) for a, b in routine_spans(lines) if a < line_no <= b)
a, b = span
W = re.compile(rf'set\s+\*\(\s*{re.escape(local)}\s*\+\s*(.+?)\s*\):\s*(.*)$')
R = re.compile(rf'\*\(\s*{re.escape(local)}\s*\+\s*([^()]*(?:\([^()]*\)[^()]*)*)\)')
n_w = n_r = 0
for i in range(a, b):
    code = lines[i]
    if i == line_no - 1:
        indent = code[:len(code) - len(code.lstrip())]
        lines[i] = indent + newbind
        continue
    if strip_comment(code).strip().startswith('set '):
        m = W.search(code)
        if m:
            lines[i] = code[:m.start()] + f'{local}.put({m.group(1)}, {m.group(2)})'
            n_w += 1; continue
    new, k = R.subn(rf'{local}[\1]', code)
    if k: lines[i] = new; n_r += k
open(path, 'w', encoding='utf8').write('\n'.join(lines))
print(f'{path}:{line_no}  {local}: {n_w} writes, {n_r} reads  (Routine {a+1}..{b})')

"""python3 listings.py <out-dir> <chapter.xml>... -- the programs of the
book's chapters on scaling and what each must print (tests/book/run.sh).

Every <programlisting> not marked role="fragment" is a whole program, and the
<screen> elements after it -- before the next listing -- say what it does.
A screen of plain lines, directly the first, is what `scaly run` prints. A
screen whose first line begins with "$ " is a transcript: each such line is a
command, run in a directory that holds the program under the file name the
command names (its first word ending in .scaly), and the lines up to the next
command are what it prints; a listing may have several. A listing without a
screen is an error -- the expected output stands at the listing, so that the
reader sees it and the suite checks it --, and so is a plain screen that is
not the first.

Written as <out-dir>/<nn>-<section-id>/: prog.scaly, expected.txt (the
output of `scaly run`) and transcript-<k>.txt (each transcript as written).
"""
import html
import os
import re
import sys

out = sys.argv[1]
token = re.compile(r'xml:id="([^"]+)"'
                   r'|<programlisting( role="([^"]*)")?>(.*?)</programlisting>'
                   r'|<screen>(.*?)</screen>', re.S)
n = 0
errors = 0
for chapter in sys.argv[2:]:
    text = open(chapter, encoding='utf-8').read()
    current = os.path.basename(chapter)
    pending = None          # (dir, section) of a listing still without its screen
    last = None             # (dir, section, screens so far) of the latest listing
    for m in token.finditer(text):
        if m.group(1):
            current = m.group(1)
            continue
        if m.group(4) is not None:
            if m.group(3) == 'fragment':
                continue
            if pending:
                print('listings.py: %s: a listing without a screen of its output' % pending[1])
                errors += 1
            n += 1
            d = os.path.join(out, '%02d-%s' % (n, current))
            os.makedirs(d, exist_ok=True)
            with open(os.path.join(d, 'prog.scaly'), 'w', encoding='utf-8') as f:
                f.write(html.unescape(m.group(4)).strip('\n') + '\n')
            pending = (d, current)
            last = [d, current, 0]
            continue
        if last is None:
            continue
        screen = html.unescape(m.group(5)).strip('\n') + '\n'
        last[2] += 1
        if screen.startswith('$ '):
            name = 'transcript-%d.txt' % last[2]
        elif last[2] == 1:
            name = 'expected.txt'
        else:
            print('listings.py: %s: a second screen of plain output' % last[1])
            errors += 1
            continue
        with open(os.path.join(last[0], name), 'w', encoding='utf-8') as f:
            f.write(screen)
        pending = None
    if pending:
        print('listings.py: %s: a listing without a screen of its output' % pending[1])
        errors += 1
if n == 0:
    sys.exit('listings.py: no listing in ' + ' '.join(sys.argv[2:]))
sys.exit(1 if errors else 0)

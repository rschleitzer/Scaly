#!/usr/bin/env python3
"""docs/runnable.py <lang-dir> <scaly> -- which listings of the book run.

Every listing of the pages docs/html.sh wrote (a pre of class "listing", not
one marked a fragment) is run as a program with this tree's tool, `scaly run`,
and the ones that end with status 0 go into <lang-dir>/runnable.js: the FNV-1a
hash of their text, as book.js computes it, which then gives them a Run button
that runs them in the page. A listing that is a piece of a program, a shell
command or shows an error gets none -- the button is only where it works.
"""
import concurrent.futures
import html
import os
import re
import subprocess
import sys
import tempfile

LISTING = re.compile(r'<pre\s+class="(listing[^"]*)"\s*><code\s*>(.*?)</code\s*></pre', re.S)


def text_of(body):
    # as book.js reads it: the text, no line ends before it, no white space after
    return re.sub(r'\s+$', '', re.sub(r'^[\r\n]+', '', html.unescape(body)))


def fnv1a(text):
    h = 0x811C9DC5
    for b in text.encode('utf-8'):
        h = ((h ^ b) * 0x01000193) & 0xFFFFFFFF
    return '%08x' % h


def main():
    lang, scaly = sys.argv[1], sys.argv[2]
    home = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    env = dict(os.environ, SCALY_HOME=home)
    listings = {}
    for name in sorted(os.listdir(lang)):
        if name.endswith('.htm'):
            for m in LISTING.finditer(open(os.path.join(lang, name), encoding='utf-8').read()):
                if 'fragment' not in m.group(1).split():
                    text = text_of(m.group(2))
                    listings[fnv1a(text)] = text
    work = tempfile.mkdtemp(prefix='runnable.')

    def runs(item):
        key, text = item
        path = os.path.join(work, 'l%s.scaly' % key)
        with open(path, 'w', encoding='utf-8') as f:
            f.write(text + '\n')
        try:
            done = subprocess.run([scaly, 'run', path], capture_output=True, timeout=120, env=env,
                                  stdin=subprocess.DEVNULL)
            return key if done.returncode == 0 else None
        except subprocess.TimeoutExpired:
            return None

    with concurrent.futures.ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as pool:
        keys = sorted(k for k in pool.map(runs, listings.items()) if k)
    with open(os.path.join(lang, 'runnable.js'), 'w', encoding='utf-8') as f:
        f.write('// runnable.js - written by docs/runnable.py: the listings of the book that\n'
                '// run as a program (the FNV-1a hash of their text), which get a Run button\n'
                'window.SCALY_RUNNABLE = [\n')
        f.write(''.join('  "%s",\n' % k for k in keys))
        f.write('];\n')
    print('runnable.py: %d of %d listings run' % (len(keys), len(listings)))


main()

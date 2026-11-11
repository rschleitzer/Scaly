"""python3 parity.py <file>[#section] <file>[#section] -- the two texts have
the same sections (by id, in order) with as many programs each (listings
not marked role="fragment"): a translation of the playground's chapters
(docs/playground/de/) against its original (tests/tour/run.sh). The
<file>#<section> form takes that section of a larger file, as listings.py
does."""
import re
import sys


def body(arg):
    path, _, section = arg.partition('#')
    text = open(path, encoding='utf-8').read()
    if not section:
        return text
    start = text.find(f'xml:id="{section}"')
    if start < 0:
        sys.exit(f'parity.py: no section {section} in {path}')
    line = text.rfind('\n', 0, start)
    indent = start - line - len('<section ') - 1
    end = text.find('\n' + ' ' * indent + '</section>', start)
    return text[text.rfind('<', 0, start):end if end > 0 else len(text)]


def sections(text):
    out = []
    for m in re.finditer(r'<section xml:id="([^"]+)"|<programlisting( role="([^"]*)")?>', text):
        if m.group(1):
            out.append([m.group(1), 0])
        elif m.group(3) != 'fragment' and out:
            out[-1][1] += 1
    return [tuple(s) for s in out]


a, b = sys.argv[1:3]
sa, sb = sections(body(a)), sections(body(b))
if sa != sb:
    print(f'parity: {b} is not a translation of {a}:')
    print(f'  {a}: {sa}')
    print(f'  {b}: {sb}')
    sys.exit(1)

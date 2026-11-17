"""python3 parity.py <book.xml> <translated.xml> -- the translated book has
the English one's chapters and sections, by id and in order, and in each the
same programs and screens, character for character: the programs of the
book are written once, in English, and stand in both.

python3 parity.py --chapters <book.xml> -- the book's chapter files, in the
order of its entities (what tests/book/run.sh runs).
"""
import os
import re
import sys

ENTITY = re.compile(r'<!ENTITY\s+\S+\s+SYSTEM\s+"([^"]+)"\s*>')
TOKEN = re.compile(r'xml:id="([^"]+)"|<programlisting[^>]*>(.*?)</programlisting>|<screen>(.*?)</screen>', re.S)


def chapters(book):
    base = os.path.dirname(book)
    return [os.path.join(base, f) for f in ENTITY.findall(open(book, encoding='utf-8').read())]


def outline(book):
    items = []
    for chapter in chapters(book):
        for m in TOKEN.finditer(open(chapter, encoding='utf-8').read()):
            if m.group(1):
                items.append(('id', m.group(1)))
            elif m.group(2) is not None:
                items.append(('program', m.group(2)))
            else:
                items.append(('screen', m.group(3)))
    return items


if sys.argv[1] == '--chapters':
    print(' '.join(chapters(sys.argv[2])))
    sys.exit(0)
english, other = outline(sys.argv[1]), outline(sys.argv[2])
bad = 0
for k in range(max(len(english), len(other))):
    a = english[k] if k < len(english) else None
    b = other[k] if k < len(other) else None
    if a != b:
        where = next((v for kind, v in reversed(english[:k + 1]) if kind == 'id'), 'the beginning')
        print('parity.py: %s differs from %s after %s: %s against %s'
              % (sys.argv[2], sys.argv[1], where,
                 b[0] + ' ' + b[1][:60].strip() if b else 'nothing',
                 a[0] + ' ' + a[1][:60].strip() if a else 'nothing'))
        bad += 1
        break
print('parity: %d ids, programs and screens alike' % len(english) if not bad else 'parity: FAIL')
sys.exit(1 if bad else 0)

"""python3 mathml.py <page.htm>... -- every formula of the pages the book's
stylesheet wrote is MathML a browser sets: a <math> element that reads as
XML, holding only elements of MathML Core, and each with the number of
children its kind takes (a fraction two, a sub-and-superscript three).
"""
import re
import sys
import xml.etree.ElementTree as ET

CORE = {'math', 'mrow', 'mi', 'mn', 'mo', 'mtext', 'mspace', 'mfrac', 'msub', 'msup', 'msubsup',
        'msqrt', 'mroot', 'mover', 'munder', 'munderover', 'mtable', 'mtr', 'mtd', 'mstyle',
        'mpadded', 'mphantom'}
ARITY = {'mfrac': 2, 'msub': 2, 'msup': 2, 'mroot': 2, 'mover': 2, 'munder': 2,
         'msubsup': 3, 'munderover': 3}
MATH = re.compile(r'<math\b.*?</math\s*>', re.S)

bad = 0
count = 0


def check(e, where):
    global bad
    if e.tag not in CORE:
        print('mathml.py: %s: <%s> is not MathML Core' % (where, e.tag))
        bad += 1
    if e.tag in ARITY and len(e) != ARITY[e.tag]:
        print('mathml.py: %s: <%s> has %d children, not %d' % (where, e.tag, len(e), ARITY[e.tag]))
        bad += 1
    for c in e:
        check(c, where)


for page in sys.argv[1:]:
    for m in MATH.finditer(open(page, encoding='utf-8').read()):
        count += 1
        where = '%s, formula %d' % (page, count)
        try:
            check(ET.fromstring(m.group(0)), where)
        except ET.ParseError as e:
            print('mathml.py: %s: not XML (%s)' % (where, e))
            bad += 1
if count == 0:
    sys.exit('mathml.py: no formula in ' + ' '.join(sys.argv[1:]))
print('mathml: %d formulas, %s' % (count, 'all MathML Core' if not bad else '%d FAIL' % bad))
sys.exit(1 if bad else 0)

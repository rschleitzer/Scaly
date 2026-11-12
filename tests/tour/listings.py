"""python3 listings.py <chapter.xml> <section-id> <out-dir> [empty] -- every
<programlisting> of the section, but those marked role="fragment", as
<out-dir>/<nn>-<subsection-id>.scaly, numbered in document order
(tests/tour/run.sh). The tour page's steps come out of the same section by
the DSSSL engine (docs/dsssl/scaly-tour.dsl); this reads only the listings.
A section without a listing is an error, unless the fourth argument is
`empty` (a step of prose alone). A listing marked role="solution" -- the
answer to a task of the playground -- comes out with the section's
<screen role="expect"> beside it, as <nn>-<subsection-id>.expect."""
import html
import os
import re
import sys

chapter, section, out = sys.argv[1:4]
text = open(chapter, encoding='utf-8').read()
start = text.find(f'xml:id="{section}"')
if start < 0:
    sys.exit(f'listings.py: no section {section} in {chapter}')
# the section ends where the next section of its level begins
depth = text.rfind('\n', 0, start)
indent = start - depth - len('<section ') - 1
end_tag = '\n' + ' ' * indent + '</section>'
end = text.find(end_tag, start)
body = text[start:end if end > 0 else len(text)]
os.makedirs(out, exist_ok=True)
n = 0
current = section
for m in re.finditer(r'xml:id="([^"]+)"|<programlisting( role="([^"]*)")?>(.*?)</programlisting>', body, re.S):
    if m.group(1):
        current = m.group(1)
        continue
    if m.group(3) == 'fragment':
        continue
    n += 1
    with open(os.path.join(out, f'{n:02d}-{current}.scaly'), 'w', encoding='utf-8') as f:
        f.write(html.unescape(m.group(4)) + '\n')
    # a task's solution: beside it what it must print, the section's
    # <screen role="expect"> (tests/tour/run.sh compares)
    if m.group(3) == 'solution':
        sec = body.find(f'xml:id="{current}"')
        expect = re.search(r'<screen role="expect">(.*?)</screen>', body[max(sec, 0):], re.S)
        if not expect:
            sys.exit(f'listings.py: the solution in {current} has no <screen role="expect">')
        with open(os.path.join(out, f'{n:02d}-{current}.expect'), 'w', encoding='utf-8') as f:
            f.write(html.unescape(expect.group(1)))
if n == 0 and sys.argv[4:5] != ['empty']:
    sys.exit(f'listings.py: no listing in {section}')

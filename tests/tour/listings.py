"""python3 listings.py <chapter.xml> <section-id> <out-dir> -- every
<programlisting> of the section, but those marked role="fragment", as
<out-dir>/<nn>-<subsection-id>.scaly, numbered in document order
(tests/tour/run.sh). The tour page's steps come out of the same section by
the DSSSL engine (docs/dsssl/scaly-tour.dsl); this reads only the listings."""
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
if n == 0:
    sys.exit(f'listings.py: no listing in {section}')

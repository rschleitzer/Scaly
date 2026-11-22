#!/usr/bin/env python3
"""docs/figures.py <out-dir> <scaly> <program.scaly>... -- the pictures of a book.

A picture in the book is drawn by a program (docs/ml/fig/*.scaly): it
computes every point and writes them as picture.<width>x<height>.rgba, four
bytes a point -- red, green, blue, opacity --, in its working directory.
Each is run with this tree's tool, `scaly run`, in a directory of its own,
and its picture is written as <out-dir>/fig/<name>.png, the name the
program's file has; the book's pages show it (dsssl/scaly-book.dsl,
imagedata fileref="fig/<name>.png"). A program may draw instead
(scaly/draw.scaly): the drawing it shows, drawing-1.svg, is written as
<out-dir>/fig/<name>.svg, told where the formula font lies (fonts/ beside
the pages). A program that ends with a status other than 0, or writes
neither a picture of the size its name says nor a drawing, is an error.
"""
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import zlib


def png(width, height, rgba):
    # a picture whose every point is opaque is written without its opacity
    # (colour type 2, not 6): smaller, and a PDF takes it as it is, where it
    # would have to unpack one with an alpha channel (docs/ml-pdf.sh)
    opaque = all(rgba[i] == 255 for i in range(3, len(rgba), 4))
    channels = 3 if opaque else 4
    if opaque:
        rgba = bytes(b for i, b in enumerate(rgba) if i % 4 != 3)
    # each row filtered by "up" (2) or "sub" (1), whichever leaves smaller
    # numbers, then deflated
    stride = width * channels
    rows = bytearray()
    above = bytes(stride)
    for y in range(height):
        row = rgba[y * stride:(y + 1) * stride]
        up = bytes((row[i] - above[i]) & 255 for i in range(stride))
        sub = bytes((row[i] - (row[i - channels] if i >= channels else 0)) & 255 for i in range(stride))
        cost = lambda r: sum(b if b < 128 else 256 - b for b in r)
        kind, data = (2, up) if cost(up) <= cost(sub) else (1, sub)
        rows.append(kind)
        rows += data
        above = row

    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 6 if channels == 4 else 2, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(bytes(rows), 9)) + chunk(b'IEND', b''))


# the formula font of the pages (book/fonts, beside them in fonts/), for a
# drawing's letters: the drawing lies in fig/
FONT_FACE = ('\n<style>@font-face { font-family: "Latin Modern Math"; '
             'src: url("../fonts/latinmodern-math.otf"); }</style>')


def main():
    out, scaly, programs = sys.argv[1], os.path.abspath(sys.argv[2]), sys.argv[3:]
    home = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    env = dict(os.environ, SCALY_HOME=home)
    os.makedirs(os.path.join(out, 'fig'), exist_ok=True)
    errors = done = 0
    for program in programs:
        name = os.path.splitext(os.path.basename(program))[0]
        work = tempfile.mkdtemp(prefix='figure.')
        shutil.copy(program, os.path.join(work, 'figure.scaly'))
        run = subprocess.run([scaly, 'run', 'figure.scaly'], cwd=work, env=env, capture_output=True,
                             stdin=subprocess.DEVNULL, timeout=300)
        pictures = [f for f in os.listdir(work) if re.fullmatch(r'picture\.\d+x\d+\.rgba', f)]
        drawings = [f for f in os.listdir(work) if f == 'drawing-1.svg']
        if run.returncode == 0 and not pictures and drawings:
            # a drawing (scaly/draw.scaly): the SVG as it is, told where the
            # formula font lies beside the pages, for the letters of `math`
            svg = open(os.path.join(work, drawings[0]), encoding='utf-8').read()
            head = svg.index('>') + 1
            svg = svg[:head] + FONT_FACE + svg[head:]
            with open(os.path.join(out, 'fig', name + '.svg'), 'w', encoding='utf-8') as f:
                f.write(svg)
            done += 1
        elif run.returncode != 0 or len(pictures) != 1:
            print('figures.py: %s: status %d, %d pictures' % (name, run.returncode, len(pictures)))
            print((run.stdout + run.stderr).decode(errors='replace')[-600:])
            errors += 1
        else:
            width, height = map(int, re.findall(r'\d+', pictures[0]))
            rgba = open(os.path.join(work, pictures[0]), 'rb').read()
            if len(rgba) != width * height * 4:
                print('figures.py: %s: %d bytes, not %d x %d x 4' % (name, len(rgba), width, height))
                errors += 1
            else:
                with open(os.path.join(out, 'fig', name + '.png'), 'wb') as f:
                    f.write(png(width, height, rgba))
                done += 1
        shutil.rmtree(work, ignore_errors=True)
    print('figures: %d drawn%s' % (done, ', %d FAIL' % errors if errors else ''))
    sys.exit(1 if errors else 0)


main()

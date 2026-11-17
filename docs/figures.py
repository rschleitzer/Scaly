#!/usr/bin/env python3
"""docs/figures.py <out-dir> <scaly> <program.scaly>... -- the pictures of a book.

A picture in the book is drawn by a program (docs/ml/fig/*.scaly): it
computes every point and writes them as picture.<width>x<height>.rgba, four
bytes a point -- red, green, blue, opacity --, in its working directory.
Each is run with this tree's tool, `scaly run`, in a directory of its own,
and its picture is written as <out-dir>/fig/<name>.png, the name the
program's file has; the book's pages show it (dsssl/scaly-book.dsl,
imagedata fileref="fig/<name>.png"). A program that ends with a status other
than 0 or writes no picture of the size its name says is an error.
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
    # each row filtered by "up" (2) or "sub" (1), whichever leaves smaller
    # numbers, then deflated
    stride = width * 4
    rows = bytearray()
    above = bytes(stride)
    for y in range(height):
        row = rgba[y * stride:(y + 1) * stride]
        up = bytes((row[i] - above[i]) & 255 for i in range(stride))
        sub = bytes((row[i] - (row[i - 4] if i >= 4 else 0)) & 255 for i in range(stride))
        cost = lambda r: sum(b if b < 128 else 256 - b for b in r)
        kind, data = (2, up) if cost(up) <= cost(sub) else (1, sub)
        rows.append(kind)
        rows += data
        above = row

    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(bytes(rows), 9)) + chunk(b'IEND', b''))


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
        if run.returncode != 0 or len(pictures) != 1:
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

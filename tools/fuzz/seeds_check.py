#!/usr/bin/env python3
"""seeds_check.py -- with a server of fuzz_http.py's ports running: send
every seed unmutated and print the first line of what came back, so that a
seed the server refuses (and every mutation of it with it) shows."""
import os
import random
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fuzz_http  # noqa: E402

rng = random.Random(0)
for i, seed in enumerate(fuzz_http.SEEDS):
    got = fuzz_http.send(seed, random.Random(0), fuzz_http.PORT, timeout=0.5)
    first = (got or b'').split(b'\r\n', 1)[0][:60]
    if seed.startswith(fuzz_http.PREFACE):
        # HTTP/2: the frame types that came back
        kinds, at = [], 0
        while got and at + 9 <= len(got):
            n = int.from_bytes(got[at:at + 3], 'big')
            kinds.append(got[at + 3])
            at += 9 + n
        first = f'h2 frames {kinds}'.encode()
    print(f'{i:2} {seed[:40]!r:48} -> {first.decode(errors="replace")}')

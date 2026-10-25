#!/usr/bin/env python3
"""replay.py <server> <finding-dir> [--repeat N] [--whole] -- the cases a
fuzz_http.py finding kept (case-<i>-<port>.bin, in their order), sent again
to a fresh server one after the other: says after which case the server
ended or stopped answering, or that it survived. --repeat sends the whole
list N times; --whole sends each case in one piece (fuzz_http cuts them at
random)."""
import argparse
import os
import random
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fuzz_http  # noqa: E402


class Whole(random.Random):
    """A generator whose cuts never cut: random() above every threshold."""

    def random(self):
        return 0.99


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('server')
    ap.add_argument('finding')
    ap.add_argument('--repeat', type=int, default=1)
    ap.add_argument('--whole', action='store_true')
    args = ap.parse_args()
    cases = []
    for name in os.listdir(args.finding):
        m = re.match(r'case-(\d+)-(\d+)\.bin$', name)
        if m:
            with open(os.path.join(args.finding, name), 'rb') as f:
                cases.append((int(m.group(1)), int(m.group(2)), f.read(), name))
    cases.sort()
    server = fuzz_http.Server(args.server, dict(os.environ, SCALY_POISON='1'))
    rng = Whole() if args.whole else random.Random(1)
    try:
        for round_ in range(args.repeat):
            for i, port, data, name in cases:
                fuzz_http.send(data, rng, port, timeout=0.2)
                time.sleep(0.05)
                if server.proc.poll() is not None:
                    print(f'ended after {name} (round {round_ + 1}): rc {server.proc.returncode}')
                    for line in server.take():
                        print(f'stderr: {line}')
                    return 1
                if not fuzz_http.healthy():
                    print(f'no answer after {name} (round {round_ + 1})')
                    return 1
        print(f'survived {len(cases)} cases x {args.repeat}')
        for line in server.take():
            print(f'stderr: {line}')
        return 0
    finally:
        server.stop()


if __name__ == '__main__':
    sys.exit(main())

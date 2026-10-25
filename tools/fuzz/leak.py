#!/usr/bin/env python3
"""leak.py <server> <what> [--count N] ... -- does a path of the server
leak? The same traffic, many times, against a fresh server, its resident
memory every tenth of the way: a slope that does not level off is a leak.

  <what>     an index into fuzz_http.SEEDS or a file: that request, whole and
             unmutated; with --fuzz a kind of case (h1, h2, ws, all) made as
             fuzz_http.py makes them
  --set N    with --fuzz: N cases made once and sent round and round -- a
             leak grows with every round, a high-water mark stays where the
             first rounds left it
  --jobs N   with --set: N senders at once (the server spreads connections
             over its schedulers, and each keeps pages of its own)
  --keep     everything on one connection, one request after the other
  --cut      in random pieces, as fuzz_http.py sends
  --ops      with --fuzz: the mutation operators allowed, e.g. 0,4
  --seeds    HttpArena's requests/, as for fuzz_http.py

Each connection gets the first piece of its answer and is closed with a
reset: a kept-alive connection never says its answer is complete, and
thousands of TIME_WAITs in a row use up the ephemeral ports.

What it said on 2026-10-25 (the arena, macOS, ten schedulers): every path
levels off -- HTTP/1.1, HTTP/2, gRPC, WebSocket, chunked, gzip, 400, 431,
the database routes without a database; a fixed set of 1000 mutated cases
stays at 13.5 MB sent in series; by four senders it climbs for longer, and
with SCALY_WORKERS=1 it stays at 8.4 MB: every scheduler fills its own page
store up to its own high-water mark, which is the store's hysteresis and
not a leak."""
import argparse
import os
import random
import socket
import struct
import sys
import threading

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fuzz_http  # noqa: E402


def one(port, data, wait):
    """data out whole, the first piece of the answer in, a reset."""
    c = socket.create_connection(('127.0.0.1', port), timeout=2)
    c.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 0))
    # a mutated request is often unfinished: the server waits for its rest,
    # and so would the read
    c.settimeout(wait)
    try:
        c.sendall(data)
        c.recv(65536)
    except OSError:
        pass
    c.close()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('server')
    ap.add_argument('what')
    ap.add_argument('--count', type=int, default=20000)
    ap.add_argument('--port', type=int, default=fuzz_http.PORT)
    ap.add_argument('--fuzz', action='store_true')
    ap.add_argument('--set', type=int)
    ap.add_argument('--jobs', type=int, default=1)
    ap.add_argument('--keep', action='store_true')
    ap.add_argument('--cut', action='store_true')
    ap.add_argument('--ops')
    ap.add_argument('--seeds')
    args = ap.parse_args()
    if args.seeds:
        for name in sorted(n for n in os.listdir(args.seeds) if n.endswith('.raw')):
            with open(os.path.join(args.seeds, name), 'rb') as f:
                fuzz_http.SEEDS.append(f.read())
    rng = random.Random(5)
    if args.fuzz:
        fuzz_http.ONLY = None if args.what == 'all' else args.what
        if args.ops:
            fuzz_http.OPS = [int(x) for x in args.ops.split(',')]
        fixed = [fuzz_http.case(rng) for _ in range(args.set)] if args.set else None
        label = f'{args.what} mutated' + (f', {args.set} fixed' if fixed else '')

        def nth(i, k=0):
            return fixed[(i + k * 251) % len(fixed)] if fixed else fuzz_http.case(rng)
    else:
        if args.what.isdigit():
            data = fuzz_http.SEEDS[int(args.what)]
        else:
            with open(args.what, 'rb') as f:
                data = f.read()
        label = repr(data[:30])

        def nth(i, k=0):
            return data
    wait = 0.02 if args.fuzz else 2
    jobs = args.jobs if args.fuzz and args.set else 1
    per = args.count // jobs
    marks = []
    server = fuzz_http.Server(args.server, dict(os.environ))

    def sender(k):
        kept = None
        for i in range(per):
            if k == 0 and i % max(1, per // 10) == 0:
                marks.append(server.rss_kb())
            if args.keep:
                if kept is None:
                    kept = socket.create_connection(('127.0.0.1', args.port))
                kept.sendall(nth(i, k))
                kept.recv(65536)
            elif args.cut:
                fuzz_http.send(nth(i, k), rng, args.port, timeout=0.002)
            else:
                one(args.port, nth(i, k), wait)

    threads = [threading.Thread(target=sender, args=(k,)) for k in range(jobs)]
    try:
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        marks.append(server.rss_kb())
    finally:
        server.stop()
    how = ' on one connection' if args.keep else ' cut' if args.cut else ''
    print(f'{label} x {args.count}{how} by {jobs} sender(s): rss KB {marks}')


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
"""fuzz_http.py <server> [--seconds N] [--seed S] [--out DIR] [--seeds DIR]

Stage 1 of fuzzing the http package: from the outside, over TCP. It starts
<server> -- the HttpArena entry (tools/bench/httparena/scaly/arena.scaly),
built with `scaly build --release` -- under SCALY_POISON=1 on ports of its
own with the TLS ports closed, and sends it mutated requests, one
connection each, the bytes cut into random pieces so that the resumable
parser meets every boundary:

  HTTP/1.1   the entry's routes, Content-Length and chunked bodies,
             pipelining, the WebSocket upgrade followed by frames
  HTTP/2     the client preface (h2c by prior knowledge, on the main port
             and on the h2c-only one), then frames: HEADERS with HPACK,
             DATA, gRPC, and frames of every type with lengths, flags,
             padding and stream IDs gone wrong

What it watches, and what it keeps under --out (default fuzz-out/):
  crash-N/   the process ended -- the last cases sent, its stderr
  hang-N/    alive, but a well-formed request went unanswered twice
  fault-N    a case after which the server wrote "a fault ended a
             connection" (caught, so the server lives on -- but every one
             is a defect in the package)
  rss        the server's resident memory, every ten seconds (a leak shows
             as a slope)

--seeds DIR adds every *.raw file in DIR as a seed (HttpArena's requests/).
--jobs N senders run at once (default 4); each one's cases repeat from
--seed, their interleaving does not -- a finding keeps the last cases of
all of them. --only h1|h2|ws sends one kind of case alone. replay.py sends
a finding's cases again, leak.py tells a leak from a high-water mark.
Nothing here needs more than the Python standard library.
"""
import argparse
import os
import random
import select
import socket
import struct
import subprocess
import sys
import threading
import time

PORT, H2C_PORT = 18080, 18082


# ---- seeds ----------------------------------------------------------------

def h1(method, path, headers=(), body=b''):
    lines = [f'{method} {path} HTTP/1.1'.encode(), b'Host: localhost']
    lines += [f'{k}: {v}'.encode() for k, v in headers]
    return b'\r\n'.join(lines) + b'\r\n\r\n' + body


def chunked(parts, trailer=b''):
    out = b''
    for p in parts:
        out += b'%x\r\n' % len(p) + p + b'\r\n'
    return out + b'0\r\n' + trailer + b'\r\n'


PREFACE = b'PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n'


def frame(kind, flags, stream, payload):
    return struct.pack('>I', len(payload))[1:] + bytes([kind, flags]) + struct.pack('>I', stream) + payload


def hpack_literal(name, value):
    # a literal without indexing, new name, no Huffman (lengths below 127)
    return b'\x00' + bytes([len(name)]) + name + bytes([len(value)]) + value


def h2_headers(method, path, extra=()):
    block = hpack_literal(b':method', method) + hpack_literal(b':scheme', b'http')
    block += hpack_literal(b':authority', b'localhost') + hpack_literal(b':path', path)
    for k, v in extra:
        block += hpack_literal(k, v)
    return block


SETTINGS = frame(4, 0, 0, b'')


def h2(method, path, extra=(), data=None):
    end = 0x4 | (0x1 if data is None else 0)
    out = PREFACE + SETTINGS + frame(1, end, 1, h2_headers(method, path, extra))
    if data is not None:
        out += frame(0, 0x1, 1, data)
    return out


def grpc_sum(a, b):
    msg = b'\x08' + varint(a) + b'\x10' + varint(b)
    return b'\x00' + struct.pack('>I', len(msg)) + msg


def varint(n):
    out = b''
    while True:
        b = n & 0x7f
        n >>= 7
        out += bytes([b | (0x80 if n else 0)])
        if not n:
            return out


def ws_frame(opcode, payload, fin=True, mask=True):
    head = bytes([(0x80 if fin else 0) | opcode])
    n = len(payload)
    m = 0x80 if mask else 0
    if n < 126:
        head += bytes([m | n])
    elif n < 65536:
        head += bytes([m | 126]) + struct.pack('>H', n)
    else:
        head += bytes([m | 127]) + struct.pack('>Q', n)
    if not mask:
        return head + payload
    key = b'\x01\x02\x03\x04'
    return head + key + bytes(c ^ key[i % 4] for i, c in enumerate(payload))


WS_UPGRADE = h1('GET', '/ws', [('Upgrade', 'websocket'), ('Connection', 'Upgrade'),
                               ('Sec-WebSocket-Key', 'dGhlIHNhbXBsZSBub25jZQ=='),
                               ('Sec-WebSocket-Version', '13')])

SEEDS = [
    h1('GET', '/baseline11?a=1&b=2'),
    h1('POST', '/baseline11?a=1&b=2', [('Content-Length', '2')], b'40'),
    h1('POST', '/baseline11?a=1&b=2', [('Transfer-Encoding', 'chunked')], chunked([b'4', b'0'])),
    h1('GET', '/baseline11?a=1&b=2') * 3,
    h1('GET', '/pipeline') + h1('GET', '/pipeline'),
    h1('GET', '/delay/1'),
    h1('GET', '/json/5?m=2', [('Accept-Encoding', 'gzip')]),
    h1('GET', '/json/50?m=1'),
    h1('POST', '/echo', [('Content-Length', '5')], b'hello'),
    h1('GET', '/static/index.html'),
    h1('GET', '/public/baseline?a=3&b=4'),
    h1('GET', '/api/items/7'),
    h1('GET', '/api/me', [('Authorization', 'Bearer x.y.z')]),
    h1('GET', '/nothing/here', [('Connection', 'close')]),
    WS_UPGRADE + ws_frame(1, b'hello') + ws_frame(2, b'\x00\x01') + ws_frame(9, b'p') + ws_frame(8, b'\x03\xe8'),
    WS_UPGRADE + ws_frame(1, b'frag', fin=False) + ws_frame(0, b'ment'),
    h2(b'GET', b'/baseline2?a=1&b=2'),
    h2(b'GET', b'/json/3?m=1', [(b'accept-encoding', b'gzip')]),
    h2(b'POST', b'/benchmark.BenchmarkService/GetSum',
       [(b'content-type', b'application/grpc'), (b'te', b'trailers')], grpc_sum(3, 4)),
    PREFACE + SETTINGS + frame(6, 0, 0, b'12345678') + frame(8, 0, 0, b'\x00\x00\x10\x00'),
]

ALPHABET = [b'\r\n', b'\r', b'\n', b'\x00', b' ', b':', b'\t', b'%', b'?', b'&', b'=',
            b'/', b'..', b'chunked', b'Content-Length: ', b'Transfer-Encoding: chunked\r\n',
            b'\r\n\r\n', b'HTTP/1.1', b'HTTP/1.0', b'HTTP/2.0', b'\xff', b'\x80']
NUMBERS = [b'0', b'-1', b'1', b'18446744073709551615', b'18446744073709551616',
           b'9223372036854775807', b'ffffffffffffffff', b'7fffffff', b'4294967296',
           b'000000000000000000000001', b'1e9', b'0x10', b' 5', b'5 ', b'+5', b'16777216']


# ---- mutation -------------------------------------------------------------

# the mutation operators allowed (leak.py narrows them; None: all eleven)
OPS = None


def mutate(rng, data):
    data = bytearray(data)
    for _ in range(rng.randint(1, 4)):
        op = rng.randrange(11) if OPS is None else rng.choice(OPS)
        n = len(data)
        if op == 0 and n:                                   # flip bits
            for _ in range(rng.randint(1, 8)):
                data[rng.randrange(n)] ^= 1 << rng.randrange(8)
        elif op == 1 and n:                                 # random byte
            data[rng.randrange(n)] = rng.randrange(256)
        elif op == 2:                                       # insert a token
            at = rng.randint(0, n)
            data[at:at] = rng.choice(ALPHABET) * rng.choice((1, 1, 2, 50))
        elif op == 3 and n:                                 # delete a span
            at = rng.randrange(n)
            del data[at:at + rng.randint(1, 16)]
        elif op == 4 and n:                                 # duplicate a span
            at = rng.randrange(n)
            span = data[at:at + rng.randint(1, 64)]
            data[at:at] = span * rng.choice((1, 2, 10, 300))
        elif op == 5:                                       # replace a number
            digits = [i for i, c in enumerate(data) if 48 <= c <= 57]
            if digits:
                at = rng.choice(digits)
                end = at
                while end < n and 48 <= data[end] <= 57:
                    end += 1
                data[at:end] = rng.choice(NUMBERS)
        elif op == 6 and n:                                 # truncate
            del data[rng.randrange(n):]
        elif op == 7:                                       # splice a seed
            other = rng.choice(SEEDS)
            at = rng.randint(0, n)
            data[at:at] = other[rng.randrange(len(other)):]
        elif op == 8:                                       # a long header
            data[0:0] = b''
            at = data.find(b'\r\n')
            if at > 0:
                name = rng.choice((b'X', b'Cookie', b'Host', b'Content-Length', b'A' * 300))
                data[at + 2:at + 2] = name + b': ' + b'v' * rng.choice((1, 4000, 17000, 70000)) + b'\r\n'
        elif op == 9:                                       # many headers
            at = data.find(b'\r\n')
            if at > 0:
                data[at + 2:at + 2] = b'X-A: b\r\n' * rng.choice((100, 1000, 5000))
        elif op == 10 and n > 8:                            # swap two spans
            a, b = sorted(rng.sample(range(n), 2))
            data[a:b] = data[a:b][::-1]
    return bytes(data)


def h2_random(rng):
    """The preface, then frames of any type with fields gone wrong."""
    out = PREFACE + (SETTINGS if rng.random() < 0.8 else b'')
    for _ in range(rng.randint(1, 12)):
        kind = rng.choice((0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 10, 0xff))
        flags = rng.choice((0, 1, 4, 5, 8, 0x20, 0x25, 0xff))
        stream = rng.choice((0, 1, 1, 3, 2, 0x7fffffff, rng.randrange(1 << 31)))
        if kind in (1, 9):
            payload = h2_headers(b'GET', rng.choice((b'/baseline2?a=1&b=2', b'/json/2', b'/x')))
            if rng.random() < 0.5:
                payload = mutate(rng, payload)
        elif kind == 4:
            payload = b''.join(struct.pack('>HI', rng.choice((1, 2, 3, 4, 5, 6, 0x99)),
                                           rng.choice((0, 1, 0x7fffffff, 0xffffffff, 16384, 16777215)))
                               for _ in range(rng.randint(0, 4)))
        elif kind == 8:
            payload = struct.pack('>I', rng.choice((0, 1, 0x7fffffff, 0x80000000, 0xffffffff)))
        elif kind == 0:
            payload = bytes(rng.randrange(256) for _ in range(rng.choice((0, 1, 9, 300))))
        else:
            payload = bytes(rng.randrange(256) for _ in range(rng.choice((0, 4, 5, 8, 9, 64))))
        if flags & 0x8 and payload and rng.random() < 0.7:           # padding
            payload = bytes([rng.choice((0, min(len(payload), 255), 255))]) + payload
        f = frame(kind, flags, stream, payload)
        if rng.random() < 0.1:                                       # a lying length
            f = struct.pack('>I', rng.choice((0, 16384, 16777215, len(payload) + 1)))[1:] + f[3:]
        out += f
        if kind == 3 and rng.random() < 0.5:                         # rapid reset
            out += (frame(1, 5, stream, h2_headers(b'GET', b'/baseline2')) + frame(3, 0, stream, b'\x00\x00\x00\x08')) * 50
    return out


def ws_random(rng):
    out = WS_UPGRADE
    for _ in range(rng.randint(1, 8)):
        opcode = rng.choice((0, 1, 2, 8, 9, 10, 3, 0xf))
        size = rng.choice((0, 1, 125, 126, 200, 65535, 65536, 70000))
        payload = bytes(rng.randrange(256) for _ in range(min(size, 2000))) * max(1, size // 2000)
        f = ws_frame(opcode, payload[:size], fin=rng.random() < 0.8, mask=rng.random() < 0.9)
        if rng.random() < 0.2:                                       # a lying 64-bit length
            f = bytes([f[0], 0x80 | 127]) + struct.pack('>Q', rng.choice((1 << 63, (1 << 64) - 1, 1 << 40))) + f[2:]
        out += f
    return out


ONLY = None


def case(rng):
    if ONLY == 'h2':
        return h2_random(rng) if rng.random() < 0.6 else mutate(rng, rng.choice([s for s in SEEDS if s.startswith(PREFACE)]))
    if ONLY == 'ws':
        return ws_random(rng)
    if ONLY == 'h1':
        return mutate(rng, rng.choice([s for s in SEEDS if not s.startswith(PREFACE) and not s.startswith(WS_UPGRADE)]))
    r = rng.random()
    if r < 0.12:
        return h2_random(rng)
    if r < 0.2:
        return ws_random(rng)
    return mutate(rng, rng.choice(SEEDS))


# ---- the server -----------------------------------------------------------

class Server:
    def __init__(self, binary, env):
        self.binary, self.env = binary, env
        self.lines = []
        self.lock = threading.Lock()
        self.start()

    def start(self):
        self.proc = subprocess.Popen([self.binary, str(PORT), '0', '0', str(H2C_PORT)],
                                     stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, env=self.env)
        threading.Thread(target=self.drain, args=(self.proc,), daemon=True).start()
        deadline = time.time() + 30
        while time.time() < deadline:
            if healthy(timeout=1):
                return
            time.sleep(0.2)
        raise SystemExit('fuzz_http: the server did not come up')

    def drain(self, proc):
        for line in proc.stderr:
            with self.lock:
                self.lines.append(line.decode(errors='replace').rstrip())

    def take(self):
        with self.lock:
            out, self.lines = self.lines, []
        return out

    def rss_kb(self):
        try:
            return int(subprocess.check_output(['ps', '-o', 'rss=', '-p', str(self.proc.pid)]).strip())
        except (subprocess.CalledProcessError, ValueError):
            return -1

    def stop(self):
        if self.proc.poll() is None:
            self.proc.kill()
        self.proc.wait()


def send(data, rng, port, timeout=0.05):
    """One connection: data out in random pieces, whatever comes back read."""
    try:
        s = socket.create_connection(('127.0.0.1', port), timeout=2)
    except OSError:
        return None
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    got = b''
    try:
        at = 0
        while at < len(data):
            n = len(data) - at if rng.random() < 0.5 else rng.randint(1, max(1, min(64, len(data) - at)))
            s.sendall(data[at:at + n])
            at += n
            if rng.random() < 0.05:
                time.sleep(0.002)
        if rng.random() < 0.3:
            s.shutdown(socket.SHUT_WR)
        end = time.time() + timeout
        s.setblocking(False)
        while time.time() < end and len(got) < 1 << 20:
            r, _, _ = select.select([s], [], [], max(0, end - time.time()))
            if not r:
                break
            chunk = s.recv(65536)
            if not chunk:
                break
            got += chunk
    except OSError:
        pass
    finally:
        s.close()
    return got


def healthy(timeout=2):
    try:
        s = socket.create_connection(('127.0.0.1', PORT), timeout=timeout)
        s.settimeout(timeout)
        s.sendall(h1('GET', '/baseline11?a=1&b=2'))
        got = b''
        while b'\r\n\r\n' not in got or not got.endswith(b'3'):
            chunk = s.recv(4096)
            if not chunk:
                break
            got += chunk
        s.close()
        return got.startswith(b'HTTP/1.1 200') and got.endswith(b'3')
    except OSError:
        return False


# ---- the run --------------------------------------------------------------

def keep(out, kind, n, recent, lines):
    d = os.path.join(out, f'{kind}-{n}')
    os.makedirs(d, exist_ok=True)
    for i, (port, data) in enumerate(recent):
        with open(os.path.join(d, f'case-{i}-{port}.bin'), 'wb') as f:
            f.write(data)
    with open(os.path.join(d, 'stderr.txt'), 'w') as f:
        f.write('\n'.join(lines) + '\n')
    return d


class Run:
    """The cases in flight: --jobs senders, each with a generator of its own
    (seeded from --seed and its number, so a run repeats), the last cases of
    all of them kept for a finding, a gate the monitor closes while it
    restarts the server."""

    def __init__(self, jobs, seed):
        self.lock = threading.Lock()
        self.recent = []
        self.sent = 0
        self.go = threading.Event()
        self.go.set()
        self.done = threading.Event()
        self.idle = threading.Semaphore(0)
        self.threads = [threading.Thread(target=self.sender, args=(random.Random(seed * 1000 + i),), daemon=True)
                        for i in range(jobs)]

    def sender(self, rng):
        while not self.done.is_set():
            if not self.go.is_set():
                self.idle.release()
                self.go.wait()
                continue
            data = case(rng)
            port = H2C_PORT if data.startswith(PREFACE) and rng.random() < 0.3 else PORT
            with self.lock:
                self.recent = (self.recent + [(port, data)])[-40:]
                self.sent += 1
            send(data, rng, port)

    def pause(self):
        self.go.clear()
        for _ in self.threads:
            self.idle.acquire()

    def last(self, n):
        with self.lock:
            return list(self.recent[-n:])


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('server')
    ap.add_argument('--seconds', type=int, default=600)
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--jobs', type=int, default=4)
    ap.add_argument('--out', default='fuzz-out')
    ap.add_argument('--seeds')
    ap.add_argument('--dataset', help='the dataset /json serves (DATASET_PATH)')
    ap.add_argument('--only', choices=('h1', 'h2', 'ws'), help='one kind of case alone (to find which one a finding needs)')
    args = ap.parse_args()
    global ONLY
    ONLY = args.only
    if args.seeds:
        for name in sorted(n for n in os.listdir(args.seeds) if n.endswith('.raw')):
            with open(os.path.join(args.seeds, name), 'rb') as f:
                SEEDS.append(f.read())
    os.makedirs(args.out, exist_ok=True)
    env = dict(os.environ, SCALY_POISON='1')
    if args.dataset:
        env['DATASET_PATH'] = args.dataset
    server = Server(args.server, env)
    found = {'crash': 0, 'hang': 0, 'fault': 0}
    run = Run(args.jobs, args.seed)
    try:
        for t in run.threads:
            t.start()
        start = last_rss = last_health = time.time()
        rss = open(os.path.join(args.out, 'rss'), 'w')
        while time.time() - start < args.seconds:
            time.sleep(0.05)
            for line in server.take():
                if 'a fault ended a connection' in line:
                    found['fault'] += 1
                    keep(args.out, 'fault', found['fault'], run.last(3 * args.jobs), [line])
                    print(f'fault {found["fault"]}: {line}', flush=True)
                else:
                    print(f'stderr: {line}', flush=True)
            dead = server.proc.poll() is not None
            hung = False
            if not dead and time.time() - last_health >= 1:
                last_health = time.time()
                hung = not healthy() and not healthy()
            if dead or hung:
                run.pause()
                kind = 'crash' if dead else 'hang'
                found[kind] += 1
                time.sleep(0.2)
                d = keep(args.out, kind, found[kind], run.last(40), server.take())
                print(f'{kind} {found[kind]} after {run.sent} cases (rc {server.proc.returncode}): {d}', flush=True)
                server.stop()
                server.start()
                with run.lock:
                    run.recent = []
                run.go.set()
            if time.time() - last_rss >= 10:
                last_rss = time.time()
                rss.write(f'{int(last_rss - start)} {run.sent} {server.rss_kb()}\n')
                rss.flush()
    finally:
        run.done.set()
        run.go.set()
        server.stop()
    print(f'fuzz_http: {run.sent} cases in {args.seconds} s, seed {args.seed}, {args.jobs} jobs: '
          f'{found["crash"]} crash, {found["hang"]} hang, {found["fault"]} fault', flush=True)
    return 1 if any(found.values()) else 0


if __name__ == '__main__':
    sys.exit(main())

#!/usr/bin/env python3
"""tools/bench/race.py — time Scaly against the benchmarks-game C champions.

Usage: tools/bench/race.sh [--rounds N] [--one-core] [--only mandelbrot,nbody,...]
(race.sh finds python3 and the work directory on the Windows box, too; on a
POSIX host race.py can be run directly.)

Runs the binaries tools/bench/build.sh left in BENCH_WORK/bin (default
/tmp/scaly-bench/bin). The rounds are INTERLEAVED — every program once per
round — so a machine that throttles as it warms up slows all of them alike
instead of whichever happens to run last; the table reports the median wall
time per program. On POSIX every run gets `ulimit -s 65520`, C included; on
Windows the stack is fixed at link time and the binaries (`.exe`) start
directly — a bare `bash` there may resolve to WSL's.

--one-core sets SCALY_WORKERS=1 and OMP_NUM_THREADS=1 and SKIPS, by name, the
C programs that choose their own thread count (spectral #4 from the CPU
affinity, fannkuch #6 and binary-trees #5 through pthreads) — their number
would not be a one-core number.

The outputs are printed at the end, one line per program (an md5 for
mandelbrot's binary image): the Scaly programs of a problem must agree with
each other, the C programs with each other.
"""
import hashlib
import os
import statistics
import subprocess
import sys
import time

W = os.environ.get("BENCH_WORK", "/tmp/scaly-bench")
B = os.path.join(W, "bin")
WINDOWS = os.name == "nt"
EXE = ".exe" if WINDOWS else ""

# (problem, label, binary, args, threads settable by OMP_NUM_THREADS/SCALY_WORKERS)
PROGRAMS = [
    ("mandelbrot", "scaly", "mandelbrot", [], True),
    ("mandelbrot", "scaly native", "mandelbrot_native", [], True),
    ("mandelbrot", "scaly simd", "mandelbrot_simd", [], True),
    ("mandelbrot", "scaly simd native", "mandelbrot_simd_native", [], True),
    ("mandelbrot", "C #6 (SSE)", "mandel6", ["8000"], True),
    ("mandelbrot", "C #5", "mandel5", ["8000"], True),
    ("spectral", "scaly", "spectralnorm", [], True),
    ("spectral", "scaly native", "spectralnorm_native", [], True),
    ("spectral", "scaly simd", "spectralnorm_simd", [], True),
    ("spectral", "scaly simd native", "spectralnorm_simd_native", [], True),
    ("spectral", "C #5 (SSE)", "spectral5", ["10000"], True),
    ("spectral", "C #4", "spectral4", ["10000"], False),
    ("nbody", "scaly", "nbody", [], True),
    ("nbody", "scaly native", "nbody_native", [], True),
    ("nbody", "C #4 (SSE)", "nbody4", ["50000000"], True),
    ("nbody", "C #6", "nbody6", ["50000000"], True),
    ("fannkuch", "scaly", "fannkuch", [], True),
    ("fannkuch", "scaly native", "fannkuch_native", [], True),
    ("fannkuch", "C #6 (SSE)", "fannkuch6", ["11"], False),
    ("fannkuch", "C #5", "fannkuch5", ["11"], True),
    ("binarytrees", "scaly", "binarytrees", [], True),
    ("binarytrees", "scaly native", "binarytrees_native", [], True),
    ("binarytrees", "C #5", "btree5", ["21"], False),
]


def run(path, args, env):
    if WINDOWS:
        cmd = [path] + args
    else:
        cmd = ["bash", "-c", 'ulimit -s 65520 2>/dev/null; exec "$0" "$@"', path] + args
    t0 = time.perf_counter()
    p = subprocess.run(cmd, env=env, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    return time.perf_counter() - t0, p.returncode, p.stdout


def summary(out):
    try:
        text = out.decode("ascii")
        if "\x00" not in text:
            lines = [l for l in text.splitlines() if l.strip()]
            return " | ".join(lines[-2:]) if lines else "(no output)"
    except UnicodeDecodeError:
        pass
    return "md5 " + hashlib.md5(out).hexdigest()


def main():
    rounds = 3
    one_core = False
    only = None
    a = sys.argv[1:]
    while a:
        x = a.pop(0)
        if x == "--rounds":
            rounds = int(a.pop(0))
        elif x == "--one-core":
            one_core = True
        elif x == "--only":
            only = set(a.pop(0).split(","))
        else:
            sys.exit(__doc__)
    env = dict(os.environ)
    if one_core:
        env["SCALY_WORKERS"] = "1"
        env["OMP_NUM_THREADS"] = "1"
    progs = []
    for prob, label, binary, args, settable in PROGRAMS:
        if only and prob not in only:
            continue
        path = os.path.join(B, binary + EXE)
        if not os.path.exists(path):
            print(f"skip {prob} {label}: {path} not built")
            continue
        if one_core and not settable:
            print(f"skip {prob} {label}: sets its own thread count")
            continue
        progs.append((prob, label, path, args))
    times = {i: [] for i in range(len(progs))}
    outs = {}
    for r in range(rounds):
        for i, (prob, label, path, args) in enumerate(progs):
            t, rc, out = run(path, args, env)
            if rc != 0:
                print(f"{prob} {label}: exit {rc}")
            times[i].append(t)
            outs.setdefault(i, out)
        print(f"round {r + 1} of {rounds} done", file=sys.stderr)
    mode = "one core" if one_core else "all cores"
    print(f"\n{mode}, median of {rounds} interleaved rounds (wall seconds; min .. max)")
    last = None
    for i, (prob, label, path, args) in enumerate(progs):
        if prob != last:
            print(f"\n{prob}")
            last = prob
        ts = times[i]
        print(f"  {label:20s} {statistics.median(ts):7.2f}   ({min(ts):.2f} .. {max(ts):.2f})")
    print("\noutputs")
    for i, (prob, label, path, args) in enumerate(progs):
        print(f"  {prob:12s} {label:20s} {summary(outs[i])}")


if __name__ == "__main__":
    main()

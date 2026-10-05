#!/usr/bin/env python3
"""tests/fiber/run.py -- the fiber suite's fixture loop (tests/fiber/run.sh is its
entry point and does the set-up: the platform, the stage compiler and its tool).

Each fixture is built with the tool and run; it passes when its exit code, its
stdout and (where given) its stderr are what its markers say:

  ; Expected: <line>       stdout, one marker per line (default: empty)
  ; ExpectedExit: <n>      the exit code (default 0)
  ; ExpectedErr: <text>    stderr must contain it (several: any of them, as the
                           loop's `grep -F` read a multi-line pattern)
  ; Timeout: <seconds>     killed after that long (the loop's `perl alarm`)
  ; Isolated: <reason>     run ALONE, after the others: its verdict counts
                           forks, submits or steals, which follow measured time,
                           and a core shared with another fixture moves them

A driver and not a shell loop since 2026-10-03:
the loop ran the 65 fixtures one after another, each wrapped in a handful of
processes, and Git Bash emulates every fork. Here every build runs in parallel,
the programs without `Isolated` run in parallel, and the isolated ones one at a
time with the machine to themselves -- which matters most on a machine with few
cores, where the contention a parallel run adds is largest.
"""
import concurrent.futures
import glob
import os
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def markers(text, key):
    prefix = "; %s: " % key
    return [line[len(prefix):] for line in text.split("\n") if line.startswith(prefix)]


def exit_text(rc):
    # what bash's $? said: a signal is 128 + its number
    return str(128 - rc if rc < 0 else rc)


class Fixture:
    def __init__(self, path, tmp, exe):
        self.path = os.path.relpath(path, REPO).replace("\\", "/")
        self.name = os.path.basename(path)[:-len(".scaly")]
        text = open(path, encoding="utf-8", errors="replace").read()
        self.expected = "\n".join(markers(text, "Expected")).rstrip("\n")
        self.want_rc = (markers(text, "ExpectedExit") or ["0"])[0] or "0"
        self.want_err = [w for w in markers(text, "ExpectedErr") if w]
        limit = markers(text, "Timeout")
        self.timeout = float(limit[0]) if limit and limit[0].strip() else None
        self.isolated = bool(markers(text, "Isolated"))
        self.binp = os.path.join(tmp, "fiber_" + self.name + exe)
        self.errp = os.path.join(tmp, "fiber_" + self.name + ".err")
        self.built = False
        self.verdict = None


def build(fx, scaly):
    if os.path.exists(fx.binp):
        os.remove(fx.binp)
    p = subprocess.run([scaly, "build", fx.path, "-o", fx.binp], cwd=REPO,
                       stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                       stderr=subprocess.DEVNULL)
    fx.built = p.returncode == 0
    if not fx.built:
        fx.verdict = (False, fx.name + "(compile)")


def run(fx):
    with open(fx.errp, "wb") as err:
        try:
            p = subprocess.run([fx.binp], cwd=REPO, stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE, stderr=err, timeout=fx.timeout)
            rc, out = exit_text(p.returncode), p.stdout
        except subprocess.TimeoutExpired as e:
            rc, out = "142", e.stdout or b""      # SIGALRM, as perl's alarm ended it
    out = out.decode(errors="replace").rstrip("\n")
    errtext = open(fx.errp, "rb").read().decode(errors="replace")
    ok = rc == fx.want_rc and out == fx.expected
    if fx.want_err and not any(w in errtext for w in fx.want_err):
        ok = False
    if ok:
        fx.verdict = (True, None)
    else:
        err1 = errtext.split("\n")[0] if errtext else ""
        fx.verdict = (False, "%s: rc=%s '%s'%s" % (fx.name, rc, out,
                                                  " stderr: '%s'" % err1 if err1 else ""))


def main():
    # run.py <tool> <exe suffix> <tmp> <jobs>
    scaly, exe, tmp, jobs = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4])
    fixtures = [Fixture(p, tmp, exe)
                for p in sorted(glob.glob(os.path.join(REPO, "tests", "fiber", "*.scaly")))]
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        list(pool.map(lambda fx: build(fx, scaly), fixtures))
        list(pool.map(run, [fx for fx in fixtures if fx.built and not fx.isolated]))
    for fx in fixtures:
        if fx.built and fx.isolated:
            run(fx)
    failures = [fx.verdict[1] for fx in fixtures if not fx.verdict[0]]
    print("fiber: %d PASS, %d FAIL %s" % (len(fixtures) - len(failures), len(failures),
                                           " ".join(failures)))
    sys.exit(0 if not failures else 1)


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""tests/escape/run.py -- the escape suite's fixture loop (tests/escape/run.sh is
its entry point; it raises the stack limit, which the compiles inherit).

Compile-only: a negative fixture must be REJECTED with its diagnostic, a positive
one must compile. The groups, as the shell loops had them:

  neg_*   --no-prelude  rejected, "escapes"          pos_*   --no-prelude  compiles
  negp_*  prelude       rejected, "escapes"          posp_*  prelude       compiles
  negm_*  prelude       rejected, "use after send"   posm_*  prelude       compiles

plus two diagnostic LOCATION checks over multi-file roots (mfloc_main,
mfloc_call_main): the escape must be reported in the HELPER file at its line.

A driver and not six shell loops since 2026-10-03:
the fixtures were compiled one after another, each inside a subshell, and
Git Bash emulates every fork. Here they compile in parallel; the summary line
and the exit code are the loops'.

  run.py <compiler> <tmp dir> [jobs]
"""
import concurrent.futures
import glob
import os
import re
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# prefix, compile with the prelude?, expected diagnostic (None: must compile)
GROUPS = [("neg_", False, "escapes"), ("pos_", False, None),
          ("negp_", True, "escapes"), ("posp_", True, None),
          ("negm_", True, "use after send"), ("posm_", True, None)]
# root, regex a line of the output must match (grep's reading: one line at a time)
LOCATIONS = [("mfloc", "tests/escape/mfloc_main.scaly", r"mfloc_helper\.scaly:13:.*escapes",
              "mfloc_helper.scaly:13"),
             ("mfloc_call", "tests/escape/mfloc_call_main.scaly", r"mfloc_call_helper\.scaly:23:.*escapes",
              "mfloc_call_helper.scaly:23")]


def compile_(cc, tmp, name, path, prelude):
    args = [cc, "-c"] + ([] if prelude else ["--no-prelude"]) + ["-o", os.path.join(tmp, "esc_%s.o" % name), path]
    p = subprocess.run(args, cwd=REPO, stdin=subprocess.DEVNULL,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    # $( ... ) dropped the trailing newlines
    return p.returncode, p.stdout.decode(errors="replace").rstrip("\n")


def fixture(cc, tmp, path, prelude, want):
    t = os.path.basename(path)[:-len(".scaly")]
    rc, out = compile_(cc, tmp, t, os.path.relpath(path, REPO).replace("\\", "/"), prelude)
    if want is None:
        return None if rc == 0 else "%s: expected compile, got rc=%d: %s" % (t, rc, out)
    if rc != 0 and want in out:
        return None
    return "%s: expected rejection, got rc=%d" % (t, rc)


def location(cc, tmp, name, root, pattern, where):
    rc, out = compile_(cc, tmp, name, root, False)
    if rc != 0 and any(re.search(pattern, line) for line in out.split("\n")):
        return None
    return "%s: expected %s escape, got rc=%d: %s" % (name, where, rc, out)


def main():
    cc, tmp = sys.argv[1], sys.argv[2]
    jobs = int(sys.argv[3]) if len(sys.argv) > 3 else (os.cpu_count() or 4)
    work = []
    for prefix, prelude, want in GROUPS:
        for path in sorted(glob.glob(os.path.join(REPO, "tests", "escape", prefix + "*.scaly"))):
            work.append((fixture, (cc, tmp, path, prelude, want)))
    for name, root, pattern, where in LOCATIONS:
        work.append((location, (cc, tmp, name, root, pattern, where)))
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        results = list(pool.map(lambda w: w[0](*w[1]), work))
    failures = [r for r in results if r is not None]
    print("escape: %d PASS, %d FAIL %s" % (len(results) - len(failures), len(failures), " ".join(failures)))
    sys.exit(0 if not failures else 1)


if __name__ == "__main__":
    main()

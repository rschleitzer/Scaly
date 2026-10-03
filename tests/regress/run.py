#!/usr/bin/env python3
"""tests/regress/run.py -- the regression suite's fixture loop (tests/regress/run.sh
is its entry point and does the set-up: the platform, the stage compiler and its
tool, the LLVM link flags, the warm-up build).

One fixture -> one verdict, `PASS name` or `FAIL name: detail`; the summary line
and the exit code are run.sh's of before. The branches, by MARKER in the fixture
(never by name, except `xfail_`):

  xfail_*        the compile must FAIL, and its output (stdout+stderr) must
                 contain the `; xfail:` text
  ; expect-ir:   compiled with -S; the IR must contain every such line
  ; expect-rc:   must compile, then abort with that code; `; expect-out:` lines
                 must appear in its output
  (otherwise)    must compile and print exactly PASS on stdout
  ; env: K=V     environment of the COMPILE;  ; run-env: K=V  of the PROGRAM
  ; args: ...    compiler flags;  ; link: llvm  the libLLVM link flags

A driver and not `xargs -P bash -c run_one` since 2026-10-03
(tests/win32/WINDOWS-BOX.md §8): each fixture started a bash and some ten
processes around the one compile and the one run, and Git Bash emulates every
fork -- measured on the Windows box, the median fixture spent 1.3 s in that set-up
against 0.7 s compiling and linking.
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


def env_with(pairs):
    env = dict(os.environ)
    for kv in pairs:
        if kv:
            k, _, v = kv.partition("=")
            env[k] = v
    return env


def words(lines):
    out = []
    for line in lines:
        out += line.split()
    return out


def exit_text(rc):
    # what bash's $? said: a signal is 128 + its number
    return str(128 - rc if rc < 0 else rc)


def one_line(text):
    return text.replace("\n", " ")


def run(cmd, env=None, merge=False, timeout=None):
    p = subprocess.run(cmd, cwd=REPO, env=env, stdin=subprocess.DEVNULL,
                       stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT if merge else subprocess.PIPE,
                       timeout=timeout)
    return p.returncode, p.stdout.decode(errors="replace"), \
        (p.stderr.decode(errors="replace") if not merge else "")


def run_one(path, stage, scaly, exe, tmp, llvm):
    t = os.path.basename(path)[:-len(".scaly")]
    text = open(path, encoding="utf-8", errors="replace").read()
    # relative, as the loop passed it: a diagnostic names the file as given
    path = os.path.relpath(path, REPO).replace("\\", "/")
    binp = os.path.join(tmp, "rt_" + t + exe)
    if os.path.exists(binp):
        os.remove(binp)
    env_compile = env_with(markers(text, "env"))
    args = words(markers(text, "args"))

    if t.startswith("xfail_"):
        want = (markers(text, "xfail") or [""])[0]
        rc, out, _ = run([scaly, "build", path] + args + ["-o", binp], env=env_compile, merge=True)
        # grep -F "" matches any line, but no line of an empty output
        if rc != 0 and want in out and (want or out):
            return "PASS " + t
        return "FAIL %s: rc=%d '%s'" % (t, rc, one_line(out.rstrip("\n")))

    extra = llvm if markers(text, "link") and markers(text, "link")[0].startswith("llvm") else []

    if markers(text, "expect-ir"):
        ll = binp + ".ll"
        rc, out, _ = run([stage, "-S"] + args + ["-o", ll, path], env=env_compile, merge=True)
        missing = ""
        if rc == 0:
            ir = open(ll, encoding="utf-8", errors="replace").read()
            for w in markers(text, "expect-ir"):
                if w and w not in ir:
                    missing = w
        if os.path.exists(ll):
            os.remove(ll)
        if rc == 0 and not missing:
            return "PASS " + t
        return "FAIL %s: rc=%d missing='%s' '%s'" % (t, rc, missing, one_line(out.rstrip("\n")))

    if markers(text, "expect-rc"):
        want_rc = markers(text, "expect-rc")[0]
        crc, _, _ = run([scaly, "build", path, "-o", binp] + extra, merge=True)
        if crc != 0:
            return "FAIL %s: compile failed rc=%d" % (t, crc)
        rc, out, _ = run([binp], merge=True)
        out = out.rstrip("\n")
        missing = ""
        for w in markers(text, "expect-out"):
            if w and w not in out:
                missing = w
        if exit_text(rc) == want_rc and not missing:
            return "PASS " + t
        return "FAIL %s: rc=%s want=%s missing='%s' out='%s'" % (t, exit_text(rc), want_rc, missing, one_line(out))

    crc, cout, _ = run([scaly, "build", path] + args + ["-o", binp] + extra, merge=True)
    if os.path.exists(binp):
        rc, out, err = run([binp], env=env_with(markers(text, "run-env")))
    else:
        rc, out, err = 127, "", "%s: No such file or directory\n" % binp
    out = out.rstrip("\n")
    if out == "PASS":
        return "PASS " + t
    detail = "rc=" + exit_text(rc)
    if crc != 0:
        detail += " compile-rc=%d compile='%s'" % (crc, one_line("\n".join(cout.rstrip("\n").split("\n")[-2:]) + "\n"))
    if err:
        detail += " stderr='%s'" % one_line(err[:300])
    return "FAIL %s: '%s' %s" % (t, one_line(out), detail)


def main():
    # run.py <stage> <tool> <exe suffix> <tmp> <jobs> [llvm link flags...]
    stage, scaly, exe, tmp, jobs = sys.argv[1:6]
    llvm = sys.argv[6:]
    fixtures = sorted(glob.glob(os.path.join(REPO, "tests", "regress", "*.scaly")))
    with concurrent.futures.ThreadPoolExecutor(max_workers=int(jobs)) as pool:
        verdicts = list(pool.map(lambda f: run_one(f, stage, scaly, exe, tmp, llvm), fixtures))
    fails = [v[len("FAIL "):] for v in verdicts if v.startswith("FAIL ")]
    passed = len(verdicts) - len(fails)
    print("regress: %d PASS, %d FAIL %s" % (passed, len(fails), "".join(f + " " for f in fails)))
    sys.exit(0 if not fails else 1)


if __name__ == "__main__":
    main()

#!/bin/bash
# The AOT + fiber corpora on Windows (stage 7, breadth after the substrate).
#
# ONE script, TWO modes, because the selection logic must not exist twice:
#
#   corpus.sh emit <compiler> <outdir>   on the LINUX leg — cross-emits one
#                                        Windows object per eligible test
#   corpus.sh run  <outdir> <archive>    on the WINDOWS runner — links each
#                                        object and checks what it printed
#
# The expectations are read from the SOURCES in both modes (the Windows job
# checks the repo out too), so there is no manifest to drift out of step.
#
# ELIGIBILITY: a test is eligible if it carries its own ground truth in ANY of
# the three forms below. A test with none of them is cross-checked on POSIX
# against a reference binary built by another compiler, which cannot exist on
# the Windows runner where nothing but the cross-emitted object arrives. Those
# are SKIPPED and the count is printed: a corpus that quietly covered less than
# it looked like would be worse than a smaller one that says so.
#
# ★The first version required `; Expected:` and thereby dropped the three most
# interesting fiber cases — `guard_overflow` (exit 108 plus a stderr
# substring), which is the only thing that exercises the Windows vectored
# exception handler at all, and the two deadlock detectors (exit 106). Their
# truth is an exit code, not stdout. Checking for the WIDEST form of ground
# truth rather than the most common one is what keeps a corpus honest.
#
# Conventions honoured, same as the POSIX drivers:
#   ; Expected:      exact stdout (required for eligibility)
#   ; ExpectedExit:  exit code, default 0
#   ; ExpectedErr:   substring that must appear on stderr
#
# Every run is under a TIMEOUT. A hang is the one outcome worse than a failure
# here — a fiber test that waits on something Windows reports differently would
# otherwise stall the job until the six-hour job limit, and tell us nothing.
set -u
cd "$(dirname "$0")/../.." || exit 1

MODE=${1:-}
SUITES="aot fiber"
TIMEOUT_S=60
TRIPLE=x86_64-pc-windows-msvc

# Objects are named <suite>__<test>.o so the run mode can find the source again
# without a manifest.
src_of() {
  local base=$1
  echo "tests/${base%%__*}/${base#*__}.scaly"
}

case "$MODE" in
emit)
  SC=${2:?usage: corpus.sh emit <compiler> <outdir>}
  OUT=${3:?usage: corpus.sh emit <compiler> <outdir>}
  mkdir -p "$OUT"
  n=0; skipped=0; failed=0; names=""
  for s in $SUITES; do
    for f in tests/$s/*.scaly; do
      [ -f "$f" ] || continue
      t=$(basename "$f" .scaly)
      if ! grep -q '^; Expected\(Exit\|Err\)\?: ' "$f"; then
        skipped=$((skipped+1)); continue
      fi
      if "$SC" -c --target "$TRIPLE" -o "$OUT/${s}__${t}.o" "$f" > /dev/null 2>&1; then
        n=$((n+1))
      else
        failed=$((failed+1)); names="$names ${s}/${t}"
      fi
    done
  done
  echo "corpus emit: $n objects, $skipped skipped (no self-contained ground truth), $failed cross-emit failures$names"
  [ "$failed" -eq 0 ]
  ;;

run)
  OUT=${2:?usage: corpus.sh run <outdir> <archive>}
  # ★NOT named LIB, and that is not cosmetic (2026-08-10). `LIB` is MSVC's
  # library search path, and vcvars64.bat EXPORTS it — so on any box where the
  # caller has a developer environment (which on Windows is how clang finds the
  # SDK at all), assigning to `LIB` here inherits the export attribute and
  # replaces those search paths with this one archive. Every one of the 90 links
  # then fails with `could not open 'ws2_32.lib'`, pointing at the SDK rather
  # than at the harness. CI never saw it because its runner has a registered VS
  # install that clang detects on its own, with no LIB in the environment.
  ARCHIVE=${3:?usage: corpus.sh run <outdir> <archive>}
  # Git Bash usually ships coreutils' timeout; without it the guard degrades to
  # "no guard", which is said out loud rather than assumed.
  TO=""
  if command -v timeout > /dev/null 2>&1; then
    TO="timeout $TIMEOUT_S"
  else
    echo "corpus run: WARNING — no timeout(1); a hanging test will stall the job"
  fi
  pass=0; fail=0; failures=""
  rm -f "$OUT/.linkerrs"
  # Detail is printed for the first few run failures only. A corpus of 90 can
  # fail wide, and an unbounded dump buries the aggregate below it.
  verbose_left=6
  for o in "$OUT"/*.o; do
    [ -f "$o" ] || continue
    base=$(basename "$o" .o)
    src=$(src_of "$base")
    if [ ! -f "$src" ]; then
      fail=$((fail+1)); failures="$failures $base(no-source)"; continue
    fi
    want=$(sed -n 's/^; Expected: //p' "$src")
    want_rc=$(sed -n 's/^; ExpectedExit: //p' "$src"); want_rc=${want_rc:-0}
    want_err=$(sed -n 's/^; ExpectedErr: //p' "$src")

    exe="$OUT/$base.exe"
    if ! clang --target="$TRIPLE" "$o" "$ARCHIVE" -lws2_32 -o "$exe" > "$OUT/$base.link" 2>&1; then
      fail=$((fail+1)); failures="$failures $base(link)"
      # The first error line of every failed link, collected for the distinct
      # summary below. Aggregating SYMBOLS alone was not enough: it reported two
      # names while twenty-two links had failed, so eighteen failures carried a
      # message of a shape nothing was looking for. Distinct MESSAGES cannot
      # have that blind spot.
      grep -i -m1 "error" "$OUT/$base.link" >> "$OUT/.linkerrs" 2>/dev/null \
        || echo "(no line matching 'error' in $base.link)" >> "$OUT/.linkerrs"
      continue
    fi
    # shellcheck disable=SC2086
    # ★NO PIPELINE HERE. `got=$(cmd | tr -d '\r'); rc=$?` reads the status of
    # `tr`, not of the program — so `rc` was 0 for EVERY test, no matter what
    # the program returned, and the timeout guard below (rc = 124) was blind
    # with it. That silently broke all three exit-code expectations
    # (guard_overflow 108, both deadlock detectors 106) and, worse, hid the
    # exit code of every failing test, which is the one number that says WHERE
    # a program stopped. Same class as the `| tail` trap CLAUDE.md records for
    # tools/aot_corpus.sh; the fix here is to have no pipeline at all rather
    # than to remember PIPESTATUS.
    $TO "$exe" > "$OUT/$base.out" 2> "$OUT/$base.err"; rc=$?
    got=$(tr -d '\r' < "$OUT/$base.out")
    # WHY each check is reported separately: the first version printed only
    # "rc=N", which collapsed "wrong output", "wrong exit code" and "missing
    # stderr text" into one indistinguishable label and made 27 failures
    # undiagnosable. A harness that cannot say WHICH expectation broke costs a
    # whole CI round per question.
    why=""
    [ "$rc" = "$want_rc" ] || why="$why,exit(want $want_rc got $rc)"
    [ "$got" = "$want" ] || why="$why,stdout"
    if [ -n "$want_err" ] && ! grep -q "$want_err" "$OUT/$base.err"; then
      why="$why,stderr"
    fi
    if [ -z "$why" ]; then
      pass=$((pass+1))
    else
      fail=$((fail+1))
      [ "$rc" = 124 ] && why=",TIMEOUT"
      failures="$failures $base(${why#,})"
      if [ "${why#,}" != "TIMEOUT" ] && [ "$verbose_left" -gt 0 ]; then
        verbose_left=$((verbose_left-1))
        echo "  --- $base ---"
        echo "      want stdout: $want"
        echo "      got  stdout: $got"
        # ★stderr is printed UNCONDITIONALLY, and says `(empty)` when it is
        # (2026-08-11). It used to be guarded by `[ -s ]`, so an empty stderr
        # and an unprinted one looked identical — and for the one test whose
        # entire content is a stderr message, `guard_overflow`, the EMPTINESS
        # is the finding: it separates "the handler ran and the terminator
        # failed" from "no handler ran at all", which are different defects
        # with different fixes. A flake that leaves no evidence behind costs a
        # CI round every time it fires. The wanted substring rides along for
        # the same reason: a mismatch is unreadable without both halves.
        # ★The RAW Windows exit code, asked for rather than inferred
        # (2026-09-19). What bash reports here is a POSIX wait status, eight
        # bits wide, and MSYS maps an abnormal termination onto it: the six
        # catching programs all read `127`, which is also bash's own "command
        # not found" and was already the reported code once before, for a CRT
        # invalid-parameter kill (CLAUDE-tooling.md). Three very different
        # deaths therefore look alike. The NTSTATUS separates them —
        # 0xC0000005 access violation, 0xC0000409 fast fail (which is what the
        # CRT's invalid-parameter handler raises), 0xC0000028 bad stack — and
        # only the launcher that started the process can still report it, so
        # the program is run a SECOND time, under PowerShell, for its code
        # alone. Diagnosis only: the verdict above stands on the first run.
        if command -v powershell > /dev/null 2>&1 && command -v cygpath > /dev/null 2>&1; then
          winexe=$(cygpath -w "$exe")
          raw=$($TO powershell -NoProfile -NonInteractive -Command \
                  "\$p = Start-Process -FilePath '$winexe' -NoNewWindow -PassThru -Wait; '{0:X8}' -f \$p.ExitCode" \
                2>/dev/null | tr -d '\r' | tail -1)
          [ -n "$raw" ] && echo "      raw exit code: 0x$raw (bash saw $rc)"
        fi
        if [ -n "$want_err" ]; then echo "      want stderr: $want_err"; fi
        if [ -s "$OUT/$base.err" ]; then
          echo "      got  stderr: $(head -c 300 "$OUT/$base.err" | tr -d '\r' | tr '\n' ' ')"
        else
          echo "      got  stderr: (empty)"
        fi
      fi
    fi
  done

  # The undefined symbols AGGREGATED across every failed link. Individually the
  # logs are noise; the distinct set is the actual work list, and it is short.
  #
  # ★BOTH message formats, because assuming one cost a CI round: the clang
  # driver on Windows uses MSVC's link.exe when Visual Studio is present
  # ("error LNK2019: unresolved external symbol X referenced in ...") and
  # lld-link otherwise ("undefined symbol: X"). And if neither matches, the head
  # of one log is dumped verbatim — a diagnostic that silently finds nothing is
  # the failure this whole harness keeps re-learning.
  if ls "$OUT"/*.link > /dev/null 2>&1; then
    undef=$( { grep -ho "undefined symbol: [^ ]*" "$OUT"/*.link 2>/dev/null \
                 | sed 's/^undefined symbol: //'
               grep -ho "unresolved external symbol [^ ]*" "$OUT"/*.link 2>/dev/null \
                 | sed 's/^unresolved external symbol //'
             } | sed 's/[",]*$//' | sort -u )
    if [ -n "$undef" ]; then
      echo "  --- distinct undefined symbols across all failed links ---"
      printf '%s\n' "$undef" | sed 's/^/      /'
    fi
  fi

  # The distinct first-error lines, with how many links each accounts for.
  # This is the diagnosis that cannot go blind: whatever the message looks
  # like, it appears here with a count, and the counts must add up to the
  # number of (link) failures.
  if [ -f "$OUT/.linkerrs" ]; then
    echo "  --- distinct link errors (count x message) ---"
    sed 's/^.*: error/error/' "$OUT/.linkerrs" | sort | uniq -c \
      | sort -rn | head -12 | sed 's/^/      /'
  fi

  echo "corpus run: $pass PASS, $fail FAIL$failures"
  [ "$fail" -eq 0 ]
  ;;

*)
  echo "usage: tests/win32/corpus.sh emit <compiler> <outdir>"
  echo "       tests/win32/corpus.sh run  <outdir> <archive>"
  exit 2
  ;;
esac

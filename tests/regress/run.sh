#!/bin/bash
# Self-hosted regression suite — fixes NOT present in the frozen C++ stage-0.
#
# Step-C/D-era corrections (s192+) sometimes make the self-hosted compiler
# strictly MORE correct than stage-0 (e.g. use-aliased free-function call
# mangling: stage-0 silently drops the call). Such cases can't live in the
# stage-0-referenced AOT corpus or the stage-0-baselined selfhosted suite, so
# they run only against a self-hosted stage binary. Each test compiles+runs
# and must print PASS.
#
# Usage: tests/regress/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$(tools/scaly-of.sh "$STAGE")
# Every fixture is built with the stage's `build` command, its runtime out of
# the build cache. Build one program first, alone: the fixtures run in parallel,
# and each would otherwise compile the runtime into an empty cache at once.
"$SCALY" build tests/tool/hello.scaly -o "/tmp/rt_warm_$$$SCALY_EXE" > /dev/null 2>&1
rm -f "/tmp/rt_warm_$$$SCALY_EXE"
# One fixture per call, its verdict as ONE line on stdout (`PASS name` or
# `FAIL name: detail`), so the fixtures run in parallel (REGRESS_JOBS, default
# every core; 1 is the old serial loop). Each fixture owns its binary
# /tmp/rt_<name> and its temp files, and since 2026-09-23 every compile links
# from an object named by its PID, so concurrent fixtures share nothing.
report_fail() { printf 'FAIL %s\n' "$(printf '%s' "$*" | tr '\n' ' ')"; }
run_one() {
  f=$1
  t=$(basename "$f" .scaly)
  bin=/tmp/rt_$t$SCALY_EXE; rm -f "$bin"
  case "$t" in
    xfail_*)
      # Expected-failure test: the compile must FAIL loudly. PASS when the
      # compiler exits nonzero AND its output contains the `; xfail:`
      # substring (stdout+stderr — emitter traps print to stderr, planner
      # diagnostics via Console.println to stdout).
      want=$(sed -n 's/^; xfail: //p' "$f" | head -1)
      # `; env: VAR=VALUE` (repeatable) sets environment for the compile only.
      # Needed by guards whose trip point depends on the machine rather than on
      # the source — xfail_nesting_too_deep pins the stack guard's budget so the
      # test does not depend on the caller's `ulimit -s`.
      env_args=()
      while IFS= read -r kv; do [ -n "$kv" ] && env_args+=("$kv"); done < <(sed -n 's/^; env: //p' "$f")
      # `; args: <flags>` (repeatable) passes compiler flags. Needed when the
      # fixture is a PROGRAM ROOT of its own SCALY_HOME: a home holding only the
      # fixture's package has no prelude, so the compile would fail with
      # "prelude not found" — the wrong reason for an xfail to pass.
      arg_args=()
      while IFS= read -r a; do [ -n "$a" ] && arg_args+=($a); done < <(sed -n 's/^; args: //p' "$f")
      err=$(env "${env_args[@]}" "$SCALY" build "$f" "${arg_args[@]}" -o "$bin" 2>&1); rc=$?
      if [ $rc -ne 0 ] && printf '%s' "$err" | grep -qF "$want"; then
        echo "PASS $t"
      else
        report_fail "$t: rc=$rc '$err'"
      fi
      ;;
    *)
      # `; link: llvm` in the test file appends the libLLVM link flags
      # (for tests exercising extern LLVM-C calls, e.g. the wrapper-param
      # unwrap). llvm-env.sh is sourced lazily on first use.
      extra=()
      if grep -q '^; link: llvm' "$f"; then
        if [ -z "$LLVM_LIBDIR" ]; then source tools/llvm-env.sh >/dev/null; fi
        extra=(-L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME")
      fi
      if grep -q '^; expect-ir: ' "$f"; then
        # IR gate: the fixture must COMPILE (to LLVM IR, `-S`) and the IR must
        # contain every `; expect-ir:` line. It exists for a fixture that needs
        # its own SCALY_HOME (a second package the root does not walk) and so
        # cannot link -- such a home has no runtime archive -- and for a defect
        # whose whole evidence is a LAYOUT (a struct type), not a printed value.
        # `; env:` and `; args:` work as in the xfail branch.
        env_args=()
        while IFS= read -r kv; do [ -n "$kv" ] && env_args+=("$kv"); done < <(sed -n 's/^; env: //p' "$f")
        arg_args=()
        while IFS= read -r a; do [ -n "$a" ] && arg_args+=($a); done < <(sed -n 's/^; args: //p' "$f")
        err=$(env "${env_args[@]}" "$STAGE" -S "${arg_args[@]}" -o "$bin.ll" "$f" 2>&1); crc=$?
        missing=""
        if [ $crc -eq 0 ]; then
          while IFS= read -r w; do
            [ -n "$w" ] && ! grep -qF -- "$w" "$bin.ll" && missing="$w"
          done < <(sed -n 's/^; expect-ir: //p' "$f")
        fi
        if [ $crc -eq 0 ] && [ -z "$missing" ]; then
          echo "PASS $t"
        else
          report_fail "$t: rc=$crc missing='$missing' '$err'"
        fi
        rm -f "$bin.ll"
      elif grep -q '^; expect-rc: ' "$f"; then
        # RUNTIME-TRAP gate: the fixture must COMPILE cleanly and then abort
        # with the given exit code. It exists because a hard trap cannot live
        # in the PASS branch above — the program never reaches a print — so
        # without this branch nothing in the tree ever fires a trap, and a
        # bounds check that stopped being emitted would leave every suite
        # green (TRAPS.md 1.5). Keyed on the MARKER and not on a name prefix,
        # so a fixture cannot fall into the wrong branch by being renamed.
        #
        # The compile must SUCCEED: a broken compiler exits nonzero too, and
        # scoring that as a pass is exactly the failure this gate is for.
        # `; expect-out:` (repeatable) pins the trap's own message, so an
        # unrelated abort with the same code cannot satisfy the test.
        want_rc=$(sed -n 's/^; expect-rc: //p' "$f" | head -1)
        crc=0
        "$SCALY" build "$f" -o "$bin" "${extra[@]}" >/dev/null 2>&1 || crc=$?
        if [ $crc -ne 0 ]; then
          report_fail "$t: compile failed rc=$crc"
        else
          out=$("$bin" 2>&1); rc=$?
          missing=""
          while IFS= read -r w; do
            [ -n "$w" ] && ! printf '%s' "$out" | grep -qF "$w" && missing="$w"
          done < <(sed -n 's/^; expect-out: //p' "$f")
          if [ "$rc" = "$want_rc" ] && [ -z "$missing" ]; then
            echo "PASS $t"
          else
            report_fail "$t: rc=$rc want=$want_rc missing='$missing' out='$out'"
          fi
        fi
      else
        # ★The build's own rc and the program's STDERR are part of the
        # verdict, not noise. Until 2026-09-07 this branch threw both away —
        # `failures+=("$t: '$out'")` — so a failure that printed nothing to
        # stdout was reported as `name: ''` and said NOTHING about which of
        # three very different things happened: the compile failed (the binary
        # was rm'd above, so `$bin` is missing and `$out` is empty), the
        # program crashed, or a runtime trap fired. All three of those write
        # to stderr — exit 19's page diagnostic, scaly_release_root_page's
        # LIFO abort, every emitter trap — and closure_escape failed exactly
        # once that way, unreproducibly in 500+ runs, with no evidence left.
        # The comparison itself stays STDOUT-ONLY: a test that writes to
        # stderr and still prints PASS must keep passing.
        cerr=$(mktemp); rerr=$(mktemp)
        # `; args: <flags>` (repeatable) passes compiler flags here too: one
        # fixture body can then be run through both lowerings of an operation
        # (simd_lookup_join / _portable, --portable-simd).
        arg_args=()
        while IFS= read -r a; do [ -n "$a" ] && arg_args+=($a); done < <(sed -n 's/^; args: //p' "$f")
        # `; run-env: VAR=VALUE` (repeatable) sets environment for the PROGRAM
        # (`; env:` is the compile's): a runtime switch read from it, such as
        # SCALY_POISON (poison_from_env).
        run_env=()
        while IFS= read -r kv; do [ -n "$kv" ] && run_env+=("$kv"); done < <(sed -n 's/^; run-env: //p' "$f")
        crc=0
        "$SCALY" build "$f" "${arg_args[@]}" -o "$bin" "${extra[@]}" > "$cerr" 2>&1 || crc=$?
        out=$(env "${run_env[@]+"${run_env[@]}"}" "$bin" 2>"$rerr"); rc=$?
        if [ "$out" = "PASS" ]; then
          echo "PASS $t"
        else
          detail="rc=$rc"
          [ $crc -ne 0 ] && detail="$detail compile-rc=$crc compile='$(tail -2 "$cerr" | tr '\n' ' ')'"
          [ -s "$rerr" ] && detail="$detail stderr='$(head -c 300 "$rerr" | tr '\n' ' ')'"
          report_fail "$t: '$out' $detail"
        fi
        rm -f "$cerr" "$rerr"
      fi
      ;;
  esac
}
export -f run_one report_fail
export STAGE SCALY SCALY_EXE
JOBS=${REGRESS_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}
res=$(mktemp)
ls tests/regress/*.scaly | xargs -P "$JOBS" -I{} bash -c 'run_one "$1"' _ {} > "$res"
pass=$(grep -c '^PASS ' "$res"); fail=$(grep -c '^FAIL ' "$res")
failures=$(grep '^FAIL ' "$res" | sed 's/^FAIL //' | tr '\n' ' ')
rm -f "$res"
echo "regress: $pass PASS, $fail FAIL $failures"
[ $fail -eq 0 ]

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
scaly_need_archive regress "$STAGE" || exit 1
pass=0; fail=0; failures=()
for f in tests/regress/*.scaly; do
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
      err=$(env "${env_args[@]}" "$STAGE" "${arg_args[@]}" -o "$bin" "$f" 2>&1); rc=$?
      if [ $rc -ne 0 ] && printf '%s' "$err" | grep -qF "$want"; then
        pass=$((pass+1))
      else
        fail=$((fail+1)); failures+=("$t: rc=$rc '$err'")
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
      if grep -q '^; expect-rc: ' "$f"; then
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
        "$STAGE" -o "$bin" "$f" "${extra[@]}" >/dev/null 2>&1 || crc=$?
        if [ $crc -ne 0 ]; then
          fail=$((fail+1)); failures+=("$t: compile failed rc=$crc")
        else
          out=$("$bin" 2>&1); rc=$?
          missing=""
          while IFS= read -r w; do
            [ -n "$w" ] && ! printf '%s' "$out" | grep -qF "$w" && missing="$w"
          done < <(sed -n 's/^; expect-out: //p' "$f")
          if [ "$rc" = "$want_rc" ] && [ -z "$missing" ]; then
            pass=$((pass+1))
          else
            fail=$((fail+1)); failures+=("$t: rc=$rc want=$want_rc missing='$missing' out='$out'")
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
        crc=0
        "$STAGE" -o "$bin" "$f" "${extra[@]}" > "$cerr" 2>&1 || crc=$?
        out=$("$bin" 2>"$rerr"); rc=$?
        if [ "$out" = "PASS" ]; then
          pass=$((pass+1))
        else
          fail=$((fail+1))
          detail="rc=$rc"
          [ $crc -ne 0 ] && detail="$detail compile-rc=$crc compile='$(tail -2 "$cerr" | tr '\n' ' ')'"
          [ -s "$rerr" ] && detail="$detail stderr='$(head -c 300 "$rerr" | tr '\n' ' ')'"
          failures+=("$t: '$out' $detail")
        fi
        rm -f "$cerr" "$rerr"
      fi
      ;;
  esac
done
echo "regress: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]

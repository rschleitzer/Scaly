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
STAGE=${1:-/tmp/scalyc_stage2}
pass=0; fail=0; failures=()
for f in tests/regress/*.scaly; do
  t=$(basename "$f" .scaly)
  bin=/tmp/rt_$t; rm -f "$bin"
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
      "$STAGE" -o "$bin" "$f" "${extra[@]}" >/dev/null 2>&1
      out=$("$bin" 2>/dev/null)
      if [ "$out" = "PASS" ]; then
        pass=$((pass+1))
      else
        fail=$((fail+1)); failures+=("$t: '$out'")
      fi
      ;;
  esac
done
echo "regress: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]

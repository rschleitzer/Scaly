#!/usr/bin/env bash
# tests/abi/run.sh — the extern-C ABI gate.
#
# Nothing in the compiler verifies that an `extern` declaration matches the C
# function it stands for. A mismatch shows up as wrong values at runtime and
# never as a diagnostic, so this suite is the only thing standing between the
# tree and that class. Three checks:
#
#   1. RESULT widths — HARD gate. A C function returning `int` leaves the upper
#      32 bits of the return register unspecified, so reading it as Scaly `int`
#      (64 bits) can flip the SIGN. Proven with a three-line shim:
#
#          int neg_int(void) { return -1; }
#
#      declared `returns int` answered 4294967295 (so `> 0`), declared
#      `returns i32` answered -1. Every `if rc < 0` error check on such a
#      function was a coin flip decided by the callee's codegen. 109 of these
#      were fixed on 2026-08-05; zero is the only acceptable number.
#
#   2. PARAMETER widths — pinned by COUNT, both directions. These are ABI-safe
#      the other way round (the caller writes the whole register and a 32-bit
#      callee reads exactly the low half it is entitled to), so they are not
#      bugs today. But "benign today" is not a property to rely on blindly, and
#      a NEW one must not slip in unnoticed — hence the pin rather than a pass.
#      Fixing one means narrowing the Scaly side too (fds and pids are `int`
#      throughout the tree), which is why they are recorded instead of chased.
#
#   3. CONSISTENCY — the same C symbol declared `extern` twice in this tree may
#      not disagree at the ABI level. `poll` did: `(ptr, i32, i32) -> i32` in
#      scaly/fiber.scaly against `(ptr, i64, i64) -> i64` in
#      scalyls/worker.scaly, both linked into one binary, so the LSP read a
#      64-bit result from a function returning 32. Comparison is on the LLVM
#      shape, so a handle struct and `pointer[void]` count as equal.
#
#   4. LLP64 — no bare `long` in an exported signature of our own C shims.
#      Checks 1-3 can only judge what is declared on THIS host; this one asks
#      whether a signature changes width on a target we do not compile here.
#      ONE seed serves every target, so a Scaly extern cannot be
#      target-conditional — it says `i64`/`size_t` once, for all of them. C's
#      `long` is 64-bit on LP64 (mac/linux) and 32-bit on LLP64 (Win64), so a
#      `long` in a shim contradicts its own declaration on exactly one target,
#      silently, and the RESULT direction flips the sign exactly as in check 1.
#      Six of these were fixed in eio.c on 2026-08-09 (ctime.c was written
#      portable from the start); zero is the only acceptable number. Fix on the
#      C SIDE — `long long` for results, `size_t` for counts — which leaves the
#      Scaly declaration alone and needs no seed refresh.
#
# Usage: tests/abi/run.sh
#
# Checks 1 and 2 need C headers; without them they SKIP and say so rather than
# passing quietly. Checks 3 and 4 need nothing and always run.
cd "$(dirname "$0")/../.." || exit 1

# The parameter findings this tree is known to carry. Raise or lower ONLY
# together with a note in CLAUDE.md saying which declaration changed and why.
EXPECTED_PARAM_FINDINGS=9

fail=0

echo "abi: 3) consistency — doppelte extern-Deklarationen"
out=$(python3 tools/abi-consistency.py) || { echo "$out"; echo "abi: FAIL (consistency crashed)"; exit 1; }
echo "$out" | tail -2 | sed 's/^/    /'
if ! echo "$out" | grep -q "^Symbole mit ABI-widersprüchlichen Deklarationen: 0 "; then
  echo "$out"
  echo "abi: FAIL — widersprüchliche extern-Deklarationen"
  fail=1
fi

echo
echo "abi: 1+2) Breiten gegen die echten Header"
# shellcheck disable=SC1091
source tools/llvm-env.sh > /dev/null 2>&1
hdrs=()
for cand in "$LLVM18/include/llvm-c" "$llvm_env_prefix/include/llvm-c" \
            /opt/homebrew/opt/llvm@18/include/llvm-c /usr/lib/llvm-18/include/llvm-c; do
  [ -n "$cand" ] && [ -d "$cand" ] && { hdrs+=(--headers "$cand"); break; }
done
for cand in /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include /usr/include; do
  [ -d "$cand" ] && { hdrs+=(--headers "$cand"); break; }
done
if [ ${#hdrs[@]} -eq 0 ]; then
  echo "    SKIP (keine C-Header gefunden — Breitenprüfung übersprungen)"
else
  out=$(python3 tools/abi-audit.py --quiet "${hdrs[@]}" $(find packages -name '*.scaly'))
  rc=$?
  echo "$out" | head -4 | sed 's/^/    /'
  if [ "$rc" -ne 0 ]; then
    python3 tools/abi-audit.py --quiet "${hdrs[@]}" $(find packages -name '*.scaly') | head -40
    echo "abi: FAIL — RESULT-Breite weicht vom C-Prototyp ab (kann das Vorzeichen kippen)"
    fail=1
  fi
  got=$(echo "$out" | sed -n 's/^PARAM-BEFUNDE: //p')
  if [ "$got" != "$EXPECTED_PARAM_FINDINGS" ]; then
    echo "abi: FAIL — PARAM-BEFUNDE $got, erwartet $EXPECTED_PARAM_FINDINGS"
    echo "         (mehr = eine neue Abweichung; weniger = eine wurde behoben," \
         "dann EXPECTED_PARAM_FINDINGS hier nachziehen)"
    python3 tools/abi-audit.py --quiet "${hdrs[@]}" $(find packages -name '*.scaly') | head -40
    fail=1
  fi
fi

echo
echo "abi: 4) LLP64 — bare \`long\` in exportierten Shim-Signaturen"
out=$(python3 tools/llp64-audit.py --quiet) || fail=1
echo "$out" | sed 's/^/    /'
if ! echo "$out" | grep -q "^LLP64-BEFUNDE: 0$"; then
  echo "abi: FAIL — \`long\` ist auf Win64 32 Bit; auf der C-Seite beheben" \
       "(\`long long\` für Ergebnisse, \`size_t\` für Zählungen)"
  fail=1
fi

echo
if [ "$fail" = 0 ]; then
  echo "abi: PASS"
else
  echo "abi: FAIL"
fi
exit $fail
